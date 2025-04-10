#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <stddef.h>
#include <stdlib.h>
#include <functional>
#include <memory>

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

// Structure to hold comparison context with proper lifetime management
struct ComparisonContext {
    py::object comp_func;
    
    // Constructor to ensure proper reference counting
    ComparisonContext(py::object func) : comp_func(func) {}
    
    // Destructor to ensure proper cleanup
    ~ComparisonContext() {
        // No need to explicitly clear comp_func, py::object handles this
    }
    
    // Static comparison function that can be passed to C code
    static Ordertype compare(void* ctx_ptr, void* a, void* b) {
        auto* context = static_cast<ComparisonContext*>(ctx_ptr);
        try {
            py::object* obj_a = static_cast<py::object*>(a);
            py::object* obj_b = static_cast<py::object*>(b);
            py::object result = context->comp_func(*obj_a, *obj_b);
            return py::cast<Ordertype>(result);
        } catch (const py::error_already_set& e) {
            e.clear();
            return NOT_COMPARABLE;
        }
    }
};

// Wrapper for merge_sort that works with Python lists
py::list py_merge_sort(py::list input_list, py::object comp_func) {
    // Create a context that will be kept alive during the entire sort operation
    ComparisonContext context(comp_func);
    
    // Convert Python list to vector of objects with proper reference counting
    std::vector<py::object> vec;
    vec.reserve(input_list.size());
    for (const auto& item : input_list) {
        vec.push_back(py::cast<py::object>(item));
    }
    
    // Create vector of pointers to objects
    std::vector<void*> ptr_vec(vec.size());
    for (size_t i = 0; i < vec.size(); i++) {
        ptr_vec[i] = &vec[i];
    }
    
    // Call merge_sort with our context-aware comparison function
    merge_sort(ptr_vec.data(), static_cast<int>(vec.size()), 
              [&context](void* a, void* b) -> Ordertype {
                  return ComparisonContext::compare(&context, a, b);
              });
    
    // Create result list
    py::list result;
    for (size_t i = 0; i < vec.size(); i++) {
        result.append(*static_cast<py::object*>(ptr_vec[i]));
    }
    
    return result;
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
    m.def("merge_sort", &py_merge_sort,
          "Sort a list using merge sort with a custom comparison function",
          py::arg("lst"), py::arg("comp_func"));
} 