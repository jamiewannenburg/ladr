"""
Python interface for LADR top_input functions.
"""

import os
import sys
import io
import re
from typing import List, Optional, Union, Any, TextIO
from . import ladr_bindings
term = ladr_bindings.term
parse = ladr_bindings.parse
symbols = ladr_bindings.symbols
options = ladr_bindings.options
formula = ladr_bindings.formula
from .parse_wrapper import parse_term
TERMS=0
FORMULAS=1

class TopInputError(Exception):
    """Exception raised for errors in top_input functions."""
    pass

# Global variables
Input_lists = []  # List of Readlist objects

class Readlist:
    """Structure to hold input lists."""
    def __init__(self, name: str, type_: int, aux: bool, p: List[Any] = None):
        self.name = name
        self.type = type_
        self.aux = aux
        self.p = p if p is not None else []

def readlist_member(lists: List[Readlist], name: str, type_: int) -> Optional[Readlist]:
    """
    Find a list by name and type.
    
    Args:
        lists: List of Readlist objects
        name: Name to search for
        type_: Type to search for
        
    Returns:
        Matching Readlist or None if not found
    """
    for r in lists:
        if r.name == name and r.type == type_:
            return r
    return None

def readlist_member_wild(lists: List[Readlist], type_: int) -> Optional[Readlist]:
    """
    Find a list by type only (wildcard name).
    
    Args:
        lists: List of Readlist objects
        type_: Type to search for
        
    Returns:
        Matching Readlist or None if not found
    """
    for r in lists:
        if r.name == "*" and r.type == type_:
            return r
    return None

def condition_is_true(t: Any) -> bool:
    """
    Evaluate if a condition term is true.
    
    Args:
        t: Term to evaluate
        
    Returns:
        True if condition is true, False otherwise
    """
    if term.is_term(t, "flag", 1):
        flag_name = symbols.sn_to_str(t[0].symnum)
        flag_id = options.str_to_flag_id(flag_name)
        if flag_id == -1:
            raise TopInputError(f"Unknown flag in condition: {flag_name}")
        return term.flag(flag_id)
    elif term.is_term(t, "parm", 1):
        parm_name = symbols.sn_to_str(t[0].symnum)
        parm_id = options.str_to_parm_id(parm_name)
        if parm_id == -1:
            raise TopInputError(f"Unknown parameter in condition: {parm_name}")
        return term.parm(parm_id) != 0
    else:
        raise TopInputError(f"Unknown condition type: {t}")

def flag_handler(t: Any, echo: bool, unknown_action: int) -> None:
    """
    Handle set/clear commands for flags.
    
    Args:
        t: Term representing the command
        echo: Whether to echo the command
        unknown_action: Action for unknown flags (0=ignore, 1=warn, 2=error)
    """
    flag_name = symbols.sn_to_str(t[0].symnum)
    flag_id = options.str_to_flag_id(flag_name)
    
    if flag_id == -1:
        if unknown_action == 2:
            raise TopInputError(f"Unknown flag: {flag_name}")
        elif unknown_action == 1:
            print(f"% WARNING: Unknown flag: {flag_name}")
        return
        
    if term.is_term(t, "set", 1):
        options.set_flag(flag_id, True)
    else:  # clear
        options.set_flag(flag_id, False)

def parm_handler(t: Any, echo: bool, unknown_action: int) -> None:
    """
    Handle assign commands for parameters.
    
    Args:
        fout: Output file
        t: Term representing the command
        echo: Whether to echo the command
        unknown_action: Action for unknown parameters (0=ignore, 1=warn, 2=error)
    """
    parm_name = symbols.sn_to_str(t[0].symnum)
    value = t[1]
    
    if not value.constant():
        raise TopInputError(f"Parameter value must be a constant: {value}")
        
    parm_id = options.str_to_parm_id(parm_name)
    if parm_id == -1:
        if unknown_action == 2:
            raise TopInputError(f"Unknown parameter: {parm_name}")
        elif unknown_action == 1:
            print(f"% WARNING: Unknown parameter: {parm_name}", file=sys.stderr)
        return
        
    options.set_parm(parm_id, value.symnum)

def process_op(t: Any, echo: bool, fout: TextIO) -> None:
    """
    Process op commands.
    
    Args:
        t: Term representing the op command
        echo: Whether to echo the command
        fout: Output file
    """
    if echo:
        print(t)
        
    f = t[0]
    if not f.constant():
        raise TopInputError(f"First argument of op must be a symbol: {f}")
        
    if term.is_term(t, "op", 3):
        # op(symbol, precedence, type)
        prec = t[1]
        type_ = t[2]
        
        if not prec.constant() or not type_.constant():
            raise TopInputError("Precedence and type must be constants")
            
        symbols.set_precedence(symbols.sn_to_str(f.symnum), prec.symnum)
        symbols.set_type(symbols.sn_to_str(f.symnum), type_.symnum)
    else:
        # op(symbol, type)
        type_ = t[1]
        if not type_.constant():
            raise TopInputError("Type must be a constant")
        symbols.set_type(symbols.sn_to_str(f.symnum), type_.symnum)

def process_redeclare(t: Any, echo: bool, fout: TextIO) -> None:
    """
    Process redeclare commands.
    
    Args:
        t: Term representing the redeclare command
        echo: Whether to echo the command
        fout: Output file
    """
    if echo:
        print(t)
        
    f = t[0]
    type_ = t[1]
    
    if not f.constant() or not type_.constant():
        raise TopInputError("Arguments of redeclare must be constants")
        
    symbols.redeclare(symbols.sn_to_str(f.symnum), type_.symnum)

def process_symbol_list(t: Any, command: str, p: List[Any]) -> None:
    """
    Process a list of symbols for lex/predicate_order/function_order/skolem.
    
    Args:
        t: Term representing the command
        command: Name of the command
        p: List of symbols
    """
    for sym in p:
        if not sym.constant():
            raise TopInputError(f"List must contain only symbols: {t}")
            
    if command == "lex":
        symbols.set_lex_order([symbols.sn_to_str(sym.symnum) for sym in p])
    elif command == "predicate_order":
        symbols.set_predicate_order([symbols.sn_to_str(sym.symnum) for sym in p])
    elif command == "function_order":
        symbols.set_function_order([symbols.sn_to_str(sym.symnum) for sym in p])
    elif command == "skolem":
        symbols.set_skolem_order([symbols.sn_to_str(sym.symnum) for sym in p])

def read_all_input(files: List[str], echo: bool = False, unknown_action: int = 0) -> None:
    """
    Read input from files or stdin.
    
    Args:
        files: List of file paths to read from. If empty, reads from stdin.
        echo: Whether to echo the input as it's read.
        unknown_action: Action to take for unknown commands (0=ignore, 1=warn, 2=error)
    """
    if not files:
        read_from_file(sys.stdin, sys.stdout, echo, unknown_action)
    else:
        for file_path in files:
            try:
                with open(file_path, 'r') as fin:
                    if echo:
                        print(f"\n% Reading from file {file_path}\n")
                    read_from_file(fin, sys.stdout, echo, unknown_action)
            except FileNotFoundError:
                raise TopInputError(f"File {file_path} not found")
    
    process_standard_options()
    symbol_check_and_declare()

def read_from_file(fin: str, echo: bool = False, unknown_action: int = 0) -> None: # TODO: support TextIO
    """
    Read input from a file object.
    
    Args:
        fin: File object to read from
        fout: File object to write output to
        echo: Whether to echo the input as it's read
        unknown_action: Action to take for unknown commands (0=ignore, 1=warn, 2=error)
    """
    formulas = []
    terms = []
    wild_list = []
    if_depth = 0  # for conditional inclusion
    # read file into io.BytesIO
    with open(fin, 'r') as f:
        data = f.read()
    # remove comments block comments %BEGIN %END
    data = re.sub(r'%BEGIN.*?%END', '', data, flags=re.DOTALL)
    # remove comments line comments %
    data = re.sub(r'%.*', '', data)
    # remove all whitespace
    data = re.sub(r'\.\s+', '.', data, flags=re.MULTILINE)
    
    infile = io.BytesIO(data.encode('ascii'))
    T = parse_term(infile)
    t = T._term
    
    while t is not None:
        if term.is_term(t, "set", 1) or term.is_term(t, "clear", 1):
            # set, clear
            if echo:
                print(t)
            flag_handler(t, echo, unknown_action)
            
        elif term.is_term(t, "assign", 2):
            # assign
            if echo:
                print(t)
            parm_handler(t, echo, unknown_action)
            
        elif term.is_term(t, "assoc_comm", 1) or term.is_term(t, "commutative", 1):
            # AC, etc.
            f = term[0]
            if not f.constant():
                raise TopInputError(f"Argument must be symbol only: {t}")
            else:
                if echo:
                    print(t)
                if term.is_term(t, "assoc_comm", 1):
                    symbols.set_assoc_comm(symbols.sn_to_str(f.symbol()), True)
                else:
                    symbols.set_commutative(symbols.sn_to_str(f.symbol()), True)
                    
        elif term.is_term(t, "op", 3) or term.is_term(t, "op", 2):
            # op
            process_op(t, echo)
            
        elif term.is_term(t, "redeclare", 2):
            # redeclare
            process_redeclare(t, echo)
            
        elif (term.is_term(t, "lex", 1) or term.is_term(t, "predicate_order", 1) or
              term.is_term(t, "function_order", 1) or term.is_term(t, "skolem", 1)):
            # lex, skolem
            command = symbols.sn_to_str(t[0].symnum)
            p = term.listterm_to_tlist(term.ARG(t, 0))
            if p is None:
                raise TopInputError(f"Function_order/predicate_order/skolem command must contain a list, e.g., [a,b,c]: {t}")
            else:
                if echo:
                    print(t)
                process_symbol_list(t, command, p)
                
        elif (term.is_term(t, "formulas", 1) or term.is_term(t, "clauses", 1) or
              term.is_term(t, "terms", 1) or term.is_term(t, "list", 1)):
            # list of objects
            type_ = (FORMULAS if (term.is_term(t, "formulas", 1) or term.is_term(t, "clauses", 1))
                    else TERMS)
            name = symbols.sn_to_str(t[0].symnum)
            objects = []
            
            if term.is_term(t, "clauses", 1):
                print("\nWARNING: \"clauses(...)\" should be replaced with \"formulas(...)\".\n"
                      "Please update your input files. Future versions will not accept \"clauses(...)\".\n",
                      file=sys.stderr)
            elif term.is_term(t, "terms", 1):
                print("\nWARNING: \"terms(...)\" should be replaced with \"list(...)\".\n"
                      "Please update your input files. Future versions will not accept \"terms(...)\".\n",
                      file=sys.stderr)
                      
            # read the list of objects
            ti = parse_term(infile)._term
            while ti and not ti.is_constant and not symbols.is_symbol(ti.symnum, "end_of_list", 0):
                if type_ == FORMULAS:
                    objects.append(formula.term_to_formula(ti))
                else:
                    objects.append(ti)
                ti = parse_term(infile)._term

            if echo:
                if type_ == FORMULAS:
                    print(objects, name)
                else:
                    print(objects, name)
                        
            # Find the correct list and append the objects to it
            if type_ == FORMULAS:
                formulas.extend(objects)
            elif type_ == TERMS:
                terms.extend(objects)
            else:
                wild_list.extend(objects)
                
        elif term.is_term(t, "if", 1):
            # if() ... end_if
            condition = t[0]
            if echo:
                print(t)
            if condition_is_true(condition):
                if echo:
                    print("% Conditional input included.")
                if_depth += 1
            else:
                # skip to matching end_if
                depth = 1  # if-depth
                while True:
                    t2 = parse_term(infile)._term
                    if t2 is None:
                        raise TopInputError(f"Missing end_if (condition is false): {t}")
                    elif term.is_term(t2, "if", 1):
                        depth += 1
                    elif term.is_term(t2, "end_if", 0):
                        depth -= 1
                    if depth == 0:
                        break
                        
                if echo:
                    print("% Conditional input omitted.\nend_if.")
                    
        elif term.is_term(t, "end_if", 0):
            # end_if (true case)
            if_depth -= 1
            if echo:
                print(t)
            if if_depth < 0:
                raise TopInputError(f"Extra end_if: {t}")
                
        else:
            raise TopInputError(f"Unrecognized command or list: {t}")
            
        T = parse_term(infile)
        if T:
            t = T._term
        else:
            t = None
        
    if if_depth != 0:
        raise TopInputError(f"Missing end_if (condition is true): {t}")

def process_standard_options() -> None:
    """
    Process standard options after reading input.
    """
    # This function needs to be bound in C++
    raise NotImplementedError("process_standard_options needs to be bound in C++")

def symbol_check_and_declare() -> None:
    """
    Check and declare symbols after reading input.
    """
    # This function needs to be bound in C++
    raise NotImplementedError("symbol_check_and_declare needs to be bound in C++")

def process_input_formulas(formulas: List[Any], echo: bool = False) -> List[Any]:
    """
    Process input formulas.
    
    Args:
        formulas: List of formulas to process
        echo: Whether to echo the processing
        
    Returns:
        List of processed formulas
    """
    # This function needs to be bound in C++
    raise NotImplementedError("process_input_formulas needs to be bound in C++")

def process_demod_formulas(formulas: List[Any], echo: bool = False) -> List[Any]:
    """
    Process demodulator formulas.
    
    Args:
        formulas: List of formulas to process
        echo: Whether to echo the processing
        
    Returns:
        List of processed formulas
    """
    # This function needs to be bound in C++
    raise NotImplementedError("process_demod_formulas needs to be bound in C++")

def process_goal_formulas(formulas: List[Any], echo: bool = False) -> List[Any]:
    """
    Process goal formulas.
    
    Args:
        formulas: List of formulas to process
        echo: Whether to echo the processing
        
    Returns:
        List of processed formulas
    """
    # This function needs to be bound in C++
    raise NotImplementedError("process_goal_formulas needs to be bound in C++")

def embed_formulas_in_topforms(formulas: List[Any], assumption: bool = False) -> List[Any]:
    """
    Embed formulas in topforms.
    
    Args:
        formulas: List of formulas to embed
        assumption: Whether the formulas are assumptions
        
    Returns:
        List of embedded formulas
    """
    # This function needs to be bound in C++
    raise NotImplementedError("embed_formulas_in_topforms needs to be bound in C++")

def set_program_name(name: str) -> None:
    """
    Set the program name.
    
    Args:
        name: Name of the program
    """
    # This function needs to be bound in C++
    raise NotImplementedError("set_program_name needs to be bound in C++")

def init_standard_ladr() -> None:
    """
    Initialize standard LADR packages.
    """
    # This function is already bound in C++ and called during module initialization
    pass
