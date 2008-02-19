/*
 * Daemon internals
 *
 * Copyright (c) 2008    Nico Schottelius <nico-gpm2008 at schottelius.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _GPM_DAEMON_H
#define _GPM_DAEMON_H

/*************************************************************************
 * Includes
 */
#include "gpm.h"           /* Gpm_Event         */
#include <sys/select.h>    /* fd_set            */

/*************************************************************************
 * Types / structures
 */

struct micetab {
   struct micetab *next;
   char *device;
   char *protocol;
   char *options;
};

struct options {
   int            autodetect;          /* -u [aUtodetect..'A' is unavailable] */
   int            no_mice;             /* number of mice                      */
   int            repeater;            /* repeat data                         */
   char           *repeater_type;      /* repeat data as which mouse type     */
   int            run_status;          /* startup/daemon/debug                */
   char           *progname;           /* hopefully gpm ;)                    */
   struct micetab *micelist;           /* mice and their options              */
   char           *consolename;        /* /dev/tty0 || /dev/vc/0              */
};

typedef struct Gpm_Cinfo {
   Gpm_Connect data;
   int fd;
   struct Gpm_Cinfo *next;
} Gpm_Cinfo;

/* and this is the entry in the mouse-type table */
typedef struct Gpm_Type {
   char              *name;
   char              *desc;      /* a descriptive line */
   char              *synonyms;  /* extra names (the XFree name etc) as a list */
   int               (*fun)(Gpm_Event *state, unsigned char *data);

   struct Gpm_Type   *(*init)(int fd, unsigned short flags,
                     struct Gpm_Type   *type, int argc, char **argv);

   unsigned short    flags;
   unsigned char     proto[4];
   int               packetlen;
   int               howmany;    /* how many bytes to read at a time */
   int               getextra;   /* does it get an extra byte? (only mouseman) */
   int               absolute;   /* flag indicating absolute pointing device */

                     /* repeat this event into fd */
   int               (*repeat_fun)(Gpm_Event *state, int fd);
} Gpm_Type;

/* this structure is used to hide the dual-mouse stuff */
struct mouse_features {
   char  *opt_type,
         *opt_dev,
         *opt_sequence;
   int   opt_baud,
         opt_sample,
         opt_delta,
         opt_accel,
         opt_scale,
         opt_scaley,
         opt_time,
         opt_cluster,
         opt_three,
         opt_glidepoint_tap;
   char  *opt_options;           /* extra textual configuration */
   Gpm_Type *m_type;
   int fd;
};


/*************************************************************************
 * Macros
 */

/* How many virtual consoles are managed? */
#ifndef MAX_NR_CONSOLES
#  define MAX_NR_CONSOLES 64 /* this is always sure */
#endif

#define MAX_VC    MAX_NR_CONSOLES  /* doesn't work before 1.3.77 */



/*************************************************************************
 * Global variables
 */

extern int              opt_resize;       /* not really an option          */
extern struct options   option;           /* one should be enough for us   */
extern int              mouse_argc[3];    /* 0 for default (unused)        */
extern char           **mouse_argv[3];    /* and two mice                  */
extern int              opt_aged;
extern int              statusX,
                        statusY,
                        statusB,
                        statusC;          /* clicks */

extern fd_set           selSet,
                        readySet,
                        connSet;
extern int              eventFlag;
extern Gpm_Cinfo       *cinfo[MAX_VC+1];

extern struct mouse_features  mouse_table[3],
                             *which_mouse;      /*the current one*/





/*************************************************************************
 * Functions
 */

void gpm_killed(int signo);
int open_console(const int mode);
char **build_argv(char *argv0, char *str, int *argcptr, char sep);

int do_client(Gpm_Cinfo *cinfo, Gpm_Event *event);
int do_selection(Gpm_Event *event);


void get_console_size(Gpm_Event *ePtr);

int processConn(int fd);
int processMouse(int fd, Gpm_Event *event, Gpm_Type *type, int kd_mode);

void selection_copy(int x1, int y1, int x2, int y2, int mode);
void selection_paste(void);

void startup(int argc, char **argv);


int wait_text(int *fdptr);

/* meta-mouse functions */
void add_mouse (int type, char *value);
int  init_mice (struct micetab *micelist);
int  reset_mice(struct micetab *micelist);

/* gpn.c */
void cmdline(int argc, char **argv);
int giveInfo(int request, int fd);
int loadlut(char *charset);
int usage(char *whofailed);
struct Gpm_Type *find_mouse_by_name(char *name);
void check_uniqueness(void);
void check_kill(void);

/* gpm.c */
int old_main();



#endif