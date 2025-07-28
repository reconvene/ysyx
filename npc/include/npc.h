//
// Created by admin123 on 7/17/25.
//

#ifndef NPC_H
#define NPC_H
#include <macro.h>

extern _Bool batchMode;
extern word_t *gprBaseAddress;
extern word_t imageSize;

void diagnoseError();
void listReg();
word_t obtainRegValue(const char *s, _Bool *success);
uint8_t *vaddr2pptr(word_t vaddr);
word_t pmemRead(vaddr_t vaddr, uint8_t len);
sword_t pmemSignedRead(vaddr_t vaddr, uint8_t len);
void pmemWrite(vaddr_t vaddr, word_t data, uint8_t len);
void executeN(word_t step);

#endif //NPC_H
