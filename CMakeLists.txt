#Minumum cmake requirements
cmake_minimum_required(VERSION 3.15)

#Project statement
project(
	cli_pos
	LANGUAGES CXX
	)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(cli_pos src/main.cpp
	src/database_functions.cpp
	src/binary_search.cpp
	src/menus.cpp
	src/input_string.cpp
	src/definitions_and_variables.cpp
)

add_subdirectory(libpqxx build-pqxx)

target_link_libraries(cli_pos -lncurses pqxx -lpq)

install(TARGETS cli_pos RUNTIME DESTINATION bin)
