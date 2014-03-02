/**
 * Copyright (c) 2014, Miguel Angel Astor Romero. All rights reserved.
 * See the file LICENSE for more details.
 */

#include <stdlib.h>
#include <time.h>
#include <ncursesw/ncurses.h>
#include <island.h>

#include "constants.h"
#include "in_game.h"

static const int I_SIZE = 257;
static int ** imap;
static bool ** wmap;
static bool w_mov = FALSE;
static clock_t then;

void input();
gsname_t update();
void render(int, int);

void initInGameState( gs_t * gs) {
    int             n, i, j;
    float **        map;

    gs->name = IN_GAME;
    gs->input = &input;
    gs->update = &update;
    gs->render = &render;

    n = I_SIZE;

    srand(time(NULL));

    map = ( float ** ) malloc ( sizeof ( float * ) * n);
    for ( i = 0; i < n; ++i ) {
        map[ i ] = ( float * ) calloc ( n, sizeof ( float ) );
    }

    imap = ( int ** ) malloc ( sizeof ( int * ) * n);
    for ( i = 0; i < n; ++i ) {
        imap[ i ] = ( int * ) calloc ( n, sizeof ( int ) );
    }

    wmap = ( bool ** ) malloc ( sizeof ( bool * ) * n);
    for ( i = 0; i < n; ++i ) {
        wmap[ i ] = ( bool * ) calloc ( n, sizeof ( bool ) );
        for(j = 0; j < n; ++j){
            wmap[i][j] = rand() % 2;
        }
    }

    ds ( &map, n );
    island ( &imap, n );
    normInt ( &imap, n );
    norm ( &map, n );
    mult ( &map, &imap, n );
    smooth( &imap, n );
    normInt ( &imap, n );

    for ( i = 0; i < n; ++i ) {
        free(map[ i ]);
    }
    free(map);
}

void input(){
    int key = 0;

    key = getch();

    if(key != ERR){
        fprintf(stderr, "\t%s: Caught keycode %d\n", __FILE__, key);
    }
}

gsname_t update(){
    return IN_GAME;
}

void render(int w, int h){
    clock_t now, delta;
    int i, j;

    now = clock();
    delta = now - then;
    if((float)delta / (float)CLOCKS_PER_SEC >= 0.25f){
        then = now;
        w_mov = TRUE;
    }

	for(i = 0; i < w; i++){
		for(j = 0; j < h; j++){
            move(j, i);

            switch(terrainType( imap[(i + (I_SIZE/4)) % I_SIZE][(j + (I_SIZE/4)) % I_SIZE] )){
                case DEEP_WATER:
                    attron(COLOR_PAIR(DW_COLOR));
                    if(w_mov)
                        wmap[(i + (I_SIZE/4)) % I_SIZE][(j + (I_SIZE/4)) % I_SIZE] = !wmap[(i + (I_SIZE/4)) % I_SIZE][(j + (I_SIZE/4)) % I_SIZE];
                    if(wmap[(i + (I_SIZE/4)) % I_SIZE][(j + (I_SIZE/4)) % I_SIZE])
                        printw("\u2248");
                    else
                        printw("~");
                    break;
                case SHALLOW_WATER:
                    attron(COLOR_PAIR(SW_COLOR));
                    if(w_mov)
                        wmap[(i + (I_SIZE/4)) % I_SIZE][(j + (I_SIZE/4)) % I_SIZE] = !wmap[(i + (I_SIZE/4)) % I_SIZE][(j + (I_SIZE/4)) % I_SIZE];
                    if(wmap[(i + (I_SIZE/4)) % I_SIZE][(j + (I_SIZE/4)) % I_SIZE])
                        printw("\u2248");
                    else
                        printw("~");
                    break;
                case SAND:
                    attron(COLOR_PAIR(SN_COLOR));
                    printw(".");
                    break;
                case GRASS:
                    attron(COLOR_PAIR(GR_COLOR));
                    printw("n");
                    break;
                case FOREST:
                    attron(COLOR_PAIR(FR_COLOR));
                    printw("\u2660");
                    break;
                case HILL:
                    attron(COLOR_PAIR(HL_COLOR));
                    printw("\u2302");
                    break;
                case MOUNTAIN:
                    attron(COLOR_PAIR(MN_COLOR));
                    printw("\u25B2");
                    break;
            }
            //printw("\u2588");
		}
	}
    w_mov = FALSE;
}
