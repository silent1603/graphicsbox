#include <X11/Xlib.h>

int main(int argc, char** argv)
{
    Display* dpy = XOpenDisplay(NULL);
    Window w = XCreateWindow(dpy,DefaultRootWindow(dpy),0,0,200,100,0,CopyFromParent,CopyFromParent,CopyFromParent,NULL,0);
    XMapWindow(dpy,w);
    return 0;
}