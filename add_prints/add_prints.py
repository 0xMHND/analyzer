from __future__ import print_function #Python 3.x only?
import sys, re

DbCreate_pattern = re.compile(r"(ocrDbCreate\(([^;]+),([^;]+),([^;]+),([^;]+),([^;]+),([^;]+)\)[^ \t\n\r\f\v]*;)(?!\/\*Auto-Generated Print:\*\/)",re.MULTILINE)
DbCreate_substr = "\\1/*Auto-Generated Print:*/PRINTF(\"DbCreate: db=%p addr=%p len=%lu flags=0x%x\\\\n\",\\2,\\3,\\4,\\5);"

def main(filename):

	old_file = open(filename, 'r')
	s = old_file.read(-1)
	old_file.close()

	s = DbCreate_pattern.sub(DbCreate_substr, s)

	new_file = open("output.c", 'w')
	new_file.write(s)
	new_file.close();

	print("Done.")
	


if __name__=="__main__":
	if len(sys.argv) == 2: main(sys.argv[1]);
	else:
		print("Usage: python3 ", sys.argv[0], " <filename>")
		sys.exit(1)
