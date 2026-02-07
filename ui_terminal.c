/*
 * ui_terminal.c - Terminal UI Layer
 * 
 * Handles all terminal-specific rendering and input.
 * This layer reads engine state but never modifies it directly.
 * All state changes go through the engine API.
 * 
 * Responsibilities:
 * - ASCII rendering of the game board
 * - User input collection
 * - User feedback messages
 * - Terminal control (clearing screen)
 * 
 * This layer can be replaced with:
 * - ui_ncurses.c (for fancy terminal UI)
 * - ui_raylib.c (for graphical UI)
 * - ui_web.c (for web frontend)
 * without touching the engine
 */

#include "engine.h"
#include <stdio.h>
/* ============================================================================
 * RENDERING
 * ============================================================================ */
void board_render(const Board *board) {
    printf("\033[2J\033[H");
    
    printf("=== ZIP PUZZLE ===\n");
    printf("Next number to reach: %d / %d\n", 
           board->player.next_number, board->max_number);
    printf("WASD to move, U to undo, Q to quit\n\n");
    
    for (int i = 0; i < board->height; i++) {
        for (int j = 0; j < board->width; j++) {
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
 * USER FEEDBACK
 * ============================================================================ */
void ui_show_invalid_move(void) {
    printf("Invalid move! Try again.\n");
}

void ui_show_undo_failed(void) {
    printf("Nothing to undo!\n");
}

void ui_show_win(void) {
    printf("*** CONGRATULATIONS! YOU WON! ***\n");
    printf("Press any key to exit...\n");
}
/* ============================================================================
 * INPUT HANDLING
 * ============================================================================ */
char ui_get_input(void) {
    printf("Your move: ");
    char input[10];
    if (!fgets(input, sizeof(input), stdin)) {
        return 'q';  
    }
    return input[0];
}

