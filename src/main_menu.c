/**
 * Copyright (c) 2014, Miguel Angel Astor Romero. All rights reserved.
 * See the file LICENSE for more details.
 */

#include <stdio.h>
#include <string.h>
#include <ncursesw/ncurses.h>

#include "constants.h"
#include "main_menu.h"

static const char * title    = "NOT SO ROGUE-LIKE";
static const char * subtitle = "A game for the Cyberpunk Jam 2014";
static const char * opt1     = "New game";
static const char * opt2     = "Replay intro";
static const char * opt3     = "Quit";
static const char * creds    = "Designed and programmed by Miky";
static const char * info     = "Press enter to select an option. Scroll with the arrow keys.";
static const char * info2    = "Press escape while in game to return to the main menu.";

static int	   selOpt = 0;
static bool	  uK, dK, esc, enter;

void mmInput();
gsname_t mmUpdate();
void mmRender(int, int);

void initMMState(gs_t * gs){
	gs->name = MENU;
	gs->input = &mmInput;
	gs->update = &mmUpdate;
	gs->render = &mmRender;
}

void mmInput(){
	int key = 0;

	key = getch();

	if(key != ERR){
		if(key == KEY_UP) uK = TRUE;
		if(key == KEY_DOWN) dK = TRUE;
		if(key == 27) esc = TRUE;
		if(key == KEY_ENTER || key == '\n') enter = TRUE;
	}
}

gsname_t mmUpdate(){
	if(uK){
		selOpt = selOpt - 1 < 0 ? 2 : selOpt - 1;
		uK = FALSE;
	}

	if(dK){
		selOpt = (selOpt + 1) % 3;
		dK = FALSE;
	}

	if(esc) return -1;

	if(enter){
		enter = FALSE;
		if(selOpt == 0) return IN_GAME;
		else if(selOpt == 1) return INTRO;
		else return -1;
	}

	return MENU;
}

void mmRender(int w, int h){
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

	/* Print the menu options. */
	sW = strlen(opt1);
	sW /= 2;

	move((h / 2) - 2, (w / 2) - sW);
	if(selOpt == 0) attron(COLOR_PAIR(GR_COLOR));
	else attron(COLOR_PAIR(DW_COLOR));
	printw(opt1);

	sW = strlen(opt2);
	sW /= 2;

	move((h / 2) - 1, (w / 2) - sW);
	if(selOpt == 1) attron(COLOR_PAIR(GR_COLOR));
	else attron(COLOR_PAIR(DW_COLOR));
	printw(opt2);

	sW = strlen(opt3);
	sW /= 2;

	move((h / 2), (w / 2) - sW);
	if(selOpt == 2) attron(COLOR_PAIR(GR_COLOR));
	else attron(COLOR_PAIR(DW_COLOR));
	printw(opt3);

	/* Print help. */
	sW = strlen(info);
	sW /= 2;

	move(h - 5, (w / 2) - sW);
	attron(COLOR_PAIR(MN_COLOR));
	printw(info);

	sW = strlen(info2);
	sW /= 2;

	move(h - 4, (w / 2) - sW);
	attron(COLOR_PAIR(MN_COLOR));
	printw(info2);

	/* Print credits. */
	sW = strlen(creds);
	sW /= 2;

	attron(A_BOLD);
	move(h - 2, (w / 2) - sW);
	attron(COLOR_PAIR(SW_COLOR));
	printw(creds);
	attroff(A_BOLD);
}
