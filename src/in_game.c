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
#include "map.h"

typedef struct PLAYER {
	unsigned short x;
	unsigned short y;
} player_t;

static const int I_SIZE = 513;
static int ** imap;
static bool ** wmap;
static bool w_mov = FALSE;
static bool uK, dK, lK, rK; 
static clock_t then;
static player_t player;
static map_cell_t ** map;
game_obj_t objs[MAX_OBJECTS];
static int mW, mH, nO;

void input();
gsname_t update();
void render(int, int);
void drawGui(int, int);
void setPlayerStart();
void initObjects();

void initInGameState( gs_t * gs) {
	int			 n, i, j;
	float **		fmap;

	gs->name = IN_GAME;
	gs->input = &input;
	gs->update = &update;
	gs->render = &render;

	n = I_SIZE;

	srand(time(NULL));

	fmap = ( float ** ) malloc ( sizeof ( float * ) * n);
	for ( i = 0; i < n; ++i ) {
		fmap[ i ] = ( float * ) calloc ( n, sizeof ( float ) );
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

	ds ( &fmap, n );
	island ( &imap, n );
	normInt ( &imap, n );
	norm ( &fmap, n );
	mult ( &fmap, &imap, n );
	smooth( &imap, n );
	normInt ( &imap, n );

	for ( i = 0; i < n; ++i ) {
		free(fmap[ i ]);
	}
	free(fmap);

	setPlayerStart();
	uK = dK = lK = rK = FALSE;

    /*map = ( map_cell_t ** ) malloc ( sizeof ( map_cell_t * ) * 32);
	for ( i = 0; i < 32; ++i ) {
		map[ i ] = ( map_cell_t * ) calloc ( 32 , sizeof ( map_cell_t ) );
	}

    initObjects();

    errcode_t rc = readMapData("map_file.map", &map, &mW, &mH);

    fprintf(stderr, "\t%s: readMapData() returned %d\n", __FILE__, rc);
    fprintf(stderr, "\t%s: Map size is (%d, %d).\n", __FILE__, mW, mH);

    game_obj_t * objsP = objs;
    rc = readMapObjects("map_file.map", &objsP, &nO);
    fprintf(stderr, "\t%s: readMapObjects() returned %d\n", __FILE__, rc);
    fprintf(stderr, "\t%s: Number of objects is %d.\n", __FILE__, nO);

    for(i = 0; i < nO; i++){
        fprintf(stderr, "\t%s: Object %d\n", __FILE__, i);
        fprintf(stderr, "\t\t Type %d\n", (int)objs[i].type);
        fprintf(stderr, "\t\t x: %d -- y: %d -- eX: %d -- eY: %d -- sX: %d -- sY: %d\n", objs[i].x, objs[i].x, objs[i].eX, objs[i].eY, objs[i].sX, objs[i].sY);
        fprintf(stderr, "\t\t iD: %d -- dId %d\n", objs[i].id, objs[i].dId);
        fprintf(stderr, "\t\t name: %s\n", objs[i].name);
        fprintf(stderr, "\t\t target: %s\n", objs[i].target);
        fprintf(stderr, "\t\t dialog: %s\n", objs[i].dialog);
        fprintf(stderr, "\t\t unlocked: %d\n", objs[i].unlocked);
        fprintf(stderr, "\n");
    }

    for ( i = 0; i < 32; ++i ) {
		free(map[ i ]);
	}
	free(map);*/
}

void input(){
	int key = 0;

	key = getch();

	if(key != ERR){
		fprintf(stderr, "\t%s: Caught keycode %d\n", __FILE__, key);
		if(key == KEY_UP) uK = TRUE;
		if(key == KEY_DOWN) dK = TRUE;
		if(key == KEY_LEFT) lK = TRUE;
		if(key == KEY_RIGHT) rK = TRUE;
	}
}

gsname_t update(){
	if(uK){
		if(terrainType( imap[player.x][player.y - 1] ) != DEEP_WATER && terrainType( imap[player.x][player.y - 1] ) != MOUNTAIN) player.y -= 1;
		uK = FALSE;
	}

	if(dK){
		if(terrainType( imap[player.x][player.y + 1]) != DEEP_WATER && terrainType( imap[player.x][player.y + 1]) != MOUNTAIN ) player.y += 1;
		dK = FALSE;
	}

	if(lK){
		if(terrainType( imap[player.x - 1][player.y]) != DEEP_WATER && terrainType( imap[player.x - 1][player.y]) != MOUNTAIN) player.x -= 1;
		lK = FALSE;
	}

	if(rK){
		if(terrainType( imap[player.x + 1][player.y]) != DEEP_WATER && terrainType( imap[player.x + 1][player.y]) != MOUNTAIN) player.x += 1;
		rK = FALSE;
	}

	return IN_GAME;
}

void render(int w, int h){
	clock_t now, delta;
	int i, j, pi, pj, ioff, joff, di, dj;

	now = clock();
	delta = now - then;
	if((float)delta / (float)CLOCKS_PER_SEC >= 0.25f){
		then = now;
		w_mov = TRUE;
	}

	pi = (((w - 1) - 27) / 2) + 27;
	pj = (h - 2) / 2 + 1;

	ioff = (w - 28) / 2;
	joff = (h - 2) / 2;

	for(i = 27; i < w - 1; i++){
		for(j = 1; j < h - 1; j++){
			move(j, i);

			di = i - 27 + player.x - ioff;
			dj = j - 1 + player.y - joff;

			if( di < 0 || di >= I_SIZE){
				printw(" ");
			}else{
				switch(terrainType( imap[di][dj] )){
					case DEEP_WATER:
						attron(COLOR_PAIR(DW_COLOR));
						if(w_mov)
							wmap[di][dj] = !wmap[di][dj];
						if(wmap[di][dj])
							printw("\u2248");
						else
							printw("~");
						break;
					case SHALLOW_WATER:
						attron(COLOR_PAIR(SW_COLOR));
						if(w_mov)
							wmap[di][dj] = !wmap[di][dj];
						if(wmap[di][dj])
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
			}
		}
	}
	w_mov = FALSE;

	move(pj, pi);
	attron(COLOR_PAIR(BSC_COLOR));
	printw(/*"\u263A"*/ "@");

	drawGui(w, h);
}

void drawGui(int w, int h){
	int i, j;

	attron(COLOR_PAIR(BSC_COLOR));

	/* Clear the gui space. */
	for(i = 1; i < 26; i++){
		for(j = 1; j < h - 1; j++){
			move(j, i);
			printw(" "); 
		}
	}

	/* Upper horizontal bar. */
	move(0, 0);
	printw("\u2554");
	for(i = 0; i < w - 2; i++){
		if(i != 25){
			printw("\u2550");
		}else{
			printw("\u2566");
		}
	}
	printw("\u2557");

	/* Lower horizontal bar. */
	move(h - 1, 0);
	printw("\u255A");
	for(i = 0; i < w - 2; i++){
		if(i != 25){
			printw("\u2550");
		}else{
			printw("\u2569");
		}
	}
	printw("\u255D");

	/* Vertical bars. */
	for(i = 1; i < h - 1; i++){
		move(i, 0);
		printw("\u2551");
		move(i, 26);
		printw("\u2551");
		move(i, w-1);
		printw("\u2551");
	}
}

void setPlayerStart(){
	int x, y;
	bool posFound = false;

	while(!posFound){
		x = (I_SIZE / 4) + (rand() % (I_SIZE / 2));
		y = (I_SIZE / 4) + (rand() % (I_SIZE / 2));

		if(terrainType(imap[x][y]) == GRASS){
			player.x = x;
			player.y = y;
			posFound = true;
		}
	}
}

void initObjects(){
    int i;

    for(i = 0; i < MAX_OBJECTS; ++i){
        objs[i].type = NONE;
        objs[i].x = 0;
        objs[i].y = 0;
        objs[i].eX = 0;
        objs[i].eY = 0;
        objs[i].sX = 0;
        objs[i].sY = 0;
        objs[i].id = 0;
        objs[i].dId = 0;
        objs[i].name[0] = '\0';
        objs[i].target[0] = '\0';
        objs[i].dialog[0] = '\0';
        objs[i].unlocked = 0;
    }
}
