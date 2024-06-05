// drag 'n drop support for tigr (win,lin,osx)
// - rlyeh, public domain

// header:
// returns NULL-terminated list of utf8 string items dropped in this frame.
// optional positions for (x,y) drop can be retrieved too.

char **tigrDropFiles(Tigr *app, int *x, int *y);

// implementation

#pragma once
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

// @fixme: these vars belong to TigrInternal struct
static char *m_droppedItem;      // "abc\ndef\n"
static int   m_droppedCount;     // 2
static int   m_droppedCoord[2];  // X:123,Y:456
static char**m_droppedList;      // {"abc","def",NULL}

static void tigrDragAcceptFiles(Tigr *, int);

char **tigrDropFiles(Tigr *app, int *x, int *y) {
    // drag'n'drop, enabled on-the-fly
    // until this call, the window has never been accepting dropped files before
    tigrDragAcceptFiles(app, 1);

    // prepare null-terminated list[] of strings
    if( m_droppedCount ) {
        // reallocate list. make room for terminator too
        m_droppedList = realloc(m_droppedList, sizeof(char*) * (m_droppedCount+1) );

        // populate list
        for( int i = 0, j = 0; m_droppedItem && m_droppedItem[i]; ++i) {
            // store pointers
            if( i == 0 || m_droppedItem[i+1] == '\n' )
                m_droppedList[j] = m_droppedItem + i + 2 * !!j, ++j;
            // convert linefeeds into null terminators
            m_droppedItem[i] *= m_droppedItem[i] != '\n';
        }

        // convert file://uris into plain uris
        for( int i = 0; i < m_droppedCount; ++i ) {
            m_droppedList[i] += 7 * !strncmp(m_droppedList[i], "file://", 7);
        }

        // terminate list
        m_droppedList[m_droppedCount] = NULL;
        m_droppedCount = 0;

        // return results
        if(x) *x = m_droppedCoord[0];
        if(y) *y = m_droppedCoord[1];
        return m_droppedList;
    }

    static char* empty[1] = {0};
    return empty[0] = NULL, empty;
}


#ifdef __APPLE__
#import <Cocoa/Cocoa.h>
#include <objc/runtime.h>
#include <objc/message.h>

#define objc_msgSend_id ((id (*)(id, SEL))objc_msgSend)

NSUInteger draggingEntered(id self, SEL sel, id sender) {
    return NSDragOperationCopy; // Generic; // Copy;
}
BOOL performDragOperation(id self, SEL sel, id sender) {
    NSWindow* window = objc_msgSend_id(sender, sel_registerName("draggingDestinationWindow")); // [sender draggingDestinationWindow]
    NSPasteboard* pasteboard = ((NSPasteboard*) objc_msgSend_id(sender, sel_registerName("draggingPasteboard"))); // [sender draggingPasteboard];

    NSDictionary* options = @{NSPasteboardURLReadingFileURLsOnlyKey:@YES};
    NSArray* objs = [pasteboard readObjectsForClasses:@[[NSURL class], [NSString class]]options:options];
    if (objs) {
        const NSPoint point = [sender draggingLocation];
        m_droppedCoord[0] = point.x;
        m_droppedCoord[1] = point.y;
        m_droppedCount = [objs count];

        NSMutableString *list = [NSMutableString stringWithCapacity:4096]; // auto-released
        for( NSUInteger i = 0; i < m_droppedCount; ++i ) {
            id obj = objs[i];
            /**/ if ([obj isKindOfClass:[NSString class]]) {
                const char *str = [obj UTF8String];
                [list appendString:@(str)];
                [list appendString:@("\n")];
            }
            else if ([obj isKindOfClass:[NSURL class]]) {
                NSURL *url = (NSURL*)obj;
                if (url.fileURL) [list appendString:url.filePathURL.absoluteString];
                else [list appendString:url.absoluteString];
                [list appendString:@("\n")];
            }
        }

        int len = [list length] > 0 ? strlen(list.UTF8String) : 0;
        m_droppedItem = realloc(m_droppedItem, len + 1);
        len[(char*)memcpy(m_droppedItem, len ? list.UTF8String : "", len)] = '\0';

        return YES;
    }
    return NO;
}
static
void tigrDragAcceptFiles(Tigr *app, int on) {
    static NSWindow *obj1 = 0;
    if(!obj1) {
        obj1 = (NSWindow*)tigrInternal(app)->window;
        [obj1 registerForDraggedTypes:@[NSPasteboardTypeURL, NSPasteboardTypeFileURL, NSPasteboardTypeString]];
    }
}

#define TIGR_HANDLE_DRAG_N_DROP(WindowDelegateClass) do { \
    class_addMethod(WindowDelegateClass, sel_registerName("draggingEntered:"), (IMP)draggingEntered,  "v@:@"); \
    class_addMethod(WindowDelegateClass, sel_registerName("performDragOperation:"), (IMP)performDragOperation,  "v@:@"); \
} while(0)

#undef objc_msgSend_id

#elif defined _WIN32
#include <shellapi.h>

static
void tigrDragAcceptFiles(Tigr *app, int on) {
    DragAcceptFiles((HWND)app->handle, on ? TRUE : FALSE);
}

static
int tigrWriteUTF8FromWideString(LPWSTR instr, char *outstr) { // returns written bytes, \0 included
    int len = WideCharToMultiByte(CP_UTF8, 0, instr, -1, 0, 0, NULL, NULL);
    WideCharToMultiByte(CP_UTF8, 0, instr, -1, outstr, len, NULL, NULL);
    return len;
}

static
void tigrHandleDragNDropEvent(HWND hWnd, HDROP drop) {
    // update coords
    POINT point;
    DragQueryPoint(drop, &point);
    m_droppedCoord[0] = point.x;
    m_droppedCoord[1] = point.y;

    // count number of dropped files
    int count = DragQueryFileW(drop, 0xffffffff, NULL, MAX_PATH);

    // allocate mem, worst case
    int bytes = 0;
    for (int i = 0; i < count; i++) {
        UINT length = DragQueryFileW(drop, i, NULL, MAX_PATH);
        bytes += (length + 1) * 6;
    }
    m_droppedItem = realloc(m_droppedItem, bytes);

    // concatenate utf8 strings, \n separated
    char *ptr = m_droppedItem;
    for (int i = 0; i < count; i++) {
        WCHAR buffer[ MAX_PATH + 1 ];
        UINT length = DragQueryFileW(drop, i, NULL, MAX_PATH);
        DragQueryFileW(drop, i, buffer, MAX_PATH);

        ptr += tigrWriteUTF8FromWideString(buffer, ptr);
        memcpy(ptr - 1, "\n\0", 2);
    }

    // signal event
    DragFinish(drop);
    
    m_droppedCount = count;
}

#define TIGR_HANDLE_DRAG_N_DROP(hWnd, wParam) \
    case WM_DROPFILES: return tigrHandleDragNDropEvent(hWnd, (HDROP)wParam), 0;

#else // unix

static Atom XdndAware;
static Atom XdndEnter;
static Atom XdndPosition;
static Atom XdndStatus;
static Atom XdndActionCopy;
static Atom XdndDrop;
static Atom XdndSelection;
#if 0
static Atom XdndFinished;
static Atom XdndLeave;
static Atom XdndTypeList;
static Atom text_uri_list;
#endif
static Atom text_plain;
static Atom XDND_DATA;
static Atom INCR;

void tigrDragAcceptFiles(Tigr *app, int on) {
    Window m_window = tigrInternal(app)->win;
    Display *m_display = tigrInternal(app)->dpy;

    // init atoms
    if( !XdndAware ) {
        XdndAware      = XInternAtom(m_display, "XdndAware", False);
        XdndEnter      = XInternAtom(m_display, "XdndEnter", False);
        XdndPosition   = XInternAtom(m_display, "XdndPosition", False);
        XdndStatus     = XInternAtom(m_display, "XdndStatus", False);
        XdndActionCopy = XInternAtom(m_display, "XdndActionCopy", False);
        XdndDrop       = XInternAtom(m_display, "XdndDrop", False);
        XdndSelection  = XInternAtom(m_display, "XdndSelection", False);
#if 0
        XdndFinished   = XInternAtom(m_display, "XdndFinished", False);
        XdndLeave      = XInternAtom(m_display, "XdndLeave", False);
        XdndTypeList   = XInternAtom(m_display, "XdndTypeList", False);
        text_uri_list  = XInternAtom(m_display, "text_uri_list", False);        
#endif
        text_plain     = XInternAtom(m_display, "text/plain", False);        
        XDND_DATA      = XInternAtom(m_display, "XDND_DATA", False);        
        INCR           = XInternAtom(m_display, "INCR", False);        
    }

    // add/remove XdndAware property accordingly
    Atom xdnd_version = 5;
    if( on ) XChangeProperty(m_display, m_window, XdndAware, ((Atom)4)/*XA_ATOM*/, 32, PropModeReplace, (unsigned char*)(&xdnd_version), 1);
    else XDeleteProperty(m_display, m_window, XdndAware);
}

int tigrHandleDropEvent(TigrInternal *app, XEvent e) {
    Window m_window = app->win;
    Display *m_display = app->dpy;
    static long int m_otherWindow = 0;

    // 1st event: enter drag. save source window
    if (e.type == ClientMessage && e.xclient.message_type == XdndEnter) {
        m_otherWindow = e.xclient.data.l[0];

        return 1;
    }

    // 2nd event: position coordinates. save them.
    if (e.type == ClientMessage && e.xclient.message_type == XdndPosition) {
        XEvent reply = {0};
        reply.xclient.type = ClientMessage;
        reply.xclient.display = m_display;
        reply.xclient.window = m_otherWindow;
        reply.xclient.message_type = XdndStatus;
        reply.xclient.format = 32;
        reply.xclient.data.l[0] = m_window; // current window
        reply.xclient.data.l[1] = 1;        // accept and want position flags
        reply.xclient.data.l[2] = 0; // coords
        reply.xclient.data.l[3] = 0; // width
        reply.xclient.data.l[4] = XdndActionCopy; // action we accept
        XSendEvent(m_display, m_otherWindow, false, 0, &reply);

        // save positions
        m_droppedCoord[0] = e.xclient.data.l[2] >> 16;
        m_droppedCoord[1] = e.xclient.data.l[2] & 0xFFFF;

        return 1;
    }

    // 3rd: drop event. request a 4th conversion event.
    if (e.type == ClientMessage && e.xclient.message_type == XdndDrop) {
        // request a SelectionNotify event. only text/plain drops supported
        XConvertSelection(m_display, XdndSelection, text_plain, XDND_DATA, m_window, (Time)e.xclient.data.l[2]);
        return 1;
    }

    // 4th: response from XConvertSelection request. save datas.
    if (e.type == SelectionNotify && e.xclient.message_type == XdndSelection) {
        Atom           type;
        int            format;
        unsigned long  bytes;
        unsigned long  remainingBytes;
        unsigned char* data = 0;

        // grab selection data put there by selection owner, that responded to our request
        int result = XGetWindowProperty(m_display, m_window, e.xselection.property, 0, 0x7fffffff, False, AnyPropertyType, &type, &format, &bytes, &remainingBytes, &data);

        // we dont support INCR
        if (result == Success && type != INCR ) {
            // count number of items: "abc" and "abc\ndef\n" form
            m_droppedCount = 0;
            for( int i = 0; data[i]; ++i ) m_droppedCount += data[i] == '\n';
            m_droppedCount += !m_droppedCount;
            // copy item. do not use strdup(). we only use realloc() in this source file
            strcpy(m_droppedItem = realloc(m_droppedItem, strlen(data) + 1), data);
        }

        // we must remove the request we made earlier
        if (result == Success) {
            XDeleteProperty(m_display, m_window, e.xselection.property);
        }

        if( data ) {
            XFree(data);
        }

        XFlush(m_display);

        return 1;
    }

    return 0;
}

#define TIGR_HANDLE_DRAG_N_DROP(app,ev) \
    tigrHandleDropEvent(app, ev)

#endif
