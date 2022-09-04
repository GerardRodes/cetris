#ifndef OGL_UTIL_H
#define OGL_UTIL_H

#include <math.h>
#define __USE_POSIX199309

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>

#include "GL/gl3w.h"


#define SET_UNIFORM(_prog, _name, _cb)\
if (_prog != 0) {\
	glUseProgram(_prog);\
	GLint _loc = glGetUniformLocation(_prog, #_name);\
	if (_loc == -1) {\
		fprintf(stderr, "uniform " #_name " not found\n");\
	} else {\
		_cb;\
	}\
	glUseProgram(0);\
}


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


void APIENTRY ogl_on_message(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *msg,
	const void *userParam
) {
	char* _source;
	char* _type;
	char* _severity;

	switch (source) {
		case GL_DEBUG_SOURCE_API:
			_source = "API";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			_source = "WINDOW_SYSTEM";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			_source = "SHADER_COMPILER";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			_source = "THIRD_PARTY";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			_source = "APPLICATION";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			_source = "OTHER";
			break;
		default:
		_source = "UNKNOWN";
		break;
	}

	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			_type = "ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			_type = "DEPRECATED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			_type = "UNDEFINED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			_type = "PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			_type = "PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_OTHER:
			_type = "OTHER";
			break;
		case GL_DEBUG_TYPE_MARKER:
			_type = "MARKER";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			_type = "PUSH_GROUP";
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			_type = "POP_GROUP";
			break;
		default:
		_type = "UNKNOWN";
		break;
	}

	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			_severity = "HIGH";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			_severity = "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			_severity = "LOW";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			_severity = "NOTIFICATION";
			break;
		default:
		_severity = "UNKNOWN";
		break;
	}

	fprintf(stderr,
		"%d: %s of %s severity, raised from %s: %s\n",
		id, _type, _severity, _source, msg
	);
}

void msleep(long msec) {
	struct timespec ts;
	int res;

	ts.tv_sec = msec / 1000;
	ts.tv_nsec = (msec % 1000) * 1000000;

	do {
		res = nanosleep(&ts, &ts);
	} while (res && errno == EINTR);
}

float lerp(float start, float end, float t) {
	return (1-t)*start + t*end;
}

float normalize(float min, float max, float v) {
	return (v-min)/(max-min);
}

float bezier3(float p0, float p1, float p2, float p3, float t) {
	return
		powf(1-t, 3)*p0 +
		3*powf(1-t, 2)*t*p1 +
		3*(1-t)*powf(t, 2)*p2 +
		powf(t, 3)*p3
	;
}

#endif
