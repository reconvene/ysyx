#ifndef ELFPARSER_H
#define ELFPARSER_H
#include <stdint.h>

typedef struct {
  char *name;
  uint32_t pc;
  uint32_t size;
} funcInfo;

char *readELF(const char *filename);
void obtainTables(char *elfData);
funcInfo *lookupFunctions(uint32_t pc);
void destroyElfParser();

#endif  // ELFPARSER_H
