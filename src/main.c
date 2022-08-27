#include <stdio.h>
#include <stdlib.h>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"

#include "piece.h"
#include "util.h"
#include "board.h"
#include "game.h"

#define OGL_MAJOR 3
#define OGL_MINOR 3

size_t WIN_W = 500;
size_t WIN_H = 500;

GLuint quad_prog, board_prog;

mat4 proj_tx = {0};

void update_proj() {
	glm_ortho_default((float)WIN_W / WIN_H, &proj_tx[0]);
	SET_UNIFORM(quad_prog, u_proj_tx, glUniformMatrix4fv(_loc, 1, GL_FALSE, proj_tx[0]))
	SET_UNIFORM(board_prog, u_proj_tx, glUniformMatrix4fv(_loc, 1, GL_FALSE, proj_tx[0]))
}

GLFWwindow* window;
game g;

void on_key (GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_RELEASE) {
		return;
	}

	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case GLFW_KEY_SPACE:
		board_falling_rotate(&g.main_board);
		break;
	case GLFW_KEY_LEFT:
		board_falling_move_left(&g.main_board);
		break;
	case GLFW_KEY_RIGHT:
		board_falling_move_right(&g.main_board);
		break;
	case GLFW_KEY_UP:
		board_falling_lock(&g.main_board);
		break;
	case GLFW_KEY_DOWN:
		board_falling_move_down(&g.main_board);
		break;
	case GLFW_KEY_R:
		board_falling_spawn(&g.main_board);
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
	srand(time(0));

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
		// glfwSwapInterval(0); // remove vsync (60fps cap)
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

	piece_init();

	board_prog = load_program("shaders/board.vert", "shaders/board.frag");
	quad_prog = load_program("shaders/quad.vert", "shaders/quad.frag");
	g = game_new(board_prog, quad_prog);
	on_framebuffer_size(window, WIN_W, WIN_H);

	size_t frame = 0;
	while (!glfwWindowShouldClose(window)) {
		double t = glfwGetTime();

		game_tick(&g, t);
		game_draw(&g);

    glfwSwapBuffers(window);
    glfwPollEvents();
		frame++;
	}

	glfwTerminate();
	return 0;
}

