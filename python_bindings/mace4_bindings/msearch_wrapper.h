#ifndef MSEARCH_WRAPPER_H
#define MSEARCH_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

// Opaque pointers for types used in Mace4 bindings to avoid conflicts
struct mace_options;
typedef struct mace_options * Mace_options;
struct mace_results;
typedef struct mace_results * Mace_results;
struct mace_stats;
typedef struct mace_stats Mace_stats;
struct plist;
typedef struct plist * Plist;
typedef int Mace_BOOL;

// Wrapper functions to avoid C++ keyword conflicts
void msearch_init_mace_options(Mace_options opt);
Mace_results msearch_mace4(Plist clauses, Mace_options opt);
void msearch_mace4_exit(int exit_code);

#ifdef __cplusplus
}
#endif

// Structure definitions for C++ code without including original headers
#ifdef __cplusplus

struct mace_options {
  // flags
  int print_models,
    print_models_tabular,
    lnh,
    trace,
    negprop,
    neg_assign,
    neg_assign_near,
    neg_elim,
    neg_elim_near,
    verbose,
    integer_ring,
    order_domain,
    arithmetic,
    iterate_primes,
    iterate_nonprimes,
    skolems_last,
    return_models;  // special case

  // parms
  int domain_size,
    start_size,
    end_size,
    iterate_up_to,
    increment,
    max_models,
    selection_order,
    selection_measure,
    max_seconds,
    max_seconds_per,
    max_megs,
    report_stderr;

  // stringparms
  int iterate;
};

struct mace_stats {
  // stats for the current domain size
  unsigned
    current_models,
    selections,
    assignments,
    propagations,
    cross_offs,
    rewrite_terms,
    rewrite_bools,
    indexes,
    ground_clauses_seen,
    ground_clauses_kept,
    rules_from_neg,
    neg_elim_attempts,
    neg_elim_agone,
    neg_elim_egone,
    neg_assign_attempts,
    neg_assign_agone,
    neg_assign_egone,
    neg_near_assign_attempts,
    neg_near_assign_agone,
    neg_near_assign_egone,
    neg_near_elim_attempts,
    neg_near_elim_agone,
    neg_near_elim_egone;
};

struct mace_results {
  Mace_BOOL success;
  void *models;  // Changed from Plist to void* to represent Python list
  double user_seconds;
  int return_code;
};

#endif

#endif // MSEARCH_WRAPPER_H
