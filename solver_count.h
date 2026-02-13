/*
 * solver_count.h - Solution Counting Solver
 * 
 * Extends the basic existence checker to count exact number of solutions.
 * Critical for generating high-quality puzzles with unique solutions.
 * 
 * Usage:
 *   int count = puzzle_count_solutions(board, 2);
 *   if (count == 0) -> Unsolvable (generator bug)
 *   if (count == 1) -> Unique solution (perfect!)
 *   if (count >= 2) -> Ambiguous (reject)
 */

#ifndef SOLVER_COUNT_H
#define SOLVER_COUNT_H

#include "engine.h"

/*
 * Count number of distinct solutions
 * 
 * Parameters:
 *   board         - Puzzle to analyze (immutable)
 *   max_solutions - Stop searching after finding this many
 *                   (Use 2 for uniqueness checking)
 * 
 * Returns:
 *   0   - No solution exists (broken puzzle)
 *   1   - Exactly one solution (high quality)
 *   >=2 - Multiple solutions found (ambiguous)
 * 
 * Performance:
 *   Early exit when max_solutions reached
 *   Typical uniqueness check (max=2): <5ms for 10x10
 * 
 * Why max_solutions parameter?
 *   We don't need to find ALL solutions, just enough to know
 *   if puzzle is unique. Stopping at 2 makes uniqueness checking fast.
 */
int puzzle_count_solutions(const Board *board, int max_solutions);

#endif 
