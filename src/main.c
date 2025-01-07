#include <unistd.h>

#include "daemonize.h"
#include "log.h"

#define MODE_DAEMONIZE 0x0001

int help(const char *process)
{
	fprintf(stderr, "%s [-h][-W dir][-D][-L file][-P file][-U user]\n");
	fprintf(stderr, "\th     \tprint this message\n");
	fprintf(stderr, "\tW dir \tchroot the working directory\n");
	fprintf(stderr, "\tD     \tdaemonize the process\n");
	fprintf(stderr, "\tL file\tset the logfile\n");
	fprintf(stderr, "\tP file\tset the pid file\n");
	fprintf(stderr, "\tU user\tset the process owner\n");
	return -1;
}

int main(int argc, char * const argv[])
{
	const char *owner = NULL;
	const char *pidfile= NULL;
	const char *configfile = NULL;
	const char *logfile = "/var/log/"PACKAGE".log";
	const char *workingdir = NULL;
	int mode = 0;

	int opt;
	do
	{
		opt = getopt(argc, argv, "hDL:W:P:U:");
		switch (opt)
		{
			case 'h':
				return help(argv[0]);
			case 'D':
				mode |= MODE_DAEMONIZE;
			break;
			case 'L':
				logfile = optarg;
			break;
			case 'P':
				pidfile = optarg;
			break;
			case 'U':
				owner = optarg;
			break;
			case 'W':
				workingdir = optarg;
			break;
		}
	} while(opt != -1);

	if ((mode & MODE_DAEMONIZE) && daemonize(pidfile) == -1)
		return 0;

	if (workingdir != NULL)
		changeworkingdir(workingdir);
	if (owner)
		setprocessowner(owner);

	killdaemon(pidfile);

	return 0;
}
