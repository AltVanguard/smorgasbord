set(CMAKE_CXX_STANDARD 17)

find_package(glm REQUIRED)
find_package(fmt REQUIRED)
find_package(SDL2 REQUIRED)

file(GLOB_RECURSE PUBLIC_HEADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl"
)

file(GLOB_RECURSE SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(${PROJECT_NAME}-static STATIC
	${PUBLIC_HEADERS}
	${SOURCES}
	"${CMAKE_SOURCE_DIR}/thirdparty/lodepng/lodepng.cpp"
)

target_link_libraries(${PROJECT_NAME}-static
	PUBLIC
		$<IF:$<BOOL:MINGW>,"-lmingw32","">
		fmt::fmt
		glm
		SDL2::SDL2main SDL2::SDL2-static
)

target_include_directories(${PROJECT_NAME}-static
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
		$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/thirdparty>
		$<INSTALL_INTERFACE:include>
)

target_compile_definitions(${PROJECT_NAME}-static
	PUBLIC
		GLM_SWIZZLE
		NOMINMAX # windows.h interferes with GLM under MSVC if not defined
)

set_target_properties(${PROJECT_NAME}-static PROPERTIES
	DEBUG_POSTFIX _d
)

configure_file("${PROJECT_NAME}-static-config.cmake.in" "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-static-config.cmake" @ONLY)
# TODO: install target version config file

install(TARGETS ${PROJECT_NAME}-static EXPORT ${PROJECT_NAME}-static)
install(EXPORT ${PROJECT_NAME}-static DESTINATION "lib/cmake/${PROJECT_NAME}-static" NAMESPACE ${PROJECT_NAME}::)
install(DIRECTORY "src/smorgasbord" DESTINATION "include" FILES_MATCHING REGEX ".+\.hpp|.+\.inl")
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-static-config.cmake" DESTINATION "lib/cmake/${PROJECT_NAME}-static")

option(SMORGASBORD_BUILD_SAMPLES "Build sample projects" ON)

if (SMORGASBORD_BUILD_SAMPLES)
	# samples would call find_library(smorgasbord-static REQUIRED), but we can make the call NOP with this
	add_library(${PROJECT_NAME}::${PROJECT_NAME}-static ALIAS ${PROJECT_NAME}-static)
	file(CONFIGURE OUTPUT "${PROJECT_BINARY_DIR}/cmake_dummy/${PROJECT_NAME}-static-config.cmake" CONTENT "# dummy config cmake")
	set(${PROJECT_NAME}-static_ROOT "${PROJECT_BINARY_DIR}/cmake_dummy" CACHE FILEPATH "Path to dummy config file for in-project build")

	add_subdirectory("${PROJECT_SOURCE_DIR}/samples/liltown" liltown_sample)
endif()
