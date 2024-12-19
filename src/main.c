#include "main.h"
#include <math.h>
#include <ncurses.h>
#include <unistd.h>

#define K 1000

#define GRAPHICAL_X_MULTIPLIER 2

#define MIN(A, B) ((A < B) ? A : B)

/* TODO:
  - figure out what the stack smash is all about
	- only seems to trigger when quitting

	- make color depend on something like...
	RGB: (R_CONST * sin(rad), G_CONST * sin(rad), B_CONST * sin(rad))

	- make center tail(/s?) than the others

	- generalize everything and scale to screen
	- i.e., scale amount of vectors made and drawn,
	- perhaps scale buffer length as well
*/

Point calc_point(const int line_len, double *rad) {
  // rad will grow infinitely, so we will keep
  // modulo'ing it to ensure it doesn't
  *rad = fmod(*rad, 2 * M_PI);

  Point new_point;
  new_point.x = (int)(line_len * sin(*rad));
  new_point.y = (int)(line_len * cos(*rad));

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

PointBuffer create_pb() {
  PointBuffer new_pb;
  new_pb.len = 0;
  return new_pb;
}

void append_pb(PointBuffer *pb, Point new_point) {

  // if len isn't 4 yet, inc len
  if (pb->len < 10) {
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
  // start_color();

  refresh();

  // approx center of screen
  Point screen_center;
  screen_center.x = COLS / 2;
  screen_center.y = LINES / 2;
  // reasonable circle radius
  int radius = MIN(COLS / 2, LINES / 2) - 2;

  // variable to hold radians
  double rad = 0.0;

  // make some vectors
	// to generalize this: find minimum radius vector, maximum width vector
	// then create one for each radius
  Vector vecs[3];
  vecs[0] =
      create_vector(radius - 1, screen_center.x, screen_center.y - 1);
  vecs[1] = create_vector(radius, screen_center.x, screen_center.y);
  vecs[2] =
      create_vector(radius + 1, screen_center.x, screen_center.y + 1);

  // we will draw these characters:
  // OOo.
  // (first O is bold)

  // we'll make some buffers to hold the points
  // we will write points calculated from the vectors to these buffers
  // and then draw all the points from the buffers each frame
  PointBuffer pbs[4];
  for (int i = 0; i < 4; i++) {
    pbs[i] = create_pb();
  }

  while (1) {
    // main loop
    int ch;
    ch = getch();

    if (ch == 'q') {
      break;
    }

    erase();

    rad += 0.1;
    for (int i = 0; i < 4; i++) {

      append_pb(&pbs[i], vecs[i].pos);

      for (int j = 0; j < pbs[i].len; j++) {
        char char_to_print;
        if (j <= 3) {
          char_to_print = 'O';
        } else if (j <= 7) {
          char_to_print = 'o';
        } else {
          char_to_print = '.';
        }

        if (j < 4) {
          attron(A_BOLD);
        }

        mvprintw(screen_center.y - pbs[i].buffer[j].y,
                 screen_center.x + (int)(pbs[i].buffer[j].x * 2.5), "%c",
                 char_to_print);

        if (j < 4) {
          attroff(A_BOLD);
        }
      }

      vecs[i].pos = calc_point(vecs[i].len, &rad);
    }

    refresh();

    usleep(50 * K);
  }

  curs_set(1);
  endwin();

  return 0;
}
