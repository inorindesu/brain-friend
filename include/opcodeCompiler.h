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

#ifndef OPCODECOMPILER_H
#define OPCODECOMPILER_H

#include <opcodes.h>

typedef struct opcode_compiler_t opcode_compiler_t;

opcode_compiler_t* opcode_compiler_new();
void opcode_compiler_set_insert_state_dumper(opcode_compiler_t* c, bool insertStateDumper);
void opcode_compiler_reset(opcode_compiler_t* c);
void opcode_compiler_destroy(opcode_compiler_t* c);
void opcode_compiler_feed_code(opcode_compiler_t* c, const char* code, unsigned int size);
void opcode_compiler_done_compilation(opcode_compiler_t* c);
opcode_list_t* opcode_compiler_result_new(opcode_compiler_t* c);
int opcode_compiler_get_current_column_number(opcode_compiler_t* c);
int opcode_compiler_get_current_line_number(opcode_compiler_t* c);
char* opcode_compiler_get_error_new(opcode_compiler_t* c);
void opcode_compiler_set_optimization(opcode_compiler_t* c, bool optimize);

#endif
