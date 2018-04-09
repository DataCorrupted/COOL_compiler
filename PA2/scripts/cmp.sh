./scripts/compile.sh
./tools-bin/lexer $1 | ./parser $1 >O1
./tools-bin/lexer $1 | ./tools-bin/parser >O2
rm parser src/parser
clear
cmp O1 O2
