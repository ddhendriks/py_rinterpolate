#pragma once

#ifndef RINTERPOLATE_PROTOTYPES_H
#define RINTERPOLATE_PROTOTYPES_H
#include "rinterpolate.h"

struct rinterpolate_data_t * rinterpolate(
    const rinterpolate_float_t * RESTRICT const table, // (const pointer to) the data table
    struct rinterpolate_data_t * rinterpolate_data, // where rinterpolate stores data
    const rinterpolate_counter_t n, // the number of parameters (i.e. dimensions)
    const rinterpolate_counter_t d, // the number of data items
    const rinterpolate_counter_t l, // the number of lines of data
    const rinterpolate_float_t * RESTRICT const x, // the values of the parameters
    rinterpolate_float_t * RESTRICT const r,  // the result of the interpolation
    const rinterpolate_counter_t cache_length // tells us to use the cache, or not
    );

void rinterpolate_free_data(struct rinterpolate_data_t * RESTRICT const rinterpolate_data);
rinterpolate_counter_t rinterpolate_alloc_dataspace(struct rinterpolate_data_t ** RESTRICT const r);
void rinterpolate_build_flags(struct rinterpolate_data_t * RESTRICT const rinterpolate_data);

/*
 * Internal functions 
 */

void rinterpolate_error(rinterpolate_counter_t errnum,
                        char * RESTRICT format,
                        struct rinterpolate_data_t * RESTRICT const rinterpolate_data,
                        ...) No_return Gnu_format_args(2,4);
rinterpolate_signed_counter_t Pure_function rinterpolate_id_table(
    struct rinterpolate_data_t * RESTRICT const rinterpolate_data,
    const rinterpolate_float_t * RESTRICT const data
    );



rinterpolate_Boolean_t rinterpolate_check_cache(
    struct rinterpolate_table_t * RESTRICT const table,
    const rinterpolate_float_t * RESTRICT const x,
    rinterpolate_float_t * RESTRICT const r);
#ifdef RINTERPOLATE_CACHE
void rinterpolate_alloc_cacheline(struct rinterpolate_table_t * RESTRICT const table);
#endif
void rinterpolate_make_steps(struct rinterpolate_table_t * RESTRICT const table);
void rinterpolate_alloc_hypertable(struct rinterpolate_table_t * RESTRICT const table);
void rinterpolate_alloc_varcount(struct rinterpolate_table_t * RESTRICT const table);
void rinterpolate_search_table(
    struct rinterpolate_table_t * RESTRICT const table,
    const rinterpolate_float_t * RESTRICT const x
    );
void rinterpolate_construct_hypercube(
    struct rinterpolate_table_t * RESTRICT const table
    );

void rinterpolate_interpolate(
    const struct rinterpolate_table_t * RESTRICT const table,
    const rinterpolate_float_t * RESTRICT const x,
    rinterpolate_float_t * RESTRICT const r);

void rinterpolate_store_cache(struct rinterpolate_table_t * RESTRICT const table,
                              const rinterpolate_float_t * RESTRICT const x,
                              const rinterpolate_float_t * RESTRICT const r);

void rinterpolate_make_presearch(struct rinterpolate_table_t * RESTRICT const table);

#ifdef RINTERPOLATE_CACHE
void rinterpolate_resize_cache(struct rinterpolate_table_t * RESTRICT const table,
                               const rinterpolate_counter_t cache_length);
#endif



rinterpolate_counter_t rinterpolate_add_new_table(
    struct rinterpolate_data_t * RESTRICT const rinterpolate_data,
    const rinterpolate_float_t * RESTRICT const table,
    const rinterpolate_counter_t n,
    const rinterpolate_counter_t d,
    const rinterpolate_counter_t line_length,
    const rinterpolate_counter_t cache_length);


void rinterpolate_free_hypertable(struct rinterpolate_hypertable_t * RESTRICT hypertable);

#endif//RINTERPOLATE_PROTOTYPES_H
