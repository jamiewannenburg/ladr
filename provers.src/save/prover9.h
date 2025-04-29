#ifndef TP_PROVER9_H
#define TP_PROVER9_H

#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include "../ladr/signal_util.h"

/* Platform-specific includes for signals (not needed with signal_util.h) */
#if defined(_WIN32) || defined(WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
    /* Windows-specific headers - handled in signal_util.h */
#else
    /* Unix-specific headers */
    #include <sys/wait.h>
#endif

// LADR includes

#include "../ladr/nonport.h"
#include "../ladr/header.h"
#include "../ladr/clock.h"
#include "../ladr/options.h"
#include "../ladr/sos.h"
#include "../ladr/subsume.h"
#include "../ladr/symbolcheck.h"
#include "../ladr/clausify.h"
#include "../ladr/demod.h"
#include "../ladr/flatterm.h"
#include "../ladr/paramod.h"
#include "../ladr/resolve.h"
#include "../ladr/just.h"
#include "../ladr/clause2.h"
#include "../ladr/features.h"
#include "../ladr/ioutil.h"
#include "../ladr/backdemod.h"
#include "../ladr/hints.h"
#include "../ladr/weight2.h"
#include "../ladr/hash.h"
#include "../ladr/top_input.h"
#include "../ladr/di_tree.h"
#include "../ladr/compress.h"
#include "../ladr/int_code.h"
#include "../ladr/clause3.h"
#include "../ladr/xproofs.h"
#include "../ladr/ac_redun.h"
#include "../ladr/std_options.h"

/* Exit codes */

enum {
  MAX_PROOFS_EXIT   = 0,
  FATAL_EXIT        = 1,
  SOS_EMPTY_EXIT    = 2,
  MAX_MEGS_EXIT     = 3,
  MAX_SECONDS_EXIT  = 4,
  MAX_GIVEN_EXIT    = 5,
  MAX_KEPT_EXIT     = 6,
  ACTION_EXIT       = 7,

  SIGINT_EXIT       = 101,
  SIGSEGV_EXIT      = 102,
};

#endif  /* conditional compilation of whole file */
