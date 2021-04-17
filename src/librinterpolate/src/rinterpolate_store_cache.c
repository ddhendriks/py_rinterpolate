#include "rinterpolate.h"
#include "rinterpolate_internal.h"

#ifdef RINTERPOLATE_CACHE
void rinterpolate_store_cache(struct rinterpolate_table_t * RESTRICT const table,
                              const rinterpolate_float_t * RESTRICT const x,
                              const rinterpolate_float_t * RESTRICT const r
    )
{
    /* use the next line of the cache */
    table->cache_spin_line++;

    /* avoid falling off the end of the cache */
    table->cache_spin_line =
        table->cache_spin_line % table->cache_length;

    /* insert data : NB memcpy is definitely faster than a loop */
    memcpy(Rinterpolate_cache_param(table->cache_spin_line),x,table->n_float_sizeof);
    memcpy(RINTERPOLATE_CACHE_RESULT(table->cache_spin_line),r,table->d_float_sizeof);
}
#endif // RINTERPOLATE_CACHE
