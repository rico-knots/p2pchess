#include "protocol.h"
#include "packets.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

// Serialization format: packet fields seperated by semicolon
// Example move packet: "1;18;26;0;30000" -> type;from;to;promotion;time_left

// Returns 0 on error
char *serialize_packet(Packet *packet, int *len) {
    char buff[1024];
    int pos;

    switch(packet->type) {
        case PKT_MOVE:
            *len = snprintf(buff, sizeof(buff), "%d;%d;%d;%d;%u",
                packet->type,
                packet->move.from_square,
                packet->move.to_square,
                packet->move.promotion,
                packet->move.time_left);
            break;
        case PKT_SYNC: {
            int pos = 0;
            
            pos += snprintf(buff + pos, sizeof(buff) - pos, "%d;", packet->type);
            
            for (int i = 0; i < 12; i++) {
                pos += snprintf(buff + pos, sizeof(buff) - pos, "%" PRIu64 ";", packet->state.boards[i]);
            }
            
            pos += snprintf(buff + pos, sizeof(buff) - pos, "%d;%d;%d;%d",
                packet->state.turn,
                packet->state.castling_rights,
                packet->state.en_passant_sq,
                packet->state.halfmove_clock);
            *len = pos;
            break;
        }
        case PKT_RESIGN:
            *len = snprintf(buff, sizeof(buff), "%d;%d",
                packet->type,
                packet->resign.side);
            break;
        default: return NULL;
    }

    if (pos < 0 || pos >= (int)sizeof(buff) - 1) return NULL;

    // Mark the end of a packet with "\n" -> needed for stream may not send the full string at once
    buff[pos] = '\n';
    pos++;
    buff[pos] = '\0';

    // Alloc the needed space for the result
    char *result = malloc(*len + 1); // + 1 for the null terminator
    if (!result) return NULL;

    // Copy over the value of result to the allocated memory
    memcpy(result, buff, *len + 1);

    // Return said allocated piece of memory
    *len = pos;
    return result;
}