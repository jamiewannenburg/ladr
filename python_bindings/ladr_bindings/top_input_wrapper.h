#ifndef TOP_INPUT_WRAPPER_H
#define TOP_INPUT_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

// Opaque pointers for types used in Mace4 bindings to avoid conflicts
struct topform;
typedef struct topform * Topform;
struct plist;
typedef struct plist * Plist;

Plist embed_formulas_in_topforms_wrapper(Plist formulas, int is_input);

#ifdef __cplusplus
}
#endif

// Structure definitions for C++ code without including original headers
#ifdef __cplusplus

/* from topform.h */
struct topform {

  /* for both clauses and formulas */

  int              id;
//  struct clist_pos *containers;     /* Clists that contain the Topform, TODO from clist.h, add bindings? */
//  Attribute        attributes; // TODO from attribute.h, add bindings?
//  struct just      *justification //TODO from just.h, add bindings?
  double           weight;
  char             *compressed;     /* if nonNULL, a compressed form */
  Topform          matching_hint;   /* hint that matches clause, if any */

  /* for clauses only */

//  Literals         literals;        /* NULL can mean the empty clause  *TODO from literals.h, add bindings?*/

  /* for formulas only */

//  Formula          formula; // TODO from formula.h, add bindings?

  int   semantics;        /* evaluation in interpretations */

  /* The rest of the fields are flags.  These could be bits. */

  char   is_formula;      /* is this really a formula? */
  char   normal_vars;     /* variables have been renumbered */
  char   used;            /* used to infer a clause that was kept */
  char   official_id;     /* Topform is in the ID table */
  char   initial;         /* existed at the start of the search */
  char   neg_compressed;  /* negative and compressed */
  char   subsumer;        /* has this clause back subsumed anything? */

};

#endif

#endif // TOP_INPUT_WRAPPER_H