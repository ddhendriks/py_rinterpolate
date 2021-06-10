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


############################################################
# Making the extension function
############################################################

# Find all the files of the rinterpolate and add them to the sources
librinterpolate_src_path = "src/librinterpolate/src"

path_contents = os.listdir(librinterpolate_src_path)

c_files = [
    os.path.join(librinterpolate_src_path, file)
    for file in path_contents
    if file.endswith(".c")
]

h_files = [
    os.path.join(librinterpolate_src_path, file)
    for file in path_contents
    if file.endswith(".h")
]

SOURCES = ["src/py_rinterpolate_interface.c"] + c_files
HEADERS_FILES = h_files

PY_RINTERPOLATE_MODULE = Extension(
    name="py_rinterpolate._py_rinterpolate",
    sources=SOURCES,
    include_dirs=[librinterpolate_src_path],
    extra_compile_args=[
        "-O3",
        "-Wpedantic",
        "-fPIC",
        "-g",
        "-Wstrict-prototypes",
        "-Wno-nonnull-compare",
        "-std=gnu99",
        "-Wall",
        "-Wformat-signedness",
        "-D__RINTERPOLATE__",
        "-Wformat",
        "-D__RINTERPOLATE_BUILD_BUILD_FLAGS__ ",
    ],
    # define_macros=[("DEBUG", None)],
)

setup(name="py_rinterpolate",
    version="0.13.0",
    description="Python wrapper for the linear interpolation library 'rinterpolate' (https://gitlab.eps.surrey.ac.uk/ri0005/librinterpolate)",
    author="David Hendriks, Robert Izzard",
    author_email="davidhendriks93@gmail.com",
    maintainer="David Hendriks",
    maintainer_email="davidhendriks93@gmail.com",
    long_description_content_type="text/markdown",
    long_description=readme(),
    keywords=["linear interpolation", "science"],
    license="GPL",
    url="https://github.com/ddhendriks/py_rinterpolate",
    install_requires=["numpy", "pytest",],
    python_requires=">=3.6",
    ext_modules=[PY_RINTERPOLATE_MODULE],
    headers=HEADERS_FILES,
    packages=["py_rinterpolate",],
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
