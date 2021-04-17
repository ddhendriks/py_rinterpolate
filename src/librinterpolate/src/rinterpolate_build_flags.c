#ifdef __RINTERPOLATE_BUILD_BUILD_FLAGS__

#include "rinterpolate.h"
#include "rinterpolate_internal.h"
#include <stdarg.h>
#include <string.h>

/*
 * Macro to convert a macro to a string
 */ 
#ifndef Stringify
#define Stringify(item) "" #item
#endif

/*
 * Macro to expand a macro and then convert to a string
 */
#ifndef Stringify_macro
#define Stringify_macro(item) (Stringify(item))
#endif

#ifndef Macrotest
/*
 * Macro to test a macro
 */
#define Macrotest(macro)                                                \
    printf("%s is %s\n",                                                \
           "" #macro,                                                   \
           (strcmp("" #macro,                                           \
                   Stringify(macro)) ? "on" : "off" ));
#endif

void rinterpolate_build_flags(struct rinterpolate_data_t * RESTRICT const rinterpolate_data)
{
    Macrotest(RINTERPOLATE_USE_REALLOC);
    Macrotest(RINTERPOLATE_USE_POINTER_ARITHMETIC);
    Macrotest(RINTERPOLATE_POINTER_ARITHMETIC_J_LOOP);
    Macrotest(RINTERPOLATE_DEBUG);
    Macrotest(RINTERPOLATE_DEBUG_SHOW_TABLE);
    Macrotest(RINTERPOLATE_ALLOC_CHECKS);
    Macrotest(RINTERPOLATE_CACHE);
    Macrotest(RINTERPOLATE_CACHE_USE_MEMCPY);
    Macrotest(RINTERPOLATE_CACHE_USE_MEMCMP);
    Macrotest(RINTERPOLATE_PRESEARCH);

}

#endif
