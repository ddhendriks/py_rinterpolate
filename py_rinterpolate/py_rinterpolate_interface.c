#include <Python.h>
#include "rinterpolate.h"
#include <assert.h>

/************************************************************
 * python & rinterpolate api bindings.
 * 
 * Module that interfaces python with the rinterpolate c program. 
 * 
 * Copied/ported from perl version of Rob Izzard
 * 
 * extra: https://docstore.mik.ua/orelly/perl2/advprog/ch20_03.htm
 * https://stackoverflow.com/questions/15287590/why-should-py-increfpy-none-be-required-before-returning-py-none-in-c
 ************************************************************/

#ifdef DEBUG
  #define debug_printf(fmt, ...)  printf(fmt, ##__VA_ARGS__);
#else
  #define debug_printf(fmt, ...)    /* Do nothing */
#endif

/***********************************************************
 * Set docstrings
 ***********************************************************/

static char module_docstring[] MAYBE_UNUSED =
    "This module is a python3 wrapper for the rinterpolate library by Rob Izzard.";
static char rinterpolate_set_C_table_docstring[] =
    "Interface function to set the C_table in memory and get its location back.";
static char rinterpolate_alloc_dataspace_wrapper_docstring[] =
    "Interface function to initialise the datapace for the interpolator and get its location back.";
static char rinterpolate_free_dataspace_wrapper_docstring[] =
    "Interface function to free the memory of the dataspace.";
static char rinterpolate_free_C_table_docstring[] =
    "Interface function to free the C_table.";
static char rinterpolate_check_C_table_docstring[] =
    "Interface function to check the contents of the C_table";
static char rinterpolate_wrapper_docstring[] =
    "Interface function to interpolate the table with the given input coefficients";

/***********************************************************
 * Initialize pyobjects/prototypes
 ***********************************************************/

static PyObject* rinterpolate_set_C_table(PyObject *self, PyObject *args);
static PyObject* rinterpolate_alloc_dataspace_wrapper(PyObject *self, PyObject *args);
static PyObject* rinterpolate_free_dataspace_wrapper(PyObject *self, PyObject *args);
static PyObject* rinterpolate_free_C_table(PyObject *self, PyObject *args);
static PyObject* rinterpolate_check_C_table(PyObject *self, PyObject *args);
static PyObject* rinterpolate_wrapper(PyObject *self, PyObject *args);

/***********************************************************
 * Set the module functions
 ***********************************************************/

static PyMethodDef module_methods[] = {
    {"_rinterpolate_set_C_table", rinterpolate_set_C_table, METH_VARARGS, rinterpolate_set_C_table_docstring},
    {"_rinterpolate_alloc_dataspace_wrapper", rinterpolate_alloc_dataspace_wrapper, METH_NOARGS, rinterpolate_alloc_dataspace_wrapper_docstring},
    {"_rinterpolate_free_dataspace_wrapper", rinterpolate_free_dataspace_wrapper, METH_VARARGS, rinterpolate_free_dataspace_wrapper_docstring},
    {"_rinterpolate_free_C_table", rinterpolate_free_C_table, METH_VARARGS, rinterpolate_free_C_table_docstring},
    {"_rinterpolate_check_C_table", rinterpolate_check_C_table, METH_VARARGS, rinterpolate_check_C_table_docstring},
    {"_rinterpolate_wrapper", rinterpolate_wrapper, METH_VARARGS, rinterpolate_wrapper_docstring},

    {NULL, NULL, 0, NULL}
};

/***********************************************************
 * Making the module
 ***********************************************************/

/* Creation function for the module */
static struct PyModuleDef Py__py_rinterpolate =
{
    PyModuleDef_HEAD_INIT,
    "_py_rinterpolate", /* name of module */
    module_docstring,          /* module documentation, may be NULL */
    -1,          /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    module_methods
};

/* Initialize function for module */
PyMODINIT_FUNC PyInit__py_rinterpolate(void)
{
    return PyModule_Create(&Py__py_rinterpolate);
}

/***********************************************************
 * Function definitions
 ***********************************************************/

/* Function to allocate dataspace for the rinterpolate. Returns 0 on error */
static PyObject* rinterpolate_alloc_dataspace_wrapper(PyObject *self, PyObject *args)
{
    /*  */

    // int x;

    // /* Parse the input tuple */
    // if(!PyArg_ParseTuple(args, "i", &x))
    //     return NULL;

    struct rinterpolate_data_t * rinterpolate_data = NULL;
    rinterpolate_counter_t status MAYBE_UNUSED = rinterpolate_alloc_dataspace(&rinterpolate_data);
    debug_printf("rinterpolate_alloc_dataspace_wrapper: Status of allocation: %d\n", status);

    if (status!=0){
        PyErr_SetString(PyExc_ValueError, "rinterpolate_alloc_dataspace_wrapper: Allocation of dataspace unsuccesful");
        return NULL;        
    }

    // Transform the pointer to an unsigned_int pointer.
    uintptr_t rinterpolate_data_memaddr_int = (uintptr_t)rinterpolate_data;
    debug_printf("rinterpolate_alloc_dataspace_wrapper: rinterpolate_data is at address: %p rinterpolate_data_memaddr_int: %ld\n", (void*)&rinterpolate_data, rinterpolate_data_memaddr_int);

    return Py_BuildValue("l", rinterpolate_data_memaddr_int);
}

// /* free dataspace */
// void rinterpolate_free_dataspace_wrapper(SV * dataspace)
// {
//     struct rinterpolate_data_t * rinterpolate_data
//         = Perl_var_to_C_pointer(dataspace,struct rinterpolate_data_t *);
//     if(rinterpolate_data != NULL)
//     {
//         //printf("table free rinterpolate_data1 %p\n",rinterpolate_data);
//         rinterpolate_free_data(rinterpolate_data);
//         if(rinterpolate_data != NULL)
//         {
//             //printf("table free rinterpolate_data2 %p\n",rinterpolate_data);
//             free(rinterpolate_data);
//         }
//     }
// }

/* 
 * Function to free the memory allocated for the dataspace. 
 * Takes a long int as input that represents the memory adress stored as an int
 */
static PyObject* rinterpolate_free_dataspace_wrapper(PyObject *self, PyObject *args)
{
    long int dataspace_memaddr = -1;

    /* Parse the input tuple */
    if(!PyArg_ParseTuple(args, "l", &dataspace_memaddr))
        return NULL;

    // cast the dataspace_memaddr integer back to the real memory adress
    struct rinterpolate_data_t * rinterpolate_data = (struct rinterpolate_data_t *)dataspace_memaddr;
    debug_printf("rinterpolate_free_dataspace_wrapper: Took long int memaddr %ld and loaded it to %p\n", dataspace_memaddr, (void*)&rinterpolate_data);

    // TODO: ask rob how the freeing goes here.
    if(rinterpolate_data != NULL)
    {
        debug_printf("rinterpolate_free_dataspace_wrapper: dataspace free rinterpolate_data 1 (free via rinterpolate_free_data) %p\n", rinterpolate_data);
        rinterpolate_free_data(rinterpolate_data);
        free(rinterpolate_data);

        // if(rinterpolate_data != NULL)
        // {
        //     debug_printf("rinterpolate_free_dataspace_wrapper: dataspace free rinterpolate_data 2 (regular free) %p\n", rinterpolate_data);
        //     free(rinterpolate_data);
        // }
    }

    Py_RETURN_NONE;
}


// void rinterpolate_free_C_table(SV * C_table)
// {
//     double * table = Perl_var_to_C_pointer(C_table,double*);
//     if(table != NULL)
//     {
//         //printf("free table %p\n",table);
//         free(table);
//         table = NULL;
//     }
// }

/* 
 * Function to free the memory allocated for the C_table. 
 * Takes a long int as input that represents the memory adress stored as an int
 */
static PyObject* rinterpolate_free_C_table(PyObject *self, PyObject *args)
{
    long int C_table = -1;

    /* Parse the input tuple */
    if(!PyArg_ParseTuple(args, "l", &C_table))
        return NULL;

    // Convert to real real pointer again
    double * table = (void*)C_table;
    debug_printf("rinterpolate_free_C_table: Took long int memaddr %ld and loaded it to %p\n", C_table, (void *)&table);

    // TODO: mention to rob that this freeing doesnt `unset` the values in the table. 
    if(table != NULL)
    {
        debug_printf("rinterpolate_free_C_table: free table %p\n",table);
        free(table); // TODO: as rob if this works. 
        table = NULL;
    }

    Py_RETURN_NONE;
}

/*
 * Function to check the contents of the C_table
 */
static PyObject* rinterpolate_check_C_table(PyObject *self, PyObject *args)
{
     /* Function to free the memory allocated for the C_table. takes a long int as input that represents the memory adress stored as an int*/
    long int C_table = -1;
    int C_size = -1;

    /* Parse the input tuple */
    if(!PyArg_ParseTuple(args, "li", &C_table, &C_size))
        return NULL;

    // Convert to real real pointer again
    double * table = (double*)C_table;

    debug_printf("rinterpolate_check_C_table: Took long int memaddr %ld and loaded it to %p\n", C_table, (void *)&table);

    int i;
    if(table != NULL)
    {
        for (i=0; i<C_size; i++)
        {
            debug_printf("rinterpolate_check_C_table: table[%d]=%f\n", i, table[i]);
        }
    }

    Py_RETURN_NONE;
}

/*
 * Function to call librinterpolate to do
 * the hard work. On failure, tries to deallocate
 * memory and nothing is set in perl_r (the \@r list
 * reference).
 */
static PyObject* rinterpolate_wrapper(PyObject *self, PyObject *args)
{
    long int C_table = -1;
    long int dataspace = -1;
    int nparams = -1;
    int ndata = -1;
    int nlines = -1;
    int usecache = -1;

    PyObject *xList;
    PyObject *xItem;
    int i;
    PyObject* num;

    /* Parse the input tuple */
    if(!PyArg_ParseTuple(args, "lliiiO!i", &C_table, &dataspace, &nparams, &ndata, &nlines, &PyList_Type, &xList, &usecache))
        return NULL;

    /*
     * Allocate memory for the input array, x, and return array, r
     */
    double * x = malloc(sizeof(double) * nparams);
    if(x == NULL) Py_RETURN_NONE;
    double * r = malloc(sizeof(double) * ndata);
    if(r == NULL) Py_RETURN_NONE; // TODO: ask rob about the purpose of the return. should make it a return None probably


    /*
     * Get the table from Perl->C pointer conversion
     */
    double * table = (double *)C_table;

    /*
     * Get the interpolate_data struct pointer
     */
    struct rinterpolate_data_t * rinterpolate_data = (struct rinterpolate_data_t *)dataspace;

    // Fill the C-array with the python input
    for(i=0; i<nparams; i++)
    {
        xItem = PyList_GetItem(xList, i);
        if(!PyFloat_Check(xItem)) 
        {
            PyErr_SetString(PyExc_TypeError, "list items must be floats.");
            return NULL;
        }
        double cItem = PyFloat_AsDouble(xItem);
        debug_printf("i=%d input_table[i]=%f\n", i, cItem);

        if (PyErr_Occurred() != NULL)
        {
            PyErr_SetString(PyExc_TypeError, "Conversion python float to C double failed.");
            return NULL;
        } else {
            x[i] = cItem;
        }
    }

    /*
     * Call rinterpolate
     */
    rinterpolate(table,
                 rinterpolate_data,
                 nparams,
                 ndata,
                 nlines,
                 x,
                 r,
                 usecache);

    /*
     * Set results in Python array
     */
    PyObject *rList = PyList_New(ndata);
    for(i=0; i<ndata; i++)
    {
        num = PyFloat_FromDouble(r[i]);
        if(!num){ // TODO: check if this is the proper way to do things. 
            Py_DECREF(rList); 
            return NULL;  
        }
        PyList_SetItem(rList, i, num);
    }

    /*
     * Free memory
     */
    free(x);
    free(r);

    // return stuff
    PyObject *Result = Py_BuildValue("O",rList);
    Py_DECREF(rList);
    return Result;
}

/* 
 * Build the c-version of the python table and return the pointer to it
 * 
 * Got inspiration from:
 * https://stackoverflow.com/questions/22458298/extending-python-with-c-pass-a-list-to-pyarg-parsetuple
 */
static PyObject* rinterpolate_set_C_table(PyObject *self, PyObject *args)
{

    PyObject *pList;
    PyObject *pItem;
    Py_ssize_t n_check;

    /* initialise parameters. */
    int nparams;
    int ndata;
    int nlines;

    /* Parse the input tuple */
    if(!PyArg_ParseTuple(args, "O!iii", &PyList_Type, &pList, &nparams, &ndata, &nlines))
        return NULL;

    /*
     * Number of lines in the table
     */
    const long int ntable = (ndata + nparams) * nlines;
    n_check = PyList_Size(pList);

    if (n_check-ntable != 0)
    {
        printf("rinterpolate_set_C_table: Error, the length of the input table (%ld) does not match the length calculated (ndata + nparams) * nlines (%ld)\n", n_check, ntable);
        PyErr_SetString(PyExc_ValueError, "rinterpolate_set_C_table: Wrong input for nparams and ndata");
        return NULL;
    }

    /*
     * Allocate memory for a C version of the interpolation
     * table, and fill it.
     */
    double * table = malloc(sizeof(double) * ntable);

    if(table != NULL)
    {
        int i;
        for(i=0; i<n_check; i++)
        {
            pItem = PyList_GetItem(pList, i);
            if(!PyFloat_Check(pItem)) 
            {
                PyErr_SetString(PyExc_TypeError, "list items must be floats.");
                return NULL;
            }
            double cItem = PyFloat_AsDouble(pItem);

            if (PyErr_Occurred() != NULL)
            {
                PyErr_SetString(PyExc_TypeError, "error occured in converting the python float to C double\n");
            } else {
                table[i] = cItem;
            }
        }
    }
    else
    {
        PyErr_SetString(PyExc_ValueError, "rinterpolate_set_C_table: Table not set succesfully");
        return NULL;
    }

    // return the pointer to the table. 
    uintptr_t C_table_memaddr_int = (uintptr_t)table;
    return Py_BuildValue("l", C_table_memaddr_int);
}