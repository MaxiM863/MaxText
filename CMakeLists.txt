cmake_minimum_required(VERSION 3.10)

project(MaxText LANGUAGES "CXX")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_LINKER_TYPE "CXX")

add_subdirectory(TextEngine)

# Define a library target
add_library(MaxText INTERFACE TextEngine/test.hpp)

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
NAMESPACE MaxText::MaxText

)
