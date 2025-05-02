#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>

// Include the C header
extern "C" {
    #include "../../../ladr/glist.h"
}

namespace py = pybind11;

//------------------------------------------------
// Ilist Caster - Complete implementation
//------------------------------------------------

namespace pybind11 { namespace detail {
    template <>
    struct type_caster<Ilist> {
    public:
        PYBIND11_TYPE_CASTER(Ilist, _("List[int]"));

        // Python list -> C Ilist
        bool load(py::handle src, bool convert) {
            if (!py::isinstance<py::list>(src))
                return false;

            py::list pylist = src.cast<py::list>();
            
            // Create empty Ilist
            value = NULL;
            
            // Convert each item to int and add to Ilist
            for (auto item : pylist) {
                if (!py::isinstance<py::int_>(item))
                    return false;
                
                int val = item.cast<int>();
                // Use LADR's functions to build the Ilist (append to head)
                value = ilist_prepend(value, val);
            }
            
            // Reverse to maintain original order (since we built it backwards)
            value = reverse_ilist(value);
            
            return true;
        }

        // C Ilist -> Python list
        static py::handle cast(Ilist src, py::return_value_policy policy, py::handle parent) {
            py::list result;
            
            // Convert Ilist to Python list
            Ilist current = src;
            while (current != NULL) {
                result.append(py::int_(current->i));
                current = current->next;
            }
            
            return result.release();
        }
    };
}} // namespace pybind11::detail


//------------------------------------------------
// Plist Template Caster
//------------------------------------------------

// Forward declaration - we don't need the complete Term here
extern "C" {
    // This is the C struct that matches what's in term.h
    struct term;
    typedef struct term * Term;
}

// Template trait to convert between Plist<T> and Python list
template<typename T>
struct PlistTraits {
    // These will be specialized for each type
    static py::object to_python(void* ptr);
    static void* from_python(py::handle obj);
    static bool check_type(py::handle obj);
};

// Generic Plist caster implementation
namespace pybind11 { namespace detail {
    template<typename T>
    struct plist_caster {
        // Convert a Plist to a Python list
        static py::list to_python_list(Plist list) {
            py::list result;
            Plist current = list;
            
            while (current != NULL) {
                result.append(PlistTraits<T>::to_python(current->v));
                current = current->next;
            }
            
            return result;
        }
        
        // Convert a Python list to a Plist
        static Plist from_python_list(py::list pylist) {
            Plist result = NULL;
            
            // Process list in reverse to maintain O(n) complexity
            for (auto item : pylist) {
                if (!PlistTraits<T>::check_type(item))
                    throw py::type_error("Invalid type in list");
                
                Plist node = get_plist();  // LADR allocation function
                node->v = PlistTraits<T>::from_python(item);
                node->next = result;
                result = node;
            }
            
            // Reverse to restore original order
            return reverse_plist(result);
        }
    };
    
    // Type caster for Plist of type T - we'll specialize this in the conversions.hpp file
    // rather than trying to use enable_if here
}} // namespace pybind11::detail 