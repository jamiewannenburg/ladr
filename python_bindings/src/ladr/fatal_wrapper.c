/*  Modified fatal.c for Python bindings that doesn't terminate the program
    Based on the original LADR fatal.c
*/

#include "../../../ladr/fatal.h"
#include <string.h>
// #include <Python.h>
#include "fatal.h"

/* Global variables for error handling */
static int Fatal_exit_code = 1;

/*************
 *
 *   bell() - modified to respect suppression
 *
 *************/

void bell(FILE *fp)
{
  // if (!Suppress_error_messages) {
  //   fprintf(fp, "%c", '\007');
  // }
  // should not ring bell in python
}

/*************
 *
 *   get_fatal_exit_code() - unchanged from original
 *
 *************/

int get_fatal_exit_code()
{
  return Fatal_exit_code;
}

/*************
 *
 *   set_fatal_exit_code() - unchanged from original
 *
 *************/

void set_fatal_exit_code(int exit_code)
{
  Fatal_exit_code = exit_code;
}

/*************
 *
 *   fatal_error() - MODIFIED to throw c++ exception
 *
 *************/

void fatal_error(char *message)
{
  throw_ladr_fatal_error(message, Fatal_exit_code);
  // directly raise a python exception (does not work)
  //PyErr_SetString(PyExc_ValueError, message); 

  /* Code for using flags to store the error message
  // Store the error message
  strncpy(Fatal_error_message, message, sizeof(Fatal_error_message) - 1);
  Fatal_error_message[sizeof(Fatal_error_message) - 1] = '\0';
  
  // Set the error flag
  Error_occurred = 1;

  // Print the message to stdout and stderr if not suppressed
  if (!Suppress_error_messages) {
    fprintf(stdout, "\nFatal error:  %s\n\n", message);
    fprintf(stderr, "\nFatal error:  %s\n\n", message);
  }
  */
  
  // We return instead of exiting, allowing the C++ wrapper to check for errors
  return;
}
