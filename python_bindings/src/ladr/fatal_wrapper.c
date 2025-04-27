/*  Modified fatal.c for Python bindings that doesn't terminate the program
    Based on the original LADR fatal.c
*/

#include "../../../ladr/fatal.h"
#include <string.h>
#include <Python.h>

/* Global variables for error handling */
static int Fatal_exit_code = 1;
static char Fatal_error_message[1024];
static int Suppress_error_messages = 0;
static int Error_occurred = 0;

/*************
 *
 *   bell() - modified to respect suppression
 *
 *************/

void bell(FILE *fp)
{
  if (!Suppress_error_messages) {
    fprintf(fp, "%c", '\007');
  }
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
 *   fatal_error() - MODIFIED to store the error message instead of exiting
 *
 *************/

void fatal_error(char *message)
{
  
  // directly raise a python exception
  PyErr_SetString(PyExc_ValueError, message); 
  /* original code: 
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

/*************
 *
 *   has_error_occurred() - NEW function to check if an error occurred
 *
 *************/

int has_error_occurred(void)
{
  return Error_occurred;
}

/*************
 *
 *   reset_error_flag() - NEW function to reset the error flag
 *
 *************/

void reset_error_flag(void)
{
  Error_occurred = 0;
  Fatal_error_message[0] = '\0';
}

/*************
 *
 *   get_fatal_error_message() - NEW function to retrieve the stored error message
 *
 *************/

char* get_fatal_error_message(void)
{
  return Fatal_error_message;
}

/*************
 *
 *   suppress_error_messages() - NEW function to control error message printing
 *
 *************/

void suppress_error_messages(int flag)
{
  Suppress_error_messages = flag;
} 