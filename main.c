/*
 * main.c - Main Game Loop
 * 
 * This is the glue layer that connects the engine to the UI.
 * It orchestrates the game flow but contains minimal logic.
 * 
 * Responsibilities:
 * - Initialize engine and UI components
 * - Run the game loop
 * - Delegate rendering to UI
 * - Delegate logic to engine
 * - Handle cleanup
 * 
 * This file demonstrates how the engine and UI are decoupled:
 * - Engine knows nothing about rendering
 * - UI knows nothing about game rules
 * - Main just coordinates between them
 */

#include "engine.h"
#include <stdio.h>
#include <stdlib.h>

void board_render(const Board *board);
void ui_show_invalid_move(void);
void ui_show_undo_failed(void);
void ui_show_win(void);
char ui_get_input(void);
/* ============================================================================
 * MAIN GAME LOOP
 * ============================================================================ */
int main(void) {
    Board *board = create_puzzle();
    if (!board) {
        fprintf(stderr, "Failed to create puzzle\n");
        return 1;
    }
    
    UndoStack *undo_stack = undo_stack_create();
    if (!undo_stack) {
        fprintf(stderr, "Failed to create undo stack\n");
        board_free(board);
        return 1;
    }
    
    int running = 1;
    int invalid_move = 0;
    int undo_failed = 0;
    
    while (running) {
        board_render(board);
        
        if (invalid_move) {
            ui_show_invalid_move();
            invalid_move = 0;
        }
        if (undo_failed) {
            ui_show_undo_failed();
            undo_failed = 0;
        }
        
        if (check_win(board)) {
            ui_show_win();
            ui_get_input();  
            running = 0;
            continue;
        }
        
        char command = ui_get_input();
        
        if (command == 'q' || command == 'Q') {
            running = 0;
        } else if (command == 'u' || command == 'U') {
            if (!undo_move(board, undo_stack)) {
                undo_failed = 1;
            }
        } else if (command == 'w' || command == 'W' ||
                   command == 'a' || command == 'A' ||
                   command == 's' || command == 'S' ||
                   command == 'd' || command == 'D') {
            if (!try_move(board, command, undo_stack)) {
                invalid_move = 1;
            }
        } else {
            invalid_move = 1;
        }
    }
    
    undo_stack_free(undo_stack);
    board_free(board);
    
    return 0;
}

