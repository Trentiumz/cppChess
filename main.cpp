#include <iostream>
#include "program.h"

using namespace std;

int startingIDs[8][8] = {{ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK},
                         {PAWN, PAWN,   PAWN,   PAWN,  EMPTY, PAWN,   PAWN,   PAWN},
                         {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                         {EMPTY, EMPTY, EMPTY, EMPTY, PAWN, EMPTY, EMPTY, EMPTY},
                         {EMPTY, EMPTY, EMPTY, PAWN, EMPTY, EMPTY, EMPTY, EMPTY},
                         {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                         {PAWN, PAWN,   PAWN,   EMPTY,  PAWN, PAWN,   PAWN,   PAWN},
                         {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK}
};
bool startingIsWhites[8][8] = {{false, false, false, false, false, false, false, false},
                               {false, false, false, false, false, false, false, false},
                               {false, false, false, false, false, false, false, false},
                               {false, false, false, false, false, false, false, false},
                               {false, false, false, true, false, false, false, false},
                               {false, false, false, true, false, false, false, false},
                               {true,  true,  true,  true,  true,  true,  true,  true},
                               {true,  true,  true,  true,  true,  true,  true,  true}};

int main() {
    Board startingBoard(startingIDs, startingIsWhites);
    float rating = Evaluator::getExpectedRating(&startingBoard, 6, true);
    cout << rating << endl;
    cout << Evaluator::getAMT() << endl;
    return 0;
}
