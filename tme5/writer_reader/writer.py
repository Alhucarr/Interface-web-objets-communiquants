#!/usr/bin/env python
import os, time

pipe_name = '/tmp/myfifo'

if not os.path.exists(pipe_name):
    os.mkfifo(pipe_name)

pipe_out = open(pipe_name,'w')

i=0;
while i < 5:
    pipe_out.write("hello %d fois from python\n" % (i+1,))
    pipe_out.flush()
    time.sleep(1)
    i=i+1

pipe_out.write("end\n")
