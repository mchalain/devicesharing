#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "devicesharing.h"
#include "log.h"

typedef struct device_s device_t;
struct device_s
{
	char name[PATH_MAX];
	unsigned long capabilities;
	unsigned long usage;
	unsigned long refcount;
	int fd;
	device_t *next;
	device_t *prev;
};

struct devicesharingd_s
{
	device_t* devices;
	devicesharing_msg_t *msg;
};

devicesharingd_t *devicesharingd_create()
{
	devicesharingd_t *data = calloc(1, sizeof(*data));
	return data;
}

void devicesharingd_destroy(devicesharingd_t *data)
{
	device_t *next = NULL;
	for (device_t* device = data->devices; device != NULL; device = next)
	{
		close(device->fd);
		next = device->next;
		free(device);
	}
	free(data);
}

static int _devicesharingd_register(devicesharingd_t *data, client_t *client, devicesharing_msg_register_t *msg)
{
	int status = -1;
	device_t* device = NULL;
	for (device = data->devices; device != NULL; device = device->next)
	{
		if (device->fd == msg->fd)
			break;
	}
	if (device == NULL)
	{
		device = calloc(1, sizeof(*device));
		if (data->devices)
			data->devices->prev = device;
		device->next = data->devices;
		data->devices = device;
		device->capabilities = msg->capabilities;
		device->usage = msg->capabilities;
		device->fd = msg->fd;
		/// decrease the size of 1 to keep the last zero of the string
		memcpy(device->name, msg->name, sizeof(device->name) - 1);
		status = 0;
	}
	client_send(client, &status, sizeof(status));
	return 0;
}

static int _devicesharingd_request(devicesharingd_t *data, client_t *client, devicesharing_msg_request_t *msg)
{
	int status = -1;
	int fd = -1;
	device_t* device = NULL;
	for (device = data->devices; device != NULL; device = device->next)
	{
		if (!strcmp(device->name, msg->name))
			break;
	}
	if (device != NULL)
	{
		if ((device->capabilities & msg->capabilities) &&
				(device->usage & msg->capabilities))
		{
			device->usage &= ~msg->capabilities;
			fd = device->fd;
			device->refcount++;
			status = 0;
		}
	}
	client_sendfd(client, &status, sizeof(status), fd);
	return 0;
}

static int _devicesharingd_close(devicesharingd_t *data, client_t *client, devicesharing_msg_close_t *msg)
{
	int status = -1;
	device_t* device = NULL;
	for (device = data->devices; device != NULL; device = device->next)
	{
		if (device->fd == msg->fd)
			break;
	}
	if (device)
	{
		/// decrease the refcount
		device->refcount--;
		/// disable the free of the device
		if (device->refcount)
			device = NULL;
		device->usage |= msg->capabilities;
		status = 0;
	}
	if (device)
	{
		/// remove the device form the list
		if (device->prev)
			device->prev->next = device->next;
		if (device->next)
			device->next->prev = device->prev;
		close(device->fd);
		free(device);
	}
	client_send(client, &status, sizeof(status));
	return 0;
}

int devicesharingd_receive_cb(void *data, client_t *clt, const char *buffer, size_t length)
{
	devicesharing_msg_t *msg = (devicesharing_msg_t *)buffer;
	((devicesharingd_t *)data)->msg = msg;
	switch (msg->cmd)
	{
		case DEVICESHARING_CMD_REGISTER:
		break;
		case DEVICESHARING_CMD_REQUEST:
			warn("devicesharing request");
			_devicesharingd_request(data, clt, &(msg->data.requesting));
		break;
		case DEVICESHARING_CMD_CLOSE:
		break;
	}
	return 0;
}

int devicesharingd_receivefd_cb(void *data, client_t *clt, int fd)
{
	devicesharing_msg_t *msg = ((devicesharingd_t *)data)->msg;
	switch (msg->cmd)
	{
		case DEVICESHARING_CMD_REGISTER:
			warn("devicesharing register");
			msg->data.registering.fd = fd;
			_devicesharingd_register(data, clt, &(msg->data.registering));
		break;
		case DEVICESHARING_CMD_REQUEST:
		break;
		case DEVICESHARING_CMD_CLOSE:
			warn("devicesharing close");
			msg->data.closing.fd = fd;
			_devicesharingd_close(data, clt, &(msg->data.closing));
		break;
	}
	return 0;
}
