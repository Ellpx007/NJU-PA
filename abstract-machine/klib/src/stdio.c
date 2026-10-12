#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

//Integer to ASCII
static char *itoa(char *buf, int val){
  char tmp[16];
  int i = 0;
  uint32_t num;

  if(val < 0){
    *buf++ = '-';
    num = (uint32_t)(-val + 1) + 1;//c语言int负数比正数多一个，-1再+1规避这个问题
  } else {
    num = (uint32_t)val;
  }
  if(num == 0){ 
    *buf++ = '0';
    return buf;
  }
  while(num > 0){
    tmp[i++] = (num % 10) + '0';//取出每一位整数 +'0'->ASCII
    num /= 10;
  }
  while(i > 0){
    *buf++ = tmp[--i];
  }
  return buf;
}

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *start = out;

  while (*fmt != '\0'){
    if(*fmt != '%'){
      *out++ = *fmt++;
      continue;
    }

    fmt++; //跳过%

    switch(*fmt){
      case 's':{ //String
        const char *s = va_arg(ap, const char *);
        if(!s) s = "(null)";
        while(*s != '\0'){
          *out++ = *s++;
        }
        break;
      }
      case 'd':{ //Int
        int d = va_arg(ap, int);
        out = itoa(out, d);
        break;
      }
      case '%':{
        *out++ = '%';
        *out++ = *fmt;
        break;
      }
    }

    fmt++;
  }
  *out = '\0';
  return out - start;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
