cmake_minimum_required(VERSION 3.18)
project(ladr)

# Find Python and pybind11
find_package(Python 3.7 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# Create a library from our order implementation
add_library(order_lib STATIC
    ../ladr/order.c
)

# Create the Python modules
pybind11_add_module(order ladr_bindings/order_bindings.cpp)
target_link_libraries(order PRIVATE order_lib)

# Create the header module
pybind11_add_module(header ladr_bindings/header_bindings.cpp)

# Install the Python modules
install(TARGETS order DESTINATION ladr)
install(TARGETS header DESTINATION ladr)
