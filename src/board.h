#ifndef OGL_BOARD_H
#define OGL_BOARD_H

#include <stdlib.h>
#include <assert.h>

#include "GL/gl3w.h"
#include "GL/glcorearb.h"
#include "cglm/cglm.h"

#include "piece.h"

typedef struct {
	unsigned char cols;
	unsigned char rows;
	struct {
		GLuint vao;
		GLuint prog;
		struct {
			GLint pos;
		} a;
	} gl_board;
	struct {
		unsigned char* quads_buf;
		unsigned int quads_cap;
		unsigned int quads_len;
		GLuint prog;
		GLuint vao;
		GLuint quads_vbo;
		struct {
			GLint ins_pos;
			GLint ins_color;
			GLint quad;
		} a;
	} gl_quad;
	char* grid_buf; // internal representation of grid

	struct {
		piece p;
		struct {
			int row;
			int col;
		} pos;
		double last_autofall;
	} falling;
} board;

void board_falling_spawn(board* b) {
	b->falling.p = piece_random();

	b->falling.pos.row = 0;
	b->falling.pos.col = b->cols / 2;
}

void board_set_cell(board b, unsigned char row, unsigned char col, unsigned char v) {
	*(b.grid_buf+(row*b.cols)+col) = v;
}

unsigned char board_get_cell(board b, unsigned char row, unsigned char col) {
	return *(b.grid_buf+(row*b.cols)+col);
}

int board_falling_overlaps_conflict(board b) {
	for (unsigned char row = 0; row < 4; row++) {
		for (unsigned char col = 0; col < 4; col++) {
			if (PIECE_DEC(b.falling.p)[row][col] == 0) {
				continue;
			}

			const int board_row = b.falling.pos.row + row;
			const int board_col = b.falling.pos.col + col;

			if (board_col < 0 || board_col >= b.cols) {
				return 1;
			}

			if (board_row >= b.rows) {
				return 1;
			}

			if (board_get_cell(b, board_row, board_col) != 0) {
				return 1;
			}
		}
	}

	return 0;
}

int board_falling_rotate(board* b) {
	const unsigned char prev_rotation = b->falling.p.rotation;
	piece_rotate(&b->falling.p);

	if (board_falling_overlaps_conflict(*b)) {
		b->falling.p.rotation = prev_rotation;
		return 0;
	}

	return 1;
}

void board_free (board b) {
	free(b.grid_buf);
	free(b.gl_quad.quads_buf);
}

void board_tx_matrix (board b, mat4* tx_matrix, vec2 center) {
	float board_aspect_ratio = (float)b.cols / b.rows;
	float board_scale_factor = board_aspect_ratio/((float)b.cols/2);
	glm_mat4_identity(tx_matrix[0]);
	glm_scale(tx_matrix[0], (vec3){board_scale_factor,-board_scale_factor,0});
	glm_scale(tx_matrix[0], (vec3){0.9,0.9,0});
	glm_translate_x(tx_matrix[0], -((float)b.cols/2));
	glm_translate_y(tx_matrix[0], -((float)b.rows/2));
}

void board_quads_pos(board b, unsigned char* out, unsigned int* out_len) {
	unsigned int len = 0;
	for (unsigned char row = 0; row < b.rows; row++) {
		for (unsigned char col = 0; col < b.cols; col++) {
			if (board_get_cell(b, row, col) == 0) {
				continue;
			}
			out[(len*6)+0] = col;  // x
			out[(len*6)+1] = row;  // y
			out[(len*6)+2] = 0xFF; // r
			out[(len*6)+3] = 0xFF; // g
			out[(len*6)+4] = 0xFF; // b
			out[(len*6)+5] = 0xFF; // a
			len++;
		}
	}

	// add falling piece
	{
		for (unsigned char row = 0; row < 4; row++) {
			for (unsigned char col = 0; col < 4; col++) {
				if (PIECE_DEC(b.falling.p)[row][col] != 0) {
					out[(len*6)+0] = b.falling.pos.col + col;  // x
					out[(len*6)+1] = b.falling.pos.row + row;  // y
					out[(len*6)+2] = 0xFF; // r
					out[(len*6)+3] = 0xFF; // g
					out[(len*6)+4] = 0xFF; // b
					out[(len*6)+5] = 0xFF; // a
					len++;
				}
			}
		}
	}

	// add ghost piece
	{

	}


	*out_len = len;
}

void board_send_quads_pos(board* b) {
	assert(b->gl_quad.quads_vbo != 0);
	glBindBuffer(GL_ARRAY_BUFFER, b->gl_quad.quads_vbo);

	board_quads_pos(*b, b->gl_quad.quads_buf, &b->gl_quad.quads_len);
	glBufferData(GL_ARRAY_BUFFER, b->gl_quad.quads_cap, b->gl_quad.quads_buf, GL_DYNAMIC_DRAW);
	// todo: send only updated slice of the buffer
	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBufferSubData.xhtml

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void board_init_vao(board* b) {
	const float board_vtx[4][2] = {
		{0,b->rows},
		{0,0},
		{b->cols,0},
		{b->cols,b->rows}
	};

	glGenVertexArrays(1, &b->gl_board.vao);
	glBindVertexArray(b->gl_board.vao);
	{
		GLuint vtx_vbo;
		glGenBuffers(1, &vtx_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vtx_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(board_vtx), board_vtx, GL_STATIC_DRAW);
		{
			glVertexAttribPointer(b->gl_board.a.pos, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(b->gl_board.a.pos);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);
}

void board_init_grid_vao(board* b) {
	const float quad_vtx[4][2] = {{0,1},{0,0},{1,0},{1,1}}; // on board coords

	glGenVertexArrays(1, &b->gl_quad.vao);
	glBindVertexArray(b->gl_quad.vao);
	// quad vtxs
	{
		GLuint vtx_vbo;
		glGenBuffers(1, &vtx_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vtx_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vtx), quad_vtx, GL_STATIC_DRAW);
		{
			glVertexAttribPointer(b->gl_quad.a.quad, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(b->gl_quad.a.quad);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	// quads positions in grid
	{
		glGenBuffers(1, &b->gl_quad.quads_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, b->gl_quad.quads_vbo);
		const unsigned int stride = (sizeof(char)*2) + sizeof(int);
		{
			glVertexAttribPointer(b->gl_quad.a.ins_pos, 2, GL_UNSIGNED_BYTE, GL_FALSE, stride, (void*)0);
			glEnableVertexAttribArray(b->gl_quad.a.ins_pos);
			glVertexAttribDivisor(b->gl_quad.a.ins_pos, 1);
		}
		{ // todo: not working :( maybe try to send unpacked first
			glVertexAttribPointer(b->gl_quad.a.ins_color, 1, GL_UNSIGNED_INT, GL_FALSE, stride, (void*)(sizeof(char)*2));
			glEnableVertexAttribArray(b->gl_quad.a.ins_color);
			glVertexAttribDivisor(b->gl_quad.a.ins_color, 1);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		board_send_quads_pos(b);
	}
	glBindVertexArray(0);
}

void board_draw (board b) {
	{
		glUseProgram(b.gl_board.prog);
		glBindVertexArray(b.gl_board.vao);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		glUseProgram(0);
	}
	{
		glUseProgram(b.gl_quad.prog);
		glBindVertexArray(b.gl_quad.vao);
		glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, (int)b.gl_quad.quads_len);
		glUseProgram(0);
	}
}

void board_debug (board b, unsigned int mod) {
	for (unsigned int row = 0; row < b.rows; row++) {
		for (unsigned int col = 0; col < b.cols; col++) {
			board_set_cell(b, row, col, (col+row) % (mod+1));
		}
	}
}

int board_falling_move_left(board* b) {
	b->falling.pos.col--;

	if(board_falling_overlaps_conflict(*b)) {
		b->falling.pos.col++;
		return 0;
	}

	return 1;
}

int board_falling_move_right(board* b) {
	b->falling.pos.col++;

	if(board_falling_overlaps_conflict(*b)) {
		b->falling.pos.col--;
		return 0;
	}

	return 1;
}

// returns 1 if can move down, 0 if don't
int board_falling_move_down(board* b) {
	b->falling.pos.row++;

	if(board_falling_overlaps_conflict(*b)) {
		b->falling.pos.col--;
		return 0;
	}

	return 1; // true
}

void board_falling_lock(board* b) {

}

void board_tick (board* b, double t) {
	if ((t - b->falling.last_autofall) > 1) {
		board_falling_move_down(b);
		b->falling.last_autofall = t;
	}
}

board board_new (
	unsigned char cols,
	unsigned char rows,
	GLuint board_prog,
	GLuint quad_prog
) {
	// todo: combine allocs
	const unsigned int quads_max_cells = rows * cols;
	const unsigned int quads_cap =
		(quads_max_cells * 2 * sizeof(unsigned char)) + // coordinates
		(quads_max_cells * sizeof(unsigned int)) 				// color
	;
	board b = {
		.cols = cols,
		.rows = rows,
		.gl_board = {
			.prog = board_prog,
			.vao = 0,
			.a = {
				.pos = -1,
			}
		},
		.gl_quad = {
			.prog = quad_prog,
			.vao = 0,
			.quads_cap = quads_cap,
			.quads_buf = calloc(quads_cap, 1),
			.a = {
				.ins_pos = -1,
				.quad = -1,
			}
		},
		// internal representation in bytes of the tetris grid
		.grid_buf = calloc(cols * rows, sizeof(char)),
		.falling = {
			.p = { .t = PT_NONE },
			.last_autofall = 0,
		}
	};

	b.gl_board.a.pos = glGetAttribLocation(board_prog, "a_pos");
		assert(b.gl_board.a.pos != -1);
	b.gl_quad.a.ins_pos = glGetAttribLocation(quad_prog, "a_ins_pos");
		assert(b.gl_quad.a.ins_pos != -1);
	b.gl_quad.a.ins_color = glGetAttribLocation(quad_prog, "a_ins_color");
		assert(b.gl_quad.a.ins_color != -1);
	b.gl_quad.a.quad = glGetAttribLocation(quad_prog, "a_quad");
		assert(b.gl_quad.a.quad != -1);

	board_init_vao(&b);
	board_init_grid_vao(&b);

	return b;
}

#endif
