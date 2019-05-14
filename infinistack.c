#include <stdio.h>
#include <stdlib.h>
#include "infinistack.h"

struct Infinistack* createStack(unsigned capacity) {
    struct Infinistack* stack = (struct Infinistack*) malloc(sizeof(struct Infinistack));
    stack->capacity = capacity;
    stack->top = capacity;
    stack->positions = calloc(capacity, sizeof(struct Vector2));

    return stack;
};

void push(struct Infinistack* stack, int x, int y) {
    stack->top = stack->top == 0 ? stack->capacity - 1 : stack->top - 1;
    struct Vector2* pos = &stack->positions[stack->top];
    pos->x = x;
    pos->y = y;
}

void peakTopN(struct Infinistack* stack, struct Vector2* output, int count) {
    int i;
    int idx = stack->top;
    for(i=0; i<count; i++) {
        output[i] = stack->positions[idx];

        idx = (idx == stack->capacity-1) ? 0 : idx+1;
    }
}

void cleanupStack(struct Infinistack *stack) {
    free(stack->positions);
    free(stack);
}
