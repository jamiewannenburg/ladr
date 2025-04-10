cmake_minimum_required(VERSION 3.18)
project(ladr)

# Find Python and pybind11
find_package(Python 3.7 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# Set position independent code for all targets
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-g)

# Create a library from our order implementation
add_library(order_lib STATIC
    ../ladr/order.c
)

# Create a library from our fatal implementation
add_library(fatal_lib STATIC
    ../ladr/fatal.c
)

# Create the Python modules
pybind11_add_module(order ladr_bindings/order_bindings.cpp)
target_link_libraries(order PRIVATE order_lib)

# Create the header module
pybind11_add_module(header ladr_bindings/header_bindings.cpp)

# Create the fatal module - link against fatal_lib to use the original implementation
pybind11_add_module(fatal ladr_bindings/fatal_bindings.cpp)
target_link_libraries(fatal PRIVATE fatal_lib)

# Install the Python modules
install(TARGETS order DESTINATION ladr)
install(TARGETS header DESTINATION ladr)
install(TARGETS fatal DESTINATION ladr)
