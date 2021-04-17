#include "rinterpolate.h"
#include "rinterpolate_internal.h"
/*
 * Given a hypertable struct, free everything in it.
 */

void rinterpolate_free_hypertable(struct rinterpolate_hypertable_t * RESTRICT hypertable)
{
    Safe_free(hypertable->data);
    Safe_free(hypertable->f);
    Safe_free(hypertable->sum);
}
