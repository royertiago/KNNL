#!/bin/bash

cvs -z3 -d:ext:habdankw@knnl.cvs.sourceforge.net:/cvsroot/knnl commit Doxyfile README gcccpuopt.sh Makefile plots_octave.m plots_matlab.m
cvs -z3 -d:ext:habdankw@knnl.cvs.sourceforge.net:/cvsroot/knnl commit *.cpp
cvs -z3 -d:ext:habdankw@knnl.cvs.sourceforge.net:/cvsroot/knnl commit *.hpp

