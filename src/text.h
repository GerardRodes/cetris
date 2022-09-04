#ifndef CETRIS_TEXT_H
#define CETRIS_TEXT_H


#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#include "GL/gl3w.h"

#include "util.h"
#include "cglm/cglm.h"


#define TEXT_MAX_SIZE 1024


GLuint text_prog;
GLint text_prog_a_pos;

typedef struct {
	uint32_t x;
	uint32_t y;

	// todo: or this with start x,y as uniform?
	// uint32_t col;
	// uint32_t line;

	char ascii_code;
} text_vert_attrs;

typedef struct {
	char buf[TEXT_MAX_SIZE];
	uint32_t buf_len;
	va_list argptr;

	uint32_t x;
	uint32_t y;
	uint8_t size;
	uint8_t bg_color;

	GLuint vao;
	GLuint vbo;
	text_vert_attrs attrs[TEXT_MAX_SIZE];
} text;

text text_new(uint32_t x, uint32_t y, uint8_t size, uint8_t bg_color) {
	text t = {
		.x = x,
		.y = y,
		.size = size,
		.bg_color = bg_color,

		.attrs = {{ .ascii_code = 0 }},
	};

	glGenVertexArrays(1, &t.vao);
	glBindVertexArray(t.vao);
		glGenBuffers(1, &t.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, t.vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(t.attrs), t.attrs, GL_DYNAMIC_DRAW);

			// todo: enable attributes, pos and ascii_code
			//
			// glVertexAttribPointer(text_prog_a_pos, 4, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);
			// glEnableVertexAttribArray(text_prog_a_pos);
			//
			// glVertexAttribPointer(text_prog_a_pos, 4, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);
			// glEnableVertexAttribArray(text_prog_a_pos);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return t;
}

void text_init() {
	text_prog = load_program("shaders/text.vert", "shaders/text.frag");
	text_prog_a_pos = glGetAttribLocation(text_prog, "a_pos");
		assert(text_prog_a_pos != -1);
}

void text_set(text* t, const char* format, ...) {
	va_list argptr;
	va_start(argptr, format);
	snprintf(t->buf, TEXT_MAX_SIZE, format, argptr);
	va_end(argptr);

	t->buf_len = 0;
	for (;t->buf_len < TEXT_MAX_SIZE; t->buf_len++) {
		if (t->buf[t->buf_len] == '\0') {
			break;
		}
	}
}

void text_gen_vtx(text* t) {
	uint32_t char_c = 0;
	uint32_t nl_c = 0;
	for (uint32_t i = 0; i < t->buf_len; i++) {
		if (t->buf[i] == '\n') {
			// todo:
			nl_c++;
			char_c = 0;
			continue;
		}

		if (t->buf[i] == '\t') {
			for (uint8_t j = 0; j < 4; j++) {
				t->attrs[i].x = char_c*t->size;
				t->attrs[i].y = nl_c*t->size;
				t->attrs[i].ascii_code = ' ';
				char_c++;
			}
			continue;
		}

		// todo: add the actual characters
	}
}

void text_draw(text* t) {

}

#endif
