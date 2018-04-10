./scripts/compile.sh
clear
./tools-bin/lexer $1 | ./parser -v >O1
./tools-bin/lexer $1 | ./tools-bin/parser -v >O2
rm parser src/parser
cmp O1 O2
