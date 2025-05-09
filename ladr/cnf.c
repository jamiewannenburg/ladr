/*  Copyright (C) 2006, 2007 William McCune

    This file is part of the LADR Deduction Library.

    The LADR Deduction Library is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License,
    version 2.

    The LADR Deduction Library is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the LADR Deduction Library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "cnf.h"

#include <setjmp.h>  /* Yikes! */
#include <signal.h>

/* The following has an optimization in which formulas are
   shared.  The main benefit of this is that when checking for
   identical formulas, we can compare pointers instead of
   traversing the formulas.  Another benefit is that when copying
   formulas (i.e., applying distributivity), we can copy pointers
   instead of whole formulas.

   The routine consolidate_formula() causes some subformulas to
   be shared.  It is currently applied to quantifier-free nnf formulas.

   shared:     ATOM, NOT
   not shared: AND, OR, ALL, EXISTS
*/   

/* Private definitions and types */

static jmp_buf Jump_env;        /* for setjmp/longjmp */

static unsigned Fid_call_limit = UINT_MAX;
static unsigned Fid_calls = 0;

/*************
 *
 *   share_formula()
 *
 *************/

static
Formula share_formula(Formula f, Hashtab h)
{
  if (f->type == AND_FORM || f->type == OR_FORM ||
      f->type == ALL_FORM || f->type == EXISTS_FORM) {
    int i;
    for (i = 0; i < f->arity; i++)
      f->kids[i] = share_formula(f->kids[i], h);
    return f;
  }
  else {
    unsigned hashval;
    Formula g;
    if  (f->type == NOT_FORM)
      f->kids[0] = share_formula(f->kids[0], h);

    hashval = hash_formula(f);
    g = hash_lookup(f, hashval, h,
		    (LADR_BOOL (*)(void *, void *)) formula_ident);
    if (g) {
      zap_formula(f);
      g->excess_refs++;
      return g;
    }
    else {
      hash_insert(f, hashval, h);
      return f;
    }
  }
}  /* share_formula */

/*************
 *
 *   consolidate_formula()
 *
 *************/

static
Formula consolidate_formula(Formula f)
{
  Hashtab h = hash_init(10000);
  f = share_formula(f, h);
  /* hash_info(h); */
  hash_destroy(h);
  return f;
}  /* consolidate_formula */

/*************
 *
 *   formula_ident_share()
 *
 *************/

/* DOCUMENTATION
This Boolean function checks if two formulas are identical.
The routine term_ident() checks identity of atoms.
<P>
The test is for strict identity---it does not consider
renamability of bound variables, permutability of AND or OR,
or symmetry of IFF or equality.
*/

/* PUBLIC */
LADR_BOOL formula_ident_share(Formula f, Formula g)
{
  if (Fid_call_limit != 0 && ++Fid_calls > Fid_call_limit) {
    printf("\n%% Fid_call limit; jumping home.\n");
    longjmp(Jump_env, 1);
  }

  if (f->type != g->type || f->arity != g->arity)
    return FALSE;
  else if (f->type == AND_FORM || f->type == OR_FORM) {
    int i;
    for (i = 0; i < f->arity; i++)
      if (!formula_ident_share(f->kids[i], g->kids[i]))
	return FALSE;
    return TRUE;
  }
  else if (f->type == ALL_FORM || f->type == EXISTS_FORM) {
    return
      str_ident(f->qvar, g->qvar) &&
      formula_ident_share(f->kids[0], g->kids[0]);
  }
  else
    return f == g;
}  /* formula_ident_share */

/*************
 *
 *   formula_copy_share()
 *
 *************/

/* DOCUMENTATION
This function returns a copy of the given formula.
All subformulas, including the atoms, are copied.
*/

/* PUBLIC */
Formula formula_copy_share(Formula f)
{
  if (f->type == AND_FORM || f->type == OR_FORM) {
    Formula g = formula_get(f->arity, f->type);
    int i;
    for (i = 0; i < f->arity; i++)
      g->kids[i] = formula_copy_share(f->kids[i]);
    return g;
  }
  else if (f->type == ALL_FORM || f->type == EXISTS_FORM) {
    Formula g = formula_get(1, f->type);
    g->qvar = f->qvar;
    g->kids[0] = formula_copy_share(f->kids[0]);
    return g;
  }
  else {
    f->excess_refs++;
    return f;
  }
}  /* formula_copy_share */

/*************
 *
 *   complementary_share()
 *
 *************/

static
LADR_BOOL complementary_share(Formula a, Formula b)
{
  return
    (a->type == NOT_FORM && formula_ident_share(a->kids[0], b))
    ||
    (b->type == NOT_FORM && formula_ident_share(a, b->kids[0]));
}  /* complementary_share */

/*************
 *
 *   contains_complements_share()
 *
 *   Assume AND_FORM or OR_FORM.
 *
 *************/

static
LADR_BOOL contains_complements_share(Formula f)
{
  int i, j;
  for (i = 0; i < f->arity-1; i++) {
    for (j = i+1; j < f->arity; j++) {
      if (complementary_share(f->kids[i], f->kids[j]))
	return TRUE;
    }
  }
  return FALSE;
}  /* contains_complements_share */

/*************
 *
 *   prop_member_share() -- is f an argument of g?
 *
 *************/

static
LADR_BOOL prop_member_share(Formula f, Formula g)
{
  int i;
  for (i = 0; i < g->arity; i++)
    if (formula_ident_share(f, g->kids[i]))
      return TRUE;
  return FALSE;
}  /* prop_member_share */

/*************
 *
 *   prop_subset_share() -- are the arguments of f a subset of the arguments of g?
 *
 *************/

static
LADR_BOOL prop_subset_share(Formula f, Formula g)
{
  int i;
  for (i = 0; i < f->arity; i++)
    if (!prop_member_share(f->kids[i], g))
      return FALSE;
  return TRUE;
}  /* prop_subset_share */

/*************
 *
 *   prop_subsume_share()
 *
 *   Assume disjunctions, atomic, TRUE, or FALSE
 *
 *************/

static
LADR_BOOL prop_subsume_share(Formula f, Formula g)
{
  if (FALSE_FORMULA(f))
    return TRUE;
  else if  (TRUE_FORMULA(g))
    return TRUE;
  else if (g->type == OR_FORM) {
    if (f->type == OR_FORM)
      return prop_subset_share(f, g);
    else
      return prop_member_share(f, g);
  }
  return formula_ident_share(f, g);
}  /* prop_subsume_share */

/*************
 *
 *   remove_subsumed_share()
 *
 *   Assume flat conjunction.  Always return conjunction.
 *
 *************/

static
Formula remove_subsumed_share(Formula f)
{
  if (f->type != AND_FORM)
    return f;
  else {
    Formula h;
    int new_arity = f->arity;
    int i, j;
    for (i = 0; i < f->arity; i++) {
      for (j = i+1; j < f->arity; j++) {
	if (f->kids[i] && f->kids[j] &&
	    prop_subsume_share(f->kids[i], f->kids[j])) {
	  zap_formula(f->kids[j]);
	  f->kids[j] = NULL;
	  new_arity--;
	}
	else if (f->kids[i] && f->kids[j] &&
		 prop_subsume_share(f->kids[j], f->kids[i])) {
	  zap_formula(f->kids[i]);
	  f->kids[i] = NULL;
	  new_arity--;
	}
      }
    }
    h = formula_get(new_arity, AND_FORM);
    j = 0;
    for (i = 0; i < f->arity; i++) {
      if (f->kids[i])
	h->kids[j++] = f->kids[i];
    }
    free_formula(f);
    return h;
  }
}  /* remove_subsumed_share */

/*************
 *
 *   bbt()
 *
 *************/

static
Formula bbt(Formula f, int start, int end)
{
  if (start == end)
    return f->kids[start];
  else {
    int mid = (start + end) / 2;
    Formula b = formula_get(2, f->type);
    b->kids[0] = bbt(f, start, mid);
    b->kids[1] = bbt(f, mid+1, end); 
    return b;
  }
}  /* bbt */

/*************
 *
 *   balanced_binary()
 *
 *   Take a flat OR or AND, and make it into a balanced binary tree.
 *
 *************/

static
Formula balanced_binary(Formula f)
{
  if (f->type != AND_FORM && f->type != OR_FORM)
    return f;
  else if (f->arity == 0)
    return f;
  else {
    Formula b = bbt(f, 0, f->arity-1);
    free_formula(f);
    return b;
  }
}  /* balanced_binary */

/*************
 *
 *   disjoin_flatten_simplify(a, b)   a OR b
 *
 *   Remove duplicates; if it contains complements, return TRUE.
 *
 *************/

static
Formula disjoin_flatten_simplify(Formula a, Formula b)
{
  Formula c;
  int new_arity, i, j;
  a = make_disjunction(a);
  b = make_disjunction(b);
  new_arity = a->arity + b->arity;
  for (i = 0; i < a->arity; i++) {
    for (j = 0; j < b->arity; j++) {
      if (b->kids[j] != NULL) {
	if (complementary_share(a->kids[i], b->kids[j])) {
	  zap_formula(a);
	  zap_formula(b);  /* this can handle NULL kids */
	  return formula_get(0, AND_FORM);  /* TRUE formula */
	}
	else if (formula_ident_share(a->kids[i], b->kids[j])) {
	  /* Note that this makes b non-well-formed. */
	  zap_formula(b->kids[j]);  /* really FALSE */
	  b->kids[j] = NULL;
	  new_arity--;
	}
      }
    }
  }
  c = formula_get(new_arity, OR_FORM);
  j = 0;
  for (i = 0; i < a->arity; i++)
    c->kids[j++] = a->kids[i];
  for (i = 0; i < b->arity; i++)
    if (b->kids[i] != NULL)
      c->kids[j++] = b->kids[i];
  free_formula(a);
  free_formula(b);
  return c;
}  /* disjoin_flatten_simplify */

/*************
 *
 *   simplify_and_share()
 *
 *   Assume flattened conjunction, and all kids are simplified flat
 *   disjunctions (or atomic, TRUE, FALSE).
 *   
 *
 *************/

static
Formula simplify_and_share(Formula f)
{
  if (f->type != AND_FORM)
    return f;
  else {
    f = remove_subsumed_share(f);  /* still AND */
    if (f->arity == 1) {
      Formula g = f->kids[0];
      free_formula(f);
      return g;
    }
    else if (contains_complements_share(f)) {
      zap_formula(f);
      return formula_get(0, OR_FORM);  /* FALSE */
    }
    else
      return f;
  }
}  /* simplify_and_share */

/*************
 *
 *   distribute_top()
 *
 *   Assume it's a binary disjunction.
 *
 *************/

static
Formula distribute_top(Formula h)
{
  Formula f = h->kids[0];
  Formula g = h->kids[1];
  int arity, i, j, k;
  Formula a;
  free_formula(h);
  /* If not conjunctions, make them so. */
  f = make_conjunction(f);
  g = make_conjunction(g);

  /* printf("DT: %5d x %5d\n", f->arity, g->arity); fflush(stdout); */

  arity = f->arity * g->arity;

  a = formula_get(arity, AND_FORM);
  k = 0;
  for (i = 0; i < f->arity; i++) {
    for (j = 0; j < g->arity; j++) {
      Formula fi = formula_copy_share(f->kids[i]);
      Formula gj = formula_copy_share(g->kids[j]);
      a->kids[k++] = disjoin_flatten_simplify(fi, gj);
    }
  }
  zap_formula(f);
  zap_formula(g);
  a = simplify_and_share(a);
  return a;
}  /* distribute_top */

/*************
 *
 *   distribute()
 *
 *************/

static
Formula distribute(Formula f)
{
  if (f->type != OR_FORM || f->arity == 0)
    return f;
  else {
    if (f->arity != 2)
      fatal_error("distribute: not binary");
    f->kids[0] = distribute(f->kids[0]);
    f->kids[1] = distribute(f->kids[1]);
    f = distribute_top(f);
    return f;
  }
}  /* distribute */

/*************
 *
 *   cnf()
 *
 *   Assume NNF and flattened.
 *
 *   This does not go below quantifiers; that is,
 *   quantified formulas are treated as atomic.
 *
 *************/

/* DOCUMENTATION
*/

/* PUBLIC */
Formula cnf(Formula f)
{
  if (f->type != AND_FORM && f->type != OR_FORM)
    return f;
  else {
    int i;
    for (i = 0; i < f->arity; i++)
      f->kids[i] = cnf(f->kids[i]);
    
    if (f->type == AND_FORM) {
      f = flatten_top(f);
      f = simplify_and_share(f);
      return f;
    }
    else {  /* OR_FORM */
      f = dual(remove_subsumed_share(dual(f)));
      f = balanced_binary(f);  /* make the top OR-tree binary */
      f = distribute(f);
      return f;
    }
  }
}  /* cnf */

/*************
 *
 *   dnf()
 *
 *************/

/* DOCUMENTATION
*/

/* PUBLIC */
Formula dnf(Formula f)
{
  return dual(cnf(dual(f)));
}  /* dnf */

/*************
 *
 *   skolem()
 *
 *************/

static
Formula skolem(Formula f, Ilist uvars)
{
  if (f->type == ATOM_FORM || f->type == NOT_FORM)
    return f;
  else if (f->type == ALL_FORM) {
    Term var = get_rigid_term(f->qvar, 0);
    Ilist uvars_plus;
    if (ilist_member(uvars, SYMNUM(var))) {
      /* We are in the scope of another variable with this name, so
       * rename this variable.
       */
      int sn = gen_new_symbol("x", 0, uvars);
      Term newvar = get_rigid_term(sn_to_str(sn), 0);
      subst_free_var(f->kids[0], var, newvar);
      f->qvar = sn_to_str(sn);
      free_term(var);
      var = newvar;
    }

    uvars_plus = ilist_prepend(uvars, SYMNUM(var));
    f->kids[0] = skolem(f->kids[0], uvars_plus);
    free_term(var);
    free_ilist(uvars_plus);  /* frees first node only; uvars still good */
    return f;
  }
  else if (f->type == EXISTS_FORM) {
    Formula g;
    int n = ilist_count(uvars);
    int sn = next_skolem_symbol(n);
    Term sk = get_rigid_term(sn_to_str(sn), n);
    Term evar = get_rigid_term(f->qvar, 0);
    Ilist p;
    int i;  /* uvars is backward */

    for (p = uvars, i = n-1; p; p = p->next, i--)
      ARG(sk,i) = get_rigid_term(sn_to_str(p->i), 0);
      
    subst_free_var(f->kids[0], evar, sk);

    zap_term(sk);
    zap_term(evar);

    g = skolem(f->kids[0], uvars);
    free_formula(f);
    return g;
  }
  else if (f->type == AND_FORM || f->type == OR_FORM) {
    int i;
    for (i = 0; i < f->arity; i++) {
      f->kids[i] = skolem(f->kids[i], uvars);
    }
    return f;
  }
  else {
    /* Not in NNF!  Let the caller beware! */
    return f;
  }
}  /* skolem */

/*************
 *
 *   skolemize()
 *
 *************/

/* DOCUMENTATION
This routine Skolemizes an NNF formula.
The quantified variables need not be named in any particular way.
If there are universally quantified variables with the same name,
one in the scope of another, the inner variable will be renamed.
(Existential nodes are removed.)
*/

/* PUBLIC */
Formula skolemize(Formula f)
{
  f = skolem(f, NULL);
  return f;
}  /* skolemize */

/*************
 *
 *   unique_qvars()
 *
 *************/

static
Ilist unique_qvars(Formula f, Ilist vars)
{
  if (f->type == ATOM_FORM)
    return vars;
  else if (quant_form(f)) {
    Term var = get_rigid_term(f->qvar, 0);
    if (ilist_member(vars, SYMNUM(var))) {
      /* Rename this variable. */
      int sn = gen_new_symbol("x", 0, vars);
      Term newvar = get_rigid_term(sn_to_str(sn), 0);
      subst_free_var(f->kids[0], var, newvar);
      f->qvar = sn_to_str(sn);
      free_term(var);
      var = newvar;
    }
    vars = ilist_prepend(vars, SYMNUM(var));
    return unique_qvars(f->kids[0], vars);
  }
  else {
    int i;
    for (i = 0; i < f->arity; i++)
      vars = unique_qvars(f->kids[i], vars);
    return vars;
  }
}  /* unique_qvars */

/*************
 *
 *   unique_quantified_vars()
 *
 *************/

/* DOCUMENTATION
Rename quantified variables, if necessary, so that each is unique.
This works for any formula.
<P>
If you wish to rename a quantified variable only if it occurs in
the scope of of a quantified variable with the same name, you
can use the routine eliminate_rebinding() instead.
<P>
(This could be a void routine, because none of the formula nodes
is changed.)
*/

/* PUBLIC */
Formula unique_quantified_vars(Formula f)
{
  Ilist uvars = unique_qvars(f, NULL);
  zap_ilist(uvars);
  return f;
}  /* unique_quantified_vars */

/*************
 *
 *   mark_free_vars_formula()
 *
 *************/

/* Replace all free occurrences of CONSTANT *varname with
 * a VARIABLE of index varnum.
 */

static
void mark_free_vars_formula(Formula f, char *varname, int varnum)
{
  if (f->type == ATOM_FORM)
    f->atom = subst_var_term(f->atom, str_to_sn(varname, 0), varnum);
  else if (quant_form(f) && str_ident(f->qvar, varname))
    return;
  else {
    int i;
    for (i = 0; i < f->arity; i++)
      mark_free_vars_formula(f->kids[i], varname, varnum);
  }
}  /* mark_free_vars_formula */

/*************
 *
 *   remove_uni_quant()
 *
 *************/

static
Formula remove_uni_quant(Formula f, int *varnum_ptr)
{
  if (f->type == AND_FORM || f->type == OR_FORM) {
    int i;
    for (i = 0; i < f->arity; i++)
      f->kids[i] = remove_uni_quant(f->kids[i], varnum_ptr);
    return f;
  }
  else if (f->type == ALL_FORM) {
    Formula g = f->kids[0];
    mark_free_vars_formula(g, f->qvar, *varnum_ptr);
    *varnum_ptr += 1;
    free_formula(f);
    return remove_uni_quant(g, varnum_ptr);
  }
  else {
    /* If not ATOM_FORM, something's probably wrong,
     * but let the caller beware!
     */
    return f;
  }
}  /* remove_uni_quant */

/*************
 *
 *   remove_universal_quantifiers()
 *
 *************/

/* DOCUMENTATION
For each universally quantified variable in the given formula,
*/

/* PUBLIC */
Formula remove_universal_quantifiers(Formula f)
{
  int varnum = 0;
  return remove_uni_quant(f, &varnum);
}  /* remove_universal_quantifiers */

/*************
 *
 *   clausify_prepare()
 *
 *************/

/* DOCUMENTATION
This routine gets a formula all ready for translation into clauses.
The sequence of transformations is
<UL>
<LI> change to negation normal form;
<LI> propositional simplification;
<LI> make the universally quantified variables unique;
<LI> skolemize (nothing fancy here);
<LI> remove universal quantifiers, changing the
     constants-which-represent-variables into genuine variables;
<LI> change to conjunctive normal form
     (with basic propositional simplification).
</UL>
The caller should not refer to the given formula f after the call;
A good way to call is <TT>f = clausify_prepare(f)</TT>
*/

/* PUBLIC */
Formula clausify_prepare(Formula f)
{
  formula_canon_eq(f);
  f = nnf(f);
  f = unique_quantified_vars(f);
  f = skolemize(f);
  f = remove_universal_quantifiers(f);
  f = formula_flatten(f);

  f = consolidate_formula(f);  /* causes sharing of some subformulas */
#if 0
  printf("%% CNF translation, nnf_size=%d, ", formula_size(f));
  fflush(stdout);
#endif
  f = cnf(f);
#if 0
  printf("cnf_size=%d, cnf_clauses=%d\n",
	 formula_size(f), f->type == AND_FORM ? f->arity : 1);
  fflush(stdout);
#endif   
  return f;
}  /* clausify_prepare */

/*************
 *
 *   ms_free_vars()
 *
 *************/

static
Formula ms_free_vars(Formula f)
{
  /* f is ALL_FORM, kids are rms, kids not AND_FORM */
  Formula child = f->kids[0];
  if (child->type != OR_FORM) {
    if (free_variable(f->qvar, child))
      return f;
    else {
      free_formula(f);
      return child;
    }
  }
  else {
    Plist free = NULL;        /* children with qvar free */
    Plist notfree = NULL;     /* children without qvar free */
    int free_count = 0;       /* size of free */
    int notfree_count = 0;    /* size of notfree */
    int i;
    for (i = child->arity-1; i >= 0; i--) {
      if (!free_variable(f->qvar, child->kids[i])) {
	notfree = plist_prepend(notfree, child->kids[i]);
	notfree_count++;
      }
      else {
	free = plist_prepend(free, child->kids[i]);
	free_count++;
      }
    }
    if (notfree_count == 0)
      return f;      /* all children have qvar free */
    else if (free_count == 0) {
      free_formula(f);
      return child;  /* no child has qvar free */
    }
    else {
      Formula or_free = formula_get(free_count , OR_FORM);
      Formula or_top = formula_get(notfree_count + 1 , OR_FORM);
      Plist p;
      for (p = free, i = 0; p; p = p->next, i++)
	or_free->kids[i] = p->v;
      for (p = notfree, i = 0; p; p = p->next, i++)
	or_top->kids[i] = p->v;
      or_top->kids[i] = f;
      f->kids[0] = or_free;
      free_formula(child);
      return or_top;
    }
  }
}  /* ms_free_vars */

/*************
 *
 *   miniscope()
 *
 *************/

/* DOCUMENTATION
*/

/* PUBLIC */
Formula miniscope(Formula f)
{
  if (f->type == ATOM_FORM || f->type == NOT_FORM)
    return f;
  if (f->type == AND_FORM) {
    int i;
    for (i = 0; i < f->arity; i++)
      f->kids[i] = miniscope(f->kids[i]);
    f = flatten_top(f);
    f = simplify_and_share(f);
    return f;
  }
  else if (f->type == OR_FORM) {
    int i;
    for (i = 0; i < f->arity; i++)
      f->kids[i] = miniscope(f->kids[i]);
    f = flatten_top(f);
    f = dual(remove_subsumed_share(dual(f)));
    f = balanced_binary(f);  /* make the top OR-tree binary */
    f = distribute(f);
    return f;
  }
  else if (f->type == EXISTS_FORM) {
    f = dual(f);
    f = miniscope(f);
    f = dual(f);
    return f;
  }
  else if (f->type == ALL_FORM) {

    f->kids[0] = miniscope(f->kids[0]);
    
    if (f->kids[0]->type == AND_FORM) {
      /* distribute all to children */
      int i;
      Formula and = f->kids[0];
      free_formula(f);  /* shallow */
      for (i = 0; i < and->arity; i++) {
	Formula g = get_quant_form(ALL_FORM, f->qvar, and->kids[i]);
	g = ms_free_vars(g);
	and->kids[i] = g;
      }
      return and;  /* need to simplify first? */
    }
    else {
      f = ms_free_vars(f);
      return f;
    }
  }  /* ALL */
  else {
    fatal_error("miniscope: formula not in nnf");
    return NULL;  /* to please the complier */
  }
}  /* miniscope */

/*************
 *
 *   miniscope_formula()
 *
 *************/

typedef void (*sighandler_t)(int);

/* DOCUMENTATION
*/

/* PUBLIC */
Formula miniscope_formula(Formula f, unsigned mega_fid_call_limit)
{
  int return_code;
  if (mega_fid_call_limit <= 0)
    Fid_call_limit = 0;  /* no limit */
  else {
    Fid_call_limit = mega_fid_call_limit * 1000000;
    Fid_calls = 0;
  }

  return_code = setjmp(Jump_env);
  if (return_code != 0) {
    /* We just landed from longjmp(), because the limit was exceeded.
       (I'd like to reclaim the formula memory, but that would take some
       thought, because the partly transformed formula is not well formed.)
    */
    Fid_call_limit = 0;  /* no limit */
    return NULL;
  }
  else {
    /* ordinary execution */
    Formula f2 = NULL;

    formula_canon_eq(f);  /* canonicalize (naively) eqs for maximum sharing */
    f = nnf(f);
    f = formula_flatten(f);
    f = consolidate_formula(f);  /* share some subformulas */
  
    f = miniscope(f);  /* do the work */

    /* return a formula without shared subformulas */

    f2 = formula_copy(f);
    zap_formula(f);

    Fid_call_limit = 0;  /* no limit */
    return f2;
  }
}  /* miniscope_formula */

/*************
 *
 *   cnf_max_clauses()
 *
 *************/

/* DOCUMENTATION
Given an NNF formula, return the maximum number of clauses that
it can produce.  (The maximum happens if no simplification occurs.)
*/

/* PUBLIC */
int cnf_max_clauses(Formula f)
{
  if (f->type == ATOM_FORM || f->type == NOT_FORM)
    return 1;
  else if (f->type == ALL_FORM || f->type == EXISTS_FORM)
    return cnf_max_clauses(f->kids[0]);
  else if (f->type == AND_FORM) {
    int i;
    int n = 0;
    for (i = 0; i < f->arity; i++)
      n += cnf_max_clauses(f->kids[i]);
    return n;
  }
  else if (f->type == OR_FORM) {
    int i;
    int n = 1;
    for (i = 0; i < f->arity; i++)
      n *= cnf_max_clauses(f->kids[i]);
    return n;
  }
  else {
    fatal_error("cnf_max_clauses, formula not NNF");
    return -1;  /* won't happen */
  }
}  /* cnf_max_clauses */

