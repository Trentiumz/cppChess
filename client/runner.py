import pygame
import time
import sys

sleeper = pygame.time.Clock()

LEFT = 1
RIGHT = 3
MIDDLE = 2

mouse_x = 0
mouse_y = 0

cur_fill = (0, 0, 0)

fps = 30
time_delta = 1 / fps

looping = []


class Image:
    surface = None

    def __init__(self, path, width, height):
        self.surface = pygame.transform.scale(pygame.image.load(path), (width, height))

    def set_alpha(self, alpha):
        self.surface.set_alpha(alpha)


class Sound:
    cur_sound = None
    last_start = 0

    def __init__(self, path):
        if not pygame.mixer.get_init():
            pygame.mixer.init()
        self.cur_sound = pygame.mixer.Sound(path)

    def play(self, loop=False):
        self.cur_sound.play()
        self.last_start = time.time()
        if loop and self not in looping:
            looping.append(self)

    def stop(self, fadeout_time=0):
        self.cur_sound.fadeout(fadeout_time)
        self.last_start = 0

        looping.remove(self)

    def playing(self):
        return time.time() - self.last_start > self.cur_sound.get_length()


class Application:
    width = None
    height = None
    screen = None

    def __init__(self, width, height):
        global main_application, mouse_x, mouse_y, fps
        self.width = width
        self.height = height
        pygame.init()
        self.screen = pygame.display.set_mode((width, height))

        if main_application:
            raise Exception("You can create an application once!")
        else:
            main_application = self

        self.setup()
        while True:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    sys.exit()
                elif event.type == pygame.MOUSEBUTTONDOWN:
                    self.mouse_pressed(event)
                elif event.type == pygame.MOUSEBUTTONUP:
                    self.mouse_released(event)

            for sound in looping:
                if not sound.playing():
                    sound.play()

            mouse_x = pygame.mouse.get_pos()[0]
            mouse_y = pygame.mouse.get_pos()[1]
            self.draw()
            pygame.display.update()

            sleeper.tick(fps)

    def setup(self):
        pass

    def draw(self):
        pass

    def mouse_pressed(self, event):
        pass

    def mouse_released(self, event):
        pass


main_application: Application = None


def background(r, g, b):
    global main_application
    main_application.screen.fill((r, g, b))


def rect(x, y, w, h):
    global cur_fill
    pygame.draw.rect(main_application.screen, cur_fill, (x, y, w, h))


def ellipse(x, y, w, h):
    global cur_fill
    pygame.draw.ellipse(main_application.screen, cur_fill, (x, y, w, h))


def circle(cx, cy, r):
    global cur_fill
    pygame.draw.circle(main_application.screen, cur_fill, (cx, cy), r)


def mouse_down(button):
    return pygame.mouse.get_pressed(button)


def fill(r, g, b):
    global cur_fill
    cur_fill = (r, g, b)


def image(to_add: Image, x, y, alpha=255):
    to_add.set_alpha(alpha)
    main_application.screen.blit(to_add.surface, (x, y))
