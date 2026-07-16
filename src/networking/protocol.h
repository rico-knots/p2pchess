#ifndef PROTOCOL_H_INCLUDED
#define PROTOCOL_H_INCLUDED

#include "packets.h"
int send_packed();

char *serialize_packet(ChessPacket *packet, int *len);

#endif