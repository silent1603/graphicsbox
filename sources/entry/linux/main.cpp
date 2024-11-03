
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Xatom.h>
#include <glad/gl.h>
#include <glad/glx.h>
#include <imgui_impl_opengl3.h>

int width = 800;
int height = 600;
//_NET_WM_STATE_REMOVE = 0;    /* remove/unset property */
//_NET_WM_STATE_ADD = 1;    /* add/set property */
//_NET_WM_STATE_TOGGLE = 2;    /* toggle property  */
void setSizeHint(Display* display, Window window, int minWidth, int minHeight,
	int maxWidth, int maxHeight) //minimize window
{
	XSizeHints hints = {};
	if (minWidth > 0 && minHeight > 0) hints.flags |= PMinSize;
	if (maxWidth > 0 && maxHeight > 0) hints.flags |= PMaxSize;

	hints.min_width = minWidth;
	hints.min_height = minHeight;
	hints.max_width = maxWidth;
	hints.max_height = maxHeight;

	XSetWMNormalHints(display, window, &hints);
}

Status toggleBorderMaximize(Display* display, Window window)
{
	XClientMessageEvent ev = {};
	Atom wmState = XInternAtom(display, "_NET_WM_STATE", False);
	Atom maxH = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
	Atom maxV = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", False);

	if (wmState == None) return 0;

	ev.type = ClientMessage;
	ev.format = 32;
	ev.window = window;
	ev.message_type = wmState;
	ev.data.l[0] = 2; // _NET_WM_STATE_TOGGLE 2 according to spec; Not defined in my headers
	ev.data.l[1] = maxH;
	ev.data.l[2] = maxV;
	ev.data.l[3] = 1;

	return XSendEvent(display, DefaultRootWindow(display), False,
		SubstructureNotifyMask,
		(XEvent*)&ev);
}

Status toggleNoBorderMaximize(Display* display, Window window)
{
	XClientMessageEvent ev = {};
	Atom wmState = XInternAtom(display, "_NET_WM_STATE", False);
	Atom noBorderFullScreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

	if (wmState == None) return 0;

	ev.type = ClientMessage;
	ev.format = 32;
	ev.window = window;
	ev.message_type = wmState;
	ev.data.l[0] = 2; // _NET_WM_STATE_TOGGLE 2 according to spec; Not defined in my headers
	ev.data.l[1] = noBorderFullScreen;
	ev.data.l[2] = 0;    // no second property to toggle
	ev.data.l[3] = 1;    // source indication: application
	ev.data.l[4] = 0;    // unused
	return XSendEvent(display, DefaultRootWindow(display), False,
		SubstructureNotifyMask,
		(XEvent*)&ev);
}

void applyNoBorderMaximize(Display* display, Window window)
{
	Atom wm_state = XInternAtom(display, "_NET_WM_STATE", true);
	Atom wm_fullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", true);

	XChangeProperty(display, window, wm_state, XA_ATOM, 32,
		PropModeReplace, (unsigned char*)&wm_fullscreen, 1);
}

int main(void) {
	Display* display = XOpenDisplay(NULL);
	if (display == NULL) {
		printf("cannot connect to X server\n");
		return 1;
	}

	int screen = DefaultScreen(display);
	Window root = RootWindow(display, screen);
	Visual* visual = DefaultVisual(display, screen);

	Colormap colormap = XCreateColormap(display, root, visual, AllocNone);

	XSetWindowAttributes attributes;
	attributes.bit_gravity = StaticGravity;
	attributes.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask;
	attributes.colormap = colormap;
	unsigned long attributeMask = CWColormap | CWEventMask | CWBitGravity;
	Window window =
		XCreateWindow(display, root, 0, 0, width, height, 0,
			DefaultDepth(display, screen), InputOutput, visual,
			attributeMask, &attributes);
	//applyNoBorderMaximize(display, window);

	XMapWindow(display, window);
	XStoreName(display, window, "Graphics Box (X11)");

	if (!window) {
		printf("Unable to create window.\n");
		return 1;
	}

	int glx_version = gladLoaderLoadGLX(display, screen);
	if (!glx_version) {
		printf("Unable to load GLX.\n");
		return 1;
	}
	printf("Loaded GLX %d.%d\n", GLAD_VERSION_MAJOR(glx_version), GLAD_VERSION_MINOR(glx_version));
	setSizeHint(display, window, 400, 300, 0, 0);
	GLint visual_attributes[] = {
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_DOUBLEBUFFER, 1,
		GLX_DEPTH_SIZE, 24,
		None
	};

	int num_fbc = 0;
	GLXFBConfig* fbc = glXChooseFBConfig(display, screen, visual_attributes, &num_fbc);

	GLint context_attributes[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 3,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None
	};

	GLXContext context =
		glXCreateContextAttribsARB(display, fbc[0], NULL, 1, context_attributes);
	if (!context) {
		printf("Unable to create OpenGL context.\n");
		return 1;
	}

	glXMakeCurrent(display, window, context);

	int gl_version = gladLoaderLoadGL();
	if (!gl_version) {
		printf("Unable to load GL.\n");
		return 1;
	}
	printf("Loaded GL %d.%d\n", GLAD_VERSION_MAJOR(gl_version), GLAD_VERSION_MINOR(gl_version));

	XWindowAttributes gwa;
	XGetWindowAttributes(display, window, &gwa);
	glViewport(0, 0, gwa.width, gwa.height);
	glEnable(GL_DEPTH_TEST);

	toggleNoBorderMaximize(display, window);
	Atom WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False);
	if (!XSetWMProtocols(display, window, &WM_DELETE_WINDOW, 1))
	{
		printf("Couldn't register WM_DELETE_WINDOW property\n");
	}


	bool quit = false;
	int sizeChange = 0;
	while (!quit)
	{
		XEvent xev = {};
		while (XPending(display)) {
			XNextEvent(display, &xev);
			switch (xev.type)
			{
				case  Expose:
				{
					XExposeEvent* e = (XExposeEvent*)&xev;
					width = e->width;
					height = e->height;
					sizeChange = 1;
					
				}
				break;
				case DestroyNotify:
				{
					XDestroyWindowEvent* e = (XDestroyWindowEvent*)&xev;
					if (e->window == window)
					{
						quit = true;
					}
				}
				break;
				case ClientMessage:
				{
					XClientMessageEvent* e = (XClientMessageEvent*)&xev;
					if ((Atom)e->data.l[0] == WM_DELETE_WINDOW)
					{
						quit = true;
					}
				}
				break;
				case ConfigureNotify:
				{

					XConfigureEvent* e = (XConfigureEvent*)&xev;
					width = e->width;
					height = e->height;
					sizeChange = 1;
				}
				break;
				case KeyPress:
				{
					XKeyPressedEvent* e = (XKeyPressedEvent*)&xev;

					if (e->keycode == XKeysymToKeycode(display, XK_Left)) printf("left arrow pressed\n");
					if (e->keycode == XKeysymToKeycode(display, XK_Right)) printf("right arrow pressed\n");
					if (e->keycode == XKeysymToKeycode(display, XK_Up)) printf("up arrow pressed\n");
					if (e->keycode == XKeysymToKeycode(display, XK_Down)) printf("down arrow pressed\n");
				}
				break;
				case KeyRelease:
				{

					XKeyPressedEvent* e = (XKeyPressedEvent*)&xev;
					if (e->keycode == XKeysymToKeycode(display, XK_Escape))
					{
						quit = true;
					}
					if (e->keycode == XKeysymToKeycode(display, XK_Left)) printf("left arrow released\n");
					if (e->keycode == XKeysymToKeycode(display, XK_Right)) printf("right arrow released\n");
					if (e->keycode == XKeysymToKeycode(display, XK_Up)) printf("up arrow released\n");
					if (e->keycode == XKeysymToKeycode(display, XK_Down)) printf("down arrow released\n");

				}
				break;
			}
		}

		if (sizeChange)
		{
			sizeChange = 0;
			//glViewport(0, 0, width, height);
		}
		glClearColor(0.8, 0.6, 0.7, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glXSwapBuffers(display, window);
		
	}
	glXMakeCurrent(display, 0, 0);
	glXDestroyContext(display, context);
	XDestroyWindow(display, window);
	XFreeColormap(display, colormap);
	XCloseDisplay(display);
	gladLoaderUnloadGLX();
	return 0;
}