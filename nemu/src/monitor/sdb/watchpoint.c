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
/*
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  // TODO: Add more members if necessary
  char expr[128];
  word_t old_val;
} WP;  */

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(){ //将free_（空闲）中的节点转移到head（工作）
  if(free_ == NULL){
  assert(0);
  }
  WP *wp = free_;
  free_ = free_ -> next;
  wp -> next = head;
  head = wp;

  return wp;
}

void free_wp(WP *wp){
  assert(wp != NULL);
  if(head == wp){ //要移除的是第一个节点，直接指向后一个节点
    head = head -> next;
  } else {
    WP *curr = head;
    while (curr != NULL && curr -> next != wp){
      curr = curr -> next;
    }
    assert(curr != NULL);
    curr -> next = wp -> next;
  }
  wp -> expr[0] = '\0';
  wp -> old_val = 0;
  wp -> next = free_;
  free_ = wp;
}

WP* add_watchpoint(char *e, word_t val){
  WP *wp = new_wp();
  strncpy(wp->expr, e, sizeof(wp->expr) - 1);
  wp->expr[sizeof(wp -> expr) - 1] = '\0';
  wp->old_val = val;
  return wp;    
}

bool delete_watchpoint(int no){
  WP *curr = head;
  while(curr != NULL){
    if(curr->NO == no){
      free_wp(curr);
      return true;
    }
    curr = curr->next;
  }
  return false;
}

void print_watchpoints(){
  if(head == NULL){
    printf("There is no watchpoint\n");
    return;
  }
  printf("%-8s %-16s %-16s %s\n", "Num", "Type", "Disp", "What");
  WP *curr = head;
  while(curr != NULL){
    printf("%-8d %-16s %-16s %s (值: %u / 0x%08x)\n",
           curr->NO, "watchpoint", "keep", curr->expr, curr->old_val, curr->old_val);
    curr = curr->next;
  }
}

bool check_watchpoints(){
  bool changed = false;
  WP *curr = head;
  while(curr != NULL){
    bool success = false;
    word_t new_val = expr(curr->expr, &success);
    if(!success){
      printf("警告: 监视点 %d 的表达式求值失败: \"%s\"\n", curr->NO, curr->expr);
      curr = curr->next;
      continue;
    }
    if(new_val != curr->old_val){
      printf("\n触发监视点 %d: %s\n", curr->NO, curr->expr);
      printf("旧值 = %u (0x%08x)\n", curr->old_val, curr->old_val);//打印十进制以及十六进制
      printf("新值 = %u (0x%08x)\n", new_val, new_val);
      curr->old_val = new_val;
      changed = true;
    }
    curr = curr->next;
  }
  return changed;
}