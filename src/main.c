/**
 * Copyright (c) 2014, Miguel Angel Astor Romero. All rights reserved.
 * See the file LICENSE for more details.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <ncurses.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "constants.h"

void leave(void);
void manage_signal(int signal);
int start_ncurses(void);
void set_colors(void);
void clear_screen(void);
void on_resize(int);

static int w = 0, h = 0;

int main() {
	bool finished = false;
	char *home_dir = getenv("HOME");
	char *data_dir;
	char *log_file;
	time_t raw_date;
	struct tm *current_date;

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
		strcat(data_dir, "/.tomb");

		/* Redirect stderr output to a file. */
		log_file = (char*)malloc(strlen(data_dir) + 8);
		strcpy(log_file, data_dir);
		strcat(log_file, "/stderr");
		freopen(log_file, "a", stderr);

		/* Log the current date and time. */
		time(&raw_date);
		current_date = localtime(&raw_date);
		fprintf(stderr, "%s", asctime(current_date));

		/* Try to create the data directory with permissions 775. */
		if(mkdir(data_dir, S_IRWXU | S_IWGRP | S_IRGRP| S_IROTH | S_IXOTH) == 0){
			/* The data directory was sucessfully created. */
			//init_scores(data_dir);
		}else{
			if(errno != EEXIST){
				/* The directory does not exists and could not be created. */
				perror("\tmain.c");
				fprintf(stderr, "\tdata_dir is: %s\n", data_dir);
				//init_scores(NULL);
			}else{
				/* The directory already exits. */
				//init_scores(data_dir);
			}
		}
	}else{
		/* If there is no HOME environment variable, quit. */
		fprintf(stderr, "\tmain.c: Couldn't find the user's home directory\n");
		return EXIT_FAILURE;
	}

	/* Start ncurses. */
	if(start_ncurses() != 0){
		fprintf(stderr, "\tmain.c: Ncurses could not be initialized.\n");
		return EXIT_FAILURE;
	}
	set_colors();

	do{
	    clear_screen();

	    refresh();
	  }while(!finished);

	return EXIT_SUCCESS;
}


void leave(void){
	int i;

	/* Finish ncurses. */
	endwin();
	/* Close the scores database and todays log. */
	// close_scores();
	for(i = 0; i < 80; i++)
		fprintf(stderr, "-");
	fprintf(stderr, "\n");
}

void manage_signal(int signal){

	switch(signal){
	case SIGINT:
		fprintf(stderr, "\tSIGINT caught.\n");
		break;
	case SIGSEGV:
		fprintf(stderr, "\tSegmentation fault.\n");
	case SIGTERM:
		exit(EXIT_FAILURE);
		break;
	}
}

void on_resize(int signal){
	struct winsize ws;

	/* Request the new size of the terminal. */
	ioctl(1, TIOCGWINSZ, &ws);
	/* Resize ncurse's stdscr. */
	resizeterm(ws.ws_row, ws.ws_col);
	/* Get the new size of the window. */
	getmaxyx(stdscr, h, w);
	fprintf(stderr, "\tSIGWINCH caught. (W: %d, H: %d)\n", w, h);
}

int start_ncurses(void){
	WINDOW *win_ptr;
	int ret_code;
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
	/* Initialize the screen size variables. */
	getmaxyx(stdscr, h, w);

	return 0;
}

void set_colors(void){
	int ret_code;
	ret_code = start_color();
	if(ret_code == OK){
		if(has_colors() == TRUE){
			init_pair(1, COLOR_WHITE, COLOR_RED);     /* The color for the top and bottom bars. */
			init_pair(2, COLOR_WHITE, COLOR_BLACK);   /* Basic text color. */
			init_pair(3, COLOR_YELLOW, COLOR_BLACK);  /* Highlighted text color. */
			init_pair(4, COLOR_BLUE, COLOR_BLACK);    /* Lights off color. */
			init_pair(5, COLOR_RED, COLOR_BLACK);     /* Dim light color. */
			init_pair(6, COLOR_YELLOW, COLOR_BLACK);  /* Lights on color. */
			init_pair(7, COLOR_YELLOW, COLOR_YELLOW); /* Main GUI bar color. */
			init_pair(8, COLOR_WHITE, COLOR_WHITE);   /* Empty GUI bar color. */
		}
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
