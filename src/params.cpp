// Copyright 2015 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
//

const int Scoring::Params::RB_ADJUST[4] = {250, 75, -75, -250};
const int Scoring::Params::RBN_ADJUST[4] = {500, 675, 825, 1000};
const int Scoring::Params::QR_ADJUST[4] = {-500, 0, 500, 500};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2400, -1500};
const int Scoring::Params::PAWN_TRADE[3] = {-450, -250, -100};
const int Scoring::Params::CASTLING[6] = {0, -70, -100, 280, 200, -280};
const int Scoring::Params::KING_COVER[5] = {220, 310, 120, 30, 20};
const int Scoring::Params::KING_FILE_OPEN = -150;
const int Scoring::Params::KING_DISTANCE_BASIS = 320;
const int Scoring::Params::KING_DISTANCE_MULT = 80;
const int Scoring::Params::PIN_MULTIPLIER_MID = 200;
const int Scoring::Params::PIN_MULTIPLIER_END = 300;
const int Scoring::Params::KING_ATTACK_PARAM1 = 500;
const int Scoring::Params::KING_ATTACK_PARAM2 = 320;
const int Scoring::Params::KING_ATTACK_PARAM3 = 1500;
const int Scoring::Params::KING_ATTACK_BOOST_THRESHOLD = 480;
const int Scoring::Params::KING_ATTACK_BOOST_DIVISOR = 500;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 420;
const int Scoring::Params::BISHOP_PAIR_END = 550;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -160;
const int Scoring::Params::BAD_BISHOP_MID = -40;
const int Scoring::Params::BAD_BISHOP_END = -60;
const int Scoring::Params::OUTPOST_NOT_DEFENDED = 42;
const int Scoring::Params::CENTER_PAWN_BLOCK = -120;
const int Scoring::Params::OUTSIDE_PASSER_MID = 120;
const int Scoring::Params::OUTSIDE_PASSER_END = 250;
const int Scoring::Params::WEAK_PAWN_MID = -80;
const int Scoring::Params::WEAK_PAWN_END = -80;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -100;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -100;
const int Scoring::Params::SPACE = 20;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 30;
const int Scoring::Params::ROOK_ON_7TH_MID = 260;
const int Scoring::Params::ROOK_ON_7TH_END = 260;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 570;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 660;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 200;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 200;
const int Scoring::Params::ROOK_ATTACKS_WEAK_PAWN_MID = 100;
const int Scoring::Params::ROOK_ATTACKS_WEAK_PAWN_END = 100;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 50;
const int Scoring::Params::ROOK_BEHIND_PP_END = 100;
const int Scoring::Params::QUEEN_OUT = -60;
const int Scoring::Params::PASSER_OWN_PIECE_BLOCK_MID = -20;
const int Scoring::Params::PASSER_OWN_PIECE_BLOCK_END = -50;

const int Scoring::Params::PP_BLOCK_BASE_MID = 140;
const int Scoring::Params::PP_BLOCK_BASE_END = 140;
const int Scoring::Params::PP_BLOCK_MULT_MID = 72;
const int Scoring::Params::PP_BLOCK_MULT_END = 32;
const int Scoring::Params::ENDGAME_PAWN_BONUS = 120;
const int Scoring::Params::KING_NEAR_PASSER = 200;
const int Scoring::Params::OPP_KING_NEAR_PASSER = -280;
const int Scoring::Params::PAWN_SIDE_BONUS = 280;
const int Scoring::Params::SUPPORTED_PASSER6 = 380;
const int Scoring::Params::SUPPORTED_PASSER7 = 760;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -100;

const int Scoring::Params::KING_ATTACK_SCALE[512] =
{
   0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
   16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
   32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
   48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
   64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
   80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
   96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
   112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
   128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
   144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
   160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
   176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
   192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
   208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
   224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
   240,241,242,243,244,245,246,247,248,249,250,251,251,252,253,254,
   254,255,256,257,257,258,259,260,260,261,262,263,263,264,265,266,
   266,267,268,269,269,270,271,272,272,273,274,275,275,276,277,278,
   278,279,280,281,281,282,283,284,284,285,286,287,287,288,289,289,
   290,290,291,291,292,292,293,293,294,294,295,295,296,296,297,297,
   298,298,299,299,300,300,301,301,302,302,303,303,304,304,305,305,
   306,306,307,307,308,308,309,309,310,310,311,311,312,312,313,313,
   313,314,314,314,314,315,315,315,315,316,316,316,316,317,317,317,
   317,318,318,318,318,319,319,319,319,320,320,320,320,321,321,321,
   321,322,322,322,322,323,323,323,323,324,324,324,324,325,325,325,
   325,326,326,326,326,327,327,327,327,328,328,328,328,329,329,329,
   329,330,330,330,330,331,331,331,331,332,332,332,332,333,333,333,
   333,334,334,334,334,335,335,335,335,336,336,336,336,337,337,337,
   337,338,338,338,338,339,339,339,339,340,340,340,340,341,341,341,
   341,342,342,342,342,343,343,343,343,344,344,344,344,345,345,345,
   345,346,346,346,346,347,347,347,347,348,348,348,348,349,349,349,
   349,350,350,350,350,351,351,351,351,352,352,352,352,353,353,353
};

   
const int Scoring::Params::TRADE_DOWN[16] = {682, 640, 409, 597, 516, 469, 426, 384, 341, 298, 256, 213, 171, 128, 85, 42};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 60, 110, 180, 270, 560, 1110}, {0, 0, 90, 160, 280, 420, 840, 1410}};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 20, 40, 60, 90, 200, 0}, {0, 0, 30, 60, 100, 150, 300, 0}};
const int Scoring::Params::CONNECTED_PASSERS[2][8] = {{0, 0, 0, 100, 190, 240, 480, 830}, {0, 0, 0, 100, 190, 240, 480, 830}};
const int Scoring::Params::ADJACENT_PASSERS[2][8] = {{0, 0, 0, 80, 150, 170, 340, 700},{0, 0, 0, 80, 150, 170, 340, 700}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-60, -80, -100, -100, -100, -100, -80, -60}, {-100, -130, -170, -170, -170, -170, -130, -100}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{-80, -80, -80, -80, -80, -80, -80, -80}, {-120, -120, -120, -120, -120, -120, -120, -120}};


