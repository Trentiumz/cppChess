//
// Created by danie on 5/1/2021.
//
#include <iostream>
#include <cmath>
#include "program.h"


int sigNum(int num){return num > 0 ? 1 : (num == 0 ? 0 : -1);}
bool operator==(const coord &first, const coord &second){return first.row == second.row && first.col == second.col;}
bool operator!=(const coord &first, const coord &second){return first.row != second.row || first.col != second.col;}

void Board::doMove(move toDo) {
    // Move the kings to the expected positions
    if (toDo.start == this->whiteKing) {
        this->whiteKing = toDo.end;
    } else if (toDo.start == this->blackKing) {
        this->blackKing = toDo.end;
    }

    if (toDo.type == MOVE) {
        this->board[toDo.end.row][toDo.end.col] = this->board[toDo.start.row][toDo.start.col];
        this->board[toDo.start.row][toDo.start.col] = nullptr;
        this->board[toDo.end.row][toDo.end.col]->didMove = true;

    } else if (toDo.type == DOUBLEPAWN_MOVE) {
        this->board[toDo.end.row][toDo.end.col] = this->board[toDo.start.row][toDo.start.col];
        this->board[toDo.start.row][toDo.start.col] = nullptr;
        this->board[toDo.end.row][toDo.end.col]->lastDoubleMove = this->moveNum;

    } else if (toDo.type == PROMOTE) {
        this->pieceAt(toDo.start)->id = toDo.promotionID;

    } else if (toDo.type == REMOVE) {
        this->board[toDo.start.row][toDo.start.col] = nullptr;
    }
}

void Board::doTurn(turn toDo) {
    this->moves[++moveNum] = toDo;
    for(int i = 0; i < toDo.numMoves; ++i)
        this->doMove(toDo.moves[i]);
}

/// Board::getTurnOf: gets the turn equivalent of a move, doesn't support editing
void Board::getTurnOf(coord start, coord end, turn & toEdit){
    piece * curPiece = this->pieceAt(start);
    // King Castling
    if(curPiece->id == KING && abs(end.col - start.col) == 2){
        toEdit.moves[toEdit.numMoves] = {MOVE, start, end};
        toEdit.moves[toEdit.numMoves].previousDidMove = curPiece->didMove;
        ++toEdit.numMoves;
        int rookCol = end.col > start.col ? 7 : 0;
        toEdit.moves[toEdit.numMoves] = {MOVE, {start.row, rookCol}, {start.row, (start.col + end.col) / 2}};
        toEdit.moves[toEdit.numMoves].previousDidMove = this->board[start.row][rookCol]->didMove;
        ++toEdit.numMoves;
        return;
    }

     if(curPiece->id == PAWN){
         if(abs(end.row - start.row) == 2){
             toEdit.moves[toEdit.numMoves] = {DOUBLEPAWN_MOVE, start, end};
             toEdit.moves[toEdit.numMoves].previousDidMove = curPiece->didMove;
             ++toEdit.numMoves;
         }
         else{
             // capturing smth
             if(abs(end.col - start.col) == 1){
                 // diagonal capture
                 if(this->board[end.row][end.col]){
                     toEdit.moves[toEdit.numMoves] = {REMOVE, end};
                     toEdit.moves[toEdit.numMoves].captured = this->pieceAt(end);
                     ++toEdit.numMoves;
                 }
                 else{
                     // en passant
                     int backDirection = this->board[start.row][start.col]->isWhite ? 1 : -1;
                     toEdit.moves[toEdit.numMoves] = {REMOVE, {end.row + backDirection, end.col}};
                     toEdit.moves[toEdit.numMoves].captured = this->pieceAt({end.row + backDirection, end.col});
                     ++toEdit.numMoves;
                 }
             }
             // move to the end
             toEdit.moves[toEdit.numMoves] = {MOVE, start, end};
             toEdit.moves[toEdit.numMoves].previousDidMove = curPiece->didMove;
             ++toEdit.numMoves;
         }
     }else{
         if(this->board[end.row][end.col]){
             toEdit.moves[toEdit.numMoves] = {REMOVE, end};
             toEdit.moves[toEdit.numMoves].captured = this->pieceAt(end);
             ++toEdit.numMoves;
         }
         toEdit.moves[toEdit.numMoves] = {MOVE, start, end};
         toEdit.moves[toEdit.numMoves].previousDidMove = this->pieceAt(start)->didMove;
         ++toEdit.numMoves;
     }
}

void Board::getPromotionTurnOf(coord start, coord end, int promotionID, turn & toEdit){
    if(abs(end.col - start.col) == 1){
        toEdit.moves[toEdit.numMoves] = {REMOVE, end};
        toEdit.moves[toEdit.numMoves].captured = this->pieceAt(end);
        ++toEdit.numMoves;
    }
    toEdit.moves[toEdit.numMoves] = {MOVE, start, end};
    toEdit.moves[toEdit.numMoves].previousDidMove = this->pieceAt(start);
    ++toEdit.numMoves;
    toEdit.moves[toEdit.numMoves] = {PROMOTE, end};
    toEdit.moves[toEdit.numMoves].promotionID = promotionID;
    ++toEdit.numMoves;
}

void Board::undoLast() {
    turn& lastMove = moves[moveNum--];
    move curMove;
    for(int i = lastMove.numMoves - 1; i >= 0; --i){
        curMove = lastMove.moves[i];
        switch(curMove.type){
            case MOVE:
                this->pieceAt(curMove.end)->didMove = curMove.previousDidMove;
                if(this->pieceAt(curMove.end)->id == KING){
                    if(this->pieceAt(curMove.end)->isWhite)
                        this->whiteKing = curMove.start;
                    else
                        this->blackKing = curMove.start;
                }

                this->board[curMove.start.row][curMove.start.col] = this->board[curMove.end.row][curMove.end.col];
                this->board[curMove.end.row][curMove.end.col] = nullptr; // original was start -> end, so now it's end -> start
                break;
            case DOUBLEPAWN_MOVE:
                this->pieceAt(curMove.end)->lastDoubleMove = NEVERDOUBLEMOVED_VAL;
                this->board[curMove.start.row][curMove.start.col] = this->board[curMove.end.row][curMove.end.col];
                this->board[curMove.end.row][curMove.end.col] = nullptr; // original was start -> end, so now it's end -> start
                break;
            case PROMOTE:
                this->pieceAt(curMove.start)->id = PAWN;
                break;
            case REMOVE:
                this->board[curMove.start.row][curMove.start.col] = curMove.captured;
                break;
        }
    }
}

Board::Board(int id[ROWS][COLS], bool isWhite[ROWS][COLS]){
    int numPieces = 0;
    for(int r = 0; r < ROWS; ++r)
        for(int c = 0; c < COLS; ++c)
            if(id[r][c] != EMPTY){
                pieces[numPieces] = {id[r][c], isWhite[r][c], false, NEVERDOUBLEMOVED_VAL};
                board[r][c] = &pieces[numPieces++];

                if(id[r][c] == KING){
                    if(isWhite[r][c])
                        whiteKing = {r, c};
                    else
                        blackKing = {r, c};
                }
            }
}

void printBoard(class Board * board){
    std::cout << "board!" << std::endl;
    for(int r = 0; r < ROWS; ++r){
        for(int c = 0; c < COLS; ++c){
            if(board->pieceAt({r, c})){
                std::cout << board->pieceAt({r, c})->id << " ";
            }
            else{
                std::cout << "- ";
            }
        }
        std::cout << std::endl;
    }
}