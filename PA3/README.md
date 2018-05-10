# PA3: cool semantic analysis

This folder originates from Stanford CS143. Missing folders and files(mainly binary, which is not suitable for git) can be retrived from their [couse page](http://web.stanford.edu/class/cs143/).(Or [ShanghaiTech](http://sist.shanghaitech.edu.cn/faculty/songfu/course/spring2018/CS131/)'s CS131 should the course page work by the time you see this.)

## src

	semant.(cc/h)	are my code. You can read them or modify them.
	cool-tree.h* 	also my code.
	Makefile 	Use command "make" to generate a binary. You should not modify them. By 		default we uses C++09.

## test

	(good/bad).cl 	are provided test. 
	unit/ 		I will write my own unit test when I have time.

## scripts

	compile.sh 	While compling can be tiresome, I write a script for that. It will generate
			a binary and put it in PA3/
	cmp.py <file>	will compile your code, run lexer and compare your parser's output and 
			official's(tools-bin/semant) given the input <file> you specified.
	all_test.py <path> 
			Test all *.cl files in the given path. 

The environment for these two scripts should be in _PA3/_, or they may not work. Run them by
	
	./scripts/<script> {file}

## cool-support

cool-support is provided by CS143, yet it contains some important files that you should really read and understand.

