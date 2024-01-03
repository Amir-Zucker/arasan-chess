// Copyright 1996, 2013, 2014, 2017, 2021-2024 by Jon Dart. All Rights Reserved.

// Utility to add ECO codes to PGN game files.

#include "board.h"
#include "chessio.h"
#include "eco.h"
#include "globals.h"
#include "legal.h"
#include "movearr.h"
#include "movegen.h"
#include "notation.h"
#include "options.h"
#include "scoring.h"

extern "C" {
#include <ctype.h>
#include <stddef.h>
#include <string.h>
};
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <vector>

static struct ECOptions {
    ECOptions() : force(false), names(false) {}

    bool force, names;
} opts;

static void show_usage() {
    std::cerr << "Usage: ecocoder [-f (force)] [-n (opening names)] pgn_file" << std::endl;
}

int CDECL main(int argc, char **argv) {
    Bitboard::init();
    Board::init();
    Attacks::init();
    Scoring::init();
    if (!globals::initGlobals(false)) {
        globals::cleanupGlobals();
        exit(-1);
    }
    atexit(globals::cleanupGlobals);

    Board board;
    ECO eco;
    int arg = 1;

    for (; arg < argc && *(argv[arg]) == '-'; ++arg) {
        if (strcmp(argv[arg], "-f") == 0) {
            opts.force = true;
        } else if (strcmp(argv[arg], "-n") == 0) {
            opts.names = true;
        } else {
            show_usage();
            exit(-1);
        }
    }
    if (arg >= argc) {
        show_usage();
        exit(-1);
    }
    std::ifstream pgn_file(argv[arg], std::ios::in | std::ios::binary);
    int c;
    ColorType side;
    std::string result, white, black;
    if (!pgn_file.good()) {
        std::cerr << "could not open file " << argv[arg] << std::endl;
        exit(-1);
    } else {
        std::vector<ChessIO::Header> hdrs;
        while (!pgn_file.eof()) {
            long first;
            MoveArray moves;
            board.reset();
            side = White;
            while (pgn_file.good() && (c = pgn_file.get()) != EOF) {
                if (c == '[') {
                    int c1 = pgn_file.get();
                    if (c1 == 'E') {
                        int c2 = pgn_file.get();
                        if (c2 == 'v') {
                            pgn_file.putback(c2);
                            pgn_file.putback(c1);
                            pgn_file.putback(c);
                            break;
                        }
                    }
                }
            }
            hdrs.clear();
            ChessIO::collect_headers(pgn_file, hdrs, first);
            if (!hdrs.size())
                continue;
            bool ok = true;
            bool done = false;
            bool exit = false;
            while (ok && !exit) {
                ChessIO::Token tok = ChessIO::get_next_token(pgn_file);
                std::string num;
                switch (tok.type) {
                case ChessIO::Eof: {
                    exit = true;
                    break;
                }
                case ChessIO::Number: {
                    num = tok.val;
                    break;
                }
                case ChessIO::GameMove: {
                    if (done) {
                        // we should not have moves after the result
                        // if it looks like a tag, push it back
                        if (tok.val.size() && tok.val[0] == '[') {
                            for (int i = (int)tok.val.size() - 1; i >= 0; --i)
                                pgn_file.putback(tok.val[0]);
                        }
                        exit = true;
                        break;
                    }
                    // parse the move
                    Move m = Notation::value(board, side, Notation::InputFormat::SAN, tok.val);
                    if (IsNull(m) || !legalMove(board, StartSquare(m), DestSquare(m))) {
                        // echo to both stdout and stderr
                        std::cerr << "Illegal move: " << tok.val << std::endl;
                        std::cout << "Illegal move: " << tok.val << std::endl;
                        ok = false;
                    } else {
                        BoardState bs = board.state;
                        std::string img;
                        // convert to SAN
                        Notation::image(board, m, Notation::OutputFormat::SAN, img);
                        moves.add_move(board, bs, m, img, false);
                        board.doMove(m);
                    }
                    side = OppositeColor(side);
                    break;
                }
                case ChessIO::Unknown: {
                    if (done) {
                        // ignore unknown text after result
                        // if it looks like a tag, push it back
                        if (tok.val.size() && tok.val[0] == '[') {
                            for (int i = (int)tok.val.size() - 1; i >= 0; --i)
                                pgn_file.putback(tok.val[0]);
                        }
                        exit = true;
                        break;
                    }
                    std::cerr << "Unrecognized text: " << tok.val << std::endl;
                    break;
                }
                case ChessIO::Comment: {
                    // ignored for now
                    break;
                }
                case ChessIO::Result: {
                    result = tok.val;
                    done = true;
                    break;
                }
                case ChessIO::OpenVar:
                    std::cerr << "Warning: variations not supported" << std::endl;
                    done = true;
                default:
                    break;

                } // end switch
            }
            // output headers
            std::string ecoC, name;
            eco.classify(moves, ecoC, name);
            std::string currentECO;
            bool found = ChessIO::get_header(hdrs, "ECO", currentECO);
            if (!found || (opts.force && ecoC != "")) {
                if (opts.force) {
                    if (name == "") {
                        ChessIO::remove_header(hdrs, "ECO");
                    } else if (found) {
                        ChessIO::replace_header(hdrs, "ECO", ecoC);
                    } else {
                        hdrs.push_back(ChessIO::Header("ECO", ecoC));
                    }
                } else if (!found && name != "") {
                    hdrs.push_back(ChessIO::Header("ECO", ecoC));
                }
            }
            if (opts.names) {
                std::string currentOpening;
                found = ChessIO::get_header(hdrs, "Opening", currentOpening);
                if (opts.force) {
                    if (name == "") {
                        ChessIO::remove_header(hdrs, "Opening");
                    } else if (found) {
                        ChessIO::replace_header(hdrs, "Opening", name);
                    } else {
                        hdrs.push_back(ChessIO::Header("Opening", name));
                    }
                } else if (!found && name != "") {
                    hdrs.push_back(ChessIO::Header("Opening", name));
                }
            }
            if (moves.num_moves() > 0)
                ChessIO::store_pgn(std::cout, moves, result, hdrs);
        }
        pgn_file.close();
    }
    return 0;
}
