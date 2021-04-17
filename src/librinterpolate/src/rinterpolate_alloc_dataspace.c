#include "rinterpolate.h"
#include "rinterpolate_internal.h"
/*
 * Allocate memory for rinterpolate: to be called ONCE per process only.
 */

#ifdef RINTERPOLATE_DEBUG
int rinterpolate_debug;
#endif

rinterpolate_counter_t rinterpolate_alloc_dataspace(struct rinterpolate_data_t ** RESTRICT const r)
{
    if(*r != NULL)
    {
        rinterpolate_error(RINTERPOLATE_ALLOCATE_OVER,
                           "Attempted to allocate rinterpolate a non-NULL pointer\n",
                           *r);
        return RINTERPOLATE_ALLOCATE_OVER;
    }
    else
    {
        /*
         * Allocate space
         */
        *r = Rinterpolate_calloc(1,sizeof(struct rinterpolate_data_t));
        return 0;
    }
}

