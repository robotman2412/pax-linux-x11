
#pragma once

#include <stdlib.h>
#include <stdio.h>

#include <pax_gfx.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <unistd.h>
#include <time.h>

extern Display  *disp;
extern int       screen;
extern Window    window;
extern GC        gc;
extern XEvent    event;
extern KeySym    key;
extern pax_buf_t buf;

uint64_t millis();
void init_x();
void close_x();
void disp_flush();
void disp_sync();
void redraw();
