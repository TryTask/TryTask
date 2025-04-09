# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/hturu/Downloads/TryTask-chor-client-side-aplication-2/build/Desktop_Qt_6_8_1_MinGW_64_bit-Debug/_deps/curl-src"
  "C:/Users/hturu/Downloads/TryTask-chor-client-side-aplication-2/build/Desktop_Qt_6_8_1_MinGW_64_bit-Debug/_deps/curl-build"
  "C:/Users/hturu/Downloads/TryTask-chor-client-side-aplication-2/build/Desktop_Qt_6_8_1_MinGW_64_bit-Debug/_deps/curl-subbuild/curl-populate-prefix"
  "C:/Users/hturu/Downloads/TryTask-chor-client-side-aplication-2/build/Desktop_Qt_6_8_1_MinGW_64_bit-Debug/_deps/curl-subbuild/curl-populate-prefix/tmp"
  "C:/Users/hturu/Downloads/TryTask-chor-client-side-aplication-2/build/Desktop_Qt_6_8_1_MinGW_64_bit-Debug/_deps/curl-subbuild/curl-populate-prefix/src/curl-populate-stamp"
  "C:/Users/hturu/Downloads/TryTask-chor-client-side-aplication-2/build/Desktop_Qt_6_8_1_MinGW_64_bit-Debug/_deps/curl-subbuild/curl-populate-prefix/src"
  "C:/Users/hturu/Downloads/TryTask-chor-client-side-aplication-2/build/Desktop_Qt_6_8_1_MinGW_64_bit-Debug/_deps/curl-subbuild/curl-populate-prefix/src/curl-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/hturu/Downloads/TryTask-chor-client-side-aplication-2/build/Desktop_Qt_6_8_1_MinGW_64_bit-Debug/_deps/curl-subbuild/curl-populate-prefix/src/curl-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/hturu/Downloads/TryTask-chor-client-side-aplication-2/build/Desktop_Qt_6_8_1_MinGW_64_bit-Debug/_deps/curl-subbuild/curl-populate-prefix/src/curl-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
