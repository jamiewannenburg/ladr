/* Replacement for <getopt.h> on platforms without it (e.g. MSVC). */

#ifndef LADR_GETOPT_WIN_H
#define LADR_GETOPT_WIN_H

#ifdef __cplusplus
extern "C" {
#endif

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

int getopt(int argc, char *const argv[], const char *optstring);

#ifdef __cplusplus
}
#endif

#endif
