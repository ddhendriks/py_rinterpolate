#include "rinterpolate.h"
#include "rinterpolate_internal.h"
/*
 * Attempt to match a table to those in the rinterpolate
 * data structure.
 */

rinterpolate_signed_counter_t Pure_function rinterpolate_id_table(
    struct rinterpolate_data_t * RESTRICT const rinterpolate_data,
    const rinterpolate_float_t * RESTRICT const data
    )
{
    /* look for data table in the existing table_ids */

    rinterpolate_Boolean_t found = FALSE;

    Rinterpolate_print("Look for table in existing table ids\n");
    Rinterpolate_print("currently have %u tables\n",rinterpolate_data->number_of_interpolation_tables);

    rinterpolate_counter_t table_num = 0;
    while(table_num < rinterpolate_data->number_of_interpolation_tables)
    {
        if(rinterpolate_data->tables[table_num]->data ==
           (rinterpolate_float_t *)data)
        {
            /* found : break out of loop and use it */
            found = TRUE;
            break;
        }
        table_num++;
    }

    /* not found */
    if(found==FALSE)
    {
        /* if not found, allocate */
        Rinterpolate_print("Not found : realloc\n");
        table_num = -1;
    }
    else
    {
        Rinterpolate_print("Found at %u\n",table_num);
    }

    return table_num;
}
