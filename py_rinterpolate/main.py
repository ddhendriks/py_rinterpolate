"""
Entry point for py_rinterpolate. 

imports the API functions as underscores and keeps them _private_

contains the main class for the interpolator

TODO: use numpy to improve the array mutations

example of input table:

p1[0] p2[0] p3[0] ... d1[0] d2[0] d3[0] d4[0] ...
p1[1] p2[1] p3[1] ... d1[1] d2[1] d3[1] d4[1] ...
p1[2] p2[2] p3[2] ... d1[2] d2[2] d3[2] d4[2] ...

For a good description of the requirements and workings of the rinterpolate, see: https://gitlab.eps.surrey.ac.uk/ri0005/librinterpolate
"""

import numpy as np

from py_rinterpolate import _py_rinterpolate  # Import the c-module


class Rinterpolate(object):
    """
    Class to interpolate on parameters given a certain input table. 

    The input _should_ be a multidimensional array. For now it doesnt work with dictionaries.
    """

    def __init__(
        self,
        table=None,
        nparams=-1,
        ndata=-1,
        usecache=0,
        _dataspace=None, 
        _localcache=None,
        **kwargs
    ):
        self.nparams = nparams  # Amount of parameters contained in the table
        self.ndata = ndata  # Amount of datapoints contained in a table row
        self.usecache = usecache  # Whether to use cache
        self._dataspace = _dataspace  # Dataspace memory capsule

        # Handle table. self.table holds the table, which upon input gets flattened. See module description
        if not table:
            self._table = []
        else:
            self._table = self._handle_table_setting(table)

        # Handle localcache
        if not _localcache:  # Holds information about the cached table
            self._localcache = {
                "C_table": None,  # Holds the memory adress of the C_table
                "C_size": -1,  # Holds the size (amount of entries) of the C_table
            }
        else:
            self._localcache = _localcache

        # Add extra properties to the class
        self.__dict__.update(kwargs)

        # Allocate dataspace if not defined
        # self._dataspace contains the memory adress for the actual dataspace.
        if not self._dataspace:
            self._dataspace = (
                _py_rinterpolate._rinterpolate_alloc_dataspace_wrapper()
            )  # API call
            print('self._dataspace: {}'.format(self._dataspace))

        # Check whether that was succesful
        if not self._dataspace:
            print("Could not allocate memory for rinterpolate")
            raise ValueError

    def _handle_table_setting(self, table):
        """
        Function to check the input table and flatten it.
        """

        if not ((isinstance(table, list)) or (isinstance(table, np.ndarray))):
            print("Please input either a nested list of a nested numpy array")
            raise ValueError

        flattened_table = self._flatten(table)

        # if numpy
        if isinstance(flattened_table, np.ndarray):
            # if not float
            if not flattened_table.dtype == "float64":
                flattened_table = flattened_table.astype(np.float)

                # convert to list
                flattened_table = list(flattened_table)
        else:
            # if list already, convert stuff to floats to be sure
            flattened_table = [float(el) for el in flattened_table]

        return flattened_table

    def destroy(self):
        """
        Formally required function to free memory and release variables

        This is a bit of a remnant from perl, as you need to explicitly undef the object with perl.

        However, having this function is still needed to release/free the allocated memories
        """

        # Free the dataspace by passing the dataspace memory location to the freeing function
        if self._dataspace:
            print("freeing self._dataspace: {}".format(self._dataspace))
            _py_rinterpolate._rinterpolate_free_dataspace_wrapper(
                self._dataspace
            )  # API call

        # Free the C_table by passing the memory location to the free-ing function
        if self._localcache["C_table"]:
            print("freeing self._localcache['C_table']: {}".format(self._localcache["C_table"]))
            _py_rinterpolate._rinterpolate_free_C_table(
                self._localcache["C_table"]
            )  # API call

    def DESTROY(self):
        """
        Alias for destroy function
        """

        self.destroy()

    def clear_localcache(self):
        """
        Clear the local cache
        """

        # Remove it if it exists
        if self._localcache["C_table"]:
            # rinterpolate_free_C_table(self._localcache['C_table']) # API call
            self._localcache["C_table"] = None
            self._localcache["C_size"] = 1

    def return_ndata(self, input_val=None):
        """
        return ndata and sets the value if input is passed
        """

        # sub ndata
        # {
        #     my $self = shift;
        #     if(defined $_[0])
        #     {
        #         $self->{'ndata'} = $_[0];
        #     }
        #     return $self->{'ndata'};
        # }

        if input_val:
            self.ndata = input_val
        return self.ndata

    def return_nparams(self, input_val=None):
        """
        returns nparams and sets the value if input is passed
        """

        # sub nparams
        # {
        #     my $self = shift;
        #     if(defined $_[0])
        #     {
        #         $self->{'nparams'} = $_[0];
        #     }
        #     return $self->{'nparams'};
        # }

        if input_val:
            self.nparams = input_val
        return self.nparams

    def return_nlines(self, input_val=None):
        """
        return nlines and sets the value if input is passed
        """

        # sub nlines
        # {
        #     my $self = shift;
        #     if(defined $_[0])
        #     {
        #         $self->{'nlines'} = $_[0];
        #     }
        #     elsif(!defined $self->{'nlines'})
        #     {
        #         $self->{'nlines'} = $self->calc_nlines();
        #     }
        #     return $self->{'nlines'};
        # }

        if input_val:
            self.nlines = input_val
        elif not self.nlines:
            self.nlines = self.calc_nlines()
        return self.nlines

    def calc_nlines(self):
        """
        Function to calculate nlines based on the flattened table
        """

        # sub calc_nlines
        # {
        #     # given a full object, return the number of lines
        #     my $self = shift;
        #     return scalar @{$self->{'table'}}/($self->{'nparams'}+$self->{'ndata'});
        # }

        nlines = len(self._table) / (self.nparams + self.ndata)
        if not (nlines % 1 == 0):
            print(
                "Something went wrong in calculating the amount of lines. Found a fractional amount"
            )
            raise ValueError

        return int(nlines)

    def multiply_table_column(self, column, factor):
        """
        Multiple table <column> (0 = first) by <factor>
        """

        # sub multiply_table_column
        # {
        #     # multiply table column $col (0=first, etc.) by $factor
        #     my ($self,$column,$factor) = @_;
        #     $self->clear_localcache();
        #     my $nlines = $self->nlines(); # total num lines
        #     my $nl = ($self->{'ndata'} + $self->{'nparams'}); # num data + params in line
        #     for(my $i=0;$i<$nlines;$i++)
        #     {
        #         $self->{'table'}->[$i * $nl + $column] *= $factor;
        #     }
        # }

        # clear caches
        self.clear_localcache()
        nlines = self.return_nlines()
        nl = self.ndata + self.nparams  #

        # Set the values TODO: Make use of vectorized calculations here. 
        for i in range(nlines):
            self._table[i * nl + column] *= factor

    def set_table(self, new_table):
        """
        Sets new table data and flattens it. 
        Both a normal list and a numpy array of floats is accepted. 

        Rebuilding the cache gets done at interpolate
        """

        self.clear_localcache()

        self._table = self._handle_table_setting(new_table)

    def _flatten(self, table):
        """
        Function to flatten the input. calls the flatten_iterator function
        """

        return list(self._flatten_iterator(table))

    def _flatten_iterator(self, items):
        """
        Yield items from any nested iterable; see https://stackoverflow.com/a/40857703

        TODO: ask rob what to expect at all from the structure of the things. 
        """

        from collections.abc import Iterable

        for x in items:
            if isinstance(x, Iterable) and not isinstance(x, (str, bytes)):
                for sub_x in self._flatten_iterator(x):
                    yield sub_x
            else:
                yield x

    def interpolate(self, x):
        """
        Actual interpolation function. 

        Passes the C_table and _dataspace memory locations to the interpolate wrapper, along with info about the table.

        the array X gets passed to the interpolator, containing the coordinates we are interested in. 

        The function returns an array r, as the result.

        Flag usecache determines whether the 
        """

        if not self._table:
            print("Table not set or empty. Aborting")
            raise ValueError

        if self.ndata == -1:
            print("Ndata is not set. Aborting")
            raise ValueError

        if self.nparams == -1:
            print("Nparams is not set. Aborting")
            raise ValueError

        # put input in correct type
        input_x = [float(el) for el in x]

        # Set data, nparams, ndata:
        nlines = self.calc_nlines()

        # total number of items in the table:
        n = (self.ndata + self.nparams) * nlines

        # Get localcache
        localcache = self._localcache

        ## check if an existing table requires an update.
        # If it doesnt match what we expect, then set it to 0
        if (not localcache["C_table"] == None) and (not localcache["C_size"] == n):
            print("Freeying the table")
            _py_rinterpolate._rinterpolate_free_C_table(localcache["C_table"])

            localcache["C_table"] = None
            localcache["C_size"] = -1

        # set up C copy of the table if we haven't
        # one already (or just freed it above)
        if localcache["C_table"] == None:
            print("Set table {}, nparams={}, ndata={}\n".format(self._table, self.nparams, self.ndata))
            localcache["C_table"] = _py_rinterpolate._rinterpolate_set_C_table(
                self._table, self.nparams, self.ndata, nlines
            )
            # api call
            localcache["C_size"] = n

        # do the interpolation through librinterpolate
        result = _py_rinterpolate._rinterpolate_wrapper(
            localcache["C_table"],
            self._dataspace,
            self.nparams,
            self.ndata,
            nlines,
            input_x,
            self.usecache,
        )

        return result
