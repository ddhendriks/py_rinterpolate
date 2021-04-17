#pragma once
#ifndef RINTERPOLATE_INTERNAL_H
#define RINTERPOLATE_INTERNAL_H


/*************************************************************
 * librinterpolate's internal macros
 *************************************************************/

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (1)
#endif

/* Convert a C (integer) expression to rinterpolate_Boolean_t type */
#ifndef Boolean_
#define Boolean_(EXPR) ((EXPR) ? (TRUE) : (FALSE))
#endif

/* fast bit-shift equivalent to pow(2,A) but for integers */
#define Intger_power_of_two(A) (1<<(A))

/* macros to define less and more than operations */
#define Less_than(A,B) ((A)<(B))
#define More_than(A,B) ((A)>(B))
#define Max(A,B) __extension__                  \
    ({                                          \
        const Autotype(A) _a = (A);             \
        const Autotype(B) _b = (B);             \
        More_than(_a,_b) ? _a : _b;             \
    })
#define Min(A,B) __extension__                  \
    ({                                          \
        const Autotype(A) _a = (A);             \
        const Autotype(B) _b = (B);             \
        Less_than(_a,_b) ? _a : _b;             \
    })
#define Force_range(A,B,X) __extension__                \
    ({                                                  \
        const Autotype(A) _a = (A);                     \
        const Autotype(B) _b = (B);                     \
        const Autotype(X) _x = (X);                     \
        (unlikely(Less_than(_x,_a))) ? _a :             \
            (unlikely(More_than(_x,_b)) ? _b : _x);     \
    })

#ifndef Is_zero
#define Is_zero(A) (fabs((A))<TINY)
#endif
#ifndef Fequal
#define Fequal(A,B) (Is_zero((A)-(B)))
#endif

#ifdef RINTERPOLATE_DEBUG
extern int rinterpolate_debug;
#  define Iprint(...) if(rinterpolate_debug==TRUE)  \
        fprintf(RINTERPOLATE_STREAM,__VA_ARGS__);
#else
#  define Iprint(...)
#endif // RINTERPOLATE_DEBUG

#undef TINY
#define TINY (DBL_EPSILON)

#ifdef RINTERPOLATE_DEBUG
#  define FLUSH fflush(NULL);
#else
#  define FLUSH /* */
#endif // RINTERPOLATE_DEBUG

/*************************************************************
 * Compiler options
 *************************************************************/
#include "rinterpolate_compiler.h"



#endif // RINTERPOLATE_INTERNAL_H
