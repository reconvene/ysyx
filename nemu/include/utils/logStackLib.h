#ifndef CLEARN_STACKLIB_H
#define CLEARN_STACKLIB_H
#define STACK_SIZE 16

typedef struct {
    char **logArr;
    int eleCount;
    int capacity;
} logStack;

#define MALLOC_ERROR "There is something wrong when you try to use the function malloc()"
#define REALLOC_ERROR "There is something wrong when you try to use the function realloc()"

logStack *createLogStack(int initialEleCount);

void destroyLogStack(logStack *inputLogStack);

void shiftLogStack(logStack *inputLogStack, char *fmt, ...) __attribute__((format(printf, 2, 3)));

void unshiftLogStack(logStack *inputLogStack, int adjustMemory);

void listLogStack(logStack *inputLogStack);

char *peekLogStack(logStack *inputLogStack);

#endif //CLEARN_STACKLIB_H