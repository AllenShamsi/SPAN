# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\SPAN_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\SPAN_autogen.dir\\ParseCache.txt"
  "SPAN_autogen"
  )
endif()
