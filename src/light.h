#ifndef CETRIS_LIGHT_H
#define CETRIS_LIGHT_H
#include <stdint.h>
#define LIGHTS_NUM 1

#include "GL/gl3w.h"
#include "cglm/cglm.h"
#include "util.h"
#include "prog.h"
#include <math.h>

GLuint light_vbo;
GLuint light_vao;

vec3 lights[LIGHTS_NUM];

void light_tick (double t) {
	for (uint8_t i = 0; i < LIGHTS_NUM; i++) {
		lights[i][0] = sin(t);
		lights[i][1] = cos(t);
		lights[i][2] = 1;
	}
	// todo: set on loop
	SET_UNIFORM(prog_quad, u_light_pos, glUniform3f(_loc, lights[0][0], lights[0][1], lights[0][2]));
}

void light_update_vbo () {
	glBindVertexArray(light_vao);
		glBindBuffer(GL_ARRAY_BUFFER, light_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lights), lights, GL_STATIC_DRAW);
			glVertexAttribPointer(prog_light_a_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(prog_light_a_pos);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void light_draw () {
	light_update_vbo();
	glUseProgram(prog_light);
		glBindVertexArray(light_vao);
			glDrawArrays(GL_POINTS, 0, LIGHTS_NUM);
		glBindVertexArray(0);
	glUseProgram(0);
}

void light_init () {
	glEnable(GL_PROGRAM_POINT_SIZE);
	glGenVertexArrays(1, &light_vao);
	glGenBuffers(1, &light_vbo);
}

#endif
