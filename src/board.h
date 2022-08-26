#ifndef OGL_BOARD_H
#define OGL_BOARD_H

#include <stdlib.h>

#include "GL/glcorearb.h"
#include "cglm/cglm.h"
#include "GL/gl3w.h"

typedef struct {
	const unsigned char cols;
	const unsigned char rows;
	struct {
		GLuint vao;
		GLuint prog;
	} gl_board;
	struct {
		GLuint vao;
		GLuint prog;
		unsigned int count;
	} gl_grid;
	char* grid;
} board;

board board_init (
	unsigned char cols,
	unsigned char rows,
	GLuint board_prog,
	GLuint grid_prog
) {
	return (board){
		.cols = cols,
		.rows = rows,
		.gl_board = { .prog = board_prog, .vao = 0 },
		.gl_grid = { .prog = grid_prog, .vao = 0 },
		.grid = calloc(cols * rows, sizeof(char)),
	};
}

void board_free (board b) {
	free(b.grid);
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
	*(b.grid+(row*b.cols)+col) = v;
}

void board_quads_pos(board b, unsigned char* out, unsigned int* out_len) {
	unsigned int len = 0;
	for (unsigned char y = 0; y < b.rows; y++) {
		for (unsigned char x = 0; x < b.cols; x++) {
			// warning: ptr arithmetic yolo
			if (*(b.grid+(y*b.cols)+x) == 0) {
				continue;
			}
			//todo: quad color
			out[len*2] = x;
			out[(len*2)+1] = y;
			len++;
		}
	}
	*out_len = len;
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
			GLint pos = glGetAttribLocation(b->gl_board.prog, "a_pos");
			if (pos == -1) {
				fprintf(stderr, "attrib a_pos not found\n");
			} else {
				glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(pos);
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);
}

void board_init_grid_vao(board* b) {
	const float quad_vtx[4][2] = {{0,1},{0,0},{1,0},{1,1}}; // on board coords

	glGenVertexArrays(1, &b->gl_grid.vao);
	glBindVertexArray(b->gl_grid.vao);
	{
		GLuint vtx_vbo;
		glGenBuffers(1, &vtx_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vtx_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vtx), quad_vtx, GL_STATIC_DRAW);
		{
			GLint pos = glGetAttribLocation(b->gl_grid.prog, "a_quad");
			if (pos == -1) {
				fprintf(stderr, "attrib a_quad not found\n");
			} else {
				glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(pos);
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	{
		GLuint quad_pos_vbo;
		glGenBuffers(1, &quad_pos_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, quad_pos_vbo);
		{
			unsigned int malloc_size = b->rows * b->cols * 2 * sizeof(unsigned char);
			unsigned char* quad_pos = malloc(malloc_size);
			board_quads_pos(*b, quad_pos, &b->gl_grid.count);
			glBufferData(GL_ARRAY_BUFFER, malloc_size, quad_pos, GL_STATIC_DRAW);
			free(quad_pos); // todo: reuse buffer, add to board struct
		}
		{
			GLint pos = glGetAttribLocation(b->gl_grid.prog, "a_ins_pos");
			if (pos == -1) {
				fprintf(stderr, "attrib a_ins_pos not found\n");
			} else {
				glVertexAttribPointer(pos, 2, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(pos);
				glVertexAttribDivisor(pos, 1);
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
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
		glUseProgram(b.gl_grid.prog);
		glBindVertexArray(b.gl_grid.vao);
		glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, (int)b.gl_grid.count);
		glUseProgram(0);
	}
}

#endif
