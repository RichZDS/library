# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "F:/Code/Library/build/_deps/oatpp_web-src")
  file(MAKE_DIRECTORY "F:/Code/Library/build/_deps/oatpp_web-src")
endif()
file(MAKE_DIRECTORY
  "F:/Code/Library/build/_deps/oatpp_web-build"
  "F:/Code/Library/build/_deps/oatpp_web-subbuild/oatpp_web-populate-prefix"
  "F:/Code/Library/build/_deps/oatpp_web-subbuild/oatpp_web-populate-prefix/tmp"
  "F:/Code/Library/build/_deps/oatpp_web-subbuild/oatpp_web-populate-prefix/src/oatpp_web-populate-stamp"
  "F:/Code/Library/build/_deps/oatpp_web-subbuild/oatpp_web-populate-prefix/src"
  "F:/Code/Library/build/_deps/oatpp_web-subbuild/oatpp_web-populate-prefix/src/oatpp_web-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "F:/Code/Library/build/_deps/oatpp_web-subbuild/oatpp_web-populate-prefix/src/oatpp_web-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "F:/Code/Library/build/_deps/oatpp_web-subbuild/oatpp_web-populate-prefix/src/oatpp_web-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
