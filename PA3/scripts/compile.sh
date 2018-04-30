cd src/
make semant
cp semant ../
shopt -s extglob
rm *.o 
rm !(semant.cc|semant.h|Makefile|cool-tree.h*)
cd ..