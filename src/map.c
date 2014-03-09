/**
 * Copyright (c) 2014, Miguel Angel Astor Romero. All rights reserved.
 * See the file LICENSE for more details.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"

errcode_t readMapData(const char * file, map_cell_t *** map, int * w, int * h){
	size_t n = 2048;
	char *buffer;
	FILE * f;

	f = fopen(file, "r");
	if(f == NULL) return FILE_NOT_FOUND;

	buffer = (char*)calloc(n + 1, sizeof(char));
	if(buffer == NULL) return OUT_OF_MEMORY;

	while(getline(&buffer, &n, f) != -1){
		if(strcmp(buffer, "[MAP]\n") == 0){
			int rc, i, j;
			char *end;

			rc = getline(&buffer, &n, f);
			if(rc == -1){
				free(buffer);
				return PREMATURE_EOF;
			}

			*w = strtol(buffer, &end, 10);
			*h = strtol(end, NULL, 10);

			if((*w <= 0 || *w > MAX_MAP_SIZE) || (*h <= 0 || *h > MAX_MAP_SIZE)){
				*w = -1;
				*h = -1;
				free(buffer);
				return MAP_TOO_LARGE;
			}

			for(i = 0; i < *w; i++){
				rc = getline(&buffer, &n, f);
				if(rc == -1){
					free(buffer);
					return PREMATURE_EOF;
				}

				/* Skip commentaries. */
				if(buffer[0] == '%'){
					i--;
					continue;
				}

				for(j = 0; buffer[j] && j < *h; j++){
					if(buffer[j] >= '0' && buffer[j] <= '9'){
						switch(buffer[j]){
							case '0': (*map)[i][j].f = VOID;		break;
							case '1': (*map)[i][j].f = SOLID_WALL;  break;
							case '2': (*map)[i][j].f = SECRET_WALL; break;
							case '3': (*map)[i][j].f = CLEAR_WALL;  break;
							case '4': (*map)[i][j].f = NEON_WALL;   break;
							case '5': (*map)[i][j].f = WINDOW_WALL; break;
							case '6': (*map)[i][j].f = EMPTY_FLOOR; break;
							case '7': (*map)[i][j].f = RUG;		 break;
							case '8': (*map)[i][j].f = WATER;	   break;
							case '9': (*map)[i][j].f = BAR;		 break;
							default:
								fprintf(stderr, "\t%s.readMapData() : Invalid character %c in map file %s\n", __FILE__, buffer[j], file);
								(*map)[i][j].f = VOID;
								break;
						}
					}else{
						fprintf(stderr, "\t%s.readMapData() : Invalid character %c in map file %s\n", __FILE__, buffer[j], file);
						(*map)[i][j].f = VOID;
					}
				}
			}
			/* Skip the rest of the file. */
			break;
		}
	}

	fclose(f);
	free(buffer);

	return NO_ERROR;
}

errcode_t readMapObjects(const char * file, game_obj_t ** objArr, int *nObj){
	size_t n = 2048;
	int rc;
	char *buffer;
	FILE * f;

	*nObj = 0;

	f = fopen(file, "r");
	if(f == NULL) return FILE_NOT_FOUND;

	buffer = (char*)calloc(n + 1, sizeof(char));
	if(buffer == NULL) return OUT_OF_MEMORY;

	rc = getline(&buffer, &n, f);
	if(rc == -1){
		free(buffer);
		return PREMATURE_EOF;
	}

	while(*nObj < MAX_OBJECTS && rc != -1){
		if(buffer[0] == '%') continue;

		if(strcmp(buffer, "[MAP]\n") == 0){
			int i, w;

			rc = getline(&buffer, &n, f);
			if(rc == -1){
				free(buffer);
				return PREMATURE_EOF;
			}

			w = strtol(buffer, NULL, 10);

			for(i = 0; i <= w; ++i){
				rc = getline(&buffer, &n, f);
				if(rc == -1){
					free(buffer);
					return PREMATURE_EOF;
				}
			}

		}else if(strcmp(buffer, "[PLAYER]\n") == 0){
			do{
				rc = getline(&buffer, &n, f);
				if(rc == -1) break;

				if(buffer[0] == '%') continue;
				else if(buffer[0] == 'p'){
					int i;

					for(i = 0; buffer[i] && buffer[i] != '='; ++i);

					if(strncmp(buffer, "player =", i) == 0){
						int sX, sY;
						char *end;

						sX = strtol(&buffer[i + 1], &end, 10);
						sY = strtol(end, NULL, 10);

						(*objArr)[*nObj].type = PLAYER_START;
						(*objArr)[*nObj].sX = sX;
						(*objArr)[*nObj].sY = sY;

						*nObj += 1;

					}else{
						fprintf(stderr, "\t%s.readMapObjects(): Skipped invalid data in PLAYER key in map file %s.\n", __FILE__, file);
						fprintf(stderr, "\t%s.readMapObjects(): The invalid line is %s.\n", __FILE__, buffer);
						continue;
					}
				}
			}while(buffer[0] != '[');

		}else if(strcmp(buffer, "[EXITS]\n") == 0){
			do{
				rc = getline(&buffer, &n, f);
				if(rc == -1) break;

				if(buffer[0] == '%') continue;
				else if(buffer[0] == 'e'){
					int i;

					for(i = 0; buffer[i] && buffer[i] != '='; ++i);

					if(strncmp(buffer, "exit =", i) == 0){
						int x, y, eX, eY;
						char *end, *end2, *end3;

						x = strtol(&buffer[i + 1], &end, 10);
						y = strtol(end, &end2, 10);
						end2++;

						(*objArr)[*nObj].type = EXIT;
						(*objArr)[*nObj].x = x;
						(*objArr)[*nObj].y = y;

						for(i = 0; end2[i] && end2[i] != ' '; ++i);

						strncpy((*objArr)[*nObj].target, end2, i);
						(*objArr)[*nObj].target[i] = '\0';

						eX = strtol(&end2[i], &end3, 10);
						eY = strtol(end3, NULL, 10);

						(*objArr)[*nObj].eX = eX;
						(*objArr)[*nObj].eY = eY;

						*nObj += 1;

					}else{
						fprintf(stderr, "\t%s.readMapObjects(): Skipped invalid data in EXITS key in map file %s.\n", __FILE__, file);
						fprintf(stderr, "\t%s.readMapObjects(): The invalid line is %s.\n", __FILE__, buffer);
						continue;
					}
				}
			}while(buffer[0] != '[');

		}else if(strcmp(buffer, "[DOORS]\n") == 0){
			do{
				rc = getline(&buffer, &n, f);
				if(rc == -1) break;

				if(buffer[0] == '%') continue;
				else if(buffer[0] == 'd'){
					int i;

					for(i = 0; buffer[i] && buffer[i] != '='; ++i);

					if(strncmp(buffer, "door =", i) == 0){
						int dX, dY, dId, un;
						char *end, *end2, *end3;

						dX = strtol(&buffer[i + 1], &end, 10);
						dY = strtol(end, &end2, 10);
						dId = strtol(end2, &end3, 10);
						un = strtol(end3, NULL, 10);

						(*objArr)[*nObj].type = DOOR;
						(*objArr)[*nObj].x = dX;
						(*objArr)[*nObj].y = dY;
						(*objArr)[*nObj].id = dId;
						(*objArr)[*nObj].unlocked = un;

						*nObj += 1;

					}else{
						fprintf(stderr, "\t%s.readMapObjects(): Skipped invalid data in DOORS key in map file %s.\n", __FILE__, file);
						fprintf(stderr, "\t%s.readMapObjects(): The invalid line is %s.\n", __FILE__, buffer);
						continue;
					}
				}
			}while(buffer[0] != '[');

		}else if(strcmp(buffer, "[KEYS]\n") == 0){
			do{
				rc = getline(&buffer, &n, f);
				if(rc == -1) break;

				if(buffer[0] == '%') continue;
				else if(buffer[0] == 'k'){
					int i;

					for(i = 0; buffer[i] && buffer[i] != '='; ++i);

					if(strncmp(buffer, "key =", i) == 0){
						int kX, kY, kId;
						char *end, *end2;

						kX = strtol(&buffer[i + 1], &end, 10);
						kY = strtol(end, &end2, 10);
						kId = strtol(end2, NULL, 10);

						(*objArr)[*nObj].type = KEY;
						(*objArr)[*nObj].x = kX;
						(*objArr)[*nObj].y = kY;
						(*objArr)[*nObj].id = kId;

						*nObj += 1;

					}else{
						fprintf(stderr, "\t%s.readMapObjects(): Skipped invalid data in KEYS key in map file %s.\n", __FILE__, file);
						fprintf(stderr, "\t%s.readMapObjects(): The invalid line is %s.\n", __FILE__, buffer);
						continue;
					}
				}
			}while(buffer[0] != '[');

		}else if(strcmp(buffer, "[PERSONS]\n") == 0){
			do{
				rc = getline(&buffer, &n, f);
				if(rc == -1) break;

				if(buffer[0] == '%') continue;
				else if(buffer[0] == 'p'){
					int i;

					for(i = 0; buffer[i] && buffer[i] != '='; ++i);

					if(strncmp(buffer, "person =", i) == 0){
						int pX, pY, pDId;
						char *end, *end2;

						pX = strtol(&buffer[i + 1], &end, 10);
						pY = strtol(end, &end2, 10);
						end2++;

						(*objArr)[*nObj].type = PERSON;
						(*objArr)[*nObj].x = pX;
						(*objArr)[*nObj].y = pY;

						for(i = 0; end2[i] && end2[i] != ' '; ++i);

						strncpy((*objArr)[*nObj].name, end2, i);
						(*objArr)[*nObj].name[i] = '\0';

						pDId = strtol(&end2[i], NULL, 10);
						(*objArr)[*nObj].dId = pDId;

						*nObj += 1;

					}else{
						fprintf(stderr, "\t%s.readMapObjects(): Skipped invalid data in PERSONS key in map file %s.\n", __FILE__, file);
						fprintf(stderr, "\t%s.readMapObjects(): The invalid line is %s.\n", __FILE__, buffer);
						continue;
					}
				}
			}while(buffer[0] != '[');

		}else if(strcmp(buffer, "[DIALOGS]\n") == 0){
			do{
				rc = getline(&buffer, &n, f);
				if(rc == -1) break;

				if(buffer[0] == '%') continue;
				else if(buffer[0] == 'd'){
					int i;

					for(i = 0; buffer[i] && buffer[i] != '='; ++i);

					if(strncmp(buffer, "dialog =", i) == 0){
						int dId;
						char *end;

						dId = strtol(&buffer[i + 1], &end, 10);
						end++;

						(*objArr)[*nObj].type = DIALOG;
						(*objArr)[*nObj].id = dId;

						strcpy((*objArr)[*nObj].dialog, end);
						for(i = 0; (*objArr)[*nObj].dialog[i] && (*objArr)[*nObj].dialog[i] != '\n'; ++i);
						(*objArr)[*nObj].dialog[i] = '\0';

						*nObj += 1;

					}else{
						fprintf(stderr, "\t%s.readMapObjects(): Skipped invalid data in DIALOGS key in map file %s.\n", __FILE__, file);
						fprintf(stderr, "\t%s.readMapObjects(): The invalid line is %s.\n", __FILE__, buffer);
						continue;
					}
				}
			}while(buffer[0] != '[');

		}else{
			fprintf(stderr, "\t%s.readMapObjects(): Found invalid key in map file %s. Key is %s\n", __FILE__, file, buffer);
			free(buffer);
			return INVALID_KEY;
		}
	}

	fclose(f);
	free(buffer);

	return NO_ERROR;
}
