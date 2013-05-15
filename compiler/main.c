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
#include <string.h>

const int BUF_SIZE = 1024;

/*
 * bflua_write:
 * will be mapped to write(string) in lua script
 */
int bflua_write(lua_State* state)
{
  size_t size = 0;
  const char* str = lua_tolstring(state, 1, &size);
  
  fwrite(str, 1, size, stdout);
  lua_pop(state, 1);
  
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
  
  fwrite(str, 1, size, stdout);
  fputc('\n', stdout);

  lua_pop(state, 1);
  
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
  
  lua_pushcfunction(stat, luaopen_table);
  lua_pushliteral(stat, LUA_TABLIBNAME);
  lua_call(stat, 1, 0);

  return stat;
}

void print_help(char* appName)
{
  printf("Usage: %s [-d|-o] compiler.lua\n\n", appName);
  printf("  Input is received from stdin\n");
  printf("  Output is written to stdout\n");
  printf("  Compiler lua script file should be assigned.\n");
  printf("  -d :     Emit dumpstat opcode.\n");
  printf("  -o :     Enabling optimization.\n");
  printf("  (Only one of -d and -o can be activated)\n");
}

void opcode_sender(inst_t instruction, int param, void* userData)
{
  lua_State* s = (lua_State*)userData;
  lua_getglobal(s, "receiveCode");
  lua_pushinteger(s, instruction);
  lua_pushinteger(s, param);
  if (lua_pcall(s, 2, 0, 0) != 0)
    {
      fprintf(stderr, "ERROR: while transferring opcodes.\n");
      fprintf(stderr, "%s\n", lua_tostring(s, lua_gettop(s)));
      lua_pop(s, 1);
    }
}

int main(int argc, char** argv)
{
  /* basic check of parameters*/
  if (argc < 2)
    {
      print_help(argv[0]);
      return 0;
    }
  
  /*
   * scan parameters
   * compare for -d and -o, 
   * first of the items left will be used as main script
   */
  bool debugFlag = false;
  bool optimizeFlag = false;
  char* scriptName = NULL;
  for(int i = 1; i < argc; i++)
    {
      if (argv[i][0] == '\0')
        {
          continue;
        }
      else if (strcmp(argv[i], "-d") == 0)
        {
          debugFlag = true;
        }
      else if (strcmp(argv[i], "-o") == 0)
        {
          optimizeFlag = true;
        }
      else if(argv[i][0] != '\0' && argv[i][0] == '-')
        {
          fprintf(stderr, "INFO: omit unknown switch %s\n", argv[i]);
        }
      else if (scriptName == NULL)
        {
          FILE* fp = fopen(argv[i], "r");
          if (fp == NULL)
            {
              fprintf(stderr, "INFO: omit nonexistent file %s\n", argv[i]);
            }
          else
            {
              fclose(fp);
              scriptName = argv[i];
            }
        }
      else
        {
          fprintf(stderr, "INFO: omit file %s\n", argv[i]);
        }
    }
  
  if (debugFlag && optimizeFlag)
    {
      fprintf(stderr, "ERROR: -d and -o cannot be specified at the same time.\n");
      return -1;
    }
  if (scriptName == NULL)
    {
      fprintf(stderr, "ERROR: no compilation script specified.\n");
      return -1;
    }

  /*
   * Load script
   */
  lua_State* s = lua_vm_new();
  if(luaL_dofile(s, scriptName))
    {
      const char* msg = lua_tostring(s, lua_gettop(s));
      fprintf(stderr, "ERROR: message is:\n%s\n", msg);
      lua_close(s);
      return -1;
    }
  
  /*
   * check for 3 functions
   * receiveCode should exist in lua script
   * init and atEnd are optional
   */
  lua_getglobal(s, "receiveCode");
  if (lua_isfunction(s, lua_gettop(s)) == 0)
    {
      fprintf(stderr, "ERROR: receiveCode is not provided in script %s\n", scriptName);
      lua_close(s);
      return -1;
    }
  lua_pop(s, 1);

  bool hasInit = false;
  bool hasAtEnd = false;
  lua_getglobal(s, "init");
  if (lua_isfunction(s, lua_gettop(s)))
    {
      hasInit = true;
    }
  lua_pop(s, 1);
  
  lua_getglobal(s, "atEnd");
  if (lua_isfunction(s, lua_gettop(s)))
    {
      hasAtEnd = true;
    }
  lua_pop(s, 1);
  
  /*
   * Compile codes from stdin
   */
  opcode_compiler_t* compiler = opcode_compiler_new();
  opcode_compiler_set_insert_state_dumper(compiler, debugFlag);
  opcode_compiler_set_optimization(compiler, optimizeFlag);
  size_t read = 0;
  char* buffer = malloc(BUF_SIZE);
  do
    {
      read = fread(buffer, 1, BUF_SIZE, stdin);
      opcode_compiler_feed_code(compiler, buffer, read);
      char* err = opcode_compiler_get_error_new(compiler);
      if (err != NULL)
        {
          int lineNo = opcode_compiler_get_current_line_number(compiler);
          int colNo = opcode_compiler_get_current_column_number(compiler);
          fprintf(stderr, "ERROR: at line %d, column %d:\n", lineNo, colNo);
          fprintf(stderr, "%s\n", err);
          return -1;
        }
    }
  while (read > 0);
  free(buffer);
  opcode_compiler_done_compilation(compiler);

  /*
   * Pass compiled opcodes to lua engine
   */
  if (hasInit == true)
    {
      lua_getglobal(s, "init");
      if (lua_pcall(s, 0, 0, 0) != 0)
        {
          fprintf(stderr, "ERROR: when running init.");
          fprintf(stderr, "%s\n", lua_tostring(s, lua_gettop(s)));
          lua_pop(s, 1);
          return -1;
        }
    }

  opcode_list_t* opcodes = opcode_compiler_result_new(compiler);
  opcode_compiler_destroy(compiler);
  opcode_list_traverse(opcodes, opcode_sender, s);
  opcode_list_destroy(opcodes);
  
  if (hasAtEnd == true)
    {
      lua_getglobal(s, "atEnd");
      if (lua_pcall(s, 0, 0, 0) != 0)
        {
          fprintf(stderr, "ERROR: when running init.");
          fprintf(stderr, "%s\n", lua_tostring(s, lua_gettop(s)));
          lua_pop(s, 1);
          return -1;
        }
    }

  /*
   * cleanup
   */
  lua_close(s);
  return 0;
}
