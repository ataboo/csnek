
#ifndef CSNEK_H
    #define CSNEK_H

    // Time in micros between render frames
    #define U_DELTA_RENDER 1e6 / 60.0f
    // Time in micros between step frames
    #define U_DELTA_STEP 1e6f / 15.0f
    
    #define GRID_WIDTH 70
    #define GRID_HEIGHT 40
    
    #define FOOD_VALUE 4
    #define MAX_SNAKE_LENGTH 512

    #define WALL_COLOR 1
    #define SNAKE_COLOR 2
    #define FOOD_COLOR 3
    #define HIT_COLOR 4

    enum Direction{UP, RIGHT, DOWN, LEFT};

    // Update rendering of moving objects
    void renderTick();
    
    // Get the micros `refTime` is after `now`
    unsigned uSecondsAfter(struct timeval refTime, struct timeval now);
    
    void drawWalls();
    void drawSnek();
    void drawFood();
    
    // Increment the snek forward for a frame and detect/handle consequences
    bool stepSnek();

    // Handle any key presses
    void updateInput();

    // Reset the level
    void reset();
    void printInMiddle();

    // Handle the snek dying at a position
    void die(char* msg, struct Vector2* diePos);

    // Transform a grid position to screen position
    struct Vector2 gridToScreen(int x, int y);

    // Move the food to a random empty square
    bool moveFood();

    // Determine if a grid point is within the snek's body
    bool pointInBody();

    // Generate a random int between min and max
    int randInRange(int min, int max);

    // Release any allocated memory
    void cleanup();

#endif

