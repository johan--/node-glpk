/* glpenv.h (GLPK environment) */

/***********************************************************************
*  This code is part of GLPK (GNU Linear Programming Kit).
*
*  Copyright (C) 2000-2015 Andrew Makhorin, Department for Applied
*  Informatics, Moscow Aviation Institute, Moscow, Russia. All rights
*  reserved. E-mail: <mao@gnu.org>.
*
*  GLPK is free software: you can redistribute it and/or modify it
*  under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  GLPK is distributed in the hope that it will be useful, but WITHOUT
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
*  License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with GLPK. If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#ifndef GLPK_ENV_H
#define GLPK_ENV_H

#include "stdc.h"
#include <execinfo.h>

#ifdef HAVE_ENV
typedef struct ENV ENV;
typedef struct MBD MBD;
#endif

#ifndef SIZE_T_MAX
#define SIZE_T_MAX (~(size_t)0)
#endif
/* largest value of size_t type */


#ifdef HAVE_ENV
#define TBUF_SIZE 4096
#else
#define TBUF_SIZE 256
#endif
/* terminal output buffer size, in bytes */

#define EBUF_SIZE 1024
/* error message buffer size, in bytes */

/* enable/disable flag: */
#define GLP_ON  1
#define GLP_OFF 0

#ifdef HAVE_ENV
typedef struct _glp_environ_state {
    pthread_rwlock_t env_lock;
    ENV* env;
} glp_environ_state_t;


#define _set_peak(NAME, PEAK)                                \
    do {                                                      \
        if (env->mem_##NAME > env->mem_##PEAK) env->mem_##PEAK = env->mem_##NAME; \
    } while (0)

#define _set_peak_tls(NAME, PEAK) _set_peak(NAME##_tls, PEAK##_tls)


#define GET_MEM_COUNT() env->mem_count
#define GET_MEM_TOTAL() env->mem_total

#define GET_MEM_LIMIT() env->mem_limit
#define GET_MEM_CPEAK() env->mem_cpeak
#define GET_MEM_TPEAK() env->mem_tpeak

#define SET_MEM_LIMIT(VALUE) (env->mem_limit = VALUE)
#define SET_MEM_TPEAK() _set_mem_tpeak_func(env);
#define SET_MEM_CPEAK() _set_mem_cpeak_func(env);
#define ADD_MEM_COUNT(x) _add_mem_count_func(env, x);
#define ADD_MEM_TOTAL(x) _add_mem_total_func(env, x);

struct ENV
{     /* GLPK environment block */
      char version[7+1];
      /* version string returned by the routine glp_version */
      ENV *self;
      /* pointer to this block to check its validity */
      /*--------------------------------------------------------------*/
      /* terminal output */
      char *term_buf; /* char term_buf[TBUF_SIZE]; */
      /* terminal output buffer */
      int term_out;
      /* flag to enable/disable terminal output */
      int (*term_hook)(void *info, const char *s);
      /* user-defined routine to intercept terminal output */
      void *term_info;
      /* transit pointer (cookie) passed to the routine term_hook */
      FILE *tee_file;
      /* output stream used to copy terminal output */
      /*--------------------------------------------------------------*/
      /* error handling */
#if 1 /* 07/XI-2015 */
      int err_st;
      /* error state flag; set on entry to glp_error */
#endif
      const char *err_file;
      /* value of the __FILE__ macro passed to glp_error */
      int err_line;
      /* value of the __LINE__ macro passed to glp_error */
      uint8_t env_tls_init_flag;
      /* Flag indicating tls_init already occurred on this env */
      void (*err_hook)(void *info);
      /* user-defined routine to intercept abnormal termination */
      void *err_info;
      /* transit pointer (cookie) passed to the routine err_hook */
      char *err_buf; /* char err_buf[EBUF_SIZE]; */
      /* buffer to store error messages (used by I/O routines) */
      /*--------------------------------------------------------------*/
      /* dynamic memory allocation */
      MBD *mem_ptr;
      /* pointer to the linked list of allocated memory blocks */
      size_t mem_limit;
      /* maximal amount of memory, in bytes, available for dynamic
       * allocation */
      size_t mem_count;
      /* total number of currently allocated memory blocks */
      size_t mem_count_tls;
      /* total count of currently allocated memory blockson this thread */
      size_t mem_cpeak;
      /* peak value of mem_count */
      size_t mem_cpeak_tls;
      /* peak value of mem_count */
      size_t mem_total;
      /* total on this thread */
      size_t mem_total_tls;
      /* total amount of currently allocated memory, in bytes; it is
       * the sum of the size field over all memory block descriptors */
      size_t mem_tpeak;
      /* peak value of mem_total */
      size_t mem_tpeak_tls;
      /* peak value of mem_total */
      /*--------------------------------------------------------------*/
      /* dynamic linking support (optional) */
      void *h_odbc;
      /* handle to ODBC shared library */
      void *h_mysql;
      /* handle to MySQL shared library */
};

struct glp_memory_counters {
    size_t mem_count;
    /* total number of currently allocated memory blocks */
    size_t mem_cpeak;
    /* peak value of mem_count */
    size_t mem_total;
    /* total amount of currently allocated memory, in bytes; it is
     * the sum of the size field over all memory block descriptors */
    size_t mem_tpeak;
};

struct MBD
{     /* memory block descriptor */
      size_t size;
      /* size of block, in bytes, including descriptor */
      MBD *self;
      /* pointer to this descriptor to check its validity */
      MBD *prev;
      /* pointer to previous memory block descriptor */
      MBD *next;
      /* pointer to next memory block descriptor */
      ENV *env;
      /* pointer to tls environment */
};

static inline __attribute__((always_inline)) void _add_mem_total_func(ENV* env, size_t x) {
    env->mem_total += x;
    env->mem_total_tls += x;
}

static inline __attribute__((always_inline)) void _add_mem_count_func(ENV* env, size_t x) {
    env->mem_count_tls += x;
    env->mem_count += x;
}

static inline __attribute__((always_inline)) void _set_mem_tpeak_func(ENV* env) {
    _set_peak_tls(total, tpeak);
    _set_peak(total, tpeak);
}

static inline __attribute__((always_inline)) void _set_mem_cpeak_func(ENV* env) {
    _set_peak_tls(count, cpeak);
    _set_peak(count, cpeak);
}
#endif // ifdef HAVE_ENV


#ifdef HAVE_ENV
#define get_env_ptr _glp_get_env_ptr
ENV *get_env_ptr(void);
/* retrieve pointer to environment block */

#define tls_set_ptr _glp_tls_set_ptr
void tls_set_ptr(void *ptr);
/* store global pointer in TLS */

#define tls_get_ptr _glp_tls_get_ptr
void *tls_get_ptr(void);
/* retrieve global pointer from TLS */
#endif

#define xputs glp_puts
void glp_puts(const char *s);
/* write string on terminal */

#define xprintf glp_printf
void glp_printf(const char *fmt, ...);
/* write formatted output on terminal */

#define xvprintf glp_vprintf
void glp_vprintf(const char *fmt, va_list arg);
/* write formatted output on terminal */

int glp_term_out(int flag);
/* enable/disable terminal output */

#ifdef HAVE_ENV
void glp_term_hook(int (*func)(void *info, const char *s), void *info);
#else
void glp_term_hook(void (*func)(const char *s));
#endif

/* install hook to intercept terminal output */
int glp_open_tee(const char *fname);
/* start copying terminal output to text file */

int glp_close_tee(void);
/* stop copying terminal output to text file */

#ifndef GLP_ERRFUNC_DEFINED
#define GLP_ERRFUNC_DEFINED
typedef void (*glp_errfunc)(const char *fmt, ...);
#endif

#define xerror glp_error_(__FILE__, __LINE__)
glp_errfunc glp_error_(const char *file, int line);
/* display fatal error message and terminate execution */

#define xassert(expr) \
      ((void)((expr) || (glp_assert_(#expr, __FILE__, __LINE__), 1)))
void glp_assert_(const char *expr, const char *file, int line);
/* check for logical condition */

#ifdef HAVE_ENV
void glp_error_hook(void (*func)(void *info), void *info);
#else
void glp_error_hook(void (*func)(const char *s));
#endif

/* install hook to intercept abnormal termination */

#define put_err_msg _glp_put_err_msg
void put_err_msg(const char *msg);
/* provide error message string */

#define get_err_msg _glp_get_err_msg
const char *get_err_msg(void);
/* obtain error message string */

#define xmalloc(size) glp_alloc(1, size)
/* allocate memory block (obsolete) */

#define xcalloc(n, size) glp_alloc(n, size)
/* allocate memory block (obsolete) */

#define xalloc(n, size) glp_alloc(n, size)
#define talloc(n, type) ((type *)glp_alloc(n, sizeof(type)))
void *glp_alloc(int n, int size);
/* allocate memory block */

#define xrealloc(ptr, n, size) glp_realloc(ptr, n, size)
#define trealloc(ptr, n, type) ((type *)glp_realloc(ptr, n, \
      sizeof(type)))
void *glp_realloc(void *ptr, int n, int size);
/* reallocate memory block */

#define xfree(ptr) glp_free(ptr)
#define tfree(ptr) glp_free(ptr)
void glp_free(void *ptr);
/* free memory block */

void glp_mem_limit(int limit);
/* set memory usage limit */

void glp_mem_usage(size_t *count, size_t *cpeak, size_t *total, size_t *tpeak);
/* get memory usage information */

#ifdef HAVE_ENV
/** Initialize environment state (per problem, etc) 
 */
glp_environ_state_t* glp_init_env_state(void* default_info, int (*nodeHookCallback)(void*, const char*));

/** Get memory counters from an eniron_state
 */
struct glp_memory_counters glp_counters_from_state(glp_environ_state_t* env_state);
/**
 * Free all resources associated with ane env
 */
void glp_free_env_state(glp_environ_state_t *env_state);

/**
 * Reentrant and threadsafe function for migrating all environment data from the thread-local environment to the given env_state 
 * Call this whenever a thread is done working on a given problem to ensure all thread-local state is preserved.
 */
void glp_env_tls_finalize_r(glp_environ_state_t* env_state);
void glp_env_tls_init_r(glp_environ_state_t* env_state, void* info);

#endif

typedef struct glp_file glp_file;
/* sequential stream descriptor */

#define glp_open _glp_open
glp_file *glp_open(const char *name, const char *mode);
/* open stream */

#define glp_eof _glp_eof
int glp_eof(glp_file *f);
/* test end-of-file indicator */

#define glp_ioerr _glp_ioerr
int glp_ioerr(glp_file *f);
/* test I/O error indicator */

#define glp_read _glp_read
int glp_read(glp_file *f, void *buf, int nnn);
/* read data from stream */

#define glp_getc _glp_getc
int glp_getc(glp_file *f);
/* read character from stream */

#define glp_write _glp_write
int glp_write(glp_file *f, const void *buf, int nnn);
/* write data to stream */

#define glp_format _glp_format
int glp_format(glp_file *f, const char *fmt, ...);
/* write formatted data to stream */

#define glp_close _glp_close
int glp_close(glp_file *f);
/* close stream */

#define xtime glp_time
double glp_time(void);
/* determine current universal time */

#define xdifftime glp_difftime
double glp_difftime(double t1, double t0);
/* compute difference between two time values */

#define xdlopen _glp_dlopen
void *xdlopen(const char *module);
/* open dynamically linked library */

#define xdlsym _glp_dlsym
void *xdlsym(void *h, const char *symbol);
/* obtain address of symbol from dynamically linked library */

#define xdlclose _glp_dlclose
void xdlclose(void *h);
/* close dynamically linked library */

#endif

/* eof */
