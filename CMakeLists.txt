cmake_minimum_required(VERSION 2.6)
project(brainf)

# if your compiler uses different c99 flag,
# change it here :)
#
# My code NEEDS c99.
set(C99_FLAG
  --std=c99)

# ! please remember to change /unset this variable
# ! if default LUA is good for your need.
#
# After changing this, please remove all cmake caches
# and rerun cmake!
set(LUA_OVERRIDE
  $ENV{HOME}/applications/lua-5.2.1)

# source lists
# Modify the following is not recommended

set(COMMON_SRC
  ./common/stack.c
  ./common/opcodes.c
  ./common/opcodeCompiler.c
  )

set(INTER_SRC
  ./interpreter/main.c
  )

set(COMPILER_SRC
  ./compiler/main.c
  )

set(COMMON_TEST
  ./test/common/main.c
  ./test/common/stack_test.c
  ./test/common/opcodes_test.c
  ./test/common/opcodeCompiler_test.c
  )

set(INCLUDE_DIR
  include)

if (DEFINED LUA_OVERRIDE)
  set(INCLUDE_DIR ${INCLUDE_DIR} ${LUA_OVERRIDE}/include)
  find_library(LUA_LIB lua PATHS ${LUA_OVERRIDE}/lib64 ${LUA_OVERRIDE}/lib NO_DEFAULT_PATH)
  message(STATUS ${LUA_LIB})
endif (DEFINED LUA_OVERRIDE)
find_library(LUA_LIB lua)
message(STATUS ${LUA_LIB})

include_directories(${INCLUDE_DIR})
list(APPEND CMAKE_C_FLAGS ${C99_FLAG})

add_executable(bfi ${COMMON_SRC} ${INTER_SRC})
#add_executable(bfc ${COMMON_SRC} ${COMPILER_SRC})
#target_link_library(bfc lua)
add_executable(test_common ${COMMON_SRC} ${COMMON_TEST})