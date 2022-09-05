#ifndef CETRIS_TEXT_H
#define CETRIS_TEXT_H


#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#include "GL/gl3w.h"

#include "util.h"
#include "cglm/cglm.h"


#define TEXT_MAX_SIZE 1024


GLuint text_prog;
GLint text_prog_a_quad;
GLint text_prog_a_col;
GLint text_prog_a_line;
GLint text_prog_a_ascii_code;

typedef struct {
	uint16_t col;
	uint16_t line;
	uint8_t ascii_code;
} text_vert_attrs;

typedef struct {
	char buf[TEXT_MAX_SIZE];
	uint32_t buf_len;
	va_list argptr;

	GLuint vao;
	GLuint quad_vbo;
	GLuint attrs_vbo;
	text_vert_attrs attrs[TEXT_MAX_SIZE];
} text;

vec2 quad_vtx[] = {{-1,-1},{-1,1},{1,1},{1,-1}};

text text_new(uint32_t x, uint32_t y, uint32_t font_size, uint32_t fg_color, uint32_t bg_color) {
	SET_UNIFORM(text_prog, u_x, glUniform1ui(_loc, x));
	SET_UNIFORM(text_prog, u_y, glUniform1ui(_loc, y));
	SET_UNIFORM(text_prog, u_font_size, glUniform1ui(_loc, font_size));
	SET_UNIFORM(text_prog, u_bg_color, glUniform1ui(_loc, bg_color));
	SET_UNIFORM(text_prog, u_bg_color, glUniform1ui(_loc, fg_color));

	text t = {
		.attrs = {{ .ascii_code = 0 }},
	};

	glGenVertexArrays(1, &t.vao);
	glBindVertexArray(t.vao);
		glGenBuffers(1, &t.quad_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, t.quad_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vtx), quad_vtx, GL_STATIC_DRAW);
			glVertexAttribPointer(text_prog_a_quad, 4*2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(text_prog_a_quad);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &t.attrs_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, t.attrs_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(t.attrs), t.attrs, GL_DYNAMIC_DRAW);
			glVertexAttribIPointer(text_prog_a_col, 1, GL_UNSIGNED_SHORT, sizeof(text_vert_attrs), (void*)offsetof(text_vert_attrs, col));
				glEnableVertexAttribArray(text_prog_a_col);
				glVertexAttribDivisor(text_prog_a_col, 1);
			glVertexAttribIPointer(text_prog_a_line, 1, GL_UNSIGNED_SHORT, sizeof(text_vert_attrs), (void*)offsetof(text_vert_attrs, line));
				glEnableVertexAttribArray(text_prog_a_line);
				glVertexAttribDivisor(text_prog_a_line, 1);
			glVertexAttribIPointer(text_prog_a_ascii_code, 1, GL_UNSIGNED_BYTE, sizeof(text_vert_attrs), (void*)offsetof(text_vert_attrs, ascii_code));
				glEnableVertexAttribArray(text_prog_a_ascii_code);
				glVertexAttribDivisor(text_prog_a_ascii_code, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return t;
}

void text_init() {
	text_prog = load_program("shaders/text.vert", "shaders/text.frag");

	text_prog_a_col = glGetAttribLocation(text_prog, "a_col");
		assert(text_prog_a_col != -1);
	text_prog_a_line = glGetAttribLocation(text_prog, "a_line");
		assert(text_prog_a_line != -1);
	text_prog_a_ascii_code = glGetAttribLocation(text_prog, "a_ascii_code");
		// assert(text_prog_a_ascii_code != -1);

	// todo: send texture https://www.khronos.org/opengl/wiki/Texture
	// todo: check opengl errors GL_INVALID_VALUE
}

void text_set(text* t, const char* format, ...) {
	va_list argptr;
	va_start(argptr, format);
	snprintf(t->buf, TEXT_MAX_SIZE, format, argptr);
	va_end(argptr);
}

void text_gen_attrs(text* t) {
	uint32_t line = 0;
	uint32_t col = 0;
	uint32_t len = 0;
	t->buf_len = TEXT_MAX_SIZE;
	for (uint32_t i = 0; i < TEXT_MAX_SIZE; i++) {
		switch (t->buf[i]) {
			case '\n':
				line++;
				col = 0;
				len++;
				break;
			case '\t':
				for (uint8_t j = 0; j < 2; j++) {
					t->attrs[i].col = col++;
					t->attrs[i].line = line;
					t->attrs[i].ascii_code = ' ';
					len++;
				}
				break;
			case '\0':
				t->buf_len = len;
				return;
			default:
				t->attrs[i].col = col++;
				t->attrs[i].line = line;
				t->attrs[i].ascii_code = t->buf[i];
				len++;
				break;
		}
	}
}

void text_draw(text* t) {
	text_gen_attrs(t);

	glBindBuffer(GL_ARRAY_BUFFER, t->quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(t->attrs), t->attrs, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(text_prog);
	glBindVertexArray(t->vao);
	glDrawArraysInstanced(GL_LINE_LOOP, 0, 4, t->buf_len);
	glUseProgram(0);
}

#endif
