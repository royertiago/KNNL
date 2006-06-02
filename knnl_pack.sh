#!/bin/bash

src=SF
trgt=knnl

src=$1
trgt=$2

echo $src $trgt

rm -v $trgt/*
cp -v $src/*.cpp $trgt/
cp -v $src/*.hpp $trgt/

for i in Doxyfile README gcccpuopt.sh Makefile plots_octave.m plots_matlab.m ;
do cp -v $src/$i $trgt/ ;
done

rm knnl-0.1.0.*
zip knnl-0.1.0.zip $trgt/*
tar -vzcf knnl-0.1.0.tar.gz $trgt/*
tar -vjcf knnl-0.1.0.tar.bz2 $trgt/*

