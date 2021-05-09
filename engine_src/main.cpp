#include <iostream>
#include "program.h"

using namespace std;

int startingIDs[8][8] = {{ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK},
                         {PAWN, PAWN,   PAWN,   PAWN,  PAWN, PAWN,   PAWN,   PAWN},
                         {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                         {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                         {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                         {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                         {PAWN, PAWN,   PAWN,   PAWN,  PAWN, PAWN,   PAWN,   PAWN},
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
bool startingDidMove[8][8] = {{false, false, false, false, false, false, false, false},
                              {false, false, false, false, false, false, false, false},
                              {false, false, false, false, false, false, false, false},
                              {false, false, false, false, false, false, false, false},
                              {false, false, false, false, false, false, false, false},
                              {false, false, false, false, false, false, false, false},
                              {false, false, false, false, false, false, false, false},
                              {false, false, false, false, false, false, false, false}};

int main() {
    Mover::moveLite toDo = Mover::getOptimalMove(startingIDs, startingIsWhites, startingDidMove, {-1, -1}, true, 7);
    cout << "(" << toDo.start.row << " " << toDo.start.col << ")" << " to " << "(" << toDo.end.row << " " << toDo.end.col << ")" << endl;
    return 0;
}
