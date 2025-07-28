#ifndef DIIFTEST_H
#define DIIFTEST_H

#include <dlfcn.h>
#include <macro.h>
enum { DIFFTEST_TO_DUT, DIFFTEST_TO_REF };

void difftest_step(word_t n, vaddr_t currentPC);
void difftest_skip_ref();

#endif //DIIFTEST_H
