add_library(ladr_header MODULE
    header_bindings.cpp
)

target_link_libraries(ladr_header PRIVATE
    pybind11::module
    ladr
)

set_target_properties(ladr_header PROPERTIES
    PREFIX ""
    SUFFIX ".so"
) 