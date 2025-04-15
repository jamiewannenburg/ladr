#ifndef GLIST_WRAPPER_H
#define GLIST_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

// Opaque pointers for types used in Mace4 bindings to avoid conflicts
struct plist;
typedef struct plist * Plist;


// Conversion functions for Python lists to Plist and back
Plist python_list_to_plist(void *py_list);
void *plist_to_python_list(Plist plist);

#ifdef __cplusplus
}
#endif

// Structure definitions for C++ code without including original headers
#ifdef __cplusplus
// Needed?
// struct plist {
//   void *v;
//   Plist next;
// };
#endif

#endif // GLIST_WRAPPER_H