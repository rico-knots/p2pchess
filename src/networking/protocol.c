#include "protocol.h"
#include "packets.h"
#include <inttypes.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

static uint64_t my_htonll(uint64_t val) {
	if (htonl(1) == 1)
		return val; // already big-endian
	return ((uint64_t)htonl(val & 0xFFFFFFFF) << 32) | htonl(val >> 32);
}

static uint64_t my_ntohll(uint64_t val) {
	if (htonl(1) == 1)
		return val; // already big-endian
	return ((uint64_t)ntohl(val & 0xFFFFFFFF) << 32) | ntohl(val >> 32);
}

// Returns -1 on error, 0 on success
int serialize_packet(const Packet *pkt, uint8_t *buf, size_t buf_cap,
					 size_t *len) {
	if (buf_cap < 1)
		return -1;

	uint32_t net_time;

	PacketHeader header = pkt->header;
	switch (header.type) {
	case PKT_PING:
		buf[0] = header.type;
		*len = 1;
		return 0;
	case C2S_JOIN:
		if (buf_cap < 33)
			return -1;
		buf[0] = header.type;
		memcpy(buf + 1, pkt->join.name, 32);
		*len = 33;
		return 0;
	case C2S_MOVE:
		if (buf_cap < 4)
			return -1;
		buf[0] = header.type;
		buf[1] = pkt->move.from_square;
		buf[2] = pkt->move.to_square;
		buf[3] = pkt->move.promotion;
		*len = 4;
		return 0;
	case C2S_RESIGN:
		buf[0] = header.type;
		*len = 1;
		return 0;
	case S2C_GAME_START:
		if (buf_cap < 38)
			return -1;
		buf[0] = header.type;
		buf[1] = pkt->game_started.your_side;
		net_time = htonl(pkt->game_started.time_control_ms);
		memcpy(buf + 2, &net_time, sizeof(net_time));
		memcpy(buf + 6, pkt->game_started.opponent_name, 32);
		*len = 38;
		return 0;
	case S2C_GAME_OVER:
		if (buf_cap < 3)
			return -1;
		buf[0] = header.type;
		buf[1] = pkt->gameover.winner;
		buf[2] = pkt->gameover.cause;
		*len = 3;
		return 0;
	case S2C_MOVE_BROADCAST:
		if (buf_cap < 9)
			return -1;
		buf[0] = header.type;
		buf[1] = pkt->movebroadcast.side;
		buf[2] = pkt->movebroadcast.from_square;
		buf[3] = pkt->movebroadcast.to_square;
		buf[4] = pkt->movebroadcast.promotion;
		net_time = htonl(pkt->movebroadcast.time_left);
		memcpy(buf + 5, &net_time, 4);
		*len = 9;
		return 0;
	case S2C_ILLEGAL_MOVE:
		if (buf_cap < 4)
			return -1;
		buf[0] = header.type;
		buf[1] = pkt->illegal_move.from_square;
		buf[2] = pkt->illegal_move.to_square;
		buf[3] = pkt->illegal_move.reason;
		*len = 4;
		return 0;
	case S2C_SYNC: {
		if (buf_cap < 101)
			return -1;
		buf[0] = header.type;
		buf[1] = pkt->sync.turn;
		buf[2] = pkt->sync.castling_rights;
		buf[3] = pkt->sync.en_passant_sq;
		buf[4] = pkt->sync.halfmove_clock;

		uint64_t net_boards[12];
		for (int i = 0; i < 12; i++) {
			net_boards[i] = my_htonll(pkt->sync.boards[i]);
		}
		memcpy(buf + 5, net_boards, 96);
		*len = 101;
		return 0;
	}
	default:
		return -1;
	}
}

// Returns bytes received, -1 for error, 0 for nothing read (unknown packet
// type)
int deserialize_packet(const uint8_t *buf, size_t len, Packet *out) {
	if (len < 1)
		return -1;

	uint32_t net_time;

	out->header.type = buf[0];
	switch (out->header.type) {
	case PKT_PING:
	case C2S_RESIGN:
		return 1;
	case C2S_JOIN:
		if (len < 33)
			return -1;
		memcpy(out->join.name, buf + 1, 32);
		return 33;
	case C2S_MOVE:
		if (len < 4)
			return -1;
		out->move.from_square = buf[1];
		out->move.to_square = buf[2];
		out->move.promotion = buf[3];
		return 4;
	case S2C_GAME_START:
		if (len < 38)
			return -1;
		out->game_started.your_side = buf[1];
		memcpy(&net_time, buf + 2, 4);
		out->game_started.time_control_ms = ntohl(net_time);
		memcpy(out->game_started.opponent_name, buf + 6, 32);
		return 38;
	case S2C_MOVE_BROADCAST:
		if (len < 9)
			return -1;
		out->movebroadcast.side = buf[1];
		out->movebroadcast.from_square = buf[2];
		out->movebroadcast.to_square = buf[3];
		out->movebroadcast.promotion = buf[4];
		memcpy(&net_time, buf + 5, 4);
		out->movebroadcast.time_left = ntohl(net_time);
		return 9;
	case S2C_ILLEGAL_MOVE:
		if (len < 4)
			return -1;
		out->illegal_move.from_square = buf[1];
		out->illegal_move.to_square = buf[2];
		out->illegal_move.reason = buf[3];
		return 4;
	case S2C_SYNC: {
		if (len < 101)
			return -1;
		out->sync.turn = buf[1];
		out->sync.castling_rights = buf[2];
		out->sync.en_passant_sq = buf[3];
		out->sync.halfmove_clock = buf[4];
		uint64_t net_boards[12];
		memcpy(net_boards, buf + 5, 96);
		for (int i = 0; i < 12; i++) {
			out->sync.boards[i] = my_ntohll(net_boards[i]);
		}
		return 101;
	}

	default:
		return 0;
	}
}

// const = the function will not change the data buf points to

// returns the size sent
ssize_t send_all(int fd, const void *buf, size_t len) {
	const uint8_t *p = buf;
	size_t n = 0;
	while (n < len) {
		ssize_t r = send(fd, p + n, len - n, 0);
		if (r <= 0)
			return -1;
		n += (size_t)r;
	}
	return (ssize_t)n;
}

// returns size received
ssize_t recv_all(int fd, void *buf, size_t len) {
	uint8_t *p = buf;
	size_t n = 0;
	while (n < len) {
		ssize_t r = recv(fd, p + n, len - n, 0);
		if (r <= 0)
			return -1;
		n += (size_t)r;
	}
	return (ssize_t)n;
}
