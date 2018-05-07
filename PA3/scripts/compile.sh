#!/bin/bash
cd src/
make semant
mv semant ../
shopt -s extglob
rm *.o 
rm !(semant.cc|semant.h|Makefile|cool-tree.h*)
cd ..
clear