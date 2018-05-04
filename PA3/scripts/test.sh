#! /bin/bash

# make first
cd src
make
cd ..

echo 
echo "-----------------------------------------------"
echo 

# check # of arguments
if [[ $# -lt 1 ]]; then
	echo "Illegal number of parameters"
	exit 1
fi

echo "Using testcase: $1"

# reference
./tools-bin/lexer $1 | ./tools-bin/parser | ./tools-bin/semant > ./ref_out
./tools-bin/lexer $1 | ./tools-bin/parser > ./ref_int

# my result
./tools-bin/lexer $1 | ./tools-bin/parser | ./src/semant > ./my_out
./tools-bin/lexer $1 | ./tools-bin/parser > ./my_int

# diff
diff ref_out my_out
echo 
echo 
echo "-------------------Done-----------------------"