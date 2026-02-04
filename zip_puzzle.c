#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* ============================================================================
 *CORE ENGINE - DATA STRUCTURES
 * ============================================================================ */

/* Cell types in the grid */
typedef enum {
    CELL_EMPTY,      /* Walkable empty cell */
    CELL_WALL,       /* Impassable wall */
    CELL_NUMBER,     /* Numbered waypoint (1..N) */
    CELL_PATH        /* Visited cell (part of the path) */
} CellType;

/* Grid cell structure */
typedef struct {
    CellType type;
    int number;      /* Only meaningful when type == CELL_NUMBER */
} Cell;

/* Player/cursor state */
typedef struct {
    int row;
    int col;
    int next_number; /* Next number to visit (1 initially) */
} PlayerState;

/* Game board structure */
typedef struct {
    int height;
    int width;
    int max_number;  /* Highest number on the board (goal) */
    Cell **grid;
    PlayerState player;
} Board;
