import unittest
import numpy as np

from py_rinterpolate import Rinterpolate

import test_data


class TestClass(unittest.TestCase):
    """
    Unittest class
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
        rinterpolator = Rinterpolate(verbosity=1)

        # Check if flattening works
        flattened_table = rinterpolator._flatten(self.INPUT_TABLE)

        assert flattened_table == [1, 2, 3, 4, 5, 6], "Flattening not working   "

    def test_destroy(self):
        """
        Unit test to test the destroy function
        """

        # Create the object
        rinterpolator = Rinterpolate(verbosity=1)

        print('Testing {} rinterpolator._localcache["C_table"]: {}'.format(rinterpolator, rinterpolator._localcache["C_table"]))
        print('Testing {} rinterpolator._dataspace: {}'.format(rinterpolator, rinterpolator._dataspace))


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

        test_data_nparams = 3
        test_data_ndata = 10

        #
        rinterpolator = Rinterpolate(
            table=test_data_table,  # Contains the table of data
            nparams=test_data_nparams,  # The amount of parameters in the table
            ndata=test_data_ndata,  # The amount of datapoints (the parameters that we want to interpolate)
            verbosity=1
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
            "\tlocal output:\n\t\t{}".format(
                ["{:.6f}".format(el) for el in result_local]
            )
        )
        print("\tComparison output:\n\t\t{}".format(first_test_result))
        print("\tmax diff: {}".format(max(diff_list)))

        assert max(diff_list) < 1e-5, "Difference is too big"

    def test_multiply_table_column(self):
        """
        Unit test to see if the multiply column works
        """

        # Create the object
        rinterpolator = Rinterpolate(
            table=self.INPUT_TABLE,  # Contains the table of data
            nparams=self.NPARAMS,  # The amount of parameters in the table
            ndata=self.NDATA,  # The amount of datapoints (the parameters that we want to interpolate)
            verbosity=1
        )

        rinterpolator.multiply_table_column(1, 2)
        compare_table = np.array([[1, 2, 3], [4, 5, 6]]) * [1, 2, 1]
        flattened_compare_table = rinterpolator._flatten(compare_table)

        # Destroy the object
        rinterpolator.destroy()

        assert rinterpolator._table == list(flattened_compare_table)

if __name__ == "__main__":
    unittest.main()
