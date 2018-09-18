cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 14)
set(SUPPRESS_UNUSED_VARIABLE_WARNING_FOR ${QT_QMAKE_EXECUTABLE})

set(DEP_SRC_DIRECTORY
	"${CMAKE_CURRENT_SOURCE_DIR}/../dep_src"
	CACHE STRING "Path to the dep_src package")

add_subdirectory(
	"${DEP_SRC_DIRECTORY}/fmt-4.1.0"
	${CMAKE_CURRENT_BINARY_DIR}/fmt)
add_subdirectory(
	"${DEP_SRC_DIRECTORY}/glew-2.0.0"
	${CMAKE_CURRENT_BINARY_DIR}/glew)
add_subdirectory(
	"${DEP_SRC_DIRECTORY}/lodepng"
	${CMAKE_CURRENT_BINARY_DIR}/lodepng)
option(SDL_SHARED "" OFF)
option(SDL_STATIC "" ON)
option(DIRECTX "" OFF)
option(RENDER_D3D "" OFF)
option(VIDEO_OPENGLES "SDL2 VIDEO_OPENGLES" OFF)
add_subdirectory(
	"${DEP_SRC_DIRECTORY}/SDL2"
	${CMAKE_CURRENT_BINARY_DIR}/SDL2)

project(smorgasbord)

set(PRIVATE_HEADERS
)

file(GLOB_RECURSE PUBLIC_HEADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

file(GLOB_RECURSE SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

set(WORKING_DIRECTORY_FILES
)

set(CONFIGURE_FILES
)

set_source_files_properties(
	${WORKING_DIRECTORY_FILES}
	${CONFIGURE_FILES}
	PROPERTIES
	HEADER_FILE_ONLY TRUE
)

add_library(${PROJECT_NAME} STATIC
	${PUBLIC_HEADERS}
	${PRIVATE_HEADERS}
	${SOURCES}
	${WORKING_DIRECTORY_FILES}
	${CONFIGURE_FILES}
)

find_package(OpenGL REQUIRED)
#find_package(Vulkan REQUIRED)

if (MINGW)
	set(PLATFORM_LIBRARIES "-lmingw32")
endif()

target_link_libraries(${PROJECT_NAME}
	PUBLIC ${PLATFORM_LIBRARIES}
	PUBLIC lodepng
	PUBLIC fmt
	PUBLIC SDL2main SDL2-static
	PUBLIC GLEW
	PUBLIC ${OPENGL_LIBRARIES}
#	PUBLIC ${Vulkan_LIBRARIES}
)

target_include_directories(${PROJECT_NAME}
	PUBLIC "src/"
	PUBLIC "${DEP_SRC_DIRECTORY}/fmt-4.1.0"
	PUBLIC "${DEP_SRC_DIRECTORY}/glew-2.0.0/include"
	PUBLIC "${DEP_SRC_DIRECTORY}/glm"
	PUBLIC "${DEP_SRC_DIRECTORY}/lodepng"
	PUBLIC "${DEP_SRC_DIRECTORY}/SDL2/include"
	PRIVATE ${SDL_INCLUDE_DIR}
#	PRIVATE ${Vulkan_INCLUDE_DIRS}
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
	${CONFIGURE_FILES}
)
