 ## OUTLINE 
  ## World 
    Grid of cells
    Each cell can be:
    empty
    wall
    numbered node (1..N)
    path (drawn by player)
  
  ## Player 
    Current cursor position
    Current target number (next expected)
  
  ## Rules
    You can move up/down/left/right
    You can only step into:
      empty cells
      the next number
    If you hit a wall → invalid
    If you hit the wrong number → invalid
    Finish when you reach N

## Phase 1 — Core Data Model + Static Board
  ## Goal:
      Represent the puzzle correctly in C, print it, and validate movement — no solving yet.
      No undo, hints, or fancy inputs.
      
