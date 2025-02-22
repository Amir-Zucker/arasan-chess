// Copyright 2006-2008, 2011, 2017-2021, 2023-2024 by Jon Dart. All Rights Reserved.

#include "searchc.h"
#include "search.h"
#include "tunable.h"

#include <cassert>

TUNABLE(MAX_HISTORY_DEPTH,17,8,20);
TUNABLE(HISTORY_BASE,-10,-150,150);
TUNABLE(HISTORY_SLOPE,25,0,600);
TUNABLE(HISTORY_SLOPE2,5,0,15);
TUNABLE(MAX_CAPTURE_HISTORY_DEPTH,10,4,20);
TUNABLE(CAPTURE_HISTORY_BASE,-129,-150,150);
TUNABLE(CAPTURE_HISTORY_SLOPE,440,0,600);
TUNABLE(CAPTURE_HISTORY_SLOPE2,4,0,15);
TUNABLE(CAPTURE_HISTORY_ORDERING_DIVISOR,111,1,128);

// not tunable
static constexpr int HISTORY_DIVISOR = 2048;
static constexpr int CAPTURE_HISTORY_DIVISOR = 2048;

static int bonus(int depth) {
    const int d = std::max<int>(1, depth / DEPTH_INCREMENT);
    if (d > MAX_HISTORY_DEPTH)
        return 0;
    else
        return HISTORY_BASE + HISTORY_SLOPE*d + HISTORY_SLOPE2*d*d;
}

static int captureBonus(int depth) {
    const int d = std::max<int>(1, depth / DEPTH_INCREMENT);
    if (d > MAX_CAPTURE_HISTORY_DEPTH)
        return 0;
    else
        return CAPTURE_HISTORY_BASE + CAPTURE_HISTORY_SLOPE*d + CAPTURE_HISTORY_SLOPE2*d*d;
}

SearchContext::SearchContext() {
    history = new ButterflyArray<int>();
    counterMoves = new PieceToArray<Move>();
    counterMoveHistory = new PieceTypeToMatrix<int>();
    fuMoveHistory = new PieceTypeToMatrix<int>();
    captureHistory = new CaptureHistoryArray();
    clear();
}

SearchContext::~SearchContext() {
    delete history;
    delete counterMoves;
    delete counterMoveHistory;
    delete fuMoveHistory;
    delete captureHistory;
}

void SearchContext::clear() {
    clearKiller();
    for (int side = 0; side < 2; side++)
        for (int i = 0; i < 64; i++) {
            (*history)[side][i].fill(0);
        }
    for (int p = 0; p < 16; p++)
        for (int sq = 0; sq < 64; sq++) {
            (*captureHistory)[p][sq].fill(0);
        }
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 64; j++) {
            (*counterMoves)[i][j] = NullMove;
        }
    }
    // clear counter move history
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 64; j++)
            for (int k = 0; k < 8; k++) {
                (*counterMoveHistory)[i][j][k].fill(0);
                (*fuMoveHistory)[i][j][k].fill(0);
            }
}

void SearchContext::clearKiller() {
    for (int i = 0; i < Constants::MaxPly + 2; i++) {
        killers1[i] = killers2[i] = NullMove;
    }
}

int SearchContext::historyScore(Move m, const NodeInfo *node,
                                ColorType side) const noexcept {
    int score = (*history)[side][StartSquare(m)][DestSquare(m)];
    if (node->ply > 0 && !IsNull((node - 1)->last_move)) {
        Move prevMove = (node - 1)->last_move;
        score +=
            (*counterMoveHistory)[PieceMoved(prevMove)][DestSquare(prevMove)]
                                 [PieceMoved(m)][DestSquare(m)];
    }
    if (node->ply > 1 && !IsNull((node - 2)->last_move)) {
        Move prevMove = (node - 2)->last_move;
        score += (*fuMoveHistory)[PieceMoved(prevMove)][DestSquare(prevMove)]
                                 [PieceMoved(m)][DestSquare(m)];
    }
    return score;
}

void SearchContext::update(int &val, int bonus, int divisor, bool is_best) {
    val -= val * bonus / divisor;
    if (is_best)
        val += bonus;
    else
        val -= bonus;
}

void SearchContext::updateStats(const Board &board, const NodeInfo *node) {
    // sanity checks
    assert(!IsNull(node->best));
    assert(OnBoard(StartSquare(node->best)) && OnBoard(DestSquare(node->best)));
    assert(node->num_quiets < Constants::MaxMoves);
    if (CaptureOrPromotion(node->best)) {
        assert(node->num_captures<Constants::MaxCaptures);
        for (int i = 0; i < node->num_captures; i++) {
            const Move m = node->captures[i];
            updateNonQuietMove(board, node, m, MovesEqual(m, node->best));
        }
    } else {
        // Do not update on fail high of 1st quiet and low depth (idea from
        // Ethereal).
        if (node->num_quiets == 1 && node->depth <= 3 * DEPTH_INCREMENT)
            return;
        for (int i = 0; i < node->num_quiets; i++) {
            const Move m = node->quiets[i];
            updateQuietMove(board, node, m, MovesEqual(m,node->best), false);
        }
    }
}

void SearchContext::updateQuietMove(const Board &board, const NodeInfo *node, Move m,
                                    bool positive, bool continuationOnly) {
    const int b = bonus(node->depth);
    if (!continuationOnly) {
        update((*history)[board.sideToMove()][StartSquare(m)][DestSquare(m)], b,
           HISTORY_DIVISOR, positive);
        if (positive && PieceMoved(m) != Pawn) {
            update((*history)[board.sideToMove()][DestSquare(m)][StartSquare(m)], b,
                   HISTORY_DIVISOR, false);
        }
    }
    if (node->ply > 0) {
        Move lastMove = (node - 1)->last_move;
        if (!IsNull(lastMove)) {
            update((*counterMoveHistory)[PieceMoved(lastMove)][DestSquare(
                       lastMove)][PieceMoved(m)][DestSquare(m)],
                   b, HISTORY_DIVISOR, positive);
        }
        if (node->ply > 1) {
            lastMove = (node - 2)->last_move;
            if (!IsNull(lastMove)) {
                update((*fuMoveHistory)[PieceMoved(lastMove)][DestSquare(
                           lastMove)][PieceMoved(m)][DestSquare(m)],
                       b, HISTORY_DIVISOR, positive);
            }
        }
    }
}

void SearchContext::updateNonQuietMove(const Board &board, const NodeInfo *node, Move m, bool positive) {
    update((*captureHistory)[board[StartSquare(m)]][DestSquare(m)][Capture(m)],
            captureBonus(node->depth),
            CAPTURE_HISTORY_DIVISOR, positive);
}

int SearchContext::getCmHistory(NodeInfo *node, Move move) const noexcept {
    if (node->ply == 0 || IsNull((node - 1)->last_move)) {
        return 0;
    }
    Move prev((node - 1)->last_move);
    return (*counterMoveHistory)[PieceMoved(prev)][DestSquare(prev)]
                                [PieceMoved(move)][DestSquare(move)];
}

int SearchContext::getFuHistory(NodeInfo *node, Move move) const noexcept {
    if (node->ply < 2 || IsNull((node - 2)->last_move)) {
        return 0;
    }
    Move prev((node - 2)->last_move);
    return (*fuMoveHistory)[PieceMoved(prev)][DestSquare(prev)]
                           [PieceMoved(move)][DestSquare(move)];
}

int SearchContext::captureHistoryScoreForOrdering(const Board &b, Move m) const noexcept {
    return captureHistoryScore(b, m) / CAPTURE_HISTORY_ORDERING_DIVISOR;
}
