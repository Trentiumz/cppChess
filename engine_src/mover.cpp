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
    moveLite getOptimalMove(int startingIDs[ROWS][COLS], bool startingIsWhites[ROWS][COLS], bool startingDidMove[ROWS][COLS], coord doubleMove, bool whiteToMove, int layers){
        Board sampleBoard(startingIDs, startingIsWhites, doubleMove, startingDidMove);

        int numThreats = preprocess::fillThreats(sampleBoard, whiteToMove, threats);
        preprocess::fillAttacked(sampleBoard, attacked, whiteToMove);

        vector<turn> addTo;
        fillMoves::fillValidMoves(sampleBoard, whiteToMove, threats, numThreats, attacked, addTo);

        vector<thread> evaluations;
        evaluations.reserve(addTo.size());

        for(int i = 0; i < addTo.size(); ++i){
//            Evaluator::setExpectedRating(startingIDs, startingIsWhites, startingDidMove, doubleMove, addTo[i], layers - 1, !whiteToMove, ratings[i]);
            evaluations.emplace_back(Evaluator::setExpectedRating, startingIDs, startingIsWhites, startingDidMove, doubleMove, addTo[i], layers - 1, !whiteToMove, ref(ratings[i]));
        }

        for(auto & evaluation : evaluations){
            evaluation.join();
        }

        float optimalRating = whiteToMove ? -10000 : 10000;
        moveLite optimalMove{};

        for(int i = 0; i < addTo.size(); ++i){
            float n = ratings[i];
            if(whiteToMove && ratings[i] > optimalRating || !whiteToMove && ratings[i] < optimalRating){
                optimalRating = ratings[i];
                for(int m = 0; m < addTo[i].numMoves; ++m){
                    if(addTo[i].moves[m].type == MOVE){
                        optimalMove.start = addTo[i].moves[m].start;
                        optimalMove.end = addTo[i].moves[m].end;
                    }else if(addTo[i].moves[m].type == PROMOTE){
                        optimalMove.promotionID = addTo[i].moves[m].promotionID;
                    }
                }
            }
        }

        return optimalMove;
    }
}