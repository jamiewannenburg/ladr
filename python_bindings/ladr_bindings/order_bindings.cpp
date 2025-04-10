#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
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

// Thread-safe comparison context with proper memory management
class ComparisonContext {
private:
    static thread_local std::shared_ptr<ComparisonContext> instance;
    py::object comp_func;

public:
    ComparisonContext(py::object func) : comp_func(func) {}
    
    ~ComparisonContext() {
        // Release the Python function reference
        comp_func = py::none();
    }
    
    static void setInstance(const std::shared_ptr<ComparisonContext>& ctx) {
        instance = ctx;
    }
    
    static std::shared_ptr<ComparisonContext> getInstance() {
        return instance;
    }
    
    Ordertype compare(py::object a, py::object b) {
        try {
            py::object result = comp_func(a, b);
            return py::cast<Ordertype>(result);
        } catch (const py::error_already_set& e) {
            return NOT_COMPARABLE;
        }
    }
    
    // Static wrapper for C callbacks
    static Ordertype staticCompare(void* a, void* b) {
        if (!instance) {
            return NOT_COMPARABLE;
        }
        
        py::object* obj_a = static_cast<py::object*>(a);
        py::object* obj_b = static_cast<py::object*>(b);
        
        return instance->compare(*obj_a, *obj_b);
    }
};

// Initialize the thread_local instance
thread_local std::shared_ptr<ComparisonContext> ComparisonContext::instance = nullptr;

// Wrapper for merge_sort that works with Python lists
py::list py_merge_sort(py::list input_list, py::object comp_func) {
    // Create a context that will be kept alive during the entire sort operation
    auto context = std::make_shared<ComparisonContext>(comp_func);
    ComparisonContext::setInstance(context);
    
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
    
    // Call merge_sort with our static comparison function
    merge_sort(ptr_vec.data(), static_cast<int>(vec.size()), &ComparisonContext::staticCompare);
    
    // Create result list
    py::list result;
    for (size_t i = 0; i < vec.size(); i++) {
        result.append(*static_cast<py::object*>(ptr_vec[i]));
    }
    
    // Clear the context to avoid memory leaks
    ComparisonContext::setInstance(nullptr);
    
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
    
    // Bind compare_vecs to work with NumPy arrays or Python lists
    m.def("compare_vecs", [](py::object a, py::object b) {
        // Try to convert to NumPy arrays if they're not already
        py::array_t<int> arr_a;
        py::array_t<int> arr_b;
        
        try {
            arr_a = py::cast<py::array_t<int>>(a);
            arr_b = py::cast<py::array_t<int>>(b);
        } catch (const py::cast_error&) {
            // If we can't cast to NumPy arrays, try to handle as Python lists
            try {
                std::vector<int> vec_a = py::cast<std::vector<int>>(a);
                std::vector<int> vec_b = py::cast<std::vector<int>>(b);
                
                if (vec_a.size() != vec_b.size()) {
                    throw py::value_error("Vectors must have the same length");
                }
                
                return compare_vecs(vec_a.data(), vec_b.data(), static_cast<int>(vec_a.size()));
            } catch (const py::cast_error&) {
                throw py::type_error("Arguments must be lists or arrays of integers");
            }
        }
        
        py::buffer_info buf_a = arr_a.request(), buf_b = arr_b.request();
        
        if (buf_a.ndim != 1 || buf_b.ndim != 1) {
            throw py::value_error("Number of dimensions must be one");
        }
        
        if (buf_a.shape[0] != buf_b.shape[0]) {
            throw py::value_error("Vectors must have the same length");
        }
        
        return compare_vecs(static_cast<int*>(buf_a.ptr), 
                            static_cast<int*>(buf_b.ptr), 
                            static_cast<int>(buf_a.shape[0]));
    }, "Compare two integer vectors or arrays", py::arg("a"), py::arg("b"));
    
    // Bind copy_vec to work with NumPy arrays or Python lists
    m.def("copy_vec", [](py::object a) -> py::object {
        // Try to convert to NumPy array if it's not already
        py::array_t<int> arr_a;
        
        try {
            arr_a = py::cast<py::array_t<int>>(a);
            py::buffer_info buf_a = arr_a.request();
            
            if (buf_a.ndim != 1) {
                throw py::value_error("Number of dimensions must be one");
            }
            
            py::array_t<int> result(buf_a.shape[0]);
            py::buffer_info buf_result = result.request();
            
            copy_vec(static_cast<int*>(buf_a.ptr), 
                    static_cast<int*>(buf_result.ptr),
                    static_cast<int>(buf_a.shape[0]));
            
            return result;
        } catch (const py::cast_error&) {
            // If we can't cast to NumPy array, try to handle as Python list
            try {
                std::vector<int> vec_a = py::cast<std::vector<int>>(a);
                std::vector<int> result(vec_a.size());
                
                copy_vec(vec_a.data(), result.data(), static_cast<int>(vec_a.size()));
                
                return py::cast(result);
            } catch (const py::cast_error&) {
                throw py::type_error("Argument must be a list or array of integers");
            }
        }
    }, "Copy an integer vector or array", py::arg("a"));
    
    // Bind merge_sort with a more Pythonic interface
    m.def("merge_sort", &py_merge_sort,
          "Sort a list using merge sort with a custom comparison function",
          py::arg("lst"), py::arg("comp_func"));
} 