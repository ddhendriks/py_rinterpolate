#include "rinterpolate.h"
#ifdef RINTERPOLATE_CACHE
#include "rinterpolate_internal.h"
rinterpolate_Boolean_t rinterpolate_check_cache(
    struct rinterpolate_table_t * RESTRICT const table,
    const rinterpolate_float_t * RESTRICT const x,
    rinterpolate_float_t * RESTRICT const r)
{
    /*
     * Set the cache location
     */
    rinterpolate_Boolean_t match = FALSE;

    /*
     * Now check the cache to see if it matches with the current search.
     *
     * ... but if no cache was previously saved there is no point trying.
     */
    if(table->cache_spin_line == -1 || table->cache_length == 0)
    {
        match = FALSE;
    }
    else
    {
        /*
         * Start the loop
         */
        rinterpolate_counter_t imax =
            table->cache_match_line + table->cache_length;
        rinterpolate_counter_t iloop,iline;
        for(iloop = table->cache_match_line;
            iloop < imax;
            iloop++)
        {
            iline = iloop % table->cache_length;
            /*
             * Which is quicker, using MEMCMP or using a direct comparison?
             * try them!
             */

#ifdef RINTERPOLATE_CACHE_USE_MEMCMP
#ifdef RINTERPOLATE_DEBUG
            {
                Rinterpolate_print("check cache : cf %u\n    x={ ",iline);
                int m;
                for(m=0;m<table->n;m++)
                {
                    Rinterpolate_print("%g%s",*(x+m),m==(table->n-1) ? " }\n":",");
                }
                rinterpolate_float_t * cl = Rinterpolate_cache_param(iline);
                Rinterpolate_print("   c={ ");
                for(m=0;m<table->n;m++)
                {
                    Rinterpolate_print("%g%s",*(cl+m),m==(table->n-1) ? " }\n":",");
                }
                Rinterpolate_print("memcmp = %d\n",
                       memcmp(Rinterpolate_cache_param(iline),x,table->n_float_sizeof));
            }
#endif//RINTERPOLATE_DEBUG
            match = (memcmp(Rinterpolate_cache_param(iline),x,table->n_float_sizeof)==0) ? TRUE : FALSE;
#else // RINTERPOLATE_CACHE_USE_MEMCMP
            rinterpolate_counter_t m;
            rinterpolate_float_t * cacheline = Rinterpolate_cache_param(iline);
            match = TRUE;
            Rinterpolate_print("start of for loop\n");
            for(m=0; m<table->n; m++)
            {
                if(!Fequal(x[m],cacheline[m]))
                {
                    match = FALSE;
                    break;
                }
            }

            Rinterpolate_print("end of for loop\n");

#endif // RINTERPOLATE_CACHE_USE_MEMCMP

            Rinterpolate_print("Post Match = %u\n",match);

            if(match==TRUE)
            {
                /*
                 * cache matches at line iline so
                 * set the interpolation result directly from
                 * the cache
                 */
                memcpy(r,RINTERPOLATE_CACHE_RESULT(iline),table->d_float_sizeof);

                Rinterpolate_print("cache match at line iline=%u (iloop=%u loop start %u)\n",iline,iloop,table->cache_match_line);

                /*
                 * Save the position of the match for
                 * next time so we start searching at the match
                 * rather than the beginning of the cache
                 */
                table->cache_match_line = iline;

                /* skip everything else */
                goto cache_match;
            }
        }
    }

cache_match:
    Rinterpolate_print("return match = %u\n",match);
    return match;
}
#endif//RINTERPOLATE_CACHE
