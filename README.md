# Readme for py_rinterpolate
This package contains a python wrapper for the rinterpolate library by Rob Izzard (see https://gitlab.eps.surrey.ac.uk/ri0005/librinterpolate)

## Installation:
### Pre-requisites
This code is written in python3.6, and uses NumPy.

To use the package it is necessary to have the rinterpolate library installed properly (not the version shipped with binary_c). 

Installation of that package is straightforward:

```
export DESTDIR=
export PREFIX=<location to install. like $HOME/.local>
make
make install
```

Make sure that the location where `rinterpolate-config` is installed is included in your `$PATH` variable. We use `interpolate-config` to get the location of the header en library files. 

### Installation
To install this package via pip:

```
pip install py-rinterpolate
```

If you build this package manually, then:

```
python setup.py install
```

Using `install` will install the package in the site_packages of the currently used python, which will be either the global python or a virtualenv. 

In case you don't have permissions to install things globally, you can append `--user` to either of the above lines, like:
```
pip install --user py-rinterpolate

or

python setup.py install --user
```

This will then install it somewhere in your home folder. If you don't seem to be able to use the package after this, check whether the location where the package was installed is actually in the $PATH or $PYTHONPATH. More info on this is in: https://stackoverflow.com/questions/38112756/how-do-i-access-packages-installed-by-pip-user

## Structure
There are two parts to this module:

### \_py_rinterpolate
Contains the python-C interface functions that communicate with the shared library `librinterpolate.so`. The functions of this are defined in py_rinterpolate/py_rinterpolate_interface.c

### py_rinterpolate
Contains the python class Rinterpolate which contains the controller functionality: 

it handles a table as input and will call `_py_rinterpolate` functions to interface with the actual library.

It also handles handles the allocation of workspaces and the freeing of the memory, all via interface functions stored in the `_py_rinterpolate` C-extension. 

## Usage
The entry point object is the Rinterpolate object. An example of usage is shown below, for a good explanation about the input tables please refer to https://gitlab.eps.surrey.ac.uk/ri0005/librinterpolate 

```
data_table = <your table> # This data table is the table you interpolate on, can be a nested list or a nested numpy array of type float64  
input_list = <your coordinates> # list of the coordinates you want to have the interpolation to. Should contain <data_nparams> of items.

data_nparams = 3 # amount of parameters in the table
data_ndata = 10 # amount of different data points per line in the table

# setting up the interpolator and loading in the table
rinterpolator = Rinterpolate(data_table, data_nparams, data_ndata)

result = rinterpolator.interpolate(input_list)
```