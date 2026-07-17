#ifndef PACKETS_H
#define PACKETS_H

#include <stdint.h>

typedef enum {
    PKT_PING,

    // Client to server in range 50-99
    C2S_JOIN = 50,
    C2S_MOVE,
    C2S_RESIGN,

    // Server to client in range 100+
    S2C_GAME_START = 100,
    S2C_GAME_OVER,
    S2C_MOVE_BROADCAST,
    S2C_ILLEGAL_MOVE,
    S2C_SYNC,
} PacketType;

typedef enum {
    ILLEGAL_UNKNOWN,
    ILLEGAL_NOT_YOUR_TURN,
    ILLEGAL_NO_PIECE,
    ILLEGAL_WRONG_COLOR_PIECE,
    ILLEGAL_MOVE_PATTERN,       // piece can't move that way
    ILLEGAL_BLOCKED,           // path obstructed
    ILLEGAL_LEAVES_KING_IN_CHECK,
    ILLEGAL_NOT_CONNECTED,     // game hasn't started / wrong game state
} IllegalMoveReason;

typedef struct {
    uint8_t type;
} PacketHeader;

typedef struct {
    PacketHeader header;
} PingPacket;

// C2S Packets

typedef struct {
    PacketHeader header;
    char name[32];
} C2S_JoinPacket;

typedef struct {
    PacketHeader header;
    uint8_t from_square;
    uint8_t to_square;
    uint8_t promotion;       // 0=None, 1=Queen, 2=Rook, 3=Bishop, 4=Knight
}  C2S_MovePacket;

typedef struct {
    PacketHeader header;
} C2S_ResignPacket;          // No need to provide the side to the server, server should know what color the sender is.

// S2C packets

typedef struct {
    PacketHeader header;
    uint8_t your_side;
    uint32_t time_control_ms;
    char opponent_name[32];
} S2C_GameStartPacket;

typedef struct {
    PacketHeader header;
    uint8_t winner;         // 0=White, 1=Black, 2=Draw
    uint8_t cause;          // 0=Resign, 1=Time over, 2=50-move rule
} S2C_GameOverPacket;

typedef struct {
    PacketHeader header;
    uint8_t side;           // 0=White, 1=Black
    uint8_t from_square;
    uint8_t to_square;
    uint8_t promotion;      // 0=None, 1=Queen, 2=Rook, 3=Bishop, 4=Knight
    uint32_t time_left;     // Remaining time for side who just moved in milliseconds
} S2C_MoveBroadcastPacket;

typedef struct {
    PacketHeader header;
    uint8_t from_square;
    uint8_t to_square;
    uint8_t reason;
} S2C_IllegalMovePacket;

typedef struct {
    PacketHeader header;
    uint64_t boards[12];     // WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK
    uint8_t turn;            // 0 = White, 1 = Black
    uint8_t castling_rights; // Bitmask (1=WK, 2=WQ, 4=BK, 8=BQ)
    uint8_t en_passant_sq;   // 0-63 index, or 255 if none
    uint8_t halfmove_clock;  // For the 50-move rule
} S2C_SyncPacket;

typedef struct {
    union {
        PacketHeader header;
        PingPacket ping;
        C2S_JoinPacket join;
        C2S_MovePacket move;
        C2S_ResignPacket resign;

        S2C_GameOverPacket gameover;
        S2C_MoveBroadcastPacket movebroadcast;
        S2C_IllegalMovePacket illegal_move;
        S2C_SyncPacket sync;
    };
}  Packet;

#endif