// mouse
struct mouse {
    int x, y, lb, mb, rb, buttons, cursor; // buttons: R2M1L0 bits
};
struct mouse mouse() {
    extern Tigr *app;
    int mx, my, mb, lmb, mmb, rmb;
    tigrMouse(app, &mx, &my, &mb);

#ifdef _WIN32
    lmb = mb & 1; mmb = !!(mb & 2); rmb = !!(mb & 4);
#else
    lmb = mb & 1; rmb = !!(mb & 2); mmb = !!(mb & 4);
#endif

#if 1 // adjust mouse coords when shader for CRT distortion is applied. move to sys_input.h
    extern int ZX_CRT;
    if( ZX_CRT ) {
        const float CURVATURE = 8.2f; // must match value in fxShader
        float u = (mx * 1.f / _320) - 0.5f; // [-1..1]
        float v = (my * 1.f / _240) - 0.5f; // [-1..1]
        float offx = v / CURVATURE; // v is intentional
        float offy = u / CURVATURE; // u is intentional
        u += (u * offx * offx); 
        u *= 2; //u = u * 0.5 + 0.5;
        v += (v * offy * offy); 
        v *= 2; //v = v * 0.5 + 0.5;

        u = (u+1) * 0.5f;
        v = (v+1) * 0.5f;

        mx = _320 * u;
        my = _240 * v;

        // debug
        // extern window* ui;
        // tigrFill(ui, ui_mx - 4, ui_my - 4, 4*2, 4*2, ui_00);
    }
#endif

    return ( (struct mouse) {mx, my, lmb, mmb, rmb, mb, tigrGetMouseCursor(app) } );
}
void mouse_cursor(int mode) { // 0(hide),1(arrow),2(hand),3(ibeam),4(cross)
    extern window *app;
    tigrSetMouseCursor(app, mode);
}
void mouse_clip(int clip) {
#ifdef _WIN32
    static RECT restore; do_once GetClipCursor(&restore);

    extern Tigr *app;
    HWND hWnd = (HWND)app->handle;

    extern int active; // ui

    // get client area (0,0,w,h)
    RECT dims;
    GetClientRect(hWnd, &dims);

    // convert area to desktop coordinates
    RECT win = dims;
    ClientToScreen(hWnd, (POINT*)&win.left); // convert top-left
    ClientToScreen(hWnd, (POINT*)&win.right); // convert bottom-right

    // wrap mouse over the edges
    if( clip && !active ) {
        int w = dims.right, h = dims.bottom;
        int x = win.left, y = win.top;

        // mouse coords relative to top-left (0,0) client area
        struct mouse m = mouse();
        // convert to desktop coords
        int mx = win.left + m.x;
        int my = win.top + m.y;

        // difference between Tigr's and Windows' mouse desktop coords
        POINT p;
        GetCursorPos(&p);
        int diffx = mx - p.x, diffy = my - p.y;

        if(mx<x) while(mx<x) mx += w;
        else while(mx>(x+w/2)) mx -= w;

        if(my<y) while(my<y) my += h;
        else while(my>(y+h/2)) my -= h;

        SetCursorPos(mx - diffx,my - diffy);
    }

    // Clip or restore the cursor to its previous area.
    // ClipCursor(clip && !active ? &win : &restore);

    // Hide or show cursor
    // mouse_cursor(clip && !active ? 0 : 1);
#endif
}

