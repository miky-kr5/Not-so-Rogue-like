/**
 * Copyright (c) 2014, Miguel Angel Astor Romero. All rights reserved.
 * See the file LICENSE for more details.
 */

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ncursesw/ncurses.h>
#include <fov.h>

#include "constants.h"
#include "in_game.h"
#include "map.h"

#define MAX_KEYS 15

static const char *keyMsg = "You picked up a key.";
static const char *saysMsg = " says: ";
static const char *openDoor = "The door opens.";
static const char *doorLock = "This door is locked.";

typedef struct PLAYER {
	unsigned short x;
	unsigned short y;
} player_t;

static bool **vis;
static bool **seen;
static bool ** wmap;
static bool w_mov = FALSE;
static bool uK, dK, lK, rK, esc; 
static clock_t then, msgThen;
static player_t player;
static map_cell_t ** map;
static game_obj_t objs[MAX_OBJECTS];
static int mW, mH, nO;
static fov_settings_type fov_settings;
static int keys[MAX_KEYS];
static int freeKey;
static char msg[128];
static bool newMsg = FALSE;

void input();
gsname_t update();
void render(int, int);
void drawGui(int, int);
void setPlayerStart();
void initObjects();
void initKeys();
void drawNeon(int, int, floor_t);
void apply(void *, int, int, int, int, void *);
bool opaque(void *, int, int);
void loadMap(const char *);
bool canMoveTo(int, int);

void initInGameState( gs_t * gs) {
	int i, j;

	uK = dK = lK = rK = esc = FALSE;

	gs->name = IN_GAME;
	gs->input = &input;
	gs->update = &update;
	gs->render = &render;

	map = ( map_cell_t ** ) malloc ( sizeof ( map_cell_t * ) * MAX_MAP_SIZE);
	for ( i = 0; i < MAX_MAP_SIZE; ++i ) {
		map[ i ] = ( map_cell_t * ) calloc ( MAX_MAP_SIZE , sizeof ( map_cell_t ) );
	}

	wmap = ( bool ** ) malloc ( sizeof ( bool * ) * MAX_MAP_SIZE);
	for ( i = 0; i < MAX_MAP_SIZE; ++i ) {
		wmap[ i ] = ( bool * ) calloc ( MAX_MAP_SIZE, sizeof ( bool ) );
		for(j = 0; j < MAX_MAP_SIZE; ++j){
			wmap[i][j] = rand() % 2;
		}
	}

	vis = ( bool ** ) malloc ( sizeof ( bool * ) * MAX_MAP_SIZE);
	seen = ( bool ** ) malloc ( sizeof ( bool * ) * MAX_MAP_SIZE);
	for ( i = 0; i < MAX_MAP_SIZE; ++i ) {
		vis[ i ] = ( bool * ) calloc ( MAX_MAP_SIZE, sizeof ( bool ) );
		seen[ i ] = ( bool * ) calloc ( MAX_MAP_SIZE, sizeof ( bool ) );
		for(j = 0; j < MAX_MAP_SIZE; ++j){
			vis[i][j] = TRUE;
			seen[i][j] = FALSE;
		}
	}

	initObjects();
	loadMap("maps/start.map");

	fov_settings_init(&fov_settings);
	fov_settings_set_opacity_test_function(&fov_settings, opaque);
	fov_settings_set_apply_lighting_function(&fov_settings, apply);
}

void input(){
	int key = 0;

	key = getch();

	if(key != ERR){
		if(key == KEY_UP) uK = TRUE;
		if(key == KEY_DOWN) dK = TRUE;
		if(key == KEY_LEFT) lK = TRUE;
		if(key == KEY_RIGHT) rK = TRUE;
		if(key == 27) esc = TRUE;
	}
}

gsname_t update(){
	clock_t msgNow, delta;
	int i, j, k, d, iX, iY, nX, nY;

	iX = player.x;
	iY = player.y;
	nX = iX;
	nY = iY;

	if(uK) nY = iY - 1 < 0 ? mH - 1 : iY - 1;

	if(dK) nY = (iY + 1) % mH;

	if(lK) nX = iX - 1 < 0 ? mW - 1 : iX - 1;

	if(rK) nX = (iX + 1) % mW;

	if(esc){
		/* Reset the game and go to the main menu. */
		esc = FALSE;
		initObjects();
		loadMap("maps/start.map");
		return MENU;
	}

	/* Find if the player is standing on an exit, then load the next map. */
	for(i = 0; i < nO; i++){
		if(objs[i].type == EXIT){
			if(objs[i].x == iY && objs[i].y == iX){
				player.x = objs[i].eX;
				player.y = objs[i].eY;
				if(strcmp(objs[i].target, "END") != 0){
					loadMap(objs[i].target);
					return IN_GAME;
				}else{
					initObjects();
					loadMap("maps/start.map");
					return GAME_OVER;
				}
			}
		}
	}

	/* If the player is standing on a key, pick it up. */
	for(i = 0; i < nO; i++){
		if(objs[i].type == KEY){
			if(objs[i].x == iY && objs[i].y == iX){
				keys[freeKey] = objs[i].id;
				objs[i].type = NONE;
				for(j = 0; keyMsg[j] && j < 128; j++){
					msg[j] = keyMsg[j];
				}
				newMsg = TRUE;
				msgThen = clock();
			}
		}
	}

	/* If the player bumps into a person, listen to what they have to say. */
	for(i = 0; i < nO; i++){
		if(objs[i].type == PERSON){
			if(objs[i].x == nY && objs[i].y == nX){
				for(k = 0; k < nO; k++)
					if(objs[k].type == DIALOG && objs[k].id == objs[i].dId) break;

				for(j = 0; objs[i].name[j] && j < 128; j++)
					msg[j] = objs[i].name[j];

				for(d = 0; saysMsg[d] && j < 128; j++, d++)
					msg[j] = saysMsg[d];

				for(d = 0; objs[k].dialog[d] && j < 128; j++, d++)
					msg[j] = objs[k].dialog[d];

				newMsg = TRUE;
				msgThen = clock();
			}
		}
	}

	/* If the player bumps into a door, open it if the key is available. */
	for(i = 0; i < nO; i++){
		if(objs[i].type == DOOR){
			if(objs[i].x == nY && objs[i].y == nX){
				if(!objs[i].unlocked){
					for(j = 0; j < MAX_KEYS; j++){
						if(keys[j] == objs[i].id){
							objs[i].unlocked = 1;
							break;
						}
					}

					if(objs[i].unlocked){
						for(j = 0; openDoor[j] && j < 128; j++){
							msg[j] = openDoor[j];
						}
					}else{
						for(j = 0; doorLock[j] && j < 128; j++){
							msg[j] = doorLock[j];
						}
					}

					newMsg = TRUE;
					msgThen = clock();
				}
			}
		}
	}

	/* Clear the message buffer after a timeout. */
	if(newMsg){
		msgNow = clock();
		delta = msgNow - msgThen;
		if((int)delta / (int)CLOCKS_PER_SEC >= 4){
			msgThen = msgNow;
			for(j = 0; j < 128; j++){
				msg[j] = '\0';
			}
			newMsg = FALSE;
		}
	}

	/* Move the player. */
	if(uK){
		iY = iY - 1 < 0 ? mH - 1 : iY - 1;
		if(canMoveTo(iY, iX)) player.y = iY;
		uK = FALSE;
	}

	if(dK){
		iY = (iY + 1) % mH;
		if(canMoveTo(iY, iX)) player.y = iY;
		dK = FALSE;
	}

	if(lK){
		iX = iX - 1 < 0 ? mW - 1 : iX - 1;
		if(canMoveTo(iY, iX)) player.x = iX;
		lK = FALSE;
	}

	if(rK){
		iX = (iX + 1) % mW;
		if(canMoveTo(iY, iX)) player.x = iX;
		rK = FALSE;
	}

	return IN_GAME;
}

void render(int w, int h){
	clock_t now, delta;
	int i, j, k, pi, pj, ioff, joff, di, dj;

	now = clock();
	delta = now - then;
	if((float)delta / (float)CLOCKS_PER_SEC >= 0.25f){
		then = now;
		w_mov = TRUE;
	}

	pi = (((w - 2) - 1) / 2) + 1;
	pj = (h - 3) / 2 + 1;

	ioff = (w - 28 - 27) / 2;
	joff = (h - 3) / 2;

	fov_circle(&fov_settings, &map, NULL, player.x, player.y, (MAX_MAP_SIZE / 2) - 1);

	for(i = 1; i < w - 1; i++){
		for(j = 1; j < h - 3; j++){
			move(j, i);

			di = i - 27 + player.x - ioff;
			dj = j - 1 + player.y - joff;

			if( di < 0 || di >= mW || dj < 0 || dj >= mH ){
				printw(" ");
			}else{
				if(vis[dj][di] || seen[dj][di]){
					switch(map[dj][di].f){
						case WATER:
							attron(COLOR_PAIR(DW_COLOR));
							if(w_mov)
								wmap[dj][di] = !wmap[dj][di];
							if(wmap[dj][di])
								printw("\u2248");
							else
								printw("~");
							break;

						case VOID:
							if(!vis[dj][di]) attron(COLOR_PAIR(DW_COLOR));
							else attron(COLOR_PAIR(MN_COLOR));
							printw(" ");
							break;

						case EMPTY_FLOOR:
							if(!vis[dj][di]) attron(COLOR_PAIR(DW_COLOR));
							else attron(COLOR_PAIR(MN_COLOR));
							printw(" ");
							break;

						case RUG:
							if(!vis[dj][di]) attron(COLOR_PAIR(DW_COLOR));
							else attron(COLOR_PAIR(SN_COLOR));
							printw("\u2592");
							break;

						case WINDOW_WALL:
							if(!vis[dj][di]) attron(COLOR_PAIR(DW_COLOR));
							else attron(COLOR_PAIR(SW_COLOR));
							printw("\u2591");
							break;

						case CLEAR_WALL:
							if(!vis[dj][di]) attron(COLOR_PAIR(DW_COLOR));
							else attron(COLOR_PAIR(SW_COLOR));
							printw("\u2588");
							break;

						case SECRET_WALL:
						case SOLID_WALL:
							if(!vis[dj][di]) attron(COLOR_PAIR(DW_COLOR));
							else attron(COLOR_PAIR(MN_COLOR));
							printw("\u2588");
							break;

						case NEON_WALL:
							drawNeon(dj, di, NEON_WALL);
							break;

						case BAR:
							drawNeon(dj, di, BAR);
							break;
					}

					move(j, i);
					for(k = 0; k < nO; k++){
						if(objs[k].x == dj && objs[k].y == di){
							if(!vis[dj][di]) attron(COLOR_PAIR(DW_COLOR));
							else attron(COLOR_PAIR(MN_COLOR));

							if(objs[k].type == DOOR){
								printw("\u25D9");
							}else{
								if(vis[dj][di]){
									if(objs[k].type == KEY){
										printw("k");
									}else if(objs[k].type == PERSON){
										printw("\u263A");
									}
								}
							}
						}
					}

				}else{
					attron(COLOR_PAIR(MN_COLOR));
					printw(" ");
				}
			}
		}
	}
	w_mov = FALSE;

	move(pj, pi);
	attron(COLOR_PAIR(BSC_COLOR));
	printw(/*"\u263A"*/ "@");

	drawGui(w, h);

	for ( i = 0; i < MAX_MAP_SIZE; ++i ) {
		for(j = 0; j < MAX_MAP_SIZE; ++j){
			vis[i][j] = FALSE;
		}
	}
}

void drawNeon(int i, int j, floor_t floor){
	int r;
	bool n, s, e, w;

	if(floor == NEON_WALL){
		if(!vis[i][j]) attron(COLOR_PAIR(DW_COLOR));
		else{
			r = rand() % 1000;
			if(r < 998)
				attron(COLOR_PAIR(FR_COLOR));
			else
				attron(COLOR_PAIR(MN_COLOR));
		}
	}else if(floor == BAR){
		if(!vis[i][j]) attron(COLOR_PAIR(DW_COLOR));
		else attron(COLOR_PAIR(SN_COLOR));
	}

	n = map[i						 ][j - 1 < 0 ? mH - 1 : j - 1].f == floor;
	s = map[i						 ][(j + 1) % mH			  ].f == floor;
	e = map[i - 1 < 0 ? mW - 1 : i - 1][j						 ].f == floor;
	w = map[(i + 1) % mW			  ][j						 ].f == floor;

	if((n && s && e) && (!w)){
		printw("\u2560");
		return;
	}

	if((n || s) && (!e && !w)){
		printw("\u2550");
		return;
	}

	if((e || w) && (!n && !s)){
		printw("\u2551");
		return;
	}

	if((e && n) && (!s && !w)){
		printw("\u255D");
		return;
	}

	if((w && n) && (!s && !e)){
		printw("\u2557");
		return;
	}

	if((e && s) && (!n && !w)){
		printw("\u255A");
		return;
	}

	if((w && s) && (!n && !e)){
		printw("\u2554");
		return;
	}

	if((s && e && w) && (!n)){
		printw("\u2560");
		return;
	}

	if((n && w && e) && (!s)){
		printw("\u2563");
		return;
	}

	if(n && s && e && w){
		printw("\u256C");
		return;
	}
}

void drawGui(int w, int h){
	int i;

	attron(COLOR_PAIR(BSC_COLOR));

	/* Clear the gui space. */
	for(i = 1; i < w - 1; i++){
		move(h - 2, i);
		printw(" "); 
	}

	/* Upper horizontal bar. */
	move(0, 0);
	printw("\u2554");
	for(i = 0; i < w - 2; i++){
		printw("\u2550");
	}
	printw("\u2557");

	/* Lower horizontal bars. */
	move(h - 3, 0);
	printw("\u255A");
	for(i = 0; i < w - 2; i++){
		printw("\u2550");
	}
	printw("\u255D");

	move(h - 1, 0);
	printw("\u255A");
	for(i = 0; i < w - 2; i++){
		printw("\u2550");
	}
	printw("\u255D");

	/* Vertical bars. */
	for(i = 1; i < h - 1; i++){
		move(i, 0);
		if(i != h - 3) printw("\u2551");
		else printw("\u2560");
		move(i, w-1);
		if(i != h - 3) printw("\u2551");
		else printw("\u2563");
	}

	move(h - 2, 1);
	for(i = 0; msg[i] && i < w - 2; i++){
		printw("%c", msg[i]);
	}
}

void setPlayerStart(){
	int i;

	for(i = 0; i < nO; i++){
		if(objs[i].type == PLAYER_START){
			player.y = objs[i].sX;
			player.x = objs[i].sY;
			break;
		}
	}
}

void initObjects(){
	int i, j;

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
		for(j = 0; j < MAX_STR; j++){
			objs[i].name[j] = '\0';
			objs[i].target[j] = '\0';
			objs[i].dialog[j] = '\0';
		}
		objs[i].unlocked = 0;
	}
}

void initKeys(){
	int i;

	freeKey = 0;
	for(i = 0; i < MAX_KEYS; ++i){
		keys[i] = -1;
	}
}

void apply(void *map, int x, int y, int dx, int dy, void *src){
	if(x < 0 || x >= MAX_MAP_SIZE) return;
	if(y < 0 || y >= MAX_MAP_SIZE) return;

	vis[y][x] = TRUE;
	seen[y][x] = TRUE;
}

bool opaque(void *m, int x, int y){
	int k;
	if(x < 0 || x >= MAX_MAP_SIZE) return FALSE;
	if(y < 0 || y >= MAX_MAP_SIZE) return FALSE;

	if(map[y][x].f == SOLID_WALL || map[y][x].f == SECRET_WALL){
		return TRUE;
	}else{
		for(k = 0; k < nO; k++){
			if(objs[k].type == DOOR){
				if(objs[k].x == y && objs[k].y == x){
					return TRUE;
				}
			}
		}

		return FALSE;
	}
}

void loadMap(const char * file){
	int i, j;

	errcode_t rc = readMapData(file, &map, &mW, &mH);
	if(rc != NO_ERROR){
		fprintf(stderr, "\t%s.loadMap(): readMapData() returned %d\n", __FILE__, rc);
		exit(rc);
	}

	game_obj_t * objsP = objs;
	rc = readMapObjects(file, &objsP, &nO);
	if(rc != NO_ERROR){
		fprintf(stderr, "\t%s.loadMap(): readMapObjects() returned %d\n", __FILE__, rc);
		exit(rc);
	}

	setPlayerStart();
	initKeys();

	for ( i = 0; i < MAX_MAP_SIZE; ++i ) {
		for(j = 0; j < MAX_MAP_SIZE; ++j){
			vis[i][j] = FALSE;
			seen[i][j] = FALSE;
		}
	}
}

bool canMoveTo(int iY, int iX){
	int k;

	for(k = 0; k < nO; k++){
		if(objs[k].x == iY && objs[k].y == iX){
			if(objs[k].type == DOOR){
				if(objs[k].unlocked) return TRUE;
				else return FALSE;
			}else if(objs[k].type == PERSON){
				return FALSE;
			}
		}
	}

	if((map[iY][iX].f > WINDOW_WALL && map[iY][iX].f <= WATER) || map[iY][iX].f == SECRET_WALL)
		return TRUE;
	else
		return FALSE;
}
