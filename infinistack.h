// Heavily inspired https://www.geeksforgeeks.org/stack-data-structure-introduction-program/

#ifndef INFINISTACK_H
    #define INFINISTACK_H

    #define MAX_SNAKE_LENGTH 100

    struct Vector2 {
        int x;
        int y;
    };

    struct Vector2* createVector2(int x, int y);

    struct Infinistack {
        int top;
        unsigned capacity;
        struct Vector2 *positions;
    };

    struct Infinistack* createStack(unsigned capacity);
    void push(struct Infinistack* stack, int x, int y);
    void peakTopN(struct Infinistack* stack, struct Vector2* output[], int count);
    void peakTop(struct Infinistack* stack, struct Vector2* output);
    // struct Vector2* pop(struct Infinistack* stack);
#endif