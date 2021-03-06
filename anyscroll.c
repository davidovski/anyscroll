#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <unistd.h>
#include <X11/extensions/XInput2.h>
#include <pthread.h>
#include <math.h>


static Display *dpy;
static int scr, sw, sh;
static int sx = -1, sy = -1;
static Window root;

static int tx, ty;
static int vx, vy;

static const int direction = -1; // direction to scroll in. 1 is natural, -1 is in direction of movement
static const int pixels_to_scroll = 8; // number of pixels to move before one scroll event


static void scroll(int v) {
	v = v * direction;
	printf("scrolling with v of %d\n", v);

	int btn;
	if (v > 0) btn = 4;
	else if (v < 0) btn = 5;
	else return;	
	
	int times = abs(v);
	for (int i = 0; i < times; i++) {
		tx++;
		if (tx % pixels_to_scroll == 0) {
			XTestFakeButtonEvent(dpy, btn, True, CurrentTime);
			XFlush(dpy);
			XTestFakeButtonEvent(dpy, btn, False, CurrentTime);
			XFlush(dpy);
		}
	}
}

static void select_events(Display *dpy, Window win) {
	XIEventMask evmasks[1];
	unsigned char mask1[(XI_LASTEVENT + 7)/8];

	memset(mask1, 0, sizeof(mask1));

	XISetMask(mask1, XI_RawButtonPress);
	XISetMask(mask1, XI_RawButtonRelease);

	evmasks[0].deviceid = XIAllMasterDevices;
	evmasks[0].mask_len = sizeof(mask1);
	evmasks[0].mask = mask1;

	XISelectEvents(dpy, win, evmasks, 1);
	XFlush(dpy);
}

static void getmousepos(int *px, int *py) {
	Window r, child;
	int rx, ry;
	unsigned int mask;
	XQueryPointer(dpy, root, &r,  &child, px, py, &rx, &ry, &mask);
}

static void* loop() {
	int px, py, lx, ly, dx, dy;
	while (1) {
 		lx =  px, ly = py;
		getmousepos(&px, &py);
		dx = px - lx;
		dy = py - ly;

		if (sx != -1 && sy != -1) scroll(dy);

		if (dx != 0 | dy != 0) {
			//printf("%d, %d with deltas (%d, %d)\n", sx, sy,  dx, dy);
		}
		sleep(0.1);
	}
}

static void mouse_down(XIRawEvent *xev) {
	getmousepos(&sx, &sy);
}

static void mouse_up(XIRawEvent *xev) {
	sx = -1;
	sy = -1;
}

int main(int argc, const char **argv) {
	XInitThreads();
	for (int i = 1; i < argc; i++) {
		if (argv[i][1] == 'h') {
			fprintf(stdout, "there is no help to be given\n");
		}
	}
	
	if (!(dpy = XOpenDisplay(0x0))) return 1;

	scr = DefaultScreen(dpy);
	root = XDefaultRootWindow(dpy);
	sw = DisplayWidth(dpy, scr);
	sh = DisplayHeight(dpy, scr);
	
	fprintf(stdout, "waiting for events...\n");

	XEvent ev;
	XIEvent *xi_event;
	XIRawEvent *xev;
	XGenericEventCookie *cookie = &ev.xcookie;
	
	select_events(dpy, root);

	pthread_t id[2];
	pthread_create(&id[0], NULL, loop, &argv);
	
	for (;;) {
		if (XCheckTypedEvent(dpy, GenericEvent ,&ev)) {
			if (cookie->type != GenericEvent || !XGetEventData(dpy, cookie)) {
				continue;
			}


			xi_event = (XIEvent *) cookie->data;
			xev = (XIRawEvent *) xi_event;
			switch (cookie->evtype) {
				case XI_RawButtonPress:
					if (xev->detail == 2) mouse_down(xev);
				break;
				case XI_RawButtonRelease:
					if (xev->detail == 2) mouse_up(xev);
				break;
			}
			//if (t - CurrentTime < 500) {
			//	XFreeEventData(dpy, cookie);
			//}
		}
	} // there is no way out of this loop lol
	XCloseDisplay(dpy);
	return 0;
}
