#include "rinterpolate.h"
#include "rinterpolate_internal.h"

#ifdef RINTERPOLATE_CACHE

void rinterpolate_resize_cache(struct rinterpolate_table_t * RESTRICT const table,
                               const rinterpolate_counter_t cache_length)
{
    /*
     * Change the size of the rinterpolate_cache to cache_length, which
     * could be zero.
     *
     * Note that this wipes the cache in the process.
     */
    Safe_free(table->cache);
    table->cache_length = cache_length;
    if(cache_length>0)
    {
        rinterpolate_alloc_cacheline(table);
    }
}
#endif // RINTERPOLATE_CACHE    
