#ifndef CETRIS_PIECE_H
#define CETRIS_PIECE_H

#include "cglm/cglm.h"
#include <stdlib.h>

typedef enum {
	T = 0,
	S,
	Z,
	I,
	J,
	L,
	O,
} piece_t;

typedef struct {
	piece_t t;
	unsigned char rotation;
} piece;

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

void piece_decode(piece p, unsigned char* out) {
	unsigned int shape = pieces[p.t][p.rotation];

	for (unsigned char y = 0; y < 4; y++) {
		for (unsigned char x = 0; x < 4; x++) {
			unsigned char* ptr = out + (y*4) + x;
			if (shape & (0x8000 >> (y * 4 + x))) {
				*ptr = piece_color[p.t];
			} else {
				*ptr = 0;
			}
		}
	}
}

piece piece_random(){
	return (piece){
		.t = (piece_t)(rand() % (7 + 1)),
		.rotation = 0,
	};
}

void piece_rotate(piece* p) {
	p->rotation = (p->rotation+1) % 4;
}

#endif
