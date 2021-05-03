//
// Created by danie on 5/3/2021.
//

#include "program.h"

namespace Evaluator{
    using namespace std;

// since we assume single threaded environments, each layer will only deal with their own lists
    coord threats[MAX_LAYERS][ROWS * COLS];
    bool attackedSquares[MAX_LAYERS][ROWS][COLS];

    int amt = 0;
    float temp = 0;
    int getAMT(){return amt;}
    // INCOMPLETE
    float getExpectedRating(class Board *curBoard, int layers, bool whiteToMove) {
        if (layers == 1){
            return curBoard->getRating();
        }
        ++amt;

        // PREPROCESSING
        // Fill the threats
        int numThreats = preprocess::fillThreats(curBoard, whiteToMove, threats[layers]);

        // fill in which squares are attacked by the enemy
        preprocess::fillAttacked(curBoard, attackedSquares[layers], whiteToMove);

        vector<turn> possibleMoves;
        fillMoves::fillValidMoves(curBoard, whiteToMove, threats[layers], numThreats, attackedSquares[layers], possibleMoves);

        float optimal = whiteToMove ? FLOAT_MIN : FLOAT_MAX;
        for(turn curTurn : possibleMoves){
            curBoard->doTurn(curTurn);
            if(whiteToMove)
                optimal = max(optimal, getExpectedRating(curBoard, layers - 1, !whiteToMove));
            else
                optimal = min(optimal, getExpectedRating(curBoard, layers - 1, !whiteToMove));
            curBoard->undoLast();
        }

        if(optimal > 1000.0 || optimal < -1000.0){
            printBoard(curBoard);
        }

        return optimal;
    }
}