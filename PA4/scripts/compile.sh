#!/bin/bash

cd src/
shopt -s extglob
if_fail=$(make)

if (echo "$if_fail" | grep failed)
then
	echo "Compilation failed."
else
	mv cgen ../
	rm *.o 
	rm !(cgen.cc|cgen.h|Makefile|emit.h|README*|cool-tree.h|cgen_supp.cc)
	cd ..
	clear
fi
