#include "main.h"
#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#define K 1000

#define GRAPHICAL_X_MULTIPLIER 2.5

// if 1, white bg, else black bg
#define WHITE_BACKGROUND 1

#define MIN(A, B) ((A < B) ? A : B)

Point calc_point(const int line_len, double *rad) {
  // rad will grow infinitely, so we will keep
  // modulo'ing it to ensure it resets appropriately
  *rad = fmod(*rad, 2 * M_PI);

  Point new_point;
  new_point.x = line_len * sin(*rad);
  new_point.y = line_len * cos(*rad);

  return new_point;
}

Vector create_vector(int length, int start_x, int start_y) {
  Point new_point;
  new_point.x = start_x;
  new_point.y = start_y;

  Vector new_vec;
  new_vec.pos = new_point;
  new_vec.len = length;

  return new_vec;
}

PointBuffer create_pb(int buffer_len) {
  PointBuffer new_pb;
  new_pb.buffer = malloc(buffer_len * sizeof(*new_pb.buffer));
  new_pb.len = 0;
  new_pb.max_len = buffer_len;
  return new_pb;
}

void append_pb(PointBuffer *pb, Point new_point) {

  // if len isn't 4 yet, inc len
  if (pb->len < pb->max_len) {
    pb->len += 1;
  }

  // if we have more than one point,
  // we want to shift them all back by one index
  // (effectively dropping the last one)
  if (pb->len > 1) {
    for (int i = pb->len - 1; i > 0; i--) {
      pb->buffer[i].x = pb->buffer[i - 1].x;
      pb->buffer[i].y = pb->buffer[i - 1].y;
    }
  }

  // make the new point the first point in the buffer
  pb->buffer[0] = new_point;
}

int nc_col(double v) { return (int)(v / 256 * 1000); }

void redef_color_pair(double rad) {
  double red = fabs(256.0 * sin(rad / 2));
  double green = 256.0 - fabs(192.0 * sin(rad / 2));
  const double blue = 128;
  init_color(1, nc_col(red), nc_col(green), nc_col(blue));
}

int main() {
  // ncurses init screen
  initscr();

  // more ncurses init
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  nodelay(stdscr, TRUE);
  // need color or no?
  start_color();

  // verified by printing COLORS: 256 colors in term

  // max color number is 255, 0 is redefinable
  // max pair is 255, 0 is not redefinable
  if (WHITE_BACKGROUND) {
    init_pair(1, COLOR_WHITE, COLOR_WHITE);
    bkgd(COLOR_PAIR(1));
    init_pair(255, 1, COLOR_WHITE);
  } else {
    init_pair(255, 1, COLOR_BLACK);
  }

  refresh();

  // approx center of screen
  Point screen_center;
  screen_center.x = COLS / 2;
  screen_center.y = LINES / 2;

  // reasonable maximum circle radius
  int radius = MIN(COLS / 2, LINES / 2) - 1;
  // radius * 0.5 amount of vectors?

  // variable to hold radians
  double rad = 0.0;

  // dynamic amount of vectors
  int dyn_v_len = radius * 0.25;
  Vector *dyn_v = malloc(dyn_v_len * sizeof(*dyn_v));
  for (int i = dyn_v_len - 1; i >= 0; i--) {
    dyn_v[i] = create_vector(radius - i, screen_center.x, screen_center.y - i);
  }

  // dynamic buffers: we need the same amount of buffers as we have
  // vectors. each will be 4 times as long as the amount of vecs.
  PointBuffer *pbs = malloc(dyn_v_len * sizeof(*pbs));
  for (int i = 0; i < dyn_v_len; i++) {
    // each buffer is dyn_v_len * 2 indices
    pbs[i] = create_pb(dyn_v_len * 4);
  }

  while (1) {
    // main loop
    int ch;
    ch = getch();

    if (ch == 'q') {
      break;
    }

    // clear screen before each draw -- maybe not how it's
    // intended to be done?
    erase();

    rad += 0.1;
    for (int i = 0; i < dyn_v_len; i++) {
      redef_color_pair(rad);

      append_pb(&pbs[i], dyn_v[i].pos);

      for (int j = 0; j < pbs[i].len; j++) {
        char char_to_print;
        if (j <= pbs[i].max_len * 0.33) {
          char_to_print = 'O';
        } else if (j <= pbs[i].max_len * 0.82) {
          char_to_print = 'o';
        } else {
          char_to_print = '.';
        }

        if (j < pbs[i].max_len * 0.16) {
          attron(A_BOLD);
        }

        attron(COLOR_PAIR(255));
        mvprintw(screen_center.y - pbs[i].buffer[j].y,
                 screen_center.x +
                     (pbs[i].buffer[j].x * GRAPHICAL_X_MULTIPLIER),
                 "%c", char_to_print);
        attroff(COLOR_PAIR(255));

        if (j < pbs[i].max_len * 0.16) {
          attroff(A_BOLD);
        }
      }

      dyn_v[i].pos = calc_point(dyn_v[i].len, &rad);
    }

    refresh();

    usleep(32 * K); // 32 ms per draw => 30 fps
  }

  curs_set(1);
  endwin();

  return 0;
}
