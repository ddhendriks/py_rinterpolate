#ifdef __TEST_RINTERPOLATE__
#include "rinterpolate.h"
#include "rinterpolate_internal.h"
#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif //_MSC_VER
#define _HAVE_RDTSC
typedef unsigned long long ticks;
ticks getticks(void);
#define _random_number ((rinterpolate_float_t)rand()/RAND_MAX)
#include <time.h>

/*
 * Test program for librinterpolate
 */

#define FIXED_TESTS 1
#define RANDOM_TESTS 1
#define SPIN_TESTS 1

#define Report(TEST,TICKS)                              \
    printf("Report: %20s : %8.3f\n",TEST,TICKS);

void f(const rinterpolate_float_t u,
       const rinterpolate_float_t v,
       rinterpolate_float_t * RESTRICT x);

int main (int argc,
          char **  argv)
{
    struct rinterpolate_data_t * rinterpolate_data = NULL;

    /*
     * Verbosity.
     * 1 is normal.
     * 2 is more.
     */
    const rinterpolate_counter_t vb = 1;


#define TICKSCALE 100000000.0
    /*
     * Resolution multiplier (>=2).
     * NB errors are likely to be tiny when > 1000
     * (1000)
     */
#define NRES 1000

    /*
     * Number of tests to perform (10000000)
     */
#define NTESTS 10000000
//#define NTESTS 10


    /*
     * Number of tests when checking the cache
     */
#define NSPINTESTS 1000000
//#define NSPINTESTS 10


        /*
         * Use current time as seed for random generator
         */
    srand(time(0));

    /*
     * Number of parameters
     */
#define N 2
    /*
     * Number of data
     */
#define D 3

    /*
     * Number of lines of data types 1,2
     */
#define L1 (1*NRES)
#define L2 (4*NRES)

    /* length of each line (in doubles) i.e. N+D */
#define ND ((N)+(D))

    /* total number of lines */
#define L ((L1)*(L2))

#define COMPARE if(vb>=2)                                               \
                {                                                       \
                    printf("f(%g,%g)\n",x[0],x[1]);                     \
                    printf("Expected : %g %g %g\n",rr[0],rr[1],rr[2]);  \
                    printf("Got      : %g %g %g\n", r[0], r[1], r[2]);  \
                }


    rinterpolate_float_t table[ND*L];
    rinterpolate_counter_t i,j;

    /*
     * IDX = offset index of position i,j
     */
#define IDX(I,J) (((I)*L2+(J))*ND)

    /*
     * Fill the data table
     */
    for(i=0;i<L1;i++)
    {
        const rinterpolate_float_t u = (1.0*i)/(1.0*(L1-1));
        for(j=0;j<L2;j++)
        {
            const rinterpolate_float_t v = (1.0*j)/(1.0*(L2-1));
            const int offset = IDX(i,j);
            rinterpolate_float_t x[D];
            f(u,v,x);

            table[offset+0] = u;
            table[offset+1] = v;
            table[offset+2] = x[0];
            table[offset+3] = x[1];
            table[offset+4] = x[2];
        }
    }

    /*
     * Show data table
     */
    if(0)
    {
        for(i=0;i<L;i++)
        {
            rinterpolate_counter_t offset = i * ND;
            printf("%4u -> %4u : ",i,offset);
            for(j=0;j<ND;j++)
            {
                printf("%g ",table[offset+j]);
            }
            printf("\n");
        }
    }
    /*
     * Arrays for the interpolation location and
     * interpolated data.
     */
    rinterpolate_float_t x[N],r[D];

    /*
     * Allocate data space
     */
    rinterpolate_counter_t status = rinterpolate_alloc_dataspace(&rinterpolate_data);
    rinterpolate_build_flags(rinterpolate_data);

#ifdef RINTERPOLATE_DEBUG
    rinterpolate_debug = TRUE;
#endif

    if(status != 0)
    {
        printf("alloc data status != 0 = %u\n",status);
        fflush(NULL);
        exit(status);
    }

    /*
     * Calculations of the maximum difference between
     * the exact functions and the interpolated functions.
     */
    rinterpolate_float_t diffmax = 0.0;
#define DIFF(X,Y) (fabs(X-Y)/Min(fabs(X),fabs(Y)))
#define MAXDIFF                                 \
    Max(Max(diffmax,                            \
            DIFF(r[0],rr[0])),                  \
        Max(DIFF(r[1],rr[1]),                   \
            DIFF(r[2],rr[2])));

    if(status==0)
    {
        ticks tstart;

        /*
         * Fixed x
         */
        for(i=0;i<N;i++)
        {
            x[i] = _random_number;
        }

        rinterpolate_float_t t_cache = 0, t_nocache = 0;

        if(FIXED_TESTS)
        {
            printf("\nFixed input tests\n");
            /* without cache */
            diffmax = 0.0;
            tstart = getticks();
            for(i=0;i<NTESTS;i++)
            {
                rinterpolate(table,
                             rinterpolate_data,
                             N,
                             D,
                             L,
                             x,
                             r,
                             0);
                rinterpolate_float_t rr[D];
                f(x[0],x[1],rr);
                diffmax = MAXDIFF;
                COMPARE;
            }
            t_nocache = (getticks() - tstart)/TICKSCALE;
            printf("%7s cache : %8.3f, maxdiff %6.4f %%\n","Without",t_nocache,100.0*diffmax);
            Report("Fixed nocache",t_nocache);

            /* with cache */
            tstart = getticks();
            diffmax = 0.0;
            for(i=0;i<NTESTS;i++)
            {
                rinterpolate(table,
                             rinterpolate_data,
                             N,
                             D,
                             L,
                             x,
                             r,
                             5);
                rinterpolate_float_t rr[D];
                f(x[0],x[1],rr);
                diffmax = MAXDIFF;
                COMPARE;
            }
            t_cache = (getticks() - tstart)/TICKSCALE;
            printf("%7s cache : %8.3f, maxdiff = %6.4f %%\n","With",t_cache,100.0*diffmax);
            printf("Cache speed up : %5.3f %%\n",
                   100.0*((rinterpolate_float_t)t_nocache - (rinterpolate_float_t)t_cache)/
                   Min((rinterpolate_float_t)t_cache,(rinterpolate_float_t)t_nocache));
            Report("Fixed cache",t_cache);
        }


        if(RANDOM_TESTS)
        {
            printf("\nRandom input tests\n");
            /* without cache */
            diffmax = 0.0;
            tstart = getticks();
            for(i=0;i<NTESTS;i++)
            {
                for(j=0;j<N;j++)
                {
                    x[j] = _random_number;
                }
                rinterpolate(table,
                             rinterpolate_data,
                             N,
                             D,
                             L,
                             x,
                             r,
                             0);
                rinterpolate_float_t rr[D];
                f(x[0],x[1],rr);
                diffmax = MAXDIFF;
                COMPARE;
            }
            t_nocache = (getticks() - tstart)/TICKSCALE;
            printf("%7s cache : %8.3f, maxdiff %6.4f %%\n","Without",t_nocache,100.0*diffmax);
            Report("Random nocache",t_nocache);

            /* with cache */
            diffmax = 0.0;
            tstart = getticks();
            for(i=0;i<NTESTS;i++)
            {
                x[0] = _random_number;
                x[1] = _random_number;
                rinterpolate(table,
                             rinterpolate_data,
                             N,
                             D,
                             L,
                             x,
                             r,
                             5);

                rinterpolate_float_t rr[D];
                f(x[0],x[1],rr);
                diffmax = MAXDIFF;
                COMPARE;
            }
            t_cache = (getticks() - tstart)/TICKSCALE;
            Report("Random cache",t_cache);

            printf("%7s cache : %8.3f, maxdiff = %6.4f %%\n","With",t_cache,100.0*diffmax);
            printf("Cache speed up : %6.4f %%\n",
                   100.0*((rinterpolate_float_t)t_nocache - (rinterpolate_float_t)t_cache)/
                   Min((rinterpolate_float_t)t_cache,(rinterpolate_float_t)t_nocache));
        }



        if(SPIN_TESTS)
        {
            const rinterpolate_counter_t n = 5;
            printf("Spun input (n=%u) tests \n",n);
            double ** xx = malloc(n*sizeof(rinterpolate_float_t*));
            for(i=0;i<n;i++)
            {
                int kk;
                *(xx+i) = malloc(n*sizeof(rinterpolate_float_t));
                for(kk=0;kk<N;kk++)
                {
                    xx[i][kk] = _random_number;
                }
            }

            rinterpolate_counter_t ncache;
            const rinterpolate_counter_t maxncache = n*2;
            for(ncache=0; ncache < maxncache; ncache++)
            {
                diffmax = 0.0;
                tstart = getticks();
                for(i=0;i<NSPINTESTS;i++)
                {
                    memcpy(x,xx[i%n],sizeof(rinterpolate_float_t)*N);
                    rinterpolate(table,
                                 rinterpolate_data,
                                 N,
                                 D,
                                 L,
                                 x,
                                 r,
                                 ncache);
                    rinterpolate_float_t rr[D];
                    f(x[0],x[1],rr);
                    diffmax = MAXDIFF;
                    COMPARE;
                }
                t_cache = (getticks() - tstart)/TICKSCALE;
                if(ncache==0)
                {
                    t_nocache = t_cache;
                }

                printf("%7s cache_length %2u : %8.3f, maxdiff = %6.4f %%\n","With",ncache,t_cache,100.0*diffmax);
                printf("                  Cache speed up : %5.3f %%\n",
                       100.0*((rinterpolate_float_t)t_nocache - (rinterpolate_float_t)t_cache)/
                       Min((rinterpolate_float_t)t_cache,(rinterpolate_float_t)t_nocache));
                char c[100];
                sprintf(c,"Spun %u",ncache);
                Report(c,t_cache);
            }

            for(i=0;i<n;i++)
            {
                free(xx[i]);
            }
            free(xx);
        }
    }

    rinterpolate_free_data(rinterpolate_data);
    Safe_free(rinterpolate_data);

    return 0;
}

/*
 * Function used for testing
 */
void f(const rinterpolate_float_t u,
       const rinterpolate_float_t v,
       rinterpolate_float_t * RESTRICT x)
{
    x[0] = sin(u)*cos(v);
    x[1] = cos(v)*sin(u);
    x[2] = tan(u*v);
}





/*
 * Timer function
 */
ticks getticks(void) {

#ifdef _HAVE_RDTSC
    /*
     * Use inbuilt __rdtsc function as a timer
     */
    return __rdtsc();
#else
    /*
     * Backup: use clock_gettime() to estimate the
     * number of "ticks".
     */
    struct timespec tp;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&tp);
    ticks t = (ticks) (
        ((uint64_t)tp.tv_sec  * (1000000000U * 1e-3 * CPUFREQ))
        +
        ((uint64_t)tp.tv_nsec * (1e-3 * CPUFREQ))
        );
    return t;
#endif // MEMOIZE_HAVE_RDTSC

}




#endif //__TEST_RINTERPOLATE__

typedef int prevent_ISO_warning;
