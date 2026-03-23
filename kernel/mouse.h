#ifndef MOUSE_H
#define MOUSE_H

void mouse_init();
int mouse_poll();
int mouse_get_x();
int mouse_get_y();
int mouse_left_button();
int mouse_right_button();

#endif
