import unittest
import ladr.term
import ladr.formula
import debug

class TestFormula(unittest.TestCase):
    #@debug.traced
    def test_formula_types(self):
        """Test the formula module types"""
        self.assertEqual(ladr.formula.Ftype.ATOM_FORM, 0)
        self.assertEqual(ladr.formula.Ftype.AND_FORM, 1)
        self.assertEqual(ladr.formula.Ftype.OR_FORM, 2)
        self.assertEqual(ladr.formula.Ftype.NOT_FORM, 3)
        self.assertEqual(ladr.formula.Ftype.IFF_FORM, 4)
        self.assertEqual(ladr.formula.Ftype.IMP_FORM, 5)
        self.assertEqual(ladr.formula.Ftype.IMPBY_FORM, 6)
        self.assertEqual(ladr.formula.Ftype.ALL_FORM, 7)
        self.assertEqual(ladr.formula.Ftype.EXISTS_FORM, 8)
    
    #@debug.traced
    def test_atom_formula(self):
        """Test atomic formula creation and properties"""
        # Create a term to use as an atom
        term = ladr.term.term0("P")
        
        # Convert term to atomic formula
        atom_formula = ladr.formula.term_to_formula(term)
        
        # Check properties
        self.assertEqual(atom_formula.type, ladr.formula.Ftype.ATOM_FORM)
        self.assertEqual(atom_formula.arity, 0)
        self.assertTrue(atom_formula.is_atom)
        self.assertFalse(atom_formula.is_quantified)
        self.assertEqual(str(atom_formula.atom), "P")
        self.assertEqual(len(atom_formula), 0)
        
        # Test with term containing arguments
        term2 = ladr.term.term2("Q", ladr.term.get_variable_term(1), ladr.term.term0("a"))
        atom_formula2 = ladr.formula.term_to_formula(term2)
        self.assertEqual(atom_formula2.type, ladr.formula.Ftype.ATOM_FORM)
        self.assertEqual(str(atom_formula2.atom), "Q(v1,a)")
        
        # Test error on accessing qvar for atomic formula
        with self.assertRaises(ValueError):
            _ = atom_formula.qvar
    
    #@debug.traced
    def test_compound_formula(self):
        """Test compound formula creation and properties"""
        # Create atomic formulas
        term_p = ladr.term.term0("P")
        term_q = ladr.term.term0("Q")
        atom_p = ladr.formula.term_to_formula(term_p)
        atom_q = ladr.formula.term_to_formula(term_q)
        
        # Create conjunction (AND)
        conj = ladr.formula.and_form(atom_p, atom_q)
        self.assertEqual(conj.type, ladr.formula.Ftype.AND_FORM)
        self.assertEqual(conj.arity, 2)
        self.assertFalse(conj.is_atom)
        self.assertEqual(len(conj), 2)
        self.assertEqual(str(conj[0]), "P")
        self.assertEqual(str(conj[1]), "Q")
        
        # Create disjunction (OR)
        disj = ladr.formula.or_form(atom_p, atom_q)
        self.assertEqual(disj.type, ladr.formula.Ftype.OR_FORM)
        self.assertEqual(disj.arity, 2)
        
        # Create implication (IMP)
        impl = ladr.formula.imp_form(atom_p, atom_q)
        self.assertEqual(impl.type, ladr.formula.Ftype.IMP_FORM)
        self.assertEqual(impl.arity, 2)
        
        # Create negation (NOT)
        neg = ladr.formula.negate(atom_p)
        self.assertEqual(neg.type, ladr.formula.Ftype.NOT_FORM)
        self.assertEqual(neg.arity, 1)
        self.assertEqual(str(neg[0]), "P")
        
        # Test error on accessing atom for non-atomic formula
        with self.assertRaises(ValueError):
            _ = conj.atom
    
    #@debug.traced
    def test_quantified_formula(self):
        """Test quantified formula creation and properties"""
        # Create a variable term
        var_term = ladr.term.get_variable_term(1)
        
        # Create an atomic formula with that variable
        pred_term = ladr.term.term1("P", var_term)
        atom = ladr.formula.term_to_formula(pred_term)
        
        # Create a universal closure of the atom
        univ = ladr.formula.universal_closure(atom)
        
        # Check properties
        self.assertEqual(univ.type, ladr.formula.Ftype.ALL_FORM)
        self.assertEqual(univ.arity, 1)
        self.assertTrue(univ.is_quantified)
        self.assertEqual(univ.qvar, "v1")
        
        # Check the subformula
        subformula = univ[0]
        self.assertEqual(subformula.type, ladr.formula.Ftype.ATOM_FORM)
        self.assertEqual(str(subformula.atom), "P(v1)")
    
    #@debug.traced
    def test_formula_operations(self):
        """Test formula operations"""
        # Create atomic formulas
        term_p = ladr.term.term0("P")
        term_q = ladr.term.term0("Q")
        atom_p = ladr.formula.term_to_formula(term_p)
        atom_q = ladr.formula.term_to_formula(term_q)
        
        # Test formula copy
        p_copy = ladr.formula.formula_copy(atom_p)
        self.assertTrue(ladr.formula.formula_ident(atom_p, p_copy))
        
        # Test formula conversion to term and back
        p_term = ladr.formula.formula_to_term(atom_p)
        p_formula = ladr.formula.term_to_formula(p_term)
        self.assertTrue(ladr.formula.formula_ident(atom_p, p_formula))
        
        # Test formula size
        conj = ladr.formula.and_form(atom_p, atom_q)
        self.assertEqual(ladr.formula.formula_size(conj), 3)  # 1 AND node + 2 atom nodes
        
        # Test formula literal and clausal checks
        self.assertTrue(ladr.formula.literal_formula(atom_p))
        self.assertTrue(ladr.formula.clausal_formula(atom_p))
        self.assertFalse(ladr.formula.literal_formula(conj))  # Conjunction is not a literal
        self.assertFalse(ladr.formula.clausal_formula(conj))  # Simple conjunction is not clausal
        
        # Negation is a literal
        neg_p = ladr.formula.negate(atom_p)
        self.assertTrue(ladr.formula.literal_formula(neg_p))
        
        # Disjunction of literals is clausal
        disj = ladr.formula.or_form(atom_p, atom_q)
        self.assertTrue(ladr.formula.clausal_formula(disj))
    
    #@debug.traced
    def test_true_false_formulas(self):
        """Test TRUE and FALSE formula creation and check"""
        # Create TRUE formula (empty conjunction)
        true_formula = ladr.formula.get_true_formula()
        self.assertEqual(true_formula.type, ladr.formula.Ftype.AND_FORM)
        self.assertEqual(true_formula.arity, 0)
        self.assertTrue(ladr.formula.true_formula(true_formula))
        
        # Create FALSE formula (empty disjunction)
        false_formula = ladr.formula.get_false_formula()
        self.assertEqual(false_formula.type, ladr.formula.Ftype.OR_FORM)
        self.assertEqual(false_formula.arity, 0)
        self.assertTrue(ladr.formula.false_formula(false_formula))
    
    #@debug.traced
    def test_formula_transformations(self):
        """Test formula transformations like NNF and flattening"""
        # Create atomic formulas
        term_p = ladr.term.term0("P")
        term_q = ladr.term.term0("Q")
        term_r = ladr.term.term0("R")
        atom_p = ladr.formula.term_to_formula(term_p)
        atom_q = ladr.formula.term_to_formula(term_q)
        atom_r = ladr.formula.term_to_formula(term_r)
        
        # Create a complex formula to transform: ~((P & Q) & R)
        conj1 = ladr.formula.and_form(atom_p, atom_q)
        conj2 = ladr.formula.and_form(conj1, atom_r)
        formula = ladr.formula.negate(conj2)
        
        # Convert to NNF
        nnf_formula = ladr.formula.nnf(formula)
        
        # Should be ~P | ~Q | ~R in NNF
        self.assertEqual(nnf_formula.type, ladr.formula.Ftype.OR_FORM)
        
        # Test flattening of nested AND formulas
        conj1 = ladr.formula.and_form(atom_p, atom_q)
        conj2 = ladr.formula.and_form(conj1, atom_r)
        
        flat_formula = ladr.formula.formula_flatten(conj2)
        self.assertEqual(flat_formula.type, ladr.formula.Ftype.AND_FORM)
        self.assertEqual(flat_formula.arity, 3)

if __name__ == '__main__':
    debug.reexecute_if_unbuffered()
    unittest.main() 