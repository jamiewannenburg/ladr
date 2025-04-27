"""
Python bindings for LADR term module with a SymPy-like interface.
"""

# Import the combined C++ extension module
from . import ladr_bindings

# Import the individual C++ extension modules that now use the combined module
# from . import term as _term_cpp
# from . import parse as _parse_cpp
# from . import memory as _memory_cpp  # This initializes the LADR memory system on import
# from . import error_handler as _error_cpp
from .ladr_bindings import term as _term_cpp
from .ladr_bindings import parse as _parse_cpp
from .ladr_bindings import memory as _memory_cpp
#from .ladr_bindings import error_handler as _error_cpp

from .ladr_bindings import term
from .ladr_bindings import parse
from .ladr_bindings import memory
#from .ladr_bindings import error_handler
from .ladr_bindings import LadrFatalError

# Import the Python wrapper class and factory functions
from .term_wrapper import Term, variables, constants, unary, binary, nary
from .parse_wrapper import parse_term, init_parser
from .memory_wrapper import init_memory, set_memory_limit, get_memory_usage

# Initialize LADR memory system when the package is imported
# This happens automatically when importing _memory_cpp, but we can also call it explicitly
init_memory()

# Set default memory limit to unlimited
set_memory_limit(0)

# Initialize the parser - this happens automatically when importing _parse_cpp
# but we can also call it explicitly to ensure it's done
init_parser() # TODO: Add package options for this

__all__ = [
    'Term',
    'variables',
    'constants',
    'unary',
    'binary',
    'nary',
    'parse_term',
    'init_parser',
    'init_memory',
    'set_memory_limit',
    'get_memory_usage',
    'LadrFatalError',
    '_term_cpp',
    '_parse_cpp',
    '_memory_cpp',
    #'_error_cpp',
    'ladr_bindings',
    'term',
    'parse',
    'memory',
    #'error_handler'
] 