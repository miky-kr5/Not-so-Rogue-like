/**
 * Copyright (c) 2014, Miguel Angel Astor Romero. All rights reserved.
 * See the file LICENSE for more details.
 */

#ifndef GAME_STATE_H
#define GAME_STATE_H

static const int NUM_STATES = 4;

typedef enum GAME_STATE_NAMES { INTRO = 0, MENU = 1, IN_GAME = 2, GAME_OVER = 3 } gsname_t;

typedef struct GAME_STATE {
    gsname_t name;
    void (*input)();
    gsname_t (*update)();
    void (*render)(int, int);
} gs_t;

extern void initStateArray(gs_t **);

#endif
