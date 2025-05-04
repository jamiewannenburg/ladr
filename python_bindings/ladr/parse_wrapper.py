"""
Python wrapper for LADR parse module.
"""
import re
import sys
import tempfile
import os
import io
from . import parse as _parse_cpp
from .term_wrapper import Term
from . import LadrFatalError
#from .error_wrapper import LadrFatalError, catch_fatal_errors
# Parse error extending LadrFatalError
class ParseError(LadrFatalError):
    """
    Parse error extending LadrFatalError printing logfile content.
    """
    # accept message and filename
    def __init__(self, message, detail):
        super().__init__(message)
        # send detail to stderr
        print(detail, file=sys.stderr)

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

#@catch_fatal_errors
def parse_term(text):
    """
    Parse a term from a string or io.BytesIO buffer.
    """
    buffer_mine = False
    if isinstance(text, io.BytesIO):
        buffer = text
    else:
        buffer = io.BytesIO(text.encode('ascii'))
        buffer_mine = True
    # if I'm at the end of buffer, return None
    if buffer.tell() == buffer.getbuffer().nbytes:
        return None
    # Create a temporary file
    logfile = tempfile.NamedTemporaryFile(delete=False)
    try:
        cpp_term = _parse_cpp.sread_term(buffer, logfile.name)
        # Wrap it in a Term object
        if buffer_mine:
            buffer.close()
        logfile.close()
        os.remove(logfile.name)
        return Term(cpp_term)
    except LadrFatalError as e:
        # Re-raise with a more descriptive message
        # read logfile content
        
        logfile.close()
        with open(logfile.name, 'r') as f:
            detail = f.read()
            print(detail)
        if buffer_mine:
            buffer.close()
        #os.remove(logfile.name)
        # raise ParseError
        raise ParseError(f"Failed to parse term: {buffer.getvalue()}", detail) from e 
