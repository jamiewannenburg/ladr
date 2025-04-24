cmake_minimum_required(VERSION 3.18)
project(ladr)

# Find Python and pybind11
find_package(Python 3.7 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# Find NumPy
execute_process(
  COMMAND "${Python_EXECUTABLE}" -c "import numpy; print(numpy.get_include())"
  OUTPUT_VARIABLE NumPy_INCLUDE_DIRS
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Set position independent code for all targets
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-g)


# Create a library from our term implementation and all its dependencies
add_library(term_lib STATIC
    ../ladr/term.c
    ../ladr/symbols.c
    ../ladr/memory.c
    ../ladr/strbuf.c
    ../ladr/string.c
    ../ladr/hash.c
    ../ladr/avltree.c
    ../ladr/glist.c
    ../ladr/order.c
    ../ladr/fatal.c
)

# Create the term module
pybind11_add_module(term src/ladr/term.cpp)
target_link_libraries(term PRIVATE term_lib)

# Install the Python modules
install(TARGETS term DESTINATION ladr)
