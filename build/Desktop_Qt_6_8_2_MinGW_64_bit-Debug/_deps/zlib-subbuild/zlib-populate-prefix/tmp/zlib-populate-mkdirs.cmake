# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Users/Ice/Documents/Pro_1_2/build/Desktop_Qt_6_8_2_MinGW_64_bit-Debug/_deps/zlib-src")
  file(MAKE_DIRECTORY "C:/Users/Ice/Documents/Pro_1_2/build/Desktop_Qt_6_8_2_MinGW_64_bit-Debug/_deps/zlib-src")
endif()
file(MAKE_DIRECTORY
  "C:/Users/Ice/Documents/Pro_1_2/build/Desktop_Qt_6_8_2_MinGW_64_bit-Debug/_deps/zlib-build"
  "C:/Users/Ice/Documents/Pro_1_2/build/Desktop_Qt_6_8_2_MinGW_64_bit-Debug/_deps/zlib-subbuild/zlib-populate-prefix"
  "C:/Users/Ice/Documents/Pro_1_2/build/Desktop_Qt_6_8_2_MinGW_64_bit-Debug/_deps/zlib-subbuild/zlib-populate-prefix/tmp"
  "C:/Users/Ice/Documents/Pro_1_2/build/Desktop_Qt_6_8_2_MinGW_64_bit-Debug/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp"
  "C:/Users/Ice/Documents/Pro_1_2/build/Desktop_Qt_6_8_2_MinGW_64_bit-Debug/_deps/zlib-subbuild/zlib-populate-prefix/src"
  "C:/Users/Ice/Documents/Pro_1_2/build/Desktop_Qt_6_8_2_MinGW_64_bit-Debug/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Ice/Documents/Pro_1_2/build/Desktop_Qt_6_8_2_MinGW_64_bit-Debug/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/Ice/Documents/Pro_1_2/build/Desktop_Qt_6_8_2_MinGW_64_bit-Debug/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
