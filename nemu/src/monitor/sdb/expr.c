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

enum {
  TK_NOTYPE = 256, TK_EQ, TK_INT, TK_NEG

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
  {"\\(" , '('},
  {"\\)" , ')'},
  {"\\*" , '*'},        // mul
  {"/" , '/'},          // div
  {"==", TK_EQ},        // equal
  {"\\+", '+'},         // plus
  {"-" , '-'},          // sub
  {"[0-9]+[uU]?", TK_INT}    // int
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

static Token tokens[256] __attribute__((used)) = {};
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

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE: break;
          case TK_EQ:  tokens[nr_token].type = TK_EQ ; nr_token++; break;
          case TK_INT: {
              tokens[nr_token].type = TK_INT ; 
              strncpy(tokens[nr_token].str, e + position - substr_len , substr_len);
              tokens[nr_token].str[substr_len] = '\0';
              nr_token++;
              break;}
          case '+': tokens[nr_token].type = '+'; nr_token++; break;
          case '-': tokens[nr_token].type = '-'; nr_token++; break;
          case '*': tokens[nr_token].type = '*'; nr_token++; break;
          case '/': tokens[nr_token].type = '/'; nr_token++; break;
          case '(': tokens[nr_token].type = '('; nr_token++; break;
          case ')': tokens[nr_token].type = ')'; nr_token++; break;

          default: TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  // printf测试正则表达式
  /*for(int i=0; i<nr_token; i++)
  {
    printf("tokens[%d]: type = %d , str = \"%s\"\n" , i , tokens[i].type , tokens[i].str);
  }
  */

  return true;
}

static bool check_parentthese(int p , int q){ //去除括号
  if(tokens[p].type != '(' || tokens[q].type != ')'){
    return false;
  }
  int depth = 0;
  for(int i = p; i < q ; i++){
    if(tokens[i].type == '('){
      depth++;
    }
    else if(tokens[i].type == ')')
    {
      depth--;
    }
    if(depth == 0){
      return false;
    }
  }

  return depth == 1;
}

static int get_priority(int type){
  switch(type){
    case '+' :
    case '-' :
    return 1;

    case '*' :
    case '/' :
    return 2;

    case TK_NEG :
    return 3;

    default :
    //printf("Unexpected token type in get_priority: %d\n", type);
    return -1;
  }
}

static int find_main_op(int p , int q){
  int op = -1;
  int l_priority = 5;
  int depth = 0;

  for(int i = p; i < q; i++){
    if(tokens[i].type == '('){
      depth++;
    }

    else if(tokens[i].type == ')'){
      depth--;
    }

    else if(depth == 0){
      int priority = get_priority(tokens[i].type);
      bool is_neg = (tokens[i].type == TK_NEG);

      if(priority == -1){ 
        continue;
      }
     
      if(priority < l_priority || (priority == l_priority && !is_neg)){
        l_priority = priority;
        op = i;
      }
    }
  } 
  return op;
}
static word_t eval(int p , int q){ //计算表达式的值，p->开始的token，q->结束的token
  if(p > q) {
    printf("This is wrong!");
    assert(0);
    return 0;
  }

  else if(p == q){ 
    assert(tokens[p].type == TK_INT);
    return strtoul(tokens[p].str, NULL, 0);
  }

  else if(check_parentthese(p , q) == true ){ //去除表达式中的括号
    return eval(p + 1 , q - 1);
  } 

  else {
    int op = find_main_op(p , q);
    assert(op != -1);

    if(tokens[op].type == TK_NEG){
      word_t val3 = eval(op + 1 ,q);
      return -val3;
    }

    word_t val1 = eval(p , op - 1);
    word_t val2 = eval(op + 1 , q);

    switch(tokens[op].type){  
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      default: 
      assert(0);
    }
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  for(int i = 0; i < nr_token; i++){
    if(tokens[i].type == '-'){
      if(i == 0 || (tokens[i-1].type != TK_INT && tokens[i-1].type != ')')){
        tokens[i].type = TK_NEG;
      }
    }
  }
  *success = true;
  return eval(0, nr_token - 1);
} 