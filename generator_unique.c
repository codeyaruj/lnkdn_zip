/*
 * generator_unique.c - Unique Solution Puzzle Generator Implementation
 * 
 * Generation Pipeline:
 * 1. Generate puzzle using path-first algorithm
 * 2. Count solutions (stop at 2)
 * 3. If unique (count==1): return
 * 4. If not unique: discard and retry
 * 5. Repeat until unique puzzle found or max_attempts exceeded
 */

#include "generator_unique.h"
#include "generator.h"
#include "solver_count.h"
#include <stdlib.h>

Board *generate_unique_puzzle(
    int rows,
    int cols,
    float path_ratio,
    float wall_ratio,
    unsigned int seed,
    int max_attempts
) {
    /* Validate parameters */
    if (rows < 5 || cols < 5) {
        return NULL;
    }
    
    if (path_ratio <= 0.0f || path_ratio > 1.0f) {
        return NULL;
    }
    
    if (wall_ratio < 0.0f || wall_ratio > 1.0f) {
        return NULL;
    }
    
    /* Generation loop with uniqueness validation */
    int attempt = 0;
    unsigned int current_seed = seed;
    
    while (max_attempts == 0 || attempt < max_attempts) {
        attempt++;
        
        /* Generate candidate puzzle
         * 
         * Note: We use a different seed for each attempt to ensure variety.
         * Simply incrementing seed ensures deterministic retry sequence.
         */
        Board *candidate = generate_puzzle(rows, cols, path_ratio, wall_ratio, current_seed);
        
        if (!candidate) {
            /* Generation failed - try next seed */
            current_seed++;
            continue;
        }
        
        /* Count solutions (stop at 2 for efficiency)
         * 
         * Why max_solutions = 2?
         * We only need to distinguish:
         *   0 solutions  -> broken (should never happen with path-first)
         *   1 solution   -> PERFECT (this is what we want)
         *   2+ solutions -> ambiguous (reject)
         * 
         * Stopping at 2 means we don't waste time finding ALL solutions
         * when we only need to know "is it unique or not".
         */
        int solution_count = puzzle_count_solutions(candidate, 2);
        
        if (solution_count == 0) {
            /* This should NEVER happen with path-first generation
             * If it does, it's a generator bug - log and try again
             */
            #ifdef DEBUG
            fprintf(stderr, "Warning: Generated unsolvable puzzle (seed: %u)\n", current_seed);
            #endif
            board_free(candidate);
            current_seed++;
            continue;
        }
        
        if (solution_count == 1) {
            /* SUCCESS: Found a puzzle with unique solution */
            return candidate;
        }
        
        /* solution_count >= 2
         * Puzzle has multiple solutions - reject and try again
         * 
         * This happens when random walls create alternative paths
         * around the generated solution path.
         */
        board_free(candidate);
        current_seed++;
    }
    
    /* Max attempts exceeded - no unique puzzle found
     * 
     * This can happen with very restrictive parameters
     * (e.g., very high wall_ratio limits maze connectivity)
     */
    return NULL;
}

/* ============================================================================
 * STATISTICS AND DEBUGGING (Optional)
 * ============================================================================ */

#ifdef GENERATOR_STATS

/*
 * Extended version that tracks generation statistics
 * Useful for tuning parameters and understanding generation quality
 */

typedef struct {
    int total_attempts;
    int unsolvable_count;    /* Should always be 0 with path-first */
    int multiple_solutions_count;
    int unique_found;
    unsigned int final_seed;
} GenerationStats;

Board *generate_unique_puzzle_stats(
    int rows,
    int cols,
    float path_ratio,
    float wall_ratio,
    unsigned int seed,
    int max_attempts,
    GenerationStats *stats
) {
    if (stats) {
        stats->total_attempts = 0;
        stats->unsolvable_count = 0;
        stats->multiple_solutions_count = 0;
        stats->unique_found = 0;
        stats->final_seed = seed;
    }
    
    int attempt = 0;
    unsigned int current_seed = seed;
    
    while (max_attempts == 0 || attempt < max_attempts) {
        attempt++;
        if (stats) stats->total_attempts = attempt;
        
        Board *candidate = generate_puzzle(rows, cols, path_ratio, wall_ratio, current_seed);
        if (!candidate) {
            current_seed++;
            continue;
        }
        
        int solution_count = puzzle_count_solutions(candidate, 2);
        
        if (solution_count == 0) {
            if (stats) stats->unsolvable_count++;
            board_free(candidate);
            current_seed++;
            continue;
        }
        
        if (solution_count >= 2) {
            if (stats) stats->multiple_solutions_count++;
            board_free(candidate);
            current_seed++;
            continue;
        }
        
        /* Found unique solution */
        if (stats) {
            stats->unique_found = 1;
            stats->final_seed = current_seed;
        }
        return candidate;
    }
    
    return NULL;
}

#endif 
