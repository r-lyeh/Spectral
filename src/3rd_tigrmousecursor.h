// - rlyeh, public domain

static
int tigrMouseCursor_(Tigr *app, int mode) { // -1(getter),0(hide),1(arrow),2(hand),3(ibeam),4(cross)
    static HCURSOR cursor[5];
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
        SetClassLongPtr((HWND)app->handle, GCLP_HCURSOR, (LONG_PTR)cursor[mode % 5]);
    } else {
        HCURSOR c = (HCURSOR)GetClassLongPtr((HWND)app->handle, GCLP_HCURSOR);
        for(int i = 0; i < 5; ++i) if( c == cursor[i] ) return i;
    }
    return 0;
}

unsigned tigrGetMouseCursor(Tigr *app) {
    return tigrMouseCursor_(app, -1);
}
void tigrSetMouseCursor(Tigr *app, unsigned mode) {
    tigrMouseCursor_(app, mode);
}

