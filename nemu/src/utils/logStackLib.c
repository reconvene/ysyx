#include <common.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <utils/logStackLib.h>



_Noreturn static void echoError(char *errorMessage) {
    if (errno) {
        perror(errorMessage);
        exit(1);
    }
    printf("ERROR:%s\n", errorMessage);
    exit(1);
}

// 创建栈，指定初始大小
logStack *createLogStack(int initialEleCount) {
    if(initialEleCount<1){
        printf("The initialEleCount must be larger than zero\n");
        return NULL;
    }

    logStack *newLogStack = malloc(sizeof(logStack));
    if (!newLogStack) echoError(MALLOC_ERROR);

    newLogStack->logArr = calloc(initialEleCount, sizeof(char *));
    if (!newLogStack->logArr) {
        free(newLogStack);
        echoError(MALLOC_ERROR);
    }

    newLogStack->eleCount = 0;
    newLogStack->capacity=initialEleCount;
    return newLogStack;
}

// 销毁栈
void destroyLogStack(logStack *inputLogStack) {
    for (int i = 0; i < inputLogStack->eleCount; i++) {
      free(inputLogStack->logArr[i]);
    }
    free(inputLogStack->logArr);
    free(inputLogStack);
}

// 压栈
void shiftLogStack(logStack *inputLogStack, char *fmt, ...) {
    if (inputLogStack->eleCount+1>inputLogStack->capacity) {
        char **newLogArr = realloc(inputLogStack->logArr,2 * inputLogStack->capacity * sizeof(char *));
        if (!newLogArr) {
          free(inputLogStack->logArr);
          free(inputLogStack);
          echoError(MALLOC_ERROR);
        }
        inputLogStack->logArr=newLogArr;
        inputLogStack->capacity*=2;
    }

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

    memmove(inputLogStack->logArr+1, inputLogStack->logArr,inputLogStack->eleCount* sizeof(char *));
    inputLogStack->logArr[0] = currentLog;
    inputLogStack->eleCount += 1;
}

// 出栈
void unshiftLogStack(logStack *inputLogStack, int adjustMemory) {
    if ((inputLogStack->eleCount - 1 <= 0 && adjustMemory) || inputLogStack->eleCount-1 < 0) {
        // printf("Too few elements to execute this function");
        return;
    }

    free(inputLogStack->logArr[0]);
    inputLogStack->eleCount -= 1;
    memmove(inputLogStack->logArr, inputLogStack->logArr+1, inputLogStack->eleCount* sizeof(char *));

    if (adjustMemory) {
        char **newLogArr = realloc(inputLogStack->logArr, inputLogStack->eleCount * sizeof(char *));
        if (!newLogArr) {
          free(inputLogStack->logArr);
          free(inputLogStack);
          echoError(MALLOC_ERROR);
        }
        inputLogStack->logArr = newLogArr;
        inputLogStack->capacity=inputLogStack->eleCount;
    }
}

// 列出栈内所有元素
void listLogStack(logStack *inputLogStack) {
    if (inputLogStack->eleCount <= 0) {
        printf("Too few elements to list logs\n");
        return;
    }

    for (int i = 0; i < inputLogStack->eleCount; ++i) printf("#%d  %s\n", i, inputLogStack->logArr[i]);
}

// 查看栈中第一个元素
char *peekLogStack(logStack *inputLogStack) {
    return inputLogStack->logArr[0];
}