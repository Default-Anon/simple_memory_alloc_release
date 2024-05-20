#include <stdio.h>
#include <unistd.h>

typedef char ALIGNED[16];
union header {
  struct {
    unsigned size;
    int is_free;
    union header *next;
  } s;
  ALIGNED arr;
};

int my_free(void *space);
void *my_malloc(size_t sz);
union header *get_free_block(int sz);
void *my_realloc(void *block, size_t size);
void *my_calloc(int num, size_t num_sz);
void default_init_header_t_union(union header **header_t_ptr, void **free_block,
                                 int total_sz);
