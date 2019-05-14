#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include "csnek.h"
#include "infinistack.h"

struct timeval lastRenderFrame, lastStepFrame, thisFrameTime;
static enum GridState gridSquares[GRID_HEIGHT * GRID_WIDTH];
int lastKey;
char blankRow[GRID_WIDTH+1];
char statusBuffer[GRID_WIDTH];
struct Infinistack* snekStack;
enum Direction heading = UP;

int snekLength = 2;
int snekTargetLength = 5;
int running = 1;

int main(int argc, char *argv[]) {
    unsigned uDelta;
    
    initscr();
    raw();
    noecho();
    curs_set(0);
    nodelay(stdscr, true);
    keypad(stdscr, true);
    
    initSnek();

    drawWalls();

    while(running) {
        updateInput();
        gettimeofday(&thisFrameTime, NULL);

        
        if (uSecondsAfter(lastStepFrame, thisFrameTime) >= U_DELTA_STEP) {
            stepSnek();
        }
        
        if (uSecondsAfter(lastRenderFrame, thisFrameTime) > U_DELTA_RENDER) {
            renderTick();
        }
    }

    nodelay(stdscr, false);
    getch();

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
            running = 0;
            break;
        case KEY_LEFT:
        case 'a':
            heading = LEFT;
            break;
        case KEY_UP:
        case 'w':
            heading = UP;
            break;
        case KEY_RIGHT:
        case 'd':
            heading = RIGHT;
            break;
        case KEY_DOWN:
        case 's':
            heading = DOWN;
            break;
        default:
            break;
            // printw("pushed key: %d\n", ch);
        }
    }
}

void renderTick() {
    gettimeofday(&lastRenderFrame, NULL);

    drawSnek();
    refresh();
}

void stepSnek() {
    gettimeofday(&lastStepFrame, NULL);

    int i;
    struct Vector2 nextPos;
    struct Vector2* body[snekLength];

    peakTopN(snekStack, body, snekLength);
    memcpy(&nextPos, body[0], sizeof(struct Vector2));

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
    default:
        break;
    }

    move(GRID_HEIGHT + 2, 1);
    printw("next: %d, %d head: %d, %d", nextPos.x, nextPos.y, body[0]->x, body[0]->y);

    if (nextPos.x == 0 || nextPos.x == GRID_WIDTH || nextPos.y == 0 || nextPos.y == GRID_HEIGHT-1) {
        running = 0;
    }

    push(snekStack, nextPos.x, nextPos.y);

    if (snekLength < snekTargetLength) {
        snekLength++;
    }
}

unsigned uSecondsAfter(struct timeval refTime, struct timeval now) {
    return (now.tv_sec - refTime.tv_sec) * 1e6f + (now.tv_usec - refTime.tv_usec);
}

void initSnek() {
    snekStack = createStack(MAX_SNAKE_LENGTH);
    push(snekStack, 10, 10);
    push(snekStack, 11, 10);
}

void drawWalls() {
    int i;
    char topLine[GRID_WIDTH*2+1];
    char midLine[GRID_WIDTH*2+1];
    for(i=0; i<GRID_WIDTH*2; i++) {
        topLine[i] = '=';
        midLine[i] = (i==1 || i == GRID_WIDTH*2-1) ? '|' : ' ';
    }
    topLine[GRID_WIDTH*2] = '\0';
    midLine[GRID_WIDTH*2] = '\0';

    for (i=0; i<GRID_HEIGHT; i++) {
        move(i+1, 1);
        if (i == 0 | i == GRID_HEIGHT - 1) {
            printw("%s", topLine);
        } else {
            printw("%s", midLine);
        }
    }
}

void drawSnek() {
    struct Vector2* points[snekLength+1];
    struct Vector2* lastPoint;
    int i;
    peakTopN(snekStack, points, snekLength+1);
    
    
    for(i=0; i<snekLength; i++) {
        struct Vector2* point = points[i];
        mvaddch(point->y + 1, point->x * 2 + 1, snekChar(i==0));
    }

    lastPoint = points[snekLength];
    mvaddch(lastPoint->y + 1, lastPoint->x * 2 + 1, ' ');
}

char snekChar(int isHead) {
    if (isHead) {
        switch (heading) {
        case UP:
            return 'A';
            break;
        case RIGHT:
            return '>';
            break;
        case DOWN:
            return 'V';
        case LEFT:
            return '<';
        }
    }

    return 'O';
}