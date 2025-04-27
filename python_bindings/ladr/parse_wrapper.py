"""
Python wrapper for LADR parse module.
"""
import re
from . import parse as _parse_cpp
from .term_wrapper import Term
from .error_wrapper import LadrFatalError, catch_fatal_errors

def init_parser():
    """
    Initialize or reset the LADR parser.
    
    This initializes the parser with standard operator precedence and syntax.
    It should be called before parsing any terms if the parser has not been
    initialized already, but the parser is initialized automatically when
    the module is imported.
    """
    #_parse_cpp.init_parser()
    pass

@catch_fatal_errors
def parse_term(text):
    """
    Parse a term from a string.
    
    This function takes a text string (which could be a whole Prover9/LADR file),
    and returns a Term object. It uses the LADR parse_term_from_string function
    to do the parsing.
    
    Args:
        text (str): The text to parse, typically a Prover9/LADR file or formula.
    
    Returns:
        Term: A Term object representing the parsed term.
    
    Raises:
        TypeError: If the input is not a string.
        ValueError: If the string cannot be parsed into a term.
        LadrFatalError: If a LADR fatal error occurs during parsing.
    """
    if not isinstance(text, str):
        raise TypeError("text must be a string")
    # remove comments after %
    text = re.sub(r'%.*', '', text)
    # Call the C++ parse_term_from_string function
    try:
        cpp_term = _parse_cpp.parse_term_from_string(text)
        # Wrap it in a Term object
        return Term(cpp_term)
    except ValueError as e:
        # Re-raise with a more descriptive message
        raise ValueError(f"Failed to parse term: {e}") from e 