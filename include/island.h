/**
 * Copyright (c) 2014, Miguel Angel Astor Romero. All rights reserved.
 * See the file LICENSE for more details.
 */

#ifndef ISLAND_H
#define ISLAND_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum TERRAIN_TYPE { DEEP_WATER,
                            SHALLOW_WATER,
                            SAND,
                            GRASS,
                            FOREST,
                            HILL,
                            MOUNTAIN
                          } terrain_t;

  /**
   * Generate a diamond-square fractal map.
   */
  extern void ds ( float ***, const unsigned int );

  /**
   * Generate a mask using particle deposition.
   */
  extern void island ( int ***, unsigned int );

  /**
   * Normalize a float matrix between 0 and 1.
   */
  extern void norm ( float ***, unsigned int );

  /**
   * Normalize an int matrix between 0 and 255.
   */
  extern void normInt ( int ***, unsigned int );

  /**
   * Perform a 3x3 average blur.
   */
  extern void smooth ( int ***, unsigned int );

  /**
   * Multiply the diamond square map with the island mask.
   * Both matrices must have been normalized before.
   */
  extern void mult ( float ***, int ***, unsigned int );

  /**
   * Given a sample from a heightmap, return the terrain
   * type that correspond it.
   */
  terrain_t terrainType( int );

#ifdef __cplusplus
}
#endif

#endif
