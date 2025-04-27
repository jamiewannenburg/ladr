import ladr
import ladr.ladr_combined

def test_symbol_table():
    # Test creating a term and checking its symbol
    t1 = ladr.nary("f", 2)
    print(f"Term t1 symbol: {t1.symbol}")
    
    # Test creating another term with same symbol
    t2 = ladr.nary("f", 2)
    print(f"Term t2 symbol: {t2.symbol}")
    
    # Test creating a term with different arity
    t3 = ladr.nary("f", 1)
    print(f"Term t3 symbol: {t3.symbol}")
    
    # Test parsing a term
    parsed = ladr.ladr_combined._parse_cpp.parse_term_from_string("f(a,b)")
    print(f"Parsed term symbol: {parsed.symbol}")
    print("Parsed term arguments:")
    for i in range(parsed.arity):
        print(f"  arg{i}: symbol={parsed[i].symbol}, arity={parsed[i].arity}")
    
    # Print all symbols in the table
    print("\nAll symbols in table:")
    # Since p_syms is not exposed, we'll print the symbols manually
    print("Function symbols:")
    for t in [t1, t2, t3, parsed]:
        print(f"{t.symbol}/{t.arity}")
    print("\nConstants:")
    for arg in parsed.args:
        print(f"{arg.symbol}/{arg.arity}")

def test_symbol_table_consistency():
    print("\nTesting symbol table consistency between bindings...")
    
    # Get symbol table state from both the term and parse submodules of the combined module
    # This ensures we're accessing the same symbol table
    term_symbols = ladr.ladr_combined.term.get_symbol_table_state()
    parse_symbols = ladr.ladr_combined.parse.get_symbol_table_state()
    
    print("\nTerm binding symbols:")
    for sym, id in term_symbols.items():
        print(f"  {sym}: {id}")
    
    print("\nParse binding symbols:")
    for sym, id in parse_symbols.items():
        print(f"  {sym}: {id}")
    
    # Check if they have the same symbols
    term_keys = set(term_symbols.keys())
    parse_keys = set(parse_symbols.keys())
    
    print("\nSymbols only in term binding:", term_keys - parse_keys)
    print("Symbols only in parse binding:", parse_keys - term_keys)
    
    # Check if symbol IDs match
    mismatches = []
    for sym in term_keys & parse_keys:
        if term_symbols[sym] != parse_symbols[sym]:
            mismatches.append(f"{sym}: term={term_symbols[sym]}, parse={parse_symbols[sym]}")
    
    print("\nSymbol ID mismatches:", mismatches)
    
    # Create some terms and parse them to see if they share symbols
    print("\nTesting symbol sharing:")
    # Use the combined module directly to ensure shared symbol tables
    t1 = ladr.ladr_combined.term.term2("f", 
          ladr.ladr_combined.term.term0("a"), 
          ladr.ladr_combined.term.term0("b"))
    print(f"Created term: {t1}")
    p1 = ladr.ladr_combined.parse.parse_term_from_string("f(a,b)")
    print(f"Parsed term: {p1}")
    
    # Get symbol tables again after operations
    term_symbols_after = ladr.ladr_combined.term.get_symbol_table_state()
    parse_symbols_after = ladr.ladr_combined.parse.get_symbol_table_state()
    
    print("\nSymbol tables after operations:")
    print("Term binding new symbols:", set(term_symbols_after.keys()) - term_keys)
    print("Parse binding new symbols:", set(parse_symbols_after.keys()) - parse_keys)

if __name__ == "__main__":
    test_symbol_table_consistency()
    test_symbol_table()