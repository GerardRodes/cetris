#ifndef CETRIS_TEXT_H
#define CETRIS_TEXT_H


#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#include "GL/gl3w.h"
#include "cglm/cglm.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

#include "util.h"


#define TEXT_MAX_SIZE 1024

GLuint text_font_texture;
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

	float x;
	float y;
	float font_size;
	uint32_t fg_color;
	uint32_t bg_color;
} text;

vec2 quad_vtx[] = {{0,0},{1,0},{1,1},{0,1}};

text text_new(float x, float y, float font_size, uint32_t fg_color, uint32_t bg_color) {
	text t = {
		.attrs = {{ .ascii_code = 0 }},
		.x = x,
		.y = y,
		.font_size = font_size,
		.fg_color = fg_color,
		.bg_color = bg_color,
	};

	glGenVertexArrays(1, &t.vao);
	glBindVertexArray(t.vao);
		glGenBuffers(1, &t.quad_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, t.quad_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vtx), quad_vtx, GL_STATIC_DRAW);
			glVertexAttribPointer(text_prog_a_quad, 2, GL_FLOAT, GL_FALSE, 0, 0);
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

void text_set_win(float win_w, float win_h) {
	mat4 text_proj;
	glm_ortho(0, win_w, win_h, 0, 1, -1, text_proj);
	SET_UNIFORM(text_prog, u_proj_tx, glUniformMatrix4fv(_loc, 1, GL_FALSE, text_proj[0]));
}

void text_init() {
	text_prog = load_program("shaders/text.vert", "shaders/text.frag");

	text_prog_a_quad = glGetAttribLocation(text_prog, "a_quad");
		assert(text_prog_a_quad != -1);
	text_prog_a_col = glGetAttribLocation(text_prog, "a_col");
		assert(text_prog_a_col != -1);
	text_prog_a_line = glGetAttribLocation(text_prog, "a_line");
		assert(text_prog_a_line != -1);
	text_prog_a_ascii_code = glGetAttribLocation(text_prog, "a_ascii_code");
		assert(text_prog_a_ascii_code != -1);

	stbi_set_flip_vertically_on_load(true);
	int x,y,n;
	uint8_t* data = stbi_load("textures/font.png", &x, &y, &n, 0);
		assert(data);
		glGenTextures(1, &text_font_texture);
		glBindTexture(GL_TEXTURE_2D, text_font_texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
}

void text_set(text* t, const char* format, ...) {
	va_list argptr;
	va_start(argptr, format);
	vsnprintf(t->buf, TEXT_MAX_SIZE, format, argptr);
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

	SET_UNIFORM(text_prog, u_x, glUniform1f(_loc, t->x));
	SET_UNIFORM(text_prog, u_y, glUniform1f(_loc, t->y));
	SET_UNIFORM(text_prog, u_font_size, glUniform1f(_loc, t->font_size));
	SET_UNIFORM(text_prog, u_bg_color, glUniform1ui(_loc, t->bg_color));
	SET_UNIFORM(text_prog, u_fg_color, glUniform1ui(_loc, t->fg_color));

	glBindBuffer(GL_ARRAY_BUFFER, t->attrs_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(t->attrs), t->attrs, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(text_prog);
		glBindTexture(GL_TEXTURE_2D, text_font_texture);
			glBindVertexArray(t->vao);
				glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, t->buf_len);
			glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

#endif
