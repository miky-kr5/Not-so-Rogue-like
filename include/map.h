/**
 * Copyright (c) 2014, Miguel Angel Astor Romero. All rights reserved.
 * See the file LICENSE for more details.
 */

#ifndef MAP_H
#define MAP_H

static const int MAX_MAP_SIZE = 64;

typedef enum FLOOR_TYPES {
                            VOID = 0,
                            SOLID_WALL,
                            SECRET_WALL,
                            CLEAR_WALL,
                            NEON_WALL,
                            WINDOW_WALL,
                            EMPTY_FLOOR,
                            RUG,
                            WATER,
                            BAR
                         } floor_t;

typedef enum OBJECT_TYPES {
                            DOOR = 0,
                            KEY,
                            PERSON,
                            PLAYER_START,
                            EXIT,
                            NONE = 9989
                          } obj_t;

typedef enum ERROR_CODES {
                            NO_ERROR = 0,
                            FILE_NOT_FOUND,
                            OUT_OF_MEMORY,
                            PREMATURE_EOF,
                            MAP_TOO_LARGE
                         } errcode_t;

typedef struct MAP_CELL{
    floor_t f;
} map_cell_t;

extern errcode_t readMapData(const char *, map_cell_t ***, int *, int *);

#endif
