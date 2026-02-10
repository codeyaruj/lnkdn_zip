/*
 * solver.h - Puzzle Solver
 * 
 * DFS-based solver to verify puzzle solvability.
 * 
 * The solver determines if a valid solution path exists that:
 * - Starts at number 1
 * - Visits all numbers in increasing order (1, 2, 3, ..., N)
 * - Follows movement constraints (orthogonal, no revisits, no walls)
 * 
 * This is an engine-only component:
 * - Read-only access to Board
 * - No UI dependencies
 * - No Board mutation
 * 
 * Primary use cases:
 * - Validate generated puzzles
 * - Debug puzzle design
 * - Verify puzzle quality
 */

#ifndef SOLVER_H
#define SOLVER_H

#include "engine.h"

/*
 * Check if a puzzle has at least one valid solution
 * 
 * A solution is a path that:
 * - Starts at the cell containing number 1
 * - Visits all numbers in order (1 → 2 → 3 → ... → N)
 * - Uses only orthogonal moves (up, down, left, right)
 * - Never revisits a cell
 * - Never crosses walls
 * 
 * Parameters:
 *   board - The puzzle to solve (read-only)
 * 
 * Returns:
 *   true  - At least one valid solution exists
 *   false - No solution exists (unsolvable puzzle)
 * 
 * Complexity:
 *   Time: O(4^N) worst case, but heavily pruned in practice
 *   Space: O(rows * cols) for visited tracking
 * 
 * Notes:
 *   - Does not modify the board
 *   - Does not use board->player state
 *   - Ignores CELL_PATH (treats as CELL_EMPTY)
 *   - Returns false if number 1 not found
 */
bool puzzle_has_solution(const Board *board);

#endif /* SOLVER_H */
