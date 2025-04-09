# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/Pro_1_2_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/Pro_1_2_autogen.dir/ParseCache.txt"
  "Pro_1_2_autogen"
  "_deps/cpr-build/cpr/CMakeFiles/cpr_autogen.dir/AutogenUsed.txt"
  "_deps/cpr-build/cpr/CMakeFiles/cpr_autogen.dir/ParseCache.txt"
  "_deps/cpr-build/cpr/cpr_autogen"
  "_deps/curl-build/lib/CMakeFiles/curlu_autogen.dir/AutogenUsed.txt"
  "_deps/curl-build/lib/CMakeFiles/curlu_autogen.dir/ParseCache.txt"
  "_deps/curl-build/lib/CMakeFiles/libcurl_shared_autogen.dir/AutogenUsed.txt"
  "_deps/curl-build/lib/CMakeFiles/libcurl_shared_autogen.dir/ParseCache.txt"
  "_deps/curl-build/lib/curlu_autogen"
  "_deps/curl-build/lib/libcurl_shared_autogen"
  "_deps/zlib-build/CMakeFiles/zlib_autogen.dir/AutogenUsed.txt"
  "_deps/zlib-build/CMakeFiles/zlib_autogen.dir/ParseCache.txt"
  "_deps/zlib-build/zlib_autogen"
  )
endif()
