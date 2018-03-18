./scripts/compile.sh
./lexer $1 >O1
./tools-bin/lexer $1 >O2
rm lexer src/lexer
clear
cmp O1 O2
