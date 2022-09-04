#ifndef CETRIS_GAME_H
#define CETRIS_GAME_H

#include <math.h>
#include <stdint.h>

#include "board.h"
#include "piece.h"
#include "util.h"

typedef struct {
	board main_board;
	struct {
		struct {
			struct {
				uint8_t left;
				uint8_t right;
				uint8_t up;
				uint8_t down;
			} move;
			struct {
				uint8_t left;
				uint8_t right;
				uint8_t up;
				uint8_t down;
			} held;
			struct {
				double left;
				double right;
				double up;
				double down;
			} last;
		} piece;
		struct {
			struct {
				uint8_t left;
				uint8_t right;
				uint8_t up;
				uint8_t down;
				uint8_t backward;
				uint8_t forward;
			} move;
		} camera;
	} input;
	struct {
		vec3 pos;
		vec3 center;
	} camera;
	struct {
		mat4 proj;
		mat4 view;
		mat4 board;
		mat4 cube;
	} tx_matrix;
	// board* opponents;
	// size_t score;
	// piece_t bag; // 8 == empty
} game;

void game_update_camera_tx_matrix_uniforms(game* g) {
	glm_lookat(g->camera.pos, g->camera.center, (vec3){0,1,0}, g->tx_matrix.view);
	SET_UNIFORM(g->main_board.gl_quad.prog, u_view_tx, glUniformMatrix4fv(_loc, 1, GL_FALSE, g->tx_matrix.view[0]));
	SET_UNIFORM(g->main_board.gl_board.prog, u_view_tx, glUniformMatrix4fv(_loc, 1, GL_FALSE, g->tx_matrix.view[0]));
}

void game_update_tx_matrix_uniforms(game* g, float win_width, float win_height) {
	glm_perspective_default(win_width/win_height, g->tx_matrix.proj);
	SET_UNIFORM(g->main_board.gl_quad.prog, u_proj_tx, glUniformMatrix4fv(_loc, 1, GL_FALSE, g->tx_matrix.proj[0]));
	SET_UNIFORM(g->main_board.gl_board.prog, u_proj_tx, glUniformMatrix4fv(_loc, 1, GL_FALSE, g->tx_matrix.proj[0]));

	game_update_camera_tx_matrix_uniforms(g);

	// todo: store boards txs on array and provide them to shader
	glm_mat4_identity(g->tx_matrix.board);
	board_tx_matrix(&g->main_board, g->tx_matrix.board);
	SET_UNIFORM(g->main_board.gl_board.prog, u_model_tx, glUniformMatrix4fv(_loc, 1, GL_FALSE, g->tx_matrix.board[0]));
}
game game_new(GLuint board_prog, GLuint quad_prog) {
	game g = {
		.main_board = board_new(10, 20, board_prog, quad_prog),
		.camera = { .pos = {0,0,5}, .center = {0,0,0} },
		.input = {
			.piece = {
				.move = { .down = 0, .left = 0, .right = 0, .up = 0},
				.last = { .down = 0, .left = 0, .right = 0, .up = 0},
				.held = { .down = 0, .left = 0, .right = 0, .up = 0},
			}
		}
	};
	return g;
}

void game_tick(game* g, double t, double dt) {
	{
		{
			if (g->input.piece.move.down) {
				if (
					g->input.piece.last.down == 0 ||
					(t - g->input.piece.last.down) >= 0.2
				) {
					board_falling_move_down(&g->main_board);
					if (g->input.piece.last.down == 0) {
						g->input.piece.last.down = t;
					}
				}
			} else {
				g->input.piece.last.down = 0;
			}

			if (g->input.piece.move.left) {
				if (
					g->input.piece.last.left == 0 ||
					(t - g->input.piece.last.left) >= 0.2
				) {
					board_falling_move_left(&g->main_board);
					if (g->input.piece.last.left == 0) {
						g->input.piece.last.left = t;
					}
				}
			} else {
				g->input.piece.last.left = 0;
			}

			if (g->input.piece.move.right) {
				if (
					g->input.piece.last.right == 0 ||
					(t - g->input.piece.last.right) >= 0.2
				) {
					board_falling_move_right(&g->main_board);
					if (g->input.piece.last.right == 0) {
						g->input.piece.last.right = t;
					}
				}
			} else {
				g->input.piece.last.right = 0;
			}
		}
		{
			if (g->input.camera.move.left) {
				g->camera.pos[0] -= 5 * dt;
			}
			if (g->input.camera.move.right) {
				g->camera.pos[0] += 5 * dt;
			}
			if (g->input.camera.move.up) {
				g->camera.pos[1] += 5 * dt;
			}
			if (g->input.camera.move.down) {
				g->camera.pos[1] -= 5 * dt;
			}
			if (g->input.camera.move.backward) {
				g->camera.pos[2] += 5 * dt;
			}
			if (g->input.camera.move.forward) {
				g->camera.pos[2] -= 5 * dt;
			}
			game_update_camera_tx_matrix_uniforms(g);
		}
	}

	board_tick(&g->main_board, t);
}

void game_draw(game* g) {
	// todo: this enables blend on all buffers
	//	but we only care about main framebuffer (maybe?)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.2, 0.4, 0.6, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	board_draw(&g->main_board);
}

#endif
