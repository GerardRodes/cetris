#ifndef OGL_UTIL_H
#define OGL_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "GL/gl3w.h"

void panic (const char* msg) {
	fprintf(stderr, "\npanic: %s\n\n", msg);
	exit(EXIT_FAILURE);
}

void panicf (const char* format, ...) {
  char buf[1024];
	va_list argptr;
	va_start(argptr, format);
	snprintf(buf, 1024, "\npanic: %s\n\n", format);
	vfprintf(stderr, buf, argptr);
	va_end(argptr);
	exit(EXIT_FAILURE);
}

char *load_file(const char *path) {
	FILE *file = fopen(path, "rb");
	if (!file) {
		panicf("[load_file] failed to open file %s", path);
	}
	fseek(file, 0, SEEK_END);
	unsigned long length = ftell(file);
	rewind(file);
	char *buf = calloc(length + 1, sizeof(char));
	if (fread(buf, 1, length, file) != length) {
		panicf("[load_file] failed to read file %s", path);
	}
	fclose(file);
	return buf;
}


GLuint make_shader(GLenum type, const char *source) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		GLchar *info = calloc(length, sizeof(GLchar));
		glGetShaderInfoLog(shader, length, NULL, info);
		panicf("[make_shader] failed to compile\n%s", info);
		free(info);
	}
	return shader;
}

GLuint load_shader(GLenum type, const char *path) {
	char *data = load_file(path);
	GLuint result = make_shader(type, data);
	free(data);
	return result;
}


GLuint make_program(GLuint shader1, GLuint shader2) {
	GLuint program = glCreateProgram();
	glAttachShader(program, shader1);
	glAttachShader(program, shader2);
	glLinkProgram(program);
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		GLchar *info = calloc(length, sizeof(GLchar));
		glGetProgramInfoLog(program, length, NULL, info);
		panicf("[make_program] glLinkProgram failed: %s\n", info);
	}
	glDetachShader(program, shader1);
	glDetachShader(program, shader2);
	glDeleteShader(shader1);
	glDeleteShader(shader2);
	return program;
}

GLuint load_program(const char *vert_path, const char *frag_path) {
	GLuint shader1 = load_shader(GL_VERTEX_SHADER, vert_path);
	GLuint shader2 = load_shader(GL_FRAGMENT_SHADER, frag_path);
	GLuint program = make_program(shader1, shader2);
	return program;
}

GLuint gen_buffer(GLsizei size, GLfloat *data) {
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return buffer;
}

void update_buffer() {
	// todo:
	// - https://www.khronos.org/opengl/wiki/Buffer_Object#:~:text=do%20this%20once.-,Data%20Specification,-We%20have%20seen
	// - https://www.khronos.org/opengl/wiki/Buffer_Object#:~:text=be%20written%20to.-,Mapping,-glBufferSubData%20is%20a
}

void del_buffer(GLuint buffer) {
  glDeleteBuffers(1, &buffer);
}

#endif
