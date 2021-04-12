#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char **argv) {
	Display *d;
	XEvent e;
	const char *message = "do you see the fun in gcc?";
	
	d = XOpenDisplay(NULL);
	if (d == NULL) {
		fprintf(stderr, "error opening display\n");
		exit(1);
	}

	int s = DefaultScreen(d);
	Window w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, 100, 100, 1, BlackPixel(d, s), WhitePixel(d, s));
	
	XSelectInput(d, w, ExposureMask | KeyPressMask);
	XMapWindow(d, w);
	
	while (1) {
		XNextEvent(d, &e);
		if (e.type == Expose) {
			XDrawString(d, w, DefaultGC(d, s), 10, 50, message, strlen(message));
		}	
	}

	XCloseDisplay(d);
	return 0;
}
