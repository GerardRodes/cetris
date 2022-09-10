#ifndef CETRIS_PROG_H
#define CETRIS_PROG_H

#include <assert.h>
#include "GL/gl3w.h"
#include "util.h"

GLuint
	prog_quad,
	prog_board,
	prog_light
;

GLint
	prog_quad_a_model_tx,
	prog_quad_a_color,
	prog_quad_a_quad,
	prog_quad_a_normal,

	prog_board_a_pos,
	prog_board_a_col,

	prog_light_a_pos
;


void prog_init () {
	prog_board = load_program("shaders/board.vert", "shaders/board.frag");
	prog_board_a_pos = glGetAttribLocation(prog_board, "a_pos");
		assert(prog_board_a_pos != -1);
	prog_board_a_col = glGetAttribLocation(prog_board, "a_col");
		assert(prog_board_a_col != -1);

	prog_quad = load_program("shaders/quad.vert", "shaders/quad.frag");
	prog_quad_a_model_tx = glGetAttribLocation(prog_quad, "a_model_tx");
		assert(prog_quad_a_model_tx != -1);
	prog_quad_a_color = glGetAttribLocation(prog_quad, "a_color");
		assert(prog_quad_a_color != -1);
	prog_quad_a_quad = glGetAttribLocation(prog_quad, "a_quad");
		assert(prog_quad_a_quad != -1);
	prog_quad_a_normal = glGetAttribLocation(prog_quad, "a_normal");
		assert(prog_quad_a_normal != -1);

	prog_light = load_program("shaders/light.vert", "shaders/light.frag");
	prog_light_a_pos = glGetAttribLocation(prog_light, "a_pos");
		assert(prog_light_a_pos != -1);
}

#endif
