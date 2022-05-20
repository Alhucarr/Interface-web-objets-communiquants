#ifndef COMMON
#define COMMON
//socket,buffer,buffer_size,nbBytes sent,nbCall recv
int sendTCP(int,const char*,size_t,unsigned int*,unsigned int*);
//socket,buffer,buffer_size,nbBytes recv,nbCall recv
int recvTCP(int,char*,size_t,unsigned int*,unsigned int*);
#endif