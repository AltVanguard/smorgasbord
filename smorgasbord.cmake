cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)

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
find_package(glm REQUIRED)
find_package(fmt REQUIRED)
find_package(SDL2 REQUIRED)

if (MINGW)
	set(PLATFORM_LIBRARIES "-lmingw32")
endif()

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		${PLATFORM_LIBRARIES}
		fmt::fmt
		glm
		SDL2::SDL2main SDL2::SDL2-static
#		${Vulkan_LIBRARIES}
)

target_include_directories(${PROJECT_NAME}
	PUBLIC "src/"
	PUBLIC "thirdparty/"
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
