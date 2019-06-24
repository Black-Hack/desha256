cmake_minimum_required (VERSION 3.10)

project (desha256 CXX)

add_subdirectory ("third_party/boolexpr")

add_executable (${PROJECT_NAME} "src/main.cpp")

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

target_link_libraries(${PROJECT_NAME} PRIVATE boolexpr)
