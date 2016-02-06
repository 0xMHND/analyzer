import sys, re

#This regular expression finds each ocrDbCreate() function in the code
#It also saves the whole function in a group, as well as the individual arguments
#This is implemented with a negative lookahead so that each function is only matched once
DbCreate_pattern = re.compile(r"(ocrDbCreate\(([^;]+),([^;]+),([^;]+),([^;]+),([^;]+),([^;]+)\)[^ \t\n\r\f\v]*;)(?!\/\*Auto-Generated Print:\*\/)",re.MULTILINE)
#This string is what replaces an ocrDbCreate() match.
#It is composed of the function itself, followed by a comment to identify that the following is automatically modified
#followed by a PRINTF statement of clock() and all the important arguments of the function
DbCreate_substr = "\\1/*Auto-Generated Print:*/PRINTF(\"DbCreate: %p %p %lu %#x\\\\n\",*(\\2),\\3,\\4,\\5);"

#Same for ocrDbDestroy()
DbDestroy_pattern = re.compile(r"(ocrDbDestroy\(([^;]+)\)[^ \t\n\r\f\v]*;)(?!\/\*Auto-Generated Print:\*\/)",re.MULTILINE)
DbDestroy_substr = "\\1/*Auto-Generated Print:*/PRINTF(\"DbDestroy: %p\\\\n\",\\2);"

#Same for ocrDbRelease()
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
