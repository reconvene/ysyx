//
// Created by admin123 on 7/17/25.
//

#ifndef SDB_H
#define SDB_H
#include <macro.h>

typedef struct watchpoint WP;

void sdbThread();
void init_regex();

word_t expr(char *e, _Bool *success);
int exprForTest(char *e);

void init_wp_pool();
WP *new_wp();
void free_wp(WP *inputWP);
void free_wp_by_no(int targetNo);
void set_wp(WP *inputWP, char *wpName, char *inputExp);
WP *monitor_wp();
void list_wp();

#endif //SDB_H
