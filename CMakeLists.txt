cmake_minimum_required(VERSION 3.14)
project(i8080sim C)

FIND_PACKAGE(Threads)
FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)


INCLUDE_DIRECTORIES(${GTK3_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})

include_directories(.)

add_definitions(${GTK3_CFLAGS_OTHER})

add_executable(i8080sim main.c core/i8080_state.c ui/ui_utils.c)

add_executable(i8080cli cli/main.c core/i8080_state.c core/hex.c core/i8080_asm.c)

TARGET_LINK_LIBRARIES(i8080sim ${GTK3_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} -rdynamic)

TARGET_LINK_LIBRARIES(i8080cli ${CMAKE_THREAD_LIBS_INIT} -rdynamic)
