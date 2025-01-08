#ifndef __DEVICESHARING_H__
#define __DEVICESHARING_H__

#include <limits.h>

#include "unixsocket.h"

typedef enum devicesharing_cap_e devicesharing_cap_e;
enum devicesharing_cap_e {
	DEVICESHARING_CAP_INPUT_E,
	DEVICESHARING_CAP_OUTPUT_E,
	DEVICESHARING_CAP_CONTROL_E,
};

typedef struct devicesharing_msg_register_s devicesharing_msg_register_t;
struct devicesharing_msg_register_s {
	char name[PATH_MAX];
	int fd;
	devicesharing_cap_e capabilities;
};

typedef struct devicesharing_msg_request_s devicesharing_msg_request_t;
struct devicesharing_msg_request_s {
	char name[PATH_MAX];
	devicesharing_cap_e capabilities;
};

typedef struct devicesharing_msg_close_s devicesharing_msg_close_t;
struct devicesharing_msg_close_s {
	int fd;
	devicesharing_cap_e capabilities;
};

typedef struct devicesharing_msg_s devicesharing_msg_t;
struct devicesharing_msg_s
{
	enum {
		DEVICESHARING_CMD_REGISTER,
		DEVICESHARING_CMD_REQUEST,
		DEVICESHARING_CMD_CLOSE,
	} cmd;
	union {
		devicesharing_msg_register_t registering;
		devicesharing_msg_request_t requesting;
		devicesharing_msg_close_t closing;
	} data;
};
/**
 * server API
 */
typedef struct devicesharingd_s devicesharingd_t;

/**
 * @brief create the data to store devices information
 */
devicesharingd_t *devicesharingd_create();
/**
 * @brief callback client_receive_t for server
 *
 * @param data The internal server's information
 */
int devicesharingd_receive_cb(void *data, client_t *clt, const char *buffer, size_t length);
/**
 * @brief callback client_receivefd_t for server
 *
 * @param data The internal server's information
 */
int devicesharingd_receivefd_cb(void *data, client_t *clt, int fd);

/**
 * @brief free the devices information
 */
void devicesharingd_destroy(devicesharingd_t *data);

/**
 * client API
 */
/**
 * @brief register a file descriptor for a device
 *
 * @param name the device path
 * @param fd the file descriptor
 * @param capabilities the device description
 *
 * @return 0 on success otherwise -1
 */
int devicesharing_register(const char *name, int fd, unsigned long capabilities);

/**
 * @brief request a file descriptor with some capabilities
 *
 * @param name the device path
 * @param capabilities the device capabilities requested
 *
 * @return -1 on error otherwise the file descriptor
 */
int devicesharing_request(const char *name, unsigned long capabilities);

/**
 * @brief close the file descriptor
 *
 * @param fd the file descriptor returned by @register@
 */
void devicesharing_close(int fd);

#endif
