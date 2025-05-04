#ifndef LADR_TERM_HPP
#define LADR_TERM_HPP
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "glist.hpp"

extern "C" {
    #include "../../../ladr/term.h"
}

bool is_rigid_term(Term t);
std::string term_to_string_cpp(Term t);


namespace py = pybind11;

// Declare TermDeleter defined in term.cpp

// Custom deleter for Term objects
struct TermDeleter {
    void operator()(Term t) const {
        if (t == NULL) return;
        
        try {
            // Extra safety check - don't try to free variables
            if (t->private_symbol >= 0) {
                // Variables don't need to be freed
                return;
            }
            
            bool is_rigid = is_rigid_term(t);
            if (is_rigid) {
                free_term(t);
            } else {
                // Carefully check each argument before zapping
                bool can_safely_zap = true;
                if (t->arity > 0 && t->args) {
                    for (int i = 0; i < t->arity; i++) {
                        if (t->args[i] == NULL) {
                            can_safely_zap = false;
                            break;
                        }
                    }
                }
                
                if (can_safely_zap) {
                    zap_term(t);
                } else {
                    // Fall back to free_term for any unsafe cases
                    free_term(t);
                }
            }
        } catch (...) {
            // Last resort - if anything goes wrong, try free_term
            try {
                free_term(t);
            } catch (...) {
                // If all else fails, just leak the memory rather than crash
                // std::cerr << "Warning: Failed to properly free term " << t << std::endl;
            }
        }
    }
};

using PyTerm = std::unique_ptr<struct term, TermDeleter>;

// Forward declaration of Term wrapper
// This represents the Python-wrapped Term class
class TermWrapper; 

// Specialization for Term
template<>
struct PlistTraits<Term> {
    // Convert C Term to Python Term object
    static py::object to_python(void* ptr) {
        // Assume we have a Term* in ptr
        Term term_ptr = static_cast<Term>(ptr);
        
        // We need to create a copy to ensure memory safety
        Term term_copy = copy_term(term_ptr);
        
        // Return a new Python Term object
        return py::cast(term_copy, py::return_value_policy::take_ownership);
    }
    
    // Convert Python Term object to C Term*
    static void* from_python(py::handle obj) {
        // Extract the C Term from the Python object
        Term term = obj.cast<Term>();
        
        // Make a copy for the Plist to own
        return static_cast<void*>(copy_term(term));
    }
    
    // Check if Python object is a Term
    static bool check_type(py::handle obj) {
        try {
            obj.cast<Term>();
            return true;
        } catch (const py::cast_error&) {
            return false;
        }
    }
};

namespace pybind11 { namespace detail {
    template <>
    struct type_caster<Plist> {
    public:
        PYBIND11_TYPE_CASTER(Plist, _("List[Term]"));
        
        // Python list -> Plist
        bool load(py::handle src, bool convert) {
            if (!py::isinstance<py::list>(src))
                return false;
                
            try {
                value = plist_caster<Term>::from_python_list(src.cast<py::list>());
                return true;
            } catch (const py::error_already_set&) {
                return false;
            }
        }
        
        // Plist -> Python list
        static py::handle cast(Plist src, py::return_value_policy policy, py::handle parent) {
            py::list result = plist_caster<Term>::to_python_list(src);
            return result.release();
        }
    };
}}

#endif // LADR_TERM_HPP
