/**
 * Copyright (c) 2014, Miguel Angel Astor Romero. All rights reserved.
 * See the file LICENSE for more details.
 */

#ifndef MAP_H
#define MAP_H

#define MAX_MAP_SIZE 64
#define MAX_OBJECTS 512
#define MAX_STR 128

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
                            DIALOG,
                            NONE = 9989
                          } obj_t;

typedef enum ERROR_CODES {
                            NO_ERROR = 0,
                            FILE_NOT_FOUND,
                            OUT_OF_MEMORY,
                            PREMATURE_EOF,
                            MAP_TOO_LARGE,
                            INVALID_KEY
                         } errcode_t;

typedef struct MAP_CELL{
    floor_t f;
} map_cell_t;

typedef struct OBJECT {
    obj_t type;
    short x, y, eX, eY, sX, sY;
    short id;
    short dId;
    char name[MAX_STR];
    char target[MAX_STR];
    char dialog[MAX_STR];
    unsigned char unlocked;
} game_obj_t;

extern errcode_t readMapData(const char *, map_cell_t ***, int *, int *);
extern errcode_t readMapObjects(const char *, game_obj_t **, int *);

#endif
