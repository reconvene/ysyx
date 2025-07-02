#include <assert.h>
#include <stdio.h>
#include <stdint.h>

// 定义寄存器数量和内存数量
#define regNum 4
#define memNum 16

// 定义不同类型的指令结构
typedef union {
    struct {
        uint8_t rs: 2;
        uint8_t rt: 2;
        uint8_t opcode: 4;
    } rType;

    struct {
        uint8_t addr: 4;
        uint8_t opcode: 4;
    } mType;

    uint8_t inst;
} instTree;

// R类型指令解码
#define rDecoder(instAddr) \
    uint8_t rt=(instAddr).rType.rt; \
    uint8_t rs=(instAddr).rType.rs

// M类型指令解码
#define mDecoder(instAddr) \
    uint8_t addr=(instAddr).mType.addr

// 初始化寄存器、内存、pc和结束标识符
uint8_t reg[regNum] = {0};
uint8_t mem[memNum] = {
    0b11100111,  // load  7#     | R[0] <- M[y]
    0b00000100,  // mov   r1, r0 | R[1] <- R[0]
    0b11100110,  // load  6#     | R[0] <- M[x]
    0b00010001,  // add   r0, r1 | R[0] <- R[0] + R[1]
    0b11111000,  // store 8#     | M[z] <- R[0]
    0b10100000,  // halt
    0b00001000,  // x = 8
    0b00001110,  // y = 14
    0b00000000,  // z = 0
};
uint8_t pc = 0;
uint8_t endFlag = 0;

// 执行指令
void execOnce() {
    // 读取指令
    instTree currentInst = {.inst = mem[pc]};
    // 根据操作码做对应操作
    switch (currentInst.rType.opcode) {
        case 0b0000:{
            rDecoder(currentInst);
            reg[rt] = reg[rs];
            break;
        }
        case 0b0001: {
            rDecoder(currentInst);
            /*printf("%d\n", reg[rs]);
            printf("%d\n", reg[rt]);
            printf("%d\n", pc);*/
            reg[rt] = reg[rs] + reg[rt];
            break;
        }
        case 0b1110: {
            mDecoder(currentInst);

            reg[0] = mem[addr];

            break;
        }
        case 0b1111: {
            mDecoder(currentInst);
            mem[addr] = reg[0];
            break;
        }
        case 0b1010: {
            endFlag = 1;
            break;
        }
        default: {
            printf("pc:%d\nThe wrong opcode:%x\n",pc,currentInst.rType.opcode);
            endFlag = 1;
            return;
        }
    }
    // 更新pc
    pc++;
}

int main() {
    while (!endFlag) {
        execOnce();
    }

    assert(8+14==mem[8]);
    printf("8+14=%d\n",mem[8]);
    return 0;
}