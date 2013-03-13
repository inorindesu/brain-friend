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

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <opcodeCompiler.h>
#include <stack.h>

typedef struct opcode_compiler_t
{
  stack_t* loop;
  bool compilationDone;
  bool compilationStarted;
  bool insertStateDumper;
  int lineNo;
  int columnNo;
  char* errorMsg;
}opcode_compiler_t;

opcode_compiler_t* opcode_compiler_new()
{
  opcode_compiler_t* c = malloc(sizeof(opcode_compiler_t));
  c->loop = stack_new();
  c->compilationDone = false;
  c->compilationStarted = false;
  c->insertStateDumper = false;
  c->lineNo = 0;
  c->columnNo = 0;
  c->errorMsg = NULL;
  return c;
}

void opcode_compiler_set_insert_state_dumper(opcode_compiler_t* c, bool insertStateDumper)
{
  c->insertStateDumper = insertStateDumper;
}

void opcode_compiler_reset(opcode_compiler_t* c)
{
  while(stack_is_empty(c->loop) == false)
    {
      opcode_list_t* opcodes = stack_pop(c->loop);
      opcode_list_destroy(opcodes);
    }
  c->lineNo = 0;
  c->columnNo = 0;
  c->compilationDone = false;
  c->insertStateDumper = false;
  c->compilationStarted = false;
  c->errorMsg = NULL;
}

void opcode_compiler_destroy(opcode_compiler_t* c)
{
  while(stack_is_empty(c->loop) == false)
    {
      opcode_list_t* ol = stack_pop(c->loop);
      opcode_list_destroy(ol);
    }
  stack_destroy(c->loop);
  free(c);
}

static void opcode_counter(inst_t i, int param, void* d)
{
  (*(int*)d) += 1;
}

static void opcode_copy(inst_t i, int param, void* d)
{
  opcode_list_add(d, i, param);
}

void opcode_compiler_feed_code(opcode_compiler_t* c, const char* code, unsigned int size)
{
  if (c->errorMsg)
    {
      return;
    }

  if (c->compilationStarted == false)
    {
      /* insert command for initialize execution environment */
      c->compilationStarted = true;
      opcode_list_t* ol = opcode_list_new();
      opcode_list_add(ol, INIT, 32767);
      stack_push(c->loop, ol);
    }
  
  for(int i = 0; i < size; i++)
    {
      int opcodeCount = 0;
      opcode_list_t* olEnd;
      opcode_list_t* olUpperLevel;
      if (code[i] == '\0')
        {
          return;
        }
      else
        {
          c->columnNo += 1;
          switch(code[i])
            {
            case '\n':
              c->lineNo += 1;
              c->columnNo = 0;
              continue;
            case '[':
              stack_push(c->loop, opcode_list_new());
              break;
            case ']':
              olEnd = stack_pop(c->loop);
              if (stack_is_empty(c->loop))
                {
                  c->errorMsg = "Unpaired ] found";
                  return;
                }
              olUpperLevel = stack_peek(c->loop);
              opcode_list_traverse(olEnd, opcode_counter, &opcodeCount);
              if (c->insertStateDumper)
                {
                  opcode_list_add(stack_peek(c->loop), DUMPSTAT, 0);
                }
              opcode_list_add(olUpperLevel, BZ, opcodeCount + 1);
              opcode_list_traverse(olEnd, opcode_copy, olUpperLevel);
              opcode_list_destroy(olEnd);
              break;
            case '>':
              if (c->insertStateDumper)
                {
                  opcode_list_add(stack_peek(c->loop), DUMPSTAT, 0);
                }
              opcode_list_add(stack_peek(c->loop), NEXT, 1);
              break;
            case '<':
              if (c->insertStateDumper)
                {
                  opcode_list_add(stack_peek(c->loop), DUMPSTAT, 0);
                }
              opcode_list_add(stack_peek(c->loop), PREV, 1);
              break;
            case '+':
              if (c->insertStateDumper)
                {
                  opcode_list_add(stack_peek(c->loop), DUMPSTAT, 0);
                }
              opcode_list_add(stack_peek(c->loop), ADD, 1);
              break;
            case '-':
              if (c->insertStateDumper)
                {
                  opcode_list_add(stack_peek(c->loop), DUMPSTAT, 0);
                }
              opcode_list_add(stack_peek(c->loop), SUB, 1);
              break;
            case '.':
              if (c->insertStateDumper)
                {
                  opcode_list_add(stack_peek(c->loop), DUMPSTAT, 0);
                }
              opcode_list_add(stack_peek(c->loop), OUT, 0);
              break;
            case ',':
              if (c->insertStateDumper)
                {
                  opcode_list_add(stack_peek(c->loop), DUMPSTAT, 0);
                }
              opcode_list_add(stack_peek(c->loop), IN, 0);
              break;
            default:
              /* other characters are treated as comments*/
              break;
            }
        }
    }
}

void opcode_compiler_done_compilation(opcode_compiler_t* c)
{
  if (c->insertStateDumper)
    {
      opcode_list_add(stack_pop(c->loop), DUMPSTAT, 0);
    }

  opcode_list_t* ol = stack_pop(c->loop);

  if(stack_is_empty(c->loop) == false)
    {
      c->errorMsg = "Unpaired [ found.";
    }
  else
    {
      c->compilationDone = true;
    }
  stack_push(c->loop, ol);
}

opcode_list_t* opcode_compiler_result_new(opcode_compiler_t* c)
{
  opcode_list_t* ol = opcode_list_new();
  opcode_list_traverse(stack_peek(c->loop), opcode_copy, ol);
  return ol;
}

int opcode_compiler_get_current_line_number(opcode_compiler_t* c)
{
  return c->lineNo;
}

int opcode_compiler_get_current_column_number(opcode_compiler_t* c)
{
  return c->columnNo;
}

char* opcode_compiler_get_error_new(opcode_compiler_t* c)
{
  if (c->errorMsg)
    {
      int sz = strlen(c->errorMsg) + 1;
      char* buf = malloc(sizeof(char) * sz);
      strncpy(buf, c->errorMsg, sz - 1);
      return buf;
    }
  else
    {
      return NULL;
    }
}
