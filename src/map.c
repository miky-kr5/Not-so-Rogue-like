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
    char *buffer;
    FILE * f;
    size_t n = 2048;

    f = fopen(file, "r");
    if(f == NULL) return FILE_NOT_FOUND;

    buffer = (char*)calloc(n + 1, sizeof(char));
    if(buffer == NULL) return OUT_OF_MEMORY;

    while(getline(&buffer, &n, f) != -1){
        if(strcmp(buffer, "[MAP]\n") == 0){
            int rc, i, j;
            char *end;

            fprintf(stderr, "\t%s.readMapData() : found a map.\n", __FILE__);

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
                            case '0': (*map)[i][j].f = VOID;        break;
                            case '1': (*map)[i][j].f = SOLID_WALL;  break;
                            case '2': (*map)[i][j].f = SECRET_WALL; break;
                            case '3': (*map)[i][j].f = CLEAR_WALL;  break;
                            case '4': (*map)[i][j].f = NEON_WALL;   break;
                            case '5': (*map)[i][j].f = WINDOW_WALL; break;
                            case '6': (*map)[i][j].f = EMPTY_FLOOR; break;
                            case '7': (*map)[i][j].f = RUG;         break;
                            case '8': (*map)[i][j].f = WATER;       break;
                            case '9': (*map)[i][j].f = BAR;         break;
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
