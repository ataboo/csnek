#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include "infinistack.h"
#include "csnek.h"

struct timeval lastRenderFrame, lastStepFrame, thisFrameTime;
int lastKey;
char blankRow[GRID_WIDTH+1];
char statusBuffer[GRID_WIDTH];
struct Infinistack* snekStack;
enum Direction heading;

int snekLength;
int snekTargetLength;
int running = 1;
struct Vector2 foodPos;
struct Vector2* snekBodySegments;

int main(int argc, char *argv[]) {
    unsigned uDelta;
    
    initscr();

    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, true);
    snekStack = createStack(MAX_SNAKE_LENGTH);
    snekBodySegments = calloc(MAX_SNAKE_LENGTH, sizeof(struct Vector2));

    // Allow keys to be read without blocking
    nodelay(stdscr, true);

    start_color();
    init_pair(WALL_COLOR, COLOR_WHITE, COLOR_WHITE);
    init_pair(SNAKE_COLOR, COLOR_GREEN, COLOR_GREEN);
    init_pair(FOOD_COLOR, COLOR_WHITE, COLOR_YELLOW);
    init_pair(HIT_COLOR, COLOR_RED, COLOR_RED);

    reset();

    while(running) {
        updateInput();
        gettimeofday(&thisFrameTime, NULL);
        
        if (uSecondsAfter(lastStepFrame, thisFrameTime) >= U_DELTA_STEP) {
            if (!stepSnek()) {
                printf("err: failed to move the snek!");
                return 1;

            }
        }
        
        if (uSecondsAfter(lastRenderFrame, thisFrameTime) > U_DELTA_RENDER) {
            renderTick();
        }
    }

    endwin();

    return 0;
}

void updateInput() {
    int ch = getch();
    if (ch > 0) {
        switch (ch) {
        case 3:
        case 4:
        case 27:
            // Escape sequence was pressed
            running = 0;
            break;
        case KEY_LEFT:
        case 'a':
            if (heading != RIGHT) {
                heading = LEFT;
            }
            break;
        case KEY_UP:
        case 'w':
            if (heading != DOWN) {
                heading = UP;
            }
            break;
        case KEY_RIGHT:
        case 'd':
            if (heading != LEFT) {
                heading = RIGHT;
            }
            break;
        case KEY_DOWN:
        case 's':
            if (heading != UP) {
                heading = DOWN;
            }
            break;
        default:
            // mvprintw(GRID_HEIGHT+3, 1, "key: %d", ch);
            break;
        }
    }
}

void renderTick() {
    gettimeofday(&lastRenderFrame, NULL);
    drawSnek();
    drawFood();
    
    mvprintw(GRID_HEIGHT+1, GRID_WIDTH*2 - 10, "Score: %d", snekLength);

    refresh();
}

void reset() {
    snekLength = 3;
    snekTargetLength = 3;
    heading = UP;
    push(snekStack, GRID_WIDTH/2, GRID_HEIGHT/2+2);
    push(snekStack, GRID_WIDTH/2, GRID_HEIGHT/2+1);
    push(snekStack, GRID_WIDTH/2, GRID_HEIGHT/2);
    clear();
    drawWalls();
    moveFood();
}

bool stepSnek() {
    gettimeofday(&lastStepFrame, NULL);

    int i;
    struct Vector2 nextPos;
    struct Vector2* point;
    struct Vector2* headPos;

    // Update the snekBodySegments with the last `snekLength` positions
    peakTopN(snekStack, snekBodySegments, snekLength);
    headPos = &snekBodySegments[0];
    memcpy(&nextPos, headPos, sizeof(struct Vector2));

    switch (heading) {
    case UP:
        nextPos.y--;
        break;
    case DOWN:
        nextPos.y++;
        break;
    case LEFT:
        nextPos.x--;
        break;
    case RIGHT:
        nextPos.x++;
        break;
    }

    // Check if ran into wall
    if (nextPos.x == 0 || nextPos.x == GRID_WIDTH-1 || nextPos.y == 0 || nextPos.y == GRID_HEIGHT-1) {
        die("Death by Wall!", &nextPos);
        return true;
    }

    // Check if ran into self
    if (pointInBody(nextPos.x, nextPos.y)) {
        die("Death by self!", &nextPos);
        return true;
    }

    push(snekStack, nextPos.x, nextPos.y);

    if (foodPos.x == nextPos.x && foodPos.y == nextPos.y) {
        snekTargetLength += FOOD_VALUE;
        if (!moveFood()) {
            return false;
        }
    }

    if (snekLength < snekTargetLength) {
        snekLength++;
    }

    return true;
}

void die(char* msg, struct Vector2* diePos) {
    struct Vector2 screenPos = gridToScreen(diePos->x, diePos->y);

    attron(COLOR_PAIR(HIT_COLOR));
    mvaddch(screenPos.y, screenPos.x, 'X');
    mvaddch(screenPos.y, screenPos.x+1, 'X');
    attroff(COLOR_PAIR(HIT_COLOR));
    printInMiddle(msg, 14);

    nodelay(stdscr, false);
    while(1) {
        int key = getch();
        switch(key) {
        case 3:
        case 4:
        case 27:
            running = false;
            return;
        case 32:
        case 10:
            goto resetRound;
        default:
            //
            break;
        }
    }

    resetRound:
    nodelay(stdscr, true);
    
    reset();
}

void printInMiddle(char* str, int charCount) {
    mvprintw(GRID_HEIGHT+1, GRID_WIDTH - charCount / 2, str);
}

    // Render the walls
unsigned uSecondsAfter(struct timeval refTime, struct timeval now) {
    return (now.tv_sec - refTime.tv_sec) * 1e6f + (now.tv_usec - refTime.tv_usec);
}

void drawWalls() {
    attron(COLOR_PAIR(WALL_COLOR));

    move(1, 1);
    vline('|', GRID_HEIGHT);
    move(1, 2);
    vline('|', GRID_HEIGHT);
    move(1, GRID_WIDTH * 2 - 1);
    vline('|', GRID_HEIGHT);
    move(1, GRID_WIDTH * 2);
    vline('|', GRID_HEIGHT);
    move(1, 1);
    hline('-', GRID_WIDTH * 2);
    move(GRID_HEIGHT,1);
    hline('-', GRID_WIDTH * 2);

    attroff(COLOR_PAIR(WALL_COLOR));
}

void drawSnek() {
    attron(COLOR_PAIR(SNAKE_COLOR));

    peakTopN(snekStack, snekBodySegments, snekLength+1);
    struct Vector2 point;
    struct Vector2 screenPos;
    int i;
    
    for(i=0; i<snekLength; i++) {
        point = snekBodySegments[i];
        screenPos = gridToScreen(point.x, point.y);
        mvaddch(screenPos.y, screenPos.x, ' ');
        mvaddch(screenPos.y, screenPos.x+1, ' ');
    }

    attroff(COLOR_PAIR(SNAKE_COLOR));

    if (snekLength == snekTargetLength) {
        point = snekBodySegments[snekLength];
        screenPos = gridToScreen(point.x, point.y);
        mvaddch(screenPos.y, screenPos.x, ' ');
        mvaddch(screenPos.y, screenPos.x+1, ' ');
    }
}

void drawFood() {
    attron(COLOR_PAIR(FOOD_COLOR));

    struct Vector2 screenPos = gridToScreen(foodPos.x, foodPos.y);
    mvaddch(screenPos.y, screenPos.x, '$');
    mvaddch(screenPos.y, screenPos.x+1, '$');

    attroff(COLOR_PAIR(FOOD_COLOR));
}

struct Vector2 gridToScreen(int x, int y) {
    struct Vector2 output;
    output.x = x * 2 + 1;
    output.y = y + 1;

    return output;
}

bool moveFood() {
    int tryCount = 0;
    int i;
    struct Vector2 newFoodPos;
    while(tryCount < 1000) {
        newFoodPos.y = randInRange(1, GRID_HEIGHT-1);
        newFoodPos.x = randInRange(1, GRID_WIDTH-1);

        if (!pointInBody(newFoodPos.x, newFoodPos.y)) {
            foodPos.x = newFoodPos.x;
            foodPos.y = newFoodPos.y;
            return true;
        }

        tryCount++;
    }

    return false;
}

bool pointInBody(int x, int y) {
    for(int i=0; i<snekLength; i++) {
        if (snekBodySegments[i].x == x && snekBodySegments[i].y == y) {
            return true;
        }
    }

    return false;
}

int randInRange(int min, int max) {
    double factor = (double)rand() / (RAND_MAX+1u);

    return (int)(factor * (double)(max - min) + min);
}

void cleanup() {
    cleanupStack(snekStack);
    free(snekBodySegments);
}
