#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

// Ensure C linkage for all the LADR headers
extern "C" {
    #include "../../../ladr/header.h"
    #include "../../../ladr/order.h"
}

namespace py = pybind11;

// Function to initialize the order module
void init_order_module(py::module_& m) {
    // Define the OrderType enum
    py::enum_<Ordertype>(m, "OrderType")
        .value("NOT_COMPARABLE", NOT_COMPARABLE)
        .value("SAME_AS", SAME_AS)
        .value("LESS_THAN", LESS_THAN)
        .value("GREATER_THAN", GREATER_THAN)
        .value("LESS_THAN_OR_SAME_AS", LESS_THAN_OR_SAME_AS)
        .value("GREATER_THAN_OR_SAME_AS", GREATER_THAN_OR_SAME_AS)
        .value("NOT_LESS_THAN", NOT_LESS_THAN)
        .value("NOT_GREATER_THAN", NOT_GREATER_THAN)
        .export_values();

    // Define the compare_vecs function
    m.def("compare_vecs", [](py::list& a, py::list& b) {
        if (a.size() != b.size()) {
            throw std::runtime_error("Vectors must have the same length");
        }
        std::vector<int> vec_a(a.size());
        std::vector<int> vec_b(b.size());
        for (size_t i = 0; i < a.size(); i++) {
            vec_a[i] = a[i].cast<int>();
            vec_b[i] = b[i].cast<int>();
        }
        return compare_vecs(vec_a.data(), vec_b.data(), vec_a.size());
    }, "Compare two vectors", py::arg("a"), py::arg("b"));

    // Define the copy_vec function
    m.def("copy_vec", [](py::list& a, py::list& b) {
        if (a.size() != b.size()) {
            throw std::runtime_error("Vectors must have the same length");
        }
        std::vector<int> vec_a(a.size());
        std::vector<int> vec_b(b.size());
        for (size_t i = 0; i < a.size(); i++) {
            vec_a[i] = a[i].cast<int>();
            vec_b[i] = b[i].cast<int>();
        }
        copy_vec(vec_a.data(), vec_b.data(), vec_a.size());
        // Update the Python lists
        for (size_t i = 0; i < a.size(); i++) {
            a[i] = vec_a[i];
            b[i] = vec_b[i];
        }
    }, "Copy vector a to vector b", py::arg("a"), py::arg("b"));
} 