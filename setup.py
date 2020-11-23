"""
Setup script for py_rinterpolate

https://docs.python.org/2.5/dist/describing-extensions.html
"""

import setuptools
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
    with open("LICENSE") as file:
        return file.read()

# Interface with the rinterpolate-config to get the locations of the libraries and include directories. 
RINTERPOLATE_CONFIG = "rinterpolate-config"

RINTERPOLATE_INCDIR = (
    subprocess.run(
        [RINTERPOLATE_CONFIG, "cflags"], stdout=subprocess.PIPE, check=True
    )
    .stdout.decode("utf-8")
    .split("I")[-1].strip()
)

RINTERPOLATE_LIBDIRS = (
    subprocess.run(
        [RINTERPOLATE_CONFIG, "libs"], stdout=subprocess.PIPE, check=True
    )
    .stdout.decode("utf-8")
    .split("L")[-1].strip()
)

############################################################
# Making the extension function
############################################################

PY_RINTERPOLATE_MODULE = Extension(
    name="py_rinterpolate._py_rinterpolate",
    sources=["py_rinterpolate/py_rinterpolate_interface.c"],
    libraries=[
        "rinterpolate"
    ],  # since rinterpolate is the actual library we want to interface with.
    library_dirs=[RINTERPOLATE_LIBDIRS],  # Use the location that rinterpolate-config gave us
    include_dirs=[RINTERPOLATE_INCDIR],  # Use the location that rinterpolate-config gave us
    # define_macros=[('DEBUG', None)],
)

setup(
    name="py_rinterpolate",
    version="0.11",
    description="Python wrapper for the linear interpolation libryary rinterpolate (https://gitlab.eps.surrey.ac.uk/ri0005/librinterpolate)",
    author="David Hendriks, Robert Izzard",
    author_email="davidhendriks93@gmail.com",
    maintainer="David Hendriks",
    maintainer_email="davidhendriks93@gmail.com",
    long_description_content_type='text/markdown',
    long_description=readme(),
    keywords=["linear interpolation", "science"],
    license="GPL",
    url="https://github.com/ddhendriks/py_rinterpolate",
    install_requires=["numpy", "pytest"],
    python_requires='>=3.6',
    ext_modules=[PY_RINTERPOLATE_MODULE],
    packages=[
        "py_rinterpolate",
    ],
    classifiers=[
        "Development Status :: 3 - Alpha",
        "License :: OSI Approved :: GNU General Public License v3 or later (GPLv3+)",
        "Intended Audience :: Developers",
        "Intended Audience :: Science/Research",
        "Operating System :: POSIX :: Linux",
        "Programming Language :: Python :: 3",
        "Programming Language :: C",
        "Topic :: Education",
        "Topic :: Scientific/Engineering :: Physics",
        "Topic :: Software Development :: Libraries :: Python Modules",
    ],
)
