cmake_minimum_required(VERSION 3.18)
project(ladr_string)

# Find Python and pybind11
find_package(Python 3.7 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# Create a library from our order implementation
add_library(order_lib STATIC
    ladr_string/order.c
)

# Create the Python modules
pybind11_add_module(order_bindings ladr_string/order_bindings.cpp)
target_link_libraries(order_bindings PRIVATE order_lib)

# Create the header module
pybind11_add_module(header_bindings ladr_header/header_bindings.cpp)

# Install the Python modules
install(TARGETS order_bindings DESTINATION ladr_string)
install(TARGETS header_bindings DESTINATION ladr_string)
