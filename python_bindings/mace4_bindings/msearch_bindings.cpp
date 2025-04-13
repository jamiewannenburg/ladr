#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "msearch_wrapper.h"

namespace py = pybind11;

PYBIND11_MODULE(mace4, m) {
    m.doc() = "Python bindings for Mace4";

    // Bind Mace_options structure
    py::class_<mace_options>(m, "MaceOptions")
        .def(py::init<>())
        .def_readwrite("print_models", &mace_options::print_models)
        .def_readwrite("print_models_tabular", &mace_options::print_models_tabular)
        .def_readwrite("lnh", &mace_options::lnh)
        .def_readwrite("trace", &mace_options::trace)
        .def_readwrite("negprop", &mace_options::negprop)
        .def_readwrite("neg_assign", &mace_options::neg_assign)
        .def_readwrite("neg_assign_near", &mace_options::neg_assign_near)
        .def_readwrite("neg_elim", &mace_options::neg_elim)
        .def_readwrite("neg_elim_near", &mace_options::neg_elim_near)
        .def_readwrite("verbose", &mace_options::verbose)
        .def_readwrite("integer_ring", &mace_options::integer_ring)
        .def_readwrite("order_domain", &mace_options::order_domain)
        .def_readwrite("arithmetic", &mace_options::arithmetic)
        .def_readwrite("iterate_primes", &mace_options::iterate_primes)
        .def_readwrite("iterate_nonprimes", &mace_options::iterate_nonprimes)
        .def_readwrite("skolems_last", &mace_options::skolems_last)
        .def_readwrite("return_models", &mace_options::return_models)
        .def_readwrite("domain_size", &mace_options::domain_size)
        .def_readwrite("start_size", &mace_options::start_size)
        .def_readwrite("end_size", &mace_options::end_size)
        .def_readwrite("iterate_up_to", &mace_options::iterate_up_to)
        .def_readwrite("increment", &mace_options::increment)
        .def_readwrite("max_models", &mace_options::max_models)
        .def_readwrite("selection_order", &mace_options::selection_order)
        .def_readwrite("selection_measure", &mace_options::selection_measure)
        .def_readwrite("max_seconds", &mace_options::max_seconds)
        .def_readwrite("max_seconds_per", &mace_options::max_seconds_per)
        .def_readwrite("max_megs", &mace_options::max_megs)
        .def_readwrite("report_stderr", &mace_options::report_stderr)
        .def_readwrite("iterate", &mace_options::iterate);

    // Bind Mace_stats structure
    py::class_<mace_stats>(m, "MaceStats")
        .def(py::init<>())
        .def_readonly("current_models", &mace_stats::current_models)
        .def_readonly("selections", &mace_stats::selections)
        .def_readonly("assignments", &mace_stats::assignments)
        .def_readonly("propagations", &mace_stats::propagations)
        .def_readonly("cross_offs", &mace_stats::cross_offs)
        .def_readonly("rewrite_terms", &mace_stats::rewrite_terms)
        .def_readonly("rewrite_bools", &mace_stats::rewrite_bools)
        .def_readonly("indexes", &mace_stats::indexes)
        .def_readonly("ground_clauses_seen", &mace_stats::ground_clauses_seen)
        .def_readonly("ground_clauses_kept", &mace_stats::ground_clauses_kept)
        .def_readonly("rules_from_neg", &mace_stats::rules_from_neg)
        .def_readonly("neg_elim_attempts", &mace_stats::neg_elim_attempts)
        .def_readonly("neg_elim_agone", &mace_stats::neg_elim_agone)
        .def_readonly("neg_elim_egone", &mace_stats::neg_elim_egone)
        .def_readonly("neg_assign_attempts", &mace_stats::neg_assign_attempts)
        .def_readonly("neg_assign_agone", &mace_stats::neg_assign_agone)
        .def_readonly("neg_assign_egone", &mace_stats::neg_assign_egone)
        .def_readonly("neg_near_assign_attempts", &mace_stats::neg_near_assign_attempts)
        .def_readonly("neg_near_assign_agone", &mace_stats::neg_near_assign_agone)
        .def_readonly("neg_near_assign_egone", &mace_stats::neg_near_assign_egone)
        .def_readonly("neg_near_elim_attempts", &mace_stats::neg_near_elim_attempts)
        .def_readonly("neg_near_elim_agone", &mace_stats::neg_near_elim_agone)
        .def_readonly("neg_near_elim_egone", &mace_stats::neg_near_elim_egone);

    // Bind Mace_results structure
    py::class_<mace_results>(m, "MaceResults")
        .def(py::init<>())
        .def_readonly("success", &mace_results::success)
        .def_readonly("user_seconds", &mace_results::user_seconds)
        .def_readonly("return_code", &mace_results::return_code)
        .def_property_readonly("models", [](const mace_results &res) {
            return reinterpret_cast<PyObject*>(res.models);
        });

    // Bind functions
    m.def("init_mace_options", &msearch_init_mace_options, "Initialize Mace4 options");
    m.def("mace4", [](py::list clauses, mace_options opt) {
        Plist plist_clauses = python_list_to_plist(clauses.ptr());
        Mace_results res = msearch_mace4(plist_clauses, &opt);
        // Note: Memory management for plist_clauses needs to be handled appropriately
        return res;
    }, "Run Mace4 with the given clauses and options", py::arg("clauses"), py::arg("opt"));
    m.def("mace4_exit", &msearch_mace4_exit, "Exit Mace4 with the given exit code", py::arg("exit_code"));
}
