#include <sdb/sdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <npc.h>
#include <macro.h>

// 声明单条指令基本元素
typedef struct {
    const char *name;
    const char *description;

    int (*cmdFunc)(char *args);
} singleCommand;

int commandC(char *args) {
    executeN(-1);
    return 0;
}

int commandSi(char *args) {
    long int execStep = strtol(args,NULL, 10);
    executeN(execStep);
    return 0;
}

static int commandX(char *args){
    // 分开长度字符串和起始地址表达式字符串
    char *stringLen = strtok(args," ");
    char *exp= args+ strlen(stringLen) +1;
    _Bool success=0;

    // 计算表达式字符串
    word_t evalResult=expr(exp,&success);
    assert(success);
    printf("startAddr: 0x%08X\n", evalResult);

    // 读取指定长度的内存
    for(uint i=0;i< strtol(stringLen,NULL,10);++i){
        if(i%8==0 && i!=0) printf("\n");
        printf("%02X ",pmemRead(evalResult+i,1));
    }

    // 输出读取结束时的内存地址
    printf("\nendAddr: 0x%08X\n", evalResult+(unsigned int)strtol(stringLen,NULL,10));

    return 0;
}

static int commandInfo(char *args){
    if(strcmp(args,"r")==0){
        listReg();
    }else if(strcmp(args,"w")==0){
        list_wp();
    }else{
        puts("invalid input");
    }
    return 0;
}


// 表达式求值
static int commandP(char *args){
    _Bool evalResult=false;
    printf("result:%u\n",expr(args,&evalResult));

    return 0;
}

static int commandW(char *args){
    // 创建监视点
    WP *currentWP=new_wp();
    assert(currentWP!=NULL && "watchpoint generation failed");

    // 定义监视点姓名和表达式
    char *wpName = strtok(args," ");
    char *exp= args + strlen(wpName) +1;
    set_wp(currentWP, strdup(wpName), strdup(exp));

    return 0;
}

// 删除监视点
int commandD(char *args){
    int wpNo= strtol(args,NULL,10);
    free_wp_by_no(wpNo);
    return 0;
}

int commandQ(char *args) {
    return 1;
}

// 初始化指令查询表
static int commandHelp(char *args);

static const singleCommand commandTable[] = {
    {"help", "list all commands", commandHelp},
    {"c", "Continue the execution of the program", commandC},
    {"q", "Exit NEMU", commandQ},
    {"si", "Execute the program for N steps", commandSi},
    {"x",     "View memory using an expression and a step size",commandX},
    {"info",  "View the registers using a specified method",commandInfo},
    {"p",     "Calculate the expression",commandP},
    {"w",     "Monitor the expression",commandW},
    {"d",     "Delete the watchpoint",commandD},
};
static const int commandTableLen = sizeof(commandTable) / sizeof(singleCommand);

static int commandHelp(char *args) {
    for (int i = 1; i < commandTableLen; i++) {
        printf("%s - %s\n", commandTable[i].name, commandTable[i].description);
    }

    if (args) printf("unknown arguments: %s\n", args);
    return 0;
}

// 初始化sdb
void sdbThread() {
    init_regex();
    IFDEF(CONFIG_WP, init_wp_pool());

    if (batchMode) {
        commandC(NULL);
        return;
    }

    while (1) {
        // 不断读取用户输入，直至输入不为空
        char *line = readline("(npc) ");
        if (line[0] == '\0') {
            free(line);
            continue;
        }

        // 分出命令和参数
        char *command = strtok(line, " ");
        char *args = strtok(NULL, " ");
        add_history(line);

        // 遍历命令查询表
        int i = 0;
        for (; i < commandTableLen; i++) {
            if (!strcmp(command, commandTable[i].name)) {
                if (commandTable[i].cmdFunc(args)) return;
                break;
            }
        }

        // 如果没找到，则是未知命令
        if (i == commandTableLen) printf("unknown command: %s\n", command);
    }
}