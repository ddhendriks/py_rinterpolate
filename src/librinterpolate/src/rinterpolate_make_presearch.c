#include "rinterpolate.h"
#include "rinterpolate_internal.h"

#ifdef RINTERPOLATE_PRESEARCH
void rinterpolate_make_presearch(struct rinterpolate_table_t * RESTRICT const table)
{
    /*
     * First time with this table: Find the variable presearch
     */

    /* make table for presearch data for n columns */
    table->presearch = 
        Rinterpolate_malloc(table->n*sizeof(rinterpolate_float_t *));
    table->presearch_n = table->n;

#ifdef RINTERPOLATE_ALLOC_CHECKS
    if(unlikely(table->presearch==NULL))
    {
        rinterpolate_error(RINTERPOLATE_CALLOC_FAILED,
                           "(m|c)alloc failed in interpolate() : presearch\n",
                           table->parent);
    }
#endif

    rinterpolate_counter_t i,j;
    /* loop over columns */
    for(j=0;j<table->n;j++)
    {
        table->presearch[j] =
            Rinterpolate_malloc(table->varcount[j]*
                                sizeof(rinterpolate_float_t));
        rinterpolate_float_t * pline = table->presearch[j];
        
#ifdef RINTERPOLATE_ALLOC_CHECKS
        if(unlikely(table->presearch[j]==NULL))
        {
            rinterpolate_error(RINTERPOLATE_CALLOC_FAILED,
                               "(m|c)alloc failed in interpolate() : presearch/presearch\n",
                               table->parent);
        }
#endif
        
        /* loop over lines allocating the presearch */
        rinterpolate_counter_t step = table->line_length * table->steps[j];
        rinterpolate_float_t * p = (rinterpolate_float_t*)table->data + j;
        for(i=0;i<table->varcount[j];i++)
        {
            pline[i] = *p;
            p += step;
        }        
    }
}
#endif//RINTERPOLATE_PRESEARCH
