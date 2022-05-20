#include <sys/socket.h>
#include "common.h"

int sendTCP(int socket, const char* buffer, size_t size, unsigned int* nbBytesSent, unsigned int* nbCallSend)
{
  ssize_t sent;
  ssize_t total_sent = 0;

  unsigned int cptrSend = 0;

  while(total_sent < size)
  {
    sent = send(socket, buffer+total_sent, size-total_sent,0);
    cptrSend++;
    if(sent <= 0)
    {
      *nbBytesSent += total_sent;
      *nbCallSend += cptrSend;

      return sent==0?0:-1;
    }

    total_sent += sent;
  }

  *nbBytesSent += total_sent;
  *nbCallSend += cptrSend;

  return 1;
}
int recvTCP(int socket, char* buffer, size_t buffer_size, unsigned int* nbBytesRecv, unsigned int* nbCallRecv)   
{
  ssize_t recv_;
  ssize_t total_recv = 0;

  unsigned int cptrRecv = 0;

  while(total_recv < buffer_size)
  {
    recv_ = recv(socket, buffer+total_recv, buffer_size-total_recv, 0);
    cptrRecv++;

    if(recv_ <= 0)
    {
      *nbBytesRecv += total_recv;
      *nbCallRecv += cptrRecv;

      return recv_==0?0:-1;
    }

    total_recv += recv_;
  }
  
  *nbBytesRecv += total_recv;
  *nbCallRecv += cptrRecv;

  return 1;
}