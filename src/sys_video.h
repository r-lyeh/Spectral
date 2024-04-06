#define window Tigr
#define window_open(w,h,title) tigrWindow(w, h, title, 0)
#define window_update(win) (tigrUpdate(win))
#define window_alive(win) (!tigrClosed(win))
#define window_close(win) (win = (win ? tigrFree(win), NULL : NULL))
#define window_print(win, text) window_printxy(win, text, 1, 1)
#define window_printxy(win, text, x,y) window_printxycol(win, text, x,y, tigrRGB(0xff, 0xff, 0xff))
#define window_printxycol(win, text, x,y, col) tigrPrint(win, tfont, (x)*11,(y)*11, col, "%s", text)
#define window_pressed(win, keycode) (!!(tigrKeyDown(win, keycode) || tigrKeyHeld(win, keycode)))
#define window_trigger(win, keycode) (!!tigrKeyDown(win, keycode))
#define window_title(win, text) SetWindowTextA((HWND)((win)->handle), text)
#define rgb(r,g,b) (((r)<<0)|((g)<<8)|((b)<<16)|255<<24)
#define rgb_split(p,r,g,b) (b=(p>>16)&255,g=(p>>8)&255,r=(p)&255)
byte hh,ss,vv;
#define as_rgb(h,s,v) (hsv2rgb(h,s,v,&hh,&ss,&vv), rgb(hh,ss,vv))

unsigned blend_rgb(unsigned px1, unsigned px2) {
    unsigned r,g,b;
    unsigned x,y,z;

    rgb_split(px1,r,g,b);
    rgb_split(px2,x,y,z);

    r>>=1,g>>=1,b>>=1;
    x>>=1,y>>=1,z>>=1;

    return rgb(r+x,g+y,b+z);
}


#define min_f(a, b, c)  (fminf(a, fminf(b, c)))
#define max_f(a, b, c)  (fmaxf(a, fmaxf(b, c)))
void rgb2hsv(byte r_, byte g_, byte b_, byte *h_, byte *s_, byte *v_) {
    float r = r_ / 255.0f;
    float g = g_ / 255.0f;
    float b = b_ / 255.0f;
    float h, s, v; // h:0-360.0, s:0.0-1.0, v:0.0-1.0
    float max = max_f(r, g, b);
    float min = min_f(r, g, b);

    v = max;

    if (max == 0.0f) {
        s = 0;
        h = 0;
    }
    else if (max - min == 0.0f) {
        s = 0;
        h = 0;
    }
    else {
        s = (max - min) / max;

        if (max == r) {
            h = 60 * ((g - b) / (max - min)) + 0;
        }
        else if (max == g) {
            h = 60 * ((b - r) / (max - min)) + 120;
        }
        else {
            h = 60 * ((r - g) / (max - min)) + 240;
        }
    }

    if (h < 0) h += 360.0f;

    *h_ = (byte)(h / 2);   // h : 0-180
    *s_ = (byte)(s * 255); // s : 0-255
    *v_ = (byte)(v * 255); // v : 0-255
}

void hsv2rgb(byte h_, byte s_, byte v_, byte *r_, byte *g_, byte *b_) {
    float h = h_ *   2.0f; // 0-360
    float s = s_ / 255.0f; // 0.0-1.0
    float v = v_ / 255.0f; // 0.0-1.0
    float r, g, b; // 0.0-1.0
    int   hi = (int)(h / 60.0f) % 6;
    float f  = (h / 60.0f) - hi;
    float p  = v * (1.0f - s);
    float q  = v * (1.0f - s * f);
    float t  = v * (1.0f - s * (1.0f - f));

    /**/ if(hi == 0) r = v, g = t, b = p;
    else if(hi == 1) r = q, g = v, b = p;
    else if(hi == 2) r = p, g = v, b = t;
    else if(hi == 3) r = p, g = q, b = v;
    else if(hi == 4) r = t, g = p, b = v;
    else if(hi == 5) r = v, g = p, b = q;

    *r_ = (unsigned char)(r * 255); // r : 0-255
    *g_ = (unsigned char)(g * 255); // g : 0-255
    *b_ = (unsigned char)(b * 255); // b : 0-255
}
