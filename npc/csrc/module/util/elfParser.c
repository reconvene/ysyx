#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <macro.h>
#include <utils/elfParser.h>

// 初始化读取内容
static Elf32_Ehdr *elfHeader=NULL;
static Elf32_Shdr *elfSectionHeader=NULL;
static char *elfSectionHeaderStringTable=NULL;
static Elf32_Sym *elfSymbolTable=NULL;
static char *elfStringTable=NULL;

// 初始化函数查找表
static funcInfo *funcTable = NULL;
static uint32_t funcTableCount=0;

// 读取elf文件
char *readELF(const char *filename) {
  // 读取elf文件
  FILE *elfFile = fopen(filename, "r");
  if (!elfFile) {
    panic("The input ELF file does not exist or lacks sufficient permissions");
  }
  // 获取文件大小
  struct stat elfStat;
  fstat(fileno(elfFile), &elfStat);
  if (elfStat.st_size <= 0) {
    panic("Failed to read ELF file size");
  }

  // 获取elf文件内容
  char *elfData = (char *)malloc(elfStat.st_size);
  if (!elfData) {
    panic("Failed to allocate memory for ELF data");
  }
  uint32_t readSize=fread(elfData, sizeof(char), elfStat.st_size, elfFile);
  assert(readSize == elfStat.st_size);
  printf("load ELF file successfully\n");
  fclose(elfFile);

  return elfData;
}

// 获取elf各分区内容,并初始化函数查找表
void obtainTables(char *elfData) {
  // 初始化符号计数
  uint32_t symbolTableCount=0;
  // 获取各分区数据
  elfHeader=(Elf32_Ehdr *)elfData;
  elfSectionHeader=(Elf32_Shdr *)(elfData + elfHeader->e_shoff);
  elfSectionHeaderStringTable=elfData+elfSectionHeader[elfHeader->e_shstrndx].sh_offset;
  uint32_t sectionNum=elfHeader->e_shnum;

  // 获取.strtab和.symtab
  for (uint32_t i=0; i<sectionNum; i++) {
    if (strcmp(elfSectionHeaderStringTable+elfSectionHeader[i].sh_name, ".strtab") == 0) {
      elfStringTable=elfData + elfSectionHeader[i].sh_offset;
    }
    if (elfSectionHeader[i].sh_type == SHT_SYMTAB) {
      elfSymbolTable=(Elf32_Sym *)(elfData + elfSectionHeader[i].sh_offset);
      symbolTableCount=elfSectionHeader[i].sh_size / elfSectionHeader[i].sh_entsize;
    }
  }

  // 初始化函数查找表
  funcTable=(funcInfo *)malloc(symbolTableCount * sizeof(funcInfo));
  if (!funcTable) {
    panic("Failed to allocate memory for function table");
  }

  // 往表中填入函数相关数据
  for (uint32_t i=0; i<symbolTableCount; i++) {
    if (ELF32_ST_TYPE(elfSymbolTable[i].st_info)==STT_FUNC) {
      funcTable[funcTableCount].name=elfStringTable+elfSymbolTable[i].st_name;
      funcTable[funcTableCount].pc=elfSymbolTable[i].st_value;
      funcTable[funcTableCount++].size=elfSymbolTable[i].st_size;
    }
  }

}

// 查找函数与其相关信息
funcInfo *lookupFunctions(uint32_t pc) {
  for (uint32_t i=0; i<funcTableCount; i++) {
    if (pc>=funcTable[i].pc && pc<funcTable[i].pc+funcTable[i].size) {
      return funcTable+i;
    }
  }
  return NULL;
}

// 初始化elf解析器
void init_ELFParser(const char *elfFileName) {
  if (!elfFileName) return;
  // if (!elfFileName) panic("elfFileName is null");
  char *elfData = readELF(elfFileName);
  obtainTables(elfData);
}

// 销毁elf解析器
void destroyElfParser() {
  free(elfHeader);
  free(funcTable);
}