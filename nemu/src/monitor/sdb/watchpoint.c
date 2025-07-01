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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char *wpName;
  char *exp;
  word_t resultValue;
  _Bool initialState;
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
// 存储先前的值
static word_t wpValueGroup[NR_WP]={};

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    wp_pool[i].wpName=NULL;
    wp_pool[i].exp=NULL;
    wp_pool[i].resultValue=0;
    wp_pool[i].initialState=false;
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
// 获取空闲监视点
WP *new_wp(){
  if(free_ == NULL){
    assert(0);
  }
  WP *tmpWP=free_;
  free_=free_->next;
  tmpWP->next=head;
  head=tmpWP;

  return tmpWP;
}

// 返还空闲监视点
void free_wp(WP *inputWP){
  // head为空
  if(head==NULL){
    return;
  }

  WP *nextWP=head->next;
  // 只有一个节点
  if (head->NO==inputWP->NO){
    inputWP->next=free_;
    inputWP->initialState=false;
    free_=inputWP;
    head=head->next==NULL?NULL:nextWP;
    return;
  }

  // 循环遍历剩余节点
  WP *tmpWP=head;
  while (tmpWP->next!=NULL){
    if(tmpWP->next->NO==inputWP->NO){
      tmpWP->next=inputWP->next;
      inputWP->next=free_;
      inputWP->initialState=false;
      free_=inputWP;
      return;
    }
    tmpWP=tmpWP->next;
  }
}

// 通过编号删除监视点
void free_wp_by_no(int targetNo){
  if(head==NULL){
    return;
  }

  WP *tmpWP=head;
  WP *nextWP=head->next;
  if(head->NO==targetNo){
    tmpWP->next=free_;
    tmpWP->initialState=false;
    free_=tmpWP;
    head=head->next==NULL?NULL:nextWP;
    return;
  }

  WP *prevWP=NULL;
  while (tmpWP!=NULL){
    if(tmpWP->NO==targetNo){
      prevWP->next=tmpWP->next;
      tmpWP->next=free_;
      tmpWP->initialState=false;
      free_=tmpWP;
      return;
    }
    prevWP=tmpWP;
    tmpWP=tmpWP->next;
  }
}

// 设置监视点表达式与姓名
void set_wp(WP *inputWP,char *wpName,char *inputExp){
  free(inputWP->wpName);
  free(inputWP->exp);
  inputWP->wpName=wpName;
  inputWP->exp=inputExp;
}

// 监控监视点的值是否发生变化
WP *monitor_wp(){
  if(!head){
    return NULL;
  }
  WP *changingWP=head;

  // 获取每个监视点的值，并与过去值比较
  while(changingWP!=NULL){
    // 如果未初始化，则初始化
    if(!changingWP->initialState){
      changingWP->resultValue = expr(changingWP->exp,NULL);
      changingWP->initialState=true;
      wpValueGroup[changingWP->NO]=changingWP->resultValue;
      changingWP=changingWP->next;
      continue;
    }
    // 进行计算，然后与旧值对比
    changingWP->resultValue=expr(changingWP->exp,NULL);
    // 如果发生变化则返回监视点对象
    if(changingWP->resultValue!=wpValueGroup[changingWP->NO]){
      return changingWP;
    }
    changingWP=changingWP->next;
  }

  return NULL;
}

// 列出节点
void list_wp(){
  WP *changingWP=head;

  printf("Num\tName\tinitialState\texp\tvalue\n");
  while(changingWP!=NULL){
    printf("%d\t%s\t%d\t\t%s\t%d\n",changingWP->NO,changingWP->wpName,changingWP->initialState,changingWP->exp,changingWP->resultValue);
    changingWP=changingWP->next;
  }
  printf("\n");
}

