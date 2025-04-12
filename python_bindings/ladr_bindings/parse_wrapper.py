"""
Python wrapper for the LADR parse module.
This module provides utilities to parse and manipulate terms in the LADR system.
"""

from ladr import parse as _parse
from ladr import term

class Parser:
    """A wrapper class for the parse module to provide a more Pythonic interface."""
    
    def __init__(self):
        """Initialize the parser with standard parse types."""
        _parse.declare_standard_parse_types()
    
    def parse(self, text):
        """Parse a string into a Term object."""
        return _parse.parse_term_from_string(text)
    
    def to_string(self, term_obj):
        """Convert a Term object to a string."""
        return _parse.write_term_to_string(term_obj)
    
    def set_parenthesize(self, enabled=True):
        """Set whether to add parentheses in all terms for output."""
        _parse.parenthesize(enabled)
    
    def set_simple_parse(self, enabled=True):
        """Set whether to use simple parsing (no operator precedence)."""
        _parse.simple_parse(enabled)
    
    def set_check_illegal_symbols(self, enabled=True):
        """Set whether to check for illegal symbols during parsing."""
        _parse.check_for_illegal_symbols(enabled)
    
    def set_translate_neg_equalities(self, enabled=True):
        """Set whether to translate negative equalities to ~(a=b) form."""
        _parse.translate_neg_equalities(enabled)
    
    def set_cons_char(self, char):
        """Set the list constructor character."""
        _parse.set_cons_char(char)
    
    def set_quote_char(self, char):
        """Set the quote character."""
        _parse.set_quote_char(char)
    
    def get_cons_char(self):
        """Get the list constructor character."""
        return _parse.get_cons_char()
    
    def get_quote_char(self):
        """Get the quote character."""
        return _parse.get_quote_char()
    
    def declare_operator(self, symbol, precedence, parse_type):
        """
        Declare a parsing type for a symbol.
        
        Args:
            symbol: The symbol to declare.
            precedence: The precedence of the symbol (higher number means higher precedence).
            parse_type: The parse type from the ParseType enum.
        """
        _parse.declare_parse_type(symbol, precedence, parse_type)
    
    def process_quoted_symbol(self, text):
        """Process a quoted symbol, handling escape sequences."""
        return _parse.process_quoted_symbol(text)
    
    def split_string(self, text):
        """Split a string into a list of strings based on special characters."""
        return _parse.split_string(text)

# Create a default parser instance
default_parser = Parser()

# Export convenience functions that use the default parser
parse = default_parser.parse
to_string = default_parser.to_string 