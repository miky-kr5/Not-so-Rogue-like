/**
 * Copyright (c) 2014, Miguel Angel Astor Romero. All rights reserved.
 * See the file LICENSE for more details.
 */

#include <ncursesw/ncurses.h>

#include "constants.h"
#include "game_state.h"
#include "intro.h"
#include "main_menu.h"
#include "in_game.h"

void initStateArray(gs_t ** s){
	initIntroState(&((*s)[INTRO]));
	initMMState(&((*s)[MENU]));
	initInGameState(&((*s)[IN_GAME]));
}

void clear_screen(int w, int h){
	int i, j;
	move(0,0);
	attron(COLOR_PAIR(BSC_COLOR));
	for(i = 0; i < w; i++){
		for(j = 0; j < h; j++){
			move(j, i);
			printw(" ");
		}
	}
}
