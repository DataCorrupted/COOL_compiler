# Such usage aviods any extra package and should work on all platforms.
# Check this:
# https://stackoverflow.com/questions/287871/print-in-terminal-with-colors
class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[1;94m'
    OKGREEN = '\033[1;92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    NORM = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

    def print(col, msg):
    	print(col + msg + bcolors.NORM)