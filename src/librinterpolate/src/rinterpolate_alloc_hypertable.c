#include "rinterpolate.h"
#include "rinterpolate_internal.h"

void rinterpolate_alloc_hypertable(struct rinterpolate_table_t * RESTRICT const table)
{
    /* make space for hypertable */
    table->hypertable = Rinterpolate_malloc(sizeof(struct rinterpolate_hypertable_t));

#ifdef RINTERPOLATE_DEBUG
    Rinterpolate_print("Interpolate: memory allocation\n");FLUSH;
#endif
    table->hypertable->table = table;
    table->hypertable->data = Rinterpolate_malloc(table->hypertable_length*table->line_length_sizeof);
    table->hypertable->f = Rinterpolate_malloc(table->n_float_sizeof);
    table->hypertable->sum = Rinterpolate_calloc(1,table->sum_sizeof);

#ifdef RINTERPOLATE_DEBUG
    Rinterpolate_print("MALLOC data at %p size %zu, f at %p size %zu, sum at %p size %zu\n",
           table->hypertable->data,table->hypertable_length*table->line_length_sizeof,
           table->hypertable->f,table->n_float_sizeof,
           table->hypertable->sum,table->sum_sizeof);
#endif
#ifdef RINTERPOLATE_ALLOC_CHECKS
    if(unlikely((table->hypertable->data==NULL)||
                (table->hypertable->f==NULL)||
                (table->hypertable->sum==NULL)))
    {
        rinterpolate_error(RINTERPOLATE_CALLOC_FAILED,
                           "Error allocating f or sum in rinterpolate_alloc_hypertable\n",
                           table->parent);
    }
#endif//RINTERPOLATE_ALLOC_CHECKS

#ifdef RINTERPOLATE_DEBUG
    Rinterpolate_print("table->hypertable->data alloc 2 * %u * %zu\n",table->hypertable_length,table->line_length_sizeof);
#endif

#ifdef RINTERPOLATE_USE_REALLOC
    /* remember to clear sum if it wasn't set by calloc */
    memset(table->hypertable->sum,0,table->sum_sizeof);
#endif // RINTERPOLATE_USE_REALLOC

}
