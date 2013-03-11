/* 
 * Copyright (C) 2013  Inori Sakura <inorindesu@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <stack.h>
#include <stdlib.h>

/*
 * Data structures
 */

typedef struct node_t
{
  struct node_t* next;
  int data;
}node_t;

/*
 *    head -> old -> ...
 *
 *    new -> head -> old ...
 */

typedef struct stack_t
{
  node_t *head;
}stack_t;

/*
 * functions
 */

stack_t* stack_new()
{
  stack_t* s = malloc(sizeof(stack_t));
  s->head = NULL;
  return s;
}

bool stack_is_empty(stack_t* stack)
{
  return stack->head == NULL;
}

void stack_destroy(stack_t* stack)
{
  stack_clear(stack);
  free(stack);
}

void stack_push(stack_t* stack, int item)
{
  node_t** nodep = &stack->head;
  if(*nodep)
    {
      node_t* node = malloc(sizeof(node_t));
      node->data = item;
      node->next = (*nodep);
      *nodep = node;
    }
  else
    {
      *nodep = malloc(sizeof(node_t));
      (*nodep)->next = NULL;
    }
}

int stack_pop(stack_t* stack)
{
  node_t** nodep = &stack->head;
  if(*nodep)
    {
      int r = (*nodep)->data;
      node_t* f = *nodep;
      *nodep = f->next;
      free(f);
      return r;
    }
}

void stack_clear(stack_t* stack)
{
  node_t** nodep = &stack->head;
  while(*nodep)
    {
      node_t* f = *nodep;
      *nodep = f->next;
      free(f);
    }
}
