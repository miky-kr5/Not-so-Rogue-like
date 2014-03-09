/**
 * Copyright (c) 2014, Miguel Angel Astor Romero. All rights reserved.
 * See the file LICENSE for more details.
 */

#include <stdio.h>
#include <string.h>
#include <ncursesw/ncurses.h>

#include "constants.h"
#include "game_over.h"

static const char * title        = "NOT SO ROGUE-LIKE";
static const char * subtitle     = "A game for the Cyberpunk Jam 2014";
static const char * aWinnerIsYou = "You have completed the scenario!";
static const char * thanks       = "Thank you for playing.";
static const char * goInfo       = "Press enter to return to the main menu.";

static bool	   enter;

void goInput();
gsname_t goUpdate();
void goRender(int, int);

void initGOState(gs_t * gs){
	gs->name = GAME_OVER;
	gs->input = &goInput;
	gs->update = &goUpdate;
	gs->render = &goRender;
}

void goInput(){
	int key = 0;

	key = getch();

	if(key != ERR){
		if(key == KEY_ENTER || key == '\n') enter = TRUE;
	}
}

gsname_t goUpdate(){
	if(enter){
		enter = FALSE;
		return MENU;
	}

	return GAME_OVER;
}

void goRender(int w, int h){
	int sW;

	clear_screen(w, h);

	/* Print the title. */
	sW = strlen(title);
	sW /= 2;

	attron(A_BOLD);

	move(1, (w / 2) - sW);
	attron(COLOR_PAIR(SN_COLOR));
	printw(title);

	/* Print the subtitle. */
	sW = strlen(subtitle);
	sW /= 2;

	move(2, (w / 2) - sW);
	attron(COLOR_PAIR(SW_COLOR));
	printw(subtitle);

	attroff(A_BOLD);

	/* Print the game over message. */
	sW = strlen(aWinnerIsYou);
	sW /= 2;

	move((h / 2) - 2, (w / 2) - sW);
	attron(COLOR_PAIR(GR_COLOR));
	printw(aWinnerIsYou);

	sW = strlen(thanks);
	sW /= 2;

	move((h / 2) - 1, (w / 2) - sW);
	attron(COLOR_PAIR(GR_COLOR));
	printw(thanks);

	/* Print help. */
	sW = strlen(goInfo);
	sW /= 2;

	move(h - 5, (w / 2) - sW);
	attron(COLOR_PAIR(MN_COLOR));
	printw(goInfo);
}

