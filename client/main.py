from runner import *


# HOW TO USE RUNNER.PY
# extend the Application class in runner.py
# create a new constructor of your extended class
class ChessApplication(Application):
    x = 0
    y = 250

    def __init__(self, width, height):
        super().__init__(width, height)

    def mouse_pressed(self, event):
        print(event.button)

    def draw(self):
        background(255, 0, 0)
        self.x += 30 * time_delta
        fill(0, 0, 0)
        rect(self.x, self.y, 30, 30)
        fill(0, 0, 255)
        circle(self.x, self.y, 30)


if __name__ == "__main__":
    ChessApplication(800, 500)
