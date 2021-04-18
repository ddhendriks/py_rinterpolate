from py_rinterpolate import Rinterpolate
import test_data


import unittest




class TestClass(unittest.TestCase):
    """
    Unittest class

    # https://stackoverflow.com/questions/17353213/init-for-unittest-testcase
    """

    def __init__(self, *args, **kwargs):
        """
        init
        """
        super(TestClass, self).__init__(*args, **kwargs)

        self.INPUT_TABLE = [[1, 2, 3], [4, 5, 6]]
        self.NPARAMS = 2
        self.NDATA = 1


    def test_flatten(self):
        """
        Unit test for flattening the table
        """

        # Create the object
        rinterpolator = Rinterpolate()

        # Check if flattening works
        flattened_table = rinterpolator._flatten(self.INPUT_TABLE)

        assert flattened_table == [1, 2, 3, 4, 5, 6], "Flattening not working   "


    def test_destroy(self):
        """
        Unit test to test the destroy function
        """

        # Create the object
        rinterpolator = Rinterpolate()

        # Destroy the object
        rinterpolator.destroy()

        print(rinterpolator._localcache["C_table"])
        print(rinterpolator._dataspace)


    def test_interpolate_compare_with_perl(self):
        """
        Unit test that compares the interpolation results with perl
        """

        test_result_filename = "test_results.txt"
        with open(test_result_filename, "r") as f:
            test_results = f.readlines()

        print("Loaded table with test data")

        test_data_table = test_data.test_table
        test_data_input_list = test_data.test_coeffs

        # print(test_data_table)
        print(test_data_input_list)

        test_data_nparams = 3
        test_data_ndata = 10

        quit()

        #
        rinterpolator = Rinterpolate(
            table=test_data_table,        # Contains the table of data 
            nparams=test_data_nparams,    # The amount of parameters in the table 
            ndata=test_data_ndata         # The amount of datapoints (the parameters that we want to interpolate)
        )
        print("Set up interpolator")

        input_list = [float(el) for el in test_data_input_list[0]]
        result_local = rinterpolator.interpolate([float(el) for el in input_list])

        first_test_result = [float(el) for el in test_results[1].split()][3:]

        diff_list = [el1 - el2 for (el1, el2) in zip(first_test_result, result_local)]

        print("test_interpolate_compare_with_perl:")
        print("\tInterpolating table")
        print("\tusing coefficients:\n\t\t{}".format(input_list))
        print(
            "\tlocal output:\n\t\t{}".format(["{:.6f}".format(el) for el in result_local])
        )
        print("\tComparison output:\n\t\t{}".format(first_test_result))
        print("\tmax diff: {}".format(max(diff_list)))

        assert max(diff_list) < 1e-5, "Difference is too big"


# rinterpolator = Rinterpolate()
# # rinterpolator.interpolate([1,2])

# # C-code
# # Set C_table
# array = [200000000000000.0,2.0,3.0,4.0,5.0,6.0]
# C_table = _py_rinterpolate._rinterpolate_set_C_table(array, 1, 1, 3)

# # Check C_table
# print(_py_rinterpolate._rinterpolate_check_C_table(C_table, len(array)))

# # Free C_table
# _py_rinterpolate._rinterpolate_free_C_table(C_table)

# # Check again
# print(_py_rinterpolate._rinterpolate_check_C_table(C_table, len(array)))


# import numpy as np

# arr = np.array([1,2])
# if not arr.dtype=='float64':
#     arr = arr.astype(np.float)
# print(arr.dtype)

# print(arr)
if __name__ == "__main__":
    unittest.main()

