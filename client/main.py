from runner import *
from chess import *

total_dim = 800
piece_dim = total_dim // 8


class State:
    def setup(self):
        pass

    def draw(self):
        pass

    def mouse_pressed(self, event):
        pass


class MenuState(State):
    wallpaper: Image = None
    button_idle: Image = None
    button_hover: Image = None

    button_width = 400
    button_height = 150
    button_x = (total_dim - button_width) // 2
    button_y = (total_dim - button_height) // 2

    music = Sound("./assets/menu/StartMusic.wav")

    def __init__(self):
        self.wallpaper = Image("./assets/menu/background.jpg", total_dim, total_dim)
        self.button_idle = Image("./assets/menu/button.png", self.button_width, self.button_height)
        self.button_hover = Image("./assets/menu/button_hover.png", self.button_width, self.button_height)

    def setup(self):
        self.music.play(True)

    def in_bounds(self):
        mx, my = get_mouse_pos()
        return self.button_x <= mx <= self.button_x + self.button_width and self.button_y <= my <= self.button_y + self.button_height

    def draw(self):
        image(self.wallpaper, 0, 0)
        if self.in_bounds():
            image(self.button_hover, self.button_x, self.button_y)
        else:
            image(self.button_idle, self.button_x, self.button_y)

    def mouse_pressed(self, event):
        if event.button == LEFT and self.in_bounds():
            self.music.stop(500)
            change_state(PlayingState())


class PlayingState(State):
    images = {}
    wallpaper = Image("./assets/sprites/board.png", total_dim, total_dim)
    music = Sound("./assets/DuringMusic.wav")

    def __init__(self):
        sides = ("b", "w")
        pieces = ("bishop", "king", "knight", "pawn", "rook", "queen")
        for side in sides:
            self.images[side] = {}
            for piece in pieces:
                self.images[side][piece] = Image(f"./assets/sprites/{side}{piece}.png", piece_dim, piece_dim)

    def setup(self):
        self.music.play(True)

    def draw(self):
        image(self.wallpaper, 0, 0)


cur_state: State = MenuState()
menu = MenuState()
player = PlayingState()


# HOW TO USE RUNNER.PY
# extend the Application class in runner.py
# create a new constructor of your extended class
class ChessApplication(Application):

    def __init__(self, width, height):
        super().__init__(width, height)

    def setup(self):
        cur_state.setup()
        Board()

    def mouse_pressed(self, event):
        cur_state.mouse_pressed(event)

    def draw(self):
        cur_state.draw()


def change_state(new_state):
    global cur_state
    cur_state = new_state
    cur_state.setup()


if __name__ == "__main__":
    ChessApplication(total_dim, total_dim)
