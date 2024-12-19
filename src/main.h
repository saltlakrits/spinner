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
	Point* buffer;
	int len;
	int max_len;
} PointBuffer;

int nc_col(double v);

void redef_color_pair(double rad);

Point calc_point(int line_len, double* rad);

Vector create_vector(int length, int start_x, int start_y);

PointBuffer create_pb(int buffer_len);
void append_pb(PointBuffer* pb, Point new_point);
