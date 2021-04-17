#include "rinterpolate.h"
#include <stdarg.h>
#include "rinterpolate_internal.h"

void No_return Gnu_format_args(2,4) rinterpolate_error(rinterpolate_counter_t errnum,
                                                       char * RESTRICT format,
                                                       struct rinterpolate_data_t * RESTRICT const rinterpolate_data,
                                                       ...)
{
    /*
     * Report an error in librinterpolate and exit
     */
    va_list ap;
    va_start(ap,rinterpolate_data);
    fprintf(stderr,
            "librinterpolate error %u: ",
            errnum);
    fprintf(stderr,format,ap);
    va_end(ap);
    exit(errnum);
}
