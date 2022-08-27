#ifndef CETRIS_GAME_H
#define CETRIS_GAME_H

#include "board.h"
#include "piece.h"
#include "util.h"

typedef struct {
	board main_board;
	// board* opponents;
	// size_t score;
	// piece_t bag; // 8 == empty
} game;

game game_new(GLuint board_prog, GLuint quad_prog) {
	board main_board = board_new(10, 20, board_prog, quad_prog);

	{ // todo: store boards txs on array and provide them to shader
		mat4 board_tx = {0};
		board_tx_matrix(main_board, &board_tx, (vec2){0, 0});
		SET_UNIFORM(board_prog, u_board_tx, glUniformMatrix4fv(_loc, 1, GL_FALSE, board_tx[0]));
		SET_UNIFORM(quad_prog, u_board_tx, glUniformMatrix4fv(_loc, 1, GL_FALSE, board_tx[0]));
	}

	board_falling_spawn(&main_board);

	return (game){
		.main_board = main_board,
	};
}

void game_tick(game* g, double t) {
	board_tick(&g->main_board, t);
}

void game_draw(game* g) {
	glClearColor(0.2, 0.4, 0.6, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	board_send_quads_pos(&g->main_board);
	board_draw(g->main_board);
}

#endif
