#!/usr/bin/python3

import os
import sys
from colors import bcolors
def compare(file):
	common_str = "./tools-bin/lexer " + file + " | ./tools-bin/parser | "
	bcolors.print(bcolors.BOLD, "Generating your output...")
	os.system(common_str + "./semant >O1")
	bcolors.print(bcolors.BOLD, "Generating standard output...")
	os.system(common_str + "./tools-bin/semant >O2")
	result = os.system("cmp O1 O2")
	return result >> 8

if __name__ == "__main__":
	os.system("./scripts/compile.sh")
	if (len(sys.argv) >= 2):
		compare(sys.argv[1])
	os.system("rm semant")