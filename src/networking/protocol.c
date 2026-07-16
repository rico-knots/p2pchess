#include "protocol.h"
#include "packets.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Serialization format: packet fields seperated by semicolon
// Example move packet: "1;18;26;0;30000" -> type;from;to;promotion;time_left

// Returns 0 on error
char *serialize_packet(ChessPacket *packet, int *len) {
    char buff[256];

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
                pos += snprintf(buff + pos, sizeof(buff) - pos, "%lu;", packet->state.boards[i]);
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

    if (*len < 0 || *len >= (int)sizeof(buff)) return NULL;

    // Alloc the needed space for the result
    char *result = malloc(*len + 1); // + 1 for the null terminator

    if (!result) return NULL;

    // Copy over the value of result to the allocated memory
    memcpy(result, buff, *len + 1);

    // Return said allocated piece of memory
    return result;
}