#ifndef INSTRINGBUFFERLIB_H
#define INSTRINGBUFFERLIB_H

#define LOG_BUFFER_SIZE 50
#include <stdint.h>

typedef struct {
    char **logArr;
    int capacity;
    int readIndex;
    int writeIndex;
} logRingBuffer;

#define MALLOC_ERROR "There is something wrong when you try to use the function malloc()"
#define REALLOC_ERROR "There is something wrong when you try to use the function realloc()"

logRingBuffer *createLogRingBuffer(int bufferSize);
void destroyLogRingBuffer(logRingBuffer *inputLogRingBuffer);
void writeLogRingBuffer(logRingBuffer *inputLogRingBuffer, uint32_t logSize, const char *fmt, ...) __attribute__((format(printf, 3, 4)));
char **obtainLogRingBuffer(logRingBuffer *inputLogRingBuffer, uint64_t readLength);
void printLogRingBuffer(logRingBuffer *inputLogRingBuffer, uint64_t printLength);

#endif //INSTRINGBUFFERLIB_H
