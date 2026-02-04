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
/* ============================================================================
 * UNDO SYSTEM - DATA STRUCTURES
 * ============================================================================ */

/* State snapshot for undo - stores minimum required data */
typedef struct {
    int row;
    int col;
    int next_number;
    /* We only need to store the cell we're leaving (to restore it) */
    Cell previous_cell;
    int prev_row;
    int prev_col;
} UndoState;

/* Stack node for undo history */
typedef struct StackNode {
    UndoState state;
    struct StackNode *next;
} StackNode;

/* Stack structure */
typedef struct {
    StackNode *top;
    int size;
} UndoStack;

/* ============================================================================
 * UNDO STACK OPERATIONS
 * ============================================================================ */

/* Initialize an empty undo stack */
void stack_init(UndoStack *stack) {
    stack->top = NULL;
    stack->size = 0;
}

/* Push a state onto the undo stack */
bool stack_push(UndoStack *stack, UndoState state) {
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

/* Pop a state from the undo stack */
bool stack_pop(UndoStack *stack, UndoState *state) {
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

/* Check if stack is empty */
bool stack_is_empty(UndoStack *stack) {
    return stack->top == NULL;
}

/* Free all stack memory */
void stack_free(UndoStack *stack) {
    while (stack->top != NULL) {
        StackNode *node = stack->top;
        stack->top = node->next;
        free(node);
    }
    stack->size = 0;
}
