#!/usr/bin/python3

import os
import sys
from colors import bcolors
def compare(file):
	common_str = "./tools-bin/lexer " + file + " | ./tools-bin/parser | ./tools-bin/semant | "
	bcolors.print(bcolors.BOLD, "Generating your output...")
	os.system(common_str + "./cgen >O1.asm")
	bcolors.print(bcolors.BOLD, "Generating standard output...")
	os.system(common_str + "./tools-bin/cgen >O2.asm")
	result = os.system("cmp O1.asm O2.asm")
	return result >> 8

if __name__ == "__main__":
	os.system("./scripts/compile.sh")
	if (len(sys.argv) >= 2):
		compare(sys.argv[1])
	else:
		print("Please provide file for me to test")
	os.system("rm cgen")