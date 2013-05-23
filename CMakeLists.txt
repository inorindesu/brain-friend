cmake_minimum_required(VERSION 2.6)
project(brainf)

# if your compiler uses different c99 flag,
# change it here :)
#
# My code NEEDS c99.
set(C99_FLAG
  --std=c99)

# uncomment BUILD_DOC to disable documentation
# build
set(BUILD_DOC yes)
set(DOC_ROOT_DIRS
  ./doc/zh_tw/)

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

set(LUA_SCRIPTS
  dump.lua
  pseudoOpcode.lua
  x86_64_GNU.lua
  )

if (DEFINED LUA_OVERRIDE)
  set(INCLUDE_DIR ${INCLUDE_DIR} ${LUA_OVERRIDE}/include)
  find_library(LUA_LIB lua PATHS ${LUA_OVERRIDE}/lib64 ${LUA_OVERRIDE}/lib NO_DEFAULT_PATH)
  message(STATUS ${LUA_LIB})
endif (DEFINED LUA_OVERRIDE)
find_library(LUA_LIB lua)
message(STATUS ${LUA_LIB})

include_directories(${INCLUDE_DIR})
list(APPEND CMAKE_C_FLAGS ${C99_FLAG})

# scripts
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/share/scripts)
set(LUA_COPY)
foreach(LUA_SCRIPT ${LUA_SCRIPTS})
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/share/scripts/${LUA_SCRIPT}
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/scripts/${LUA_SCRIPT}" "${CMAKE_BINARY_DIR}/share/scripts/${LUA_SCRIPT}"
    DEPENDS ${CMAKE_SOURCE_DIR}/scripts/${LUA_SCRIPT}
    )
  list(APPEND LUA_COPY ${CMAKE_BINARY_DIR}/share/scripts/${LUA_SCRIPT})
endforeach(LUA_SCRIPT)
add_custom_target(lua_copier ALL DEPENDS ${LUA_COPY})

# documentation
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/share/doc)
if (DEFINED BUILD_DOC)
  set(DOC_BUILD)
  foreach(DOC_ROOT ${DOC_ROOT_DIRS})
    get_filename_component(FF ${DOC_ROOT} ABSOLUTE) # remove trailing '/'
    get_filename_component(FNAME ${FF} NAME)
    #message(STATUS ${FF})
    get_filename_component(BOOK_SRC ${DOC_ROOT}/book.xml ABSOLUTE)
    get_filename_component(DB_STYLE ${DOC_ROOT}/style.xsl ABSOLUTE)
    file(GLOB_RECURSE DEPENDS_XML ${DOC_ROOT}/*.xml)
    add_custom_command(
      OUTPUT ${CMAKE_BINARY_DIR}/share/doc/${FNAME}/index.html
      COMMAND xsltproc --xinclude --stringparam base.dir "${CMAKE_BINARY_DIR}/share/doc/${FNAME}" ${DB_STYLE} ${BOOK_SRC}
      DEPENDS ${DEPENDS_XML}
      )
    list(APPEND DOC_BUILD ${CMAKE_BINARY_DIR}/share/doc/${FNAME}/index.html)
  endforeach(DOC_ROOT)
  add_custom_target(doc_builder ALL DEPENDS ${DOC_BUILD})
endif (DEFINED BUILD_DOC)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(bfi ${COMMON_SRC} ${INTER_SRC})
add_executable(bfc ${COMMON_SRC} ${COMPILER_SRC})
target_link_libraries(bfc ${LUA_LIB} m)
add_executable(test_common ${COMMON_SRC} ${COMMON_TEST})