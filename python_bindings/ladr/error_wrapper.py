"""
Python wrapper for LADR error handling.
"""

import functools
from . import error_handler as _error_cpp

# Re-export the LadrFatalError exception
LadrFatalError = _error_cpp.LadrFatalError

# By default, suppress error messages in Python to avoid duplicate error info
_error_cpp.suppress_error_messages(1)

def catch_fatal_errors(func):
    """
    Decorator that catches LADR fatal errors and converts them to Python exceptions.
    
    This decorator wraps a function to catch any LADR fatal errors that would
    normally cause the program to terminate. Instead, it raises a LadrFatalError
    exception that can be caught and handled in Python.
    
    Args:
        func: The function to wrap.
        
    Returns:
        The wrapped function.
    
    Raises:
        LadrFatalError: If a LADR fatal error occurs.
    """
    return _error_cpp.catch_fatal_errors(func)

def check_for_errors():
    """
    Check if a LADR fatal error has occurred and raise an exception if so.
    
    Raises:
        LadrFatalError: If a LADR fatal error has occurred.
    """
    _error_cpp.check_for_errors()

def reset_error_flags():
    """
    Reset the LADR error flags.
    
    This is useful if you want to clear error state before performing operations
    that might cause errors.
    """
    _error_cpp.reset_error_flags()

def suppress_error_messages(suppress=True):
    """
    Control whether LADR fatal error messages are printed to stdout/stderr.
    
    Args:
        suppress (bool): If True, error messages will not be printed.
                        If False, error messages will be printed.
    """
    _error_cpp.suppress_error_messages(1 if suppress else 0) 