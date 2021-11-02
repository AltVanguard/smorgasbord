set(CMAKE_CXX_STANDARD 17)

find_package(smorgasbord-static REQUIRED)

project(liltown)

file(GLOB_RECURSE HEADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

file(GLOB_RECURSE SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE SHADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/data/*.shader"
	"${CMAKE_CURRENT_SOURCE_DIR}/data/*.compute"
	"${CMAKE_CURRENT_SOURCE_DIR}/data/*.rasterize"
	"${CMAKE_CURRENT_SOURCE_DIR}/data/*.raytrace"
)

set_source_files_properties(${SHADERS} PROPERTIES
	HEADER_FILE_ONLY TRUE
)

add_executable(${PROJECT_NAME}
	${HEADERS}
	${SOURCES}
	${SHADERS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	WIN32_EXECUTABLE "${WIN32}"
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_DIR}"
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE smorgasbord::smorgasbord-static
)
