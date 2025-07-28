#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;
static const char hexMap[] = "0123456789ABCDEF";
int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

int abs(int x) {
  return (x < 0 ? -x : x);
}

int atoi(const char* nptr) {
  int x = 0;
  while (*nptr == ' ') { nptr ++; }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr ++;
  }
  return x;
}

// 整数转字符串，最大支持12位有符号十进制(包括终止符)
int itoa(char *targetStr,int x) {
  // 定义缓存字符串和写索引
  uint8_t targetPosition=0;
  char tmpBuffer[12];
  uint8_t tmpPosition = 0;

  // 如果为0，则直接返回
  if (x == 0) {
    targetStr[0]='0';
    targetStr[1]='\0';
    return 1;
  }

  // 如果小于零，则写入负号，并对x取负
  if (x<0) {
    targetStr[0]='-';
    x=-x;
    targetPosition++;
  }

  // 从低位到高位将数字转成字符串写入缓存数组
  while (x!=0) {
      tmpBuffer[tmpPosition]='0'+x%10;
      x/=10;
      tmpPosition++;
  }

  // 从后往前遍历缓存数组并写入目标字符串
  for (uint8_t i=tmpPosition; i>0; i--) {
    targetStr[targetPosition++]=tmpBuffer[i-1];
  }
  // 添加终止符
  targetStr[targetPosition]='\0';
  // 返回有效字符写入数
  return targetPosition;
}

// 16进制转字符串，最大支持9位有符号16进制(包括终止符)
int htoa(char *targetStr,int x) {
  char tmpBuffer[9];  // uint32_t 最大8位16进制数
  uint8_t tmpPosition = 0;
  uint8_t targetPosition=0;

  if (x == 0) {
    targetStr[0] = '0';
    targetStr[1] = '\0';
    return 1;
  }

  while (x != 0) {
    tmpBuffer[tmpPosition++] = hexMap[x % 16];
    x /= 16;
  }

  for (uint8_t i=tmpPosition; i>0; i--) {
    targetStr[targetPosition++]=tmpBuffer[i-1];
  }
  targetStr[targetPosition]='\0';

  return tmpPosition;
}

void *malloc(size_t size) {
  // On native, malloc() will be called during initializaion of C runtime.
  // Therefore do not call panic() here, else it will yield a dead recursion:
  //   panic() -> putchar() -> (glibc) -> malloc() -> panic()
#if !(defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__))
  void *oldPosition=heap.start;
  uintptr_t newPosition=ROUNDUP(heap.start+size,8);

  if (newPosition>=(uintptr_t)heap.end) return NULL;
  heap.start = (void *)newPosition;
  return oldPosition;
#endif
}

void free(void *ptr) {
}

#endif
