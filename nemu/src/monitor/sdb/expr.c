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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <math.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ,
  TK_NUM,
  TK_LEFT_SP,
  TK_RIGHT_SP,
  TK_PLUS,
  TK_MUL,
  TK_SUB,
  TK_DIV,
  TK_MOD,
  TK_POW

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", TK_PLUS},         // plus
  {"==", TK_EQ},        // equal
  {"\\*", TK_MUL},
  {"\\-",TK_SUB},
  {"\\/",TK_DIV},
  {"\\%",TK_MOD},
  {"\\(",TK_LEFT_SP},
  {"\\)",TK_RIGHT_SP},
  {"\\^",TK_POW},
  {"[0-9]+",TK_NUM}

};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

//        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
//            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        if(rules[i].token_type!=TK_NOTYPE){
          // 声明Token变量
          Token currentToken;
          // 最多复制32个字符
          strncpy(currentToken.str,substr_start,substr_len>32?32:substr_len);
          // 将字符串末尾加上/0
          currentToken.str[(substr_len)-1>=31?31:substr_len]='\0';

          // 标定当前token类别
          currentToken.type=rules[i].token_type;
          // 循环写入tokens
          tokens[nr_token%32]=currentToken;
          assert(tokens[nr_token%32].type==currentToken.type);
          nr_token+=1;
        }

//        switch (rules[i].token_type) {
//          default: TODO();
//        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

// 检查表达式是否被括号包裹
_Bool check_parentheses(uint8_t start, uint8_t end){

  // 声明括号数量
  uint8_t SPNum=0;
  // 遍历token
  for(uint8_t i=start;i<=end;++i){
    // 遇到左括号+1，遇到右括号-1
    if(tokens[i].type==TK_LEFT_SP) SPNum+=1;
    if(tokens[i].type==TK_RIGHT_SP) SPNum-=1;
  }

  // 内部括号不匹配
  if(SPNum!=0){
    panic("the brackets aren't matching");
    return false;
  }

  // 如果表达式没被括号包裹则进行计算
  if(tokens[start].type!= TK_LEFT_SP || tokens[end].type!=TK_RIGHT_SP){
    return false;
  }

  return true;
}

// 验证表达式
int eval(uint8_t start, uint8_t end){
  // 如果开头大于结尾则输入参数有问题
  if(start>end){
    panic("the number of len is invalid");

  // 如果开头等于结尾，则必定为数字
  } else if(end==start){
    assert(tokens[start].type==TK_NUM);
    return atoi(tokens[start].str);

  // 判断一下俩边是否有被括号包裹且格式是否正确，如果格式不正确则直接结束程序
  }else if(check_parentheses(start,end)){
    return eval(start+1,end-1);

  } else{
    uint8_t opPosition=0;
    uint8_t SPNum=0;
    // 遍历找到运算符位置
    for(uint8_t i=start;i<=end;++i){
      // 如果找到运算符且其没有被表达式包裹则设为主运算符
      if(tokens[i].type>260 && SPNum==0) opPosition=i;
      // 遇到左括号+1，遇到右括号-1
      if(tokens[i].type==TK_LEFT_SP) SPNum+=1;
      if(tokens[i].type==TK_RIGHT_SP) SPNum-=1;
    }

    // 对俩边表达式进行求值
    uint8_t leftValue= eval(start,opPosition-1);
    uint8_t rightValue= eval(opPosition+1,end);
    printf("start:%d end:%d\n",start,end);
    printf("leftValue:%d\n",leftValue);
    printf("rightValue:%d\n",rightValue);

    // 进行相应计算
    switch (tokens[opPosition].type) {
      case TK_PLUS:
        return leftValue+rightValue;
      case TK_SUB:
        return leftValue-rightValue;
      case TK_MUL:
        return leftValue*rightValue;
      case TK_DIV:
        return leftValue/rightValue;
      case TK_POW:
        return pow(leftValue,rightValue);
      case TK_MOD:
        return leftValue%rightValue;
      default:
        panic("the expression has something wrong");
    }
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  int resultNum=eval(0,nr_token-1);
  printf("result:%d\n",resultNum);

  return 1;
}
