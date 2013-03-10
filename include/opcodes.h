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

#ifndef OPCODES_H
#define OPCODES_H

#include <stdbool.h>

typedef enum inst_t
  {
    NOP,
    ADD,
    SUB,
    NEXT,
    PREV,
    CHECK,
    IN,
    OUT,
    INIT,
    DUMPSTAT
  } inst_t;

typedef struct opcode_list_t opcode_list_t;
typedef void (*traveler) (inst_t instruction, int parameter, void* userData);

opcode_list_t* opcode_list_new();
void opcode_list_destroy(opcode_list_t* l);
void opcode_list_add(opcode_list_t* l, inst_t instruction, int parameter);
bool opcode_list_is_empty(opcode_list_t* l);
void opcode_list_traverse(opcode_list_t* l, traveler func, void* userData);

#endif
