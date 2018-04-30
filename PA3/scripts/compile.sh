cd src/
make
cp semant ../
shopt -s extglob
rm *.o 
rm !(semant.cc|semant.h|Makefile|cool-tree.h*)
cd ..