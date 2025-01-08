#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "devicesharing.h"
#include "unixsocket.h"
#include "log.h"

static client_t *g_client = NULL;
static struct {
	int fd;
	int status;
} g_data;

static int _devicesharing_receive(void *data, client_t *client, const char *buffer, size_t length)
{
	if (data != &g_data)
		return -1;
	if (length != sizeof(int))
		return -1;
	g_data.status = *(int *)buffer;
	return 0;
}

static int _devicesharing_receivefd(void *data, client_t *client, int fd)
{
	if (data != &g_data)
		return -1;
	g_data.fd = fd;
	return 0;
}

int devicesharing_register(const char *name, int fd, unsigned long capabilities)
{
	warn("devicesharing register");
	if (g_client == NULL)
		return -1;
	devicesharing_msg_t msg = {
		.cmd = DEVICESHARING_CMD_REGISTER,
		.data = {
			.registering = {
				.fd = fd,
				.capabilities = capabilities
			}
		},
	};
	snprintf(msg.data.registering.name, sizeof(msg.data.registering.name) - 1, "%s", name);
	ssize_t ret = client_sendfd(g_client, &msg, sizeof(msg), fd);
	if (ret > 0)
	{
		fd_set rfds;
		FD_ZERO(&rfds);
		ret = client_wait(g_client, 0, &rfds);
		if (ret == 0)
			ret = g_data.status;
	}
	return (int)ret;
}

int devicesharing_request(const char *name, unsigned long capabilities)
{
	warn("devicesharing request");
	if (g_client == NULL)
		return -1;
	devicesharing_msg_t msg = {
		.cmd = DEVICESHARING_CMD_REQUEST,
		.data = {
			.requesting = {
				.capabilities = capabilities
			}
		},
	};
	snprintf(msg.data.requesting.name, sizeof(msg.data.requesting.name) - 1, "%s", name);
	ssize_t ret = client_send(g_client, &msg, sizeof(msg));
	if (ret > 0)
	{
		fd_set rfds;
		FD_ZERO(&rfds);
		int ret = client_wait(g_client, 0, &rfds);
		if (ret || g_data.status)
			ret = -1;
		else
			ret = g_data.fd;
	}
	return (int)ret;
}

void devicesharing_close(int fd)
{
	warn("devicesharing close");
	if (g_client == NULL && fd >= 0)
		close(fd);
	devicesharing_msg_t msg = {
		.cmd = DEVICESHARING_CMD_CLOSE,
		.data = {
			.closing = {
				.fd = fd,
			}
		},
	};
	ssize_t ret = client_sendfd(g_client, &msg, sizeof(msg), fd);
	if (ret > 0)
	{
		fd_set rfds;
		FD_ZERO(&rfds);
		client_wait(g_client, 0, &rfds);
	}
}

static __attribute__((constructor)) void _devicesharing_connect(void)
{
	const char *serverpath = "/tmp/"PACKAGE"_socket";
	const char *envpath = getenv("UNIXSOCKET_PATH");
	if (envpath)
		serverpath = envpath;
	g_client = client_create(serverpath);
	if (g_client)
	{
		client_attach_receive(g_client, _devicesharing_receive, (void*) &g_data);
		client_attach_receivefd(g_client, _devicesharing_receivefd, (void*) &g_data);
	}
}

static __attribute__((destructor)) void _devicesharing_destroy(void)
{
	if (g_client)
	{
		client_destroy(g_client);
	}
}
