#include "glist_bindings.h"
#include "glist_wrapper.h"

namespace py = pybind11;

Plist python_list_to_plist(py::list py_list) {
    return python_list_to_plist_wrapper(py_list.ptr());
}

py::list plist_to_python_list(Plist plist) {
    py::list pylist = py::cast(plist_to_python_list_wrapper(plist));
    return pylist;
}
