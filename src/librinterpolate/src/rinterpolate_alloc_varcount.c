#include "rinterpolate.h"
#include "rinterpolate_internal.h"

void rinterpolate_alloc_varcount(struct rinterpolate_table_t * RESTRICT const table)
{
    table->varcount = Rinterpolate_calloc(table->n,sizeof(rinterpolate_counter_t));
#ifdef RINTERPOLATE_ALLOC_CHECKS
    if(unlikely(table->varcount==NULL))
    {
        rinterpolate_error(RINTERPOLATE_CALLOC_FAILED,
                           "Error allocating varcount %u in interpolate\n",
                           table->parent,
                           table->table_number);
    }
#endif
    /* fast counting method! */
    rinterpolate_counter_t b = table->l;
    rinterpolate_counter_t j;
    for(j=0; j<table->n; j++)
    {
#ifdef RINTERPOLATE_DEBUG
        Rinterpolate_print("Interpolate debug: varcount[%u]=%u, b=%u, steps[%u]=%u\n",
               j,table->varcount[j],b,j,table->steps[j]);
        FLUSH;
#endif
        table->varcount[j] = b/table->steps[j];
        b = table->steps[j];
    }
}
