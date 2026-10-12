#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  panic("Not implemented");
}

char *strcpy(char *dst, const char *src) {
  char *ret = dst;
  while((*dst++ = *src++) != '\0');
  return ret;

}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) { //把src拼接到dst后面
  char *ret = dst;
  while(*dst != '\0'){
    dst++;
  }
  while((*dst++ = *src++) != '\0');
  return ret;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 != '\0' && (*s1 == *s2)){
    s1++;
    s2++;
  }
  return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  if(s == NULL || n == 0){
    return s;
  }
  unsigned char *p = (unsigned char *)s;
  unsigned char val = (unsigned char )c;
  for(size_t i = 0; i < n; i++){
    p[i] = val;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  if(out == NULL || in == NULL || n == 0){
    return out;
  }
  char *d = (char *)out;
  const char *s = (const char *)in;
  for(size_t i = 0; i < n; i++){
    d[i] = s[i];
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char *p1 = (const unsigned char *)s1;
  const unsigned char *p2 = (const unsigned char *)s2;
  for(size_t i = 0; i <n ; i++){
    if(p1[i] != p2[i]){
      return (int)p1[i] - (int)p2[i];
    }
  }
  return 0;
}

#endif
