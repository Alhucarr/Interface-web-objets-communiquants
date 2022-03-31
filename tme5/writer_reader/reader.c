#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define MAX_BUF 1024

int main()
{
	int		err;
	int		fd;
	char    *myfifo = "/tmp/myfifo";
	char	buf[MAX_BUF];

	/* open, read, and display the message from the FIFO */
	fd = open(myfifo, O_RDONLY);
	do {
		if ((err = read(fd, buf, MAX_BUF)) == 0) break;
		printf("%s", buf);
        fflush(stdout);
	}
    while (strcmp(buf,"end\n"));

	close(fd);

	return 0;
}
