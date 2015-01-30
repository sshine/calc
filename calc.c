#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <gmp.h>
#include "stack.h"

enum token_type_t { PLUS, MINUS, TIMES, DIVIDE, NUMBER };

struct token_t {
  enum token_type_t type;
  mpz_t *num;
};

struct token_t *new_token(enum token_type_t token_type, mpz_t *num)
{
  struct token_t *new = malloc(sizeof(struct token_t));
  new->type = token_type;
  new->num = num;
  return new;
}

void destroy_token(struct token_t *token)
{
  if (token != NULL) {
    free(token->num); // FIXME: mpz_t * should be properly freed.
    token->num = NULL;
    free(token);
  }
}

int isDigit(char c, int base)
{
  assert (base == 10);
  return c >= '0' && c <= '9';
}

// FIXME: Make a queue instead.
void reverse(struct stack_t *stack)
{
  struct stack_t tmp;
  stack_init(&tmp);

  while (!stack_empty(stack)) {
    stack_push(&tmp, stack_pop(stack));
  }
  stack->size = tmp.size;
  stack->top = tmp.top;
}

void lex(struct stack_t *stack, char *line, int max, int base)
{
  int i = 0;
  char *num_start = NULL;
  char tmp;
  mpz_t *num;

  for (; i < max && line[i] != '\0'; i++) {
    // printf("Got '%c' at %d\n", line[i], i);
    if (isDigit(line[i], base)) {
      // In case we're beginning a number...
      if (num_start == NULL) {
        num_start = line + i;
      }
    } else {
      // In case we're ending a number...
      if (num_start != NULL) {
        // Temporarily "end the string"
        tmp = line[i];
        line[i] = '\0';

        // Create a bignum from input line (stop at end of number)
        num = malloc(sizeof(mpz_t));
        mpz_init_set_str(*num, num_start, base);
        stack_push(stack, (void *)new_token(NUMBER, num));

        // Continue analysing input line
        line[i] = tmp;
        num = NULL;
        num_start = NULL;
      }

      // In any other case...
      switch (line[i]) {
      case '+': stack_push(stack, (void *)new_token(PLUS, NULL)); break;
      case '-': stack_push(stack, (void *)new_token(MINUS, NULL)); break;
      case '*': stack_push(stack, (void *)new_token(TIMES, NULL)); break;
      case '/': stack_push(stack, (void *)new_token(DIVIDE, NULL)); break;
      case ' ': continue;
      default:
        if (!isspace(line[i]))
          fprintf(stderr, "Unknown character '%c' on position %d of line.\n", line[i], i);
      }
    }
  }
}

int eval(struct stack_t *token_stack, struct stack_t *eval_stack)
{
  while (!stack_empty(token_stack)) {
    struct token_t *op = stack_pop(token_stack);
    mpz_t *a, *b;

    assert(op != NULL);

    switch (op->type) {
    case NUMBER:
      assert(op->num != NULL);
      printf("Pushing %s to the stack.\n", mpz_get_str(NULL, 10, *(op->num)));
      stack_push(eval_stack, op->num);

      // The mpz_t * that was called op->num still exists, but
      // its pointer in op is removed so assertions will fail
      // if op->num is accidentally accessed beyond here:
      op->num = NULL;
      free(op);
      break;

    case PLUS:
      if (stack_size(eval_stack) < 2) {
        fprintf(stderr, "Not enough operands on stack.\n");
        return 0;
      }

      a = (mpz_t *)stack_pop(eval_stack);
      b = (mpz_t *)stack_pop(eval_stack);

      assert(a != NULL);
      assert(b != NULL);

      printf("Adding %s and %s to give ", mpz_get_str(NULL, 10, *a),
                                          mpz_get_str(NULL, 10, *b));

      mpz_add(*a, *a, *b);
      printf("%s.\n", mpz_get_str(NULL, 10, *a));
      free(b); // FIXME: Does not really free mpz_t data. Make mpz_free()
      stack_push(eval_stack, a);
      break;

    case MINUS:
      a = (mpz_t *)stack_pop(eval_stack);
      b = (mpz_t *)stack_pop(eval_stack);
      if (a == NULL || b == NULL) {
        fprintf(stderr, "Not enough operands on stack.\n");
        return 0;
      }

      printf("Subtracting %s from %s to give ", mpz_get_str(NULL, 10, *a),
                                                mpz_get_str(NULL, 10, *b));

      mpz_sub(*a, *a, *b);
      printf("%s.\n", mpz_get_str(NULL, 10, *a));
      free(b); // FIXME: Does not really free mpz_t data. Make mpz_free()
      stack_push(eval_stack, a);
      break;

    default:
      fprintf(stderr, "Only numbers, plus and minus are supported.\n");
      break;
    }
  }
  return 1;
}

int main(int argc, char *argv[])
{
  struct stack_t token_stack, eval_stack;
  char *line = NULL;
  size_t len = 0;

  stack_init(&token_stack);
  stack_init(&eval_stack);
  while (getline(&line, &len, stdin) >= 0) {
    lex(&token_stack, line, len, 10);
    reverse(&token_stack); // FIXME: Use a queue instead.
    eval(&token_stack, &eval_stack);

    /* // Instead of evaluating, one could inspect the token list:
    while (!stack_empty(&token_stack)) {
      struct token_t *token = (struct token_t *)stack_pop(&token_stack);
      printf("Popped %s.\n", mpz_get_str(NULL, 10, *(token->num)));
    }
    */
  }
  return 0;
}
