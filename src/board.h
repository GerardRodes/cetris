#ifndef OGL_BOARD_H
#define OGL_BOARD_H

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "GL/gl3w.h"
#include "GL/glcorearb.h"
#include "cglm/cglm.h"

#include "piece.h"
#include "util.h"
#include "cube.h"
#include "prog.h"

#define QUAD_POINTS 4

typedef struct {
	mat4 model_tx;
	uint32_t rgba;
} quads_vbo_attr;

typedef struct {
	uint8_t cols;
	uint8_t rows;
	struct {
		GLuint vao;
	} gl_board;
	struct {
		quads_vbo_attr* quads_buf;
		uint32_t quads_cap;
		uint32_t quads_len;
		GLuint vao;
		GLuint quads_vbo;
	} gl_quad;

	struct {
		piece p;
		struct {
			int col;
			int row;
		} pos;
		double last_autofall;
	} falling;
	uint32_t* grid_buf; // internal representation of grid
	struct {
		struct {
			double dt;
			uint8_t duration;
			uint8_t running;
			struct {
				uint8_t start;
				uint8_t end;
			} row;
		} clear;
	} animation;
	uint32_t score;
} board;


uint32_t board_level(board* b) {
	return (b->score / 1000)+1;
}

double board_autofall_time(board* b) {
	return 1.0 / board_level(b);
}

void board_set_cell(board* b, uint8_t row, uint8_t col, uint32_t v) {
	b->grid_buf[(row*b->cols)+col] = v;
}

uint32_t board_get_cell(board* b, uint8_t row, uint8_t col) {
	return b->grid_buf[(row*b->cols)+col];
}

int board_falling_overlaps_conflict(board* b) {
	for (uint8_t row = 0; row < 4; row++) {
		for (uint8_t col = 0; col < 4; col++) {
			if (PIECE_DEC(b->falling.p)[row][col] == 0) {
				continue;
			}

			const int board_row = b->falling.pos.row + row;
			const int board_col = b->falling.pos.col + col;

			if (board_row < 0) {
				// don't take in account rows over the limit
				continue;
			}

			if (board_col < 0 || board_col >= b->cols) {
				return 1;
			}

			if (board_row >= b->rows) {
				return 1;
			}

			if (board_get_cell(b, board_row, board_col) != 0) {
				return 1;
			}
		}
	}

	return 0;
}

int board_falling_fit_in(board* b) {
	if (!board_falling_overlaps_conflict(b)) {
		return 1;
	}

	for(int8_t i = 0; i < 4; i++) {
		b->falling.pos.col--;
		if (!board_falling_overlaps_conflict(b)) {
			return 1;
		}
	}
	b->falling.pos.col += 4;

	for(int8_t i = 0; i < 4; i++) {
		b->falling.pos.col++;
		if (!board_falling_overlaps_conflict(b)) {
			return 1;
		}
	}
	b->falling.pos.col -= 4;

	return 0;
}

void board_falling_spawn(board* b) {
	b->falling.p = piece_random();

	b->falling.pos.row = -1;

	b->falling.pos.col = b->cols / 2;
	board_falling_fit_in(b);
}

int board_falling_rotate(board* b) {
	const uint8_t prev_rotation = b->falling.p.rotation;
	piece_rotate(&b->falling.p);

	if (!board_falling_overlaps_conflict(b)) {
		return 1;
	}

	if (board_falling_fit_in(b)) {
		return 1;
	}

	b->falling.p.rotation = prev_rotation;
	return 0;
}

void board_free (board* b) {
	free(b->grid_buf);
	free(b->gl_quad.quads_buf);
}

void board_tx_matrix (board* b, mat4 tx_matrix) { // todo: center to position the board
	float board_aspect_ratio = (float)b->cols / (float)b->rows;
	float board_scale_factor = board_aspect_ratio/(b->cols/4.0);
	glm_scale(tx_matrix, (vec3){board_scale_factor,-board_scale_factor,1});
	glm_translate_x(tx_matrix, -(b->cols*0.5));
	glm_translate_y(tx_matrix, -(b->rows*0.5));
}

void board_cube_tx_matrix (board* b, mat4 tx_matrix) {
	float board_aspect_ratio = (float)b->cols / (float)b->rows;
	float board_scale_factor = board_aspect_ratio/(b->cols/4.0);
	glm_scale(tx_matrix, (vec3){board_scale_factor,-board_scale_factor,board_scale_factor});
	glm_translate_x(tx_matrix, -(b->cols*0.5));
	glm_translate_y(tx_matrix, -(b->rows*0.5));
}

void board_init_vao(board* b) {
	const float board_vtx[][5] = {
		{0,0,							0,1,0},
		{0,b->rows,				1,0,0},
		{b->cols,b->rows,	1,0,1},
		{b->cols,0,				0,0,1},
	};

	glGenVertexArrays(1, &b->gl_board.vao);
	glBindVertexArray(b->gl_board.vao);
	{
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(board_vtx), board_vtx, GL_STATIC_DRAW);
			glVertexAttribPointer(prog_board_a_pos, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, 0);
				glEnableVertexAttribArray(prog_board_a_pos);
			glVertexAttribPointer(prog_board_a_col, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)(sizeof(float)*2));
				glEnableVertexAttribArray(prog_board_a_col);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);
}

void board_init_grid_vao(board* b) {
	glGenVertexArrays(1, &b->gl_quad.vao);
	glBindVertexArray(b->gl_quad.vao);
	{
		// quad vtxs
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cube_data), cube_data, GL_STATIC_DRAW);
			glVertexAttribPointer(prog_quad_a_quad, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)0);
				glEnableVertexAttribArray(prog_quad_a_quad);
			glVertexAttribPointer(prog_quad_a_normal, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)(sizeof(float)*3));
				glEnableVertexAttribArray(prog_quad_a_normal);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	{
		// quads positions in grid
		glGenBuffers(1, &b->gl_quad.quads_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, b->gl_quad.quads_vbo);
			glVertexAttribIPointer(prog_quad_a_color, 1, GL_UNSIGNED_INT, sizeof(quads_vbo_attr), (void*)offsetof(quads_vbo_attr, rgba));
				glEnableVertexAttribArray(prog_quad_a_color);
				glVertexAttribDivisor(prog_quad_a_color, 1);
			// a mat4 attribute has to be initialized as 4 vec4
			for (uint8_t i = 0; i < 4; i++) {
				glVertexAttribPointer(prog_quad_a_model_tx + i, 4, GL_FLOAT, GL_FALSE, sizeof(quads_vbo_attr), (void*)(offsetof(quads_vbo_attr, model_tx) + (sizeof(vec4)*i)));
					glEnableVertexAttribArray(prog_quad_a_model_tx + i);
					glVertexAttribDivisor(prog_quad_a_model_tx + i, 1);
			}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);
}

void board_debug (board* b, uint32_t mod) {
	for (uint32_t row = 0; row < b->rows; row++) {
		for (uint32_t col = 0; col < b->cols; col++) {
			board_set_cell(b, row, col, (col+row) % (mod+1));
		}
	}
}

int board_falling_move_left(board* b) {
	b->falling.pos.col--;

	if(board_falling_overlaps_conflict(b)) {
		b->falling.pos.col++;
		return 0;
	}

	return 1;
}

int board_falling_move_right(board* b) {
	b->falling.pos.col++;

	if(board_falling_overlaps_conflict(b)) {
		b->falling.pos.col--;
		return 0;
	}

	return 1;
}

// returns 1 if can move down, 0 if don't
int board_falling_move_down(board* b) {
	b->falling.pos.row++;

	if(board_falling_overlaps_conflict(b)) {
		b->falling.pos.row--;
		return 0;
	}

	return 1; // true
}

void board_clear_rows(board* b, uint8_t start, uint8_t end) {
	uint32_t lines = end-start+1;
	const uint32_t cells_to_del = lines*b->cols;
	const uint32_t cells_to_mov = start*b->cols;
	memmove(&b->grid_buf[cells_to_del], &b->grid_buf[0], cells_to_mov*sizeof(uint32_t));
	memset(b->grid_buf, 0, cells_to_del*sizeof(uint32_t));

	// scoring
	b->score += (uint32_t)((((float)lines+1)/2) * cells_to_del * QUAD_POINTS * board_level(b));
}

void board_animation_clear_rows_apply(board* b, mat4 tx, uint32_t* rgba, uint8_t col, uint8_t row) {
	float progress = normalize(0, b->animation.clear.duration, b->animation.clear.dt);

	float col_r = normalize(0, b->cols, col);

	if (
		row >= b->animation.clear.row.start &&
		row <= b->animation.clear.row.end
	) {
		// animation of quads to clear
		float row_r = normalize(b->animation.clear.row.start, b->animation.clear.row.end+1, row);

		float n = clamp(0, 255, 255*progress*1.2);
		uint32_t alpha = (uint8_t)(255 - n) << (3*8);
		*rgba = ((*rgba) & ~0xFF'00'00'00) | alpha;

		float p = bezier3(0, -5*col_r, 10*row_r, 40, progress);
		p *= -1;
		glm_translate_z(tx, p);
		glm_rotate(tx, glm_rad(360*progress), (vec3){1, 1, 1});
	} else if (
		row < b->animation.clear.row.start
	) {
		// animation lines above
		if (progress < 0.5) return;
		float p = normalize(0.5, 1, progress);
		float row_r = normalize(0, b->animation.clear.row.start-1, row);
		float rows_to_move = b->animation.clear.row.end - b->animation.clear.row.start + 1;
		p = bezier3(0, 0.8*col_r, 0.9*row_r, rows_to_move, p);
		glm_translate_y(tx, p);
	}
}

void board_animation_clear_rows_run(board* b, double dt) {
	if (b->animation.clear.dt > b->animation.clear.duration) {
		board_clear_rows(b, b->animation.clear.row.start, b->animation.clear.row.end);
		b->animation.clear.running = 0;
	} else {
		b->animation.clear.dt += dt;
	}
}

void board_animation_clear_rows_start(board* b, uint8_t start, uint8_t end) {
	b->animation.clear.row.start = start;
	b->animation.clear.row.end = end;
	b->animation.clear.running = 1;
	b->animation.clear.dt = 0;
}

void board_falling_lock(board* b) {
	if (b->falling.p.t == PT_NONE) {
		return;
	}

	while(!board_falling_overlaps_conflict(b)) {
		b->falling.pos.row++;
	}
	b->falling.pos.row--; // undo last

	for (uint8_t row = 0; row < 4; row++) {
		for (uint8_t col = 0; col < 4; col++) {
			const uint32_t color = PIECE_DEC(b->falling.p)[row][col];
			if (color == 0) {
				continue;
			}

			const int board_row = b->falling.pos.row + row;
			const int board_col = b->falling.pos.col + col;

			if (board_row < 0) {
				// todo: game over
				b->falling.p.t = PT_NONE;
				b->score = 0;
				memset(b->grid_buf, 0, b->rows*b->cols*sizeof(int));
				return;
			}

			board_set_cell(b, board_row, board_col, color);
		}
	}

	// clear completed rows
	int start = -1;
	for (uint8_t row = 0; row < b->rows; row++) {
		for (uint8_t col = 0; col < b->cols; col++) {
			if (board_get_cell(b, row, col) == 0) {
				if (start != -1) {
					// fixme: we are overriding subsequents clears because we
					// only have 1 set of rows clearing at the same time
					// we should allot to define different sets of rows to allow
					// to clear non-sequential rows.
					// it works if we clear directly without the animations
					board_animation_clear_rows_start(b, (int8_t)start, row-1);
					// board_clear_rows(b, (int8_t)start, row-1);
					start = -1;
				}
				goto outer;
			}
		}

		if (start == -1) {
			start = row;
		}

		outer:;
	}
	if (start != -1) {
		board_animation_clear_rows_start(b, (int8_t)start, b->rows-1);
		// board_clear_rows(b, (int8_t)start, b->rows-1);
	}

	b->falling.p.t = PT_NONE;
}

void board_quads_vbo_attrs(board* b, quads_vbo_attr* out, uint32_t* out_len) {
	uint32_t len = 0;
	for (uint8_t row = 0; row < b->rows; row++) {
		for (uint8_t col = 0; col < b->cols; col++) {
			const uint32_t color = board_get_cell(b, row, col);
			if (color == 0) {
				continue;
			}

			glm_mat4_identity(out[len].model_tx);
			board_cube_tx_matrix(b, out[len].model_tx);
			glm_translate(out[len].model_tx, (vec3){col, row, 0});
			out[len].rgba = color;

			// todo: this should bw out of this loop
			// I would need to have a dedicated vbo for model matrix
			if (b->animation.clear.running) {
				board_animation_clear_rows_apply(b, out[len].model_tx, &out[len].rgba, col, row);
			}

			len++;
		}
	}

	if (b->falling.p.t == PT_NONE) {
		return;
	}

	// add falling piece
	for (uint8_t row = 0; row < 4; row++) {
		for (uint8_t col = 0; col < 4; col++) {
			const uint32_t color = PIECE_DEC(b->falling.p)[row][col];
			if (color == 0) {
				continue;
			}
			const uint32_t board_row = b->falling.pos.row + row;
			const uint32_t board_col = b->falling.pos.col + col;

			glm_mat4_identity(out[len].model_tx);
			board_cube_tx_matrix(b, out[len].model_tx);
			glm_translate(out[len].model_tx, (vec3){board_col, board_row, 0});

			out[len].rgba = color;
			len++;
		}
	}

	// add ghost piece
	{
		int initial_row = b->falling.pos.row;
		while (!board_falling_overlaps_conflict(b)) {
			b->falling.pos.row++;
		}
		int16_t ghost_row = b->falling.pos.row-1;
		int16_t ghost_col = b->falling.pos.col;
		b->falling.pos.row = initial_row;

		for (uint8_t row = 0; row < 4; row++) {
			for (uint8_t col = 0; col < 4; col++) {
				const uint32_t color = PIECE_DEC(b->falling.p)[row][col];
				if (color == 0) {
					continue;
				}
				const uint32_t board_row = ghost_row + row;
				const uint32_t board_col = ghost_col + col;

				glm_mat4_identity(out[len].model_tx);
				board_cube_tx_matrix(b, out[len].model_tx);
				glm_translate(out[len].model_tx, (vec3){board_col, board_row, 0});
				// replace alpha channel
				out[len].rgba = (color & ~0xFF'00'00'00) | 0x44'00'00'00;
				len++;
			}
		}
	}

	*out_len = len;
}

void board_tick (board* b, double t, double dt) {
	// SET_UNIFORM(prog_board, u_time, glUniform1f(_loc, t));
	// SET_UNIFORM(prog_quad, u_time, glUniform1f(_loc, t));

	if (b->animation.clear.running) {
		board_animation_clear_rows_run(b, dt);
		return;
	}

	if (b->falling.p.t == PT_NONE) {
		board_falling_spawn(b);
	} else if ((t - b->falling.last_autofall) > board_autofall_time(b)) {
		b->falling.last_autofall = t;
		if(!board_falling_move_down(b)) {
			// todo: wait 1 second before locking
			// 	provide up to 3 seconds before locking if player moves
			board_falling_lock(b);
		}
	}
}

void board_send_quads_pos(board* b) {
	assert(b->gl_quad.quads_vbo != 0);
	glBindBuffer(GL_ARRAY_BUFFER, b->gl_quad.quads_vbo);

	board_quads_vbo_attrs(b, b->gl_quad.quads_buf, &b->gl_quad.quads_len);
	glBufferData(GL_ARRAY_BUFFER, b->gl_quad.quads_cap, b->gl_quad.quads_buf, GL_DYNAMIC_DRAW);
	// todo: send only updated slice of the buffer
	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBufferSubData.xhtml

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void board_draw (board* b) {
	glUseProgram(prog_board);
		glBindVertexArray(b->gl_board.vao);
			glLineWidth(5);
			glDrawArrays(GL_LINE_LOOP, 0, 4);
			glLineWidth(1);
		glBindVertexArray(0);
	glUseProgram(0);

	board_send_quads_pos(b);
	glUseProgram(prog_quad);
		glBindVertexArray(b->gl_quad.vao);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 3*2*6, (int)b->gl_quad.quads_len);
		glBindVertexArray(0);
	glUseProgram(0);
}

board board_new (uint8_t cols, uint8_t rows) {
	// todo: combine allocs
	const uint32_t quads_cap = rows * cols * sizeof(quads_vbo_attr);
	board b = {
		.cols = cols,
		.rows = rows,
		.gl_board = {
			.vao = 0,
		},
		.gl_quad = {
			.vao = 0,
			.quads_cap = quads_cap,
			.quads_buf = calloc(quads_cap, 1),
		},
		// internal representation in bytes of the tetris grid
		.grid_buf = calloc(cols * rows, sizeof(uint32_t)),
		.falling = {
			.p = { .t = PT_NONE },
			.last_autofall = 0,
		},
		.animation = {
			.clear = {
				.running = 0,
				.duration = 1,
			},
		},
		.score = 0,
	};

	board_init_vao(&b);
	board_init_grid_vao(&b);
	return b;
}

#endif
