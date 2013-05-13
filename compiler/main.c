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
#include <opcodeCompiler.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdio.h>
#include <stdlib.h>

char FILE_KEY[] = "BF_LUA_OUTPUT_FP";

/*
 * bflua_write:
 * will be mapped to write(string) in lua script
 */
int bflua_write(lua_State* state)
{
  size_t size = 0;
  const char* str = lua_tolstring(state, 1, &size);
  lua_pop(state, 1);
  
  lua_pushstring(state, FILE_KEY);
  lua_gettable(state, LUA_REGISTRYINDEX);
  FILE* targetFp = lua_touserdata(state, -1);
  lua_pop(state, 1);
  
  fwrite(str, 1, size, targetFp);
  
  return 0;
}

/*
 * bflua_writeLine:
 * will be mapped to writeLine(string) in lua script
 */
int bflua_writeLine(lua_State* state)
{
  size_t size = 0;
  const char* str = lua_tolstring(state, 1, &size);
  lua_pop(state, 1);
  
  lua_pushstring(state, FILE_KEY);
  lua_gettable(state, LUA_REGISTRYINDEX);
  FILE* targetFp = lua_touserdata(state, -1);
  lua_pop(state, 1);
  
  fwrite(str, 1, size, targetFp);
  fputc('\n', targetFp);
  
  return 0;
}

lua_State* lua_vm_new()
{
  lua_State* stat = luaL_newstate();

  /* public functions*/
  lua_pushcfunction(stat, bflua_write);
  lua_setglobal(stat, "write");
  lua_pushcfunction(stat, bflua_writeLine);
  lua_setglobal(stat, "writeLine");

  /* Opcodes */
  lua_newtable(stat);
  int tableIdx = lua_gettop(stat);

  lua_pushstring(stat, "Nop");
  lua_pushinteger(stat, NOP);
  lua_settable(stat, tableIdx);
  
  lua_pushstring(stat, "Add");
  lua_pushinteger(stat, ADD);
  lua_settable(stat, tableIdx);
  
  lua_pushstring(stat, "Next");
  lua_pushinteger(stat, NEXT);
  lua_settable(stat, tableIdx);
  
  lua_pushstring(stat, "Bz");
  lua_pushinteger(stat, BZ);
  lua_settable(stat, tableIdx);
  
  lua_pushstring(stat, "B");
  lua_pushinteger(stat, B);
  lua_settable(stat, tableIdx);
  
  lua_pushstring(stat, "In");
  lua_pushinteger(stat, IN);
  lua_settable(stat, tableIdx);

  lua_pushstring(stat, "Out");
  lua_pushinteger(stat, OUT);
  lua_settable(stat, tableIdx);
  
  lua_pushstring(stat, "Init");
  lua_pushinteger(stat, INIT);
  lua_settable(stat, tableIdx);
  
  lua_pushstring(stat, "Dumpstat");
  lua_pushinteger(stat, DUMPSTAT);
  lua_settable(stat, tableIdx);
  
  lua_setglobal(stat, "Opcodes");

  return stat;
}

void print_help(char* appName)
{
  printf("Usage: %s -m compiler.lua\n\n", appName);
  printf("  Input is received from stdin\n");
  printf("  Output is written to stdout\n");
  printf("  Use -m switch to assign compiler lua script file\n");
}

int main(int argc, char** argv)
{
  if (argc < 3)
    {
      print_help(argv[0]);
    }
  return 0;
}
