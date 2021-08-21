 // Copyright 1996-2012, 2014, 2016-2019, 2021 by Jon Dart.  All Rights Reserved.

#include "globals.h"
#include "hash.h"
#include "scoring.h"
#include "search.h"
#ifdef SYZYGY_TBS
#include "syzygy.h"
#endif
#include "bitbase.cpp"

#ifdef SYZYGY_TBS
static bool tb_init = false;

bool tb_init_done()
{
   return tb_init;
}
int EGTBMenCount = 0;
#endif

MoveArray *gameMoves;
Options options;
BookReader openingBook;
Log *theLog = nullptr;
string learnFileName;
LockDefine(input_lock);
#ifdef SYZYGY_TBS
LockDefine(syzygy_lock);
#endif
bool polling_terminated;
ThreadControl inputSem;
#ifdef TUNE
Tune tune_params;
#endif
#ifdef NNUE
nnue::Network network;
bool nnueInitDone = false;
#endif

static const char * LEARN_FILE_NAME = "arasan.lrn";

static const char * DEFAULT_BOOK_NAME = "book.bin";

static const char * RC_FILE_NAME = "arasan.rc";

const size_t LINUX_STACK_SIZE = 8*1024*1024;

#ifdef NNUE
// TBD: add version/hash?
const char * DEFAULT_NETWORK_NAME = "arasan.nnue";
#endif

string programPath;

string derivePath(const char *fileName) {
   return derivePath(programPath.c_str(),fileName);
}

string derivePath(const char *base, const char *fileName) {
    string result(base);
    size_t pos;
#ifdef _WIN32
    pos = result.rfind('\\',string::npos);
#else
    pos = result.rfind('/',string::npos);
#endif
    if (pos == string::npos) {
      return fileName;
    }
    else {
      return result.substr(0,pos+1) + fileName;
    }
}

int initGlobals(const char *pathName, bool initLog) {
   programPath = pathName;
   gameMoves = new MoveArray();
   polling_terminated = false;
   if (initLog) {
       theLog = new Log();
       theLog->clear();
       theLog->write_header();
   }
   LockInit(input_lock);
#ifdef SYZYGY_TBS
   LockInit(syzygy_lock);
#endif
#ifdef NNUE
   if (options.search.nnueFile == "") {
       // set default network path
       options.search.nnueFile = derivePath(DEFAULT_NETWORK_NAME);
   }
#endif   
return 1;
}

#ifdef NNUE
int loadNetwork(const std::string &fname) {
   std::ifstream in(fname,ios_base::in | ios_base::binary);
   in >> network;
   if (!in.good()) {
       return 0;
   }
   return 1;
}
#endif

void CDECL cleanupGlobals(void) {
   openingBook.close();
   delete gameMoves;
   delete theLog;
   LockFree(input_lock);
#ifdef SYZYGY_TBS
   LockFree(syzygy_lock);
#endif   
   Scoring::cleanup();
   Bitboard::cleanup();
   Board::cleanup();
}

void initOptions(const char *pathName) {
    programPath = pathName;
    string rcPath = derivePath(RC_FILE_NAME);
    // try to read arasan.rc file
    options.init(rcPath);
#ifndef _WIN32
    // Also read .rc from the user's HOME,
    // if there is one.
    if (getenv("HOME")) {
       rcPath = derivePath(getenv("HOME"),RC_FILE_NAME);
       options.init(rcPath);
    }
#endif
    learnFileName = derivePath(LEARN_FILE_NAME);
}

void delayedInit() {
#ifdef SYZYGY_TBS
   if (options.search.use_tablebases && !tb_init_done()) {
       EGTBMenCount = 0;
       string path;
       if (options.search.syzygy_path == "") {
          options.search.syzygy_path=derivePath("syzygy");
       }
       path = options.search.syzygy_path;
       EGTBMenCount = SyzygyTb::initTB(options.search.syzygy_path);
       tb_init = true;
       if (EGTBMenCount) {
          stringstream msg;
          msg << "found " << EGTBMenCount << "-man Syzygy tablebases in directory " << path << endl;
          cerr << msg.str();
       }
    }
#endif
#ifdef NNUE
    if (options.search.useNNUE && !nnueInitDone) {
        nnueInitDone = loadNetwork(derivePath(options.search.nnueFile.c_str())) != 0;
        if (nnueInitDone) {
            std::cerr << "loaded network from file ";
        } else {
            std::cerr << "warning: failed to load network file ";
        }
        std::cerr << options.search.nnueFile << std::endl;
    }
#endif   
    // also initialize the book here
    if (options.book.book_enabled && !openingBook.is_open()) {
        openingBook.open(derivePath(DEFAULT_BOOK_NAME).c_str());
    }
}

   void unloadTb() {
#ifdef SYZYGY_TBS
   if (tb_init_done()) {
      // Note: Syzygy code will free any existing memory if
      // initialized more than once. So no need to do anything here.
      tb_init = false;
   }
#endif
}

