#ifndef PROTOCOL_H_INCLUDED
#define PROTOCOL_H_INCLUDED

#include "packets.h"
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

int send_packed();

int serialize_packet(const Packet *pkt, uint8_t *buf, size_t buf_cap, size_t *len);
int deserialize_packet(const uint8_t *buf, size_t len, Packet *out);
ssize_t send_all(int fd, const void *buf, size_t len);
ssize_t recv_all(int fd, void *buf, size_t len);

#endif