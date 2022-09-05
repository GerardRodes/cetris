#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"

#include "piece.h"
#include "text.h"
#include "util.h"
#include "board.h"
#include "game.h"

#define OGL_MAJOR 3
#define OGL_MINOR 3

size_t WIN_W = 500;
size_t WIN_H = 500;

GLuint quad_prog, board_prog;

GLFWwindow* window;
game g;

void on_key (GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		// oneshot keystrokes
		switch (key) {
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
			case GLFW_KEY_SPACE:
				board_falling_lock(&g.main_board);
				break;
			case GLFW_KEY_UP:
				board_falling_rotate(&g.main_board);
				break;
			// =========================
			// DEBUG
			// =========================
			case GLFW_KEY_R:
				board_falling_spawn(&g.main_board);
				break;
			case GLFW_KEY_PERIOD:
				g.main_board.falling.p.t = (g.main_board.falling.p.t+1) % PT_NONE;
				break;
			case GLFW_KEY_COMMA:
				g.main_board.falling.p.t = (g.main_board.falling.p.t-1) % PT_NONE;
				break;
			case GLFW_KEY_1:
				g.main_board.falling.p.t = PT_T;
				break;
			case GLFW_KEY_2:
				g.main_board.falling.p.t = PT_S;
				break;
			case GLFW_KEY_3:
				g.main_board.falling.p.t = PT_Z;
				break;
			case GLFW_KEY_4:
				g.main_board.falling.p.t = PT_I;
				break;
			case GLFW_KEY_5:
				g.main_board.falling.p.t = PT_J;
				break;
			case GLFW_KEY_6:
				g.main_board.falling.p.t = PT_L;
				break;
			case GLFW_KEY_7:
				g.main_board.falling.p.t = PT_O;
				break;
		}
	}

	if (action == GLFW_PRESS || action == GLFW_RELEASE) {
		switch (key) {
			case GLFW_KEY_LEFT:
				if (action == GLFW_PRESS) {
					g.input.piece.move.left = 1;
				} else {
					g.input.piece.move.left = 0;
					g.input.piece.last.left = 0;
				}
				break;
			case GLFW_KEY_RIGHT:
				if (action == GLFW_PRESS) {
					g.input.piece.move.right = 1;
				} else {
					g.input.piece.move.right = 0;
					g.input.piece.last.right = 0;
				}
				break;
			case GLFW_KEY_DOWN:
				if (action == GLFW_PRESS) {
					g.input.piece.move.down = 1;
				} else {
					g.input.piece.move.down = 0;
					g.input.piece.last.down = 0;
				}
				break;
			// =========================
			// CAMERA
			// =========================
			case GLFW_KEY_A:
				if (action == GLFW_PRESS) {
					g.input.camera.move.left = 1;
				} else {
					g.input.camera.move.left = 0;
				}
				break;
			case GLFW_KEY_D:
				if (action == GLFW_PRESS) {
					g.input.camera.move.right = 1;
				} else {
					g.input.camera.move.right = 0;
				}
				break;
			case GLFW_KEY_W:
				if (action == GLFW_PRESS) {
					g.input.camera.move.up = 1;
				} else {
					g.input.camera.move.up = 0;
				}
				break;
			case GLFW_KEY_S:
				if (action == GLFW_PRESS) {
					g.input.camera.move.down = 1;
				} else {
					g.input.camera.move.down = 0;
				}
				break;
			case GLFW_KEY_Q:
				if (action == GLFW_PRESS) {
					g.input.camera.move.backward = 1;
				} else {
					g.input.camera.move.backward = 0;
				}
				break;
			case GLFW_KEY_E:
				if (action == GLFW_PRESS) {
					g.input.camera.move.forward = 1;
				} else {
					g.input.camera.move.forward = 0;
				}
				break;
		}
	}

}

void on_framebuffer_size(GLFWwindow* window, int width, int height) {
	WIN_W = width;
	WIN_H = height;
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	game_update_tx_matrix_uniforms(&g, WIN_W, WIN_H);
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

		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(WIN_W, WIN_H, "[float] cetris", NULL, NULL);
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
	text_init();

	text info_text = text_new(0, 0, 16, 0xFF'FF'FF'FF, 0x33'33'33'CC);

	board_prog = load_program("shaders/board.vert", "shaders/board.frag");
	quad_prog = load_program("shaders/quad.vert", "shaders/quad.frag");
	g = game_new(board_prog, quad_prog);
	on_framebuffer_size(window, WIN_W, WIN_H);

	size_t frame = 0;
	double prev_t = 0;
	while (!glfwWindowShouldClose(window)) {
		double t = glfwGetTime();
		double dt = t - prev_t;

		text_set(&info_text, "hola %d", 10);

		{
			game_tick(&g, t, dt);
			game_draw(&g);
			text_draw(&info_text);
		}
		{
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		frame++;
		prev_t = t;
	}

	glfwTerminate();
	return 0;
}

