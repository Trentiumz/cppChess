//
// Created by danie on 5/3/2021.
//

#include <iostream>
#include <chrono>
#include "program.h"

namespace Evaluator {
    using namespace std;

    float getExpectedRating(class Board &curBoard, int layers, bool whiteToMove, evalInfo &stepper) {
        ++stepper.amt;
        if (layers <= 1) {
            return rating::getRating(curBoard);
        }

        // PREPROCESSING
        // Fill the threats
        int numThreats = preprocess::fillThreats(curBoard, whiteToMove, stepper.threats[layers]);

        // fill in which squares are attacked by the enemy
        preprocess::fillAttacked(curBoard, stepper.attackedSquares[layers], whiteToMove);

        stepper.numMoves[layers] = 0;
        stepper.numMoves[layers] = fillMoves::fillValidMoves(curBoard, whiteToMove, stepper.threats[layers], numThreats,
                                                             stepper.attackedSquares[layers],
                                                             stepper.possibleMoves[layers]);

        float optimal = whiteToMove ? FLOAT_MIN : FLOAT_MAX;
        for (int turnInd = 0; turnInd < stepper.numMoves[layers]; ++turnInd) {
            boardEdit::doTurn(stepper.possibleMoves[layers][turnInd], curBoard);
            if (whiteToMove)
                optimal = max(optimal, getExpectedRating(curBoard, layers - 1, !whiteToMove, stepper));
            else
                optimal = min(optimal, getExpectedRating(curBoard, layers - 1, !whiteToMove, stepper));
            boardEdit::undoLast(curBoard);
        }

        if (optimal > 1000.0 || optimal < -1000.0) {
//            std::cout << "Extreme optimal rating of " << optimal << " found\n";
//            boardEdit::printBoard(curBoard);
        }

        return optimal;
    }

    void
    setExpectedRating(int startingIDs[ROWS][COLS], bool startingIsWhite[ROWS][COLS], bool startingDidMove[ROWS][COLS],
                      coord moved, turn toDo, int layers, bool whiteToMove, float &returnTo) {
        evalInfo info{};
        class Board b(startingIDs, startingIsWhite, moved, startingDidMove);
        boardEdit::doTurn(toDo, b);
        float thing = getExpectedRating(b, layers, whiteToMove, info);
        cout << info.amt << endl;
        returnTo = thing;
    }
}