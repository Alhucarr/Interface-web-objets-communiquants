#!/usr/bin/env python
import os, time

pipe_name = '/tmp/myfifo'

if not os.path.exists(pipe_name):
    os.mkfifo(pipe_name)

pipe_in = open(pipe_name,'r')
while str != "end\n" :
    str = pipe_in.readline()
    print '%s' % str,
