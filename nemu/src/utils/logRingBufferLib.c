#include <common.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils/logRingBufferLib.h>


_Noreturn static void echoError(char *errorMessage) {
    if (errno) {
        perror(errorMessage);
        exit(1);
    }
    printf("ERROR:%s\n", errorMessage);
    exit(1);
}

// 创建环形缓冲区
logRingBuffer *createLogRingBuffer(const int bufferSize) {
    logRingBuffer *newLogRingBuffer = malloc(sizeof(logRingBuffer));
    if (!newLogRingBuffer) {
        echoError(MALLOC_ERROR);
    }

    newLogRingBuffer->logArr = calloc(bufferSize,sizeof(char *));
    if (!newLogRingBuffer->logArr) {
        free(newLogRingBuffer);
        echoError(MALLOC_ERROR);
    }

    newLogRingBuffer->capacity = bufferSize;
    newLogRingBuffer->readIndex = 0;
    newLogRingBuffer->writeIndex = 0;

    return newLogRingBuffer;
}

// 销毁环形缓冲区
void destroyLogRingBuffer(logRingBuffer *inputLogRingBuffer) {
    for (int i = 0; i < inputLogRingBuffer->capacity; i++) {
      free(inputLogRingBuffer->logArr[i]);
    }
    free(inputLogRingBuffer->logArr);
    free(inputLogRingBuffer);
}

// 写入环形缓冲区，写满则覆写旧数据
void writeLogRingBuffer(logRingBuffer *inputLogRingBuffer, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int requiredLen = vsnprintf(NULL, 0, fmt, args);
  va_end(args);

  char *currentLog = malloc(requiredLen + 1); // +1 for null terminator
  if (!currentLog) {
    echoError(MALLOC_ERROR);
  }

  va_start(args, fmt);
  vsprintf(currentLog, fmt, args);
  va_end(args);

  free(inputLogRingBuffer->logArr[inputLogRingBuffer->writeIndex]);
  inputLogRingBuffer->logArr[inputLogRingBuffer->writeIndex++] = currentLog;
  inputLogRingBuffer->writeIndex%=inputLogRingBuffer->capacity;
}

// 读取指定长度的指令
char **obtainLogRingBuffer(logRingBuffer *inputLogRingBuffer, const uint64_t readLength){
    char **newLogArr= malloc(readLength* sizeof(char *));
    if(!newLogArr){
        echoError(MALLOC_ERROR);
    }

    for(int i=0;i<readLength;++i) newLogArr[i]=inputLogRingBuffer->logArr[(i+inputLogRingBuffer->readIndex)%inputLogRingBuffer->capacity];

    inputLogRingBuffer->readIndex = (inputLogRingBuffer->readIndex + readLength) % inputLogRingBuffer->capacity;
    // printf("%d\n",inputLogRingBuffer->readIndex);
    // printf("%lu\n", strlen(newLogArr));
    return newLogArr;
}

// 打印指定长度的指令
void printLogRingBuffer(logRingBuffer *inputLogRingBuffer, uint64_t printLength){

  for(int i=0;i<printLength;++i) printf("%s\n",inputLogRingBuffer->logArr[(i+inputLogRingBuffer->readIndex)%inputLogRingBuffer->capacity]);

  inputLogRingBuffer->readIndex = (inputLogRingBuffer->readIndex + printLength) % inputLogRingBuffer->capacity;
}