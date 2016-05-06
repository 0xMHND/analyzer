#!/usr/bin/env python

#/***************************
 #* input: Binary file cntaining the sympols of the functions. 
 #*                  Trace File from the trace generator
 #*                  The .dbg file for the program to parse.
 #* output: Parsed file of the functions called, number of instruction before called, when returned, and a summary at the end.
                
 #* description: This script analyzis the raw trace file and identify the functions called and the place where they where called
 #*              The output the results in a file with a summay at the end containing how many times a function is called
 #*                 And the number of instructions it consumed.
#***************************/

import re
import os
import sys
import getopt
import subprocess

#/***************************
 #* input:  none
 #* output: none
 #* description: This function defines how to use the program.
#***************************/
def usage():
    print 'Usage:', sys.argv[0], '[-d] [-p path] -b <xe-binary> -l <xe-log>'

#/***************************
 #* input: none 
 #* output: none
 #* description: The main function. It goes through each line of a trace file and identifies
 #* jumps and return instructions. With this and the symbols from another file, it determines
 #* which OCR functions are called and in which order.
#***************************/
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

    # This loop goes through the symbols file and matches
    # each symbol address with the name it represents
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

    # 3 regex, one for each line, one for getting register outputs and one for getting
    # register inputs when "dereferencing" a pointer
    p = re.compile("\[PC (?P<addr>[x0-9a-zA-Z]+)\].*Executed")
    p2 = re.compile("\[PC (?P<addr>[x0-9a-zA-Z]+)\].*Executed '(?P<name>[A-Za-z1-9]+)': Output: r[0-9]+=(?P<arg>[x0-9a-f]+)")
    p3 = re.compile("\[PC (?P<addr>[x0-9a-zA-Z]+)\].*Executed '(?P<name>[A-Za-z1-9]+)': Input: r0=(?P<arg1>[x0-9a-f]+), r[0-9]+=(?P<arg2>[x0-9a-f]+)")


    counter = 0
    inst_count_since_beg = 0
    prev_k = ""
    depth = 0
    count = 0
    counts = {-1:-1}
    name = ""
    c_list = []
    d_list = []
    c_cnt = 0
    d_cnt = 0
    look_for_args = [0,0]
    look_for_addr = 0
    look_for_id = 0
    incr_count = 0

    # This loop reads through a trace file and grabs each executed instruction name
    # If the instruction represents a jump to a symbol address representing an OCR function
    # Then the program writes that function name along with other information to an output file
    # It also keeps track of returns to figure out when each OCR function call ends.
    for l in log.stdout:
	inst_count_since_beg += 1
        l = l.rstrip(os.linesep)

        w = p.search(l)
        if w is None:
            print "Badly parsed fetch line!?"
            sys.exit(-2)

        addr = int(w.group('addr'), 16)

	# If a ocrDbCreate() call was issued, look in the next few lines 
	# for the registers that contain the function call's arguments
        if look_for_args[0] > 0:
            v = p2.search(l)
            if v is not None and v.group('name') == 'bit64op1i':
                c_list[c_cnt].insert(0, v.group('arg'))
                look_for_args[0]+=1
            elif look_for_args[0]==7:
                c_id = c_list[c_cnt][0]
                c_addr = c_list[c_cnt][1]
                look_for_args[0]=0
                incr_count = 1
                look_for_addr = 1
                look_for_id = 1
	# If an ID arguments was found, look for the actual
	# value the pointer is pointing to
        if look_for_id > 0:
            v = p3.search(l)
            if v is not None and v.group('name') == 'store64ri' and v.group('arg2') == c_id:
                c_list[c_cnt].pop(0)
                c_list[c_cnt].insert(0, v.group('arg1'))
                look_for_id = 0
                incr_count += 1
	# If an address pointer was found, look for the actual
	# address this pointer is pointing to
        if look_for_addr > 0:
            v = p3.search(l)
            if v is not None and v.group('name') == 'store64ri' and v.group('arg2') == c_addr:
                c_list[c_cnt].pop(1)
                c_list[c_cnt].insert(1, v.group('arg1'))
                look_for_addr = 0
                incr_count += 1
        if incr_count >= 3: 
            c_cnt += 1
            incr_count = 0 

	# If an ocrDbDestroy() call was issued, look for the function
	# argument in the next few lines
        if look_for_args[1] > 0:
            u = p2.search(l)
            if u is not None and u.group('name') == 'bit64op1i':
                d_list[d_cnt].insert(0, u.group('arg'))
                look_for_args[1]+=1
            elif look_for_args[1]==2:
                d_id = d_list[d_cnt][0]
                look_for_args[1]=0
                d_cnt += 1

        # After the addresss had been parsed in previous line
        # Check to see if it matches any parsed symbols
        # and if it does, write results to output
        for k in beg:
            if beg[k] <= addr and end[k] > addr:
                offset = addr - beg[k]
                count = count + 1
                   
                if k != prev_k:
                    if offset == 0:
                        depth = depth+1
                        counts[depth] = counter
                        print "%8ld"%count, w.group('addr'), "/%3d"%depth, ' |'*depth ,  "%s()"%k, "%ld"%inst_count_since_beg
                        if k == 'ocrDbCreate':
                            look_for_args[0] = 1
                            c_list.append([])
                        elif k == 'ocrDbDestroy':
                            look_for_args[1] = 1
                            d_list.append([])

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


    # print the ocrDbCreate() argument list that was saved
    # 2 loops are needed because it is a 2d array
    print '\n----- ocrDbCreate() arguments -----', "%d"%c_cnt
    for i in range(c_cnt):
        print 'ocrDbCreate('
        for j in range(len(c_list[i])):
			print '\t', c_list[i][j]
        print ")"

    # print the ocrDbDestroy() argument list that was saved
    # 2 loops are needed because it is a 2d array
    print '\n----- ocrDbDestroy() arguments -----', "%d"%d_cnt
    for i in range(d_cnt):
        print 'ocrDbDestroy('
        for j in range(len(d_list[i])):
			print '\t', d_list[i][j]
        print ")"

    # print more information if in debug mode
    # mainly number of calls and addresses used
    if debug > 0:
        print '    Addresses resolved:', counter
        print ''
        print '----- call count -----'
        print ''
        d_view = [ (v,k) for k,v in stats_count.iteritems() ]
        d_view.sort(reverse=True) # natively sort tuples by first element
        for v,k in d_view:
            if (v != 0):
                print "%s: %d" % (k,v)

        print ''
        print '----- instruction count (total) -----'
        print ''
        d_view = [ (v,k) for k,v in stats_inst.iteritems() ]
        d_view.sort(reverse=True) # natively sort tuples by first element
        for v,k in d_view:
            if (v != 0):
                print "%s: %d" % (k,v)
# main end

if __name__ == "__main__":
    main()
