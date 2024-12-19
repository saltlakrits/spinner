#pragma once

#define K 1000

#define GRAPHICAL_X_MULTIPLIER 2.5

// if 1, white bg, else black bg
#define WHITE_BACKGROUND 1

#define MIN(A, B) ((A < B) ? A : B)

typedef struct {
  int x;
  int y;
} Point;

typedef struct {
  Point pos;
  int len;
} Vector;

typedef struct {
  Point *buffer;
  int len;
  int max_len;
} PointBuffer;

int nc_col(double v);

void redef_color_pair(double rad);

Point calc_point(int line_len, double *rad);

Vector create_vector(int length, int start_x, int start_y);

PointBuffer create_pb(int buffer_len);
void append_pb(PointBuffer *pb, Point new_point);
