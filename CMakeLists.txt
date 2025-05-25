cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(cge-multitool VERSION 0.0.2)

add_executable(cge-multitool commands.cpp fastdl.cpp main.cpp)

target_link_libraries(cge-multitool ${CMAKE_SOURCE_DIR}/thirdparty/libssq/build/libssq.a ws2_32)

target_include_directories(cge-multitool PRIVATE thirdparty)
target_include_directories(cge-multitool PRIVATE thirdparty/libssq/include)
#add_subdirectory(thirdparty)

install(TARGETS cge-multitool FILE_SET HEADERS)
