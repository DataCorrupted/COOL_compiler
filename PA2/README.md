# PA2: cool parser

This folder originates from Stanford CS143. Missing folders and files(mainly binary, which is not suitable for git) can be retrived from their [couse page](http://web.stanford.edu/class/cs143/).(Or [ShanghaiTech](http://sist.shanghaitech.edu.cn/faculty/songfu/course/spring2018/CS131/)'s CS131 should the course page work by the time you see this.)

## src

	cool.y 		is my code. You can read them or modify them.
	Makefile 	Use command "make" to generate a binary. You should not modify them.
	cool.output 	After you _make_, this file will be generated to illustrate the state machine 
			of the parser.

## test

	examples/ 	conatins all the codes I found from Github. It's not my work. You may check 
			the README.md inside that folder to find the writer(https://github.com/shinezyy/).

## scripts

	compiler.sh 	While compling can be tiresome, I write a script for that. It will generate
			a binary and put it in PA2/
	cmp.sh <file>	will compile your code, run lexer and compare your parser's output and 
			official's(tools-bin	/parser) given the input <file> you specified.

The environment for these two scripts should be in _PA2/_, or they may not work. Run them by
	
	./scripts/<script>.sh {file}

## cool-support

cool-support is provided by CS143, yet it contains some important files that you should really read and understand.