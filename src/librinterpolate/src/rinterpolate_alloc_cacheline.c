#include "rinterpolate.h"
#include "rinterpolate_internal.h"

#ifdef RINTERPOLATE_CACHE

void rinterpolate_alloc_cacheline(struct rinterpolate_table_t * RESTRICT const table)
{
    table->cache_match_line = 0;
    table->cache_spin_line  = -1;

#ifdef RINTERPOLATE_DEBUG
    Rinterpolate_print("Allocated new cache array for table_id=%u\n",
           table->table_number);
#endif
    /*
     * Allocate cache space for this interpolation table
     */
    table->cache =
        Rinterpolate_calloc(table->line_length*table->cache_length,
                            sizeof(rinterpolate_float_t));

#ifdef RINTERPOLATE_ALLOC_CHECKS
    if(unlikely(table->cache==NULL))
    {
        rinterpolate_error(RINTERPOLATE_CALLOC_FAILED,
                           "Failed to alloc cache \n",
                           table->parent);
    }
#endif

}
#endif //RINTERPOLATE_CACHE
