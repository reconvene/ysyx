#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  // printf("handler!\n");
  // printf("before handler mepc = 0x%08X\n", c->mepc);
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      // M模式下进入异常则为自陷
      case 0xb: {
        ev.event = EVENT_SYSCALL;
        if (c->GPR1==-1) ev.event = EVENT_YIELD;
        break;
      }
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }
  // printf("after handler mepc = 0x%08X\n", c->mepc);
  c->mepc+=4;

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

// 初始化线程上下文
Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  // 初始化上下文位置
  Context *contextLocation=(Context *)(((uintptr_t)kstack.end-sizeof(Context)) & ~(sizeof(uintptr_t) - 1));

  // 初始化上下文
  Context c = {
    .gpr = {0},
    .mcause = 0xb,
    .mstatus = 0x1800,
    .mepc = (uintptr_t)entry-4
  };
  // 往a0里传输第一个形参参数
  c.gpr[10]=(uintptr_t)arg;
  // 返回上下文指针
  *contextLocation=c;
  return contextLocation;
}

void yield() {
  // printf("yield!\n");
#ifdef __riscv_e
  asm volatile("li a5, -1; ecall");
#else
  asm volatile("li a7, -1; ecall");
#endif
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
