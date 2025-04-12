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

# # Create a library from our order implementation
# add_library(order_lib STATIC
#     ../ladr/order.c
# )

# # Create a library from our fatal implementation
# add_library(fatal_lib STATIC
#     ../ladr/fatal.c
# )

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

# # Create the fatal module - link against fatal_lib to use the original implementation
# pybind11_add_module(fatal ladr_bindings/fatal_bindings.cpp)
# target_link_libraries(fatal PRIVATE fatal_lib)

# Create the term module
pybind11_add_module(term ladr_bindings/term_bindings.cpp)
target_link_libraries(term PRIVATE term_lib)

# Install the Python modules
install(TARGETS term DESTINATION ladr)
