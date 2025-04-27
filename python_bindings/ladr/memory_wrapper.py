"""
Python wrapper for LADR memory module.
"""

from . import memory as _memory_cpp

def init_memory():
    """Initialize or reset the LADR memory system."""
    #_memory_cpp.init_memory()
    pass

def set_memory_limit(megs):
    """
    Set memory limit in megabytes for the LADR memory system.
    
    Args:
        megs (int): Memory limit in megabytes. Use 0 or negative to disable the limit.
    """
    #_memory_cpp.set_memory_limit(megs)
    pass

def get_memory_usage():
    """
    Get the current memory usage of the LADR system.
    
    Returns:
        tuple: (megabytes_allocated, bytes_allocated)
    """
    return (_memory_cpp.megs_malloced(), _memory_cpp.bytes_palloced()) 