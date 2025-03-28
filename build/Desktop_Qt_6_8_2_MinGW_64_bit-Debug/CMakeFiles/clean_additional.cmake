# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Pro_1_2_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Pro_1_2_autogen.dir\\ParseCache.txt"
  "Pro_1_2_autogen"
  )
endif()
