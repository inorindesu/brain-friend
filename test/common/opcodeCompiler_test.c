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

int dataSize = 8;
char* datasets[] = {
  "+",
  "-",
  "+-><,.",
  "+[]+",
  "-[-+><]-",
  "[-[+]]",
  "+++++",
  "[+++]-"
};

char* results[] = {
  "INIT 32767\nADD 1\n",
  "INIT 32767\nADD -1\n",
  "INIT 32767\nADD 1\nADD -1\nNEXT 1\nNEXT -1\nIN\nOUT\n",
  "INIT 32767\nADD 1\nBZ 2\nB -1\nADD 1\n",
  "INIT 32767\nADD -1\nBZ 6\nADD -1\nADD 1\nNEXT 1\nNEXT -1\nB -5\nADD -1\n",
  "INIT 32767\nBZ 6\nADD -1\nBZ 3\nADD 1\nB -2\nB -5\n",
  "INIT 32767\nADD 1\nADD 1\nADD 1\nADD 1\nADD 1\n",
  "INIT 32767\nBZ 5\nADD 1\nADD 1\nADD 1\nB -4\nADD -1\n"
};

char* results_opt[] = {
  "INIT 32767\nADD 1\n",
  "INIT 32767\nADD -1\n",
  "INIT 32767\nIN\nOUT\n",
  "INIT 32767\nADD 1\nBZ 2\nB -1\nADD 1\n",
  "INIT 32767\nADD -1\nBZ 2\nB -1\nADD -1\n",
  "INIT 32767\nBZ 6\nADD -1\nBZ 3\nADD 1\nB -2\nB -5\n",
  "INIT 32767\nADD 5\n",
  "INIT 32767\nBZ 3\nADD 3\nB -2\nADD -1\n"
};

char* results_dump[] = {
  "INIT 32767\nDUMPSTAT\nADD 1\nDUMPSTAT\n",
  "INIT 32767\nDUMPSTAT\nADD -1\nDUMPSTAT\n",
  "INIT 32767\nDUMPSTAT\nADD 1\nDUMPSTAT\nADD -1\nDUMPSTAT\nNEXT 1\nDUMPSTAT\nNEXT -1\nDUMPSTAT\nIN\nDUMPSTAT\nOUT\nDUMPSTAT\n",
  "INIT 32767\nDUMPSTAT\nADD 1\nDUMPSTAT\nBZ 3\nDUMPSTAT\nB -2\nDUMPSTAT\nADD 1\nDUMPSTAT\n",
  "INIT 32767\nDUMPSTAT\nADD -1\nDUMPSTAT\nBZ 11\nDUMPSTAT\nADD -1\nDUMPSTAT\nADD 1\nDUMPSTAT\nNEXT 1\nDUMPSTAT\nNEXT -1\nDUMPSTAT\nB -10\nDUMPSTAT\nADD -1\nDUMPSTAT\n",
  "INIT 32767\nDUMPSTAT\nBZ 11\nDUMPSTAT\nADD -1\nDUMPSTAT\nBZ 5\nDUMPSTAT\nADD 1\nDUMPSTAT\nB -4\nDUMPSTAT\nB -10\nDUMPSTAT\n",
  "INIT 32767\nDUMPSTAT\nADD 1\nDUMPSTAT\nADD 1\nDUMPSTAT\nADD 1\nDUMPSTAT\nADD 1\nDUMPSTAT\nADD 1\nDUMPSTAT\n",
  "INIT 32767\nDUMPSTAT\nBZ 9\nDUMPSTAT\nADD 1\nDUMPSTAT\nADD 1\nDUMPSTAT\nADD 1\nDUMPSTAT\nB -8\nDUMPSTAT\nADD -1\nDUMPSTAT\n"
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
    case NEXT:
      snprintf(fmtBuf, 64, "NEXT %d\n", param);
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
    case B:
      snprintf(fmtBuf, 64, "B %d\n", param);
      strncat(buf, fmtBuf, catBufSize);
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

void OC_optimize(int datasetId)
{
  fprintf(stderr, "[OC] optimize %d ...", datasetId);
  opcode_compiler_t* oc = opcode_compiler_new();
  opcode_compiler_set_optimization(oc, true);
  opcode_compiler_feed_code(oc, datasets[datasetId], strlen(datasets[datasetId]));
  opcode_compiler_done_compilation(oc);
  opcode_list_t* compiledResult = opcode_compiler_result_new(oc);
  char* buf = opcodes_to_string(compiledResult);
  int cmpResult = strncmp(buf, results_opt[datasetId], strlen(results_opt[datasetId]));
  opcode_list_destroy(compiledResult);
  opcode_compiler_destroy(oc);
  if (cmpResult == 0)
    {
      fprintf(stderr, "done!\n");
    }
  else
    {
      fprintf(stderr, "failed! (result = %d,\n compiled: %s\n expected: %s)\n", cmpResult, buf, results_opt[datasetId]);
    }
  free(buf);
}

void OC_switch()
{
  opcode_compiler_t* oc = opcode_compiler_new();
  fprintf(stderr, "[OC] Switch..");
  if(opcode_compiler_get_optimization(oc) != false ||
     opcode_compiler_get_insert_state_dumper(oc) != false)
    {
      fprintf(stderr, "failed! (default not correct)\n");
      opcode_compiler_destroy(oc);
      return;
    }
  
  opcode_compiler_set_insert_state_dumper(oc, true);
  if(opcode_compiler_get_insert_state_dumper(oc) != true)
    {
      fprintf(stderr, "failed! (cannot set insert_state_dumper)\n");
      opcode_compiler_destroy(oc);
      return;
    }
  
  opcode_compiler_set_optimization(oc, true);
  if(opcode_compiler_get_optimization(oc) != true)
    {
      fprintf(stderr, "failed! (cannot set optimization)\n");
      opcode_compiler_destroy(oc);
      return;
    }
  else if(opcode_compiler_get_insert_state_dumper(oc) != false)
    {
      fprintf(stderr, "failed! (insert_state_dumper was not overridden by optimization = true)\n");
      opcode_compiler_destroy(oc);
      return;
    }  

  opcode_compiler_set_insert_state_dumper(oc, true);
  if(opcode_compiler_get_optimization(oc) != false)
    {
      fprintf(stderr, "failed! (optimization was not overridden by insert_state_dumper = true)\n");
      opcode_compiler_destroy(oc);
      return;
    }
  
  opcode_compiler_destroy(oc);
  fprintf(stderr, "done!\n");
}

void OC_dump(int datasetId)
{
  fprintf(stderr, "[OC] dumpstat %d ...", datasetId);
  opcode_compiler_t* oc = opcode_compiler_new();
  opcode_compiler_set_insert_state_dumper(oc, true);
  opcode_compiler_feed_code(oc, datasets[datasetId], strlen(datasets[datasetId]));
  opcode_compiler_done_compilation(oc);
  opcode_list_t* compiledResult = opcode_compiler_result_new(oc);
  char* buf = opcodes_to_string(compiledResult);
  int cmpResult = strncmp(buf, results_dump[datasetId], strlen(results_dump[datasetId]));
  opcode_list_destroy(compiledResult);
  opcode_compiler_destroy(oc);
  if (cmpResult == 0)
    {
      fprintf(stderr, "done!\n");
    }
  else
    {
      fprintf(stderr, "failed! (result = %d,\n compiled: %s\n expected: %s)\n", cmpResult, buf, results_dump[datasetId]);
    }
  free(buf);
}

void run_opcode_compiler_test()
{
  fprintf(stderr, "[OC] OC test started\n");
  OC_life();
  OC_switch();
  for(int i = 0; i < dataSize; i++)
    {
      OC_compile(i);
      OC_optimize(i);
      OC_dump(i);
    }
  fprintf(stderr, "[OC] OC test ended\n");
}
