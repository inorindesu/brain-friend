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

#include <opcodes.h>
#include <stdlib.h>

typedef struct opcode_t
{
  inst_t instruction;
  int param;
  struct opcode_t* next;
}opcode_t;

typedef struct opcode_list_t
{
  opcode_t* head;
  opcode_t* tail;
}opcode_list_t;

opcode_list_t* opcode_list_new()
{
  opcode_list_t* obj = malloc(sizeof(opcode_list_t));
  obj->head = NULL;
  obj->tail = NULL;
  return obj;
}

void opcode_list_destroy(opcode_list_t* l)
{
  opcode_list_clear(l);
  free(l);
}

void opcode_list_add(opcode_list_t* l, inst_t instruction, int parameter)
{
  opcode_t** o = &l->head;
  opcode_t* oNew = malloc(sizeof(opcode_t));
  oNew->instruction = instruction;
  oNew->param = parameter;
  oNew->next = NULL;
  if(*o)
    {
      l->tail->next = oNew;
    }
  else
    {
      l->head = oNew;
    }
  l->tail = oNew;
}

bool opcode_list_is_empty(opcode_list_t* l)
{
  return l->head == NULL;
}

void opcode_list_traverse(opcode_list_t* l, traveler func, void* userData)
{
  opcode_t** o = &l->head;
  opcode_t* oHead = l->head;
  while(*o)
    {
      opcode_t* oi = *o;
      func(oi->instruction, oi->param, userData);
      *o = oi->next;
    }
  *o = oHead;
}

void opcode_list_clear(opcode_list_t* l)
{
  opcode_t** o = &l->head;
  while(*o)
    {
      opcode_t* on = (*o)->next;
      free(*o);
      *o = on;
    }
  l->tail = NULL;
}
