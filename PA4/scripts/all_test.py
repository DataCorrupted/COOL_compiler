#!/usr/bin/python3

import os
import sys
from cmp import compare
from colors import bcolors

def usage():
	print("usage:")
	print(" ./scripts/all_test.py [path] [options]")
	print("  path: by default(leaving empty) it's ./test/unit, you can specify your self.")
	print("        This script will test all files ending with '.cl'")
	print("  options: ")
	print("	-S: stop only when the you have a failed test.")

def pause():
	print("Press enter to continue...")
	input()

def main():
	if "-h" in sys.argv: 
		usage()
		return

	stop_when_false = "-S" in sys.argv;
	if "-S" in sys.argv:
		sys.argv.remove("-S")

	os.system("./scripts/compile.sh")
	os.system("clear")

	if (len(sys.argv) >= 2):
		path = sys.argv[1]
	else:
		path = "test/unit/"	
	bcolors.print(bcolors.WARNING, "No path specified, using ./test/unit")
	pause()
	for f in os.listdir(path):
		if not (f[-3:] == ".cl"):
			continue
		bcolors.print(bcolors.OKBLUE, "Testing " + f + "...")
		result = compare(path + f)
		if result == 0:
			bcolors.print(bcolors.OKGREEN, f + " succeeded.")
		else:
			bcolors.print(bcolors.FAIL, f + " encountered problem")
		if (stop_when_false and result == 1) or not stop_when_false:
			pause();
		print()
	os.system("rm cgen")

if __name__ == "__main__":
	main();
