# devicesharing
This is a little UNIX server and the client library to register an opened device and to share the access between process.

## Meaning
The process opening a device, register the name and the file descriptor of the device, and join the description of capabilities of the device (input, output, control).
After any process can request the same device for a role (input, output, control). The server responds with the file descriptor if the capability is still available.
The server closes the device when all process are unregistered on it.

## Structure
*devicesharingd* is UNIX socket server. It manage a list of "device\_t" structure. After a client connexion, the client may send 3 types of message: (register, request, unregister). Each message is a *json* structure containing (command, device, capabilities) and the response is yet a *json* structure.
The message may contain a file descriptor as a Ancillary data on the socket message.

*libdevicesharing.so* is Unix library which creates a connection on the server.
The main library API contians 3 functions: *register*, *request*, *close*.

When a library customer opens a device, it may *register* the file descritor with its capabilities.

When a library customer is not able to open a device, it may *request* the file descriptor for a specific usage described by a capability.

When the library customer completes the use of the device, it must *close* it with the library.

The server stores the device description and a counter on requested client. The server increases the counter on *request* and decreases it on *close*. When the counter is 0 the server closes really the device.

