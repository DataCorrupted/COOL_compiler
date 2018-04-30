./scripts/compile.sh
clear
./tools-bin/lexer $1 | ./parser | ./semant >O1
./tools-bin/lexer $1 | ./tools-bin/parser | ./tools-bin/semant >O2
rm semant
cmp O1 O2
