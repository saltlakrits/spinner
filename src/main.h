#pragma once

#include <math.h>

typedef struct {
	int x;
	int y;
} Point;

typedef struct {
	Point pos;
	int len;
} Vector;

typedef struct {
	Point buffer[10];
	int len;
} PointBuffer;

Point calc_point(int line_len, double* rad);

Vector create_vector(int length, int start_x, int start_y);

PointBuffer create_pb();
void append_pb(PointBuffer* pb, Point new_point);
