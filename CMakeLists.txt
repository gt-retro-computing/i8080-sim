cmake_minimum_required(VERSION 3.14)
project(i8080sim)

set(CMAKE_CXX_STANDARD 11)

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)


INCLUDE_DIRECTORIES(${GTK3_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})

include_directories(.)

add_definitions(${GTK3_CFLAGS_OTHER})

add_executable(i8080sim main.cpp core/i8080_state.c ui/ui_utils.c)

TARGET_LINK_LIBRARIES(i8080sim ${GTK3_LIBRARIES} -export-dynamic)