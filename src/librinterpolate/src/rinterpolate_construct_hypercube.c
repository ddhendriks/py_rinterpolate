#include "rinterpolate.h"
#include "rinterpolate_internal.h"

void rinterpolate_construct_hypercube(struct rinterpolate_table_t * RESTRICT const table)
{
    /*
     * Construct hypercube
     */

    struct rinterpolate_hypertable_t * hypertable = table->hypertable;

    /* easily vectorized loop */
    rinterpolate_counter_t i;
    for(i=0;i<table->hypertable_length;i++)
    {
        Rinterpolate_print("SUM %u was %u now ",i,hypertable->sum[i]);
        hypertable->sum[i] *= table->line_length;
        Rinterpolate_print("%u (lnl = %u)\n",hypertable->sum[i],table->line_length);
    }

    rinterpolate_counter_t k = 0;
    for(i=0;i<table->hypertable_length;i++)
    {
#ifdef RINTERPOLATE_DEBUG
        if(rinterpolate_debug==TRUE)
        {
            Rinterpolate_print("memcpy k=%u i=%u : from %p to %p ( = %p + %u )\n",
                               k,
                               i,
                               hypertable->data + k,
                               table + hypertable->sum[i],
                               table,
                               hypertable->sum[i]
                );
        }
#endif//RINTERPOLATE_DEBUG

        memcpy(hypertable->data + k,
               table->data + hypertable->sum[i],
               table->line_length_sizeof);

        k += table->line_length;

#ifdef RINTERPOLATE_DEBUG
        {
            Rinterpolate_print("Line %u : ",i);FLUSH;
            rinterpolate_counter_t j;
            for(j=0;j<table->n;j++)
            {
                Rinterpolate_print("% 3.3e ",*(hypertable->data+i*table->line_length+j));FLUSH;
            }
            Rinterpolate_print(" | ");FLUSH;
            for(j=table->n;j<table->line_length;j++)
            {
                Rinterpolate_print("% 3.3e ",*(hypertable->data+i*table->line_length+j));FLUSH;
            }
            Rinterpolate_print(" %u/%u\n",i,table->hypertable_length-1);FLUSH;
        }
#endif

    }

#ifdef RINTERPOLATE_DEBUG
    {
        Rinterpolate_print("done hypertable\n");
        Rinterpolate_print("Interpolation (f) factors: ");
        rinterpolate_counter_t j;
        for(j=0;j<table->n;j++)
        {
            Rinterpolate_print("% 3.3e ",hypertable->f[j]);
        }
        Rinterpolate_print("\n");
    }
#endif
}
