// drag 'n drop support for tigr (win)
// - rlyeh, public domain

// header:
// returns null-terminated list of utf8 strings. pointers valid till next drop.
// returns NULL if no files were dropped.

char **tigrDropFiles(Tigr *app); 

// implementation

static
char *tigrCreateUTF8FromWideString(LPWSTR instr) {
    int len = WideCharToMultiByte(CP_UTF8, 0, instr, -1, 0, 0, NULL, NULL);
    char *outstr = (char*)malloc(len);
    WideCharToMultiByte(CP_UTF8, 0, instr, -1, outstr, len, NULL, NULL);
    return outstr;
}

// @fixme: these two should be within a Tigr or TigrInternal struct
static char** tigrDropPaths;
static int tigrDropPathsQueueForDeletion;

static
void tigrDropFree(void) {
    // release all taken memory
    if( tigrDropPaths ) {
        for(int i = 0;tigrDropPaths[i];) free(tigrDropPaths[i++]);
        free(tigrDropPaths);
        tigrDropPaths = 0;
    }
}

static
void tigrDropEvent(HWND hWnd, HDROP drop) {
    // count number of dropped files
    int count = DragQueryFileW(drop, 0xffffffff, NULL, MAX_PATH);

    // prepare mem. invalidate old results if needed
    tigrDropFree();
    tigrDropPaths = (char**)calloc(count + 1, sizeof(char*));

    // convert dropped paths into utf8
    for (int i = 0; i < count; i++) {
        const UINT length = DragQueryFileW(drop, i, NULL, MAX_PATH);
        WCHAR buffer[ MAX_PATH + 1 ];

        DragQueryFileW(drop, i, buffer, MAX_PATH);
        tigrDropPaths[i] = tigrCreateUTF8FromWideString(buffer);
    }

    // signal event
    DragFinish(drop);
}

char **tigrDropFiles(Tigr *app) {
    // enable drag'n'drop on-the-fly
    // until this call, the window was never accepting dropped files before
    DragAcceptFiles((HWND)app->handle, TRUE);

    // clear results from previous drop, if needed
    if( tigrDropPathsQueueForDeletion ) {
        tigrDropPathsQueueForDeletion = 0;
        tigrDropFree();
    }
    // queue results for deletion during next call
    if( tigrDropPaths ) {
        tigrDropPathsQueueForDeletion = 1;
    }

    return tigrDropPaths;
}

#define TIGR_DROP_MESSAGE(hWnd, wParam) \
    case WM_DROPFILES: return tigrDropEvent(hWnd, (HDROP)wParam), 0;
