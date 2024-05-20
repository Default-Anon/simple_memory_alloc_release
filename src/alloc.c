#include "../include/alloc.h"
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>

typedef union header header_t;
header_t *head = NULL, *tail = NULL;

void *my_malloc(size_t sz) {
  if (!sz)
    return NULL;
  int total_sz = sz + sizeof(header_t);
  void *new_block = sbrk(total_sz);
  header_t *curr;
  if (new_block == (void *)-1) {
    return NULL;
  }
  if (head == NULL) {
    head = new_block;
    default_init_header_t_union(&head, &new_block, total_sz);
    tail = head;
    return new_block;
  } else {
    curr = head;
    if (head != tail) {
      while (curr->s.next) {
        curr = curr->s.next;
      }
      curr->s.next = new_block;
      tail = curr->s.next;
      default_init_header_t_union(&tail, &new_block, total_sz);
      return new_block;
    }
    head->s.next = new_block;
    tail = head->s.next;
    default_init_header_t_union(&tail, &new_block, total_sz);
  }
  return new_block;
}
int my_free(void *space) {
  header_t *curr = head, *tmp;
  while (curr != (header_t *)((char *)space - sizeof(header_t)) &&
         curr->s.next != NULL) {
    tmp = curr;
    curr = curr->s.next;
  }
  if (curr == NULL) {
    fprintf(stderr, "Error: this pointer don't refer to dynamic memory\n");
    return -1;
  } else if (curr == tail && curr == head) {
    tail = head = NULL;
    if ((void *)sbrk(0 - curr->s.size - sizeof(header_t))) {
      fprintf(stdout, "clean success\n");
    }
  } else if (curr == tail) {
    tail = tmp;
    if ((void *)sbrk(0 - curr->s.size - sizeof(header_t))) {
      fprintf(stdout, "clean success\n");
    }
  } else {
    curr->s.is_free = 1;
    fprintf(stdout,
            "element is not in the end, memory will be marked as free\n");
  }
  return 0;
}
void *my_realloc(void *block, size_t size) {
  header_t *free_block;
  header_t *exist_block;
  int total_sz = size + sizeof(header_t);
  exist_block = head;
  while (exist_block) {
    if (exist_block == (header_t *)block - 1) {
      break;
    }
    exist_block = exist_block->s.next;
  }
  if (exist_block == NULL) {
    fprintf(stderr, "Ptr not refer to dynamic space\n");
    return NULL;
  }
  free_block = get_free_block(total_sz);
  if (free_block) {
    free_block += sizeof(header_t);
    return free_block;
  } else {
    free_block = (header_t *)my_malloc(total_sz - sizeof(header_t));
    if (!free_block) {
      fprintf(stderr, "Unexpected error, malloc for realloc\n");
    }
    exist_block->s.is_free = 1;
  }
  return free_block;
}
void *my_calloc(int num, size_t num_sz) {
  void *free_block;
  header_t *last_block;
  int total_sz = sizeof(header_t) + num * num_sz;
  free_block = get_free_block(total_sz);
  if (free_block) {
    free_block += sizeof(header_t);
    memset((char *)free_block, 0, sizeof(num * num_sz));
    return free_block;
  }
  free_block = sbrk(total_sz);
  if (free_block == (void *)-1) {
    fprintf(stderr, "Sbrk error, memory not initialized\n");
  }
  if (tail == NULL) {
    head = tail = free_block;
    default_init_header_t_union(&tail, &free_block, total_sz);
  } else {
    tail->s.next = free_block;
    tail = free_block;
    default_init_header_t_union(&tail, &free_block, total_sz);
  }
  return free_block;
}
void default_init_header_t_union(header_t **header_t_ptr, void **free_block,
                                 int total_sz) {
  (*header_t_ptr)->s.size = total_sz - sizeof(header_t);
  (*header_t_ptr)->s.is_free = 0;
  (*header_t_ptr)->s.next = NULL;
  *free_block = (char *)*free_block + sizeof(header_t);
}
header_t *get_free_block(int sz) {
  header_t *curr = head;
  while (curr) {
    if (curr->s.is_free && curr->s.size >= sz)
      return curr;
    curr = curr->s.next;
  }
  return NULL;
}
