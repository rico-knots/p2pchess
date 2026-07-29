#ifndef WINDOW_H
#define WINDOW_H

extern int window_width, window_height;

int window_init(int width, int height);
int window_should_close();
void window_begin_frame();
void window_end_frame();
void window_shutdown();

#endif