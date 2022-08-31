#ifndef CETRIS_PIECE_H
#define CETRIS_PIECE_H

#include "cglm/cglm.h"
#include <stdlib.h>

#define PIECE_DEC(p) pieces_decoded[p.t][p.rotation]

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
	uint8_t rotation;
} piece;

uint32_t pieces_decoded[7][4][4][4] = {0};
const uint32_t pieces[7][4] = {
  { 0x4640, 0x0E40, 0x4C40, 0x4E00 }, // 'T'
  { 0x8C40, 0x6C00, 0x8C40, 0x6C00 }, // 'S'
  { 0x4C80, 0xC600, 0x4C80, 0xC600 }, // 'Z'
  { 0x4444, 0x0F00, 0x4444, 0x0F00 }, // 'I'
  { 0x44C0, 0x8E00, 0xC880, 0xE200 }, // 'J'
  { 0x88C0, 0xE800, 0xC440, 0x2E00 }, // 'L'
  { 0xCC00, 0xCC00, 0xCC00, 0xCC00 }  // 'O'
};

const uint32_t piece_color[7] = {
	//AA'BB'GG'RR
	0xFF'FF'00'99, // 'T'
	0xFF'00'FF'00, // 'S'
	0xFF'00'00'FF, // 'Z'
	0xFF'FF'FF'00, // 'I'
	0xFF'FF'00'00, // 'J'
	0xFF'00'AA'FF, // 'L'
	0xFF'00'FF'FF, // 'O'
};

void piece_init() {
	for (uint8_t p = 0; p < PT_NONE; p++) {
		for (uint8_t r = 0; r < 4; r++) {
			for (uint8_t row = 0; row < 4; row++) {
				for (uint8_t col = 0; col < 4; col++) {
					if (pieces[p][r] & (0x8000 >> (row*4 + col))) {
						pieces_decoded[p][r][row][col] = piece_color[p];
					} else {
						pieces_decoded[p][r][row][col] = 0;
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
