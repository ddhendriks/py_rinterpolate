#include "rinterpolate.h"
#include "rinterpolate_internal.h"

rinterpolate_counter_t rinterpolate_add_new_table(
    struct rinterpolate_data_t * RESTRICT const rinterpolate_data,
    const rinterpolate_float_t * RESTRICT const data,
    const rinterpolate_counter_t n,
    const rinterpolate_counter_t d,
    const rinterpolate_counter_t l,
    const rinterpolate_counter_t cache_length
    )
{
    /*
     * Increase size of list of table_numbers, and store the appropriate pointer
     * to the table of data
     */
    const rinterpolate_counter_t table_number =
        rinterpolate_data->number_of_interpolation_tables;

    rinterpolate_data->number_of_interpolation_tables++;
    
    rinterpolate_data->tables =
        Rinterpolate_realloc(rinterpolate_data->tables,
                             sizeof(struct rinterpolate_table_t *) * rinterpolate_data->number_of_interpolation_tables);

    struct rinterpolate_table_t * table =
        Rinterpolate_malloc(sizeof(struct rinterpolate_table_t));
    rinterpolate_data->tables[table_number] = table;
    
    /*
     * Set data pointers and table number
     */
    table->parent = rinterpolate_data;
    table->data = (rinterpolate_float_t *) data;
    table->table_number = table_number;

    /*
     * Set counters
     */
    table->n = n;
    table->d = d;
    table->l = l;
    table->line_length = n + d;
    table->hypertable_length = Intger_power_of_two(n);
#ifdef RINTERPOLATE_CACHE
    table->cache_length = cache_length;
#endif
#ifndef RINTERPOLATE_PRESEARCH
    table->g = table->line_length*(table->l-1); // start of the final line of the table
#endif
    
    /*
     * Set sizes
     */
    table->d_float_sizeof =  sizeof(rinterpolate_float_t) * d;
    table->n_float_sizeof = sizeof(rinterpolate_float_t) * n;
    table->line_length_sizeof = table->d_float_sizeof + table->n_float_sizeof;
    table->sum_sizeof = sizeof(rinterpolate_counter_t) * table->hypertable_length; 

    /* make various data */
    rinterpolate_make_steps(table);        
    rinterpolate_alloc_varcount(table);
#ifdef RINTERPOLATE_CACHE
    rinterpolate_alloc_cacheline(table);
#endif//RINTERPOLATE_CACHE
#ifdef RINTERPOLATE_PRESEARCH
    rinterpolate_make_presearch(table);
#endif//RINTERPOLATE_PRESEARCH

    /* make hypertable */
    rinterpolate_alloc_hypertable(table);
    
    return table_number;
}
