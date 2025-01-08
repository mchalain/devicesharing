#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "devicesharing.h"

int help(const char *process)
{
	fprintf(stderr, "%s [-h][-d path][-u hex]\n");
	fprintf(stderr, "\th     \tprint this message\n");
	fprintf(stderr, "\td path\tset the device path\n");
	fprintf(stderr, "\tu hex\tset the usage capabilities\n");
	return -1;
}

int main(int argc, char * const argv[])
{
	const char *devicepath = "/dev/random";
	unsigned long usage = 0x0001;

	int opt;
	do
	{
		opt = getopt(argc, argv, "hd:");
		switch (opt)
		{
			case 'h':
				return help(argv[0]);
			case 'd':
				devicepath = optarg;
			break;
			case 'u':
				usage = strtoul(optarg, NULL, 16);
			break;
		}
	} while(opt != -1);

	int devicefd = open(devicepath, O_RDWR);
	if (devicefd > 0)
	{
		if (devicesharing_register(devicepath, devicefd, 0x0005))
			fprintf(stderr, "device %s registered\n", devicepath);
	}
	int fd = devicesharing_request(devicepath, usage);
	if (fd > 0)
	{
		fprintf(stderr, "device %s acquired\n", devicepath);
		devicefd = fd;
	}
	if (devicefd > 0)
	{
		fprintf(stderr, "device %s opened\n", devicepath);
		int c = 0;
		while (c == 0)
		{
			c = fgetc(stdin);
		}
		devicesharing_close(devicefd);
	}
	return 0;
}
