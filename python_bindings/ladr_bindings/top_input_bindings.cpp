#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "top_input_wrapper.h"
#include "glist_bindings.h"
namespace py = pybind11;

PYBIND11_MODULE(top_input, m) {
    m.doc() = "Python bindings for LADR top_input.h functions and structures";

    py::class_<topform>(m, "Topform")
        .def(py::init<>())
        .def_readwrite("id", &topform::id)
        .def_readwrite("weight", &topform::weight)
        .def_readwrite("compressed", &topform::compressed)
        .def_readwrite("semantics", &topform::semantics)
        .def_readwrite("is_formula", &topform::is_formula)
        .def_readwrite("normal_vars", &topform::normal_vars)
        .def_readwrite("used", &topform::used)
        .def_readwrite("official_id", &topform::official_id)
        .def_readwrite("initial", &topform::initial)
        .def_readwrite("neg_compressed", &topform::neg_compressed)
        .def_readwrite("subsumer", &topform::subsumer)
        .def_readwrite("matching_hint", &topform::matching_hint); //TODO it's a TopForm Should I tell pybind11 to use the TopForm class?
        // .def_property_readonly("containers", [](const Topform &topform) {
        //     return reinterpret_cast<PyObject*>(topform.containers);
        // })
        // .def_readwrite("justification", &topform::justification) //TODO from just.h, add bindings?
        // .def_readwrite("attributes", &topform::attributes) //TODO from attribute.h, add bindings?
        // .def_readwrite("literals", &topform::literals) //TODO from literals.h, add bindings?
        // .def_readwrite("formula", &topform::formula) //TODO from formula.h, add bindings?

    m.def("embed_formulas_in_topforms", [](py::list formulas, int is_input) {
        Plist plist = python_list_to_plist(formulas);
        Plist result = embed_formulas_in_topforms_wrapper(plist, is_input);
        py::list result_list = plist_to_python_list(result);
        return result_list;
    }, "Convert a list of formulas into Topform structures.", py::arg("formulas"), py::arg("is_input"));
} 