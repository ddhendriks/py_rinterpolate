#pragma once
#ifndef RINTERPOLATE_COMPILER_H
#define RINTERPOLATE_COMPILER_H

/*
 * macros which depend on the compiler, for librinterpolate
 */
#ifndef Autotype
#if defined __GNUC__ && __GNUC__ > 4 && __GNUC_MINOR__ > 9
#define Autotype(X) __auto_type
#endif // __GNUC__
#endif

#ifndef Autotype
#define Autotype(X) typeof(X)
#endif

/* 
 * "safe" freeing of memory via the Safe_free macro,
 * which enforces a NULL in the pointer after a call 
 * to free. 
 * Note tha the PTR must actually be a pointer (i.e. an lvalue), 
 * not an expression.
 */
#ifndef Safe_free
#define Safe_free(PTR)                                                  \
    if(likely((PTR)!=NULL))                                             \
    {                                                                   \
        free(PTR);                                                      \
        (PTR)=NULL;                                                     \
    };
#endif

/* if we're given an ALIGNSIZE, use it */
#ifndef Aligned
#ifdef ALIGNSIZE
#define Aligned __attribute__ ((aligned (ALIGNSIZE)))
#else
#define Aligned
#endif//ALIGNSIZE
#endif//Aligned

/* restrict if possible */
#ifndef RESTRICT
#if defined(__GNUC__) && ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#   define RESTRICT __restrict
#elif defined(_MSC_VER) && _MSC_VER >= 1400
#   define RESTRICT __restrict
#endif
#endif
#ifndef RESTRICT
#   define RESTRICT
#endif


#ifdef __GNUC__
#ifndef likely
#  define likely(x)      __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#  define unlikely(x)    __builtin_expect(!!(x), 0)
#endif
#if __GNUC__ >= 9
#  define equally_likely(x) __builtin_expect_with_probability(!!(x),0,0.5)
#else
#  define equally_likely(x) (x)
#endif
#ifndef prefetch
#  define prefetch(...) __builtin_prefetch(__VA_ARGS__)
#endif

#  if __GNUC__ >=4
#    define MAYBE_UNUSED __attribute__ ((unused)) 
#  endif

#else // __GNUC__
#ifndef likely
#define likely(x) (x)
#endif
#ifndef unlikely
#define unlikely(x) (x)
#endif
#ifndef equally_likely
#define equally_likely(x) (x)
#endif
#ifndef prefetch
#define prefetch(...) /* prefetch: do nothing */
#endif


    
#endif // __GNUC__

#ifndef MAYBE_UNUSED
#  define MAYBE_UNUSED
#endif

#ifndef Pure_function
#if defined __GNUC__ && __GNUC__ >=3
#define Pure_function __attribute__((pure))
#endif
#endif
#ifndef Constant_function
#if defined __GNUC__ && __GNUC__ >=3
#define Constant_function __attribute__((const))
#endif
#endif
#ifndef No_return
#if defined __GNUC__ && __GNUC__ >=4
#define No_return __attribute__((noreturn))
#endif
#endif
#ifndef Gnu_format_args
#if (__GNUC__ ==4 && __GNU_MINOR__>=7) || __GNUC__ > 4
#define Gnu_format_args(...) __attribute__((format (gnu_printf,__VA_ARGS__)))
#endif
#endif


#ifndef Constant_function
#define Constant_function
#endif
#ifndef Pure_function
#define Pure_function
#endif
#ifndef No_return
# define No_return
#endif
#ifndef Gnu_format_args
# define Gnu_format_args(...) 
#endif



#endif // RINTERPOLATE_COMPILER_H
