#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "game.h"
#include "render.h"
#include "input.h"

int main() {
    Game game;
    
    game_init(&game);
    
    input_enable_raw_mode();
    
    int running = 1;
    int levelJustSolved = 0;
    
    while (running) {
        if (game_is_solved(&game) && !levelJustSolved) {
            levelJustSolved = 1;
            render_win_screen(&game);
            
            char c;
            while (read(STDIN_FILENO, &c, 1) == 1) {
                if (c == 'q' || c == 'Q') {
                    running = 0;
                    break;
                } else if (c == 'n' || c == 'N') {
                    int nextLevel = game.levelNumber + 1;
                    if (nextLevel <= 3) {
                        game_load_level(&game, nextLevel);
                        levelJustSolved = 0;
                    }
                    break;
                } else if (c == 'r' || c == 'R') {
                    game_load_level(&game, game.levelNumber);
                    levelJustSolved = 0;
                    break;
                }
            }
            
            if (!running) break;
            continue;
        }
        
        if (levelJustSolved && !game_is_solved(&game)) {
            levelJustSolved = 0;
        }
        
        render_grid(&game);
        
        running = input_handle(&game);
    }
    
    input_disable_raw_mode();
    render_clear_screen();
        
    return 0;
}
