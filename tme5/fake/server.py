#!/usr/bin/env python
import os, time

s2fName = '/tmp/s2f_atd'
if not os.path.exists(s2fName):
   os.mkfifo(s2fName)
s2f = open(s2fName,'w+')

f2sName = '/tmp/f2s_atd'
if not os.path.exists(f2sName):
   os.mkfifo(f2sName)
f2s = open(f2sName,'r')

data_in = raw_input("Veuillez raconter votre vie : \n")

s2f.write(data_in)
s2f.flush()
str = f2s.readline()
print '%s' % str,

f2s.close()
s2f.close()
