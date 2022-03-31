#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define MAXServerResquest 1024

int main()
{
    int     f2s, s2f;                                       // fifo file descriptors
    char    *f2sName = "/tmp/f2s_fw";                       // filo names
    char    *s2fName = "/tmp/s2f_fw";                       //
    char    serverRequest[MAXServerResquest];               // buffer for the request
    fd_set  rfds;                                           // flag for select
    struct  timeval tv;                                     // timeout
    tv.tv_sec = 1;                                          // 1 second
    tv.tv_usec = 0;                                         //

    mkfifo(s2fName, 0666);                                  // fifo creation
    mkfifo(f2sName, 0666);

    /* open both fifos */
    s2f = open(s2fName, O_RDWR);                            // fifo openning
    f2s = open(f2sName, O_RDWR);

    do {
        FD_ZERO(&rfds);                                     // erase all flags
        FD_SET(s2f, &rfds);                                 // wait for s2f

        if (select(s2f+1, &rfds, NULL, NULL, &tv) != 0) {   // wait until timeout
            if (FD_ISSET(s2f, &rfds)) {                     // something to read
                int nbchar;
                if ((nbchar = read(s2f, serverRequest, MAXServerResquest)) == 0) break;
                serverRequest[nbchar]=0;
                fprintf(stderr,"%s", serverRequest);
                write(f2s, serverRequest, nbchar);
            }
        }
    }
    while (1);

    close(f2s);
    close(s2f);

    return 0;
}
