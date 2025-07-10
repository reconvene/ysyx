#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
    size_t len = 0;
    while (s[len] != '\0') len++;

    return len;
}

char *strcpy(char *dst, const char *src) {
    char *p = dst;
    while (*src != '\0') *p++ = *src++;
    *p = '\0';

    return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
    char *p = dst;
    size_t i = 0;

    for (; i < n; i++) {
        if (src[i] == '\0') break;
        p[i] = src[i];
    }

    for (; i < n; i++) p[i] = '\0';

    return dst;
}

char *strcat(char *dst, const char *src) {
    char *p = dst;

    while (*p != '\0') p++;
    while (*src != '\0') *p++ = *src++;
    *p = '\0';

    return dst;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 == *s2 && (*s1 != '\0' || *s2 != '\0')) {
        s1++;
        s2++;
    }
    return *(uint8_t *) s1 - *(uint8_t *) s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    size_t i = 0;
    while (s1[i] == s2[i] && (s1[i] != '\0' || s2[i] != '\0') && i < n - 1) i++;

    return (uint8_t) s1[i] - (uint8_t) s2[i];
}

void *memset(void *s, int c, size_t n) {
    uint8_t *byteS = s;
    for (size_t i = 0; i < n; i++) {
        byteS[i] = c;
    }
    return s;
}

void *memmove(void *dst, const void *src, size_t n) {
    uint8_t *dstByte = dst;
    const uint8_t *srcByte = src;

    if (srcByte < dstByte && srcByte + n > dstByte) {
        for (size_t i = n; i > 0; i--) dstByte[i - 1] = srcByte[i - 1];
        return dst;
    }

    for (size_t i = 0; i < n; i++) dstByte[i] = srcByte[i];
    return dst;
}

void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *dstByte = dst;
    const uint8_t *srcByte = src;

    for (size_t i = 0; i < n; i++) dstByte[i] = srcByte[i];
    return dst;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    size_t i = 0;
    const uint8_t *s1Byte = s1;
    const uint8_t *s2Byte = s2;
    while (i < n - 1 && s1Byte[i] == s2Byte[i]) i++;

    return s1Byte[i] - s2Byte[i];
}

#endif
