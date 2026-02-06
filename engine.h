*
 * engine.h - Game Engine Public API
 * 
 * This is a UI-agnostic game engine for the Zip puzzle.
 * It contains all game logic, data structures, and state management.
 * 
 * The engine can be used by any frontend (terminal, GUI, web, solver)
 * without modification.
 * 
 * Responsibilities:
 * - Game state representation (board, player, cells)
 * - Movement validation and execution
 * - Undo system (stack-based)
 * - Win condition checking
 * - Puzzle creation and lifecycle
 * 
 * Does NOT contain:
 * - Rendering logic
 * - Input handling
 * - User feedback messages
 * - Platform-specific code
 */


#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

/* ============================================================================
 * CORE DATA TYPES
 * ============================================================================ */

typedef enum {
    CELL_EMPTY,      
    CELL_WALL,       
    CELL_NUMBER,     
    CELL_PATH        
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
    PlayerState player;
} Board;
typedef struct UndoStack UndoStack;

Board *board_create(int height, int width);
void board_free(Board *board);
void board_set_wall(Board *board, int row, int col);
void board_set_number(Board *board, int row, int col, int number);
bool board_init_player(Board *board);
Board *create_puzzle(void);
