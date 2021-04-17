# librinterpolate

librinterpolate: a library to perform linear interpolation on a constant gridded dataset.
(c) Robert Izzard 2005-2019

Usage:

The table should be organised (in memory) like this

```
p1[0] p2[0] p3[0] ... d1[0] d2[0] d3[0] d4[0] ...
p1[1] p2[1] p3[1] ... d1[1] d2[1] d3[1] d4[1] ...
p1[2] p2[2] p3[2] ... d1[2] d2[2] d3[2] d4[2] ...
```


so there are n parameters (p1, p2, p3...) and d data items
per data line (d1, d2, d3, d4 ...). There are l lines of data.

The parameters should be ordered from low to high values.
The parameters should be on a constant* grid (which does NOT
need to be regular).

What does this mean?

This is a good data table:

```
0.1 -100 10 ...data...
0.1 -100 25 ...data...
0.1 -100 30 ...data...
0.1  -50 10 ...data...
0.1  -50 25 ...data...
0.1  -50 30 ...data...
0.3 -100 10 ...data...
0.3 -100 25 ...data...
0.3 -100 30 ...data...
0.3  -50 10 ...data...
0.3  -50 25 ...data...
0.3  -50 30 ...data...
0.9 -100 10 ...data...
0.9 -100 25 ...data...
0.9 -100 30 ...data...
0.9  -50 10 ...data...
0.9  -50 25 ...data...
0.9  -50 30 ...data...
```


In the above case, the parameters have values:
p0 : 0.1,0.3,0.9
p1 : -100, -50
p2 : 10,25,30

The parameter "hypercube" then is the 3D-hypercube of
323 = 18 data lines.

Note that the points on the cube are constant but not regularly spaced,
e.g. p0 has spacing (0.3-0.1)=0.2 and (0.9-0.3)=0.6, which are different,
BUT e.g. the spacing for p1 (-100 - -50 = -50) is the same, whatever the
value of p0. The same is true for p3 which always has spacings 15 and 5
from (25-10) and (30-25) respectively.

Note that the table is assumed to be sorted from SMALLEST
to LARGEST parameter values. It is also assumed to be regular and fully filled.
So no missing data please.

To interpolate data, n parameters are passed into this
routine in the array x. The result of the interpolation is put
into the array r (of size d).

If you enable RINTERPOLATE_CACHE (set on by default) then results are cached to
avoid slowing the code too much.  This means
that the interpolate routine checks your input parameters x against
the last few sets of parameters passed in. If you have used these x
recently, the result is simply returned. This saves extra interpolation
and is often faster.  This is only true in some cases of course - if your
x are always different you waste time checking the cache. This is why
the cache_hint variable exists: if this is false then the cache is skipped.
Of course only you know if you are likely to call the interpolate routine
repeated with the same values... I cannot possibly know this in advance!

The interpolation process involves finding the lines of the data table
which span each parameter x. This makes a hypercube of length 2^n (e.g.
in the above it is 8, for simple 1D linear interpolation it would be the
two spanning values). Linear interpolation is then done in the largest
dimension, above this is the third parameter (p2), then the second (p1)
and finally on the remaining variable (p0). This would reduce the table
from 2^3 lines to 2^2 to 2^1 to (finally) 2^0 i.e. one line which is the
interpolation result.

To find the spanning lines a binary search is performed. This code was 
originally donated by Evert Glebbeek. See e.g.
http://en.wikipedia.org/wiki/Binary_search_algorithm
and note the comment "Although the basic idea of binary search is comparatively
straightforward, the details can be surprisingly tricky... " haha!

Each table has its own unique table_id number. This is just to allow
us to set up caches (one per table) and save arrays such as varcount and
steps (see below) so they only have to be calculated once.
Since binary_c2 these table_id numbers have been allocated automatically,
you do not have to set one yourself, but this does assume that the tables
are each at fixed memory locations - so please do not move the data around once you start to use it.

Example code is given in test_rinterpolate.c and reproduced here

```
    /* Number of parameters */
    const rinterpolate_counter_t N = 2;

     /* Number of data */
    const rinterpolate_counter_t D = 3;

    /* length of each line (in doubles) i.e. N+D */
    const rinterpolate_counter_t ND = N + D;

    /* total number of lines */
    const rinterpolate_counter_t L = 100;

    /* make rinterpolate data (for cache etc.) */
    struct rinterpolate_data_t * rinterpolate_data = NULL;
    rinterpolate_counter_t status = rinterpolate_alloc_dataspace(&rinterpolate_data);

    /* data table : it is up to you to set the data in here*/
    rinterpolate_float_t table[ND*L];

    /* ... set the data ... */


    /*
     * Arrays for the interpolation location and
     * interpolated data. You need to set
     * the interpolation location, x.
     */
    rinterpolate_float_t x[N],r[D];

    /* ... set the x array ... */

    /* choose whether to cache (0=no, 1=yes) */
    int usecache = 1;

    /* do the interpolation */
    rinterpolate(table,
                 rinterpolate_data,
                 N,
                 D,
                 L,
                 x,
                 r,
                 usecache);

    /* the array r contains the result */


    /* ... rest of code ... */


    /* free memory on exit */
    rinterpolate_free_data(rinterpolate_data);
    free(rinterpolate_data);

```



I have optimized this as best I can, please let me know if
you can squeeze any more speed out of the function.
I am sorry this has made most of the function unreadable! The
comments should help, but you will need to know some tricks...

(c) Robert Izzard, 2005-2020, please send bug fixes!