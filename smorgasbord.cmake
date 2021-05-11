cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 14)
set(SUPPRESS_UNUSED_VARIABLE_WARNING_FOR ${QT_QMAKE_EXECUTABLE})

set(DEP_SRC_DIRECTORY
	"${CMAKE_CURRENT_SOURCE_DIR}/../dep_src"
	CACHE STRING "Path to the dep_src package")

add_subdirectory(
	"${DEP_SRC_DIRECTORY}/fmt-4.1.0"
	${CMAKE_CURRENT_BINARY_DIR}/fmt)
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
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl"
)

file(GLOB_RECURSE SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_library(${PROJECT_NAME} STATIC
	${PUBLIC_HEADERS}
	${PRIVATE_HEADERS}
	${SOURCES}
	"${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lodepng/lodepng.cpp"
)

#find_package(Vulkan REQUIRED)

if (MINGW)
	set(PLATFORM_LIBRARIES "-lmingw32")
endif()

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		${PLATFORM_LIBRARIES}
		fmt
		SDL2main SDL2-static
#		${Vulkan_LIBRARIES}
)

target_include_directories(${PROJECT_NAME}
	PUBLIC "src/"
	PUBLIC "thirdparty/"
	PUBLIC "${DEP_SRC_DIRECTORY}/fmt-4.1.0"
	PUBLIC "${DEP_SRC_DIRECTORY}/glm"
	PUBLIC "${DEP_SRC_DIRECTORY}/SDL2/include"
	PRIVATE ${SDL_INCLUDE_DIR}
#	PRIVATE ${Vulkan_INCLUDE_DIRS}
)

target_compile_definitions(${PROJECT_NAME}
	PUBLIC
		GLM_SWIZZLE
		NOMINMAX # windows.h interferes with GLM under MSVC if not defined
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	DEBUG_POSTFIX _d
)
