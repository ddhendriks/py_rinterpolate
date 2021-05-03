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
    struct rinterpolate_data_t * rinterpolate_data = NULL;
    rinterpolate_counter_t status MAYBE_UNUSED = rinterpolate_alloc_dataspace(&rinterpolate_data);
    debug_printf("rinterpolate_alloc_dataspace_wrapper: Status of allocation: %u\n", status);

    if (status!=0){
        PyErr_SetString(PyExc_ValueError, "rinterpolate_alloc_dataspace_wrapper: Allocation of dataspace unsuccesful");
        return NULL;        
    }

    debug_printf("Packing up dataspace pointer %p into capsule\n", (void *)rinterpolate_data);
    PyObject * dataspace_mem_capsule = PyCapsule_New(rinterpolate_data, "TABLE", NULL);

    return dataspace_mem_capsule;
}

/* 
 * Function to free the memory allocated for the dataspace. 
 * Takes a long int as input that represents the memory adress stored as an int
 */
static PyObject* rinterpolate_free_dataspace_wrapper(PyObject *self, PyObject *args)
{
    PyObject *  dataspace_mem_capsule = NULL;

    /* Parse the input tuple */
    if(!PyArg_ParseTuple(args, "O", &dataspace_mem_capsule))
    {
        return NULL;
    }

    /* Unpack the capsules */
    struct rinterpolate_data_t * rinterpolate_data = NULL;
    if (dataspace_mem_capsule != NULL)
    {
        if (PyCapsule_IsValid(dataspace_mem_capsule, "DATASPACE"))
        {
            if (!(rinterpolate_data = (struct rinterpolate_data_t *) PyCapsule_GetPointer(dataspace_mem_capsule, "DATASPACE")))
                return NULL;   
            debug_printf("Unpacked dataspace pointer %p from capsule\n", (void *)rinterpolate_data);                
        }
    }

    if(rinterpolate_data != NULL)
    {
        debug_printf("rinterpolate_free_dataspace_wrapper: dataspace free rinterpolate_data 1 (free via rinterpolate_free_data) %p\n", (void *)rinterpolate_data);
        rinterpolate_free_data(rinterpolate_data);
        // TODO: Consider putting the extra free here. With valgrind on a normal c script where I interpolate on a table it needs to be there. 
    }

    Py_RETURN_NONE;
}

/* 
 * Function to free the memory allocated for the C_table. 
 * Takes a long int as input that represents the memory adress stored as an int
 */
static PyObject* rinterpolate_free_C_table(PyObject *self, PyObject *args)
{
    PyObject *  C_table_capsule = NULL;

    /* Parse the input tuple */
    if(!PyArg_ParseTuple(args, "O", &C_table_capsule))
    {
        return NULL;
    }

    /* Unpack the capsules */
    double * table = NULL;
    if (C_table_capsule != NULL)
    {
        if (PyCapsule_IsValid(C_table_capsule, "TABLE"))
        {
            if (!(table = (double *) PyCapsule_GetPointer(C_table_capsule, "TABLE")))
                return NULL;   
            debug_printf("Unpacked table pointer %p from capsule\n", (void *)table);
        }
    }

    // TODO: mention to rob that this freeing doesnt `unset` the values in the table. 
    if(table != NULL)
    {
        debug_printf("rinterpolate_free_C_table: free table %p\n", (void *)table);
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
    PyObject *  C_table_capsule = NULL;
    int C_size = -1;

    /* Parse the input tuple */
    if(!PyArg_ParseTuple(args, "Oi", &C_table_capsule, &C_size))
        return NULL;

    /* Unpack the capsules */
    double * table = NULL;
    if (C_table_capsule != NULL)
    {
        if (PyCapsule_IsValid(C_table_capsule, "TABLE"))
        {
            if (!(table = (double *) PyCapsule_GetPointer(C_table_capsule, "TABLE")))
                return NULL;   
            debug_printf("Unpacked table pointer %p from capsule\n", (void *)table);                
        }
    }

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
    PyObject *  C_table_capsule = NULL;
    PyObject *  dataspace_mem_capsule = NULL;
    int nparams = -1;
    int ndata = -1;
    int nlines = -1;
    int usecache = -1;

    PyObject *xList;
    PyObject *xItem;
    int i;
    PyObject* num;

    /* Parse the input tuple */
    if(!PyArg_ParseTuple(args, "OOiiiO!i", &C_table_capsule, &dataspace_mem_capsule, &nparams, &ndata, &nlines, &PyList_Type, &xList, &usecache))
        return NULL;

    /* Unpack the capsules */
    double * table = NULL;
    if (C_table_capsule != NULL)
    {
        if (PyCapsule_IsValid(C_table_capsule, "TABLE"))
        {
            if (!(table = (double *) PyCapsule_GetPointer(C_table_capsule, "TABLE")))
                return NULL;   
            debug_printf("Unpacked table pointer %p from capsule\n", (void *)table);                
        }
    }

    struct rinterpolate_data_t * rinterpolate_data = NULL;
    if (dataspace_mem_capsule != NULL)
    {
        if (PyCapsule_IsValid(dataspace_mem_capsule, "DATASPACE"))
        {
            if (!(rinterpolate_data = (struct rinterpolate_data_t *) PyCapsule_GetPointer(dataspace_mem_capsule, "DATASPACE")))
                return NULL;   
            debug_printf("Unpacked dataspace pointer %p from capsule\n", (void *)rinterpolate_data);                
        }
    }

    /*
     * Allocate memory for the input array, x, and return array, r
     */
    double * x = malloc(sizeof(double) * nparams);
    if(x == NULL) Py_RETURN_NONE;
    double * r = malloc(sizeof(double) * ndata);
    if(r == NULL) Py_RETURN_NONE; // TODO: ask rob about the purpose of the return. should make it a return None probably

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
    PyObject *Result = Py_BuildValue("O", rList);
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

    debug_printf("Packing up table pointer %p into capsule\n", (void *)table);
    PyObject * C_table_capsule = PyCapsule_New(table, "TABLE", NULL);

    return C_table_capsule;
}