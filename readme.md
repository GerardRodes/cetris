# Cetris

Tetris game written in C and rendered with OpenGL

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/4HhIdkk7cKw/0.jpg)](https://www.youtube.com/watch?v=4HhIdkk7cKw)

## Compiling

```sh
# compile
RELEASE=y make bin/main
# run
./bin/main
```

All the dependencies are included.
You will need to fetch the submodules first
```sh
git submodule update --init --recursive
```
The only external lib used is `glfw3`, and I've included an already compiled amd64 version.
Only tested on Linux. For another platform, you'll need to compile and correctly link `glfw3`.

## Features
- minimal dependencies:
  - cglm: for maths
  - glfw: context window initialization
  - gl3w: opengl func loader
- dynamic lighting: yes really, it moves around the board, it is a very simple implementation. Can be visualized uncommenting `light_draw();` on `main.c`
- instanced rendering
- bitmap text rendering implementation

## Controls

|key|control|
|---|---|
| ESCAPE | exit |
| LEFT | move left |
| RIGHT | move right |
| DOWN | move down |
| UP | rotate |
| SPACE | place falling piece |
| W | move camera along y axis |
| A | move camera along y axis |
| S | move camera along x axis |
| D | move camera along x axis |
| Q | move camera along z axis |
| E | move camera along z axis |
| 1 | convert falling piece to piece `T` |
| 2 | convert falling piece to piece `S` |
| 3 | convert falling piece to piece `Z` |
| 4 | convert falling piece to piece `I` |
| 5 | convert falling piece to piece `J` |
| 6 | convert falling piece to piece `L` |
| 7 | convert falling piece to piece `O` |
| F1 | glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) |
| F2 | glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) |
| F3 | glPolygonMode(GL_FRONT_AND_BACK, GL_POINT) |
| F12 | fps counter |
