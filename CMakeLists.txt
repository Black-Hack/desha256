cmake_minimum_required (VERSION 3.8)

project (desha256 CXX)

add_subdirectory ("third_party/boolexpr")

add_executable (${PROJECT_NAME} "src/main.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE boolexpr)
