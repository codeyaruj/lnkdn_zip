/*
 * engine.c - Game Engine Implementation
 * 
 * Pure game logic with no UI dependencies.
 * All functions are UI-agnostic and thread-safe (if used properly).
 */

#include "engine.h"
#include <stdlib.h>
#include <string.h>
/* ============================================================================
 * UNDO STACK INTERNAL STRUCTURES
 * ============================================================================ */
typedef struct {
    int row;
    int col;
    int next_number;
    Cell previous_cell;
    int prev_row;
    int prev_col;
} UndoState;

typedef struct StackNode {
    UndoState state;
    struct StackNode *next;
} StackNode;

struct UndoStack {
    StackNode *top;
    int size;
};
/* ============================================================================
 * UNDO STACK IMPLEMENTATION
 * ============================================================================ */
UndoStack *undo_stack_create(void) {
    UndoStack *stack = (UndoStack *)malloc(sizeof(UndoStack));
    if (!stack) {
        return NULL;
    }
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

static bool stack_push(UndoStack *stack, UndoState state) {
    StackNode *node = (StackNode *)malloc(sizeof(StackNode));
    if (!node) {
        return false;
    }
    node->state = state;
    node->next = stack->top;
    stack->top = node;
    stack->size++;
    return true;
}

static bool stack_pop(UndoStack *stack, UndoState *state) {
    if (stack->top == NULL) {
        return false;
    }
    StackNode *node = stack->top;
    *state = node->state;
    stack->top = node->next;
    stack->size--;
    free(node);
    return true;
}

bool undo_stack_is_empty(const UndoStack *stack) {
    return stack->top == NULL;
}
/* ============================================================================
 * BOARD LIFECYCLE
 * ============================================================================ */
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
            for (int j = 0; j < i; j++) {
                free(board->grid[j]);
            }
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

bool board_init_player(Board *board) {
    for (int i = 0; i < board->height; i++) {
        for (int j = 0; j < board->width; j++) {
            if (board->grid[i][j].type == CELL_NUMBER && 
                board->grid[i][j].number == 1) {
                board->player.row = i;
                board->player.col = j;
                board->player.next_number = 2; /* Already at 1, looking for 2 */
                /* Mark starting cell as visited */
                board->grid[i][j].type = CELL_PATH;
                return true;
            }
        }
    }
    return false;
}
/* ============================================================================
 * PUZZLE CREATION
 * ============================================================================ */
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
  
    if (!board_init_player(board)) {
        board_free(board);
        return NULL;
    }
    
    return board;
}
/* ============================================================================
 * MOVEMENT LOGIC
 * ============================================================================ */
static bool is_in_bounds(const Board *board, int row, int col) {
    return row >= 0 && row < board->height && col >= 0 && col < board->width;
}
static bool is_valid_move(const Board *board, int target_row, int target_col) {
    if (!is_in_bounds(board, target_row, target_col)) {
        return false;
    }
    
    Cell target = board->grid[target_row][target_col];
    if (target.type == CELL_WALL) {
        return false;
    }
    if (target.type == CELL_PATH) {
        return false;
    }
    if (target.type == CELL_NUMBER) {
        if (target.number != board->player.next_number) {
            return false;
        }
    }
    
    return true;
}
static void execute_move(Board *board, int target_row, int target_col, 
                         UndoStack *undo_stack) {
    UndoState undo_state;
    undo_state.row = board->player.row;
    undo_state.col = board->player.col;
    undo_state.next_number = board->player.next_number;
    undo_state.prev_row = board->player.row;
    undo_state.prev_col = board->player.col;
    undo_state.previous_cell = board->grid[target_row][target_col];
    if (board->grid[target_row][target_col].type == CELL_NUMBER) {
        board->player.next_number++;
    }
    
    board->grid[target_row][target_col].type = CELL_PATH;
    
    board->player.row = target_row;
    board->player.col = target_col;
    
    stack_push(undo_stack, undo_state);
}

bool try_move(Board *board, char direction, UndoStack *undo_stack) {
    int new_row = board->player.row;
    int new_col = board->player.col;
    
    switch (direction) {
        case 'w': case 'W': new_row--; break;  
        case 's': case 'S': new_row++; break;  
        case 'a': case 'A': new_col--; break;  
        case 'd': case 'D': new_col++; break;  
        default: return false;
    }
    
    if (is_valid_move(board, new_row, new_col)) {
        execute_move(board, new_row, new_col, undo_stack);
        return true;
    }
    
    return false;
}
/* ============================================================================
 * UNDO IMPLEMENTATION
 * ============================================================================ */
bool undo_move(Board *board, UndoStack *undo_stack) {
    if (undo_stack_is_empty(undo_stack)) {
        return false;
    }
    
    UndoState state;
    if (!stack_pop(undo_stack, &state)) {
        return false;
    }
    
  board->grid[board->player.row][board->player.col] = state.previous_cell;
  
    board->player.row = state.row;
    board->player.col = state.col;
    board->player.next_number = state.next_number;
    
    return true;
}
/* ============================================================================
 * WIN CONDITION
 * ============================================================================ */

bool check_win(const Board *board) {
    return board->player.next_number > board->max_number;
}
