#ifndef __DEVICESHARING_H__
#define __DEVICESHARING_H__

enum{
	DEVICESHARING_CAP_INPUT_E,
	DEVICESHARING_CAP_OUTPUT_E,
	DEVICESHARING_CAP_CONTROL_E,
};

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
