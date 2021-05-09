//
// Created by danie on 5/1/2021.
//
#include <iostream>
#include <cmath>
#include <algorithm>
#include "program.h"


int sigNum(int num) { return num > 0 ? 1 : (num == 0 ? 0 : -1); }

bool operator==(const coord &first, const coord &second) { return first.row == second.row && first.col == second.col; }

bool operator!=(const coord &first, const coord &second) { return first.row != second.row || first.col != second.col; }

namespace boardEdit {
    inline piece *pieceAt(Board &board, coord &piece) {
        return board.board[piece.row][piece.col];
    }

    void doMove(move toDo, Board &board) {
        // Move the kings to the expected positions
        if (toDo.start == board.whiteKing) {
            board.whiteKing = toDo.end;
        } else if (toDo.start == board.blackKing) {
            board.blackKing = toDo.end;
        }

        if (toDo.type == MOVE) {
            board.board[toDo.end.row][toDo.end.col] = pieceAt(board, toDo.start);
            board.board[toDo.start.row][toDo.start.col] = nullptr;
            board.board[toDo.end.row][toDo.end.col]->didMove = true;

        } else if (toDo.type == DOUBLEPAWN_MOVE) {
            board.board[toDo.end.row][toDo.end.col] = pieceAt(board, toDo.start);
            board.board[toDo.start.row][toDo.start.col] = nullptr;
            board.board[toDo.end.row][toDo.end.col]->lastDoubleMove = board.moveNum;

        } else if (toDo.type == PROMOTE) {
            pieceAt(board, toDo.start)->id = toDo.promotionID;

        } else if (toDo.type == REMOVE) {
            board.board[toDo.start.row][toDo.start.col] = nullptr;
        }
    }

    void doTurn(turn toDo, Board &board) {
        board.moves[++board.moveNum] = toDo;
        for (int i = 0; i < toDo.numMoves; ++i)
            doMove(toDo.moves[i], board);
    }

/// Board::getTurnOf: gets the turn equivalent of a move, doesn't support editing
    void getTurnOf(coord start, coord end, turn &toEdit, Board &board) {
        piece *curPiece = pieceAt(board, start);
        // King Castling
        if (curPiece->id == KING && abs(end.col - start.col) == 2) {
            toEdit.moves[toEdit.numMoves] = {MOVE, start, end};
            toEdit.moves[toEdit.numMoves].previousDidMove = curPiece->didMove;
            ++toEdit.numMoves;
            int rookCol = end.col > start.col ? 7 : 0;
            toEdit.moves[toEdit.numMoves] = {MOVE, {start.row, rookCol}, {start.row, (start.col + end.col) / 2}};
            toEdit.moves[toEdit.numMoves].previousDidMove = board.board[start.row][rookCol]->didMove;
            ++toEdit.numMoves;
            return;
        }

        if (curPiece->id == PAWN) {
            if (abs(end.row - start.row) == 2) {
                toEdit.moves[toEdit.numMoves] = {DOUBLEPAWN_MOVE, start, end};
                toEdit.moves[toEdit.numMoves].previousDidMove = curPiece->didMove;
                ++toEdit.numMoves;
            } else {
                // capturing smth
                if (abs(end.col - start.col) == 1) {
                    // diagonal capture
                    if (pieceAt(board, end)) {
                        toEdit.moves[toEdit.numMoves] = {REMOVE, end};
                        toEdit.moves[toEdit.numMoves].captured = pieceAt(board, end);
                        ++toEdit.numMoves;
                    } else {
                        // en passant
                        toEdit.moves[toEdit.numMoves] = {REMOVE, {start.row, end.col}};
                        toEdit.moves[toEdit.numMoves].captured = board.board[start.row][end.col];
                        ++toEdit.numMoves;
                    }
                }
                // move to the end
                toEdit.moves[toEdit.numMoves] = {MOVE, start, end};
                toEdit.moves[toEdit.numMoves].previousDidMove = curPiece->didMove;
                ++toEdit.numMoves;
            }
        } else {
            if (pieceAt(board, end)) {
                toEdit.moves[toEdit.numMoves] = {REMOVE, end};
                toEdit.moves[toEdit.numMoves].captured = pieceAt(board, end);
                ++toEdit.numMoves;
            }
            toEdit.moves[toEdit.numMoves] = {MOVE, start, end};
            toEdit.moves[toEdit.numMoves].previousDidMove = pieceAt(board, start)->didMove;
            ++toEdit.numMoves;
        }
    }

    void getPromotionTurnOf(coord start, coord end, int promotionID, turn &toEdit, Board &board) {
        if (abs(end.col - start.col) == 1) {
            toEdit.moves[toEdit.numMoves] = {REMOVE, end};
            toEdit.moves[toEdit.numMoves].captured = pieceAt(board, end);
            ++toEdit.numMoves;
        }
        toEdit.moves[toEdit.numMoves] = {MOVE, start, end};
        toEdit.moves[toEdit.numMoves].previousDidMove = pieceAt(board, start);
        ++toEdit.numMoves;
        toEdit.moves[toEdit.numMoves] = {PROMOTE, end};
        toEdit.moves[toEdit.numMoves].promotionID = promotionID;
        ++toEdit.numMoves;
    }

    void undoLast(Board &board) {
        turn &lastMove = board.moves[board.moveNum--];
        move curMove;
        for (int i = lastMove.numMoves - 1; i >= 0; --i) {
            curMove = lastMove.moves[i];
            switch (curMove.type) {
                case MOVE:
                    pieceAt(board, curMove.end)->didMove = curMove.previousDidMove;
                    if (pieceAt(board, curMove.end)->id == KING) {
                        if (pieceAt(board, curMove.end)->isWhite)
                            board.whiteKing = curMove.start;
                        else
                            board.blackKing = curMove.start;
                    }

                    board.board[curMove.start.row][curMove.start.col] = board.board[curMove.end.row][curMove.end.col];
                    board.board[curMove.end.row][curMove.end.col] = nullptr; // original was start -> end, so now it's end -> start
                    break;
                case DOUBLEPAWN_MOVE:
                    pieceAt(board, curMove.end)->lastDoubleMove = NEVERDOUBLEMOVED_VAL;
                    board.board[curMove.start.row][curMove.start.col] = board.board[curMove.end.row][curMove.end.col];
                    board.board[curMove.end.row][curMove.end.col] = nullptr; // original was start -> end, so now it's end -> start
                    break;
                case PROMOTE:
                    pieceAt(board, curMove.start)->id = PAWN;
                    break;
                case REMOVE:
                    board.board[curMove.start.row][curMove.start.col] = curMove.captured;
                    break;
            }
        }
    }

    void printBoard(class Board &board) {
        for (auto &r : board.board) {
            for (auto &c : r) {
                if (c) {
                    std::cout << c->id << " ";
                } else {
                    std::cout << "- ";
                }
            }
            std::cout << std::endl;
        }
    }
}

Board::Board(int id[8][8], bool isWhite[8][8], coord doubleMoved, bool didMove[8][8]) {
    int numPieces = 0;
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            if (id[r][c] != EMPTY) {
                if(r == doubleMoved.row && c == doubleMoved.col)
                    this->pieces[numPieces] = {id[r][c], isWhite[r][c], didMove[r][c], this->moveNum};
                else
                    this->pieces[numPieces] = {id[r][c], isWhite[r][c], didMove[r][c], NEVERDOUBLEMOVED_VAL};
                this->board[r][c] = &pieces[numPieces++];

                if (id[r][c] == KING) {
                    if (isWhite[r][c])
                        this->whiteKing = {r, c};
                    else
                        this->blackKing = {r, c};
                }
            }
}