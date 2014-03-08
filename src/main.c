/**
 * Copyright (c) 2014, Miguel Angel Astor Romero. All rights reserved.
 * See the file LICENSE for more details.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <ncursesw/ncurses.h>
#include <locale.h>
#include <sys/stat.h>

#include "constants.h"
#include "game_state.h"

void leave(void);
void manage_signal(int signal);
int start_ncurses(void);
void set_colors(void);
void clear_screen(void);
void on_resize(int);

static int w = 0, h = 0;
static bool resize = FALSE;

int main() {
	bool finished = FALSE;
	char         *    home_dir = getenv("HOME");
	FILE         *    f; /* To avoid a warning. */
	clock_t           then, now, delta;
	unsigned int      fps = 0, pfps = 0;
	char         *    data_dir;
	char         *    log_file;
	time_t            raw_date;
	struct tm    *    current_date;
	gs_t         *    states;
	int               c_state;

	atexit(leave);
	signal(SIGINT, manage_signal);
	signal(SIGSEGV, manage_signal);
	signal(SIGTERM, manage_signal);
	signal(SIGWINCH, on_resize);

	/* If there is a HOME environment variable, enable scoring. */
	if(home_dir != NULL){
		/* If we got the user's home directory, build the data directory path. */
		data_dir = (char*)malloc(strlen(home_dir) + 7);
		strcpy(data_dir, home_dir);
		strcat(data_dir, F_SEP ".cyjam");

		/* Redirect stderr output to a file. */
		log_file = (char*)malloc(strlen(data_dir) + 8);
		strcpy(log_file, data_dir);
		strcat(log_file, F_SEP "stderr");
		f = freopen(log_file, "a", stderr);

		/* Log the current date and time. */
		time(&raw_date);
		current_date = localtime(&raw_date);
		fprintf(stderr, "%s", asctime(current_date));

		/* Try to create the data directory with permissions 775. */
		if(mkdir(data_dir, S_IRWXU | S_IWGRP | S_IRGRP| S_IROTH | S_IXOTH) != 0){
			if(errno != EEXIST){
				/* The directory does not exists and could not be created. */
				perror("\t" __FILE__);
				fprintf(stderr, "\tdata_dir is: %s\n", data_dir);
			}
		}
	}else{
		/* If there is no HOME environment variable, quit. */
		fprintf(stderr, "\t%s: Couldn't find the user's home directory\n", __FILE__);
		return EXIT_FAILURE;
	}

	/* Start ncurses. */
	if(start_ncurses() != 0){
		fprintf(stderr, "\t%s: Ncurses could not be initialized.\n", __FILE__);
		return EXIT_FAILURE;
	}
	set_colors();

	/* Create the state data structures. */
	c_state = 2;
	states = (gs_t *)malloc(sizeof(gs_t) * NUM_STATES);
	initStateArray(&states);

	/* Start the game loop. */
	then = clock();
	do{
		/* Handle terminal resize. */
		if(resize){
			endwin();
			refresh();

			getmaxyx(stdscr, h, w);

			fprintf(stderr, "\tSIGWINCH caught. (W: %d, H: %d)\n", w, h);

			resize = FALSE;
			signal(SIGWINCH, on_resize);
		}

		states[c_state].input();
		c_state = states[c_state].update();

		if(c_state == -1) finished = TRUE;

		states[c_state].render(w, h);

		fps++;

		now = clock();
		delta = now - then;
		if((int)delta / (int)CLOCKS_PER_SEC == 1){
			then = now;
			pfps = fps;
			fps = 0;
		}

		move(1, 1);
		attron(COLOR_PAIR(BSC_COLOR));
		printw("FPS: %u", pfps);

		refresh();
	}while(!finished);

	fclose(f);

	return EXIT_SUCCESS;
}


void leave(void){
	int i;

	/* Finish ncurses. */
	endwin();

	/* Mark the end of this run's log. */
	for(i = 0; i < 80; i++)
		fprintf(stderr, "-");
	fprintf(stderr, "\n");
}

void manage_signal(int signal){

	switch(signal){
	case SIGINT:
		fprintf(stderr, "\tSIGINT caught.\n");
		exit(EXIT_SUCCESS);
		break;

	case SIGSEGV:
		fprintf(stderr, "\tSegmentation fault.\n");
		exit(EXIT_FAILURE);
		break;

	case SIGTERM:
		fprintf(stderr, "\tSIGTERM caught.\n");
		exit(EXIT_FAILURE);
		break;
	}
}

void on_resize(int signal){
	resize = TRUE;
}

int start_ncurses(void){
	WINDOW *win_ptr;
	int ret_code;

	setlocale(LC_ALL, "");

	/* Prepare the terminal. */
	win_ptr = initscr();
	if(win_ptr == NULL)
		return -1;

	/* Enable special characters. */
	ret_code = keypad(stdscr, TRUE);
	if(ret_code == ERR)
		return -1;

	/* Disable line buffering. */
	ret_code = cbreak();
	if(ret_code == ERR)
		return -1;

	/* Disable echo. */
	ret_code = noecho();
	if(ret_code == ERR)
		return -1;

	/* Hide the cursor. */
	ret_code = curs_set(FALSE);
	if(ret_code == ERR)
		return -1;

	/* Set delay. */
	ret_code = nodelay(stdscr, TRUE);
	if(ret_code == ERR)
		return -1;

	/* Initialize the screen size variables. */
	getmaxyx(stdscr, h, w);

	return EXIT_SUCCESS;
}

void set_colors(void){
	int ret_code;
	ret_code = start_color();
	if(ret_code == OK){
		if(has_colors() == TRUE){
			init_color(COLOR_MAGENTA, 0, 0, 500);

			init_pair(BAR_COLOR, COLOR_WHITE, COLOR_RED);	 /* The color for the top and bottom bars. */
			init_pair(BSC_COLOR, COLOR_WHITE, COLOR_BLACK);   /* Basic text color. */
			init_pair(HLT_COLOR, COLOR_YELLOW, COLOR_BLACK);  /* Highlighted text color. */
			init_pair(OFF_COLOR, COLOR_BLUE, COLOR_BLACK);	/* Lights off color. */
			init_pair(DIM_COLOR, COLOR_RED, COLOR_BLACK);	 /* Dim light color. */
			init_pair(LIT_COLOR, COLOR_YELLOW, COLOR_BLACK);  /* Lights on color. */
			init_pair(GUI_COLOR, COLOR_YELLOW, COLOR_YELLOW); /* Main GUI bar color. */
			init_pair(EMP_COLOR, COLOR_WHITE, COLOR_WHITE);   /* Empty GUI bar color. */

			init_pair(DW_COLOR, COLOR_BLUE, COLOR_BLACK);
			init_pair(SW_COLOR, COLOR_CYAN, COLOR_BLACK);
			init_pair(SN_COLOR, COLOR_YELLOW, COLOR_BLACK);
			init_pair(GR_COLOR, COLOR_GREEN, COLOR_BLACK);
			init_pair(FR_COLOR, COLOR_RED, COLOR_BLACK);
			init_pair(HL_COLOR, COLOR_WHITE, COLOR_BLACK);
			init_pair(MN_COLOR, COLOR_WHITE, COLOR_BLACK);
		}
	}else{
		fprintf(stderr, "\t%s: Colors not supported.\n", __FILE__);
		exit(EXIT_FAILURE);
	}
}

void clear_screen(void){
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
