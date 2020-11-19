# Readme for py_rinterpolate

This package contains a python wrapper for the rinterpolate library by Rob Izzard (see https://gitlab.eps.surrey.ac.uk/ri0005/librinterpolate)

## Installation:
### Pre-requisites
This code is written in python3.6, and uses NumPy.


To use the package it is necessary to have the rinterpolate library installed properly (not the version shipped with binary_c). 

Installation of that package is straightforward:

```
export DESTDIR=
export PREFIX=
make
make install
```

The location of the directory containing the header `rinterpolate.h` needs to be contained in the envvar `C_INCLUDE_PATH`. 

The location of the directory containing the shared library `librinterpolate.so` needs to be contained in the envvar `LD_LIBRARY_PATH`

Make sure both of these are the case.

### Installation
To install this package via pip:

```
pip install py_rinterpolate
```

If you build this package manually, then:

```
python setup.py install
```

Using `install` will install the package in the site_packages of the currently used python, which will be either the global python or a virtualenv. 

In case you don't have permissions to install things globally, you can append `--user` to either of the above lines, like:
```
pip install --user py_rinterpolate

or

python setup.py install --user
```

This will then install it somewhere in your home folder. If you don't seem to be able to use the package after this, check whether the location where the package was installed is actually in the $PATH or $PYTHONPATH. More info on this is in: https://stackoverflow.com/questions/38112756/how-do-i-access-packages-installed-by-pip-user


## Structure
There are two parts to this module:

### py_rinterpolate_interface
Contains the python-C interface functions that communicate with the shared library `librinterpolate.so`. The functions of this are defined in 

### py_rinterpolate
Contains the python class Rinterpolate which contains the controller functionality: 

it handles a table as input and will call `py_rinterpolate_interface` functions to interface with the actual library.

It also handles handles the allocation of workspaces and the freeing of the memory, all via interface functions stored in the `py_rinterpolate_interface` module