cd ..
./compile.sh
clear
./lexer $1 >O1
./tools-bin/lexer $1 >O2
cmp O1 O2
rm lexer src/lexer src/lex.yy.c