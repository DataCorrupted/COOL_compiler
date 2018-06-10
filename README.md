# compiler_proj
Project done for Compiler in ShanghaiTech, which is also the project in Stanford.

## Examples
	cool_examples/ 	conatins all the codes I found from Github. It's not my work. You may check 
			the README.md inside that folder to find the writer(https://github.com/shinezyy/).

## PA1

A lexer written for COOL. Flex used.

There are potential bugs, see README inside.

## PA2

A parser written for COOL. [bison](http://gnu.org/software/bison) used. 

Do notice that PA2 is a little bit different from that in Stanford.

My _cool.y_ couldn't correctly print all the error log, but the parser functions as long as the program is correct.

## PA3

A semantic analysis written for COOL.

## PA4

Code generator.

## PA\*/tools-bin/cgen

This file is compiled under 32bit machine, you need to install the _libc_ support before you can run it on 64bit machine:

```shell
sudo apt-get install libc6-i386 lib32stdc++6 
```