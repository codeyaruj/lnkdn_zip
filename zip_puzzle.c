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

/* ============================================================================
 * BOARD OPERATIONS
 * ============================================================================ */

/* Allocate and initialize a board */
Board *board_create(int height, int width) {
    Board *board = (Board *)malloc(sizeof(Board));
    if (!board) return NULL;
    
    board->height = height;
    board->width = width;
    board->max_number = 0;
    
    /* Allocate grid */
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
    
    /* Initialize all cells as empty */
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            board->grid[i][j].type = CELL_EMPTY;
            board->grid[i][j].number = 0;
        }
    }
    
    return board;
}

/* Free board memory */
void board_free(Board *board) {
    if (!board) return;
    for (int i = 0; i < board->height; i++) {
        free(board->grid[i]);
    }
    free(board->grid);
    free(board);
}

/* Set a cell to be a wall */
void board_set_wall(Board *board, int row, int col) {
    board->grid[row][col].type = CELL_WALL;
}

/* Set a cell to be a numbered waypoint */
void board_set_number(Board *board, int row, int col, int number) {
    board->grid[row][col].type = CELL_NUMBER;
    board->grid[row][col].number = number;
    if (number > board->max_number) {
        board->max_number = number;
    }
}

/* Initialize player at starting position (cell with number 1) */
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
 * HARDCODED PUZZLE
 * ============================================================================ */

Board *create_puzzle() {
    /* Create a 10x10 puzzle */
    Board *board = board_create(10, 10);
    if (!board) return NULL;
    
    /* Define walls (border + internal obstacles) */
    /* Top and bottom borders */
    for (int j = 0; j < 10; j++) {
        board_set_wall(board, 0, j);
        board_set_wall(board, 9, j);
    }
    /* Left and right borders */
    for (int i = 0; i < 10; i++) {
        board_set_wall(board, i, 0);
        board_set_wall(board, i, 9);
    }
    
    /* Internal walls */
    board_set_wall(board, 3, 3);
    board_set_wall(board, 3, 4);
    board_set_wall(board, 3, 5);
    board_set_wall(board, 6, 3);
    board_set_wall(board, 6, 4);
    board_set_wall(board, 6, 5);
    board_set_wall(board, 4, 7);
    board_set_wall(board, 5, 7);
    
    /* Place numbers */
    board_set_number(board, 1, 1, 1);  /* Start */
    board_set_number(board, 1, 7, 2);
    board_set_number(board, 4, 8, 3);
    board_set_number(board, 7, 7, 4);
    board_set_number(board, 7, 2, 5);
    board_set_number(board, 4, 2, 6);
    board_set_number(board, 2, 5, 7);  /* End */
    
    /* Initialize player at position 1 */
    if (!board_init_player(board)) {
        board_free(board);
        return NULL;
    }
    
    return board;
}

/* ============================================================================
 * RENDERING
 * ============================================================================ */

void board_render(Board *board) {
    /* Clear screen (ANSI escape code) */
    printf("\033[2J\033[H");
    
    printf("=== ZIP PUZZLE ===\n");
    printf("Next number to reach: %d / %d\n", 
           board->player.next_number, board->max_number);
    printf("WASD to move, U to undo, Q to quit\n\n");
    
    for (int i = 0; i < board->height; i++) {
        for (int j = 0; j < board->width; j++) {
            /* Check if cursor is at this position */
            if (i == board->player.row && j == board->player.col) {
                printf("@ ");
                continue;
            }
            
            Cell cell = board->grid[i][j];
            switch (cell.type) {
                case CELL_WALL:
                    printf("# ");
                    break;
                case CELL_EMPTY:
                    printf(". ");
                    break;
                case CELL_NUMBER:
                    printf("%d ", cell.number);
                    break;
                case CELL_PATH:
                    printf("* ");
                    break;
            }
        }
        printf("\n");
    }
    printf("\n");
}

/* ============================================================================
 * MOVEMENT VALIDATION AND EXECUTION
 * ============================================================================ */

bool is_in_bounds(Board *board, int row, int col) {
    return row >= 0 && row < board->height && col >= 0 && col < board->width;
}

bool is_valid_move(Board *board, int target_row, int target_col) {

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

void execute_move(Board *board, int target_row, int target_col, 
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
    
    /* Move player */
    board->player.row = target_row;
    board->player.col = target_col;
    
    stack_push(undo_stack, undo_state);
}

bool try_move(Board *board, char direction, UndoStack *undo_stack) {
    int new_row = board->player.row;
    int new_col = board->player.col;
    
    switch (direction) {
        case 'w': case 'W': new_row--; break;  /* Up */
        case 's': case 'S': new_row++; break;  /* Down */
        case 'a': case 'A': new_col--; break;  /* Left */
        case 'd': case 'D': new_col++; break;  /* Right */
        default: return false;
    }
    
    if (is_valid_move(board, new_row, new_col)) {
        execute_move(board, new_row, new_col, undo_stack);
        return true;
    }
    
    return false;
}
/* ============================================================================
 * PHASE 2: UNDO IMPLEMENTATION
 * ============================================================================ */

/* Undo the last move */
bool undo_move(Board *board, UndoStack *undo_stack) {
    if (stack_is_empty(undo_stack)) {
        return false;
    }
    
    UndoState state;
    if (!stack_pop(undo_stack, &state)) {
        return false;
    }
    
    /* Restore the cell we moved to (restore its original state) */
    board->grid[board->player.row][board->player.col] = state.previous_cell;
    
    /* Restore player position and state */
    board->player.row = state.row;
    board->player.col = state.col;
    board->player.next_number = state.next_number;
    
    return true;
}

/* ============================================================================
 * WIN CONDITION
 * ============================================================================ */

/* Check if player has won */
bool check_win(Board *board) {
    /* Win condition: next_number > max_number means we've visited all numbers */
    return board->player.next_number > board->max_number;
}

/* ============================================================================
 * MAIN GAME LOOP
 * ============================================================================ */

int main(void) {
    /* Create puzzle */
    Board *board = create_puzzle();
    if (!board) {
        fprintf(stderr, "Failed to create puzzle\n");
        return 1;
    }
    
    /* Initialize undo stack */
    UndoStack undo_stack;
    stack_init(&undo_stack);
    
    /* Game loop */
    bool running = true;
    bool invalid_move = false;
    bool undo_failed = false;
    
    while (running) {
        /* Render board */
        board_render(board);
        
        /* Show feedback messages */
        if (invalid_move) {
            printf("Invalid move! Try again.\n");
            invalid_move = false;
        }
        if (undo_failed) {
            printf("Nothing to undo!\n");
            undo_failed = false;
        }
        
        /* Check win condition */
        if (check_win(board)) {
            printf("*** CONGRATULATIONS! YOU WON! ***\n");
            printf("Press any key to exit...\n");
            getchar();
            running = false;
            continue;
        }
        
        /* Get input */
        printf("Your move: ");
        char input[10];
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        
        char command = input[0];
        
        /* Process input */
        if (command == 'q' || command == 'Q') {
            running = false;
        } else if (command == 'u' || command == 'U') {
            /* PHASE 2: Undo */
            if (!undo_move(board, &undo_stack)) {
                undo_failed = true;
            }
        } else if (command == 'w' || command == 'W' ||
                   command == 'a' || command == 'A' ||
                   command == 's' || command == 'S' ||
                   command == 'd' || command == 'D') {
            /* PHASE 1: Movement */
            if (!try_move(board, command, &undo_stack)) {
                invalid_move = true;
            }
        } else {
            invalid_move = true;
        }
    }
    
    /* Cleanup */
    stack_free(&undo_stack);
    board_free(board);
    
    return 0;
}
