#ifndef CETRIS_CUBE_H
#define CETRIS_CUBE_H

static const float a = 0.05f;
static const float b = 0.95f;
static const float cube_data[] = {
	a, a, a, 0, 0, -1,
	b, a, a, 0, 0, -1,
	b, b, a, 0, 0, -1,
	b, b, a, 0, 0, -1,
	a, b, a, 0, 0, -1,
	a, a, a, 0, 0, -1,

	a, a, b, 0, 0, +1,
	b, a, b, 0, 0, +1,
	b, b, b, 0, 0, +1,
	b, b, b, 0, 0, +1,
	a, b, b, 0, 0, +1,
	a, a, b, 0, 0, +1,

	a, b, b, -1, 0, 0,
	a, b, a, -1, 0, 0,
	a, a, a, -1, 0, 0,
	a, a, a, -1, 0, 0,
	a, a, b, -1, 0, 0,
	a, b, b, -1, 0, 0,

	b, b, b, +1, 0, 0,
	b, b, a, +1, 0, 0,
	b, a, a, +1, 0, 0,
	b, a, a, +1, 0, 0,
	b, a, b, +1, 0, 0,
	b, b, b, +1, 0, 0,

	a, a, a, 0, -1, 0,
	b, a, a, 0, -1, 0,
	b, a, b, 0, -1, 0,
	b, a, b, 0, -1, 0,
	a, a, b, 0, -1, 0,
	a, a, a, 0, -1, 0,

	a, b, a, 0, +1, 0,
	b, b, a, 0, +1, 0,
	b, b, b, 0, +1, 0,
	b, b, b, 0, +1, 0,
	a, b, b, 0, +1, 0,
	a, b, a, 0, +1, 0
};

#endif
