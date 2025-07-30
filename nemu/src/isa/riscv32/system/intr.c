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
#include <utils/logRingBufferLib.h>

#ifdef CONFIG_ETRACE
static logRingBuffer *ecallLogRingBuffer=NULL;
static uint64_t g_nr_guest_ecall=0;

// 打印环形缓冲区
void printEcallLogBuffer(){
  ecallLogRingBuffer->readIndex=g_nr_guest_ecall>=LOG_BUFFER_SIZE ? ecallLogRingBuffer->writeIndex : 0;
  uint64_t printLen=g_nr_guest_ecall>=LOG_BUFFER_SIZE ? LOG_BUFFER_SIZE : g_nr_guest_ecall;

  printLogRingBuffer(ecallLogRingBuffer, printLen);
  Log(ANSI_FMT("ERROR ECALL", ANSI_FG_RED));
  destroyLogRingBuffer(ecallLogRingBuffer);
}
#endif


word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  cpu.csr[CSR_MEPC]=epc;
  // 机器模式的mcause码
  cpu.csr[CSR_MCAUSE] = 0x0000000b;
  cpu.csr[CSR_MSTATUS] = 0x1800;

#ifdef CONFIG_ETRACE
  if (!g_nr_guest_ecall) ecallLogRingBuffer=createLogRingBuffer(LOG_BUFFER_SIZE);

    writeLogRingBuffer(ecallLogRingBuffer, "ecall at PC: %08X, %s value: %d, MEPC: 0x%08X, MCAUSE: 0x%08X, MSTATUS: 0x%08X",
      epc,
      MUXDEF(CONFIG_RVE, "a5", "a7"),
      MUXDEF(CONFIG_RVE, cpu.gpr[15], cpu.gpr[17]),
      cpu.csr[CSR_MEPC],
      cpu.csr[CSR_MCAUSE],
      cpu.csr[CSR_MSTATUS]);
  g_nr_guest_ecall++;
#endif

  return cpu.csr[CSR_MTVEC];
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
