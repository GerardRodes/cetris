#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

uint32_t WIN_W = 1920*0.5;
uint32_t WIN_H = 1080*0.5;

GLuint quad_prog, board_prog;

GLFWwindow* window;
game g;

text board_text;
text prof_text;
uint8_t DRAW_PROF_TEXT = 0;

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
			case GLFW_KEY_F1:
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			case GLFW_KEY_F2:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			case GLFW_KEY_F3:
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				break;
			case GLFW_KEY_F12:
				DRAW_PROF_TEXT = !DRAW_PROF_TEXT;
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
	text_set_win(WIN_W, WIN_H);
	board_text.x = WIN_W-(16*7);
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
	text_set_win(WIN_W, WIN_H);

	board_prog = load_program("shaders/board.vert", "shaders/board.frag");
	quad_prog = load_program("shaders/quad.vert", "shaders/quad.frag");
	g = game_new(board_prog, quad_prog);
	on_framebuffer_size(window, WIN_W, WIN_H);

	board_text = text_new(WIN_W-(16*7), 0, 16, 0xFF'FF'FF'FF, 0xAA'00'00'00);
	prof_text = text_new(10, 10, 16*1.25, 0xFF'FF'FF'FF, 0xAA'00'00'00);

	double prev_t = 0;
	double second = 0;
	uint32_t fps = 0;
	uint32_t fps_c = 0;
	while (!glfwWindowShouldClose(window)) {
		clock_t frame_begin = clock();

		double t = glfwGetTime();
		double dt = t - prev_t;

		{
			second += dt;
			if (second >= 1.0) {
				fps = fps_c;
				second = 0;
				fps_c = 0;
			} else {
				fps_c++;
			}
		}

		double game_tick_time, game_draw_time;
		{
			text_set(&board_text, "level\n%d\nscore\n%d\nspeed\n%.2f", board_level(&g.main_board), g.main_board.score, board_autofall_time(&g.main_board));
			{
				clock_t begin = clock();
				game_tick(&g, t, dt);
				clock_t end = clock();
				game_tick_time = (double)(end - begin) / CLOCKS_PER_SEC;
			}
			{
				clock_t begin = clock();
				game_draw(&g);
				clock_t end = clock();
				game_draw_time = (double)(end - begin) / CLOCKS_PER_SEC;
			}
			{
				if (DRAW_PROF_TEXT) {
					text_draw(&prof_text);
				}
				text_draw(&board_text);
			}
		}

		{
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		clock_t frame_end = clock();
		double frame_time = (double)(frame_end - frame_begin) / CLOCKS_PER_SEC;
		text_set(
			&prof_text,
			"FPS %d\n"
			"ms %.2f\n"
			"frame: %f\n"
			"-tick: %f\n"
			"-draw: %f\n"
			, fps, dt*1000,
			frame_time,
			game_tick_time,
			game_draw_time
		);
		prev_t = t;
	}

	glfwTerminate();
	return 0;
}

