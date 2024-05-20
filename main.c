#include "include/alloc.h"

int main() {
  int a = 434;
  int *b, *c, *d;
  printf("a is %p\n", &a);
  b = (int *)my_malloc(sizeof(int));
  *b = 4343;
  printf("b is %p\n", b);
  b = my_realloc(b, 16);
  *b = 98984;
  printf("b is %p\n", b);
  c = (int *)my_calloc(34, sizeof(int));
  c[1] = 4324;
  d = (int *)my_malloc(sizeof(int));
  printf("c is %p\n", c);
  printf("d is %p\n", d);
  d = my_realloc(d, 8);
  my_free(d);
  my_free(c);
  my_free(b);
  return 0;
}
