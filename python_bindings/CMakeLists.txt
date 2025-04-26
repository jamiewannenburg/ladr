cmake_minimum_required(VERSION 3.18)
project(ladr)

# Find Python and pybind11
find_package(Python 3.7 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# # Find NumPy
# execute_process(
#   COMMAND "${Python_EXECUTABLE}" -c "import numpy; print(numpy.get_include())"
#   OUTPUT_VARIABLE NumPy_INCLUDE_DIRS
#   OUTPUT_STRIP_TRAILING_WHITESPACE
# )

# Set position independent code for all targets
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-g)

# Create a library from ladr directory (excluding fatal.c, which we'll replace)
set(LADR_GLOB "${PROJECT_SOURCE_DIR}/../ladr/*.c")
if(MSYS)
    string(REGEX REPLACE "([CDEFG]):" "/\\1" LADR_GLOB "${LADR_GLOB}")
endif()
file(GLOB LADR_SOURCES "${LADR_GLOB}")

# Remove the original fatal.c from the sources
list(FILTER LADR_SOURCES EXCLUDE REGEX ".*fatal.c$")

# Add our custom fatal_wrapper.c
list(APPEND LADR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/ladr/fatal_wrapper.c")

# Create the static library with our custom fatal_wrapper
add_library(ladr_lib STATIC ${LADR_SOURCES})

# Create the term module
pybind11_add_module(term src/ladr/term.cpp)
target_link_libraries(term PRIVATE ladr_lib)

# Create the glist module for list conversions
pybind11_add_module(glist src/ladr/glist.cpp)
target_link_libraries(glist PRIVATE ladr_lib)

# Create the parse module
pybind11_add_module(parse src/ladr/parse.cpp)
target_link_libraries(parse PRIVATE ladr_lib)

# Create the memory module
pybind11_add_module(memory src/ladr/memory.cpp)
target_link_libraries(memory PRIVATE ladr_lib)

# Create the error module
pybind11_add_module(error_handler src/ladr/error_handler.cpp)
target_link_libraries(error_handler PRIVATE ladr_lib)

# Install the Python modules
install(TARGETS term glist parse memory error_handler DESTINATION ladr)
