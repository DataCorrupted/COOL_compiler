# PA1: cool lexer

This folder originates from Stanford CS143. Missing folders and files(mainly binary, which is not suitable for git) can be retrived from their [couse page](http://web.stanford.edu/class/cs143/).(Or [ShanghaiTech](http://sist.shanghaitech.edu.cn/faculty/songfu/course/spring2018/CS131/)'s CS131 should the course page work by the time you see this.)

## src

	cool.flex 	is my code. You can read them or modify them.
	Makefile 	Use command "make" to generate a binary. You should not modify them.

## test

	unit/ 		includes all the unit test I wrote for testing each components.
	public_testcases/
			test cases we used to grade.
	flex_test.cl 	is the test file provided by CS143. It's tiny.

## scripts

	compiler.sh 	While compling can be tiresome, I write a script for that. It will generate
			a binary and put it in PA1/
	cmp.sh <file>	will compile your code, compare your lexer's output and official's(tools-bin 
			/lexer) given the input <file> you specified.

The environment for these two scripts should be in _PA1/_, or they may not work. Run them by
	
	./scripts/<script>.sh {file}

## cool-support

cool-support is provided by CS143, yet it contains some important files that you should really read and understand.

# Final grade

You can find the test cases we used in _test/public_testcases_.
I got the following wrong.

	bothcomments 				mixing the two types of comments
	endcomment				The end comment token by itself
	escapednull 				escaped null char in a string
	longstring_escapedbackslashes		long strings with escaped characters
	pathologicalstrings			some pathological strings
	s32					end of comment without start
	wq0607-c2				sequences of block comments and strings