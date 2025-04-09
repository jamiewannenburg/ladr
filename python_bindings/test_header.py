from ladr import header

# Test BOOL enum
print("BOOL enum values:")
print(f"TRUE = {header.BOOL.TRUE}")
print(f"FALSE = {header.BOOL.FALSE}")

# Test IMAX and IMIN functions
print("\nTesting IMAX and IMIN:")
a, b = 5, 10
print(f"IMAX({a}, {b}) = {header.imax(a, b)}")
print(f"IMIN({a}, {b}) = {header.imin(a, b)}")

# Test DBL_LARGE constant
print("\nDBL_LARGE constant:")
print(f"DBL_LARGE = {header.DBL_LARGE}") 