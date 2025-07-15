#include "Vnpc.h"
#include "verilated.h"
#include "verilated_fst_c.h"
#include <random>
#include <sys/stat.h>
// #include <nvboard.h>

#define MEM_SIZE 1024*1024*128
#define GPR_SIZE 32
#define CONFIG_MBASE 0x80000000
#define RESET_VECTOR 0x80000000
typedef uint32_t word_t;
typedef uint32_t sword_t;
typedef uint32_t vaddr_t;

static uint8_t pmem[MEM_SIZE]={};

// 虚拟地址转换
static inline word_t vaddr2paddr(word_t vaddr) {
    assert(vaddr-CONFIG_MBASE < MEM_SIZE && "Virtual address overflow");
    return vaddr-RESET_VECTOR;
}

// 无符号读取内存
word_t pmemRead(vaddr_t vaddr, uint8_t len) {
    uint32_t raw = 0;
    memcpy(&raw, &pmem[vaddr2paddr(vaddr)], len);
    switch (len) {
        case 1: return (uint8_t)raw;
        case 2: return (uint16_t)raw;
        case 4: return (uint32_t)raw;
        default: return 0;
    }
}

// 有符号读取内存
sword_t pmemSignedRead(vaddr_t vaddr, uint8_t len) {
    uint32_t raw = 0;
    memcpy(&raw, &pmem[vaddr2paddr(vaddr)], len);
    switch (len) {
        case 1: return (int8_t)raw;
        case 2: return (int16_t)raw;
        case 4: return (int32_t)raw;
        default: return 0;
    }
}

// 写入内存
void pmemWrite(vaddr_t vaddr, word_t data, uint8_t len) {
    memcpy(&pmem[vaddr2paddr(vaddr)], &data, len);
}


// void nvboard_bind_all_pins(Vps2Display *topModule);

// 电路更新
void moveForward(Vnpc *target ,int steps){
    for(int j=0;j<steps;++j){
        target->clock = 1;
        target->eval();
        target->clock = 0;
        target->eval();
    }
}

static const uint32_t img [] = {
    /*0x00000297,  // auipc t0,0
    0x00A00093,  // addi x1, x0, 10
    0x00028823,  // sb  zero,16(t0)
    0xFFB00113,  // addi x2, x0, -5
    0x00000297,  // auipc t0,0
    0x01408193,  // addi x3, x1, 20
    0x0102c503,  // lbu a0,16(t0)
    0x00000513,  // addi x10, x0, 0
    0x00100073,  // ebreak (used as nemu_trap)
    0xdeadbeef,  // some data*/
    // addi x1, x0, 10
    0x00A00093,

    // addi x2, x0, 10
    0x00A00113,

    // beq x1, x2, +8 (跳过下一条指令)
    0x00108263,

    // addi x3, x0, 11 (这条会被跳过)
    0x00B00193,

    // addi x4, x0, 12
    0x00C00213,

    // bne x3, x4, +8 (跳过下一条指令)
    0x0041C263,

    // addi x5, x0, 13 (这条会被跳过)
    0x00D00293,

    // addi x6, x0, 14
    0x00E00313,

    // blt x5, x6, +8 (跳过下一条指令)
    0x00630663,

    // addi x7, x0, 15 (这条会被跳过)
    0x00F00393,

    // bge x6, x7, +8 (跳过下一条指令)
    0x00738763,

    // ebreak (用于结束仿真)
    0x00100073,

    // some data
    0xdeadbeef
  };

// 加载映像文件
static void loadImage(const char* filename) {
    if (!filename) {
        printf("loading default image.....\n");
        memcpy(pmem, img, sizeof(img));
        return;
    }

    // 读取elf文件
    FILE *elfFile = fopen(filename, "rb");
    if (!elfFile) {
        // assert(0 && "The input ELF file does not exist or lacks sufficient permissions");
        printf("The input ELF file does not exist or lacks sufficient permissions\n");
        printf("loading default image.....\n");
        memcpy(pmem, img, sizeof(img));
        return;
    }

    // 获取文件大小
    struct stat elfStat;
    fstat(fileno(elfFile), &elfStat);
    if (elfStat.st_size <= 0) assert(0 && "Failed to read ELF file size");

    // 读取文件
    size_t readSize = fread(pmem, 1, elfStat.st_size, elfFile);
    assert(readSize == elfStat.st_size);
    fclose(elfFile);
}

int main(int argc, char **argv) {
    loadImage(argv[1]);
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
        moveForward(top,1);
        waveTracer->dump(timeCount++);
    }
    // 将复位信号归位
    top->reset = 0;
    top->eval();

    int i=0;
    // 如果运行未完成则一直运行
    while (i<13 && !contextP->gotFinish()) {
        top->io_inst=pmemRead(top->io_pc,4);
        if (top->io_readEnable) top->io_readData=top->io_readType?pmemSignedRead(top->io_readAddr,top->io_byteNum):pmemRead(top->io_readAddr,top->io_byteNum);
        if (top->io_writeEnable) pmemWrite(top->io_writeAddr,top->io_writeData,top->io_byteNum);

        moveForward(top,1);
        waveTracer->dump(timeCount++);
        if (top->io_inst == 0x0000006f) break;
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