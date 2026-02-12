/*
 * engine.c - Game Engine Implementation
 */

#include "engine.h"
#include <stdlib.h>
#include <string.h>

/* ========== BOARD ========== */

Board *board_create(int height, int width) {
    Board *board = (Board *)malloc(sizeof(Board));
    if (!board) return NULL;
    
    board->height = height;
    board->width = width;
    board->max_number = 0;
    board->grid = (Cell **)malloc(height * sizeof(Cell *));
    if (!board->grid) {
        free(board);
        return NULL;
    }
    
    for (int i = 0; i < height; i++) {
        board->grid[i] = (Cell *)calloc(width, sizeof(Cell));
        if (!board->grid[i]) {
            for (int j = 0; j < i; j++) free(board->grid[j]);
            free(board->grid);
            free(board);
            return NULL;
        }
    }
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            board->grid[i][j].type = CELL_EMPTY;
            board->grid[i][j].number = 0;
        }
    }
    
    return board;
}

void board_free(Board *board) {
    if (!board) return;
    for (int i = 0; i < board->height; i++) {
        free(board->grid[i]);
    }
    free(board->grid);
    free(board);
}

void board_set_wall(Board *board, int row, int col) {
    board->grid[row][col].type = CELL_WALL;
}

void board_set_number(Board *board, int row, int col, int number) {
    board->grid[row][col].type = CELL_NUMBER;
    board->grid[row][col].number = number;
    if (number > board->max_number) {
        board->max_number = number;
    }
}

bool board_find_number(const Board *board, int number, int *row, int *col) {
    for (int i = 0; i < board->height; i++) {
        for (int j = 0; j < board->width; j++) {
            if (board->grid[i][j].type == CELL_NUMBER && 
                board->grid[i][j].number == number) {
                *row = i;
                *col = j;
                return true;
            }
        }
    }
    return false;
}

Board *create_puzzle(void) {
    Board *board = board_create(10, 10);
    if (!board) return NULL;
    
    for (int j = 0; j < 10; j++) {
        board_set_wall(board, 0, j);
        board_set_wall(board, 9, j);
    }
    for (int i = 0; i < 10; i++) {
        board_set_wall(board, i, 0);
        board_set_wall(board, i, 9);
    }
    
    board_set_wall(board, 3, 3);
    board_set_wall(board, 3, 4);
    board_set_wall(board, 3, 5);
    board_set_wall(board, 6, 3);
    board_set_wall(board, 6, 4);
    board_set_wall(board, 6, 5);
    board_set_wall(board, 4, 7);
    board_set_wall(board, 5, 7);
    
    board_set_number(board, 1, 1, 1);
    board_set_number(board, 1, 7, 2);
    board_set_number(board, 4, 8, 3);
    board_set_number(board, 7, 7, 4);
    board_set_number(board, 7, 2, 5);
    board_set_number(board, 4, 2, 6);
    board_set_number(board, 2, 5, 7);
    
    return board;
}

/* ========== GAME STATE ========== */

static bool **create_visited_grid(int rows, int cols) {
    bool **visited = (bool **)malloc(rows * sizeof(bool *));
    if (!visited) return NULL;
    
    for (int i = 0; i < rows; i++) {
        visited[i] = (bool *)calloc(cols, sizeof(bool));
        if (!visited[i]) {
            for (int j = 0; j < i; j++) free(visited[j]);
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

GameState *game_state_create(const Board *board) {
    if (!board) return NULL;
    
    GameState *state = (GameState *)malloc(sizeof(GameState));
    if (!state) return NULL;
    
    state->board = board;
    state->visited = create_visited_grid(board->height, board->width);
    if (!state->visited) {
        free(state);
        return NULL;
    }
    
    int start_row, start_col;
    if (!board_find_number(board, 1, &start_row, &start_col)) {
        free_visited_grid(state->visited, board->height);
        free(state);
        return NULL;
    }
    
    state->player.row = start_row;
    state->player.col = start_col;
    state->player.next_number = 2;
    state->visited[start_row][start_col] = true;
    
    return state;
}

void game_state_free(GameState *state) {
    if (!state) return;
    free_visited_grid(state->visited, state->board->height);
    free(state);
}

bool game_state_check_win(const GameState *state) {
    return state->player.next_number > state->board->max_number;
}

/* ========== MOVEMENT ========== */

static bool is_in_bounds(const Board *board, int row, int col) {
    return row >= 0 && row < board->height && col >= 0 && col < board->width;
}

static bool is_valid_move(const GameState *state, int target_row, int target_col) {
    const Board *board = state->board;
    
    if (!is_in_bounds(board, target_row, target_col)) return false;
    
    Cell target = board->grid[target_row][target_col];
    if (target.type == CELL_WALL) return false;
    if (state->visited[target_row][target_col]) return false;
    
    if (target.type == CELL_NUMBER) {
        if (target.number != state->player.next_number) return false;
    }
    
    return true;
}

bool movement_try_move(GameState *state, char direction) {
    int new_row = state->player.row;
    int new_col = state->player.col;
    
    switch (direction) {
        case 'w': case 'W': new_row--; break;
        case 's': case 'S': new_row++; break;
        case 'a': case 'A': new_col--; break;
        case 'd': case 'D': new_col++; break;
        default: return false;
    }
    
    if (!is_valid_move(state, new_row, new_col)) return false;
    
    const Cell *target = &state->board->grid[new_row][new_col];
    if (target->type == CELL_NUMBER) {
        state->player.next_number++;
    }
    
    state->visited[new_row][new_col] = true;
    state->player.row = new_row;
    state->player.col = new_col;
    
    return true;
}

/* ========== UNDO ========== */

typedef struct {
    int prev_row, prev_col;
    int prev_next_number;
    int target_row, target_col;
} UndoState;

typedef struct StackNode {
    UndoState state;
    struct StackNode *next;
} StackNode;

struct UndoStack {
    StackNode *top;
    int size;
};

UndoStack *undo_stack_create(void) {
    UndoStack *stack = (UndoStack *)malloc(sizeof(UndoStack));
    if (!stack) return NULL;
    stack->top = NULL;
    stack->size = 0;
    return stack;
}

void undo_stack_free(UndoStack *stack) {
    if (!stack) return;
    while (stack->top != NULL) {
        StackNode *node = stack->top;
        stack->top = node->next;
        free(node);
    }
    free(stack);
}

bool undo_stack_push(UndoStack *stack, const GameState *state, int target_row, int target_col) {
    if (!stack || !state) return false;
    
    StackNode *node = (StackNode *)malloc(sizeof(StackNode));
    if (!node) return false;
    
    node->state.prev_row = state->player.row;
    node->state.prev_col = state->player.col;
    node->state.prev_next_number = state->player.next_number;
    node->state.target_row = target_row;
    node->state.target_col = target_col;
    
    node->next = stack->top;
    stack->top = node;
    stack->size++;
    return true;
}

bool undo_stack_pop(UndoStack *stack, GameState *state) {
    if (!stack || !state || stack->top == NULL) return false;
    
    StackNode *node = stack->top;
    UndoState undo = node->state;
    
    state->player.row = undo.prev_row;
    state->player.col = undo.prev_col;
    state->player.next_number = undo.prev_next_number;
    state->visited[undo.target_row][undo.target_col] = false;
    
    stack->top = node->next;
    stack->size--;
    free(node);
    return true;
}

bool undo_stack_is_empty(const UndoStack *stack) {
    return stack == NULL || stack->top == NULL;
}
