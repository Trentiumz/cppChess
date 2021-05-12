from copy import deepcopy
import subprocess
import pathlib

path = "./chessEngine"


class Piece:
    id = "E"
    is_white = False

    def __init__(self, piece_id, is_white):
        self.id = piece_id
        self.is_white = is_white


class Board:
    board: list = [[None] * 8 for _ in range(8)]

    def __init__(self):
        print(path)
        self.board[0][0] = Piece(3, True)
        k = deepcopy(self.board)
        self.board[0][0].id = 15
        print(k[0][0].id)

    def get_moves(self):
        pass
