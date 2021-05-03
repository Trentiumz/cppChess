//
// Created by danie on 4/30/2021.
//

#include <vector>

#ifndef CHESSENGINE_PROGRAM_H
#define CHESSENGINE_PROGRAM_H

const int knightMoves[8][2] = {{2,1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
const int bishopMoves[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
const int rookMoves[4][2] = {{1, 0}, {-1, 0}, {0, -1}, {0, 1}};
const int allDirectionMoves[8][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}, {1, 0}, {-1, 0}, {0, -1}, {0, 1}};

#define ROWS 8
#define COLS 8

#define MAX_LAYERS 15

#define ELEMENT_TYPES 6
#define PAWN 0
#define BISHOP 1
#define KNIGHT 2
#define ROOK 3
#define QUEEN 4
#define KING 5
#define EMPTY (-1)

const int promoteables[4] = {BISHOP, KNIGHT, ROOK, QUEEN};

#define MOVE 0
#define PROMOTE 1
#define REMOVE 2
#define DOUBLEPAWN_MOVE 3

#define NEVERDOUBLEMOVED_VAL (-100000)
#define FLOAT_MIN (-10000.0)
#define FLOAT_MAX 10000.0

#define MAX_MOVES 500

const float pieceRatings[ELEMENT_TYPES] = {10, 30, 35, 50, 90, 300};

// Piece structure for inputting
typedef struct coord {
    int row, col;
} coord;

typedef struct piece {
    int id;
    bool isWhite;

    // Occasional parameters
    bool didMove; // did the (rook or king) piece move
    int lastDoubleMove; // the last move number in which the pawn did a double move

    // TEMP VARIABLES

    // stores the elements in the path from this element to the king
    coord inPath[2];
    int numPiecesInPath;
} piece;

// promote: place, {}, promote id; remove: capture position, {}, {}, captured piece
// move: start, end, {}, {}, previously did move
typedef struct move {
    int type;
    coord start, end;

    // extra arguments that sometimes are needed
    // for promotion
    int promotionID;

    // for capturing
    piece * captured;

    // for move functions
    bool previousDidMove;
} move;

typedef struct turn{
    move moves[3];
    int numMoves;
} turn;

// equivalent of board from looking at it in debugger; row 0 is the top of the board and the side of black
class Board {
public:
    piece *board[ROWS][COLS]{};
    // this is just to have a place to store the pieces
    piece pieces[ROWS * COLS]{};

    // move index of the last completed move
    int moveNum = -1;
    turn moves[MAX_MOVES]{};

    coord whiteKing{};
    coord blackKing{};

    float getRating();
    void getTurnOf(coord start, coord end, turn & toEdit);
    void getPromotionTurnOf(coord start, coord end, int promotionID, turn & toEdit);
    inline piece *pieceAt(coord pos){return this->board[pos.row][pos.col];}

    void doTurn(turn toDo);
    void doMove(move toDo);
    void undoLast();

    Board(int id[ROWS][COLS], bool isWhite[ROWS][COLS]);
};

int sigNum(int num);
bool operator==(const coord &first, const coord &second);
bool operator!=(const coord &first, const coord &second);

namespace fillMoves{
    void fillValidMoves(class Board *curBoard, bool whiteToMove, coord threats[ROWS * COLS], int numThreats,
                        bool attackedSquares[ROWS][COLS], std::vector<turn> &addTo);
}
namespace preprocess{
    int fillThreats(class Board *curBoard, bool whiteToMove, coord threats[ROWS * COLS]);
    void fillAttacked(class Board *curBoard, bool fill[ROWS][COLS], bool whiteToMove);
}
namespace Evaluator{
    float getExpectedRating(class Board *curBoard, int layers, bool whiteToMove);
    int getAMT();
}

void printBoard(class Board *curBoard);
#endif //CHESSENGINE_PROGRAM_H
