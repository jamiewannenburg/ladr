#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <stddef.h>
#include <stdlib.h>
#include <functional>

// Define BOOL type since we're not including the full LADR headers
typedef int BOOL;
#define TRUE 1
#define FALSE 0

// Forward declare the Ordertype enum and functions from order.c
typedef enum { NOT_COMPARABLE,
              SAME_AS,
              LESS_THAN,
              GREATER_THAN,
              LESS_THAN_OR_SAME_AS,
              GREATER_THAN_OR_SAME_AS,
              NOT_LESS_THAN,
              NOT_GREATER_THAN
} Ordertype;

// Forward declare the functions we need from order.c
extern "C" {
    void merge_sort(void *a[], int n, Ordertype (*comp_proc) (void *, void *));
    Ordertype compare_vecs(int *a, int *b, int n);
    void copy_vec(int *a, int *b, int n);
}

namespace py = pybind11;

// Global comparison function and state
static std::function<Ordertype(const py::object&, const py::object&)> g_comp_func;

extern "C" {
    static Ordertype global_comp_proc(void* a, void* b) {
        return g_comp_func(*static_cast<py::object*>(a), *static_cast<py::object*>(b));
    }
}

// Wrapper for merge_sort that works with Python lists
void py_merge_sort(std::vector<py::object>& vec, 
                  const std::function<Ordertype(const py::object&, const py::object&)>& comp_func) {
    // Convert vector to array of pointers
    std::vector<py::object*> ptr_vec(vec.size());
    for (size_t i = 0; i < vec.size(); i++) {
        ptr_vec[i] = &vec[i];
    }

    // Set the global comparison function
    g_comp_func = comp_func;

    // Call merge_sort
    merge_sort(reinterpret_cast<void**>(ptr_vec.data()), 
              static_cast<int>(vec.size()), 
              global_comp_proc);

    // Reorder original vector based on sorted pointers
    std::vector<py::object> result(vec.size());
    for (size_t i = 0; i < vec.size(); i++) {
        result[i] = *ptr_vec[i];
    }
    vec = std::move(result);
}

PYBIND11_MODULE(order, m) {
    m.doc() = "Python bindings for order module";

    // Bind the Ordertype enum
    py::enum_<Ordertype>(m, "Ordertype")
        .value("NOT_COMPARABLE", NOT_COMPARABLE)
        .value("SAME_AS", SAME_AS)
        .value("LESS_THAN", LESS_THAN)
        .value("GREATER_THAN", GREATER_THAN)
        .value("LESS_THAN_OR_SAME_AS", LESS_THAN_OR_SAME_AS)
        .value("GREATER_THAN_OR_SAME_AS", GREATER_THAN_OR_SAME_AS)
        .value("NOT_LESS_THAN", NOT_LESS_THAN)
        .value("NOT_GREATER_THAN", NOT_GREATER_THAN);

    // Bind compare_vecs
    m.def("compare_vecs", [](const std::vector<int>& a, const std::vector<int>& b) {
        if (a.size() != b.size()) {
            throw py::value_error("Vectors must have the same length");
        }
        return compare_vecs(const_cast<int*>(a.data()), 
                          const_cast<int*>(b.data()), 
                          static_cast<int>(a.size()));
    }, "Compare two integer vectors", py::arg("a"), py::arg("b"));

    // Bind copy_vec
    m.def("copy_vec", [](const std::vector<int>& a) {
        std::vector<int> b(a.size());
        copy_vec(const_cast<int*>(a.data()), b.data(), static_cast<int>(a.size()));
        return b;
    }, "Copy an integer vector", py::arg("a"));

    // Bind merge_sort with a more Pythonic interface
    m.def("merge_sort", [](py::list lst, py::function comp_func) {
        // Convert Python list to vector
        std::vector<py::object> vec;
        for (auto item : lst) {
            vec.push_back(py::cast<py::object>(item));
        }

        // Create comparison function
        auto py_comp = std::function<Ordertype(const py::object&, const py::object&)>(
            [comp_func](const py::object& a, const py::object& b) -> Ordertype {
                py::object result = comp_func(a, b);
                return py::cast<Ordertype>(result);
            }
        );

        // Sort the vector
        py_merge_sort(vec, py_comp);

        // Convert back to Python list
        py::list result;
        for (const auto& item : vec) {
            result.append(item);
        }
        return result;
    }, "Sort a list using merge sort with a custom comparison function",
       py::arg("lst"), py::arg("comp_func"));
} 