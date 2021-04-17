#include "rinterpolate.h"
#include "rinterpolate_internal.h"

void rinterpolate_make_steps(struct rinterpolate_table_t * RESTRICT const table)
{
    /*
     * First time with this table: Find the variable steps
     */
    table->steps = Rinterpolate_calloc(table->n,sizeof(rinterpolate_counter_t));

#ifdef RINTERPOLATE_ALLOC_CHECKS
    if(unlikely(table->steps==NULL))
    {
        rinterpolate_error(RINTERPOLATE_CALLOC_FAILED,
                           "(m|c)alloc failed in interpolate() : steps/steps_array\n",
                           table->parent);
    }
#endif

    /* loop, find where variables change */
#ifdef RINTERPOLATE_DEBUG
    Rinterpolate_print("Find where vars change (i.e. set steps_array/steps) \n");
#endif

    rinterpolate_counter_t i,j;

    if(table->l<2)
    {
        /* special case : one line of data */
        for(j=0; j<table->n; j++)
        {
            table->steps[j] = 1;
        }
    }
    else
    {
        /* loop over columns */
        for(j=0; j<table->n; j++)
        {
            table->steps[j] = table->l; // fallback

            /* loop over lines */
            for(i=1; i<table->l; i++)
            {
                /*
                Rinterpolate_print("Line %d, item %d/%d : cf %g to prev %g \n",
                       i,
                       j,
                       table->n,
                       table->data[(i-1)*table->line_length + j],
                       table->data[i    *table->line_length + j]);
                */

                /*
                 * Compare this line to the previous,
                 * if different, set the step
                 */
#ifdef RINTERPOLATE_DEBUG
                Rinterpolate_print("cf. data at indices %zu and %zu\n",
                                   (size_t)((i-1)*table->line_length + j),
                                   (size_t)(i    *table->line_length + j));
                Rinterpolate_print("cf. pointers %p and %p\n",
                                   table->data + (size_t)((i-1)*table->line_length + j),
                                   table->data + (size_t)(i    *table->line_length + j));
                Rinterpolate_print("cf. data are %g and %g\n",
                                   table->data[(i-1)*table->line_length + j],
                                   table->data[i    *table->line_length + j]);
#endif

                if(!Fequal(table->data[(i-1)*table->line_length + j],
                           table->data[i    *table->line_length + j]))
                {
                    // change
                    table->steps[j] = i;
                    Rinterpolate_print("SET STEPS for var %u to %u\n",j,table->steps[j]);
                    i=table->l+1; // break i loop
                }
            }
        }
    }
}
