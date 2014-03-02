#include <stdlib.h>
#include <ncursesw/ncurses.h>
#include <island.h>

#include "constants.h"
#include "in_game.h"

static const int I_SIZE = 257;
static int ** imap;

void input();
gsname_t update();
void render(int, int);

void initInGameState( gs_t * gs) {
    int             n, i;
    float **        map;

    gs->name = IN_GAME;
    gs->input = &input;
    gs->update = &update;
    gs->render = &render;

    n = I_SIZE;

    map = ( float ** ) malloc ( sizeof ( float * ) * n);
    for ( i = 0; i < n; ++i ) {
        map[ i ] = ( float * ) calloc ( n, sizeof ( float ) );
    }

    imap = ( int ** ) malloc ( sizeof ( int * ) * n);
    for ( i = 0; i < n; ++i ) {
        imap[ i ] = ( int * ) calloc ( n, sizeof ( int ) );
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
    int i, j;

	for(i = 0; i < w; i++){
		for(j = 0; j < h; j++){
            move(j, i);

            switch(terrainType( imap[(i + (I_SIZE/4)) % I_SIZE][(j + (I_SIZE/4)) % I_SIZE] )){
                case DEEP_WATER:
                    attron(COLOR_PAIR(DW_COLOR));
                    printw("\u2248");
                    break;
                case SHALLOW_WATER:
                    attron(COLOR_PAIR(SW_COLOR));
                    printw("\u2248");
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
}
