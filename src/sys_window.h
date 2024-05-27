#define window Tigr
#define window_open(w,h,title) tigrWindow(w, h, title, TIGR_2X)
#define window_bitmap(w,h) tigrBitmap(w,h)
#define window_update(win) (tigrUpdate(win))
#define window_alive(win) (!tigrClosed(win))
#define window_close(win) (win = (win ? tigrFree(win), NULL : NULL))
#define window_print(win, text) window_printxy(win, text, 1, 1)
#define window_printxy(win, text, x,y) window_printxycol(win, text, x,y, tigrRGB(0xff, 0xff, 0xff))
#define window_printxycol(win, text, x,y, col) tigrPrint(win, tfont, (x)*11,(y)*11, col, "%s", text)
#define window_pressed(win, keycode) (!!(tigrKeyDown(win, keycode) || tigrKeyHeld(win, keycode)))
#define window_trigger(win, keycode) (!!tigrKeyDown(win, keycode))
#define window_title(win, text) SetWindowTextA((HWND)((win)->handle), text)

// Set the window icon for every window in your app (including MessageBox() calls and assertion failures) instead of just your primary window.
static HICON appIcon; // = (HICON)GetClassLong(hWnd, GCL_HICON);
static LRESULT window_create_callback(int type, WPARAM wparam, LPARAM lparam) {
    if (type == HCBT_CREATEWND) {
        SendMessage((HWND)wparam, WM_SETICON, ICON_SMALL, (LPARAM)appIcon);
        SendMessage((HWND)wparam, WM_SETICON, ICON_BIG, (LPARAM)appIcon);
    }
    return CallNextHookEx(NULL, type, wparam, lparam);
}
void window_override_icons() {
    do_once {
        HANDLE hInstance = GetModuleHandleA(NULL);
        appIcon = ExtractIconA(hInstance, __argv[0], 0 );
        if(!appIcon) appIcon = ExtractIconA(hInstance, va("%s.exe", __argv[0]), 0 );
    }
    SetWindowsHookEx(WH_CBT, window_create_callback, NULL, GetCurrentThreadId());
}

#define warning(msg) warning("Warning", msg)
int (warning)(const char *title, const char *msg) {
    return MessageBoxA(0,msg,title,MB_OK), 1;
}

void die(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    warning(msg);
    if(IsDebuggerPresent()) DebugBreak();
    exit(-1);
}

char* prompt( void *hwndParentWindow, const char *title, const char *caption, const char *defaults );
