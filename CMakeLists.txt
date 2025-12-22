cmake_minimum_required(VERSION 3.16)
project(LibraryCLI CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 命令行版本可执行文件
add_executable(library_cli
  Main.cpp
  library_core/btree_book.cpp
  library_core/btree_author.cpp
  library_core/library.cpp
)

target_include_directories(library_cli PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/library_core
)

if (MSVC)
  target_compile_options(library_cli PRIVATE /utf-8)
endif()
