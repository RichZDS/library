cmake_minimum_required(VERSION 3.16)
project(LibraryWeb CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  oatpp
  URL https://github.com/oatpp/oatpp/archive/refs/tags/1.3.0.zip
)
FetchContent_MakeAvailable(oatpp)


add_executable(library_web
  web/main_server.cpp
  web/AppComponent.hpp
  web/BookDto.hpp
  web/BookService.hpp
  web/BookService.cpp
  web/BookController.hpp
  web/BookController.cpp
  library_core/btree_book.cpp
  library_core/btree_author.cpp
)

target_include_directories(library_web PRIVATE
  ${oatpp_SOURCE_DIR}/src
  ${oatpp_BINARY_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/controller
  ${CMAKE_CURRENT_SOURCE_DIR}/library_core
)

target_link_libraries(library_web PRIVATE oatpp)

if (MSVC)
  target_compile_options(library_web PRIVATE /utf-8)
endif()

add_custom_command(TARGET library_web POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/webroot
  $<TARGET_FILE_DIR:library_web>/webroot)
