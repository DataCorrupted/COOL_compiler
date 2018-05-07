#!/usr/bin/python3

import os
import sys
from cmp import compare

def main():
	os.system("./scripts/compile.sh")
	os.system("clear")

	if (len(sys.argv) == 2):
		path = sys.argv[1]
	else:
		path = "test/unit/"	

	for f in os.listdir(path):
		if not (f[-3:] == ".cl"):
			continue
		print(f)
		result = compare(path + f)
		if result == 0:
			print(f + " succeeded.")
		else:
			print(f + " encountered problem")
		print("\n")
		input()
	os.system("rm semant")

if __name__ == "__main__":
	main();
