#include <macro.h>
#include <npc.h>
#include <difftest/diiftest.h>

void (*ref_difftest_init)(int port)=NULL;
void (*ref_difftest_memcpy)(word_t addr, void *buf, size_t n, bool direction)=NULL;
void (*ref_difftest_regcpy)(void *dut, bool direction)=NULL;
vaddr_t (*ref_difftest_pccpy)(vaddr_t addr, bool direction)=NULL;
void (* ref_difftest_exec)(uint64_t n)=NULL;

static _Bool skip_ref=0;

// 初始化差分测试
void init_difftest(char *fileName, word_t imageSize) {
    if (!fileName) panic("Can't read .so file\n");

    // 读取共享库
    void *soFile = dlopen(fileName, RTLD_LAZY);
    if (!soFile) panic("Failed to load shared object: %s\n",dlerror());

    // 获取相关函数
    ref_difftest_init = (void (*)(int port))dlsym(soFile, "difftest_init");
    assert(ref_difftest_init);

    ref_difftest_memcpy = (void (*)(word_t addr, void *buf, size_t n, bool direction)) dlsym(soFile, "difftest_memcpy");
    assert(ref_difftest_memcpy);

    ref_difftest_regcpy = (void (*)(void *dut, bool direction))dlsym(soFile, "difftest_regcpy");
    assert(ref_difftest_regcpy);

    ref_difftest_pccpy = (vaddr_t (*)(vaddr_t addr, bool direction))dlsym(soFile, "difftest_pccpy");
    assert(ref_difftest_pccpy);

    ref_difftest_exec = (void (*)(uint64_t n))dlsym(soFile, "difftest_exec");
    assert(ref_difftest_exec);

    // 初始化difftest(port实际上无效)
    ref_difftest_init(0);
    printf("loading image into ref memory, size:%dB\n", imageSize);
    ref_difftest_memcpy(RESET_VECTOR, vaddr2pptr(RESET_VECTOR), imageSize, DIFFTEST_TO_REF);
    ref_difftest_regcpy(gprBaseAddress, DIFFTEST_TO_REF);

    printf("load difftest successfully\n");
    printf("RESET_VECTOR: 0x%08X\ngprBase: %p\n", RESET_VECTOR, gprBaseAddress);
}

// 检查寄存器是否相同
void difftest_checkregs() {
    uint32_t refGpr[16]={};
    ref_difftest_regcpy(refGpr, DIFFTEST_TO_DUT);
    for (uint8_t i=0; i<16; i++) {
        if (refGpr[i] != gprBaseAddress[i]) {
            printf("------------------------\n");
            for (uint8_t i = 0; i < 16; ++i) {
                if (i % 8 == 0 && i != 0) printf("\n");
                printf("0x%08X\t", refGpr[i]);
            }
            printf("\n------------------------\n");
            diagnoseError();
            panic("refGpr[%d]: 0x%08X, dutGpr[%d]: 0x%08X, they are different\n", i, refGpr[i], i, gprBaseAddress[i]);
        }
    }
}

// 检查PC是否相同
void difftest_checkpc(vaddr_t currentPC) {
    vaddr_t refPC=ref_difftest_pccpy(0, DIFFTEST_TO_DUT);
    if (currentPC != refPC) panic("refPC: 0x%08X, dutPC: 0x%08X, they are different\n", refPC, currentPC);
}

// 检查内存是否相同
void difftest_checkmem() {
    uint8_t *refMem = (uint8_t *)malloc(MEM_SIZE);
    if (refMem == NULL) {
        printf("malloc failed! MEM_SIZE = %lu\n", (unsigned long)MEM_SIZE);
        exit(1);
    }
    ref_difftest_memcpy(RESET_VECTOR,refMem,MEM_SIZE, DIFFTEST_TO_DUT);
    for (uint8_t i=0; i<MEM_SIZE; i++) {
        if (refMem[i] != pmemRead(RESET_VECTOR+i,1)) {
            diagnoseError();
            panic("refMem[%d]: 0x%02X, dutMem[%d]: 0x%02X, they are different\n", i, refMem[i], i, pmemRead(RESET_VECTOR+i,1));
        }
    }
    free(refMem);
}

// 跳过下一次比对
void difftest_skip_ref() {
    skip_ref = 1;
}

// ref执行一步，并进行比对
void difftest_step(word_t n, vaddr_t currentPC) {
    // 如果跳过，则把dut的寄存器和pc复制过去
    if (skip_ref) {
        ref_difftest_regcpy(gprBaseAddress, DIFFTEST_TO_REF);
        ref_difftest_pccpy(currentPC, DIFFTEST_TO_REF);
        skip_ref = 0;
        return;
    }

    ref_difftest_exec(n);
    // difftest_checkmem();
    difftest_checkregs();
    difftest_checkpc(currentPC);
}