
#ifndef CSNEK_H
    #define CSNEK_H
    #define U_DELTA_RENDER 1e6/20.0f
    #define U_DELTA_STEP 1e6f / 2.0f
    #define GRID_WIDTH 20
    #define GRID_HEIGHT 20

    enum GridState{MT, WALL, HEAD, BODY};
    enum Direction{UP, RIGHT, DOWN, LEFT};

    void renderTick();
    unsigned uSecondsAfter(struct timeval refTime, struct timeval now);
    void initSnek();
    void drawWalls();
    void drawSnek();
    char snekChar();
    void stepSnek();
    void updateInput();

#endif

