#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "game.h"
#include "render.h"
#include "input.h"

int main() {
    Game game;
    
    // Initialize game
    game_init(&game);
    
    // Enable raw terminal mode
    input_enable_raw_mode();
    
    // Main game loop
    int running = 1;
    int levelJustSolved = 0;
    
    while (running) {
        // Check win condition
        if (game_is_solved(&game) && !levelJustSolved) {
            levelJustSolved = 1;
            render_win_screen(&game);
            
            // Wait for input on win screen
            char c;
            while (read(STDIN_FILENO, &c, 1) == 1) {
                if (c == 'q' || c == 'Q') {
                    running = 0;
                    break;
                } else if (c == 'n' || c == 'N') {
                    // Next level
                    int nextLevel = game.levelNumber + 1;
                    if (nextLevel <= 3) {
                        game_load_level(&game, nextLevel);
                        levelJustSolved = 0;
                    }
                    break;
                } else if (c == 'r' || c == 'R') {
                    // Restart current level
                    game_load_level(&game, game.levelNumber);
                    levelJustSolved = 0;
                    break;
                }
            }
            
            if (!running) break;
            continue;
        }
        
        // Reset solved flag if user makes changes
        if (levelJustSolved && !game_is_solved(&game)) {
            levelJustSolved = 0;
        }
        
        // Render
        render_grid(&game);
        
        // Handle input
        running = input_handle(&game);
    }
    
    // Cleanup
    input_disable_raw_mode();
    render_clear_screen();
    
    printf("\n");
    printf("╔═══════════════════════════════════════╗\n");
    printf("║                                       ║\n");
    printf("║     Thanks for playing ZIP PUZZLE!    ║\n");
    printf("║                                       ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    printf("\n");
    
    return 0;
}
