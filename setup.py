"""
Setup script for py_rinterpolate

https://docs.python.org/2.5/dist/describing-extensions.html
"""

from distutils.core import setup, Extension


import os
import subprocess
import re

# Functions
def readme():
    """Opens readme file and returns content"""
    with open("README.md") as file:
        return file.read()


def license():
    """Opens license file and returns the content"""
    with open("LICENSE.md") as file:
        return file.read()


############################################################
# Making the extension function
############################################################

PY_RINTERPOLATE_MODULE = Extension(
    # name="py_rinterpolate._py_rinterpolate",
    # name="py_rinterpolate.c_api",
    name="py_rinterpolate._py_rinterpolate",
    sources=["py_rinterpolate/py_rinterpolate_interface.c"],
    include_dirs=os.getenv("C_INCLUDE_PATH").split(
        ":"
    ),  # the include header of rinterpolate should be in the collection of paths stored in C_INCLUDE_PATH
    libraries=[
        "rinterpolate"
    ],  # since rinterpolate is the actual library we want to interface with.
    library_dirs=os.getenv("LD_LIBRARY_PATH").split(
        ":"
    ),  # the library should be found in the collection of paths stored in LD_Library_path
    # define_macros=[('DEBUG', None)],
)

# BINARY_C_PYTHON_API_MODULE = Extension(
#     # name="binarycpython.core.binary_c",
#     name="binary_c_python_api",
#     sources=["src/binary_c_python.c"],
#     include_dirs=INCLUDE_DIRS,
#     libraries=LIBRARIES,
#     library_dirs=LIBRARY_DIRS,
#     runtime_library_dirs=RUNTIME_LIBRARY_DIRS,
#     define_macros=[] + BINARY_C_DEFINE_MACROS,
#     extra_objects=[],
#     extra_compile_args=[],
#     language="C",
# )

setup(
    name="py_rinterpolate",
    version="0.1",
    description="description",
    author="David Hendriks, Robert Izzard",
    author_email="davidhendriks93@gmail.com/d.hendriks@surrey.ac.uk,\
        r.izzard@surrey.ac.uk/rob.izzard@gmail.com",
    long_description=readme(),
    url="https://gitlab.eps.surrey.ac.uk/ri0005/binary_c-python",
    install_requires=["numpy", "pytest",],
    license=license(),
    ext_modules=[PY_RINTERPOLATE_MODULE],
    # package_dir={
    #     "binarycpython": "binarycpython",
    #     "binarycpython.utils": "binarycpython/utils",
    #     # 'binarycpython.core': 'lib',
    # },
    packages=[
        "py_rinterpolate",
        # "binarycpython.utils",
        # 'binarycpython.core',
    ],
    # package_data={
    #     'binarycpython.core': ['libbinary_c_api.so'],
    # },
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Intended Audience :: Science/Research",
        "Operating System :: POSIX :: Linux",
        "Programming Language :: Python",
        "Programming Language :: C",
        "Topic :: Communications :: Email",
        "Topic :: Education",
        "Topic :: Scientific/Engineering :: Physics",
        "Topic :: Software Development :: Libraries :: Python Modules",
    ],
)
