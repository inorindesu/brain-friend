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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stack.h>
#include <stdbool.h>
#include <opcodes.h>
#include <opcodeCompiler.h>

typedef struct opcode_t
{
  inst_t instruction;
  int param;
}opcode_t;

typedef struct opcode_buffer_t
{
  opcode_t* opcodes;
  int currIndex;
  int size;
}opcode_buffer_t;

typedef struct state_t
{
  char* data;
  int dataIdx;
  bool debugMode;
  int dataSize;
}state_t;

static void opcode_copy(inst_t instruction, int param, void* p)
{
  opcode_buffer_t* copyStruct = p;
  opcode_t* opcode = &(copyStruct->opcodes[copyStruct->currIndex]);
  opcode->instruction = instruction;
  opcode->param = param;
  copyStruct->currIndex += 1;
}

static void opcode_count(inst_t instruction, int param, void* s)
{
  (*(int*)s) += 1;
}

#define LINE_BUFFER_SIZE 16

int main(int argc, char** argv)
{
  /*
   * Processing command line argument.
   */
  bool debug = false;  /* dump stat switch */
  bool optimize = false;
  
  for(int i = 1; i < argc; i++)
    {
      if (strcmp(argv[i], "-d") == 0)
        {
          debug = true;
        }
      else if(strcmp(argv[i], "-o") == 0)
        {
          optimize = true;
        }
      else if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
          fprintf(stdout, "Usage: %s [-d|-o] [-h]\n\n", argv[0]);
          fprintf(stdout, "-d     Dump data near current data point after each instruction.\n");
          fprintf(stdout, "-o     Do optimization\n");
          fprintf(stdout, "       (Beware that -d and -o are incompatible switches)\n\n");
          fprintf(stdout, "-h     Show this help message\n");
          return 0;
        }
      else
        {
          fprintf(stderr, "[INTERPRETER] Warning: ignoring unknown switch '%s'.\n", argv[i]);
        }
    }

  if (debug && optimize)
    {
      fprintf(stderr, "[INTERPRETER] Warning: ignoring optimization flag.");
      optimize = false;
    }

  /*
   * read string line by line.
   */
  char* buffer = calloc(sizeof(char), LINE_BUFFER_SIZE);
  int len;
  opcode_compiler_t* compiler = opcode_compiler_new();
  state_t* state = malloc(sizeof(state));
  state->data = NULL;
  state->debugMode = debug;
  stack_t* bracket = stack_new();
  
  /* Execution loop */
  while(true)
    {
      if (optimize)
        {
          opcode_compiler_set_optimization(compiler, true);
        }
      if (debug)
        {
          opcode_compiler_set_insert_state_dumper(compiler, true);
        }
      /* code collector */
      bool eofBreak = false;
      while(true)
        {
          bool hasNewLine = false;
          for(len = 1; len <= LINE_BUFFER_SIZE; len++)
            {
              int chi = getchar();
              if (chi == EOF)
                {
                  eofBreak = true;
                  break;
                }
              char ch = chi;
              buffer[len - 1] = ch;
              /* bracket checker */
              if (ch == '[')
                {
                  stack_push(bracket, NULL);
                }
              else if (ch == ']')
                {
                  stack_pop(bracket);
                }
              
              /* newLine checker*/
              if (ch == '\n')
                {
                  hasNewLine = true;
                  break;
                }
            }
          opcode_compiler_feed_code(compiler, buffer, len);
          char* err;
          err = opcode_compiler_get_error_new(compiler);
          if (err != NULL)
            {
              fprintf(stderr, "ERROR: %s\n", err);
              free(err);
              return -1;
            }
          /*
           * Only when the loop is closed and a newline character is
           * found, interpreter can then compile the codes into
           * opcodes and run them.
           */
          if (hasNewLine && stack_is_empty(bracket))
            {
              break;
            }
          
          /* To process what is left in the buffer when EOF is reached*/
          if (eofBreak)
            {
              break;
            }
        }
      
      char* err;
      opcode_compiler_done_compilation(compiler);
      err = opcode_compiler_get_error_new(compiler);
      if (err != NULL)
        {
          fprintf(stderr, "ERROR: %s\n", err);
          free(err);
          return -1;
        }
      opcode_list_t* result = opcode_compiler_result_new(compiler);

      /* copy compilation result */
      int opcodeCount = 0;
      opcode_list_traverse(result, opcode_count, &opcodeCount);
      opcode_buffer_t* instructionBuffer = malloc(sizeof(opcode_buffer_t));
      instructionBuffer->opcodes = malloc(sizeof(opcode_t) * opcodeCount);
      instructionBuffer->currIndex = 0;
      instructionBuffer->size = opcodeCount;
      opcode_list_traverse(result, opcode_copy, instructionBuffer);

      /* Run compiled codes */
      instructionBuffer->currIndex = 0;
      while(true)
        {
          opcode_t opcode = instructionBuffer->opcodes[instructionBuffer->currIndex];
          int i;
          int printCount = 0;
          //fprintf(stderr, "POS: %d\n", instructionBuffer->currIndex);
          switch(opcode.instruction)
            {
            case NOP:
              break;
            case B:
              instructionBuffer->currIndex += opcode.param;
              break;
            case BZ:
              if (state->dataIdx < 0 || state->dataIdx >= state->dataSize)
                {
                  fprintf(stderr, "[INTERPRETER] ERROR: trying to read illegal data position %d\n", state->dataIdx);
                  return -1;
                }
              else
                {
                  if (state->data[state->dataIdx] == 0)
                    {
                      instructionBuffer->currIndex += opcode.param;
                    }
                  else
                    {
                      instructionBuffer->currIndex += 1;
                    }
                }
              break;
            case ADD:
              if (state->dataIdx < 0 || state->dataIdx >= state->dataSize)
                {
                  fprintf(stderr, "[INTERPRETER] ERROR: trying to write to illegal data position %d\n", state->dataIdx);
                  return -1;
                }
              else
                {
                  state->data[state->dataIdx] += opcode.param;
                }
              instructionBuffer->currIndex += 1;
              break;
            case NEXT:
              state->dataIdx += opcode.param;
              instructionBuffer->currIndex += 1;
              break;
            case IN:
              if (state->dataIdx < 0 || state->dataIdx >= state->dataSize)
                {
                  fprintf(stderr, "[INTERPRETER] ERROR: trying to write to illegal data position %d\n", state->dataIdx);
                  return -1;
                }
              else
                {
                  state->data[state->dataIdx] = getchar();
                  instructionBuffer->currIndex += 1;
                }
              break;
            case OUT:
              if (state->dataIdx < 0 || state->dataIdx >= state->dataSize)
                {
                  fprintf(stderr, "[INTERPRETER] ERROR: trying to read illegal data position %d\n", state->dataIdx);
                  return -1;
                }
              else
                {
                  fputc(state->data[state->dataIdx], stdout);
                  instructionBuffer->currIndex += 1;
                }
              break;
            case DUMPSTAT:
              fprintf(stdout, "Current Stat:\n");
              for(i = state->dataIdx - 20; i < state->dataIdx + 20; i++)
                {
                  if (i < 0 || i >= state->dataSize)
                    continue;

                  if (i != state->dataIdx)
                    fprintf(stdout, "%7d", state->data[i]);
                  else
                    fprintf(stdout, "[%5d]", state->data[i]);

                  printCount += 1;
                  if (printCount == 10)
                    {
                      fputc('\n', stdout);
                      printCount = 0;
                    }
                }
              if (printCount != 0)
                {
                  fputc('\n', stdout);
                }
              instructionBuffer->currIndex += 1;
              break;
            case INIT:
              if (state->data == NULL)
                {
                  state->data = calloc(sizeof(char), opcode.param);
                  state->dataSize = opcode.param;
                  instructionBuffer->currIndex += 1;
                }
              else
                {
                  if (state->debugMode)
                    {
                      /*Skip INIT and DUMPSTAT*/
                      instructionBuffer->currIndex += 2;
                    }
                  else
                    {
                      instructionBuffer->currIndex += 1;
                    }
                }
              break;
            default:
              fprintf(stderr, "[INTERPRETER] Warning: ignoring unkonwn instruction.");
              instructionBuffer->currIndex += 1;
              break;
            }
          if (instructionBuffer->currIndex >= instructionBuffer->size)
            {
              break;
            }
        }

      /* Cleanup */
      opcode_list_destroy(result);
      opcode_compiler_reset(compiler);
      free(instructionBuffer->opcodes);
      free(instructionBuffer);

      if (eofBreak)
        {
          break;
        }
    }
  free(state->data);
  free(state);
}
