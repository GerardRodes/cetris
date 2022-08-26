#include <stdio.h>
#include <stdlib.h>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"

#include "util.h"
#include "board.h"

#define OGL_MAJOR 3
#define OGL_MINOR 3

size_t WIN_W = 500;
size_t WIN_H = 500;

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

GLuint quad_prog, board_prog;

mat4 proj = {0};

void update_proj() {
	glm_ortho_default((float)WIN_W / WIN_H, &proj[0]);
	SET_UNIFORM(quad_prog, u_proj_tx, glUniformMatrix4fv(_loc, 1, GL_FALSE, proj[0]))
	SET_UNIFORM(board_prog, u_proj_tx, glUniformMatrix4fv(_loc, 1, GL_FALSE, proj[0]))
}

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
	update_proj();
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

	// DEBUG
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(ogl_on_message, 0);
	}

	board_prog = load_program("shaders/board.vert", "shaders/board.frag");
	quad_prog = load_program("shaders/quad.vert", "shaders/quad.frag");

	board main_board = board_init(10, 20, board_prog, quad_prog);

	// DEBUG BOARD
	{
		for (unsigned char row = 0; row < main_board.rows; row++) {
			for (unsigned char col = 0; col < main_board.cols; col++) {
				board_set_cell(main_board, col, row, (col+row) % 2);
			}
		}
	}

	on_framebuffer_size(window, WIN_W, WIN_H);

	{
		mat4 board_tx = {0};
		board_tx_matrix(main_board, &board_tx, (vec2){0, 0});
		SET_UNIFORM(board_prog, u_board_tx, glUniformMatrix4fv(_loc, 1, GL_FALSE, board_tx[0]));
		SET_UNIFORM(quad_prog, u_board_tx, glUniformMatrix4fv(_loc, 1, GL_FALSE, board_tx[0]));
	}

	board_init_vao(&main_board);
	board_init_grid_vao(&main_board);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2, 0.4, 0.6, 1);
    glClear(GL_COLOR_BUFFER_BIT);

		board_draw(main_board);

    glfwSwapBuffers(window);
    glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

