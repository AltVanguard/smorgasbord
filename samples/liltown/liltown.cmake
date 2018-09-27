cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 14)
set(SUPPRESS_UNUSED_VARIABLE_WARNING_FOR ${QT_QMAKE_EXECUTABLE})

add_subdirectory(../../ smorgasbord)

project(liltown)

file(GLOB_RECURSE HEADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

file(GLOB_RECURSE SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

file(GLOB_RECURSE SHADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/data/*.shader"
	"${CMAKE_CURRENT_SOURCE_DIR}/data/*.compute"
)

set_source_files_properties(${SHADERS} PROPERTIES
	HEADER_FILE_ONLY TRUE
)

if (WIN32)
	set(CMAKE_WIN32_EXECUTABLE ON)
endif()

add_executable(${PROJECT_NAME}
	${HEADERS}
	${SOURCES}
	${SHADERS}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE smorgasbord
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	DEBUG_POSTFIX _d
)
