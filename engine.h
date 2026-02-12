/*
 * engine.h - Game Engine Public API
 * Immutable Board + Separate Runtime State Architecture
 */

#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

typedef enum {
    CELL_EMPTY,
    CELL_WALL,
    CELL_NUMBER
} CellType;

typedef struct {
    CellType type;
    int number;
} Cell;

typedef struct {
    int row;
    int col;
    int next_number;
} PlayerState;

typedef struct {
    int height;
    int width;
    int max_number;
    Cell **grid;
} Board;

typedef struct {
    const Board *board;
    bool **visited;
    PlayerState player;
} GameState;

typedef struct UndoStack UndoStack;

Board *board_create(int height, int width);
void board_free(Board *board);
void board_set_wall(Board *board, int row, int col);
void board_set_number(Board *board, int row, int col, int number);
bool board_find_number(const Board *board, int number, int *row, int *col);
Board *create_puzzle(void);

GameState *game_state_create(const Board *board);
void game_state_free(GameState *state);
bool game_state_check_win(const GameState *state);

bool movement_try_move(GameState *state, char direction);

UndoStack *undo_stack_create(void);
void undo_stack_free(UndoStack *stack);
bool undo_stack_push(UndoStack *stack, const GameState *state, int target_row, int target_col);
bool undo_stack_pop(UndoStack *stack, GameState *state);
bool undo_stack_is_empty(const UndoStack *stack);

#endif
