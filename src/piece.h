#ifndef CETRIS_PIECE_H
#define CETRIS_PIECE_H

#include "cglm/cglm.h"
#include <stdlib.h>

#define PIECE_DEC(p) pieces_decoded[p.t][p.rotation]
#define PIECE_LIM(p) pieces_limits[p.t][p.rotation]

typedef enum {
	PT_T = 0,
	PT_S,
	PT_Z,
	PT_I,
	PT_J,
	PT_L,
	PT_O,
	PT_NONE,
} piece_t;

typedef struct {
	piece_t t;
	unsigned char rotation;
} piece;

typedef struct {
	unsigned char row;
	unsigned char col;
} rwcl;


typedef struct {
	rwcl left;
	rwcl right;
	rwcl down;
	rwcl up;
} piece_limits;

piece_limits pieces_limits[7][4];
unsigned char pieces_decoded[7][4][4][4] = {0};
const unsigned int pieces[7][4] = {
  { 0x4640, 0x0E40, 0x4C40, 0x4E00 }, // 'T'
  { 0x8C40, 0x6C00, 0x8C40, 0x6C00 }, // 'S'
  { 0x4C80, 0xC600, 0x4C80, 0xC600 }, // 'Z'
  { 0x4444, 0x0F00, 0x4444, 0x0F00 }, // 'I'
  { 0x44C0, 0x8E00, 0xC880, 0xE200 }, // 'J'
  { 0x88C0, 0xE800, 0xC440, 0x2E00 }, // 'L'
  { 0xCC00, 0xCC00, 0xCC00, 0xCC00 }  // 'O'
};

const unsigned int piece_color[7] = {
	0xFF'FF'FF'FF, // 'T'
	0xFF'FF'FF'FF, // 'S'
	0xFF'FF'FF'FF, // 'Z'
	0xFF'FF'FF'FF, // 'I'
	0xFF'FF'FF'FF, // 'J'
	0xFF'FF'FF'FF, // 'L'
	0xFF'FF'FF'FF, // 'O'
};

void piece_decode_rotation(piece_t t, unsigned char rotation, unsigned char out[4][4]) {
	unsigned int shape = pieces[t][rotation];

	for (unsigned char row = 0; row < 4; row++) {
		for (unsigned char col = 0; col < 4; col++) {
			if (shape & (0x8000 >> (row*4 + col))) {
				out[row][col] = piece_color[t];
			} else {
				out[row][col] = 0;
			}
		}
	}
}

void piece_init() {
	for (unsigned char p = 0; p < 7; p++) {
		for (unsigned char r = 0; r < 7; r++) {
			piece_decode_rotation(p, r, pieces_decoded[p][r]);

			pieces_limits[p][r].left.col = 255;
			pieces_limits[p][r].right.col = 0;
			pieces_limits[p][r].up.row = 0;
			pieces_limits[p][r].down.row = 255;

			for (unsigned char row = 0; row < 4; row++) {
				for (unsigned char col = 0; col < 4; col++) {
					if (pieces_decoded[p][r][row][col] == 0) {
						continue;
					}

					if (col > pieces_limits[p][r].right.col) {
						pieces_limits[p][r].right.row = row;
						pieces_limits[p][r].right.col = col;
					}

					if (col < pieces_limits[p][r].left.col) {
						pieces_limits[p][r].left.row = row;
						pieces_limits[p][r].left.col = col;
					}

					if (row > pieces_limits[p][r].up.row) {
						pieces_limits[p][r].up.row = row;
						pieces_limits[p][r].up.col = col;
					}

					if (row < pieces_limits[p][r].down.row) {
						pieces_limits[p][r].down.row = row;
						pieces_limits[p][r].down.col = col;
					}
				}
			}
		}
	}
}

piece piece_random(){
	return (piece){
		.t = (piece_t)(rand() % PT_NONE),
		.rotation = 0,
	};
}

void piece_rotate(piece* p) {
	p->rotation = (p->rotation+1) % 4;
}

#endif
