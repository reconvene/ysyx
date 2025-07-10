#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <stddef.h>

#include "../../am/include/am.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  // 定义va_list
  size_t i=0;
  va_list ap;
  va_start(ap,fmt);

  // 遍历格式化字符串
  while (fmt[i] != '\0') {
    // 如果没有遇到%则不进行匹配
    if (fmt[i] != '%') {
      putch(fmt[i++]);
      continue;
    }

    // 遇到%则对后面的符号进行匹配
    i++;
    switch (fmt[i]) {
      // 十进制数字
      case 'd': {
        char currentStr[32]={0};
        // 转数字为字符串，并更新写索引
        int strLen=itoa(currentStr,va_arg(ap, int));
        for (int j=0; j<strLen; j++) putch(currentStr[j]);
        break;
      }

        // 字符串
      case 's': {
        // 取出字符串并遍历，然后更新写索引
        char *currentStr = va_arg(ap, char *);

        while (*currentStr!='\0') {
          putch(*currentStr++);
        }
        break;
      }

        // 默认/未定义情况
      default:
        putch('%');
        putch(fmt[i]);
        break;
    }

    // 更新读索引
    i++;
  }

  // 释放va_list，给输出字符串添加终止符，返回写入的有效字符数
  va_end(ap);
  return i;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

// 输出格式化字符串到目标字符数组
int sprintf(char *out, const char *fmt, ...) {
  // 定义读写索引和va_list
  size_t i=0;
  size_t outPosition=0;
  va_list ap;
  va_start(ap,fmt);

  // 遍历格式化字符串
  while (fmt[i] != '\0') {
    // 如果没有遇到%则不进行匹配
    if (fmt[i] != '%') {
      out[outPosition++] = fmt[i++];
      continue;
    }

    // 遇到%则对后面的符号进行匹配
    i++;
    switch (fmt[i]) {
      // 十进制数字
      case 'd': {
        // 转数字为字符串，并更新写索引
        outPosition+=itoa(out+outPosition,va_arg(ap, int));
        break;
      }

      // 字符串
      case 's': {
        // 取出字符串并遍历，然后更新写索引
        char *currentStr = va_arg(ap, char *);
        while (*currentStr!='\0') {
          out[outPosition++] = *currentStr++;
        }
        break;
      }

      // 默认/未定义情况
      default:
        // 写入%和匹配位，并更新写索引
        out[outPosition++] = '%';
        out[outPosition++] = fmt[i];
        break;
    }

    // 更新读索引
    i++;
  }

  // 释放va_list，给输出字符串添加终止符，返回写入的有效字符数
  va_end(ap);
  out[outPosition]='\0';
  return outPosition;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
