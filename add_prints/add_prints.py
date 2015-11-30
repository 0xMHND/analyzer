import sys, re

DbCreate_pattern = re.compile(r"(ocrDbCreate\(([^;]+),([^;]+),([^;]+),([^;]+),([^;]+),([^;]+)\)[^ \t\n\r\f\v]*;)(?!\/\*Auto-Generated Print:\*\/)",re.MULTILINE)
DbCreate_substr = "\\1/*Auto-Generated Print:*/PRINTF(\"DbCreate: %p %p %lu %#x\\\\n\",\\2,\\3,\\4,\\5);"

DbDestroy_pattern = re.compile(r"(ocrDbDestroy\(([^;]+)\)[^ \t\n\r\f\v]*;)(?!\/\*Auto-Generated Print:\*\/)",re.MULTILINE)
DbDestroy_substr = "\\1/*Auto-Generated Print:*/PRINTF(\"DbDestroy: %p\\\\n\",\\2);"

DbRelease_pattern = re.compile(r"(ocrDbRelease\(([^;]+)\)[^ \t\n\r\f\v]*;)(?!\/\*Auto-Generated Print:\*\/)",re.MULTILINE)
DbRelease_substr = "\\1/*Auto-Generated Print:*/PRINTF(\"DbRelease: %p\\\\n\",\\2);"


def main(filename):
	#Open filename into string s
	try:
		f_read = open(filename, "r")
		s = f_read.read(-1)
		f_read.close();
	except IOError as e:
		print("I/O read error({0}): {1}".format(e.errno, e.strerror))
	except:
		print("Unexpected error:", sys.exc_info()[0])
		raise

	#Add print statements after each ocrDb function call
	s = DbCreate_pattern.sub(DbCreate_substr, s)
	s = DbDestroy_pattern.sub(DbDestroy_substr, s)
	s = DbRelease_pattern.sub(DbRelease_substr, s)

	#Open filename and write string s to it
	try:
		f_write = open(filename, "w")
		f_write.write(s)
		f_write.close();
	except IOError as e:
		print("I/O write error({0}): {1}".format(e.errno, e.strerror))
	except:
		print("Unexpected error:", sys.exc_info()[0])
		raise

	#done
	print("Done.")
	

#start here
if __name__=="__main__":
	#Check that there is an entered filename
	if len(sys.argv) == 2: main(sys.argv[1]);
	#Otherwise print instructions and exit
	else:
		print("Usage: python3 ", sys.argv[0], " <filename>")
		sys.exit(1)
