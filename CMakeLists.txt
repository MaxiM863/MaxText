cmake_minimum_required(VERSION 3.10)

project(MaxText)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)

FetchContent_MakeAvailable(glm)

FetchContent_Declare(
	CookbookWrapperLib
	GIT_REPOSITORY https://github.com/MaxiM863/Cookbook.git
    GIT_TAG main
)

FetchContent_MakeAvailable(CookbookWrapperLib)

add_subdirectory(TextEngine)

# Define a library target
add_library(MaxText STATIC TextEngine/LettersObj.cpp)
target_include_directories(MaxText PUBLIC
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/TextEngine>
$<INSTALL_INTERFACE:/TextEngine> # relative to install prefix
)

install(TARGETS MaxText
    EXPORT MaxTextTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
# Export the target to a file
export(     
EXPORT MaxTextTargets
FILE "${CMAKE_CURRENT_BINARY_DIR}/MaxTextTargets.cmake"
NAMESPACE MaxText::

)

# Install the export file
install(EXPORT MaxTextTargets
    FILE MaxTextTargets.cmake
    DESTINATION lib/cmake/MaxText
)