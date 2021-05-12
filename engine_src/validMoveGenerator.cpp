//
// Created by danie on 5/3/2021.
//

#include <vector>
#include <iostream>
#include "program.h"

namespace fillMoves {

    using namespace std;
    using namespace boardEdit;

// FOR DIAGONALS
    inline bool movedAwayFromDiagonalToKing(coord &enemyKing, coord &start, coord &end, coord &blockingPiece) {
        return blockingPiece == start &&
               (abs(enemyKing.row - end.row) != abs(enemyKing.col - end.col) || // end coord is not on diagonal to king
                sigNum(end.row - enemyKing.row) != sigNum(blockingPiece.row - enemyKing.row) ||
                sigNum(end.col - enemyKing.col) != sigNum(blockingPiece.col -
                                                          enemyKing.col));// direction from enemy king to blocking piece isn't the same as from enemy king to end coord
    }

// assume the king is already in the diagonal of the current piece
    inline bool positionInDiagonalToKing(coord &enemyKing, coord &position, coord &curPiece) {
        return abs(position.row - curPiece.row) == abs(position.col - curPiece.col) && // position is on the diagonal
               sigNum(enemyKing.row - curPiece.row) == sigNum(position.row - curPiece.row) &&
               sigNum(enemyKing.col - curPiece.col) == sigNum(position.col - curPiece.col) && // on the same diagonal
               abs(enemyKing.row - curPiece.row) >
               abs(position.row - curPiece.row); // king is further away(position is in between the two)
    }

// If blockingPiece is a pawn, startCoord is also a pawn and blockingPiece is eaten by en passant
    inline bool isDisappearingEnPassant(class Board &curBoard, coord &blockingPiece, coord &startCoord, coord &end) {
        return pieceAt(curBoard, blockingPiece)->id == PAWN &&
               pieceAt(curBoard, startCoord)->id == PAWN &&
               startCoord.row == blockingPiece.row && end.row == blockingPiece.row + 1 &&
               abs(startCoord.col - blockingPiece.col) == 1 && end.col == blockingPiece.col;
    }

// FOR HORIZONTAL & VERTICAL
    inline bool movedAwayFromStraightToKing(coord &enemyKing, coord &start, coord &end, coord &blockingPiece) {
        return blockingPiece == start && (
                (enemyKing.row != end.row && enemyKing.col != end.col) ||
                // end doesn't lie on the horizontal/vertical to the king
                sigNum(end.row - enemyKing.row) != sigNum(blockingPiece.row - enemyKing.row) ||
                sigNum(end.col - enemyKing.col) !=
                sigNum(blockingPiece.col - enemyKing.col) // end isn't in the same row or col
        );
    }

// returns if both pieces are pawns and one of the pawn "en passant" the other pawn
    inline bool enPassantOffHorizontal(class Board &curBoard, coord &start, coord &end, coord &firstBlockPiece,
                                       coord &secondBlockPiece) {
        return pieceAt(curBoard, firstBlockPiece)->id == PAWN &&
               pieceAt(curBoard, secondBlockPiece)->id == PAWN &&
               firstBlockPiece.row == secondBlockPiece.row &&
               (firstBlockPiece == start && end.col == secondBlockPiece.col) ||
               (secondBlockPiece == start && end.col == firstBlockPiece.col);
    }

// assumes the king is in the "horizontal/vertical" position from this current piece
    inline bool positionInStraightToKing(coord &enemyKing, coord &position, coord &curPiece) {
        return (curPiece.row == position.row || curPiece.col == position.col) && // on the same row or column
               (sigNum(enemyKing.row - curPiece.row) == sigNum(position.row - curPiece.row)) &&
               sigNum(enemyKing.col - curPiece.col) == sigNum(position.col - curPiece.col);
        // direction from the current piece to the king is the same as the current piece to the position
    }

// FOR SPECIFIC DIRECTIONS
/// isPieceMoveValid: O(1) function seeing if a piece move is valid using a heck ton of if statements
    bool isPieceMoveValid(class Board &curBoard, coord &start, coord &end, coord *threats, int numThreats) {
        if (pieceAt(curBoard, end) && pieceAt(curBoard, end)->isWhite == pieceAt(curBoard, start)->isWhite)
            return false;
        for (int i = 0; i < numThreats; ++i) {
            piece *curPiece = pieceAt(curBoard, threats[i]);
            coord enemyKing = curPiece->isWhite ? curBoard.blackKing : curBoard.whiteKing;
            if (threats[i] == end)
                continue;
            switch (curPiece->id) {
                case PAWN: {
                    int pawnDir = curPiece->isWhite ? -1 : 1;
                    if (enemyKing.row == threats[i].row + pawnDir && abs(enemyKing.col - threats[i].col) == 1)
                        return false;
                    break;
                }
                case BISHOP: {
                    if (curPiece->numPiecesInPath == 0)
                        if (!positionInDiagonalToKing(enemyKing, end, threats[i]))
                            return false;
                        else
                            break;
                    else if (curPiece->numPiecesInPath >= 2)
                        break;

                    coord blockingPiece = curPiece->inPath[0];
                    if (curPiece->numPiecesInPath == 1 &&
                        (movedAwayFromDiagonalToKing(enemyKing, start, end, blockingPiece) ||
                         isDisappearingEnPassant(curBoard, blockingPiece, start, end))) {
                        return false;
                    }
                    break;
                }
                case KNIGHT: {
                    if (end != threats[i]) {
                        return false;
                    }
                    break;
                }
                case ROOK: {
                    if (curPiece->numPiecesInPath == 0) {
                        if (!positionInStraightToKing(enemyKing, end, threats[i])) {
                            return false;
                        }
                        break;
                    }
                    if (curPiece->numPiecesInPath == 1 &&
                        movedAwayFromStraightToKing(enemyKing, start, end, curPiece->inPath[0]))
                        return false;
                    if (curPiece->numPiecesInPath == 2 &&
                        enPassantOffHorizontal(curBoard, start, end, curPiece->inPath[0], curPiece->inPath[1]))
                        return false;
                    break;
                }
                case QUEEN: {
                    if (curPiece->numPiecesInPath == 0) {
                        if (abs(enemyKing.row - threats[i].row) == abs(enemyKing.col - threats[i].col) &&
                            !positionInDiagonalToKing(enemyKing, end, threats[i]) ||
                            // if king's on a diagonal, but position isn't
                            (enemyKing.row == threats[i].row || enemyKing.col == threats[i].col) &&
                            !positionInStraightToKing(enemyKing, end,
                                                      threats[i])) // if the king's straight ahead, but the position isn't
                            return false;
                        break;
                    } else if (curPiece->numPiecesInPath == 1) {
                        coord blockingPiece = curPiece->inPath[0];
                        if (abs(enemyKing.row - threats[i].row) == abs(enemyKing.col - threats[i].col) &&
                            (movedAwayFromDiagonalToKing(enemyKing, start, end, blockingPiece) ||
                             isDisappearingEnPassant(curBoard, blockingPiece, start,
                                                     end)) // one piece in bishop's range
                            ||
                            (enemyKing.row == threats[i].row || enemyKing.col == threats[i].col) &&
                            (movedAwayFromStraightToKing(enemyKing, start, end,
                                                         blockingPiece))) // one piece in rook's range
                            return false;
                        break;
                    } else if (curPiece->numPiecesInPath == 2) {
                        if (enemyKing.row == threats[i].row &&
                            enPassantOffHorizontal(curBoard, start, end, curPiece->inPath[0], curPiece->inPath[1]))
                            return false;
                        break;
                    } else {
                        cout << "the current piece doesn't have 0,1 or 2 pieces in numPiecesInPath; isPieceMoveValid()"
                             << endl;
                        break;
                    }
                }
                case KING: {
                    cout << "The king isn't supposed to be considered here in isPieceMoveValid()" << endl;
                    return false;
                }
            }
        }
        return true;
    }

/// considerPieceTurn: This function doesn't work for king's, those will be rendered by using the "map of influence" from the other pieces
    inline void
    considerPieceTurn(class Board &curBoard, coord start, coord end, turn turns[MAX_MOVES_PER_TURN], int &numTurns, coord *threats,
                      int numThreats) {
        if (isPieceMoveValid(curBoard, start, end, threats, numThreats)) {
            if ((end.row == 0 || end.row == 7) && pieceAt(curBoard, start)->id == PAWN) {
                for (int id : promoteables) {
                    getPromotionTurnOf(start, end, id, turns[numTurns++], curBoard);
                }
            } else {
                getTurnOf(start, end, turns[numTurns++], curBoard);
            }
        }
    }

    void
    considerMovesInDirection(class Board &curBoard, coord &start, turn turns[MAX_MOVES_PER_TURN], int &numMoves, coord *threats, int numThreats,
                             const int direction[2]) {
        for (int r = start.row + direction[0], c = start.col + direction[1];
             0 <= r && r < ROWS && 0 <= c && c < COLS; r += direction[0], c += direction[1]) {
            if (curBoard.board[r][c] == nullptr)
                considerPieceTurn(curBoard, start, {r, c}, turns, numMoves, threats, numThreats);
            else if (curBoard.board[r][c]->isWhite == pieceAt(curBoard, start)->isWhite)
                break;
            else {
                considerPieceTurn(curBoard, start, {r, c}, turns, numMoves, threats, numThreats);
                break;
            }
        }
    }

/// addMoves: For some piece, add all of the adjacent moves
    void addMoves(class Board &curBoard, coord start, coord threats[ROWS * COLS], int numThreats, turn turns[MAX_MOVES_PER_TURN], int& numMoves,
                  bool attacked[ROWS][COLS]) {
        piece *curPiece = pieceAt(curBoard, start);
        switch (curPiece->id) {
            case PAWN: {
                int pawnDir = curPiece->isWhite ? -1 : 1;
                if (!curBoard.board[start.row + pawnDir][start.col]) {
                    considerPieceTurn(curBoard, start, {start.row + pawnDir, start.col}, turns, numMoves, threats, numThreats);
                    if (start.row == (curPiece->isWhite ? 6 : 1) &&
                        !curBoard.board[start.row + 2 * pawnDir][start.col] && !curPiece->didMove) {
                        considerPieceTurn(curBoard, start, {start.row + 2 * pawnDir, start.col}, turns, numMoves, threats,
                                          numThreats);
                    }
                }
                if (start.col < COLS - 1 && curBoard.board[start.row + pawnDir][start.col + 1]) {
                    considerPieceTurn(curBoard, start, {start.row + pawnDir, start.col + 1}, turns, numMoves, threats,
                                      numThreats);
                }
                if (start.col > 0 && curBoard.board[start.row + pawnDir][start.col - 1]) {
                    considerPieceTurn(curBoard, start, {start.row + pawnDir, start.col - 1}, turns, numMoves, threats,
                                      numThreats);
                }
                for (int epCol : {start.col - 1, start.col + 1}) {
                    if (epCol < 0 || epCol >= ROWS)
                        continue;
                    coord enPassantCandidate = {start.row, epCol};
                    int enPassantRow = curPiece->isWhite ? 2 : 5;
                    if (start.row == enPassantRow && pieceAt(curBoard, enPassantCandidate) &&
                        pieceAt(curBoard, enPassantCandidate)->id == PAWN &&
                        pieceAt(curBoard, enPassantCandidate)->lastDoubleMove == curBoard.moveNum) {
                        considerPieceTurn(curBoard, start, {start.row + pawnDir, start.col + 1}, turns, numMoves, threats,
                                          numThreats);
                    }
                }
                return;
            }
            case BISHOP: {
                for (auto direction : bishopMoves) {
                    considerMovesInDirection(curBoard, start, turns, numMoves, threats, numThreats, direction);
                }
                return;
            }
            case KNIGHT: {
                int r, c;
                for (auto direction : knightMoves) {
                    r = start.row + direction[0], c = start.col + direction[1];
                    if (0 <= r && r < ROWS && 0 <= c && c < COLS)
                        considerPieceTurn(curBoard, start, {r, c}, turns, numMoves, threats, numThreats);
                }
                return;
            }
            case ROOK: {
                for (auto direction : rookMoves) {
                    considerMovesInDirection(curBoard, start, turns, numMoves, threats, numThreats, direction);
                }
                return;
            }
            case QUEEN: {
                for (auto direction : allDirectionMoves) {
                    considerMovesInDirection(curBoard, start, turns, numMoves, threats, numThreats, direction);
                }
                return;
            }
            case KING: {
                int r, c;
                for (auto direction : allDirectionMoves) {
                    r = start.row + direction[0], c = start.col + direction[1];
                    if (0 <= r && r < ROWS && 0 <= c && c < COLS &&
                        (!curBoard.board[r][c] || curBoard.board[r][c] &&
                                                  curBoard.board[r][c]->isWhite != pieceAt(curBoard, start)->isWhite) &&
                        !attacked[r][c]) {
                        getTurnOf(start, {r, c}, turns[numMoves++], curBoard);
                    }
                }
                if (!curPiece->didMove) {
                    if (!curBoard.board[start.row][start.col + 1] && !curBoard.board[start.row][start.col + 2] &&
                        !attacked[start.row][start.col + 1] && !attacked[start.row][start.col + 2]) {
                        piece *rookAtRight = curBoard.board[start.row][start.col + 3];
                        if (rookAtRight && rookAtRight->id == ROOK && !rookAtRight->didMove) {
                            getTurnOf(start, {r, c + 3}, turns[numMoves++], curBoard);
                        }
                    }
                    if (!curBoard.board[start.row][start.col - 1] && !curBoard.board[start.row][start.col - 2] &&
                        !curBoard.board[start.row][start.col - 3] &&
                        !attacked[start.row][start.col - 1] && !attacked[start.row][start.col - 2] &&
                        !attacked[start.row][start.col - 3]) {
                        piece *rookAtLeft = curBoard.board[start.row][start.col - 3];
                        if (rookAtLeft && rookAtLeft->id == ROOK && !rookAtLeft->didMove) {
                            getTurnOf(start, {r, c - 3}, turns[numMoves++], curBoard);
                        }
                    }
                }
                return;
            }
        }
    }

    int fillValidMoves(class Board &curBoard, bool whiteToMove, coord threats[ROWS * COLS], int numThreats,
                        bool attackedSquares[ROWS][COLS], turn turns[MAX_MOVES_PER_TURN]) {
        int numMoves = 0;
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c) {
                if (curBoard.board[r][c] && curBoard.board[r][c]->isWhite == whiteToMove) {
                    addMoves(curBoard, {r, c}, threats, numThreats, turns, numMoves, attackedSquares);
                }
            }
        return numMoves;
    }
}
