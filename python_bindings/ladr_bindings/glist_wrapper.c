#include "glist_wrapper.h"
#include "../../ladr/glist.h"

Plist python_list_to_plist_wrapper(PyObject* py_list) {
    Plist head = NULL;
    Plist tail = NULL;
    
    Py_ssize_t len = PyList_Size(py_list);
    for (Py_ssize_t i = 0; i < len; i++) {
        PyObject* item = PyList_GetItem(py_list, i);
        // For now, just store the PyObject pointer directly
        // In a real implementation, convert item to appropriate LADR type (e.g., Topform)
        // This requires additional bindings and type checking
        void *data = (void *)item;
        Plist new_node = get_plist();
        new_node->v = data;
        new_node->next = NULL;

        if (head == NULL) {
            head = new_node;
            tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
        }
    }

    return head;
}

PyObject *plist_to_python_list_wrapper(Plist plist) {
    PyObject *list = PyList_New(0);
    if (list == NULL) {
        return NULL;
    }

    Plist current = plist;
    while (current != NULL) {
        // For now, assume v is a PyObject pointer
        // In a real implementation, convert LADR type (e.g., Topform) to Python object
         PyObject *item = ( PyObject *)current->v;
        if (PyList_Append(list, item) == -1) {
            Py_DECREF(list);
            return NULL;
        }
        current = current->next;
    }

    return list;
}
