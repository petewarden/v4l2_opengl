#include "window_main.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "app_main.h"
#include "capture_main.h"
#include "trace.h"

Display *dpy;
Window root;
GLint att[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
XVisualInfo *vi;
Colormap cmap;
XSetWindowAttributes swa;
Window win;
GLXContext glc;
XWindowAttributes gwa;
XEvent xev;
unsigned int g_texture_id = 0;

#define CHECK_GL_ERRORS()                                                      \
    do                                                                         \
    {                                                                          \
        int err;                                                               \
        while ((err = glGetError()) != 0)                                      \
        {                                                                      \
            fprintf(stderr, "%s:%d OpenGL error %d: %s\n", __FILE__, __LINE__, \
                    err, gluErrorString(err));                                 \
        }                                                                      \
    } while (false)

static void DrawAQuad()
{
    int width;
    int height;
    uint8_t *texture_data;
    if (!get_latest_capture(&width, &height, &texture_data))
    {
        // Camera capture is not yet ready.
        return;
    }
    CHECK_GL_ERRORS();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, texture_data);
    CHECK_GL_ERRORS();
    free(texture_data);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    CHECK_GL_ERRORS();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1., 1., -1., 1., 1., 20.);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);
    CHECK_GL_ERRORS();

    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glEnd();
    CHECK_GL_ERRORS();
}

void *window_main(void *cookie)
{
    Args *args = (Args *)(cookie);
    int argc = args->argc;
    char **argv = args->argv;

    dpy = XOpenDisplay(NULL);

    if (dpy == NULL)
    {
        printf("\n\tcannot connect to X server\n\n");
        exit(0);
    }

    root = DefaultRootWindow(dpy);

    vi = glXChooseVisual(dpy, 0, att);

    if (vi == NULL)
    {
        printf("\n\tno appropriate visual found\n\n");
        exit(0);
    }
    else
    {
        printf("\n\tvisual %p selected\n", (void *)vi->visualid); /* %p creates hexadecimal output like in glxinfo */
    }

    cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask;

    win = XCreateWindow(dpy, root, 0, 0, 640, 480, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

    XMapWindow(dpy, win);
    XStoreName(dpy, win, "V4L2 OpenGL Example");

    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);

    glEnable(GL_DEPTH_TEST);

    glGenTextures(1, &g_texture_id);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    while (1)
    {
        XGetWindowAttributes(dpy, win, &gwa);
        glViewport(0, 0, gwa.width, gwa.height);
        DrawAQuad();
        glXSwapBuffers(dpy, win);
    }
}