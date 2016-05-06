#!/usr/bin/env python

import re
import os
import sys
import getopt
import subprocess

def usage():
    print 'Usage:', sys.argv[0], '-l <xe-log>'

def main():
    # parse cmdline
    try:
        optlist, args = getopt.getopt(sys.argv[1:], 'vl:')
    except getopt.GetoptError as err:
        print str(err)
        usage()
        sys.exit(-1)

    fnlog = ""
    flag = 0

    # handle parsed options
    for o, a in optlist:
        if o == "-l":
            fnlog = a
        else:
            usage()
            sys.exit(-1)

    # sanity check
    if fnlog == "":
        usage()
        sys.exit(-1)

    print 'Reading trace log file', fnlog, '...'
    log = subprocess.Popen(['grep', ' Executed ', fnlog], stdout=subprocess.PIPE)

    p = re.compile(r"\[PC ([x0-9a-zA-Z]+)\].*Executed \'([a-zA-Z0-9_]+)\'.*r509.*")

    for l in log.stdout:

        l = l.rstrip(os.linesep)

        w = p.search(l)
        if w is not None:
            print w.group(0)
            flag = 1
        elif flag == 1:
            flag = 0
            print "--------------------------------"
# main end

if __name__ == "__main__":
    main()
