#include "rinterpolate.h"
#include "rinterpolate_internal.h"
/*
 * rinterpolate_free_data
 *
 * Given a rinterpolate_data struct, free everything in it.
 */
static void rinterpolate_free_table(struct rinterpolate_table_t * const table);

void rinterpolate_free_data(struct rinterpolate_data_t * RESTRICT const rinterpolate_data)
{
    if(rinterpolate_data)
    {
        rinterpolate_counter_t i;
        for(i=0;i<rinterpolate_data->number_of_interpolation_tables;i++)
        {
            rinterpolate_free_table(rinterpolate_data->tables[i]);
            Safe_free(rinterpolate_data->tables[i]);
        }
        Safe_free(rinterpolate_data->tables);
        rinterpolate_data->number_of_interpolation_tables=0;
    }
}

static void rinterpolate_free_table(struct rinterpolate_table_t * const table)
{

#ifdef RINTERPOLATE_CACHE
    Safe_free(table->cache);
#endif//RINTERPOLATE_CACHE
#ifdef RINTERPOLATE_PRESEARCH
    rinterpolate_counter_t j;
    for(j=0;j<table->presearch_n;j++)
    {
        Safe_free(table->presearch[j]);
    }
    Safe_free(table->presearch);
#endif//RINTERPOLATE_PRESEARCH
    Safe_free(table->steps);
    Safe_free(table->varcount);
    rinterpolate_free_hypertable(table->hypertable);
    Safe_free(table->hypertable);
}
