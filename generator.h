/*
 * generator.h - Procedural Puzzle Generator
 * 
 * Generates random solvable Zip puzzles using path-first generation.
 * 
 * Strategy:
 * 1. Create empty board with borders
 * 2. Generate random non-self-intersecting path using DFS
 * 3. Place numbers sequentially along path
 * 4. Add random walls to non-path cells
 * 5. Initialize player at start position
 * 
 * This ensures every generated puzzle is solvable by construction.
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include "engine.h"

/*
 * Generate a random solvable puzzle
 * 
 * Parameters:
 *   rows       - Board height (minimum 5 for playable puzzles)
 *   cols       - Board width (minimum 5 for playable puzzles)
 *   path_ratio - Fraction of board to fill with path (0.0 to 1.0)
 *                Typical: 0.3 to 0.6
 *   wall_ratio - Fraction of empty cells to convert to walls (0.0 to 1.0)
 *                Typical: 0.1 to 0.3
 *   seed       - Random seed for deterministic generation
 * 
 * Returns:
 *   Pointer to generated board, or NULL on failure
 * 
 * Notes:
 *   - Board has borders (walls on all edges)
 *   - Path guaranteed non-self-intersecting
 *   - Numbers placed sequentially (1..N) along path
 *   - Walls only placed on non-path cells
 *   - Player initialized at number 1
 */
Board *generate_puzzle(
    int rows,
    int cols,
    float path_ratio,
    float wall_ratio,
    unsigned int seed
);

#endif /* GENERATOR_H */

