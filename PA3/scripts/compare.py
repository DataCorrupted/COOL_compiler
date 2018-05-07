#!/usr/bin/python3

import os
import sys

def compare(file):
	os.system("./tools-bin/lexer " + file + " | ./tools-bin/parser | ./semant >O1")
	os.system("./tools-bin/lexer " + file + " | ./tools-bin/parser | ./tools-bin/semant >O2")
	result = os.system("cmp O1 O2")
	return result >> 8

if __name__ == "__main__":
	os.system("./scripts/compile.sh")
	if (len(sys.argv) >= 2):
		compare(sys.argv[1])
	os.system("rm semant")