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
#include <stdio.h>
#include <stdbool.h>

/*
 * create a stack, then free it
 */
static void test_stack_life1()
{
  fprintf(stderr, "[stack] life1..");
  stack_t* stack = stack_new();
  stack_destroy(stack);
  fprintf(stderr, "done!\n");
}

/*
 * create a stack, add some elements, then free it
 */
static void test_stack_life2()
{
  fprintf(stderr, "[stack] life2..");
  stack_t* stack = stack_new();
  stack_push(stack, 33);
  stack_push(stack, 34);
  stack_destroy(stack);
  fprintf(stderr, "done!\n");
}

static void test_stack_is_empty1()
{
  fprintf(stderr, "[stack] is_empty..");
  stack_t* stack = stack_new();
  bool isEmpty = stack_is_empty(stack);
  stack_destroy(stack);
  if (isEmpty == false)
    {
      fprintf(stderr, "failed!\n");
    }
  else
    {
      fprintf(stderr, "done!\n");
    }
}

#define POP_TEST_SIZE (1048576)

static void test_stack_pop()
{
  fprintf(stderr, "[stack] popping..");
  stack_t* stack = stack_new();
  for(int i = 0; i < POP_TEST_SIZE; i++)
    {
      stack_push(stack, i);
    }
  int failNum = -1;
  int popResult = 0;
  for(int i = POP_TEST_SIZE - 1; i >= 0; i--)
    {
      popResult = stack_pop(stack);
      if(popResult != i)
        {
          failNum = i;
          break;
        }
    }
  bool isEmpty = stack_is_empty(stack);
  stack_destroy(stack);
  
  if (failNum == -1 && isEmpty == true)
    {
      fprintf(stderr, "done!\n");
    }
  else
    {
      if (failNum != -1)
        fprintf(stderr, "failed! (failed at %d, pop returns %d)\n", failNum, popResult);
      else if(isEmpty == false)
        fprintf(stderr, "failed! (isEmpty assertion)\n");
    }
}

void run_stack_test()
{
  fprintf(stderr, "[stack] stack test started\n");
  test_stack_life1();
  test_stack_life2();
  test_stack_is_empty1();
  test_stack_pop();
  fprintf(stderr, "[stack] stack test ended\n");
}
