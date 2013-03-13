cmake_minimum_required(VERSION 2.6)
project(brainf)

set(C99_FLAG
  --std=c99)

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

include_directories(include)
list(APPEND CMAKE_C_FLAGS ${C99_FLAG})

#add_executable(bfi ${COMMON_SRC} ${INTER_SRC})
#add_executable(bfc ${COMMON_SRC} ${COMPILER_SRC})
add_executable(test_common ${COMMON_SRC} ${COMMON_TEST})