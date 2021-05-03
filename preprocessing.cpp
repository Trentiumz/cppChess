//
// Created by danie on 5/3/2021.
//

#include "program.h"
#include <iostream>
using namespace std;

namespace preprocess{

// Updates the pieces that are in a piece's path to the king and returns if the path up to the king is hopelessly blocked
// Moves in the direction of the king; in one of the 8 directions, whichever one is the direction towards the king
    inline bool blockedInPathToKing(piece *curPiece, class Board *curBoard, coord &enemyKingCoord, int &r, int &c) {
        int rDiff = sigNum(enemyKingCoord.row - r);
        int cDiff = sigNum(enemyKingCoord.col - c);
        // Loop through coords up to but not including the king
        for (int cr = r + rDiff, cc = c + cDiff; cr != enemyKingCoord.row || cc != enemyKingCoord.col; cr += rDiff, cc += cDiff) {
            if (curBoard->board[cr][cc] == nullptr)
                continue;
            if (curBoard->board[cr][cc]->isWhite == curPiece->isWhite)
                return true;
            // piece is a piece of opposite color
            if (curPiece->numPiecesInPath >= 2)
                return true;
            curPiece->inPath[curPiece->numPiecesInPath++] = {cr, cc};
        }
        return false;
    }

// Updates the current piece so that it has pieces in its path to the king, and also updates whether or not it checks; returns if the piece is a threat
    bool isThreatToKing(class Board *curBoard, int r, int c) {
        piece *curPiece = curBoard->board[r][c];
        coord enemyKingCoord = curPiece->isWhite ? curBoard->blackKing : curBoard->whiteKing;

        int rDiff, cDiff;
        curPiece->numPiecesInPath = 0;

        switch (curPiece->id) {
            case PAWN:
                if ((curPiece->isWhite && enemyKingCoord.row == r - 1 &&
                     (abs(enemyKingCoord.col - c) == 1)) // if is white and KIR
                    || (!curPiece->isWhite && enemyKingCoord.row == r + 1 && // If is black and KIR
                        (abs(enemyKingCoord.col - c) == 1))) {
                    return true;
                }
                return false;
            case BISHOP:
                if (abs(enemyKingCoord.row - r) != abs(enemyKingCoord.col - c))
                    return false;

                // returns if this piece isn't hopelessly blocked in its path to the king
                return !blockedInPathToKing(curPiece, curBoard, enemyKingCoord, r, c);
            case KNIGHT:
                rDiff = abs(enemyKingCoord.row - r);
                cDiff = abs(enemyKingCoord.col - c);
                if (rDiff == 2 && cDiff == 1 || rDiff == 1 && cDiff == 2) {
                    return true;
                } else
                    return false;
            case ROOK:
                if (enemyKingCoord.row != r && enemyKingCoord.col != c)
                    return false;
                return !blockedInPathToKing(curPiece, curBoard, enemyKingCoord, r, c);
            case QUEEN:
                if ((abs(enemyKingCoord.row - r) != abs(enemyKingCoord.col - c)) && enemyKingCoord.row != r &&
                    enemyKingCoord.col != c)
                    return false;
                return !blockedInPathToKing(curPiece, curBoard, enemyKingCoord, r, c);
            case KING:
                return abs(enemyKingCoord.row - r) + abs(enemyKingCoord.col - c) <= 2;
            default:
                cout << "invalid id from current piece ( id=" << curPiece->id << "; coordinates at (" << r << ", " << c
                     << ")";
                return false;
        }
    }

// returns number of threats
    int fillThreats(class Board *curBoard, bool whiteToMove, coord threats[ROWS * COLS]) {
        int numThreats = 0;
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c) {
                if (curBoard->board[r][c] != nullptr && curBoard->board[r][c]->isWhite != whiteToMove)
                    if (isThreatToKing(curBoard, r, c))
                        threats[numThreats++] = {r, c};
            }
        return numThreats;
    }

// FILL ATTACKED -----------------------------------------------------------------------------

// Marches in one direction, setting all elements to true up and including until it hits a piece
    inline void updateInDirection(class Board *curBoard, bool fill[ROWS][COLS], int row, int col, int rDiff, int cDiff) {
        for (row += rDiff, col += cDiff; 0 <= row && row < ROWS && 0 <= col && col < COLS; row += rDiff, col += cDiff) {
            fill[row][col] = true;
            if (curBoard->board[row][col] != nullptr)
                break;
        }
    }

    void updateSurroundings(class Board *curBoard, bool fill[ROWS][COLS], int row, int col) {
        piece *curPiece = curBoard->board[row][col];
        switch (curPiece->id) {
            case PAWN: {
                int pDiff = curPiece->isWhite ? -1 : 1;
                if(col < COLS - 1)
                    fill[row + pDiff][col + 1] = true;
                if(col > 0)
                    fill[row + pDiff][col - 1] = true;
                return;
            }
            case KNIGHT:
                int nr, nc;
                for (auto knightMove : knightMoves) {
                    nr = row + knightMove[0], nc = col + knightMove[1];
                    if (0 <= nr && nr < ROWS && 0 <= nc && nc < COLS)
                        fill[nr][nc] = true;
                }
                return;
            case BISHOP:
                for (auto direction : bishopMoves) {
                    updateInDirection(curBoard, fill, row, col, direction[0], direction[1]);
                }
                return;
            case ROOK:
                for (auto direction : rookMoves) {
                    updateInDirection(curBoard, fill, row, col, direction[0], direction[1]);
                }
                return;
            case QUEEN:
                for (auto direction : allDirectionMoves) {
                    updateInDirection(curBoard, fill, row, col, direction[0], direction[1]);
                }
                return;
            case KING:
                for (auto direction : allDirectionMoves) {
                    fill[row + direction[0]][col + direction[1]] = true;
                }
                return;
        }
    }

    void fillAttacked(class Board *curBoard, bool fill[ROWS][COLS], bool whiteToMove) {
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c)
                fill[r][c] = false;
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c)
                if (curBoard->board[r][c] != nullptr && curBoard->board[r][c]->isWhite != whiteToMove) {
                    updateSurroundings(curBoard, fill, r, c);
                }
    }
}