cmake_minimum_required(VERSION 3.28)
project(Test)

set(CMAKE_CXX_STANDARD 17)

add_executable(Test main.cpp)

#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

add_subdirectory(fmt)
find_package(fmt)
target_link_libraries(Test fmt::fmt)

add_subdirectory(nlohmann_json)
find_package(nlohmann_json)
target_link_libraries(Test nlohmann_json::nlohmann_json)