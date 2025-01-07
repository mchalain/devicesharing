#include <unistd.h>

#include "devicesharing.h"

int devicesharing_register(const char *name, int fd, unsigned long capabilities)
{
	return -1;
}

int devicesharing_request(const char *name, unsigned long capabilities)
{
	return -1;
}

void devicesharing_close(int fd)
{
	if (fd >= 0)
		close(fd);
}
