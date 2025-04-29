#ifndef LADR_SIGNAL_UTIL_H
#define LADR_SIGNAL_UTIL_H

#include <signal.h>

/* Platform-independent signal utilities */

#if defined(_WIN32) || defined(WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
    /* Windows-specific headers */
    #include <windows.h>
    
    /* Define Unix signals that don't exist on Windows */
    #ifndef SIGUSR1
        #define SIGUSR1 10  /* Arbitrary number that doesn't conflict with existing signal values */
    #endif
    
    /* 
     * Function to emulate SIGUSR1 functionality on Windows.
     * This allows external processes to request status reports from the prover.
     */
    void register_custom_signal_handler(void (*handler)(int));
    
    /* Function to send a custom signal to a process (simulating kill -USR1 pid) */
    int send_custom_signal(int pid, int sig);
#else
    /* Unix signal handling is standard */
    #include <sys/types.h>
    
    /* For compatibility with Windows version */
    #define register_custom_signal_handler(handler) signal(SIGUSR1, handler)
    
    /* Wrapper around kill() */
    #define send_custom_signal(pid, sig) kill(pid, sig)
#endif

#endif /* LADR_SIGNAL_UTIL_H */ 