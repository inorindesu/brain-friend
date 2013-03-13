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
#include <stdlib.h>
#include <string.h>
#include <opcodeCompiler.h>

void OC_life()
{
  fprintf(stderr, "[OC] life...");
  opcode_compiler_t* oc = opcode_compiler_new();
  opcode_compiler_destroy(oc);
  fprintf(stderr, "done!\n");
}

int dataSize = 5;
char* datasets[] = {
  "+",
  "-",
  "+-><,.",
  "+[]+",
  "-[-+><]-"
};

char* results[] = {
  "INIT 32767\nADD 1\n",
  "INIT 32767\nSUB 1\n",
  "INIT 32767\nADD 1\nSUB 1\nNEXT 1\nPREV 1\nIN\nOUT\n",
  "INIT 32767\nADD 1\nBZ 1\nADD 1\n",
  "INIT 32767\nSUB 1\nBZ 5\nSUB 1\nADD 1\nNEXT 1\nPREV 1\nSUB 1\n"
};

int catBufSize = 8192;

static void op_to_str(inst_t instruction, int param, void* buf)
{
  char* fmtBuf = calloc(sizeof(char), 64);
  switch(instruction)
    {
    case NOP:
      strncat(buf, "NOP\n", catBufSize);
      break;
    case ADD:
      snprintf(fmtBuf, 64, "ADD %d\n", param);
      strncat(buf, fmtBuf, catBufSize);
      break;
    case SUB:
      snprintf(fmtBuf, 64, "SUB %d\n", param);
      strncat(buf, fmtBuf, catBufSize);
      break;
    case NEXT:
      snprintf(fmtBuf, 64, "NEXT %d\n", param);
      strncat(buf, fmtBuf, catBufSize);
      break;
    case PREV:
      snprintf(fmtBuf, 64, "PREV %d\n", param);
      strncat(buf, fmtBuf, catBufSize);
      break;
    case BZ:
      snprintf(fmtBuf, 64, "BZ %d\n", param);
      strncat(buf, fmtBuf, catBufSize);
      break;
    case IN:
      strncat(buf, "IN\n", catBufSize);
      break;
    case OUT:
      strncat(buf, "OUT\n", catBufSize);
      break;
    case INIT:
      snprintf(fmtBuf, 64, "INIT %d\n", param);
      strncat(buf, fmtBuf, catBufSize);
      break;
    case DUMPSTAT:
      strncat(buf, "DUMPSTAT\n", catBufSize);
      break;      
    }
  free(fmtBuf);
}

static char* opcodes_to_string(opcode_list_t* ol)
{
  char* buf = calloc(sizeof(char), catBufSize);
  opcode_list_traverse(ol, op_to_str, buf);
  return buf;
}

void OC_compile(int datasetId)
{
  fprintf(stderr, "[OC] compile %d ...", datasetId);
  opcode_compiler_t* oc = opcode_compiler_new();
  opcode_compiler_feed_code(oc, datasets[datasetId], strlen(datasets[datasetId]));
  opcode_compiler_done_compilation(oc);
  opcode_list_t* compiledResult = opcode_compiler_result_new(oc);
  char* buf = opcodes_to_string(compiledResult);
  int cmpResult = strncmp(buf, results[datasetId], strlen(results[datasetId]));
  opcode_list_destroy(compiledResult);
  opcode_compiler_destroy(oc);
  if (cmpResult == 0)
    {
      fprintf(stderr, "done!\n");
    }
  else
    {
      fprintf(stderr, "failed! (result = %d,\n compiled: %s\n expected: %s)\n", cmpResult, buf, results[datasetId]);
    }
  free(buf);
}

void run_opcode_compiler_test()
{
  fprintf(stderr, "[OC] OC test started\n");
  OC_life();
  for(int i = 0; i < dataSize; i++)
    {
      OC_compile(i);
    }
  fprintf(stderr, "[OC] OC test ended\n");
}
