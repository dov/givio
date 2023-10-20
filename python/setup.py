#!/usr/bin/env python

with open("README.md", "r") as fh:
    long_description = fh.read()

import setuptools
from distutils.core import setup

setup(name='givio',
      py_modules=['GivIO'],
      version='0.1.0',
      license='lgpl-2.1',
      description='A library for manipulating giv files',
      long_description=long_description,
      long_description_content_type="text/markdown",
      author='Dov Grobgeld',
      author_email='dov.grobgeld@gmail.com',
      url='http://github.com/dov/givio',
      download_url = 'https://github.com/dov/givio/archive/v0.1.0.tar.gz',
      keywords = ['math','geometry'],
      install_requires=[            
          'numpy',
          'pandas'
      ],
      setup_requires=['wheel'],
      classifiers = [
          "Programming Language :: Python",
          "Programming Language :: Python :: 3",
          "Development Status :: 3 - Alpha",
          "Environment :: Other Environment",
          "Intended Audience :: Developers",
          "License :: OSI Approved :: GNU Lesser General Public License v2 or later (LGPLv2+)",
          "Operating System :: OS Independent",
          "Topic :: Software Development :: Libraries :: Python Modules",
          "Topic :: Scientific/Engineering :: Mathematics",
          ],
      )

