// - rlyeh, public domain

void tigrMouseCursor(Tigr *app, int mode) { // 0(arrow),1(hand),2(ibeam),3(cross)
    static HCURSOR cursor[4];
    static int once = 1; for(;once;once=0) {
        cursor[0] = (HCURSOR)LoadCursorA(NULL, IDC_ARROW);
        cursor[1] = (HCURSOR)LoadCursorA(NULL, IDC_HAND);
        cursor[2] = (HCURSOR)LoadCursorA(NULL, IDC_IBEAM);
        cursor[3] = (HCURSOR)LoadCursorA(NULL, IDC_CROSS);
    }
    SetClassLongPtr((HWND)app->handle, GCLP_HCURSOR, (LONG_PTR)cursor[mode & 3]);
}
