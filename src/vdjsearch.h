/*
    Copyright (C) 2012-2021 Torbjorn Rognes and Frederic Mahe

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact: Torbjorn Rognes <torognes@ifi.uio.no>,
    Department of Informatics, University of Oslo,
    PO Box 1080 Blindern, NO-0316 Oslo, Norway
*/

#include <inttypes.h>

#ifndef PRIu64
#ifdef _WIN32
#define PRIu64 "I64u"
#else
#define PRIu64 "lu"
#endif
#endif

#ifndef PRId64
#ifdef _WIN32
#define PRId64 "I64d"
#else
#define PRId64 "ld"
#endif
#endif

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <getopt.h>
#include <stdlib.h>
#include <regex.h>
#include <limits.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __APPLE__
#include <sys/resource.h>
#include <sys/sysctl.h>
#elif defined _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/resource.h>
#include <sys/sysinfo.h>
#endif

#ifdef __aarch64__

#include <arm_neon.h>

#elif defined __x86_64__

#ifdef __SSE2__
#include <emmintrin.h>
#endif

#ifdef __SSSE3__
#include <tmmintrin.h>
#endif

#define CAST_m128i_ptr(x) (reinterpret_cast<__m128i*>(x))

#elif defined __PPC__

#ifdef __LITTLE_ENDIAN__
#include <altivec.h>
#else
#error Big endian ppc64 CPUs not supported
#endif

#else

#error Unknown architecture
#endif

static_assert(INT_MAX > 32767, "Your compiler uses very short integers.");

/* constants */

#define PROG_NAME "vdjsearch"
#define PROG_VERSION "0.0.2"
#define PROG_BRIEF "Immune repertoire analysis"

const unsigned int MAX_THREADS = 256;

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif

const int alphabet_size = 20;

/* common data */

extern char * opt_log;
extern char * opt_output_file;
extern int64_t opt_differences;
extern int64_t opt_help;
extern int64_t opt_indels;
extern int64_t opt_threads;
extern int64_t opt_version;

extern FILE * outfile;
extern FILE * logfile;

/* header files */

#include "util.h"

#include "arch.h"
#include "bloompat.h"
#include "db.h"
#include "hashtable.h"
#include "overlap.h"
#include "threads.h"
#include "variants.h"
#include "zobrist.h"
