//
// Created by danie on 5/3/2021.
//

#include <iostream>
#include "program.h"

namespace Evaluator {
    using namespace std;

    int amt = 0;

    int getAMT() { return amt; }

    // INCOMPLETE
    float getExpectedRating(class Board &curBoard, int layers, bool whiteToMove, evalInfo &stepper) {
        if (layers == 1) {
            return rating::getRating(curBoard);
        }
        ++amt;

        // PREPROCESSING
        // Fill the threats
        int numThreats = preprocess::fillThreats(curBoard, whiteToMove, stepper.threats[layers]);

        // fill in which squares are attacked by the enemy
        preprocess::fillAttacked(curBoard, stepper.attackedSquares[layers], whiteToMove);

        // TODO filling moves add to the vector; which changes its size; make it so that you don't need to change size(use a 2d list possibleMoves[layers][index]?)
        vector<turn> possibleMoves;
        fillMoves::fillValidMoves(curBoard, whiteToMove, stepper.threats[layers], numThreats,
                                  stepper.attackedSquares[layers], possibleMoves);

        float optimal = whiteToMove ? FLOAT_MIN : FLOAT_MAX;
        for (turn curTurn : possibleMoves) {
            boardEdit::doTurn(curTurn, curBoard);
            if (whiteToMove)
                optimal = max(optimal, getExpectedRating(curBoard, layers - 1, !whiteToMove, stepper));
            else
                optimal = min(optimal, getExpectedRating(curBoard, layers - 1, !whiteToMove, stepper));
            boardEdit::undoLast(curBoard);
        }

        if (optimal > 1000.0 || optimal < -1000.0) {
            std::cout << "Extreme optimal rating of " << optimal << " found\n";
            boardEdit::printBoard(curBoard);
        }

        return optimal;
    }
}