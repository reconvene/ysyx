#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static void putCharDirectly(char ch, char *buf, size_t *position) {
  putch(ch);
  *position +=1;
}

static void putCharIntoBuf(char ch, char *buf, size_t *position) {
  buf[*position] = ch;
  *position +=1;
}

static void generalVAParser(void (*putChar)(char ch, char *buf, size_t *position),const char *fmt, char *buf, size_t *position, va_list ap) {
  size_t i=0;

  // 遍历格式化字符串
  while (fmt[i] != '\0') {
    // 如果没有遇到%则不进行匹配
    if (fmt[i] != '%') {
      putChar(fmt[i++], buf, position);
      continue;
    }

    // 遇到%则对后面的符号进行匹配
    i++;

    // 声明修饰符
    _Bool fillZero=0;
    _Bool longType=0;
    // 声明宽度
    size_t width=0;

    if (fmt[i]=='0') {
      fillZero=1;
      i++;
    }

    while (fmt[i] >= '0' && fmt[i] <= '9') {
      width=width*10+fmt[i++]-'0';
    }

    if (fmt[i]=='l') {
      longType=1;
      i++;
    }

    switch (fmt[i]) {
        // 十进制数字
      case 'd': {
        char currentStr[32]={0};
        // 转数字为字符串，并更新写索引

        int strLen=longType?myitoa(currentStr,va_arg(ap, long)):myitoa(currentStr,va_arg(ap, int));
        for (int lackingWidth=strLen-width;lackingWidth<0;lackingWidth++) putChar(fillZero?'0':' ', buf, position);
        for (int j=0; j<strLen; j++) putChar(currentStr[j], buf, position);
        break;
      }
        // 无符号十进制数字
      case 'u': {
        char currentStr[32]={0};
        // 转数字为字符串，并更新写索引

        int strLen=longType?myitoa(currentStr,va_arg(ap, unsigned long)):myitoa(currentStr,va_arg(ap, unsigned int));
        for (int lackingWidth=strLen-width;lackingWidth<0;lackingWidth++) putChar(fillZero?'0':' ', buf, position);
        for (int j=0; j<strLen; j++) putChar(currentStr[j], buf, position);
        break;
      }
        // 十六进制数字
      case 'x':
      case 'X':
        {
        char currentStr[9]={0};
        // 转数字为字符串，并更新写索引
        int strLen=longType?htoa(currentStr,va_arg(ap, unsigned long)):htoa(currentStr,va_arg(ap, unsigned int));
        for (int lackingWidth=strLen-width;lackingWidth<0;lackingWidth++) putChar(fillZero?'0':' ', buf, position);
        for (int j=0; j<strLen; j++) putChar(currentStr[j], buf, position);
        break;
      }

        // 字符串
      case 's': {
        // 取出字符串并遍历，然后更新写索引
        char *currentStr = va_arg(ap, char *);
        int strLen=strlen(currentStr);
        for (int lackingWidth=strLen-width;lackingWidth<0;lackingWidth++) putChar(' ', buf, position);
        for (int j=0;j<strLen;j++) putChar(currentStr[j], buf, position);
        break;
      }

        // 字符
      case 'c': {
        // 取出字符串并遍历，然后更新写索引
        char currentChar = (char)va_arg(ap, int);
        for (int lackingWidth=1-width;lackingWidth<0;lackingWidth++) putChar(' ', buf, position);
        putChar(currentChar, buf, position);
        break;
      }

        // 默认/未定义情况
      default:
        panic("invalid fmt format");
        /*putChar('%', buf, position);
        putChar(fmt[i], buf, position);*/
        break;
    }

    // 更新读索引
    i++;
  }

  putChar('\0', buf, position);
  *position -=1;
}

int printf(const char *fmt, ...) {
  // 定义va_list
  va_list ap;
  va_start(ap,fmt);
  size_t position=0;
  generalVAParser(putCharDirectly,fmt,NULL,&position,ap);
  // 释放va_list
  va_end(ap);
  return position;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

// 输出格式化字符串到目标字符数组
int sprintf(char *out, const char *fmt, ...) {
  // 定义va_list
  size_t position=0;
  va_list ap;
  va_start(ap,fmt);
  generalVAParser(putCharIntoBuf,fmt,out,&position,ap);
  // 释放va_list
  va_end(ap);
  return position;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
