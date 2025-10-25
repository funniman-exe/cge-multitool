cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(cge-multitool VERSION 1.0)

add_executable(cge-multitool commands.cpp fastdl.cpp config.cpp main.cpp)

if(WIN32)
  set(plat_suffix "-win")
elseif(APPLE)
  set(plat_suffix "-mac")
elseif(UNIX)
  set(plat_suffix "-unix")
else()
  set(plat_suffix "")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(plat_bits "64")
# uncomment to specify if build is 32 bit
#elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
#  set(plat_bits "32")
else()
  set(plat_bits "")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(build_suffix "-dbg")
# uncomment to specify if build is release build
#elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
#  set(build_suffix "-rel")
else()
  set(build_suffix "")
endif()

set_target_properties(cge-multitool PROPERTIES
    OUTPUT_NAME "cge-multitool${plat_suffix}${plat_bits}${build_suffix}"
)

target_link_libraries(cge-multitool ${CMAKE_SOURCE_DIR}/thirdparty/libssq/build/libssq.a ws2_32)

target_include_directories(cge-multitool PRIVATE thirdparty)
target_include_directories(cge-multitool PRIVATE thirdparty/libssq/include)
#add_subdirectory(thirdparty)

install(TARGETS cge-multitool FILE_SET HEADERS)
