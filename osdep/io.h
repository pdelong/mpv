/*
 * unicode/utf-8 I/O helpers and wrappers for Windows
 *
 * This file is part of mplayer2.
 *
 * mplayer2 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * mplayer2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with mplayer2.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MPLAYER_OSDEP_IO
#define MPLAYER_OSDEP_IO

#include "config.h"
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if HAVE_GLOB
#include <glob.h>
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

// This is in POSIX.1-2008, but support outside of Linux is scarce.
#ifndef O_CLOEXEC
#define O_CLOEXEC 0
#endif
#ifndef FD_CLOEXEC
#define FD_CLOEXEC 0
#endif

bool mp_set_cloexec(int fd);
int mp_make_wakeup_pipe(int pipes[2]);

#ifdef _WIN32
#include <wchar.h>
wchar_t *mp_from_utf8(void *talloc_ctx, const char *s);
char *mp_to_utf8(void *talloc_ctx, const wchar_t *s);
#endif

#ifdef __CYGWIN__
#include <io.h>
#endif

#ifdef __MINGW32__

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

void mp_get_converted_argv(int *argc, char ***argv);

int mp_printf(const char *format, ...);
int mp_fprintf(FILE *stream, const char *format, ...);
int mp_open(const char *filename, int oflag, ...);
int mp_creat(const char *filename, int mode);
FILE *mp_fopen(const char *filename, const char *mode);
DIR *mp_opendir(const char *path);
struct dirent *mp_readdir(DIR *dir);
int mp_closedir(DIR *dir);
int mp_mkdir(const char *path, int mode);
FILE *mp_tmpfile(void);
char *mp_getenv(const char *name);
off_t mp_lseek(int fd, off_t offset, int whence);

// MinGW-w64 will define "stat" to something useless. Since this affects both
// the type (struct stat) and the stat() function, it makes us harder to
// override these separately.
// Corresponds to struct _stat64 (copy & pasted, but using public types).
struct mp_stat {
    dev_t st_dev;
    ino_t st_ino;
    unsigned short st_mode;
    short st_nlink;
    short st_uid;
    short st_gid;
    dev_t st_rdev;
    int64_t st_size;
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
};

int mp_stat(const char *path, struct mp_stat *buf);
int mp_fstat(int fd, struct mp_stat *buf);

typedef struct {
    size_t gl_pathc;
    char **gl_pathv;
    size_t gl_offs;
    void *ctx;
} mp_glob_t;

// glob-win.c
int mp_glob(const char *restrict pattern, int flags,
            int (*errfunc)(const char*, int), mp_glob_t *restrict pglob);
void mp_globfree(mp_glob_t *pglob);

// NOTE: stat is not overridden with mp_stat, because MinGW-w64 defines it as
//       macro.

#define printf(...) mp_printf(__VA_ARGS__)
#define fprintf(...) mp_fprintf(__VA_ARGS__)
#define open(...) mp_open(__VA_ARGS__)
#define creat(...) mp_creat(__VA_ARGS__)
#define fopen(...) mp_fopen(__VA_ARGS__)
#define opendir(...) mp_opendir(__VA_ARGS__)
#define readdir(...) mp_readdir(__VA_ARGS__)
#define closedir(...) mp_closedir(__VA_ARGS__)
#define mkdir(...) mp_mkdir(__VA_ARGS__)
#define tmpfile(...) mp_tmpfile(__VA_ARGS__)
#define getenv(...) mp_getenv(__VA_ARGS__)

// Things MinGW defines as macros, and which we want to override only for the
// user, and not io.c (which wants the original definition).
#ifndef MP_HIDE_IO_REPLACEMENTS
#undef lseek
#define lseek(...) mp_lseek(__VA_ARGS__)
#endif

#undef stat
#define stat mp_stat
#undef fstat
#define fstat(...) mp_fstat(__VA_ARGS__)

#ifndef GLOB_NOMATCH
#define GLOB_NOMATCH 3
#endif

#define glob_t mp_glob_t
#define glob(...) mp_glob(__VA_ARGS__)
#define globfree(...) mp_globfree(__VA_ARGS__)

#endif /* __MINGW32__ */

#endif
