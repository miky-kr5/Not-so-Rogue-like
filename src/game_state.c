/**
 * Copyright (c) 2014, Miguel Angel Astor Romero. All rights reserved.
 * See the file LICENSE for more details.
 */

#include "game_state.h"
#include "main_menu.h"
#include "in_game.h"

void initStateArray(gs_t ** s){
    initMMState(&((*s)[MENU]));
    initInGameState(&((*s)[IN_GAME]));
}
