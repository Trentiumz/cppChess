//
// Created by danie on 5/9/2021.
//

#include <iostream>
#include "program.h"
#include <vector>
#include <thread>

namespace Mover{
    using namespace std;

    coord threats[ROWS * COLS];
    bool attacked[ROWS][COLS];
    float ratings[ROWS * COLS * 28];
    turn getOptimalMove(int startingIDs[ROWS][COLS], bool startingIsWhites[ROWS][COLS], bool startingDidMove[ROWS][COLS], coord doubleMove, bool whiteToMove, int layers){
        Board sampleBoard(startingIDs, startingIsWhites, doubleMove, startingDidMove);

        int numThreats = preprocess::fillThreats(sampleBoard, whiteToMove, threats);
        preprocess::fillAttacked(sampleBoard, attacked, whiteToMove);

        turn possibleTurns[MAX_MOVES_PER_TURN];
        int numMoves = fillMoves::fillValidMoves(sampleBoard, whiteToMove, threats, numThreats, attacked, possibleTurns);

        vector<thread> evaluations;
        evaluations.reserve(numMoves);

        for(int i = 0; i < numMoves; ++i){
//            Evaluator::setExpectedRating(startingIDs, startingIsWhites, startingDidMove, doubleMove, possibleTurns[i], layers - 1, !whiteToMove, ratings[i]);
            evaluations.emplace_back(Evaluator::setExpectedRating, ref(startingIDs), ref(startingIsWhites), ref(startingDidMove), doubleMove, possibleTurns[i], layers - 1, !whiteToMove, ref(ratings[i]));
        }
        cout << "threads added" << endl;

        for(auto & evaluation : evaluations){
            evaluation.join();
        }

        float optimalRating = whiteToMove ? -10000 : 10000;
        turn optimalMove{};

        for(int i = 0; i < numMoves; ++i){
            if(whiteToMove && ratings[i] > optimalRating || !whiteToMove && ratings[i] < optimalRating){
                optimalRating = ratings[i];
                for(int m = 0; m < possibleTurns[i].numMoves; ++m){
                    if(possibleTurns[i].moves[m].type == MOVE){
                        optimalMove = possibleTurns[i];
                    }
                }
            }
        }

        return optimalMove;
    }
}