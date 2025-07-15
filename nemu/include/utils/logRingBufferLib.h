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

logRingBuffer *createLogRingBuffer(const int bufferSize);
void destroyLogRingBuffer(logRingBuffer *inputLogRingBuffer);
void writeLogRingBuffer(logRingBuffer *buf, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
char **obtainLogRingBuffer(logRingBuffer *inputLogRingBuffer, const uint64_t readLength);
void printLogRingBuffer(logRingBuffer *inputLogRingBuffer, uint64_t printLength);

#endif //INSTRINGBUFFERLIB_H
