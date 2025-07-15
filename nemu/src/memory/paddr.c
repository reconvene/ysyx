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

#include <utils/logRingBufferLib.h>
#include <device/mmio.h>
#include <isa.h>
#include <memory/host.h>
#include <memory/paddr.h>
#include <utils.h>

#if   defined(CONFIG_PMEM_MALLOC)
static uint8_t *pmem = NULL;
#else // CONFIG_PMEM_GARRAY
static uint8_t pmem[CONFIG_MSIZE] PG_ALIGN = {};
#endif
#ifdef CONFIG_MTRACE
static logRingBuffer *memoryLogRingBuffer=NULL;
static uint64_t g_nr_memory_action=0;
#endif

uint8_t* guest_to_host(paddr_t paddr) { return pmem + paddr - CONFIG_MBASE; }
paddr_t host_to_guest(uint8_t *haddr) { return haddr - pmem + CONFIG_MBASE; }

static word_t pmem_read(paddr_t addr, int len) {
  word_t ret = host_read(guest_to_host(addr), len);
  return ret;
}

static void pmem_write(paddr_t addr, int len, word_t data) {
  host_write(guest_to_host(addr), len, data);
}

#ifdef CONFIG_MTRACE
static void printMemoryLogBuffer() {
  memoryLogRingBuffer->readIndex=g_nr_memory_action>=LOG_BUFFER_SIZE ? memoryLogRingBuffer->writeIndex : 0;
  uint64_t printLen=g_nr_memory_action>=LOG_BUFFER_SIZE ? LOG_BUFFER_SIZE : g_nr_memory_action;

  printLogRingBuffer(memoryLogRingBuffer, printLen);
  destroyLogRingBuffer(memoryLogRingBuffer);
  Log(ANSI_FMT("WRONG MEMORY ACTION", ANSI_FG_RED));
}
#endif

static void out_of_bound(paddr_t addr) {
  IFDEF(CONFIG_MTRACE, printMemoryLogBuffer());
  panic("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR ", " FMT_PADDR "] at pc = " FMT_WORD,
      addr, PMEM_LEFT, PMEM_RIGHT, cpu.pc);
}

void init_mem() {
#if   defined(CONFIG_PMEM_MALLOC)
  pmem = malloc(CONFIG_MSIZE);
  assert(pmem);
#endif
  IFDEF(CONFIG_MEM_RANDOM, memset(pmem, rand(), CONFIG_MSIZE));
  Log("physical memory area [" FMT_PADDR ", " FMT_PADDR "]", PMEM_LEFT, PMEM_RIGHT);
  IFDEF(CONFIG_MTRACE, memoryLogRingBuffer=createLogRingBuffer(LOG_BUFFER_SIZE));
}

word_t paddr_read(paddr_t addr, int len) {
  IFDEF(CONFIG_MTRACE, g_nr_memory_action++; writeLogRingBuffer(memoryLogRingBuffer, "action: read,  targetAddr:0x%08X, length: %d", addr, len*8));
  if (likely(in_pmem(addr))) return pmem_read(addr, len);
  IFDEF(CONFIG_DEVICE, return mmio_read(addr, len));
  out_of_bound(addr);
  return 0;
}

void paddr_write(paddr_t addr, int len, word_t data) {
  IFDEF(CONFIG_MTRACE, g_nr_memory_action++; writeLogRingBuffer(memoryLogRingBuffer, "action: write, targetAddr:0x%08X, length: %d", addr, len*8));
  if (likely(in_pmem(addr))) { pmem_write(addr, len, data); return; }
  IFDEF(CONFIG_DEVICE, mmio_write(addr, len, data); return);
  out_of_bound(addr);
}

