/*
 * generator.c - Procedural Puzzle Generator Implementation
 * 
 * Uses path-first generation with randomized DFS to ensure solvability.
 * 
 * Algorithm Overview:
 * 1. Initialize board with borders
 * 2. Use DFS to create a random non-intersecting path
 * 3. Place numbers along path
 * 4. Add walls to remaining cells
 * 5. Initialize player
 * 
 * Time Complexity: O(rows * cols)
 * Space Complexity: O(rows * cols) for visited tracking
 */

#include "generator.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ============================================================================
 * PATH TRACKING STRUCTURE
 * ============================================================================ */

typedef struct PathNode {
    int row;
    int col;
    struct PathNode *next;
} PathNode;

typedef struct {
    PathNode *head;
    PathNode *tail;
    int length;
} PathList;
/* ============================================================================
 * PATH LIST OPERATIONS
 * ============================================================================ */
static PathList *path_list_create(void) {
    PathList *list = (PathList *)malloc(sizeof(PathList));
    if (!list) return NULL;
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    return list;
}

static bool path_list_append(PathList *list, int row, int col) {
    PathNode *node = (PathNode *)malloc(sizeof(PathNode));
    if (!node) return false;
    
    node->row = row;
    node->col = col;
    node->next = NULL;
    
    if (list->tail) {
        list->tail->next = node;
        list->tail = node;
    } else {
        list->head = node;
        list->tail = node;
    }
    
    list->length++;
    return true;
}

static void path_list_free(PathList *list) {
    if (!list) return;
    
    PathNode *current = list->head;
    while (current) {
        PathNode *next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
/* ============================================================================
 * HELPER FUNCTIONS
 * ============================================================================ */
static bool is_in_inner_bounds(int rows, int cols, int row, int col) {
    return row > 0 && row < rows - 1 && col > 0 && col < cols - 1;
}
static void shuffle_array(int *array, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

/* ============================================================================
 * DFS PATH GENERATION
 * ============================================================================ */
static bool generate_path_dfs(
    bool **visited,
    PathList *path,
    int rows,
    int cols,
    int row,
    int col,
    int target_length,
    int current_length
) {
    visited[row][col] = true;
    if (!path_list_append(path, row, col)) {
        return false;
    }
    
    if (current_length >= target_length) {
        return true;
    }
    
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};
    
    int directions[] = {0, 1, 2, 3};
    shuffle_array(directions, 4);
    
    for (int i = 0; i < 4; i++) {
        int dir = directions[i];
        int new_row = row + dr[dir];
        int new_col = col + dc[dir];
        
        if (is_in_inner_bounds(rows, cols, new_row, new_col) &&
            !visited[new_row][new_col]) {
            
            if (generate_path_dfs(visited, path, rows, cols,
                                 new_row, new_col, target_length,
                                 current_length + 1)) {
                return true;  
            }
        }
    }
    
     return false;
}
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
 * PUZZLE GENERATION PIPELINE
 * ============================================================================ */

static void add_borders(Board *board) {
    for (int col = 0; col < board->width; col++) {
        board_set_wall(board, 0, col);
        board_set_wall(board, board->height - 1, col);
    }
    
    for (int row = 0; row < board->height; row++) {
        board_set_wall(board, row, 0);
        board_set_wall(board, row, board->width - 1);
    }
}

static void place_numbers_on_path(Board *board, PathList *path) {
    PathNode *current = path->head;
    int number = 1;
    
    while (current) {
        board_set_number(board, current->row, current->col, number);
        number++;
        current = current->next;
    }
}

static void add_random_walls(Board *board, bool **visited, float wall_ratio) {
    for (int row = 1; row < board->height - 1; row++) {
        for (int col = 1; col < board->width - 1; col++) {
            if (!visited[row][col]) {
                if ((float)rand() / RAND_MAX < wall_ratio) {
                    board_set_wall(board, row, col);
                }
            }
        }
    }
}

/* ============================================================================
 * PUBLIC API
 * ============================================================================ */

Board *generate_puzzle(
    int rows,
    int cols,
    float path_ratio,
    float wall_ratio,
    unsigned int seed
) {
    if (rows < 5 || cols < 5) {
        return NULL;  
    }
    
    if (path_ratio <= 0.0f || path_ratio > 1.0f) {
        return NULL;  
    }
    
    if (wall_ratio < 0.0f || wall_ratio > 1.0f) {
        return NULL;  
    }
    
    srand(seed);
    
    
    Board *board = board_create(rows, cols);
    if (!board) {
        return NULL;
    }
    add_borders(board);
    
    int inner_area = (rows - 2) * (cols - 2);
    int target_length = (int)(inner_area * path_ratio);
    
    if (target_length < 3) {
        target_length = 3;
    }
    
    bool **visited = create_visited_grid(rows, cols);
    if (!visited) {
        board_free(board);
        return NULL;
    }
    
    for (int col = 0; col < cols; col++) {
        visited[0][col] = true;
        visited[rows - 1][col] = true;
    }
    for (int row = 0; row < rows; row++) {
        visited[row][0] = true;
        visited[row][cols - 1] = true;
    }
    
    PathList *path = path_list_create();
    if (!path) {
        free_visited_grid(visited, rows);
        board_free(board);
        return NULL;
    }
    bool success = false;
    const int MAX_ATTEMPTS = 10;
    
    for (int attempt = 0; attempt < MAX_ATTEMPTS && !success; attempt++) {
        path_list_free(path);
        path = path_list_create();
        if (!path) {
            free_visited_grid(visited, rows);
            board_free(board);
            return NULL;
        }
        
        for (int row = 1; row < rows - 1; row++) {
            for (int col = 1; col < cols - 1; col++) {
                visited[row][col] = false;
            }
        }
        
        int start_row = 1 + rand() % (rows - 2);
        int start_col = 1 + rand() % (cols - 2);
        
        success = generate_path_dfs(
            visited, path, rows, cols,
            start_row, start_col,
            target_length, 1
        );
    }
    
    
    if (path->length < 3) {
        path_list_free(path);
        free_visited_grid(visited, rows);
        board_free(board);
        return NULL;
    }
    
    
    place_numbers_on_path(board, path);
    
    add_random_walls(board, visited, wall_ratio);
    
    if (!board_init_player(board)) {
        path_list_free(path);
        free_visited_grid(visited, rows);
        board_free(board);
        return NULL;
    }
    
    path_list_free(path);
    free_visited_grid(visited, rows);
    
    return board;
}

