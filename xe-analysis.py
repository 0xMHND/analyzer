#/***************************
 #* input: Binary file cntaining the sympols of the functions. 
 #*				  Trace File from the trace generator
 #*				  The .dbg file for the program to parse.
 #* output: Parsed file of the functions called, number of instruction before called, when returned, and a summary at the end.
				
 #* description: This script analyzis the raw trace file and identify the functions called and the place where they where called
 #* 		     The output the results in a file with a summay at the end containing how many times a function is called
 #*				 And the number of instructions it consumed.
#***************************/

#!/usr/bin/python

import re
import os
import sys
import getopt
import subprocess

#This function defines how to use the program.
def usage():
    print 'Usage:', sys.argv[0], '[-d] [-p path] -b <xe-binary> -l <xe-log>'

#The main function.
def main():
    # parse cmdline
    try:
        optlist, args = getopt.getopt(sys.argv[1:], 'dp:b:l:')
    except getopt.GetoptError as err:
        print str(err)
        usage()
        sys.exit(-1)

    # default options
    debug = 0
    fnbin = ""
    fnlog = ""
    path  = ""

    # handle parsed options
    for o, a in optlist:
        if o == "-d":
            debug = debug + 1
        elif o == "-p":
            path = a + '/'
        elif o == "-b":
            fnbin = a
        elif o == "-l":
            fnlog = a
        else:
            usage()
            sys.exit(-1)

    # sanity check
    if fnbin == "" or fnlog == "":
        usage()
        sys.exit(-1)

    # read symbols from binary
    print 'Reading symbols from binary', fnbin, '...'
    syms = subprocess.Popen([path + 'xstg-linux-elf-nm', '-n', fnbin], stdout=subprocess.PIPE)

    beg = {}
    end = {}

    prev_name = ""
    prev_addr = -1

    stats_inst = {}
    stats_count = {}

    for l in syms.stdout:
        l = l.rstrip(os.linesep)
        w = l.split()

        if prev_addr != -1:
            beg[prev_name] = prev_addr
            end[prev_name] = int(w[0], 16)

        prev_name = w[2]
        prev_addr = int(w[0], 16)
        stats_inst[prev_name] = 0;
        stats_count[prev_name] = 0;



    # add the last symbol
    beg[prev_name] = prev_addr
    end[prev_name] = int(0x7FFFFFFFFFFFFFFF)

    if debug > 1:
        print '    Symbols read:'
        for k, v in sorted(beg.items(), key=lambda x:x[1]):
            print '   %30s => [0x%016x..0x%016x)' % (k, beg[k], end[k])
    elif debug > 0:
        print '    Symbols read:', len(beg)


    # read log & attribute to symbols
    print 'Reading trace log file', fnlog, '...'
    log = subprocess.Popen(['grep', ' Executed ', fnlog], stdout=subprocess.PIPE)

    p = re.compile("\[PC (?P<addr>[x0-9a-zA-Z]+)\].*Executed")

    counter = 0

    prev_k = ""
    depth = 0
    count = 0
    counts = {-1:-1}

    for l in log.stdout:
        l = l.rstrip(os.linesep)

        w = p.search(l)
        if w is None:
            print "Badly parsed fetch line!?"
            sys.exit(-2)

        addr = int(w.group('addr'), 16)

        for k in beg:
            if beg[k] <= addr and end[k] > addr:
                offset = addr - beg[k]
                count = count + 1
                   
                if k != prev_k:
                    if offset == 0:
                        depth = depth+1
                        counts[depth] = counter
                        print "%8ld"%count, w.group('addr'), "/%3d"%depth, ' |'*depth ,  "%s()"%k
                    else:
                        inst = counter-counts[depth-1]
                        print "%8ld"%count, w.group('addr'), "/%3d"%depth, ' |'*depth, 'returned.', inst
                        stats_inst[k] += inst
                        stats_count[k] += 1
                        depth = depth-1
                    prev_k = k
                    count = 0

                counter = counter + 1
                if debug > 0:
                    continue

    if debug > 0:
        print '    Addresses resolved:', counter
    print ''
    print '----- call count -----'
    print ''
#    d_view = [ (v,k) for k,v in stats_count.iteritems() ]
#    d_view.sort(reverse=True) # natively sort tuples by first element
#    for v,k in d_view:
#        if (v != 0):
#            print "%s: %d" % (k,v)

    print ''
    print '----- instruction count (total) -----'
    print ''
#    d_view = [ (v,k) for k,v in stats_inst.iteritems() ]
#    d_view.sort(reverse=True) # natively sort tuples by first element
#    for v,k in d_view:
#        if (v != 0):
#            print "%s: %d" % (k,v)

if __name__ == "__main__":
    main()
