// Copyright 1998, 2005, 2008, 2017-2018, 2020-2021, 2024 by Jon Dart. All Rights Reserved.

#include "legal.h"
#include "movegen.h"
#include <cstddef>

static inline int validPromotion(PieceType p) { return int(p) < 6 && int(p) > 1; }

bool validMove(const Board &board, Move move) {
    static constexpr int incr[2] = {-8, 8};
    PieceType pieceMoved = PieceMoved(move);
    const Square start = StartSquare(move);
    const Square dest = DestSquare(move);
    if (!OnBoard(start) || !OnBoard(dest) || pieceMoved == Empty || !validPiece(board[start]) ||
        !validPiece(board[dest]) || (TypeOfPiece(board[start]) != pieceMoved) ||
        (board[dest] != EmptyPiece && (PieceColor(board[dest]) == board.sideToMove())) ||
        (PieceColor(board[start]) != board.sideToMove())) {
        return false;
    } else if (TypeOfMove(move) == EnPassant) {
        Square dest2 = dest + incr[board.sideToMove()];
        if (board.enPassantSq() == InvalidSquare || dest2 != board.enPassantSq() ||
            board[dest2] != MakePiece(Pawn, board.oppositeSide()))
            return false;
    } else if (Capture(move) != TypeOfPiece(board[dest]) ||
               board[dest] != MakePiece(Capture(move), board.oppositeSide())) {
        return false;
    }
    if (TypeOfMove(move) == Promotion &&
        (board[start] != MakePiece(Pawn, board.sideToMove()) || !validPromotion(PromoteTo(move)))) {
        return false;
    }
    switch (pieceMoved) {
    case Knight:
        return Attacks::knight_attacks[start].isSet(dest);
    case King: {
        const Square kp = board.kingSquare(board.sideToMove());
        if (kp != start) return false;
        if (IsCastling(move)) {
            if (board.sideToMove() == White) {
                if (kp != chess::E1) return false;
            } else {
                if (kp != chess::E8) return false;
            }
        }
        if (TypeOfMove(move) == KCastle) {
            CastleType CS = board.castleStatus(board.sideToMove());
            return board[kp] == MakePiece(King, board.sideToMove()) &&
                   ((CS == CanCastleEitherSide) || (CS == CanCastleKSide)) &&
                   board[kp + 1] == EmptyPiece && board[kp + 2] == EmptyPiece &&
                   board.checkStatus() != InCheck &&
                   !board.anyAttacks(kp + 1, board.oppositeSide()) &&
                   !board.anyAttacks(kp + 2, board.oppositeSide());
        } else if (TypeOfMove(move) == QCastle) {
            CastleType CS = board.castleStatus(board.sideToMove());
            return board[kp] == MakePiece(King, board.sideToMove()) &&
                   ((CS == CanCastleEitherSide) || (CS == CanCastleQSide)) &&
                   board[kp - 1] == EmptyPiece && board[kp - 2] == EmptyPiece &&
                   board[kp - 3] == EmptyPiece && board.checkStatus() != InCheck &&
                   !board.anyAttacks(kp - 1, board.oppositeSide()) &&
                   !board.anyAttacks(kp - 2, board.oppositeSide());
        } else {
            return Attacks::king_attacks[start].isSet(dest);
        }
    }
    case Rook: {
        int d = std::abs(Attacks::directions[start][dest]);
        return (d == 1 || d == 8) && board.clear(start, dest);
    }
    case Bishop: {
        int d = std::abs(Attacks::directions[start][dest]);
        return (d == 7 || d == 9) && board.clear(start, dest);
    }
    case Queen: {
        int d = std::abs(Attacks::directions[start][dest]);
        return (d == 1 || d == 8 || d == 7 || d == 9) && board.clear(start, dest);
    }
    case Pawn: {
        if (board.sideToMove() == White) {
            if (Capture(move) != Empty)
                return (dest - start) == 7 || (dest - start) == 9;
            if (board[start + 8] != EmptyPiece)
                return false;
            if (start + 8 == dest)
                return true;
            else
                return (Rank(start, White) == 2 && start + 16 == dest && board[dest] == EmptyPiece);
        } else {
            if (Capture(move) != Empty)
                return (dest - start) == -7 || (dest - start) == -9;
            if (board[start - 8] != EmptyPiece)
                return false;
            if (start - 8 == dest)
                return true;
            else
                return (Rank(start, Black) == 2 && start - 16 == dest && board[dest] == EmptyPiece);
        }
    }
    default:
        break;
    }
    return false;
}

bool legalMove(const Board &board, Move move) {
    if (!validMove(board, move)) {
        return false;
    }

    const Square start = StartSquare(move);
    const Square dest = DestSquare(move);

    PieceType promoteTo = Empty;
    if (TypeOfPiece(board[start]) == Pawn && Rank(dest, board.sideToMove()) == 8)
        promoteTo = Queen;
    Move emove = CreateMove(board, start, dest, promoteTo);
    Move moves[Constants::MaxMoves];
    MoveGenerator mg(board);
    int found = 0;
    int n = mg.generateAllMoves(moves, 0);

    for (int i = 0; i < n; i++) {
        if (MovesEqual(moves[i], emove)) {
            found++;
            break;
        }
    }
    if (!found) {
        return false;
    } else {
        // check for king en prise
        Board board_copy(board);
        const ColorType side = board.sideToMove();
        board_copy.doMove(emove);
        return !board_copy.anyAttacks(board_copy.kingSquare(side), OppositeColor(side));
    }
}
