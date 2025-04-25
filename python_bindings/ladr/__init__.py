"""
Python bindings for LADR term module with a SymPy-like interface.
"""

# Import the C++ extension module first
from . import term as _term_cpp

# Import the Python wrapper class and factory functions
from .term_wrapper import Term, variables, constants, unary, binary, nary

__all__ = [
    'Term',
    'variables',
    'constants',
    'unary',
    'binary',
    'nary',
    '_term_cpp' # Expose the underlying C++ module if needed
] 