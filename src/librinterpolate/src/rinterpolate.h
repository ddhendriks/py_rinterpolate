#pragma once
#ifndef RINTERPOLATE_H
#define RINTERPOLATE_H

/*
 * Header file for librinterpolate
 */

/************************************************************
 * include a selection of standard libraries
 ************************************************************/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include "rinterpolate_compiler.h"

/************************************************************
 * rinterpolate's macros
 *************************************************************/

#define RINTERPOLATE_VERSION "1.6"
#define RINTERPOLATE_MAJOR_VERSION 1
#define RINTERPOLATE_MINOR_VERSION 6

/* types */
typedef unsigned int rinterpolate_counter_t;
typedef int rinterpolate_signed_counter_t;
typedef double rinterpolate_float_t;
typedef unsigned int rinterpolate_Boolean_t;

/* debugging */
#define Rinterpolate_print(...) /* do nothing */
//#define Rinterpolate_print(...) fprintf(stdout,__VA_ARGS__);

/* error codes */
#define RINTERPOLATE_NO_ERROR 0
#define RINTERPOLATE_CALLOC_FAILED 1
#define RINTERPOLATE_ALLOCATE_OVER 2

/* enable malloc/calloc checks : done once, should be fast */
#define RINTERPOLATE_ALLOC_CHECKS

/* enable debugging output (lots of output!) */
//#define RINTERPOLATE_DEBUG

/* output stream, i.e. stdout or stderr */
#define RINTERPOLATE_STREAM stdout

/* enable this to show the whole table */
//#define RINTERPOLATE_DEBUG_SHOW_TABLE

/*
 * Perhaps use realloc instead of an alternative algorithm?
 * On a modern i7 this is a little faster.
 */
#define RINTERPOLATE_USE_REALLOC

/*
 * Use standard *alloc
 */
#define Rinterpolate_malloc(A) malloc(A)
#define Rinterpolate_calloc(A,B) calloc((A),(B))
#define Rinterpolate_realloc(A,B) realloc((A),(B))

/*
 * In some places I have different algorithms based on either
 * pointers or arrays. The speed of execution probably depends on
 * how your compiler deals with these...  The code for pointers is smaller
 * with gcc 3.4.6 and very slightly faster. With later gcc (4.1) the
 * pointer arithmetic is even faster. Try it and see.
 */
#define RINTERPOLATE_USE_POINTER_ARITHMETIC

/* ditto for the j loop (see below)  */
#define RINTERPOLATE_POINTER_ARITHMETIC_J_LOOP

/*
 * with the new cache we use macros to
 * access cache items, rather than accessing
 * them directly
 */

/* length of a line in the cache */
#define RINTERPOLATE_CACHE_LINE (table->n+table->d)

/* pointer to cache result A */
#define Rinterpolate_cache_param(A) (table->cache+RINTERPOLATE_CACHE_LINE*(A))

/* pointer to the location of cache result A */
#define RINTERPOLATE_CACHE_RESULT(A) (table->cache+RINTERPOLATE_CACHE_LINE*(A)+table->n)

/* memcpy is usually faster for copying interpolation results to the cache */
#define RINTERPOLATE_CACHE_USE_MEMCPY

/*
 * Make short arrays through which binary searches are done:
 * this is usually faster because these are smaller than the
 * CPU cache.
 */
#define RINTERPOLATE_PRESEARCH

/* use interpolation cache? should speed up interpolation in many cases */
#define RINTERPOLATE_CACHE

/*
 * Use memcmp to compare cache lines?
 * The alternative is !Fequal, which is usually a bit faster.
 * Your mileage may vary.
 */
#define RINTERPOLATE_CACHE_USE_MEMCMP


/************************************************************
 * rinterpolate's structures
 ************************************************************/

struct rinterpolate_hypertable_t {
    struct rinterpolate_table_t * table;
    rinterpolate_float_t * data;
    rinterpolate_float_t * f;
    rinterpolate_counter_t  * sum;
#ifdef RINTERPOLATE_USE_REALLOC
    size_t RINTERPOLATE_ALLOCD;
#endif
};

struct rinterpolate_table_t {
    struct rinterpolate_data_t * parent;
    struct rinterpolate_hypertable_t * hypertable;
    rinterpolate_float_t  * data;
#ifdef RINTERPOLATE_CACHE
    rinterpolate_float_t * RESTRICT cache;
    rinterpolate_counter_t cache_match_line;
    rinterpolate_signed_counter_t cache_spin_line;
#endif
    rinterpolate_counter_t * RESTRICT steps;
#ifdef RINTERPOLATE_PRESEARCH
    rinterpolate_float_t ** RESTRICT presearch;
    rinterpolate_counter_t  presearch_n;
#endif
    size_t d_float_sizeof;
    size_t n_float_sizeof;
    size_t line_length_sizeof;
    size_t sum_sizeof;
    rinterpolate_counter_t * RESTRICT varcount;
    rinterpolate_counter_t n;
    rinterpolate_counter_t d;
    rinterpolate_counter_t l;
#ifndef RINTERPOLATE_PRESEARCH
    rinterpolate_counter_t g;
#endif
    rinterpolate_counter_t line_length;
#ifdef RINTERPOLATE_CACHE
    rinterpolate_counter_t cache_length;
#endif
    rinterpolate_counter_t hypertable_length;
    rinterpolate_counter_t table_number;
};


struct rinterpolate_data_t {
    struct rinterpolate_table_t  *  * RESTRICT tables;
    rinterpolate_counter_t number_of_interpolation_tables;
};

/************************************************************
 * rinterpolate's prototypes
 ************************************************************/

#include "rinterpolate_prototypes.h"


#endif // RINTERPOLATE_H
