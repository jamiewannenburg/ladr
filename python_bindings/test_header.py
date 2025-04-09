from ladr_string import header_bindings

# Test BOOL enum
print("BOOL enum values:")
print(f"TRUE = {header_bindings.BOOL.TRUE}")
print(f"FALSE = {header_bindings.BOOL.FALSE}")

# Test IMAX and IMIN functions
print("\nTesting IMAX and IMIN:")
a, b = 5, 10
print(f"IMAX({a}, {b}) = {header_bindings.imax(a, b)}")
print(f"IMIN({a}, {b}) = {header_bindings.imin(a, b)}")

# Test DBL_LARGE constant
print("\nDBL_LARGE constant:")
print(f"DBL_LARGE = {header_bindings.DBL_LARGE}") 