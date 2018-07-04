# PA2: cool parser

This folder originates from Stanford CS143. Missing folders and files(mainly binary, which is not suitable for git) can be retrived from their [couse page](http://web.stanford.edu/class/cs143/).(Or [ShanghaiTech](http://sist.shanghaitech.edu.cn/faculty/songfu/course/spring2018/CS131/)'s CS131 should the course page work by the time you see this.)

## src

	cool.y 		is my code. You can read them or modify them.
	Makefile 	Use command "make" to generate a binary. You should not modify them.
	cool.output 	After you _make_, this file will be generated to illustrate the state machine 
			of the parser.

## test

	bison_test_(good/bad).cl 
			provided correct and wrong code by Shanghaitech.
	unit 		unit test written by my self.
## scripts

	compiler.sh 	While compling can be tiresome, I write a script for that. It will generate
			a binary and put it in PA2/
	cmp.sh <file>	will compile your code, run lexer and compare your parser's output and 
			official's(tools-bin	/parser) given the input <file> you specified.

The environment for these two scripts should be in _PA2/_, or they may not work. Run them by
	
	./scripts/<script>.sh {file}

## cool-support

cool-support is provided by CS143, yet it contains some important files that you should really read and understand.

# Final grades & Bugs

	Your score is 92/100
	26/70 for testcases1
	56/30 for testcases2
	5/5  for bison features
	5/5  for comments

	failed on casenoexpr.test (case with no expr)  [submission crashed on this input]
	failed on ifnoelse.test (if expression without else)  [submission crashed on this input]
	failed on badfeaturenames.test (method and attribute names capitalized)  [submission crashed on this input]
	failed on nestedlet.test (nested let expressions with different numbers of bindings) 
	failed on multipleclasses.test (multiple classes in a file)  [submission crashed on this input]
	failed on baddispatch4.test (bad dispatch syntax)  [submission crashed on this input]
	failed on classnoname.test (class without a name)  [submission crashed on this input]
	failed on classbadname.test (class with incorrect name format (object not type))  [submission crashed on this input]
	failed on while.test (valid and invalid while loops)  [submission crashed on this input]
	failed on ifnofi.test (if expression with no fi)  [submission crashed on this input]
	failed on letassociativity.test (testing let associativity) 
	failed on baddispatch2.test (bad dispatch syntax)  [submission crashed on this input]
	failed on badfeatures.test (multiple errors in features in a class)  [submission crashed on this input]
	failed on badexprlist.test (bad expr lists )  [submission crashed on this input]
	 
	Score for this problem: 92.0