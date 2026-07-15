#ifndef PACKETS_H
#define PACKETS_H

#include <stdint.h>

typedef enum {
    PKT_MOVE = 1,
    PKT_SYNC = 2,
    PKT_RESIGN = 3,
} PacketType;

typedef struct {
    uint8_t type;
    uint32_t length;
} PacketHeader;

typedef struct {
    uint8_t type;
    uint8_t from_square;
    uint8_t to_square;
    uint8_t promotion;    // 0=None, 1=Queen, 2=Rook, 3=Bishop, 4=Knight
    uint32_t time_left;   // Remaining time in milliseconds
} ChessMovePacket;

typedef struct {
    uint8_t type;
    
    uint64_t boards[12];     // WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK
    
    uint8_t turn;    // 0 = White, 1 = Black
    uint8_t castling_rights; // Bitmask (e.g., 1=WK, 2=WQ, 4=BK, 8=BQ)
    uint8_t en_passant_sq;   // 0-63 index, or 255 if none
    uint8_t halfmove_clock;  // For the 50-move rule
} ChessStatePacket;

typedef struct {
    uint8_t type;
} ChessResignPacket;

#endif