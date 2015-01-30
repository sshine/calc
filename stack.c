#include <stdlib.h>
#include <assert.h>
#include "stack.h"

void stack_init(struct stack_t *stack)
{
  stack->size = 0;
  stack->top = NULL;
}

void stack_push(struct stack_t *stack, void *val)
{
  struct elem_t *elem = malloc(sizeof(struct elem_t));

  elem->val = val;
  elem->next = stack->top;
  stack->top = elem;
  stack->size += 1;
}

void *stack_pop(struct stack_t *stack)
{
  void *retval;

  // It is an error that the struct stack_t * doesn't exist,
  // but if stack->top is NULL, the stack is simply empty and
  // NULL is returned.
  assert(stack != NULL);
  if (stack->top == NULL)
    return NULL;

  retval = stack->top->val;
  struct elem_t *old = stack->top;
  stack->top = old->next;
  free(old);
  stack->size -= 1;

  return retval;
}

int stack_size(struct stack_t *stack)
{
  assert(stack != NULL);
  return stack->size;
}

int stack_empty(struct stack_t *stack)
{
  return stack_size(stack) == 0;
}
