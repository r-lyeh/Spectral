// - rlyeh, public domain

#ifdef __APPLE__

static
int tigrMouseCursor_(Tigr *app, int mode) { // -1(getter),0(hide),1(arrow),2(hand),3(ibeam),4(cross)
    static int last = 0;  // @fixme: move inside `Tigr*`
    if( mode >= 0 ) {
        if( mode != last ) {
            mode %= 5;
#ifdef __OBJC__
            if( !!mode ^ !!last ) {
                if( mode ) objc_msgSend_id(class("NSCursor"), sel("unhide")); // [NSCursor unhide];
                else       objc_msgSend_id(class("NSCursor"), sel("hide")); // [NSCursor hide];
            }

            if( mode == 1 ) //[[NSCursor arrowCursor] set];
                objc_msgSend_void( objc_msgSend_id(class("NSCursor"), sel("arrowCursor")), sel("set"));

            if( mode == 2 ) //[[NSCursor pointingHandCursor] set];
                objc_msgSend_void( objc_msgSend_id(class("NSCursor"), sel("pointingHandCursor")), sel("set"));

            if( mode == 3 ) //[[NSCursor IBeamCursor] set];
                objc_msgSend_void( objc_msgSend_id(class("NSCursor"), sel("IBeamCursor")), sel("set"));

            if( mode == 4 ) //[[NSCursor crosshairCursor] set];
                objc_msgSend_void( objc_msgSend_id(class("NSCursor"), sel("crosshairCursor")), sel("set"));
#endif
            last = mode;
        }
    }
    return last;
}

#elif defined _WIN32

static
int tigrMouseCursor_(Tigr *app, int mode) { // -1(getter),0(hide),1(arrow),2(hand),3(ibeam),4(cross)
    static int last = 0;  // @fixme: move inside `Tigr*`
    static HCURSOR cursor[5]; // @fixme: move inside `Tigr*`
    static int once = 1; for(;once;once=0) {
        int w = GetSystemMetrics( SM_CXCURSOR );
        int h = GetSystemMetrics( SM_CYCURSOR );
        unsigned char *and_mask = _alloca( w * h ); memset( and_mask, 0xFF, w*h );
        unsigned char *xor_mask = _alloca( w * h ); memset( xor_mask, 0x00, w*h );
        cursor[0] = (HCURSOR)CreateCursor( 0,1,1,w,h, and_mask, xor_mask);
        cursor[1] = (HCURSOR)LoadCursorA(NULL, IDC_ARROW);
        cursor[2] = (HCURSOR)LoadCursorA(NULL, IDC_HAND);
        cursor[3] = (HCURSOR)LoadCursorA(NULL, IDC_IBEAM);
        cursor[4] = (HCURSOR)LoadCursorA(NULL, IDC_CROSS);
    }
    if( mode >= 0 ) {
        if( mode != last )
        SetClassLongPtr((HWND)app->handle, GCLP_HCURSOR, (LONG_PTR)cursor[last = mode % 5]);
    }
    return last;
}

#else

static
int tigrMouseCursor_(Tigr *app, int mode) { // -1(getter),0(hide),1(arrow),2(hand),3(ibeam),4(cross)
    static int last = 0;  // @fixme: move inside `Tigr*`
    static Cursor cursor[5] = {0};  // @fixme: move inside `Tigr*`
    static int once = 1; for(;once;once=0) {
        static char noData[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        XColor black; black.red = black.green = black.blue = 0;
        Pixmap bitmapNoData = XCreateBitmapFromData(tigrInternal(app)->dpy, tigrInternal(app)->win, noData, 8, 8);
        Cursor invisibleCursor = XCreatePixmapCursor(tigrInternal(app)->dpy, bitmapNoData, bitmapNoData, &black, &black, 0, 0);
        //XFreeCursor(tigrInternal(app)->dpy, invisibleCursor);
        //XFreePixmap(tigrInternal(app)->dpy, bitmapNoData);
        cursor[0] = invisibleCursor;
        cursor[1] = XCreateFontCursor(tigrInternal(app)->dpy, 68);  // XC_left_ptr;
        cursor[2] = XCreateFontCursor(tigrInternal(app)->dpy, 60);  // XC_hand2;
        cursor[3] = XCreateFontCursor(tigrInternal(app)->dpy, 152); // XC_xterm;
        cursor[4] = XCreateFontCursor(tigrInternal(app)->dpy, 34);  // XC_crosshair;
    }

    if( mode >= 0 ) {
        if( mode != last )
        XDefineCursor(tigrInternal(app)->dpy, tigrInternal(app)->win, cursor[last = mode % 5]);
    }
    return last;
}

#endif

unsigned tigrGetMouseCursor(Tigr *app) {
    return tigrMouseCursor_(app, -1);
}
void tigrSetMouseCursor(Tigr *app, unsigned mode) {
    tigrMouseCursor_(app, mode);
}
