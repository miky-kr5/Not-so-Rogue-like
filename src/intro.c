/**
 * Copyright (c) 2014, Miguel Angel Astor Romero. All rights reserved.
 * See the file LICENSE for more details.
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ncursesw/ncurses.h>

#include "constants.h"
#include "intro.h"
#include "intro_img.h"

void inInput();
gsname_t inUpdate();
void inRender(int, int);

static bool anyKey;
static clock_t then = 0, nThen = 0;
static int mRows = 0, mH = 0;

void initIntroState(gs_t * gs){
	gs->name = INTRO;
	gs->input = &inInput;
	gs->update = &inUpdate;
	gs->render = &inRender;
}

void inInput(){
	int key = 0;

	key = getch();

	if(key != ERR && key != KEY_RESIZE){
		anyKey = TRUE;
	}
}

gsname_t inUpdate(){
	clock_t now, delta;

	if(anyKey){
		anyKey = FALSE;
		mRows = 0;
		then = 0;
		nThen = 0;
		return MENU;
	}

	now = clock();
	delta = now - then;
	if((float)delta / (float)CLOCKS_PER_SEC >= 0.15f){
		then = now;
		mRows = mRows + 1 <= mH ? mRows + 1 : mH;
	}

	if(mRows >= mH){
		now = clock();
		delta = now - nThen;
		if((int)delta / (int)CLOCKS_PER_SEC >= 3){
			nThen = 0;
			mRows = 0;
			then = 0;
			return MENU;
		}
	}else{
		nThen = clock();
	}

	return INTRO;
}

void inRender(int w, int h){
	int i, j, jOff;

	mH = h < 30 ? h : 30;
	jOff = mH < h ? (mH / 4) : 0;

	clear_screen(w, h);

	for(i = 0; i < w && i < 80; i++){
		for(j = 21; j < h + 21 && j < mRows + 21 && j < 80; j++){
			move((j - 21) + jOff, (w / 2) - 40 + i);
			attron(COLOR_PAIR(INTRO_IMG[i][j]));
			printw("\u2588");
		}
	}
}
