/*
 * ui_terminal.c - Terminal UI
 */

#include "engine.h"
#include <stdio.h>

void board_render(const Board *board, const GameState *game) {
    printf("\033[2J\033[H");
    
    printf("=== ZIP PUZZLE ===\n");
    printf("Next number to reach: %d / %d\n", 
           game->player.next_number, board->max_number);
    printf("WASD to move, U to undo, Q to quit\n\n");
    
    for (int i = 0; i < board->height; i++) {
        for (int j = 0; j < board->width; j++) {
            if (i == game->player.row && j == game->player.col) {
                printf("@ ");
                continue;
            }
            
            if (game->visited[i][j]) {
                printf("* ");
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
            }
        }
        printf("\n");
    }
    printf("\n");
}

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

char ui_get_input(void) {
    printf("Your move: ");
    char input[10];
    if (!fgets(input, sizeof(input), stdin)) {
        return 'q';
    }
    return input[0];
}
