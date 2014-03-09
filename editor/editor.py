#!/usr/bin/env python

# Copyright (c) 2014, Miguel Angel Astor Romero. All rights reserved.
# See the file LICENSE for more details.

"""
The map editor for TITLE PENDING, a game for the Cyberpunk Jam 2014.

Usage:
    *) Click on a game_map cell to set it's type.
       Use the keypad numbers to change the type of cell a cell will turn into
       when clicked. The active cell type is displayed on the title bar.
    
    *) Left click to place an object on the game_map.
       Objects have to be edited by hand once the game_map has been saved. Use
       the keys 'd', 'k', 'p', 'n' and 'e' to set the active object type. The
       active object type is displayed on the title bar.
    
    *) Press 'q' to set the player's starting position.

    *) Press 's' to save the map to the text file 'map_file.map'. This will
       replace any previous map with the same name!

    Press escape to quit.
"""

import pygame

CELL_SIZE    = 10
MAP_SIZE     = (64, 64)
SCREEN_SIZE  = ((MAP_SIZE[1] * CELL_SIZE) + 1, (MAP_SIZE[0] * CELL_SIZE) + 1)
SCREEN_FLAGS = pygame.HWSURFACE | pygame.DOUBLEBUF
TITLE        = "CyJam :: "
FPS          = 60

# Cell types
VOID        = 0
SOLID_WALL  = 1
SECRET_WALL = 2
CLEAR_WALL  = 3
NEON_WALL   = 4
WINDOW_WALL = 5
EMPTY_FLOOR = 6
RUG         = 7
WATER       = 8
BAR         = 9

# Object types
DOOR   = 0
KEY    = 1
PERSON = 2
PLAYER = 3
EXIT   = 4

def get_object_type_name(game_object):
    """ Return a string name for an object type. """
    name = ""
    if game_object == DOOR:
        name = "DOOR"
    elif game_object == KEY:
        name = "KEY"
    elif game_object == PERSON:
        name = "PERSON"
    elif game_object == PLAYER:
        name = "PLAYER"
    elif game_object == EXIT:
        name = "EXIT"
    elif game_object == None:
        name = "NONE"
    else:
        raise ValueError("Invalid object type.")

    return name

def get_object_type_color(game_object):
    """ Return a string name for an object type. """
    color = (0, 0, 0)
    if game_object == DOOR:
        color = (255, 0, 255)
    elif game_object == KEY:
        color = (255, 255, 0)
    elif game_object == PERSON:
        color = (0, 255, 255)
    elif game_object == PLAYER:
        color = (128, 255, 0)
    elif game_object == EXIT:
        color = (255, 0, 255)
    else:
        raise ValueError("Invalid object type.")

    return color

def get_floor_type_name(floor_type):
    """ Return a string name for a floor type. """
    name = ""

    if floor_type == VOID:
        name = "VOID"
    elif floor_type == SOLID_WALL:
        name = "SOLID WALL"
    elif floor_type == SECRET_WALL:
        name = "SECRET WALL"
    elif floor_type == CLEAR_WALL:
        name = "CLEAR WALL"
    elif floor_type == NEON_WALL:
        name = "NEON WALL"
    elif floor_type == WINDOW_WALL:
        name = "WINDOW"
    elif floor_type == EMPTY_FLOOR:
        name = "EMPTY FLOOR"
    elif floor_type == RUG:
        name = "RUG"
    elif floor_type == WATER:
        name = "WATER"
    elif floor_type == BAR:
        name = "BAR"
    else:
        raise ValueError("Invalid floor type.")

    return name

def get_floor_type_color(floor_type):
    """ Return a string name for a floor type. """
    color = (0, 0, 0)

    if floor_type == VOID:
        color = (0, 0, 0)
    elif floor_type == SOLID_WALL:
        color = (255, 255, 255)
    elif floor_type == SECRET_WALL:
        color = (128, 128, 128)
    elif floor_type == CLEAR_WALL:
        color = (50, 50, 50)
    elif floor_type == NEON_WALL:
        color = (255, 0, 0)
    elif floor_type == WINDOW_WALL:
        color = (128, 128, 255)
    elif floor_type == EMPTY_FLOOR:
        color = (128, 128, 0)
    elif floor_type == RUG:
        color = (128, 0, 128)
    elif floor_type == WATER:
        color = (64, 64, 255)
    elif floor_type == BAR:
        color = (255, 255, 0)
    else:
        raise ValueError("Invalid floor type.")

    return color

def save_map(game_map):
    p_id = 0
    dialogs = list()
    f = open("map_file.map", "w")

    f.write("[MAP]\n")
    f.write(str(MAP_SIZE [0]) + " " + str(MAP_SIZE[1]) + "\n")
    for i in range(0, MAP_SIZE[0]):
        for j in range(0, MAP_SIZE[1]):
            f_t = game_map[i][j].get_type();
            f.write(str(f_t))
        f.write("\n")

    f.write("[PLAYER]\n")
    f.write("%player = X Y\n")
    for i in range(0, MAP_SIZE[0]):
        for j in range(0, MAP_SIZE[1]):
            o_t = game_map[i][j].get_object();
            if o_t is not None and o_t == PLAYER:
                f.write("player = " + str(i) + " " + str(j) + "\n")

    f.write("[EXITS]\n")
    f.write("%exit = X Y MAP_NAME MAP_X MAP_Y\n")
    for i in range(0, MAP_SIZE[0]):
        for j in range(0, MAP_SIZE[1]):
            o_t = game_map[i][j].get_object();
            if o_t is not None and o_t == EXIT:
                f.write("exit = " + str(i) + " " + str(j))
                f.write(" map_file.map 0 0\n")

    f.write("[DOORS]\n")
    f.write("%door = X Y ID UNLOCKED\n")
    for i in range(0, MAP_SIZE[0]):
        for j in range(0, MAP_SIZE[1]):
            o_t = game_map[i][j].get_object();
            if o_t is not None and o_t == DOOR:
                f.write("door = " + str(i) + " " + str(j) + " 0" + " 0\n")

    f.write("[KEYS]\n")
    f.write("%key = X Y ID\n")
    for i in range(0, MAP_SIZE[0]):
        for j in range(0, MAP_SIZE[1]):
            o_t = game_map[i][j].get_object();
            if o_t is not None and o_t == KEY:
                f.write("key = " + str(i) + " " + str(j) + " 0\n")

    f.write("[PERSONS]\n")
    f.write("%person = X Y NAME DIALOG_ID\n")
    for i in range(0, MAP_SIZE[0]):
        for j in range(0, MAP_SIZE[1]):
            o_t = game_map[i][j].get_object();
            if o_t is not None and o_t == PERSON:
                f.write("person = " + str(i) + " " + str(j))
                f.write(" SOMEONE " + str(p_id) + "\n")
                dialogs.append(p_id)
                p_id += 1

    f.write("[DIALOGS]\n")
    f.write("%dialog = ID TEXT\n")
    for id in dialogs:
        text = " Heavy boxes perform quick waltzes and jigs.\n"
        f.write("dialog = " + str(id) + text)

    f.close()

class Cell:
    """ A game_map cell. """
    def __init__(self, x_size, y_size):
        """ Create a new cell. """
        self.x_size = x_size
        self.y_size = y_size

        self.floor_type = VOID
        self.game_object = None

        # Create the drawing rectangles.
        self.border = pygame.Rect((0, 0), (CELL_SIZE, CELL_SIZE))
        self.cell = pygame.Rect((0, 0), (CELL_SIZE - 2, CELL_SIZE - 2))

    def set_type(self, floor_type):
        """ Set this cell floor type. Raises a value error if the type is
            not valid. """
        if floor_type < VOID or floor_type > BAR:
            raise ValueError("Floor type must be a number between 0 and 9.")
        else:
            self.floor_type = floor_type

    def get_type(self):
        """ Returns this cell's type. """
        return self.floor_type

    def set_object(self, game_object):
        """ Set this cells object. """
        if game_object is not None and game_object < DOOR or game_object > EXIT:
            raise ValueError("Object type must be a number between 0 and 2.")
        else:
            self.game_object = game_object

    def get_object(self):
        """ Return this cell's object. """
        return self.game_object

    def draw(self, canvas, x_pos, y_pos):
        """ Render this cell at the given position on the given screen. """
        self.border.center = (x_pos, y_pos)
        self.cell.center = (x_pos, y_pos)
        pygame.draw.rect(canvas, (0, 0, 255), self.border)

        color = get_floor_type_color(self.floor_type)
        pygame.draw.rect(canvas, color, self.cell)

        if self.game_object is not None and self.game_object < PLAYER:
            r = (CELL_SIZE - 2) // 2
            color = get_object_type_color(self.game_object)
            pygame.draw.circle(canvas, color, (x_pos, y_pos), r, 1)
        elif self.game_object is not None and self.game_object >= PLAYER:
            r = (CELL_SIZE - 3) // 2
            color = get_object_type_color(self.game_object)
            pygame.draw.circle(canvas, color, (x_pos, y_pos), r, 0)

def main():
    """ Application entry point. """
    # Local variables
    done = False
    game_map = list()
    curr_obj = None
    curr_fl = VOID
    obj_str = ""
    fl_str = ""
    mouse_pos = (0, 0)
    mouse_cell = (0, 0)
    mouse_r_click = False
    mouse_l_click = False

    # Initialize Pygame.
    pygame.init()
    clock = pygame.time.Clock()
    screen  = pygame.display.set_mode(SCREEN_SIZE, SCREEN_FLAGS)
    pygame.mouse.set_visible(True)

    for i in range(0, MAP_SIZE[0]):
        game_map.append([Cell(10, 10) for j in range(0, MAP_SIZE[1])])

    # Main game loop.
    while(not done):
        try:
            fps = clock.get_fps() + 0.001

            # Set title bar.
            obj_str = get_object_type_name(curr_obj)
            fl_str = get_floor_type_name(curr_fl)
            title = TITLE + str(mouse_cell) + " :: FPS: " + str(int(fps))
            title += " :: " + obj_str + " :: " + fl_str
            pygame.display.set_caption(title)

            # Input capture.
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    done = True

                if event.type == pygame.MOUSEBUTTONDOWN:
                    mouse_pos = event.pos
                    if event.button == 1:
                        mouse_r_click = True
                    elif event.button == 3:
                        mouse_l_click = True

                if event.type == pygame.MOUSEMOTION:
                    i = mouse_pos[1] // CELL_SIZE
                    j = mouse_pos[0] // CELL_SIZE
                    mouse_cell = (i, j)
                    if mouse_r_click:
                        mouse_pos = event.pos

                if event.type == pygame.MOUSEBUTTONUP:
                    mouse_pos = event.pos
                    if event.button == 1:
                        mouse_r_click = False
                    elif event.button == 3:
                        mouse_l_click = False

                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_ESCAPE:
                        done = True

                    if event.key == pygame.K_d:
                        curr_obj = DOOR
                    if event.key == pygame.K_k:
                        curr_obj = KEY
                    if event.key == pygame.K_p:
                        curr_obj = PERSON
                    if event.key == pygame.K_q:
                        curr_obj = PLAYER
                    if event.key == pygame.K_e:
                        curr_obj = EXIT
                    if event.key == pygame.K_n:
                        curr_obj = None

                    if event.key == pygame.K_0:
                        curr_fl = VOID
                    if event.key == pygame.K_1:
                        curr_fl = SOLID_WALL
                    if event.key == pygame.K_2:
                        curr_fl = SECRET_WALL
                    if event.key == pygame.K_3:
                        curr_fl = CLEAR_WALL
                    if event.key == pygame.K_4:
                        curr_fl = NEON_WALL
                    if event.key == pygame.K_5:
                        curr_fl = WINDOW_WALL
                    if event.key == pygame.K_6:
                        curr_fl = EMPTY_FLOOR
                    if event.key == pygame.K_7:
                        curr_fl = RUG
                    if event.key == pygame.K_8:
                        curr_fl = WATER
                    if event.key == pygame.K_9:
                        curr_fl = BAR

                    if event.key == pygame.K_s:
                        save_map(game_map)

            # Update cycle.
            if mouse_r_click:
                i = mouse_pos[1] // CELL_SIZE
                j = mouse_pos[0] // CELL_SIZE
                if i >= MAP_SIZE[0]:
                    i = MAP_SIZE[0] - 1
                if j >= MAP_SIZE[1]:
                    j = MAP_SIZE[1] - 1
                game_map[i][j].set_type(curr_fl)

            if mouse_l_click:
                i = mouse_pos[1] // CELL_SIZE
                j = mouse_pos[0] // CELL_SIZE
                if i >= MAP_SIZE[0]:
                    i = MAP_SIZE[0] - 1
                if j >= MAP_SIZE[1]:
                    j = MAP_SIZE[1] - 1
                game_map[i][j].set_object(curr_obj)

            # Render cycle.
            screen.fill((0, 0, 0))

            for i in range(0, MAP_SIZE[0]):
                for j in range(0, MAP_SIZE[1]):
                    x = (j * CELL_SIZE) + (CELL_SIZE // 2)
                    y = (i * CELL_SIZE) + (CELL_SIZE // 2)
                    game_map[i][j].draw(screen, x, y)

            pygame.display.update()
            clock.tick(FPS)

        except KeyboardInterrupt:
            done = True

    pygame.quit()

if __name__ == "__main__":
    main()
