import weakref
from . import term as _term_cpp

# --- Caching --- 
# Use weak references to allow garbage collection when Python objects are no longer used
_variable_cache = weakref.WeakValueDictionary()
_constant_cache = weakref.WeakValueDictionary()
_function_cache = weakref.WeakValueDictionary() 


class Term:
    """Python wrapper for the LADR Term C++ object, providing a SymPy-like interface."""

    # Use __slots__ for memory efficiency, especially since many Term objects might be created
    __slots__ = ('_term', '__weakref__') 

    def __init__(self, _cpp_term):
        """Initialize a Term wrapper. Use factory functions (variables, constants, etc.) instead."""
        if not isinstance(_cpp_term, _term_cpp.Term):
            raise TypeError("Internal error: _cpp_term must be a ladr._term_cpp.Term instance")
        # Store the C++ term object provided by the binding
        self._term = _cpp_term

    # --- Properties --- 
    @property
    def is_variable(self) -> bool:
        return self._term.is_variable

    @property
    def is_constant(self) -> bool:
        return self._term.is_constant

    @property
    def is_complex(self) -> bool:
        return self._term.is_complex

    @property
    def symbol(self) -> str:
        # The C++ binding returns "vN" for variables, we want just the name from the cache if possible.
        # For constants/functions, it returns the correct symbol.
        if self.is_variable:
            # Attempt to find the original name from the cache key (might not always work if created differently)
            # This relies on the variable being created through the `variables` factory.
            var_num = self._term.varnum # Get varnum from C++ object
            for name, term_wrapper in _variable_cache.items():
                 # Compare varnum instead of relying on object identity after potential copy/recreation
                 if term_wrapper._term.varnum == var_num: 
                     return name
            # Fallback if not found in cache (e.g., term created directly via C++ functions)
            return f"v{var_num}" 
        else:
            return self._term.symbol

    @property
    def arity(self) -> int:
        return self._term.arity

    @property
    def args(self) -> tuple['Term', ...]:
        # Wrap the C++ argument terms back into Python Term objects
        # Note: The C++ binding __getitem__ returns *copies* of the args.
        # We create new Python wrappers around these copies.
        return tuple(Term(self._term[i]) for i in range(self.arity))

    # --- Operator Overloading --- 
    def __add__(self, other): 
        if not isinstance(other, Term):
            return NotImplemented
        cpp_term = _term_cpp.term2('+', self._term, other._term)
        return Term(cpp_term)
    
    def __radd__(self, other):
         # Handles cases like 'a' + Term (if 'a' were somehow adaptable)
         # For Term + Term, __add__ is sufficient.
         return NotImplemented # Keep it simple for now

    def __sub__(self, other):
        if not isinstance(other, Term):
            return NotImplemented
        cpp_term = _term_cpp.term2('-', self._term, other._term)
        return Term(cpp_term)
    
    def __rsub__(self, other):
         # Handles cases like 'a' - Term
         return NotImplemented 

    def __mul__(self, other):
        if not isinstance(other, Term):
            return NotImplemented
        cpp_term = _term_cpp.term2('*', self._term, other._term)
        return Term(cpp_term)

    def __rmul__(self, other):
         return NotImplemented

    def __truediv__(self, other):
        if not isinstance(other, Term):
            return NotImplemented
        cpp_term = _term_cpp.term2('/', self._term, other._term)
        return Term(cpp_term)

    def __rtruediv__(self, other):
        return NotImplemented

    def __neg__(self):
        cpp_term = _term_cpp.term1('-', self._term)
        return Term(cpp_term)

    def __and__(self, other):
        if not isinstance(other, Term):
            return NotImplemented
        cpp_term = _term_cpp.term2('&', self._term, other._term)
        return Term(cpp_term)

    def __rand__(self, other):
        return NotImplemented

    def __or__(self, other):
        if not isinstance(other, Term):
            return NotImplemented
        # The C++ binding might use 'bitor' or similar if '|' isn't directly mapped.
        # Assuming '|' is a valid symbol name recognized by str_to_sn.
        cpp_term = _term_cpp.term2('|', self._term, other._term)
        return Term(cpp_term)

    def __ror__(self, other):
        return NotImplemented

    def __xor__(self, other):
        if not isinstance(other, Term):
            return NotImplemented
        # Assuming '^' is a valid symbol name.
        cpp_term = _term_cpp.term2('^', self._term, other._term)
        return Term(cpp_term)

    def __rxor__(self, other):
        return NotImplemented

    # --- Function Application --- 
    def __call__(self, *args):
        """Apply this term (if complex) as a function to arguments."""
        if not self.is_complex:
            raise TypeError(f"Term '{self}' is not callable (not complex)")
        
        if len(args) != self.arity:
             raise TypeError(f"Function '{self.symbol}' expected {self.arity} arguments, got {len(args)}")

        # Extract the C++ term objects from the Python wrappers
        cpp_args = [arg._term for arg in args if isinstance(arg, Term)]
        if len(cpp_args) != len(args):
            raise TypeError("All arguments must be Term instances")
            
        # We need a general way to build a term with a given symbol number and arguments.
        # The C++ bindings expose `build_binary_term`, `build_unary_term`, `term0`, `term1`, `term2`.
        # We need a more general approach if arity > 2 or if we don't want to rely on symbol strings.
        # Let's try recreating using symbol name and arity, then applying args.
        # This assumes the underlying C++ term construction handles symbol registration.
        
        # Get the symbol number from the underlying C++ object
        sym_num = self._term.symnum 
        
        # Build the new term. We need a C++ function like build_term(sym_num, cpp_args_list)
        # Since that's not directly exposed, we might need to modify the C++ bindings.
        # For now, let's handle common cases using existing bindings if possible.
        
        # *** Temporary Workaround/Limitation ***
        # Use string-based builders, which might be less efficient or robust
        if self.arity == 0:
             cpp_term = _term_cpp.term0(self.symbol)
        elif self.arity == 1:
            cpp_term = _term_cpp.term1(self.symbol, cpp_args[0])
        elif self.arity == 2:
             cpp_term = _term_cpp.term2(self.symbol, cpp_args[0], cpp_args[1])
        else:
             # Use the general build_term function for n-ary terms
             cpp_term = _term_cpp.build_term(self.symbol, cpp_args)
             # Alternative (less safe): Reconstruct args list in C++? Difficult with ownership.

        return Term(cpp_term)


    # --- String Representations --- 
    def __str__(self):
        # Delegate to the C++ __str__ which uses term_to_string
        return str(self._term)

    def __repr__(self):
        # Attempt to create a repr that could reconstruct the object, using the factory functions
        if self.is_variable:
            return f"variables('{self.symbol}')"
        elif self.is_constant:
            return f"constants('{self.symbol}')"
        elif self.is_complex:
            # Handle operators defined via overloading
            op_map = {
                '+': '__add__', '-': '__sub__', '*': '__mul__', '/': '__truediv__',
                '&': '__and__', '|': '__or__', '^': '__xor__' 
            }
            if self.symbol in op_map and self.arity == 2:
                 # Need careful parenthesization for correctness
                 # Basic version: Assuming left-associativity for simplicity here.
                 # A robust solution needs precedence rules.
                 return f"({repr(self.args[0])} {self.symbol} {repr(self.args[1])})"
            elif self.symbol == '-' and self.arity == 1:
                 return f"(-{repr(self.args[0])})"
            else:
                 # General function application form
                 args_repr = ", ".join(repr(arg) for arg in self.args)
                 # How was the function symbol created? Assume a factory function.
                 # We don't know if it was unary, binary, or nary from the term itself.
                 # Default to a generic function representation.
                 return f"{self.symbol}({args_repr})" # Simplified, doesn't show factory
        else:
            # Should not happen
            return f"Term(<unknown_structure>)" 

    # --- Comparison and Hashing --- 
    def __eq__(self, other):
        if not isinstance(other, Term):
            return NotImplemented
        # Use the C++ term_ident for structural equality
        return _term_cpp.term_ident(self._term, other._term)

    def __hash__(self):
        # We need a stable hash. Relying on the C++ object's identity might not be 
        # stable if copies are made. Hashing the string representation is a common 
        # (though potentially inefficient) workaround if a proper hash function isn't exposed.
        # A better approach would be to expose term_hash from C++.
        # For now, use the string representation.
        return hash(str(self)) 

# --- Factory Functions --- 

# Keep track of assigned variable numbers
_next_varnum = 0 

def variables(names: str) -> Term | tuple[Term, ...]:
    """Create one or more variable Terms."""
    global _next_varnum
    if not isinstance(names, str):
        raise TypeError("names must be a string")
    
    var_list = []
    for name in names.split():
        if not name.isidentifier():
             raise ValueError(f"'{name}' is not a valid variable name")
        
        cached_term = _variable_cache.get(name)
        if cached_term is not None:
            var_list.append(cached_term)
        else:
            if _next_varnum > _term_cpp.MAX_VAR:
                 raise RuntimeError(f"Maximum number of variables ({_term_cpp.MAX_VAR}) exceeded")
            # Create C++ variable term
            cpp_term = _term_cpp.get_variable_term(_next_varnum)
            # Create Python wrapper
            term_wrapper = Term(cpp_term)
            # Cache it
            _variable_cache[name] = term_wrapper
            var_list.append(term_wrapper)
            _next_varnum += 1
            
    if len(var_list) == 1:
        return var_list[0]
    else:
        return tuple(var_list)

def constants(names: str) -> Term | tuple[Term, ...]:
    """Create one or more constant Terms (arity 0 functions)."""
    if not isinstance(names, str):
        raise TypeError("names must be a string")
        
    const_list = []
    for name in names.split():
        # Allow more flexible constant names than Python identifiers if needed
        if not name: continue # Skip empty names
        
        cached_term = _constant_cache.get(name)
        if cached_term is not None:
            const_list.append(cached_term)
        else:
            # Create C++ constant term (arity 0)
            # Use term0 for simplicity, assumes symbol registration happens correctly
            cpp_term = _term_cpp.term0(name) 
            # Create Python wrapper
            term_wrapper = Term(cpp_term)
            # Cache it
            _constant_cache[name] = term_wrapper
            const_list.append(term_wrapper)
            
    if len(const_list) == 1:
        return const_list[0]
    else:
        return tuple(const_list)


def _create_function_symbol(name: str, arity: int) -> Term:
    """Internal helper to create and cache function symbols (complex terms with no args)."""
    if not isinstance(name, str) or not name:
        raise ValueError("Function name must be a non-empty string")
    if not isinstance(arity, int) or arity < 0:
         raise ValueError("Arity must be a non-negative integer")
    if arity > _term_cpp.MAX_ARITY:
        raise ValueError(f"Arity {arity} exceeds maximum allowed ({_term_cpp.MAX_ARITY})")

    cache_key = (name, arity)
    cached_term = _function_cache.get(cache_key)
    if cached_term is not None:
        return cached_term
    else:
        # Create a C++ term representing the function symbol itself (without args)
        # Use get_rigid_term which is designed for this
        cpp_term = _term_cpp.get_rigid_term(name, arity)
        term_wrapper = Term(cpp_term)
        _function_cache[cache_key] = term_wrapper
        return term_wrapper

def unary(name: str) -> Term:
    """Create a unary function symbol."""
    return _create_function_symbol(name, 1)

def binary(name: str) -> Term:
    """Create a binary function symbol."""
    return _create_function_symbol(name, 2)

def nary(name: str, arity: int) -> Term:
    """Create a function symbol with the specified arity."""
    return _create_function_symbol(name, arity)


# --- Potential Future Enhancements ---
# - Implement __radd__, __rsub__, etc. if needed.
# - More robust __repr__ with correct parenthesization based on precedence.
# - Implement __call__ for arity > 2 (requires C++ binding changes).
# - Expose term_hash from C++ for efficient hashing.
# - Support for substitutions.
# - More specific error handling. 