#include <stdio.h>
#include <stdlib.h>

#include "GL/gl3w.h"
#include "GL/glcorearb.h"
#include "GLFW/glfw3.h"

#include "util.h"

#define OGL_MAJOR 3
#define OGL_MINOR 3

#define BOARD_W 10
#define BOARD_H 10

size_t WIN_W = 640;
size_t WIN_H = 480;

const unsigned char pieces[1][4][16] = {
	{
		{
			0,0,0,0,
			1,1,1,1,
			0,0,0,0,
			0,0,0,0,
		},{
			0,0,1,0,
			0,0,1,0,
			0,0,1,0,
			0,0,1,0,
		},{
			0,0,0,0,
			0,0,0,0,
			1,1,1,1,
			0,0,0,0,
		},{
			0,1,0,0,
			0,1,0,0,
			0,1,0,0,
			0,1,0,0,
		},
	}
};

const unsigned char board[BOARD_H][BOARD_W] = {
	{1,0,1,0,1,0,1,0,1,0},
	{0,1,1,1,0,1,0,1,0,1},
	{1,0,1,0,1,0,1,0,1,0},
	{0,1,0,1,0,1,0,1,0,1},
	{1,0,1,0,1,0,1,0,1,0},
	{0,1,0,1,0,1,0,1,0,1},
	{1,0,1,0,1,0,1,0,1,0},
	{0,1,0,1,0,1,0,1,0,1},
	{1,0,1,0,1,0,1,0,1,0},
	{0,1,0,1,0,1,0,1,0,1},
};

GLFWwindow* window;

void on_key (GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS) {
		return;
	}

	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	}
}

void on_framebuffer_size(GLFWwindow* window, int width, int height) {
	WIN_W = width;
	WIN_H = height;
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

int main () {
	// GLFW
	{
		if (!glfwInit()) {
			panic("[GLFW] init");
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OGL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OGL_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		window = glfwCreateWindow(WIN_W, WIN_H, "-ogl", NULL, NULL);
		if (!window) {
			glfwTerminate();
			panic("[GLFW] create window");
		}

		glfwMakeContextCurrent(window);
		glfwSetKeyCallback(window, on_key);
		glfwSetFramebufferSizeCallback(window, on_framebuffer_size);
	}

	// GL3W
	{
		if (gl3wInit()) {
			panic("[GL3W] init");
		}
		if (!gl3wIsSupported(OGL_MAJOR, OGL_MINOR)) {
			panic("[GL3W] opengl version not supported");
		}
	}

	glViewport(0, 0, WIN_W, WIN_H);

	GLuint quad_prog = load_program("shaders/quad.vert", "shaders/quad.frag");

	const float quad_vert[4][2] = {{-1,-1},{-1,+1},{+1,+1},{+1,-1}};

	unsigned int quad_pos[BOARD_H * BOARD_W][2] = {0};
	GLsizei quad_pos_c = 0;
	for (GLsizei x = 0; x < BOARD_W; x++) {
		for (GLsizei y = 0; y < BOARD_H; y++) {
			if (board[y][x] == 0) {
				continue;
			}
			quad_pos[quad_pos_c][0] = x;
			quad_pos[quad_pos_c][1] = y;
			quad_pos_c++;
		}
	}

	{
		glUseProgram(quad_prog);
		GLint loc = glGetUniformLocation(quad_prog, "cols");
		if (loc == -1) {
			panic("uniform cols not found");
		}
		glUniform1i(loc, BOARD_W);
		glUseProgram(0);
	}

	GLuint quad_vbo, quad_pos_vbo, quad_vao;
	{
		glGenVertexArrays(1, &quad_vao);
		glBindVertexArray(quad_vao);
		{
			glGenBuffers(1, &quad_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vert), quad_vert, GL_STATIC_DRAW);
			{
				GLint pos = glGetAttribLocation(quad_prog, "a_quad");
				if (pos == -1) {
					panic("attrib a_quad not found");
				}
				glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(pos);
			}
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		{
			glGenBuffers(1, &quad_pos_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, quad_pos_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quad_pos), quad_pos, GL_STATIC_DRAW);
			{
				GLint pos = glGetAttribLocation(quad_prog, "a_ins_pos");
				if (pos == -1) {
					printf("attrib a_ins_pos not found\n");
				}
				glVertexAttribPointer(pos, 2, GL_UNSIGNED_INT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(pos);
				glVertexAttribDivisor(pos, 1);
			}
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		glBindVertexArray(0);
	}

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2, 0.4, 0.6, 1);
    glClear(GL_COLOR_BUFFER_BIT);

		{
			glUseProgram(quad_prog);
			glBindVertexArray(quad_vao);
			glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, quad_pos_c);
		}

    glfwSwapBuffers(window);
    glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

