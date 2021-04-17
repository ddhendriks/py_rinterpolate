#include "rinterpolate.h"
#include "rinterpolate_internal.h"

void rinterpolate_interpolate(
    const struct rinterpolate_table_t * const table,
    const rinterpolate_float_t * RESTRICT const x MAYBE_UNUSED,
    rinterpolate_float_t * RESTRICT const r)
{
    rinterpolate_float_t  u,v;
    rinterpolate_counter_t n = 0;
    struct rinterpolate_hypertable_t * hypertable = table->hypertable;
    rinterpolate_counter_t g = table->line_length<<(table->n-1);
#ifdef RINTERPOLATE_USE_POINTER_ARITHMETIC
    rinterpolate_float_t Aligned * int_table_k;
    rinterpolate_float_t Aligned * int_table_g;
#endif

    prefetch(hypertable->data,0);
    prefetch(hypertable->f,0);

    {
        rinterpolate_float_t * int_table = hypertable->data;
        rinterpolate_float_t * int_table_n = int_table + table->n;
        while(n < table->n)
        {
            /*
             * Do the interpolation
             */
#ifdef RINTERPOLATE_DEBUG
            Rinterpolate_print("Interpolate n=%u f=%g\n",table->n,hypertable->f[n]);
            FLUSH;
#endif
#ifdef RINTERPOLATE_USE_POINTER_ARITHMETIC
            v = *(hypertable->f+n);
#else
            v = hypertable->f[n];
#endif

            if(likely(v>TINY))
            {
                if(unlikely(v+TINY>1.0))
                {
                    // u=0 v=1: unusual case but easy to calculate (no inner loop required)
#ifdef RINTERPOLATE_DEBUG
                    Rinterpolate_print("u=0 v=1\n");
#endif
                    rinterpolate_float_t *xxx;
                    rinterpolate_counter_t i;
                    for(i=0;i<g;i+=table->line_length)
                    {
                        xxx = int_table_n+i;
                        memcpy(xxx,xxx+g,table->d_float_sizeof);
                    }
                }
                else
                {
                    /*
                     * intermediate cases : the most common, so the most
                     * optimized!
                     */
                    u = 1.0 - v;
                    rinterpolate_counter_t i;
#ifdef RINTERPOLATE_USE_POINTER_ARITHMETIC
                    /*
                     * pointer-based version, with two increments instead of adds,
                     * might be faster?
                     */
                    rinterpolate_float_t *p_kmax;
                    for(i=0; i<g; i+=table->line_length)
                    {
                        int_table_k = int_table_n + i;
                        int_table_g = int_table_k + g;
                        p_kmax = int_table_k + table->d;
                        while(int_table_k < p_kmax)
                        {
                            *int_table_k = u*(*int_table_k) + v*(*(int_table_g++));
                            int_table_k++;
                        }
                    }
#else
                    /* either loop over j or k, but k has fewer
                     * additions, so should be faster */
                    for(i=0; i<g; i+=table->line_length)
                    {
                        const rinterpolate_counter_t kmax=i+table->line_length;
                        rinterpolate_counter_t k;
                        for(k=i+n;k<kmax;k++)
                        {
                            int_table[k] = u*int_table[k] + v*int_table[k+g];
                        }
                    }
#endif // RINTERPOLATE_USE_POINTER_ARITHMETIC
                }
            }
            // else v=0, int_table[k] stays the same
            n++;
            g >>=1; // g/=2;
        }

#ifdef RINTERPOLATE_DEBUG
        Rinterpolate_print("memcopy results\n");
        FLUSH;
#endif

        /*
         * Set the result array
         */
        memcpy(r,int_table_n,table->d_float_sizeof);
    }
}
