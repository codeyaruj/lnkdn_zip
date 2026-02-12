/*
 * main.c - Game Loop
 * Uses immutable Board + mutable GameState architecture
 */

#include "engine.h"
#include <stdio.h>
#include <stdlib.h>

void board_render(const Board *board, const GameState *game);
void ui_show_invalid_move(void);
void ui_show_undo_failed(void);
void ui_show_win(void);
char ui_get_input(void);

int main(void) {
    Board *board = create_puzzle();
    if (!board) {
        fprintf(stderr, "Failed to create puzzle\n");
        return 1;
    }
    
    GameState *game = game_state_create(board);
    if (!game) {
        fprintf(stderr, "Failed to create game state\n");
        board_free(board);
        return 1;
    }
    
    UndoStack *undo_stack = undo_stack_create();
    if (!undo_stack) {
        fprintf(stderr, "Failed to create undo stack\n");
        game_state_free(game);
        board_free(board);
        return 1;
    }
    
    int running = 1;
    int invalid_move = 0;
    int undo_failed = 0;
    
    while (running) {
        board_render(board, game);
        
        if (invalid_move) {
            ui_show_invalid_move();
            invalid_move = 0;
        }
        if (undo_failed) {
            ui_show_undo_failed();
            undo_failed = 0;
        }
        
        if (game_state_check_win(game)) {
            ui_show_win();
            ui_get_input();
            running = 0;
            continue;
        }
        
        char command = ui_get_input();
        
        if (command == 'q' || command == 'Q') {
            running = 0;
        } else if (command == 'u' || command == 'U') {
            if (!undo_stack_pop(undo_stack, game)) {
                undo_failed = 1;
            }
        } else if (command == 'w' || command == 'W' ||
                   command == 'a' || command == 'A' ||
                   command == 's' || command == 'S' ||
                   command == 'd' || command == 'D') {
            int new_row = game->player.row;
            int new_col = game->player.col;
            switch (command) {
                case 'w': case 'W': new_row--; break;
                case 's': case 'S': new_row++; break;
                case 'a': case 'A': new_col--; break;
                case 'd': case 'D': new_col++; break;
            }
            
            undo_stack_push(undo_stack, game, new_row, new_col);
            
            if (!movement_try_move(game, command)) {
                undo_stack_pop(undo_stack, game);  
                invalid_move = 1;
            }
        } else {
            invalid_move = 1;
        }
    }
    
    undo_stack_free(undo_stack);
    game_state_free(game);
    board_free(board);
    
    return 0;
}
