#include "Vnpc.h"
#include "verilated.h"
#include "verilated_fst_c.h"
#include <random>
// #include <nvboard.h>

#define MEM_SIZE 1024*1024*128
#define GPR_SIZE 32
#define CONFIG_MBASE 0x80000000
#define RESET_VECTOR 0x80000000
typedef uint32_t word_t;
typedef uint32_t sword_t;
typedef uint32_t vaddr_t;

static uint8_t pmem[MEM_SIZE]={};
static word_t gpt[GPR_SIZE]={};

static inline word_t vaddr2paddr(word_t vaddr) {
    return vaddr-CONFIG_MBASE;
}

word_t pmemRead(vaddr_t vaddr) {
    return *(word_t *)&pmem[vaddr2paddr(vaddr)];
}

void pmemWrite(vaddr_t vaddr,word_t data) {
    pmem[vaddr2paddr(vaddr)] = data;
}


// void nvboard_bind_all_pins(Vps2Display *topModule);

// 电路更新
void moveFroward(Vnpc *target ,int steps){
    for(int j=0;j<steps;++j){
        target->clock = 1;
        target->eval();
        target->clock = 0;
        target->eval();
    }
}

static const uint32_t img [] = {
    0x00000297,  // auipc t0,0
    0x00A00093,  // addi x1, x0, 10
    0x00028823,  // sb  zero,16(t0)
    0xFFB00113,  // addi x2, x0, -5
    0x00000297,  // auipc t0,0
    0x01408193,  // addi x3, x1, 20
    0x0102c503,  // lbu a0,16(t0)
    0x00000513,  // addi x10, x0, 0
    0x00100073,  // ebreak (used as nemu_trap)
    0xdeadbeef,  // some data
  };

int main(int argc, char **argv) {
    memcpy(pmem, img, sizeof(img));
    // nvboard_init();

    // 声明verilator运行时上下文对象
    VerilatedContext *contextP = new VerilatedContext;
    // 如有输入运行时参数，则指定运行时参数
    contextP->commandArgs(argc, argv);
    contextP->traceEverOn(true);
    VerilatedFstC *waveTracer = new VerilatedFstC;

    // 将顶部模板引入上下文中运行
    Vnpc *top = new Vnpc{contextP};
    // 绑定模块引脚
    // nvboard_bind_all_pins(top);


    int timeCount = 0;
    top->trace(waveTracer, 20);
    waveTracer->open("build/topWave.fst");
    // 给电路进行复位
    top->reset = 1;
    // 持续3个时钟周期
    for (int i = 0; i < 3; i++) {
        moveFroward(top,1);
        waveTracer->dump(timeCount++);
    }
    // 将复位信号归位
    top->reset = 0;
    top->eval();

    int i=0;
    // 如果运行未完成则一直运行
    while (i<10) {
        top->io_inst=pmemRead(top->io_pc);
        moveFroward(top,1);
        waveTracer->dump(timeCount++);
        // nvboard_update();
        i+=1;
    }

    // nvboard_quit();
    waveTracer->close();
    delete waveTracer;
    delete top;
    delete contextP;
    return 0;
}