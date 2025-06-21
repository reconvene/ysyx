#include "VdisplayCodecTop.h"
#include "verilated.h"
#include "verilated_fst_c.h"
#include <random>
#include <nvboard.h>

void nvboard_bind_all_pins(VdisplayCodecTop *topModule);

int main(int argc, char **argv) {
    nvboard_init();

    // 声明verilator运行时上下文对象
    VerilatedContext *contextP = new VerilatedContext;
    // 如有输入运行时参数，则指定运行时参数
    contextP->commandArgs(argc, argv);
    contextP->traceEverOn(true);
    VerilatedFstC *waveTracer = new VerilatedFstC;

    // 将顶部模板引入上下文中运行
    VdisplayCodecTop *top = new VdisplayCodecTop{contextP};
    // 绑定模块引脚
    nvboard_bind_all_pins(top);


    int timeCount = 0;
    top->trace(waveTracer, 20);
    waveTracer->open("build/topWave.fst");
    // 给电路进行复位
    top->reset = 1;
    // 持续3个时钟周期
    for (int i = 0; i < 1; i++) {
        top->clock = 0;
        top->eval();
        waveTracer->dump(timeCount++);
        top->clock = 1;
        top->eval();
        waveTracer->dump(timeCount++);
    }
    // 将复位信号归位
    top->reset = 0;
    top->eval();
    waveTracer->dump(timeCount++);

    top->encodeIO_en=1;
    top->eval();
    uint8_t i=0;
    const char *testString="0123456789abcdef";
    // 如果运行未完成则一直运行
    while (!contextP->gotFinish()) {
        top->clock = 0;
        top->eval();
        waveTracer->dump(timeCount++);
        printf("n:%c\n",testString[i% strlen(testString)]);
        top->encodeIO_n=static_cast<int>(testString[i% strlen(testString)]);
        top->clock = 1;
        top->eval();
        waveTracer->dump(timeCount++);
        printf("m:%d\n",top->encodeIO_m);
        nvboard_update();
        i+=1;
    }

    nvboard_quit();
    waveTracer->close();
    delete waveTracer;
    delete top;
    delete contextP;
    return 0;
}