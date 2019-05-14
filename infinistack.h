// Inspired by https://www.geeksforgeeks.org/stack-data-structure-introduction-program/
// Stack of Vector2's that will roll over continuously when pushed or peaked.

#ifndef INFINISTACK_H
    #define INFINISTACK_H

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

    // Create a new stack
    struct Infinistack* createStack(unsigned capacity);

    // Push a position to the top of the stack
    void push(struct Infinistack* stack, int x, int y);
    
    // Get `count` positions in the `output` array
    void peakTopN(struct Infinistack* stack, struct Vector2* output, int count);

    // Release any allocated memory
    void cleanupStack(struct Infinistack *stack);
#endif