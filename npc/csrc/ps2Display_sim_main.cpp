#include "Vps2Display.h"
#include "verilated.h"
#include "verilated_fst_c.h"
#include <random>
#include <nvboard.h>

void nvboard_bind_all_pins(Vps2Display *topModule);

// 电路更新
void moveFroward(Vps2Display *target ,int steps){
    for(int j=0;j<steps;++j){
        target->clock = 1;
        target->eval();
        target->clock = 0;
        target->eval();
    }
}

int main(int argc, char **argv) {
    nvboard_init();

    // 声明verilator运行时上下文对象
    VerilatedContext *contextP = new VerilatedContext;
    // 如有输入运行时参数，则指定运行时参数
    contextP->commandArgs(argc, argv);
    contextP->traceEverOn(true);
    VerilatedFstC *waveTracer = new VerilatedFstC;

    // 将顶部模板引入上下文中运行
    Vps2Display *top = new Vps2Display{contextP};
    // 绑定模块引脚
    nvboard_bind_all_pins(top);


    int timeCount = 0;
    top->trace(waveTracer, 20);
    waveTracer->open("build/topWave.fst");
    // 给电路进行复位
    top->reset = 1;
    // 持续3个时钟周期
    for (int i = 0; i < 3; i++) {
        moveFroward(top,1);
        waveTracer->dump(timeCount++);
        moveFroward(top,1);
        waveTracer->dump(timeCount++);
    }
    // 将复位信号归位
    top->reset = 0;
    top->eval();

    // 默认点亮所有数码管
    top->io_en=1;
    int i=0;
    // 如果运行未完成则一直运行
    while (!contextP->gotFinish()) {
        moveFroward(top,1);
        waveTracer->dump(timeCount++);
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