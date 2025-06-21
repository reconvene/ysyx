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
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include "sdb.h"
#include "memory/paddr.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state=NEMU_END;
  return -1;
}

static int cmd_si(char *args){
  long int execStep= strtol(args,NULL,10);
  cpu_exec(execStep);
  return 0;
}

static int cmd_info(char *args){
  if(strcmp(args,"r")==0){
    isa_reg_display();
  }else if(strcmp(args,"w")==0){
    list_wp();
  }else{
    puts("invalid input");
  }
  return 0;
}

static int cmd_x(char *args){
  // 分开长度字符串和起始地址表达式字符串
  char *stringLen = strtok(args," ");
  char *exp= args+ strlen(stringLen) +1;
  _Bool success=false;

  // 计算表达式字符串
  word_t evalResult=expr(exp,&success);
  assert(success);
  printf("startAddr: 0x%08X\n", evalResult);

  // 将其映射到宿主机内存地址中
  uint8_t *realAddr= guest_to_host(evalResult);

  // 读取指定长度的内存
  for(uint i=0;i< strtol(stringLen,NULL,10);++i){
    if(i%8==0 && i!=0) printf("\n");
    printf("%02X ",*(realAddr+i));
  }

  printf("\n");
  return 0;
}

// 表达式求值
static int cmd_p(char *args){
  _Bool evalResult=false;
  printf("result:%u\n",expr(args,&evalResult));

  return evalResult;
}

// 表达式测试
static int cmd_pTest(char *args) {

  // 测试表达式求值
  // 读取文件
  FILE *testFile= fopen("test/calculatorTestFile","r");
  //设置每行内容
  char *currentTest=NULL;
  size_t testLen=0;
  // 初始化计数器
  int execCount=0;
  int sucessCount=0;

  // 按行读取
  while (getline(&currentTest,&testLen,testFile)!=-1){
    execCount+=1;
    sucessCount+=exprForTest(currentTest);
    // 如果当前测试失败，则输出失败用例
    if(!sucessCount) {
      printf("%s\n",currentTest);
    }
  }
  // 输出测试结果
  printf("Total tests:%d\nPassed tests:%d\n%05.2f%%\n",execCount,sucessCount,(float)sucessCount/(float)execCount*100);

  free(currentTest);
  fclose(testFile);

  return 0;
}

static int cmd_w(char *args){
  // 创建监视点
  WP *currentWP=new_wp();
  Assert(currentWP!=NULL,"watchpoint generation failed");

  // 定义监视点姓名和表达式
  char *wpName = strtok(args," ");
  char *exp= args+ strlen(wpName) +1;
  set_wp(currentWP, strdup(wpName), strdup(exp));

  return 0;
}

// 删除监视点
int cmd_d(char *args){
  int wpNo= strtol(args,NULL,10);
  free_wp_by_no(wpNo);
  return 0;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  {"si","Execute the program for N steps",cmd_si},
  {"info","View the registers using a specified method",cmd_info},
  {"x","View memory using an expression and a step size",cmd_x},
  {"p", "Calculate the expression",cmd_p},
  {"pTest", "Test expression evaluation",cmd_pTest},
  {"w", "Monitor the expression",cmd_w},
  {"d", "Delete the watchpoint",cmd_d},

  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
