#ifndef _STACK_H
#define _STACK_H

struct stack_t {
  struct elem_t *top;
  int size;
};

struct elem_t {
  void *val;
  struct elem_t *next;
};

void stack_init(struct stack_t *);
void stack_push(struct stack_t *, void *);
void *stack_pop(struct stack_t *);
int stack_empty(struct stack_t *);
int stack_size(struct stack_t *);

#endif /* _STACK_H */
