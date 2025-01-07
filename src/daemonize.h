#ifndef __DEAMONIZE_H__
#define __DEAMONIZE_H__

int daemonize(const char *pidfile);
void killdaemon(const char *pidfile);
int changeworkingdir(const char *workingdir);
int setprocessowner(const char *user);

#endif
