#include "Vour.h"
#include "verilated.h"
#include <random>

int main(int argc, char** argv) {
    // 使用/dev/random引入外部随机源
    std::random_device randomSeed;
    // 使用外部随机源生成随机种子
    std::mt19937_64 gen(randomSeed());
    // 指定随机生成0或1
    std::uniform_int_distribution<unsigned int> dist(0,1);

    // 声明verilator运行时上下文对象
    VerilatedContext* contextP = new VerilatedContext;
    // 如有输入运行时参数，则指定运行时参数
    contextP->commandArgs(argc, argv);

    // 将顶部模板引入上下文中运行
    Vour *top = new Vour{contextP};

    // 如果运行未完成则一直运行
    while (!contextP->gotFinish()) {
        top->a=dist(gen);
        top->b=dist(gen);
        top->eval();

        printf("a=%u\tb=%u\tf=%u\t\n", top->a,top->b,top->f);
        sleep(3);
    }
    delete top;
    delete contextP;
    return 0;
}