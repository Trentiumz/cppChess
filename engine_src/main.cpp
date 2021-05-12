#include <iostream>
#include "program.h"

using namespace std;

int startingIDs[8][8];
bool startingIsWhites[8][8];
bool startingDidMove[8][8];

int main() {
    // M is get optimal move, L is list the possible moves
    char ins;
    cin >> ins;

    // get board info
    for (auto &startingID : startingIDs)
        for (int &j : startingID)
            cin >> j;
    for (auto &isWhite : startingIsWhites)
        for (bool &i : isWhite)
            cin >> i;
    for (auto &row : startingDidMove)
        for (bool &didMove : row)
            cin >> didMove;

    int dr, dc;
    cin >> dr >> dc;
    coord doubleMovedCoord = {dr, dc};

    bool whiteToMove;
    cin >> whiteToMove;

    if (ins == 'M') {
        turn toDo = Mover::getOptimalMove(startingIDs, startingIsWhites, startingDidMove, doubleMovedCoord,
                                                     whiteToMove, LAYERS);
        printTurn(toDo);
    }else{
        Board curBoard = Board(startingIDs, startingIsWhites, doubleMovedCoord, startingDidMove);
        bool attacked[ROWS][COLS];
        preprocess::fillAttacked(curBoard, attacked, whiteToMove);

        coord threats[ROWS * COLS];
        int numThreats = preprocess::fillThreats(curBoard, whiteToMove, threats);

        turn possibles[MAX_MOVES_PER_TURN];
        int numMoves = fillMoves::fillValidMoves(curBoard, whiteToMove, threats, numThreats, attacked, possibles);
        cout << numMoves << endl;
        for(int i = 0; i < numMoves; ++i)
            printTurn(possibles[i]);
    }
    return 0;
}
