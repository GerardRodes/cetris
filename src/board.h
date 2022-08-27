#ifndef OGL_BOARD_H
#define OGL_BOARD_H

#include <stdlib.h>
#include <assert.h>

#include "GL/gl3w.h"
#include "GL/glcorearb.h"
#include "cglm/cglm.h"

typedef struct {
	const unsigned char cols;
	const unsigned char rows;
	struct {
		GLuint vao;
		GLuint prog;
		struct {
			GLint pos;
		} a;
	} gl_board;
	struct {
		GLuint prog;
		GLuint vao;
		GLuint quads_vbo;
		struct {
			GLint ins_pos;
			GLint quad;
		} a;
		unsigned char* quads_buf;
		unsigned int quads_cap;
		unsigned int quads_len;
	} gl_quad;
	char* grid_buf;
} board;

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

void board_set_cell(board b, unsigned char col, unsigned char row, unsigned char v) {
	*(b.grid_buf+(row*b.cols)+col) = v;
}

unsigned char board_get_cell(board b, unsigned char col, unsigned char row) {
	return *(b.grid_buf+(row*b.cols)+col);
}

void board_quads_pos(board b, unsigned char* out, unsigned int* out_len) {
	unsigned int len = 0;
	for (unsigned char row = 0; row < b.rows; row++) {
		for (unsigned char col = 0; col < b.cols; col++) {
			if (board_get_cell(b, col, row) == 0) {
				continue;
			}
			//todo: quad color
			out[len*2] = col;
			out[(len*2)+1] = row;
			len++;
		}
	}
	*out_len = len;
}

void board_send_quads_pos(board* b) {
	assert(b->gl_quad.quads_vbo != 0);
	glBindBuffer(GL_ARRAY_BUFFER, b->gl_quad.quads_vbo);

	board_quads_pos(*b, b->gl_quad.quads_buf, &b->gl_quad.quads_len);
	glBufferData(GL_ARRAY_BUFFER, b->gl_quad.quads_cap, b->gl_quad.quads_buf, GL_DYNAMIC_DRAW);

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
			glVertexAttribPointer(b->gl_quad.a.ins_pos, 2, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(b->gl_quad.a.ins_pos);
			glVertexAttribDivisor(b->gl_quad.a.ins_pos, 1);
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
			board_set_cell(b, col, row, (col+row) % (mod+1));
		}
	}
}



board board_init (
	unsigned char cols,
	unsigned char rows,
	GLuint board_prog,
	GLuint quad_prog
) {
	// todo: combine allocs
	unsigned int quads_cap = rows * cols * 2;
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
			.quads_buf = calloc(quads_cap, sizeof(char)),
			.a = {
				.ins_pos = -1,
				.quad = -1,
			}
		},
		// internal representation in bytes of the tetris grid
		.grid_buf = calloc(cols * rows, sizeof(char)),
	};

	b.gl_board.a.pos = glGetAttribLocation(board_prog, "a_pos");
		assert(b.gl_board.a.pos != -1);
	b.gl_quad.a.ins_pos = glGetAttribLocation(quad_prog, "a_ins_pos");
		assert(b.gl_quad.a.ins_pos != -1);
	b.gl_quad.a.quad = glGetAttribLocation(quad_prog, "a_quad");
		assert(b.gl_quad.a.quad != -1);

	board_init_vao(&b);
	board_init_grid_vao(&b);

	return b;
}

#endif
