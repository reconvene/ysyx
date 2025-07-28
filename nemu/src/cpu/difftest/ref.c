/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/cpu.h>
#include <difftest-def.h>
#include <memory/paddr.h>

__EXPORT void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction) {
  if (direction==DIFFTEST_TO_DUT) {
    memcpy(buf, guest_to_host(addr), n);
    return;
  }
  // DIFFTEST_TO_REF
  memcpy(guest_to_host(addr), buf, n);
}

__EXPORT void difftest_regcpy(void *dut, bool direction) {
  if (direction==DIFFTEST_TO_DUT) {
    memcpy(dut,&cpu.gpr,sizeof(cpu.gpr));
    return;
  }
  // DIFFTEST_TO_REF
  memcpy(&cpu.gpr, dut,sizeof(cpu.gpr));
}

__EXPORT paddr_t difftest_pccpy(paddr_t addr, bool direction) {
  if (direction==DIFFTEST_TO_DUT) return cpu.pc;
  cpu.pc = addr;
  return cpu.pc;
}

__EXPORT void difftest_exec(uint64_t n) {
  // printf("refPC: 0x%08X\n",cpu.pc);
  cpu_exec(n);
}

__EXPORT void difftest_raise_intr(word_t NO) {
  assert(0);
}

__EXPORT void difftest_init(int port) {
  void init_mem();
  init_mem();
  /* Perform ISA dependent initialization. */
  init_isa();
}
