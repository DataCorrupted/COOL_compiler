../tools-bin/lexer $1 | ../tools-bin/parser | ../tools-bin/semant | ../src/cgen $2 > out.s
../tools-bin/lexer $1 | ../tools-bin/parser | ../tools-bin/semant | ../tools-bin/cgen > out_ref.s

echo "Output Generation Done"