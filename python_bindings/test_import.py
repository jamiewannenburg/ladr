from ladr import order

# Test creating a list and sorting it
test_list = [3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5]

# Define a comparison function
def compare(a, b):
    if a < b:
        return order.Ordertype.LESS_THAN
    elif a > b:
        return order.Ordertype.GREATER_THAN
    else:
        return order.Ordertype.SAME_AS

# Sort the list
sorted_list = order.merge_sort(test_list, compare)
print("Original list:", test_list)
print("Sorted list:", sorted_list) 