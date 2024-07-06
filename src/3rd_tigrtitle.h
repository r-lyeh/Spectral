char* tigrTitle(Tigr *win, const char *title) {
    static char copy[128] = {0};
    if( title ) {
#ifdef __APPLE__

#elif defined _WIN32
        SetWindowTextA((HWND)(win->handle), title);
#else
        XTextProperty prop;
        int result = Xutf8TextListToTextProperty(dpy, (char**)&title, 1, XUTF8StringStyle, &prop);
        if (result == Success) {
            Atom wmName = XInternAtom(dpy, "_NET_WM_NAME", 0);
            XSetTextProperty(tigrInternal(win)->dpy, tigrInternal(win)->win, &prop, wmName);
            XFree(prop.value);
        }
#endif
        snprintf(copy, 128, "%s", title);
    }
    return copy;
}
