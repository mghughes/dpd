#!/usr/bin/python

# ave-col.py
# takes average of columns in a file

from numpy import *
import sys

# parser for CL args
from optparse import OptionParser
parser = OptionParser()
parser.add_option('-f', action="store", dest="name", help="File name")
parser.add_option('-w', action="store", type=int, dest="col", default=0, help="Column to average. 0 for all columns, other num for num of col")
parser.add_option('-n', action="store", type=int, dest="num_col", default=1, help="Number of columns in file")

(options,args) = parser.parse_args()

# error messages for args
if not options.name:
    parser.error('filename not given')
if not options.num_col > 0 and options.num_col < 6:
    parser.error('number of columns must be in the range 1-5')
if not options.col >= 0 and options.col <= options.num_col:
    parser.error('column choice must be in the range 0-num_col')

# read data
if options.num_col == 1:
	x1 = loadtxt(options.name, unpack=True)
elif options.num_col == 2:
	x1,x2 = loadtxt(options.name, unpack=True)
elif options.num_col == 3:
	x1,x2,x3 = loadtxt(options.name, unpack=True)
elif options.num_col == 4:
    x1,x2,x3,x4 = loadtxt(options.name, unpack=True)
else:
    x1,x2,x3,x4,x5 = loadtxt(options.name, unpack=True)

# print averages
if options.col == 0:
    if options.num_col == 1:
        print '%f %f' % (mean(x1), std(x1)/sqrt(len(x1)))
    if options.num_col == 2:
        print '%f %f' % (mean(x2), std(x2)/sqrt(len(x2)))
    if options.num_col == 3:
        print '%f %f' % (mean(x3), std(x3)/sqrt(len(x3)))
    if options.num_col == 4:
        print '%f %f' % (mean(x4), std(x4)/sqrt(len(x4)))
    if options.num_col == 5:
        print '%f %f' % (mean(x5), std(x5)/sqrt(len(x5)))

elif options.col == 1:
    print '%f %f' % (mean(x1), std(x1)/sqrt(len(x1)))
elif options.col == 2:
    print '%f %f' % (mean(x2), std(x2)/sqrt(len(x2)))
elif options.col == 3:
    print '%f %f' % (mean(x3), std(x3)/sqrt(len(x3)))
elif options.col == 4:
    print '%f %f' % (mean(x4), std(x4)/sqrt(len(x4)))
elif options.col == 5:
    print '%f %f' % (mean(x5), std(x5)/sqrt(len(x5)))
