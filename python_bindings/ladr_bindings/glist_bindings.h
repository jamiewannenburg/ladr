#ifndef GLIST_BINDINGS_H
#define GLIST_BINDINGS_H

#include "glist_wrapper.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <Python.h>

namespace py = pybind11;

Plist python_list_to_plist(py::list py_list);
py::list plist_to_python_list(Plist plist);

#endif // GLIST_BINDINGS_H 