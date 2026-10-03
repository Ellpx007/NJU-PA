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

#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

word_t expr(char *e, bool *success);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

  FILE *fp = fopen("tools/gen-expr/input", "r");
  assert(fp != NULL);

  word_t expected;
  char buf[65536];
  // %u 读结果数值，%[^\n] 读整行表达式（包含空格）
  while (fscanf(fp, "%u %[^\n]", &expected, buf) == 2) {
    bool success = false;
    word_t val = expr(buf, &success);
    if (!success || val != expected) {
    printf("\n\033[1;31m[FAILED]\033[0m Expr: \"%s\"\n", buf);
    printf("         Expected: %u, Actual: %u, Success: %d\n", expected, val, success);
    assert(0);
  }
  }
  fclose(fp);
  printf("\033[1;32m[PASS]\033[0m All test cases passed successfully!\n");
  return 0; // 测试通过直接退出，不需要进入 NEMU 交互终端

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
