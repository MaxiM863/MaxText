cmake_minimum_required(VERSION 3.10)

project(MaxText)

add_subdirectory(TextEngine)

# Define a library target
add_library(MaxText STATIC TextEngine/test.hpp)

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
