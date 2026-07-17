#ifndef PROTOCOL_H_INCLUDED
#define PROTOCOL_H_INCLUDED

#include "packets.h"
int send_packed();

char *serialize_packet(Packet *packet, int *len);
char *deserialize_packet(Packet *packet, int *len);

#endif