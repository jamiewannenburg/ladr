/* Minimal POSIX getopt for hosts without libc getopt (MSVC). */

#include "getopt_win.h"
#include <stdio.h>
#include <string.h>

char *optarg;
int optind = 1;
int opterr = 1;
int optopt;

static char *nextchar = NULL;

int
getopt(int argc, char *const argv[], const char *optstring)
{
  const char *ostr = optstring;
  int colon_mode = 0;

  if (ostr[0] == ':') {
    colon_mode = 1;
    ostr++;
  }

  if (nextchar == NULL || *nextchar == '\0') {
    nextchar = NULL;
    if (optind >= argc)
      return -1;
    if (argv[optind][0] != '-' || argv[optind][1] == '\0')
      return -1;
    if (argv[optind][1] == '-' && argv[optind][2] == '\0') {
      optind++;
      return -1;
    }
    nextchar = argv[optind] + 1;
    optind++;
  }

  optopt = (unsigned char) *nextchar++;
  if (optopt == ':') {
    if (!colon_mode && opterr)
      (void) fprintf(stderr, "%s: illegal option -- %c\n", argv[0], optopt);
    nextchar = NULL;
    return '?';
  }

  {
    const char *p = strchr(ostr, (char) optopt);

    if (p == NULL) {
      if (!colon_mode && opterr)
        (void) fprintf(stderr, "%s: illegal option -- %c\n", argv[0], optopt);
      if (*nextchar == '\0')
        nextchar = NULL;
      return '?';
    }

    if (p[1] == ':') {
      if (*nextchar != '\0') {
        optarg = nextchar;
        nextchar = NULL;
        return optopt;
      }
      if (optind < argc) {
        optarg = argv[optind++];
        nextchar = NULL;
        return optopt;
      }
      optarg = NULL;
      if (!colon_mode && opterr)
        (void) fprintf(stderr, "%s: option requires an argument -- %c\n",
                       argv[0], optopt);
      nextchar = NULL;
      return colon_mode ? ':' : '?';
    }
  }

  if (*nextchar == '\0')
    nextchar = NULL;

  return optopt;
}
