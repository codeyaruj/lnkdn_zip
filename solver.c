/*
 * solver.c - Puzzle Solver Implementation
 */

#include "solver.h"
#include <stdlib.h>
#include <stdbool.h>

/* ============================================================================
 * VISITED GRID MANAGEMENT
 * ============================================================================
 */

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

static bool is_wall(const Cell *cell) {
    return cell->type == CELL_WALL;
}

static bool is_number_cell(const Cell *cell) {
    return cell->type == CELL_NUMBER;
}

static bool find_start_position(const Board *board, int *start_row, int *start_col) {
    for (int i = 0; i < board->height; i++) {
        for (int j = 0; j < board->width; j++) {
            const Cell *cell = &board->grid[i][j];
            if (is_number_cell(cell) && cell->number == 1) {
                *start_row = i;
                *start_col = j;
                return true;
            }
        }
    }
    return false;
}

static bool is_in_bounds(const Board *board, int row, int col) {
    return row >= 0 && row < board->height && col >= 0 && col < board->width;
}
static bool is_valid_move(
    const Board *board,
    bool **visited,
    int row,
    int col,
    int next_number
) {
    if (!is_in_bounds(board, row, col)) {
        return false;
    }
    
    const Cell *cell = &board->grid[row][col];
    if (is_wall(cell)) {
        return false;
    }
    
    if (visited[row][col]) {
        return false;
    }
    
    if (is_number_cell(cell)) {
        if (cell->number != next_number) {
            return false;
        }
    }
    
    return true;
}

static bool solve_dfs(
    const Board *board,
    bool **visited,
    int row,
    int col,
    int next_number
) {
    if (next_number > board->max_number) {
        return true;  
    }
    
    const int dr[] = {-1, 1, 0, 0};
    const int dc[] = {0, 0, -1, 1};
    
    for (int dir = 0; dir < 4; dir++) {
        int new_row = row + dr[dir];
        int new_col = col + dc[dir];
        
        if (!is_valid_move(board, visited, new_row, new_col, next_number)) {
            continue;  
        }
        
        const Cell *target_cell = &board->grid[new_row][new_col];
        int next_next_number = next_number;
        
        if (is_number_cell(target_cell)) {
            next_next_number++;
        }
        
        visited[new_row][new_col] = true;
        
        if (solve_dfs(board, visited, new_row, new_col, next_next_number)) {
            return true;  
        }
        
        visited[new_row][new_col] = false;
    }
    
    return false;
}

bool puzzle_has_solution(const Board *board) {
    if (!board || !board->grid) {
        return false;
    }
    
    int start_row, start_col;
    if (!find_start_position(board, &start_row, &start_col)) {
        return false;
    }
    
    bool **visited = create_visited_grid(board->height, board->width);
    if (!visited) {
        return false;
    }
    
    visited[start_row][start_col] = true;
    

    bool has_solution = solve_dfs(board, visited, start_row, start_col, 2);
    
    free_visited_grid(visited, board->height);
    
    return has_solution;
}
