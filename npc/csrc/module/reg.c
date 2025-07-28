#include <reg.h>
#include <Vnpc.h>
#include <macro.h>
#include <stdio.h>
#include <string.h>

#include "npc.h"

// 定义reg别名(riscv32e)
const static uint8_t regNum = 16;
const static char *regAlias[] = {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5"
};

// 列出reg的值
void listReg() {
    printf("------------------------\n");
    for (uint8_t i = 0; i < regNum; ++i) {
        if (i % 8 == 0 && i != 0) printf("\n");
        printf("%s:0x%08X\t", regAlias[i], gprBaseAddress[i]);
    }
    printf("\n------------------------\n");
}

// 根据别名查找reg的值
word_t obtainRegValue(const char *s, _Bool *success) {
    /*// 如果获取pc则直接返回
    if (strcmp(s, "pc") == 0) {
        *success = 1;
        return top->io_pc;
    }*/
    // 遍历寄存器，找到目标值
    for (uint8_t i = 0; i < regNum; ++i) {
        if (strcmp(s, regAlias[i]) == 0) {
            *success = 1;
            return gprBaseAddress[i];
        }
    }
    *success = 0;
    return 0;
}