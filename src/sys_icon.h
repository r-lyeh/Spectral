static void warning(const char *msg) {
    MessageBoxA(0,msg,"Warning",MB_OK);    
}

// Set the window icon for every window in your app (including MessageBox() calls and assertion failures) instead of just your primary window.
static LRESULT window_create_callback(int type, WPARAM wparam, LPARAM lparam) {
    if (type == HCBT_CREATEWND) {
        static HICON hIcon; // = (HICON)GetClassLong(hWnd, GCL_HICON);
        do_once {
        HANDLE hInstance = GetModuleHandleA(NULL);
        hIcon = ExtractIconA(hInstance, __argv[0], 0 );
        if(!hIcon) hIcon = ExtractIconA(hInstance, va("%s.exe", __argv[0]), 0 );
    //  if(!hIcon) hIcon = LoadImageA(NULL, "noto_1f47b.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE|LR_LOADFROMFILE|LR_LOADTRANSPARENT);        
        }
        SendMessage((HWND)wparam, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        SendMessage((HWND)wparam, WM_SETICON, ICON_BIG, (LPARAM)hIcon);    
    }
    return CallNextHookEx(NULL, type, wparam, lparam);
}
static void window_override_icons() {
    SetWindowsHookEx(WH_CBT, window_create_callback, NULL, GetCurrentThreadId());
}
