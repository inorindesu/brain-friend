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

#include <stdio.h>
#include <opcodes.h>

#define TRA_COUNT (1000)

static void opcode_list_life1()
{
  fprintf(stderr, "[opcodes] life1...");
  opcode_list_t* ol = opcode_list_new();
  bool isEmpty = opcode_list_is_empty(ol);
  opcode_list_destroy(ol);
  if (isEmpty == false)
    {
      fprintf(stderr, "failed (isempty)\n");
    }
  else
    {
      fprintf(stderr, "done!\n");
    }
}

static void opcode_list_life2()
{
  fprintf(stderr, "[opcodes] life2...");
  opcode_list_t* ol = opcode_list_new();
  for(int i = 0; i < 100; i++)
    {
      opcode_list_add(ol, NOP, 0);
    }
  bool isEmpty= opcode_list_is_empty(ol);
  opcode_list_destroy(ol);
  if (isEmpty == true)
    {
      fprintf(stderr, "failed (isEmpty)\n");
    }
  else
    {
      fprintf(stderr, "done!\n");
    }
}

static void counter(inst_t inst, int parameter, void* userData)
{
  *(int*)userData += 1;
}

static void opcode_list_traverse1()
{
  fprintf(stderr, "[opcodes] traverse1...");
  opcode_list_t* ol = opcode_list_new();
  for(int i = 0; i < TRA_COUNT; i++)
    {
      opcode_list_add(ol, ADD, i);
    }
  int cnt = 0;
  opcode_list_traverse(ol, counter, &cnt);
  opcode_list_destroy(ol);
  if (cnt == TRA_COUNT)
    {
      fprintf(stderr, "done!\n");
    }
  else
    {
      fprintf(stderr, "failed!(count: %d, expected: %d)\n", cnt, TRA_COUNT);
    }
}

#define TEST_DATA_SIZE (4)
static inst_t testInst[] = {INIT, ADD, NEXT, DUMPSTAT};
static int testParam[] = {4096, 50, 2, 3};

static void tr2_check(inst_t instruction, int parameter, void* userData)
{
  int index = *(int*)userData;
  if (instruction != testInst[index] || parameter != testParam[index])
    {
      *(int*)userData = -1;
    }
  else
    {
      *(int*)userData += 1;
    }
}

static void opcode_list_traverse2()
{
  fprintf(stderr, "[opcodes] traverse2...");
  opcode_list_t* ol = opcode_list_new();
  for(int i = 0; i < TEST_DATA_SIZE; i++)
    {
      opcode_list_add(ol, testInst[i], testParam[i]);
    }
  int index = 0;
  opcode_list_traverse(ol, tr2_check, &index);
  opcode_list_destroy(ol);
  if (index == TEST_DATA_SIZE)
    {
      fprintf(stderr, "done!\n");
    }
  else
    {
      fprintf(stderr, "failed! (index = %d, expected = %d)\n", index, TEST_DATA_SIZE);
    }
}

void run_opcodes_test()
{
  fprintf(stderr, "[opcodes] opcodes test started\n");
  opcode_list_life1();
  opcode_list_life2();
  opcode_list_traverse1();
  opcode_list_traverse2();
  fprintf(stderr, "[opcodes] opcodes test ended\n");
}
