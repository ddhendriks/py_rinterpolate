#include "rinterpolate.h"
#include "rinterpolate_internal.h"

void rinterpolate_search_table(
    struct rinterpolate_table_t * RESTRICT const table,
    const rinterpolate_float_t * RESTRICT const x
    )
{
    rinterpolate_counter_t j;
#ifdef RINTERPOLATE_DEBUG
    Rinterpolate_print("search table for span of { ");
    for(j=0;j<table->n;j++)
    {
        Rinterpolate_print("x[%u]=%g%s",j,x[j],j!=(table->n-1)?", ":" }\n");
    }
    FLUSH;
#endif
#if !defined RINTERPOLATE_PRESEARCH || defined RINTERPOLATE_DEBUG
    rinterpolate_counter_t g = table->line_length*(table->l-1);
#endif
    struct rinterpolate_hypertable_t * hypertable = table->hypertable;
    rinterpolate_counter_t * sum = hypertable->sum;

    /*
     * Clear the sum array
     */
    memset(hypertable->sum,
           0,
           table->sum_sizeof);

    for(j=0;j<table->n;j++)
    {
        /*
         * limit the value of our given parameter x[j] to the range we have
         * and save to the parameter v
         */
        rinterpolate_counter_t b = table->varcount[j];
#ifdef RINTERPOLATE_PRESEARCH
        const rinterpolate_float_t * RESTRICT tpre =
            table->presearch[j];
#endif
#ifdef RINTERPOLATE_POINTER_ARITHMETIC_J_LOOP
#ifdef RINTERPOLATE_PRESEARCH
        const rinterpolate_float_t v = Force_range(*tpre,*(tpre+b-1),*(x+j));
#else
        const rinterpolate_float_t * tj = table->data + j;
        const rinterpolate_float_t v = Max(*tj,Min(*(tj+g),*(x+j)));
#endif
        const rinterpolate_counter_t k = *(table->steps+j);
#else
#ifdef RINTERPOLATE_PRESEARCH
        const rinterpolate_float_t v = Max(tpre[0],Min(tpre[b-1],x[j]));
#else
        const rinterpolate_float_t v = Max(table->data[j],Min(table->data[g+j],x[j]));
#endif
        const rinterpolate_counter_t k = table->steps[j];
#endif

#ifdef RINTERPOLATE_DEBUG
        if(rinterpolate_debug)
        {
            Rinterpolate_print("Construct variable %u hypertable position\n",
                   j);
            FLUSH;

            /*
             * Check if the parameter value exceeds the end of the
             * table value
             */
            if(x[j] - DBL_EPSILON > table->data[g+j])
            {
                printf("WARNING : parameter %u is %g which exceeds (by %g cf. TINY = %g DBL_EPSILON = %g) the maximum possible which is %g\n",
                       j,
                       x[j],
                       x[j] - table->data[g+j],
                       TINY,
                       DBL_EPSILON,
                       table->data[g+j]);
            }
            FLUSH;
        }
#endif
        /*
         * Now we can guess the parameter value appropriate for us:
         * a and b are the binary search limits, start at a=0
         * and b=varcount[j] (the max possible value, set above)
         */
        rinterpolate_counter_t a = 0;

        if(likely(b>1))
        {
            /*
             * Binary search blatantly stolen (well, with permission)
             * from Evert Glebbeek's code (thanks Evert!)
             */
#ifndef RINTERPOLATE_PRESEARCH
            const rinterpolate_counter_t i = table->line_length * k;
#endif
            /*
             * choose your search method
             * https://arxiv.org/pdf/1506.08620.pdf
             *
             * BINARY_SEARCH is the best so far:
             *
             * Test time (s):
             *
             * BINARY_SEARCH 10.68
             * QUADRATIC_SEARCH 11.03 (inaccurate)
             * PULVER_SEARCH 11.95
             * DIRECT_SEARCH 44.34
             */

#define BINARY_SEARCH
//#define PULVER_SEARCH
//#define DIRECT_SEARCH
//#define QUADRATIC_SEARCH

#ifdef BINARY_SEARCH
            while(likely(b - a > 1))
            {
                /*
                 * The following three are equivalent, but the
                 * bit shift is fastest.
                 */
                //c = ( a + b ) / 2;
                //c = a + (b - a) / 2;//use this in case of overflow (unlikely!)
                const rinterpolate_counter_t c = (a+b)>>1; // bit shift

#ifdef RINTERPOLATE_POINTER_ARITHMETIC_J_LOOP
#ifdef RINTERPOLATE_PRESEARCH
                if(equally_likely(v > *(tpre+c))) a = c;
#else
                if(equally_likely(v > *(tj+c*i))) a = c;
#endif
#else
#ifdef RINTERPOLATE_PRESEARCH
                if(equally_likely(v > tpre[c])) a = c; // u=table->data[c*i+j]
#else
                if(equally_likely(v > table->data[c*i+j])) a = c; // u=table->data[c*i+j]
#endif
#endif
                else b = c; // if(LESS_OR_EQUAL(v,u)) // obviously!
            }
#endif // BINARY_SEARCH

            Rinterpolate_print("Binary search : indices a=%u b=%u : vars %g < v=%g < %g\n",
                   a,
                   b,
                   *(tpre+a),
                   v,
                   *(tpre+b));

//#include "rinterpolate_other_searchers.h"

            /* calculate interpolation factor (nasty, sorry...) */
#ifdef RINTERPOLATE_POINTER_ARITHMETIC_J_LOOP
#ifdef RINTERPOLATE_PRESEARCH
            const rinterpolate_float_t u = *(tpre+a);
            *(hypertable->f+j) = (v - u)/( *(tpre + b) - u);
#else
            const rinterpolate_float_t u = *(tj+a*i);
            *(hypertable->f+j) = (v - u)/( *(tj + b*i) - u);
#endif//RINTERPOLATE_PRESEARCH
#else//RINTERPOLATE_POINTER_ARITHMETIC_J_LOOP
#ifdef RINTERPOLATE_PRESEARCH
            const rinterpolate_float_t u = tpre[a];
            hypertable->f[j] = (v - u)/(tpre[b] - u);
#else
            const rinterpolate_float_t u = table->data[a*i+j];
            hypertable->f[j] = (v - u)/(table->data[b*i+j] - u);
#endif//RINTERPOLATE_PRESEARCH
#endif//RINTERPOLATE_POINTER_ARITHMETIC_J_LOOP
        }
        else
        {
#ifdef RINTERPOLATE_POINTER_ARITHMETIC_J_LOOP
            *(hypertable->f+j) = 0.0;
#else
            hypertable->f[j] = 0.0; // only one value to choose from! (the low value)
#endif
        }

        const rinterpolate_counter_t c = Intger_power_of_two(table->n-1-j);
        a *= k;
        b *= k;
        //rinterpolate_counter_t tmp;

        /* loop over lines of the hypertable */
        rinterpolate_counter_t m;
        for(m = 0 ; m < table->hypertable_length; m++)
        {
            /*
             * (m&c)/c is 0 for a, 1 for b
             * in fact, for line 1 it makes (for n=3) a table
             * 0 0 0
             * 0 0 1
             * 0 1 0
             * 0 1 1
             * 1 0 0
             * 1 0 1
             * 1 1 0
             * 1 1 1
             * as required!
             */

            /*
             * Add up coordinate of the line: this is the number
             * of the line in table, which will later be set in
             * interpolate_table.
             *
             * NB this used to be an if((m&c)/c==0) but of course
             * if m&c/c==0 then m&c==0 as well. Then why have an
             * if at all? Just use the result of the comparison
             * without any branching.
             *
             * We also used to use a temporary variable : this is
             * not required if we use the ternary operator.
             */
            //tmp = ((m&c)==0);
            //sum[i] += tmp*a+(1-tmp)*b;

            /* same thing without a temporary variable */
            Rinterpolate_print("DSUM(%u)=%u += (%u%%%u)==0? %u : %u -> ",
                   m,sum[m],m,c,a,b);
            sum[m] += ((m&c)==0 ? a : b);
            Rinterpolate_print("%u\n",sum[m]);
        }
    }

    /* watch for table overrun */
    for(j=0; j<table->hypertable_length; j++)
    {
        if(unlikely(hypertable->sum[j]>table->l))
            hypertable->sum[j] = hypertable->sum[j]%table->l;
    }
}
