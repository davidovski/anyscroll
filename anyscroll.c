#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <unistd.h>


static Display *dpy;
static int scr, sw, sh;

static Window root;

static void pressButton(int btn) {
	XTestFakeButtonEvent(dpy, btn, True, CurrentTime);
	XFlush(dpy);
}
static void releaseButton(int btn) {
	XTestFakeButtonEvent(dpy, btn, False, CurrentTime);
	XFlush(dpy);
}


int main(int argc, const char **argv) {
	
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
	
	fprintf(stdout, "waiting for events\n");
	
	const long mask = ButtonPressMask|ButtonReleaseMask;
//	XGrabPointer(dpy, root, True, mask, GrabModeSync, GrabModeAsync, None, None, CurrentTime);
	XGrabButton(dpy, 2, AnyModifier, root, True, mask, GrabModeSync, GrabModeAsync, None, None);
	//XSelectInput(dpy, root, KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask);

	XEvent ev;
	XButtonEvent *eb;

	int grab = 0; 

	for (;;) {
		XNextEvent(dpy, &ev);
		if (ev.type == ButtonPress) {
			eb = &ev.xbutton;
			printf("%d button pressed!\n", eb->button);
//			XGrabPointer(dpy, root, True, PointerMotionMask|ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

		} else if (ev.type == ButtonRelease) {
			eb = &ev.xbutton;
			printf("%d button released!\n", eb->button);
			//releaseButton(2);
		//	XUngrabPointer(dpy, CurrentTime);			
		}

		XAllowEvents(dpy, ReplayPointer, CurrentTime);
		XSync(dpy, 0);
//		XFlush(dpy);
	}
	XCloseDisplay(dpy);
	return 0;
}
