/*
 * generator.h - Procedural Puzzle Generator
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include "engine.h"

Board *generate_puzzle(int rows, int cols, float path_ratio, float wall_ratio, unsigned int seed);

#endif
