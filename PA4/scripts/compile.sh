#!/bin/bash
cd src/
make
mv cgen ../
shopt -s extglob
rm *.o 
rm !(cgen.cc|cgen.h|Makefile|emit.h|README*|cool-tree.h|cgen_supp.cc)
cd ..
clear