// https://stackoverflow.com/questions/1577161/passing-a-structure-through-sockets-in-c

#include <stdint.h>

typedef struct {
    uint8_t type;         // 1 = Move, 2 = State Sync, 3 = Resign/Draw
    uint8_t from_square;  // 0-63
    uint8_t to_square;    // 0-63
    uint8_t promotion;    // 0=None, 1=Queen, 2=Rook, 3=Bishop, 4=Knight
    uint32_t time_left;   // Remaining time in milliseconds
} ChessMovePacket;

typedef struct {
    uint8_t type;     // ID for this packet type (e.g., 2 = Sync)
    
    uint64_t boards[12];     // WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK
    
    // Game state metadata
    uint8_t turn;    // 0 = White, 1 = Black
    uint8_t castling_rights; // Bitmask (e.g., 1=WK, 2=WQ, 4=BK, 8=BQ)
    uint8_t en_passant_sq;   // 0-63 index, or 255 if none
    uint8_t halfmove_clock;  // For the 50-move rule
} ChessStatePacket;