#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/random.h>

#define MAX_DEPTH 4
int currentLocation = 0;

// 生成范围内的随机数字
int randInRange(int min, int max) {
  return rand() % (max - min) + min;
}

// 输入指定字符到字符串内
void gen(char inputChar, char *expBuffer) {
  sprintf(expBuffer+currentLocation, "%c", inputChar);
  currentLocation+=1;
}

// 生成随机操作符
void gen_rand_op(char *expBuffer) {
  char *opGroup[] = {"+", "-", "*", "/","==","!=","%","&&","||"};
  int randomNum = randInRange(0, 6);
  currentLocation +=sprintf(expBuffer + currentLocation, "%s", opGroup[randomNum]);
}

// 生成最大为100的数字到字符串中
void gen_rand_num(int min, int max, char *expBuffer) {
  int tmpNum = randInRange(min, max > 100 ? 100 : max);
  sprintf(expBuffer + currentLocation, "%d", tmpNum);

  if (tmpNum < 10) {
    currentLocation += 1;
    return;
  }
  currentLocation += 2;
  return;

}

// 生成表达式
void gen_rand_expr(int depth, char *expBuffer) {
  // 随机选择生成选项
  int randomOption = randInRange(0, 3);
  // 如果递归超过最大深度则直接生成数字并返回
  if (depth >= MAX_DEPTH) {
    gen_rand_num(1,100,expBuffer);
    return;
  }

  // 第一次循环选择第三个选项
  if(depth==0) randomOption=2;
  switch (randomOption) {
    case 0:
      gen_rand_num(1,100,expBuffer);
      break;

    case 1:
      gen('(', expBuffer);
      gen_rand_expr(depth + 1, expBuffer);
      gen(')', expBuffer);
      break;

    default:
      gen_rand_expr(depth + 1, expBuffer);
      gen_rand_op(expBuffer);
      gen_rand_expr(depth + 1, expBuffer);
      break;
  }

//  printf("[DEBUG] depth=%d, current/Location=%d, randomOption=%d, exp=%s\n", depth, currentLocation,randomOption, expBuffer);
}

int main() {
  // 重置随机种子
  unsigned int seed;
  getrandom(&seed, sizeof(seed), 0);
  srand(seed);

  char *exp = malloc(100 * sizeof(char));
  memset(exp, 0, 100);

  int SPNum=0;
  gen_rand_expr(0, exp);

  // 检查左右括号是否匹配
  for(int j=0;exp[j]!='\0';++j){
    if(exp[j]=='(') SPNum+=1;
    if(exp[j]==')') SPNum-=1;
  }
//    printf("SPNum:%d\n",SPNum);

    // 如果括号不匹配，则返回1
  if(SPNum!=0) {
    free(exp);
    return 1;
  }

  // 输出表达式
  printf("%s\n", exp);
  memset(exp, 0, 100);
  currentLocation=0;

  free(exp);
  return 0;
}