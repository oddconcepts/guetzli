#!/usr/bin/env python

from distutils.core import setup, Extension

import os

srcs = ['_guetzli.cc']
defs = []
libs = ['guetzli']

os.environ["CC"]  = 'gcc'

ext = Extension('_guetzli', 
            srcs,
            define_macros=defs,
            libraries=libs)

setup(name='_guetzli',
      version='0.0',
      description='guetzli extension',
      author='Tee Jung',
      author_email='naey05@gmail.com',
      url='https://github.com/Tee0125/guetzli',
      ext_modules=[ext],
      packages=['pyguetzli'],
     )

