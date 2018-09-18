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

set(WORKING_DIRECTORY_FILES
	${SHADERS}
)

set(CONFIGURE_FILES
)

set_source_files_properties(
	${WORKING_DIRECTORY_FILES}
	${CONFIGURE_FILES}
	PROPERTIES
	HEADER_FILE_ONLY TRUE
)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

add_executable(${PROJECT_NAME} WIN32
	${HEADERS}
	${SOURCES}
	${WORKING_DIRECTORY_FILES}
	${CONFIGURE_FILES}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE smorgasbord
)

add_definitions(
	-DGLEW_STATIC
	-DGLM_SWIZZLE
	-DNOMINMAX
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	DEBUG_POSTFIX _d
)

foreach (_CONFIGURE_FILE ${CONFIGURE_FILES})
	# use COPYONLY to disable or @ONLY to limit variable substitution in the
	# configured file
	configure_file(${_CONFIGURE_FILE} . COPYONLY)
endforeach()

source_group(headers FILES
	${PUBLIC_HEADERS}
	${PRIVATE_HEADERS}
)

source_group(sources FILES
	${SOURCES}
)

source_group(data FILES
	${WORKING_DIRECTORY_FILES}
	${CONFIGURE_FILES}
)
