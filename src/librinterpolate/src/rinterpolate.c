#include "rinterpolate.h"
#include "rinterpolate_internal.h"
/*
 * librinterpolate
 *
 * A library to perform n-dimensional linear interpolation on a table
 * of data.
 *
 * The table should be organised (in memory) like this
 *
 * p1[0] p2[0] p3[0] ... d1[0] d2[0] d3[0] d4[0] ...
 * p1[1] p2[1] p3[1] ... d1[1] d2[1] d3[1] d4[1] ...
 * p1[2] p2[2] p3[2] ... d1[2] d2[2] d3[2] d4[2] ...
 *
 * so there are n parameters (p1, p2, p3...) and d data items
 * per data line (d1, d2, d3, d4 ...). There are l lines of data.
 *
 * The parameters should be ordered from low to high values.
 * The parameters should be on a *constant* grid (which does NOT
 * need to be regular).
 *
 * What does this mean?
 *
 * This is a good data table:
 *
 * 0.1 -100 10 ...data...
 * 0.1 -100 25 ...data...
 * 0.1 -100 30 ...data...
 * 0.1  -50 10 ...data...
 * 0.1  -50 25 ...data...
 * 0.1  -50 30 ...data...
 * 0.3 -100 10 ...data...
 * 0.3 -100 25 ...data...
 * 0.3 -100 30 ...data...
 * 0.3  -50 10 ...data...
 * 0.3  -50 25 ...data...
 * 0.3  -50 30 ...data...
 * 0.9 -100 10 ...data...
 * 0.9 -100 25 ...data...
 * 0.9 -100 30 ...data...
 * 0.9  -50 10 ...data...
 * 0.9  -50 25 ...data...
 * 0.9  -50 30 ...data...
 *
 * In the above case, the parameters have values:
 * p0 : 0.1,0.3,0.9
 * p1 : -100, -50
 * p2 : 10,25,30
 *
 * The parameter "hypercube" then is the 3D-hypercube of
 * 3 * 2 * 3 = 18 data lines.
 *
 * Note that the points on the cube are *constant* but not *regular*,
 * e.g. p0 has spacing (0.3-0.1)=0.2 and (0.9-0.3)=0.6, which are different,
 * BUT e.g. the spacing for p1 (-100 - -50 = -50) is the same, whatever the
 * value of p0. The same is true for p3 which always has spacings 15 and 5
 * from (25-10) and (30-25) respectively.
 *
 * Note that the table is assumed to be sorted from SMALLEST
 * to LARGEST parameter values. It is also assumed to be regular and filled.
 * So no missing data please, just put some dummy values in the table.
 *
 * In order to interpolate data, n parameters are passed into this
 * routine in the array x. The result of the interpolation is put
 * into the array r (of size d).
 *
 * If you enable RINTERPOLATE_CACHE then results are cached to avoid
 * slowing the code too much. (This is set in binary_c_code_options.h) This means
 * that the interpolate routine checks your input parameters x against
 * the last few sets of parameters passed in. If you have used these x
 * recently, the result is simply returned. This saves extra interpolation
 * and is often faster.  This is only true in some cases of course - if your
 * x are always different you waste time checking the cache. This is why
 * the cache_length variable exists: if this is false then the cache is skipped.
 * Of course only *you* know if you are likely to call the interpolate routine
 * repeated with the same values... I cannot possibly know this in advance!
 *
 * The interpolation process involved finding the lines of the data table
 * which span each parameter x. This makes a hypercube of length 2^n (e.g.
 * in the above it is 8, for simple 1D linear interpolation it would be the
 * two spanning values). Linear interpolation is then done in the largest
 * dimension, above this is the third parameter (p2), then the second (p1)
 * and finally on the remaining variable (p0). This would reduce the table
 * from 2^3 lines to 2^2 to 2^1 to (finally) 2^0 i.e. one line which is the
 * interpolation result.
 *
 * To find the spanning lines a binary search is performed. This code was
 * donated by Evert Glebbeek. See e.g.
 * http://en.wikipedia.org/wiki/Binary_search_algorithm
 * and note the comment "Although the basic idea of binary search is comparatively
 * straightforward, the details can be surprisingly tricky... " haha! :)
 *
 * Each table has its own unique table_id number. This is just to allow
 * us to set up caches (one per table) and save arrays such as varcount and
 * steps (see below) so they only have to be calculated once.
 * Since binary_c2 these table_id numbers have been allocated automatically,
 * you do not have to set one yourself, but this does assume that the tables
 * are each at fixed memory locations. This is guaranteed if you declare them
 * with the recommended data type Const_data_table, e.g.
 *
 * Const_data_table mytable = { 0.0, 100.0, 1.0, 200.0, 2.0, 500.0 };
 *
 * (Const_data_table is a "const static rinterpolate_float_t" type, which implies its memory
 *  storage is fixed once allocated, and it cannot be changed hence is thread-safe.)
 *
 * However, it is best to use the data_table_t struct to make data tables.
 * This is allocated through the NewDataTable_from_Array and NewDataTable_from_Pointer
 * macros, which require the setting of the number of parameters, data items
 * and lines of the table. Then, the call to do the interpolation
 * is far simpler.
 *
 * I have optimized this as best I can, please let me know if
 * you can squeeze any more speed out of the function.
 * I am sorry this has made most of the function unreadable! The
 * comments should help, but you will need to know some tricks...
 *
 * Certain variables deserve extra mention:
 *
 * varcount
 *   This is the number of unique values of a given parameters. In the above
 * example, this would be varcount[0]=3, varcount[1]=2, varcount[2]=3
 *
 * steps
 *   This is the number of lines of the table there are before a
 * parameter changes. In the above example this would be steps[0]=6,
 * steps[1]=3, steps[2]=1
 *
 * Both varcount and steps are cached for each table.
 *
 * int_table
 *   This is the initially constructed hypercube which is reduced
 * in dimension (e.g. in the above from 3 to 2 to 1) until final
 * interpolation is done.
 *
 * RINTERPOLATE_CACHE_LENGTH
 *   The length of the table. Should be >= the number of tables you
 *   are using. Usually 5 is good, less means faster cache compares
 *   but you're less likely to match!
 *
 * Note: uses Fequal macros to check for floating-point equality, you should
 * check that TINY (current the smallest floating point epsilon)
 * is small enough for your parameters, or rescale your
 * parameters so they are much bigger than TINY (that's best,
 * as TINY may be used used elsewhere). You could, of course, define a
 * different (slower) macro.
 *
 * (c) Robert Izzard, 2005-2019, please send bug fixes!
 */


struct rinterpolate_data_t * rinterpolate(
    const rinterpolate_float_t * RESTRICT const datatable, // (const pointer to) the data table
    struct rinterpolate_data_t * rinterpolate_data, // where rinterpolate stores data
    const rinterpolate_counter_t n, // the number of parameters (i.e. dimensions)
    const rinterpolate_counter_t d, // the number of data items
    const rinterpolate_counter_t l, // the number of lines of data
    const rinterpolate_float_t * RESTRICT const x, // the values of the parameters
    rinterpolate_float_t * RESTRICT const r,  // the result of the interpolation
    const rinterpolate_counter_t cache_length // number of cache lines
    )
{
#if defined RINTERPOLATE_DEBUG && defined RINTERPOLATE_SHOW_TABLE
    rinterpolate_counter_t i=0;
#endif
    Rinterpolate_print("DEBUG RINTERPOLATE datatable=%p n=%u d=%u l=%u x=%p r=%p cache_length=%u\n",
           datatable,
           n,d,l,x,r,cache_length);

    /*
     * If table is NULL we should free all allocated memory and just return
     */
    if(unlikely(datatable==NULL))
    {
        rinterpolate_free_data(rinterpolate_data);
        return NULL;
    }
    else
    {
        rinterpolate_signed_counter_t table_id;

        /*
         * First time through:
         * set up memory space if not already done
         */
        if(unlikely(rinterpolate_data==NULL))
        {
            rinterpolate_alloc_dataspace(&rinterpolate_data);
            table_id = -1;
        }
        else
        {
            /*
             * Get the table id
             */
            table_id =
                rinterpolate_id_table(rinterpolate_data,
                                      datatable);
        }

        Rinterpolate_print("Table ID %d\n",table_id);

        if(table_id == -1)
        {
            /*
             * Table not found, so add a new table
             */
            table_id = rinterpolate_add_new_table(rinterpolate_data,
                                                  datatable,
                                                  n,
                                                  d,
                                                  l,
                                                  cache_length);
            Rinterpolate_print("New table ID %d\n",table_id);
        }

        /*
         * Pointer to the table
         */
        struct rinterpolate_table_t * RESTRICT table =
            rinterpolate_data->tables[table_id];

#ifdef RINTERPOLATE_CACHE
        /*
         * Check for cache resize (if it is resized,
         * it is wiped)
         */
        if(cache_length != table->cache_length)
        {
            rinterpolate_resize_cache(table,cache_length);
        }
#endif // RINTERPOLATE_CACHE

#ifdef RINTERPOLATE_DEBUG
#ifdef RINTERPOLATE_DEBUG_SHOW_TABLE
        if(rinterpolate_debug == TRUE)
        {
            int _i;
            for(_i=0;_i<l;_i++)
            {
                Rinterpolate_print("L%u ",_i);
                rinterpolate_counter_t j;
                for(j=0;j<table->line_length;j++)
                {
                    Rinterpolate_print("%g ",*(datatable+_i*table->line_length+j));
                }
                Rinterpolate_print("\n");
                FLUSH;
            }
        }
#endif // RINTERPOLATE_DEBUG_SHOW_TABLE
#endif //RINTERPOLATE_DEBUG


#ifdef RINTERPOLATE_CACHE
        /* check for cache match */
        if(cache_length &&
           rinterpolate_check_cache(table,x,r) == TRUE)
        {
            goto cache_match;
        }
#endif // RINTERPOLATE_CACHE

        /*
         * Result is not cached, or we did not want to search the cache,
         * we must calculate the interpolation.
         *
         * First, search the table to find the spanning indices.
         */
        rinterpolate_search_table(table,x);

#ifdef RINTERPOLATE_DEBUG
        if(rinterpolate_debug==TRUE)
        {
            rinterpolate_counter_t j;
            Rinterpolate_print("Parameter (x) values: ");
            for(j=0;j<table->n;j++)
            {
                Rinterpolate_print("% 3.3e ",x[j]);
            }
            Rinterpolate_print("\n");

            Rinterpolate_print("Interpolation (f) factors: ");
            for(j=0;j<table->n;j++)
            {
                Rinterpolate_print("% 3.3e ",table->hypertable->f[j]);
            }
            Rinterpolate_print("\n");
            Rinterpolate_print("Interpolation hypertable:\n");
        }
#endif

        /*
         * construct hypercube
         */
        rinterpolate_construct_hypercube(table);

        /*
         * Do interpolation on hypercube
         */
        rinterpolate_interpolate(table,x,r);

#ifdef RINTERPOLATE_DEBUG
        {
            rinterpolate_counter_t j;
            Rinterpolate_print("Result\n");
            for(j=0;j<table->n;j++)
            {
                Rinterpolate_print("% 3.3e ",*(table->hypertable->data+j));
            }
            Rinterpolate_print(" | ");
            for(j=n;j<table->line_length;j++)
            {
                Rinterpolate_print("% 3.3e ",*(table->hypertable->data+j));
            }
            Rinterpolate_print("\n");FLUSH;
        }
#endif

#ifdef RINTERPOLATE_CACHE
        /*
         * No cache match but interpolation done:
         *
         * Save the results of the interpolation into the cache
         */
        if(cache_length)
        {
            rinterpolate_store_cache(table,x,r);
        }

    cache_match:

#endif // RINTERPOLATE_CACHE

        return rinterpolate_data;
    }
}
