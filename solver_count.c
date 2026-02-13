/*
 * solver_count.c - Solution Counting Solver Implementation
 * 
 * DFS with early exit optimization for efficient uniqueness checking.
 */

#include "solver_count.h"
#include <stdlib.h>

/* ============================================================================
 * VISITED GRID MANAGEMENT (Same as existence solver)
 * ============================================================================ */

static bool **create_visited_grid(int rows, int cols) {
    bool **visited = (bool **)malloc(rows * sizeof(bool *));
    if (!visited) return NULL;
    
    for (int i = 0; i < rows; i++) {
        visited[i] = (bool *)calloc(cols, sizeof(bool));
        if (!visited[i]) {
            for (int j = 0; j < i; j++) {
                free(visited[j]);
            }
            free(visited);
            return NULL;
        }
    }
    
    return visited;
}

static void free_visited_grid(bool **visited, int rows) {
    if (!visited) return;
    for (int i = 0; i < rows; i++) {
        free(visited[i]);
    }
    free(visited);
}

/* ============================================================================
 * MOVEMENT VALIDATION (Same as existence solver)
 * ============================================================================ */

static bool is_in_bounds(const Board *board, int row, int col) {
    return row >= 0 && row < board->height && col >= 0 && col < board->width;
}

static bool is_valid_move(const Board *board, bool **visited, 
                         int row, int col, int next_number) {
    /* Bounds check */
    if (!is_in_bounds(board, row, col)) {
        return false;
    }
    
    /* Wall check */
    const Cell *cell = &board->grid[row][col];
    if (cell->type == CELL_WALL) {
        return false;
    }
    
    /* Visited check */
    if (visited[row][col]) {
        return false;
    }
    
    /* Number ordering check */
    if (cell->type == CELL_NUMBER) {
        if (cell->number != next_number) {
            return false;
        }
    }
    
    return true;
}

/* ============================================================================
 * DFS SOLUTION COUNTING CORE
 * ============================================================================ */

/*
 * Recursive DFS that counts all valid solution paths
 */
static void dfs_count(
    const Board *board,
    bool **visited,
    int row,
    int col,
    int next_number,
    int *solution_count,
    int max_solutions
) {
    /* EARLY EXIT OPTIMIZATION
     * 
     * If we've already found enough solutions, stop searching.
     * This is CRITICAL for performance:
     * - For uniqueness (max=2): stops as soon as 2nd solution found
     * - Prevents exhaustive search when we only need to know ">=2"
     */
    if (*solution_count >= max_solutions) {
        return;
    }
    
    /* BASE CASE: Found a complete valid path
     * 
     * Unlike existence solver which returns true here,
     * we INCREMENT the counter and CONTINUE via backtracking
     * to find other possible solutions.
     */
    if (next_number > board->max_number) {
        (*solution_count)++;
        return;  /* Backtrack to explore other paths */
    }
    
    /* Direction vectors: up, down, left, right */
    const int dr[] = {-1, 1, 0, 0};
    const int dc[] = {0, 0, -1, 1};
    
    /* Try all four directions */
    for (int dir = 0; dir < 4; dir++) {
        int new_row = row + dr[dir];
        int new_col = col + dc[dir];
        
        /* Validate move */
        if (!is_valid_move(board, visited, new_row, new_col, next_number)) {
            continue;
        }
        
        /* Determine next number to find */
        const Cell *target_cell = &board->grid[new_row][new_col];
        int next_next_number = next_number;
        
        if (target_cell->type == CELL_NUMBER) {
            next_next_number++;
        }
        
        /* Mark as visited (explore this branch) */
        visited[new_row][new_col] = true;
        
        /* Recursively count solutions from new position */
        dfs_count(board, visited, new_row, new_col, 
                 next_next_number, solution_count, max_solutions);
        
        /* BACKTRACK: Unmark cell to explore other paths
         * 
         * This is KEY to counting multiple solutions:
         * By unmarking, we allow other DFS branches to use this cell
         * in different solution paths.
         */
        visited[new_row][new_col] = false;
        
        /* Check if we should stop early (optimization) */
        if (*solution_count >= max_solutions) {
            return;
        }
    }
}

/* ============================================================================
 * PUBLIC API
 * ============================================================================ */

int puzzle_count_solutions(const Board *board, int max_solutions) {
    /* Validate input */
    if (!board || !board->grid || max_solutions <= 0) {
        return 0;
    }
    
    /* Find starting position (cell with number 1) */
    int start_row, start_col;
    if (!board_find_number(board, 1, &start_row, &start_col)) {
        /* No number 1 found - invalid puzzle */
        return 0;
    }
    
    /* Allocate visited tracking grid */
    bool **visited = create_visited_grid(board->height, board->width);
    if (!visited) {
        return 0;
    }
    
    /* Mark starting position as visited */
    visited[start_row][start_col] = true;
    
    /* Count solutions via DFS */
    int solution_count = 0;
    dfs_count(board, visited, start_row, start_col, 2, 
             &solution_count, max_solutions);
    
    /* Cleanup */
    free_visited_grid(visited, board->height);
    
    return solution_count;
}
