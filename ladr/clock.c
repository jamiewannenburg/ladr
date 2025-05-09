/*  Copyright (C) 2006, 2007 William McCune

    This file is part of the LADR Deduction Library.

    The LADR Deduction Library is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License,
    version 2.

    The LADR Deduction Library is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the LADR Deduction Library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "clock.h"

/* Private definitions and types */

struct clock {
  char       *name;                 /* name of clock */
  unsigned   accum_msec;            /* accumulated time */
  unsigned   curr_msec;             /* time since clock has been turned on */
  int        level;                 /* STARTs - STOPs */
};

static LADR_BOOL Clocks_enabled = TRUE;   /* clock() can be slow */
static unsigned Clock_starts = 0;    /* Keep a count */

static time_t Wall_start;           /* for measuring wall-clock time */

/* Macro to get the number of user CPU milliseconds since the start of
   the process. Using clock() from time.h which measures CPU time.
*/

#define CPU_TIME(msec) { msec = (unsigned)(clock() * 1000.0 / CLOCKS_PER_SEC); }

/*
 * memory management
 */

#define PTRS_CLOCK PTRS(sizeof(struct clock))
static unsigned Clock_gets, Clock_frees;

/*************
 *
 *   Clock get_clock()
 *
 *************/

static
Clock get_clock(void)
{
  Clock p = get_cmem(PTRS_CLOCK);
  Clock_gets++;
  return(p);
}  /* get_clock */

/*************
 *
 *    free_clock()
 *
 *************/

/* DOCUMENTATION
*/

/* PUBLIC */
void free_clock(Clock p)
{
  if (p != NULL) {
    free_mem(p, PTRS_CLOCK);
    Clock_frees++;
  }
}  /* free_clock */

/*
 *  end of memory management
 */

/*************
 *
 *   clock_init()
 *
 *************/

/* DOCUMENTATION
This routine initializes a clock.  You give it a string
(which is not copied), representing the name of the new clock,
and it returns a Clock to be used for all operations on the clock.
<P>
The clock operations are clock_start(), clock_stop(),
clock_seconds(), clock_milliseconds(), and clock_reset().
*/

/* PUBLIC */
Clock clock_init(char *str)
{
  Clock p = get_clock();
  p->name = str;
  p->level = 0;
  p->accum_msec = 0;
  return p;
}  /* clock_init */

/*************
 *
 *   clock_start()
 *
 *************/

/* DOCUMENTATION
This routine starts clock n.  It is okay if the clock is already going.
*/

/* PUBLIC */
void clock_start(Clock p)
{
  if (Clocks_enabled) {
    p->level++;
    if (p->level == 1) {
      CPU_TIME(p->curr_msec);
      Clock_starts++;
    }
  }
}  /* clock_start */

/*************
 *
 *   clock_stop()
 *
 *************/

/* DOCUMENTATION
This routine stops clock n and adds the time to the accumulated total,
<I>unless there have been too many starts and not enough stops</I>.
See the introduction.
*/

/* PUBLIC */
void clock_stop(Clock p)
{
  if (Clocks_enabled) {
    if (p->level <= 0)
      fprintf(stderr,"WARNING, clock_stop: clock %s not running.\n",p->name); 
    else {
      p->level--;
      if (p->level == 0) {
	unsigned msec;
	CPU_TIME(msec);
	p->accum_msec += msec - p->curr_msec;
      }
    }
  }
}  /* clock_stop */

/*************
 *
 *   clock_milliseconds()
 *
 *************/

/* DOCUMENTATION
This routine returns the current value of a clock, in milliseconds.
The value is in milliseconds.
*/

/* PUBLIC */
unsigned clock_milliseconds(Clock p)
{
  if (p == NULL)
    return 0;
  else {
    int i = p->accum_msec;
    if (p->level == 0)
      return i;
    else {
      int msec;
      CPU_TIME(msec);
      return i + (msec - p->curr_msec);
    }
  }
}  /* clock_milliseconds */

/*************
 *
 *   clock_seconds()
 *
 *************/

/* DOCUMENTATION
This routine returns the current value of a clock, in seconds.
The clock need not be stopped.
*/

/* PUBLIC */
double clock_seconds(Clock p)
{
  if (p == NULL)
    return 0.0;
  else {
    int i = p->accum_msec;
    if (p->level == 0)
      return (i / 1000.0);
    else {
      int msec;
      CPU_TIME(msec);
      return (i + (msec - p->curr_msec)) / 1000.0;
    }
  }
}  /* clock_seconds */

/*************
 *
 *   clock_running()
 *
 *************/

/* DOCUMENTATION
This routine tells you whether or not a clock is running.
*/

/* PUBLIC */
LADR_BOOL clock_running(Clock p)
{
  return p->level > 0;
}  /* clock_running */

/*************
 *
 *   clock_reset()
 *
 *************/

/* DOCUMENTATION
This routine resets a clock, as if it had just been initialized.
(You should not need this routine under normal circumstances.)
*/
  
/* PUBLIC */
void clock_reset(Clock p)
{
  if (p != NULL) {
    p->level = 0;
    p->accum_msec = 0;
  }
}  /* clock_reset */

/*************
 *
 *   fprint_clock()
 *
 *************/

/* DOCUMENTATION
This routine
*/

/* PUBLIC */
void fprint_clock(FILE *fp, Clock p)
{
  if (p != NULL)
    fprintf(fp, "clock %-15s: %6.2f seconds.\n", p->name, clock_seconds(p));
}  /* fprint_clock */

/*************
 *
 *   get_date()
 *
 *************/

/* DOCUMENTATION
This routine returns a string representation of the current date and time.
*/

/* PUBLIC */
char * get_date(void)
{
  time_t i = time(NULL);
  return asctime(localtime(&i));
}  /* get_date */

/*************
 *
 *   user_time()
 *
 *************/

/* DOCUMENTATION
This routine returns the number of milliseconds of user CPU time
used by the process.
*/

/* PUBLIC */
unsigned user_time()
{
  return (unsigned)(clock() * 1000.0 / CLOCKS_PER_SEC);
}  /* user_time */

/*************
 *
 *   user_seconds()
 *
 *************/

/* DOCUMENTATION
This routine returns the number of seconds of user CPU time
used by the process.
*/

/* PUBLIC */
double user_seconds()
{
  return (double)clock() / CLOCKS_PER_SEC;
}  /* user_seconds */

/*************
 *
 *   system_time()
 *
 *************/

/* DOCUMENTATION
This routine returns the number of milliseconds of system CPU time
used by the process.
*/

/* PUBLIC */
unsigned system_time()
{
  /* In C17, we can't easily distinguish between user and system time.
     We'll return 0 for system time as a conservative estimate. */
  return 0;
}  /* system_time */

/*************
 *
 *   system_seconds()
 *
 *************/

/* DOCUMENTATION
This routine returns the number of seconds of system CPU time
used by the process.
*/

/* PUBLIC */
double system_seconds()
{
  /* In C17, we can't easily distinguish between user and system time.
     We'll return 0 for system time as a conservative estimate. */
  return 0.0;
}  /* system_seconds */

/*************
 *
 *   absolute_wallclock()
 *
 *************/

/* DOCUMENTATION
This routine returns the number of milliseconds since the epoch.
*/

/* PUBLIC */
unsigned absolute_wallclock(void)
{
  time_t t = time(NULL);
  return (unsigned)(t * 1000);  /* Convert seconds to milliseconds */
}  /* absolute_wallclock */

/*************
 *
 *   init_wallclock()
 *
 *************/

/* DOCUMENTATION
This routine initializes the wall clock.
*/

/* PUBLIC */
void init_wallclock()
{
  Wall_start = time(NULL);
}  /* init_wallclock */

/*************
 *
 *   wallclock()
 *
 *************/

/* DOCUMENTATION
This routine returns the number of milliseconds since init_wallclock() was called.
*/

/* PUBLIC */
unsigned wallclock()
{
  time_t now = time(NULL);
  return (unsigned)((now - Wall_start) * 1000);
}  /* wallclock */

/*************
 *
 *   disable_clocks()
 *
 *************/

/* DOCUMENTATION
*/

/* PUBLIC */
void disable_clocks(void)
{
  Clocks_enabled = FALSE;
}  /* disable_clocks */

/*************
 *
 *   enable_clocks()
 *
 *************/

/* DOCUMENTATION
*/

/* PUBLIC */
void enable_clocks(void)
{
  Clocks_enabled = TRUE;
}  /* enable_clocks */

/*************
 *
 *   clocks_enabled()
 *
 *************/

/* DOCUMENTATION
*/

/* PUBLIC */
LADR_BOOL clocks_enabled(void)
{
  return Clocks_enabled;
}  /* clocks_enabled */


