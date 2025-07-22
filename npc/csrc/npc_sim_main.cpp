#include "Vnpc.h"
#include "verilated.h"
#include "verilated_fst_c.h"
#include <random>
#include <sys/stat.h>
#include <macro.h>
#include <npc.h>
#include <sdb/sdb.h>
#include <utils/logRingBufferLib.h>
#include <utils/logStackLib.h>
#include <utils/elfParser.h>
#include <argsParser.h>
#include <difftest/diiftest.h>
#include "Vnpc___024root.h"
// #include <nvboard.h>

// 定义最大指令输出数
#define INST_PRINT_COUNT 10

// 定义npc相关参数
static uint8_t pmem[MEM_SIZE] = {};
static word_t execCount = 0;

// 定义运行模式
_Bool batchMode = false;
static _Bool stopIF = false;

// 定义上下文环境和运行对象
static VerilatedContext *contextP = NULL;
static Vnpc *top = NULL;
word_t *gprBaseAddress=NULL;

// 定义错误诊断函数;
void diagnoseError();

#ifdef CONFIG_FST
// 定义FST波形追踪
static VerilatedFstC *waveTracer = NULL;
static word_t timeCount = 0;
#endif

// 内存追踪
#ifdef CONFIG_MTRACE
static logRingBuffer *memoryLogRingBuffer = NULL;
static uint64_t g_nr_memory_action = 0;

static void printMemoryLogBuffer() {
    memoryLogRingBuffer->readIndex = g_nr_memory_action >= LOG_BUFFER_SIZE ? memoryLogRingBuffer->writeIndex : 0;
    uint64_t printLen = g_nr_memory_action >= LOG_BUFFER_SIZE ? LOG_BUFFER_SIZE : g_nr_memory_action;

    printLogRingBuffer(memoryLogRingBuffer, printLen);
    destroyLogRingBuffer(memoryLogRingBuffer);
}

#endif

// 指令追踪
#ifdef CONFIG_ITRACE
static logRingBuffer *instLogRingBuffer = NULL;

static void printInstLogBuffer() {
    instLogRingBuffer->readIndex = execCount >= LOG_BUFFER_SIZE ? instLogRingBuffer->writeIndex : 0;
    uint64_t printLen = execCount >= LOG_BUFFER_SIZE ? LOG_BUFFER_SIZE : execCount;

    printLogRingBuffer(instLogRingBuffer, printLen);
    destroyLogRingBuffer(instLogRingBuffer);
}
#endif

// 函数堆栈追踪
#ifdef CONFIG_FTRACE
static logStack *funcStack = NULL;

static void printFuncStack() {
    listLogStack(funcStack);
    destroyLogStack(funcStack);
    destroyElfParser();
}
#endif

// 虚拟地址转换
static inline word_t vaddr2paddr(word_t vaddr) {
    printf("vaddr2paddr: vaddr = 0x%08X, RESET_VECTOR = 0x%08X, result = 0x%08X\n",
    top->io_readAddr, RESET_VECTOR, top->io_readAddr - RESET_VECTOR);
    if (vaddr - CONFIG_MBASE > MEM_SIZE) {
        diagnoseError();
        printf("the address reading: 0x%08X is overflow\n", vaddr);
        assert(0);
    }
    return vaddr - RESET_VECTOR;
}

// 虚拟地址转真实指针
uint8_t *vaddr2pptr(word_t vaddr) {
    if (vaddr - CONFIG_MBASE > MEM_SIZE) {
        diagnoseError();
        printf("the address reading: 0x%08X is overflow\n", vaddr);
        assert(0);
    }
    return &pmem[vaddr-CONFIG_MBASE];
}

// 无符号读取内存
word_t pmemRead(vaddr_t vaddr, uint8_t len) {
    uint32_t raw = 0;
    memcpy(&raw, &pmem[vaddr2paddr(vaddr)], len);
    IFDEF(CONFIG_MTRACE, g_nr_memory_action++; writeLogRingBuffer(memoryLogRingBuffer, "action: read ,  targetAddr: 0x%08X, value: 0x%08X, length: %d, type: unsigned", vaddr, raw, len*8));
    top->io_readReady=true;
    switch (len) {
        case 1: return (uint8_t) raw;
        case 2: return (uint16_t) raw;
        case 4: return (uint32_t) raw;
        default: return 0;
    }
}

// 有符号读取内存
sword_t pmemSignedRead(vaddr_t vaddr, uint8_t len) {
    uint32_t raw = 0;
    memcpy(&raw, &pmem[vaddr2paddr(vaddr)], len);
    IFDEF(CONFIG_MTRACE, g_nr_memory_action++; writeLogRingBuffer(memoryLogRingBuffer, "action: read ,  targetAddr: 0x%08X, value: 0x%08X, length: %d, type: signed", vaddr, raw, len*8));
    top->io_readReady=true;
    switch (len) {
        case 1: return (int8_t) raw;
        case 2: return (int16_t) raw;
        case 4: return (int32_t) raw;
        default: return 0;
    }
}

// 写入内存
void pmemWrite(vaddr_t vaddr, word_t data, uint8_t len) {
    IFDEF(CONFIG_MTRACE, g_nr_memory_action++; writeLogRingBuffer(memoryLogRingBuffer, "action: write,  targetAddr: 0x%08X, value: 0x%08X, length: %d, type: unsigned", vaddr, top->io_writeData, len*8));
    memcpy(&pmem[vaddr2paddr(vaddr)], &data, len);
}

// void nvboard_bind_all_pins(Vps2Display *topModule);

// 电路更新
void moveForward(int steps) {
    for (int j = 0; j < steps; ++j) {
        top->clock = 1;
        top->eval();
        top->clock = 0;
        top->eval();
        IFDEF(CONFIG_FST, waveTracer->dump(timeCount++));
    }
}

// 错误信息诊断
void diagnoseError() {
    listReg();
    IFDEF(CONFIG_ITRACE, printInstLogBuffer());
    IFDEF(CONFIG_MTRACE, printMemoryLogBuffer());
    IFDEF(CONFIG_FTRACE, printFuncStack());
    printf("Total execution count:%d\n", execCount);
}

// 单步 执行
static void executeOnce() {
    // if (execCount>=31) {moveForward(1); execCount++; return;}
    top->io_readReady=false;
    top->io_inst = *(word_t *) &pmem[top->io_pc - CONFIG_MBASE];
    if (top->io_readEnable) top->io_readData = top->io_readType ? pmemSignedRead(top->io_readAddr, top->io_byteNum) : pmemRead(top->io_readAddr, top->io_byteNum);
    if (top->io_writeEnable) pmemWrite(top->io_writeAddr, top->io_writeData, top->io_byteNum);

    IFDEF(CONFIG_ITRACE, writeLogRingBuffer(instLogRingBuffer, "0x%08X: %08X",top->io_pc,top->io_inst));
#ifdef CONFIG_FTRACE
    if (top->io_inst == 0x00008067) {
        unshiftLogStack(funcStack, 0);
        moveForward(1);
        // nvboard_update();
        execCount++;
        return;
    }

    // 判断指令是否为jal/jalr
    _Bool jalIF = (top->io_inst & 0x7F) == 0x6F || (top->io_inst & 0x7F) == 0x67;
    funcInfo *currentFunc = NULL;
    word_t currentPC = 0;

    if (jalIF) {
        currentFunc = lookupFunctions(top->io_pc);
        currentPC = top->io_pc;
    }
#endif

    moveForward(1);
    // nvboard_update();
    execCount++;

#ifdef CONFIG_FTRACE
    if (jalIF) {
        funcInfo *targetFunc = lookupFunctions(top->io_pc);
        // printf("funcName:%p\n", targetFunc);
        if (currentFunc && targetFunc) {
            shiftLogStack(funcStack, "call <%s> at <%s> PC: 0x%08X", targetFunc->name, currentFunc->name, currentPC);
        }
    }
#endif

    if (contextP->gotFinish()) {
        if (top->io_quitState) {
            printf("HIT BAD TRAP AT PC: 0x%08X\n",top->io_pc);
            diagnoseError();
        }
        else printf("HIT GOOD TRAP AT PC: 0x%08X\n",top->io_pc);
        printf("Total execution count:%d\n", execCount);
    }
}

// 执行N次
void executeN(word_t step) {
    stopIF=false;
    if (contextP->gotFinish()) {
        printf("The simulation has ended. Please exit and restart NPC\n");
        return;
    }

    for (word_t i = step; i > 0 && !contextP->gotFinish() && !stopIF; --i) {
        if (step <= INST_PRINT_COUNT) printf("0x%08X: %08X\n", top->io_pc, pmemRead(top->io_pc, 4));
        // if (execCount>=31) return;
        executeOnce();
        IFDEF(CONFIG_DIFFTEST, if (!top->io_readEnable) {
            printf("difftest at pc: 0x%08X\n", top->io_pc);
            difftest_step(1);
        });
#ifdef CONFIG_WP
        // 监听监视点
        WP *changedWP=monitor_wp();
        // 如果发生变化，则暂停nemu
        if(changedWP){
            stopIF=true;
            printf("the value of watchpoints has changed\n");
            // 释放变化节点
            free_wp(changedWP);
        }
#endif
    }
}

static const uint32_t img[] = {
    0x800007b7,
    0xfff78793,
    0x002786b3,
    0x0037a023,
    0x00100073,
    // some data
    0xdeadbeef
};

word_t imageSize=sizeof(img);

// 加载映像文件
void loadImage(const char *filename) {
    if (!filename) {
        printf("loading default image.....\n");
        memcpy(pmem, img, sizeof(img));
        return;
    }

    // 读取bin文件
    FILE *elfFile = fopen(filename, "rb");
    if (!elfFile) {
        // assert(0 && "The input ELF file does not exist or lacks sufficient permissions");
        printf("The input BIN file does not exist or lacks sufficient permissions\n");
        printf("loading default image.....\n");
        memcpy(pmem, img, sizeof(img));
        return;
    }

    // 获取文件大小
    struct stat elfStat;
    fstat(fileno(elfFile), &elfStat);
    if (elfStat.st_size <= 0)
        assert(0 && "Failed to read BIN file size");

    // 读取文件
    size_t readSize = fread(pmem, 1, elfStat.st_size, elfFile);
    assert(readSize == elfStat.st_size);
    imageSize=elfStat.st_size;
    printf("load specified image successfully, size:%dB\n",imageSize);
    fclose(elfFile);
}

// 初始化npc
static void initNpc(int argc, char **argv) {
    // 声明verilator运行时上下文对象
    contextP = new VerilatedContext;
    // 如有输入运行时参数，则指定运行时参数
    contextP->commandArgs(argc, argv);
    contextP->traceEverOn(true);
    // 将顶部模块引入上下文中运行
    top = new Vnpc{contextP};
    if (!top) panic("Failed to initial npc module\n");
    printf("gprBaseAddress:0x%p\n", &top->rootp->npc__DOT__gpr__DOT__regGroup_0);
    gprBaseAddress=&top->rootp->npc__DOT__gpr__DOT__regGroup_0;

    // 绑定模块引脚
    // nvboard_bind_all_pins(top);

#ifdef CONFIG_FST
    // 初始化fst
    waveTracer = new VerilatedFstC;
    top->trace(waveTracer, 20);
    waveTracer->open("build/topWave.fst");
#endif

    // 给电路进行复位
    top->reset = 1;
    // 持续3个时钟周期
    for (int i = 0; i < 3; i++) {
        moveForward(1);
    }
    // 将复位信号归位
    top->reset = 0;
    top->eval();
}

// 销毁npc
static void destroyNpc() {
    waveTracer->close();
    delete waveTracer;
    delete top;
    delete contextP;
}

int main(int argc, char **argv) {
    // 初始化npc
    initNpc(argc, argv);

    // 参数解析
    argsParser(argc, argv);

    IFDEF(CONFIG_ITRACE, instLogRingBuffer=createLogRingBuffer(LOG_BUFFER_SIZE));
    IFDEF(CONFIG_MTRACE, memoryLogRingBuffer=createLogRingBuffer(LOG_BUFFER_SIZE));
    IFDEF(CONFIG_FTRACE, funcStack=createLogStack(STACK_SIZE));

    // 初始化nvboard
    // nvboard_init();

    // 进入sdb
    sdbThread();

    // 销毁nvboard
    // nvboard_quit();

    // 销毁npc
    destroyNpc();
    return top->io_quitState;
}
