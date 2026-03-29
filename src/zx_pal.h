// contention: http://www.zxdesign.info/memContRevision.shtml
// 171,0,77 <3
// 146,89,40 <3

// @todo: test dark level at 85% voltage. this means (100%) 0xFF / 0xD8 (85%) tuples. black as 0x00

/* @todo: 16-bit pal
rgb(   6,   6,   6),rgb( 105,   2, 174),rgb( 134,   2,  98),rgb( 168,   0,   4),rgb( 143,  33,   1),rgb( 209, 105,   1),rgb( 244, 196,   0),rgb( 220, 213, 171),
rgb(   6,   6,   6),rgb(  16,  37,  88),rgb(   1, 109, 228),rgb(   0, 168, 176),rgb(  71, 254, 145),rgb(  84, 253,   2),rgb( 190, 252,   3),rgb( 244, 255, 223),

sorted as:
rgb(   6,   6,   6),rgb(  34,  24,  80),rgb( 128,  32,   0),rgb( 137,  12, 137),rgb(  22, 141,  52),rgb(   1, 109, 228),rgb( 209, 105,   1),rgb( 216, 192, 158),
rgb(   6,   6,   6),rgb(   9,  51, 121),rgb( 183,   0,   4),rgb( 192,   3,  84),rgb( 153, 204,   0),rgb(   0, 216, 128),rgb( 244, 196,   0),rgb( 241, 255, 217),
*/

#define luma(r,g,b) ((byte)((r)*0.299+(g)*0.587+(b)*0.114))
#define gray(r,g,b) rgb(luma(r,g,b),luma(r,g,b),luma(r,g,b))
#define gray3(l)    rgb(l,l,l)
#define hex(rrggbb) rgb((0x##rrggbb)>>24,(0x##rrggbb&0xff00)>>16,(0x##rrggbb&0xff))

// first 2-digit number configures ZX_BLOOM
const char *ZXPaletteNames[] = {

    /*#00*/ "00" "Spectral",
    /*#01*/ "00" "Spectral+\n",

    /*#02*/ "00" "Lumière",
    /*#03*/ "00" "Modern",
    /*#04*/ "00" "Reborn",
    /*#05*/ "00" "Remix",
    /*#06*/ "00" "Winners\n",

    /*#07*/ "00" "Merlot",
    /*#08*/ "00" "Fantasy",
    /*#09*/ "00" "Hue",
    /*#10*/ "00" "Hue+",
    /*#11*/ "00" "Dream",
    /*#12*/ "00" "Skin\n",

    /*#13*/ "00" "Quirkafleeg",
    /*#14*/ "00" /*"Quirkafleeg "*/ "II",

    /*#15*/"00" "PurpleRain",
    /*#16*/"00" "Angrite",
    /*#17*/"00" "Angrite2\n",

    /*#18*/ "00" "Origins",

    /*#19*/ "00" "Pico8",
    /*#20*/ "00" "Petit",
    /*#21*/ "00" "Bringer",
    /*#22*/"00" "Endesga\n",

    /*#23*/ "00" "Gradients", // best with ZX_BLOOM=40 ?
    /*#24*/ "00" "Vivid",

    /*#25*/"00" "Spectacle",

    /*#26*/ "00" "Konni\n",

    /*#27*/ "00" "Atkinson", // probably best with ZX_BLOOM=20
    /*#28*/ "00" "Sintez2\n",

    /*#29*/ "40" "CPC",
    /*#30*/ "00" "CGA",
    /*#31*/ "00" "EGA",
    /*#32*/ "00" "MSX2",
//"00" "PC6001",
    /*#33*/ "00" "Gameboy",
    /*#34*/ "90" "PCW", // best with ZX_BLOOM=90
    /*#35*/ "90" "Amber\n", // best with ZX_BLOOM=90

    /*#36*/ "00" "Noir",
    /*#37*/ "00" "Telly",

    /*#38*/ "00" "Ilford",
    /*#39*/"00" "B&W",

    /*#40*/ "00" "Greylit",
    /*#41*/ "20" "Negative\n", // best with ZX_BLOOM=20
/*#42*/ "00" "Extended", // must be last entry minus 1
    /*#43*/ "00" "External", // must be last entry
};

// default palettes used elsewhere
enum { ZX_PALETTE_PLAYER = 23 }; // gradients now; vivid before
enum { ZX_PALETTE_GRAY = 36 }; // noir
enum { ZX_PALETTE_EXTERNAL = countof(ZXPaletteNames)-1, ZX_PALETTE_EXTENDED = ZX_PALETTE_EXTERNAL-1 };

rgba ZXPalettes[][64] = {

    // two sections each, 16 regular colors in total. 64 entries for ulaplus, though
    // normal: black,blue,red,pink,green,cyan,yellow,white
    // bright: black,blue,red,pink,green,cyan,yellow,white

    {
        // spectral palette. note: no pure black
        rgb(0x12,0x10,0x12),rgb(0x00,0x00,0xAC),rgb(0xAC,0x00,0x00),rgb(0xAC,0x00,0xAC),
        rgb(0x00,0xAC,0x00),rgb(0x00,0xAC,0xAC),rgb(0xAC,0xAC,0x00),rgb(0xAC,0xAC,0xAC),
        rgb(0x12,0x10,0x12),rgb(0x00,0x00,0xFF),rgb(0xFF,0x00,0x00),rgb(0xFF,0x00,0xFF),
        rgb(0x00,0xFF,0x00),rgb(0x00,0xFF,0xFF),rgb(0xFF,0xFF,0x00),rgb(0xFF,0xFF,0xFF),
    },
    {
        // Spectral+
//        rgb(   0,   0,   0),rgb(  30,   0, 160),rgb( 128,   0,   0),rgb( 157,   5, 207),rgb(  50, 105,   0),rgb(   0, 129, 221),rgb( 220, 170,   0),rgb( 185, 167, 167),
//        rgb(   0,   0,   0),rgb(   0,   0, 220),rgb( 200,  33,   1),rgb( 234,   0, 170),rgb( 138, 209,   0),rgb(  54, 222, 245),rgb( 255, 220,   0),rgb( 255, 255, 255),

//        rgb(   0,   0,   0),rgb(  30,   0, 160),rgb( 128,   0,   0),rgb( 157,   5, 207),rgb(   0, 138,   0),rgb(   0, 129, 221),rgb( 220, 170,   0),rgb( 185, 167, 167),
//        rgb(   0,   0,   0),rgb(   0,   0, 220),rgb( 200,  33,   1),rgb( 234,   0, 170),rgb(   0, 215,   0),rgb(  54, 222, 245),rgb( 255, 220,   0),rgb( 255, 255, 255),

        rgb(  11,  11,  11),rgb(   0,   0, 140),rgb( 111,   0,   0),rgb( 157,   5, 207),rgb(   0, 138,   0),rgb(   0, 129, 221),rgb( 220, 170,   0),rgb( 207, 197, 186), //( 185, 167, 167),
        rgb(  11,  11,  11),rgb(   0,   0, 220),rgb( 200,  33,   1),rgb( 234,   0, 170),rgb(   0, 215,   0),rgb(  54, 222, 245),rgb( 255, 220,   0),rgb( 255, 255, 255),
    },

    {
        // "Lumière", inspired by AutoChrome plates from 1907 (orange,emerald,violet)
        rgb(   4,  12,  24),rgb(  64,  12, 184),rgb( 211,  61,   1),rgb( 208,   0,  79),rgb(  22, 165,  40),rgb(   0, 191, 153),rgb( 255, 180,  21),rgb( 192, 192, 192),
        rgb(   4,  12,  24),rgb(  83,  16, 239),rgb( 255,  71,   0),rgb( 255,   0,  96),rgb(  11, 227,  38),rgb(  26, 255, 164),rgb( 255, 227,   0),rgb( 255, 255, 175),

        // fantasy15, v1
//        rgb(0x04,0x0c,0x18),rgb(0x00,0x00,0xc0),rgb(0xc0,0x00,0x00),rgb(0x9d,0x00,0xd9),rgb(0x00,0xc0,0x00),rgb(0x00,0xc0,0xc0),rgb(0xc8,0xc8,0x00),rgb(0xc0,0xc0,0xc0),
//        rgb(0x04,0x0c,0x18),rgb(0x00,0x40,0xff),rgb(0xea,0x06,0x40),rgb(0xff,0x40,0xff),rgb(0x40,0xff,0x00),rgb(0x04,0xff,0xa2),rgb(0xff,0xd8,0x00),rgb(0xff,0xff,0xff),
    },
    {
        // modern, experiment with modern blue-ish 0, reddish 7, also yellowish 15 among other modifs
        //rgb( 37, 44, 56),rgb(  0, 82,149),rgb(170,  0, 30),rgb(165,  1, 96),
        //rgb(  0,160,  0),rgb(  0,171,221),rgb(255,212,  0),rgb(214,224,242),
        //rgb( 37, 44, 56),rgb(  0, 51,255),rgb(250, 16,  0),rgb(255, 21,109),
        //rgb(  0,227,  0),rgb(  0,210,229),rgb(255,255,  0),rgb(240,240,255),

        //rgb(  0,  0, 24),rgb( 49, 15,183),rgb(185,  0, 32),rgb(182,  1,105),
        //rgb(  0,160, 10),rgb(  0,171,221),rgb(255,212,  0),rgb(192,176,160), //rgb(191,206,234),
        //rgb(  0,  0, 24),rgb(  0,  0,255),rgb(251,  0,  6),rgb(255, 21,109),
        //rgb(  0,227,  0),rgb(  0,255,240),rgb(255,255,  0),rgb(255,255,230),

        //rgb(  0,  0, 24),rgb( 49, 15,183),rgb(185,  0, 32),rgb(182,  1,105),
        //rgb(  0,160, 10),rgb(  0,171,221),rgb(234,182,  0),rgb(204,191,179), //rgb(191,206,234),
        //rgb(  0,  0, 24),rgb(  0,  0,255),rgb(251,  0,  6),rgb(255, 21,109),
        //rgb(  0,235,  0),rgb(  0,255,240),rgb(255,240,  0),rgb(255,255,230),

        //rgb(  0,  0, 24),rgb(  0,  0,151),rgb(185,  0, 32),rgb(182,  1,105),
        //rgb(  0,160, 10),rgb(  0,171,221),rgb(232,191,  0),rgb(204,191,179), //rgb(191,206,234),
        //rgb(  0,  0, 24),rgb(  0,  0,220),rgb(251,  0,  6),rgb(255, 21,109),
        //rgb(  0,235,  0),rgb(  0,255,240),rgb(242,255,  0),rgb(255,255,230),

        rgb(  0,  0, 24),rgb(  0,  0,144),rgb(185,  0, 32),rgb(182,  1,105),
        rgb(  0,160, 10),rgb(  0,171,221),rgb(221,183,  0),rgb(196,189,170),//200,192,174), //204,191,179), //rgb(191,206,234),
        rgb(  0,  0, 24),rgb(  0,  0,240),rgb(251,  0,  6),rgb(255, 21,109),
        rgb(  0,235,  0),rgb(  0,255,240),rgb(255,242,  0),rgb(255,255,230),
    },
    {
        // reborn
//      rgb(  0,  0, 24),rgb(  0, 30,118),rgb(168,  2, 48),rgb(150,  1,150),
//      rgb( 86,149,  0),rgb(  0,166, 96),rgb(250,169,  1),rgb(169,169,169), //201,202,147), //206,187,179),
//      rgb(  0,  0, 24),rgb( 15,  0,198),rgb(249,  0, 33),rgb(204,  0,204),
//      rgb(149,230,  0),rgb(  0,236,136),rgb(255,240,  1),rgb(220,220,255), //240,238,174), //255,240,220),

        rgb(  0,  0, 24),rgb(  0, 30,118),rgb(174,  0,  4),rgb(155,  0,128),rgb( 13,136, 29),rgb(  0,152,196),rgb(250,169,  1),rgb(169,169,169),
        rgb(  0,  0, 24),rgb( 15,  0,198),rgb(231,  1, 30),rgb(243, 24,123),rgb( 84,223,  0),rgb(  0,200,255),rgb(243,232,  1),rgb(230,230,255),
    },
    {   // remix, v1
        // based on fantasy15+dream15+hue15+skin5+sintez2
        rgb(  0,  0,  0),rgb(  0, 33,133),rgb(191, 34,  0),rgb(162,  0,166),
        rgb(  0,160,  0),rgb( 10,143,247),rgb(255,160,  0),rgb(200,190,180), // 0,180,230  0,159,193 // 0,118,215
        rgb(  0,  0,  0),rgb(  0, 30,221),rgb(250, 16,  0),rgb(220,  0, 84),
        rgb(  0,227,  0),rgb(  0,242,175),rgb(255,220,  0),rgb(240,240,255),
    },
    {
        // "Winners"
        /*
        0 Fantasy
        1 Modern
        2 Hue
        3 Alt
        4 Reborn
        5 Konni
        6 modern dark+fantasy high
        7 spectral
        */
        rgb(   4,  12,  24),rgb(   0,   0, 151),rgb( 150,  24,  0),rgb( 148,  16, 175),rgb(  13, 136,  29),rgb(   0, 176, 160),rgb( 221, 183,   0),rgb( 172, 172, 172),
        rgb(   4,  12,  24),rgb(   0,   0, 220),rgb( 204,   0,  0),rgb( 209,  14, 155),rgb(  84, 223,   0),rgb(   0, 236, 220),rgb( 255, 230,   0),rgb( 255, 255, 255),
    },
    {
        // Merlot
        rgb(  0,  0, 24),rgb(  0, 30,118),rgb(140,  0, 63),rgb(200,  0,  0),rgb( 13,136, 29),rgb(  0,152,196),rgb(216,146,  1),rgb(169,169,169),
        rgb(  0,  0, 24),rgb( 15,  0,198),rgb(185,  0, 83),rgb(240, 50,  0),rgb( 84,223,  0),rgb(  0,200,255),rgb(243,232,  1),rgb(230,230,255),    
    },
    {
        // fantasy15, v1
        // rgb(0x04,0x0c,0x18),rgb(0x00,0x00,0xc0),rgb(0xc0,0x00,0x00),rgb(0x9d,0x00,0xd9),
        // rgb(0x00,0xc0,0x00),rgb(0x00,0xc0,0xc0),rgb(0xc8,0xc8,0x00),rgb(0xc0,0xc0,0xc0),
        // rgb(0x04,0x0c,0x18),rgb(0x00,0x40,0xff),rgb(0xea,0x06,0x40),rgb(0xff,0x40,0xff),
        // rgb(0x40,0xff,0x00),rgb(0x04,0xff,0xa2),rgb(0xff,0xd8,0x00),rgb(0xff,0xff,0xff),
        // fantasy15, v2
        rgb(  4, 12, 24),rgb( 12, 24,180),rgb(192, 11,  0),rgb(202,  0,106),
        rgb(  7,184, 20),rgb(  0,191,213),rgb(255,155,  0),rgb(211,199,154),
        rgb(  4, 12, 24),rgb( 67, 11,159),rgb(255, 16,  0),rgb(255,  0, 77),
        rgb( 86,230,  0),rgb( 56,252,179),rgb(255,230,  0),rgb(255,255,187),
    },
    {   // hue15, v1
        rgb(  0,  0,  0),rgb(  0, 33,133),rgb(150, 24,  0),rgb(184, 22,143),
        rgb(  0,176,  0),rgb(  0,183,173),rgb(255,170,  0),rgb(200,190,180),
        rgb(  0,  0,  0),rgb(  0,  0,204),rgb(204,  0,  0),rgb(226,  0, 94),
        rgb( 50,223,  0),rgb(  4,255,144),rgb(255,230,  0),rgb(240,240,255),
    },
    {
        // Hue+
        //rgb(  17,  17,  17),rgb(  35,  63, 105),rgb( 150,  54,   7),rgb( 148,  16, 175),rgb(   4, 145, 109),rgb(   0, 131, 174),rgb( 225, 153,   4),rgb( 151, 164, 151),
        //rgb(  17,  17,  17),rgb(  30, 100, 199),rgb( 185,  21,  63),rgb( 190,  12, 141),rgb( 143, 200,  13),rgb(  16, 216, 175),rgb( 244, 220,   0),rgb( 255, 255, 200),

        // OK!
//        rgb(  17,  17,  17),rgb(  33,  33,  95),rgb( 131,  37,   5),rgb( 148,  16, 175),rgb(   4, 145, 109),rgb(   0, 131, 174),rgb( 225, 153,   4),rgb( 151, 164, 151),
//        rgb(  17,  17,  17),rgb(   0,  41, 149),rgb( 156,  18,  53),rgb( 190,  12, 141),rgb( 143, 200,  13),rgb(  16, 216, 175),rgb( 244, 220,   0),rgb( 255, 255, 200),

        rgb(  17,  17,  17),rgb(  33,  33,  95),rgb( 131,  37,   5),rgb( 148,  16, 175),rgb(   3, 133, 101),rgb(   0, 140, 187),rgb( 225, 153,   4),rgb( 169, 180, 169),
        rgb(  17,  17,  17),rgb(   0,  41, 149),rgb( 156,  18,  53),rgb( 190,  12, 141),rgb( 143, 200,  13),rgb(  16, 216, 175),rgb(0xea,0xcd,   0),rgb( 255, 255, 200),

    },
    {
        // dream15, v1
        // rgb(  4, 12, 24),rgb( 11, 73,125),rgb(191, 57, 34),rgb(114, 63,182),
        // rgb(  0,191,115),rgb( 53, 98,253),rgb(255,128, 66),rgb(211,199,154),
        // rgb(  4, 12, 24),rgb( 41, 46,199),rgb(203,  1, 36),rgb(171, 52, 79),
        // rgb(255,230,  0),rgb(  0,186,251),rgb(250,204,112),rgb(255,255,170),
        //rgb(  4, 12, 24),rgb( 10, 64,131),rgb(204,  0,  0),rgb(176,  0,176),
        //rgb( 57,230,  0),rgb(  0, 98,244),rgb(255,128, 66),rgb(216,196,169),
        //rgb(  4, 12, 24),rgb( 28, 33,255),rgb(240,  0, 61),rgb(202,  0,106), //193, 49, 78),
        //rgb(223,255,  0),rgb(  0,255,255),rgb(250,204,112),rgb(255,255,170),
        // rgb(  4, 12, 24),rgb( 36, 61, 89),rgb(204,  0,  0),rgb( 67, 11,159),
        // rgb( 74,203, 31),rgb(  0, 98,244),rgb(255,136, 17),rgb(216,196,169),
        // rgb(  4, 12, 24),rgb(  0, 33,255),rgb(255,  0, 40),rgb(202,  0,106),
        // rgb(222,240,  0),rgb(  0,255,255),rgb(250,204,112),rgb(255,255,170),
        //rgb(  0,  0,  0),rgb( 31, 69,140),rgb(204,  0,  0),rgb(200, 55,146),
        //rgb(  0,191, 63),rgb( 57,130,249),rgb(255,120, 55),rgb(216,196,169), //green: rgb(  0,128, 64)
        //rgb(  0,  0,  0),rgb( 79, 45,125),rgb(255, 40,  3),rgb(234,  0, 88),
        //rgb( 13,233,  1),rgb(  3,218,191),rgb(250,188, 58),rgb(255,255,150),
        // rgb(  0,  0,  0),rgb( 31, 69,140),rgb(204,  0,  0),rgb(177,  7,148),
        // rgb(  0,144,  0),rgb( 57,130,249),rgb(255,120, 55),rgb(216,196,169), // yellow: rgb(255,112, 43)
        // rgb(  0,  0,  0),rgb( 79, 45,125),rgb(255, 40,  3),rgb(228,  5,134),
        // rgb( 64,192,  0),rgb(  3,218,191),rgb(250,188, 58),rgb(255,255,150), // yellow: rgb(250,190, 67)
        //rgb(  0,  0,  0),rgb( 31, 69,140),rgb(204,  0,  0),rgb(79,  45,125),
        //rgb(  0,144,  0),rgb( 57,130,249),rgb(255,112, 43),rgb(216,196,169),
        //rgb(  0,  0,  0),rgb(  0,  0,255),rgb(255, 40,  3),rgb(177,  7,148),
        //rgb( 64,192,  0),rgb(  3,218,191),rgb(250,188, 58),rgb(255,255,150),
        rgb(  0,  0,  0),rgb(  0, 64,159),rgb(204,  0,  0),rgb(196,  4,217),
        rgb(  0,144,  0),rgb( 57,130,249),rgb(255,136, 17),rgb(216,196,169),
        rgb(  0,  0,  0),rgb(  0, 80,255),rgb(255, 40,  3),rgb(254, 10,125),
        rgb( 64,192,  0),rgb(  3,218,191),rgb(250,188, 58),rgb(255,255,150),
    },
    {   // skin5, v1
        //rgb(  0,  0,  0),rgb( 54,  0,136),rgb(170, 40,  0),rgb(137,  0,140),
        //rgb(  0,147,  0),rgb(  7,141,156),rgb(230,155,  2),rgb(210,182,130),
        //rgb(  0,  0,  0),rgb( 32,  0,210),rgb(255, 26,  0),rgb(219, 17, 72),
        //rgb(204,233,  1),rgb( 27,252,168),rgb(247,209, 81),rgb(255,228,185),
        // rgb(  0,  0,  0),rgb( 54,  0,136),rgb(170, 50,  0),rgb(137,  0,140),
        // rgb(  0,147,  0),rgb(  7,141,156),rgb(230,155,  2),rgb(210,182,130),
        // rgb(  0,  0,  0),rgb( 32,  0,210),rgb(204,  0,  0),rgb(219,  0, 72),
        // rgb(  0,204,  0),rgb(  3,233,147),rgb(247,209, 81),rgb(255,228,185),
        //rgb(  0,  0,  0),rgb( 54,  0,136),rgb(170, 50,  0),rgb(137,  0,140),
        //rgb(  0,147,  0),rgb(  7,141,156),rgb(227,128,  0),rgb(254,203,133),
        //rgb(  0,  0,  0),rgb( 32,  0,210),rgb(204,  0,  0),rgb(255,  4, 92),
        //rgb(  0,204,  0),rgb(  3,233,147),rgb(255,174, 94),rgb(255,255,187),
        // rgb(  0,  0,  0),rgb( 54,  0,136),rgb(170, 50,  0),rgb(137,  0,140),
        // rgb(  0,147,  0),rgb(  7,141,156),rgb(227,128,  0),rgb(216,151, 92),
        // rgb(  0,  0,  0),rgb( 32,  0,210),rgb(204,  0,  0),rgb(255,  4, 92),
        // rgb(  0,204,  0),rgb(  3,233,147),rgb(255,174, 94),rgb(255,255,150),
        //rgb(  0,  0,  0),rgb( 54,  0,136),rgb(170, 50,  0),rgb(137,  0,140),
        //rgb(  0,147,  0),rgb(  0,118,215),rgb(227,159,  0),rgb(200,146,106),
        //rgb(  0,  0,  0),rgb( 32,  0,210),rgb(204,  0,  0),rgb(225,  0, 79),
        //rgb(  0,204,  0),rgb(  0,183,221),rgb(255,174, 94),rgb(221,221,204),
        // rgb(  0,  0,  0),rgb( 54,  0,136),rgb(170, 50,  0),rgb(137,  0,140),
        // rgb(  0,147,  0),rgb(  0,118,215),rgb(227,159,  0),rgb(216,176,148),
        // rgb(  0,  0,  0),rgb( 32,  0,210),rgb(204,  0,  0),rgb(225,  0, 79),
        // rgb(  0,204,  0),rgb(  0,183,221),rgb(255,174, 94),rgb(255,255,204),
        rgb(  0,  0,  0),rgb( 54,  0,136),rgb(157, 35,  0),rgb(137,  0,140),
        rgb(  0,147,  0),rgb(  0,118,215),rgb(207,122, 37),rgb(203,159,112),
        rgb(  0,  0,  0),rgb( 32,  0,210),rgb(204,  0,  0),rgb(225,  0, 79),
        rgb(  0,204,  0),rgb(  0,183,221),rgb(255,174,120),rgb(255,245,204),
    },
    {
        // Quirkafleeg - sampled from fat freddy's cat issue 5 comic front page: https://comixjoint.com/fatfreddyscat5-1st.html
        // which is referenced in the Room 16 of JetSetWilly: "We must perform a Quirkafleeg"
        // PS: there is an alt color scan in http://www.russandem.co.uk/quirk/image/fcover.jpg

#if 0
        // blacks
        rgb(  23,  22,  33), OR rgb(  12,  20,  29),

        // pale blues
        rgb(  25,  57,  96), OR rgb(  18,  38,  62),
        rgb(  22,  75, 121)

        // purples
        rgb(  47,  22, 127), OR rgb(  59,  26, 121),
        rgb(  97,  26, 130), OR rgb( 140,  22, 121),

        // pinks
        rgb( 229, 105, 112),
        rgb( 239,  97, 168),
        
        // greens
        rgb(  58, 155,  24),
        rgb( 119, 187,   4),

        // cyans
        rgb(  54, 129, 193),
        rgb(  57, 162, 226),

        // oranges
        rgb( 175,  84,  27),
        rgb( 230, 107,  27),

        // yellows
        rgb( 240, 188,   4),
        rgb( 250, 227,   2),
    
        // whites
        rgb( 189, 178, 144),
        rgb( 249, 244, 225),
#endif
        
        // v0: black, purples, reds, pinks...
        //rgb(  12,  20,  29),rgb(  47,  22, 127),rgb( 176,  49,  17),rgb( 229, 105, 112),rgb(  58, 155,  24),rgb(  54, 129, 193),rgb( 240, 188,   4),rgb( 189, 178, 144),
        //rgb(  12,  20,  29),rgb(  97,  26, 130),rgb( 227,  50,  19),rgb( 239,  97, 168),rgb( 119, 187,   4),rgb(  57, 162, 226),rgb( 250, 227,   2),rgb( 249, 244, 225),

        // v1: black, pale blues, reds, purples, greens, cyans, yellows, whites        
        // rgb(  12,  20,  29),rgb(  25,  57,  96),rgb( 176,  49,  17),rgb(  49,  23, 125),rgb(  58, 155,  24),rgb(  54, 129, 193),rgb( 240, 188,   4),rgb( 189, 178, 144),
        // rgb(  12,  20,  29),rgb(  22,  75, 121),rgb( 227,  50,  19),rgb(  99,  26, 127),rgb( 119, 187,   4),rgb(  57, 162, 226),rgb( 250, 227,   2),rgb( 249, 244, 225),

        // v2: dark pale blue, purples, reds, oranges, greens, cyans, yellows, whites
        // rgb(  18,  38,  62),rgb(  59,  26, 121),rgb( 176,  49,  17),rgb( 175,  84,  27),rgb(  58, 155,  24),rgb(  54, 129, 193),rgb( 240, 188,   4),rgb( 189, 178, 144),
        // rgb(  18,  38,  62),rgb(  97,  26, 130),rgb( 227,  50,  19),rgb( 230, 107,  27),rgb( 119, 187,   4),rgb(  57, 162, 226),rgb( 250, 227,   2),rgb( 249, 244, 225),

        // v3: dark pale blue, purples, reds, orange/pink, greens, cyans, yellows, whites
//        rgb(  18,  38,  62),rgb(  59,  26, 121),rgb( 176,  49,  17),rgb( 230, 107,  27),rgb(  58, 155,  24),rgb(  54, 129, 193),rgb( 240, 188,   4),rgb( 189, 178, 144),
//        rgb(  18,  38,  62),rgb(  97,  26, 130),rgb( 227,  50,  19),rgb( 239,  97, 168),rgb( 119, 187,   4),rgb(  57, 162, 226),rgb( 250, 227,   2),rgb( 249, 244, 225),

        // v4: dark pale blue, pale blues, reds, purples, greens, cyans, orange/yellow, whites
        rgb(   6,  25,  40),rgb(  25,  57,  96),rgb( 176,  49,  17),rgb(  97,  26, 130),rgb(  58, 155,  24),rgb(  54, 129, 193),rgb( 255, 154,  17),rgb( 201, 192, 165),
        rgb(   6,  25,  40),rgb(  22,  75, 121),rgb( 227,  50,  19),rgb( 140,  22, 121),rgb( 119, 187,   4),rgb(  57, 162, 226),rgb( 240, 188,   4),rgb( 249, 244, 225),
    },
    {
        // quirkafleeg II - sampled from a color transfer between fatfreddyscat5->fcover

        // original, washed
        // rgb(   0,  19,  30),rgb(   0,  58, 108),rgb( 110,  66,  52),rgb( 168,  81, 101),rgb(   0, 130, 121),rgb(   0, 151, 231),rgb( 221, 167,  73),rgb( 148, 168, 190),
        // rgb(   0,  19,  30),rgb(   0,  91, 143),rgb( 202,  96,  53),rgb( 225, 117, 144),rgb( 199, 219,  80),rgb(   0, 201, 254),rgb( 255, 212,  34),rgb( 216, 254, 254),

        // saturated
        rgb(   0,  19,  30),rgb(   0,  58, 108),rgb( 107,  39,  21),rgb( 182,  67,  92),rgb(   0, 130, 121),rgb(   0, 151, 231),rgb( 255, 174,  38),rgb( 182, 196, 211),
        rgb(   0,  19,  30),rgb(   6,  87, 136),rgb( 176,  48,  17),rgb( 248,  95, 133),rgb( 100, 216,  56),rgb(   0, 201, 254),rgb( 255, 212,  34),rgb( 216, 254, 254),
    },
    {
        // purple rain
        rgb(  24,  20,  37),rgb(   0,  32, 140),rgb( 160,   7,  26),rgb( 146,   6, 157),rgb(   5, 124,  33),rgb(   0, 149, 174),rgb( 220, 176,   0),rgb( 188, 188, 188),
        rgb(  24,  20,  37),rgb(   0,  32, 219),rgb( 228,   0,   0),rgb( 181,   0, 136),rgb(   0, 199,  33),rgb(   0, 232, 245),rgb( 254, 230,   0),rgb( 255, 255, 255),
    },
    {
        // nwa12774-meteorite https://www.meteorite-times.com/nwa-12774-angrite/
        //rgb(  6,  6,  6),rgb(  3, 55,103),rgb(177, 37,  0),rgb(176,  0,113),rgb( 70,187,  0),rgb( 27,184,168),rgb(244,196,  0),rgb(220,213,171),
        //rgb(  6,  6,  6),rgb(  0, 77,165),rgb(255, 85,  0),rgb(255,  0,103),rgb(111,255,  0),rgb( 75,250,201),rgb(255,233,  0),rgb(244,255,223),
        ///rgb(  6,  6,  6),rgb( 16, 37, 88),rgb(177, 37,  0),rgb(176,  0,113),rgb( 23,161, 45),rgb( 27,184,168),rgb(244,196,  0),rgb(220,213,171),
        ///rgb(  6,  6,  6),rgb(  5, 60,130),rgb(240, 78,  0),rgb(255,  0,103),rgb(102,240,  0),rgb( 75,250,201),rgb(255,233,  0),rgb(244,255,223),
        rgb(  6,  6,  6),rgb( 16, 37, 88),rgb(144, 21,  0),rgb(176,  0,113),rgb( 23,161, 45),rgb( 27,184,168),rgb(244,196,  0),rgb(220,213,171),
        rgb(  6,  6,  6),rgb(  5, 60,130),rgb(177, 37,  0),rgb(255,  0,103),rgb( 98,230,  0),rgb( 75,250,201),rgb(255,233,  0),rgb(244,255,223),
    },
    {
        // angrite2, try to do a 16-bit palette from above
        // rgb(   6,   6,   6),rgb(  34,  24,  80),rgb( 128,  32,   0),rgb( 137,  12, 137),rgb(  22, 141,  52),rgb(   1, 109, 228),rgb( 209, 105,   1),rgb( 216, 192, 158),
        // rgb(   6,   6,   6),rgb(   9,  51, 121),rgb( 183,   0,   4),rgb( 192,   3,  84),rgb( 153, 204,   0),rgb(   0, 216, 128),rgb( 244, 196,   0),rgb( 241, 255, 217),
        ///rgb(   6,   6,   6),rgb(  34,  24,  80),rgb( 128,  32,   0),rgb( 137,  12, 137),rgb(  22, 141,  52),rgb(   1, 109, 228),rgb( 209, 105,   1),rgb( 200, 182, 169),
        ///rgb(   6,   6,   6),rgb(   9,  51, 121),rgb( 172,   0,   0),rgb( 192,   3,  84),rgb( 153, 204,   0),rgb(   0, 216, 128),rgb( 244, 196,   0),rgb( 235, 222, 205),
        rgb(   6,  25,  40),rgb(  16,  37,  88),rgb( 128,  32,   0),rgb( 137,  12, 137),rgb(  22, 141,  52),rgb(   1, 109, 228),rgb( 209, 105,   1),rgb( 200, 182, 169),
        rgb(   6,  25,  40),rgb(  16,  37, 140),rgb( 172,   0,   0),rgb( 192,   3,  84),rgb( 153, 204,   0),rgb(   0, 216, 128),rgb( 244, 196,   0),rgb( 235, 222, 205),
    },
    {
        // origins
        rgb(   0,  19,  30),rgb(   0,   0, 155),rgb( 145,   0,  29),rgb( 182,   0,  92),rgb(   0, 130,   0),rgb(   0, 151, 231),rgb( 255, 174,   0),rgb( 182, 196, 211),
        rgb(   0,  19,  30),rgb(  11,   0, 217),rgb( 204,   0,  20),rgb( 248,   0, 133),rgb( 100, 216,   0),rgb(   0, 201, 254),rgb( 255, 212,   0),rgb( 216, 254, 254),
    },

    {
        // pico8: bright blue had to be added. dark gray dropped
        // rgb(000,000,000),rgb( 29, 43, 83),rgb(171, 82, 54),rgb(126, 37, 83),
        // rgb(000,135, 81),rgb(131,118,156),rgb(255,163,000),rgb(252,202,168),
        // rgb(000,000,000),rgb( 29, 43,155),rgb(255,000, 77),rgb(255,119,168),
        // rgb(000,228, 54),rgb( 41,173,255),rgb(255,236, 39),rgb(255,241,232)

        // redone: dropped dark gray.
        rgb(0x00,0x00,0x00),rgb(0x1d,0x2b,0x53),rgb(0xab,0x52,0x36),rgb(0x7e,0x25,0x53),
        rgb(0x00,0x87,0x51),rgb(0x83,0x76,0x9c),rgb(0xff,0xa3,0x00),rgb(0xff,0xcc,0xaa),
        rgb(0x00,0x00,0x00),rgb(0x1d,0x2b,0x53),rgb(0xff,0x00,0x4d),rgb(0xff,0x77,0xa8),
        rgb(0x00,0xe4,0x36),rgb(0x29,0xad,0xff),rgb(0xff,0xec,0x27),rgb(0xff,0xf1,0xe8),
    },
    {
        // petit: adapted from https://lospec.com/palette-list/petit-computer
        // darkgray as blue1, brown as red1. patched gray into cyan5. dropped skintone
        // rgb(  0,  0,  0),rgb( 56, 56, 56),rgb(146, 89, 40),rgb(121, 56,251),
        // rgb(  0,121,  0),rgb(  0,140,140),rgb(251,162,  0),rgb(186,186,186),
        // rgb(  0,  0,  0),rgb(  0, 56,243),rgb(251, 24,  0),rgb(251, 89,195),
        // rgb(  0,243, 24),rgb(  0,186,251),rgb(251,227,  0),rgb(251,251,251),

        // redone: no new colors: gray as cyan, dupe blue
        rgb(0x00,0x00,0x00),rgb(0x00,0x38,0xf3),rgb(0x92,0x59,0x28),rgb(0x79,0x38,0xfb),
        rgb(0x00,0x79,0x00),rgb(0x00,0xba,0xfb),rgb(0xfb,0xa2,0x00),rgb(0xfb,0xcb,0xa2),
        rgb(0x00,0x00,0x00),rgb(0x00,0x38,0xf3),rgb(0xfb,0x18,0x00),rgb(0xfb,0x59,0xc3),
        rgb(0x00,0xf3,0x18),rgb(0xba,0xba,0xba),rgb(0xfb,0xe3,0x00),rgb(0xfb,0xfb,0xfb),
    },
    {
        // dawnbringer's original
        // rgb( 20, 12, 28),rgb( 48, 52,109),rgb(133, 76, 48),rgb( 68, 36, 52),
        // rgb( 52,101, 36),rgb(133,149,161),rgb(210,125, 44),rgb(117,113, 97),
        // rgb( 20, 12, 28),rgb( 89,125,206),rgb(208, 70, 72),rgb(210,170,153),
        // rgb(109,170, 44),rgb(109,194,202),rgb(218,212, 94),rgb(222,238,214),
        // dawnbringer-ish, saturated by 175%, then tweaked blue1,pink3,gray7 to fit the ZX mood
        // then adjusted manually per entry
        rgb( 20, 12, 28),rgb( 40, 57,151),rgb(180, 31, 14),rgb(186, 39, 98),
        rgb( 49,160, 16),rgb(100,175,189),rgb(222,177,  1),rgb(205,201,190),
        rgb( 20, 12, 28),rgb( 33, 33,255),rgb(255, 33, 33),rgb(233, 33,126),
        rgb( 89,199,  0),rgb( 68,216,224),rgb(234,234,  0),rgb(232,252,190),
    },
    {
        // EDG32 adapted from https://lospec.com/palette-list/endesga-32 
        rgb(   0,   0,   0),rgb(  37,  42,  67),rgb( 162,  38,  51),rgb( 104,  56, 108),rgb(  38,  92,  66),rgb(   0, 155, 180),rgb( 254, 174,  52),rgb( 192, 203, 220),
        rgb(   0,   0,   0),rgb(  18,  78, 138),rgb( 228,  59,  68),rgb( 181,  80, 136),rgb(  99, 199,  77),rgb(  44, 232, 245),rgb( 254, 231,  97),rgb( 255, 255, 255),
    },

    {
        // Gradients (from specemu; probably chev's), needs to be used with gloom so it burns the colors and increases luma
        rgb(000,000,000),rgb(000,000,143),rgb(143,000,000),rgb(143,000,143),
        rgb(000,143,000),rgb(000,143,143),rgb(143,143,000),rgb(143,143,143),
        rgb(000,000,000),rgb(000,000,255),rgb(217,000,000),rgb(217,000,255),
        rgb(000,217,000),rgb(000,217,255),rgb(217,217,000),rgb(217,217,255),
    },
    {
        // Vivid: what most pc emulators use. this was a previous spectral palette (C0/FF) with pure blacks.
        // note: C0->D7 seems fine too
        // note: D8->FF is another option. see @Polyducks: "A true-to-hardware palette. Please note that the luminesence of the ZX Spectrum is dictated by the voltage output of the hardware (85% voltage for non-bright, 100% for bright) - so instead of using E/F values as dictated in the wikipedia article in the hex for non-bright/bright, the colours are instead D8/FF (D8 being 85% of FF). For example, non-bright red is given as EE0000 in the article - instead it has been portrayed here as D80000 to give as close to hardware output as possible on modern screens."
        rgb(0x00,0x00,0x00),rgb(0x00,0x00,0xC0),rgb(0xC0,0x00,0x00),rgb(0xC0,0x00,0xC0),
        rgb(0x00,0xC0,0x00),rgb(0x00,0xC0,0xC0),rgb(0xC0,0xC0,0x00),rgb(0xC0,0xC0,0xC0),
        rgb(0x00,0x00,0x00),rgb(0x00,0x00,0xFF),rgb(0xFF,0x00,0x00),rgb(0xFF,0x00,0xFF),
        rgb(0x00,0xFF,0x00),rgb(0x00,0xFF,0xFF),rgb(0xFF,0xFF,0x00),rgb(0xFF,0xFF,0xFF),
    },

#if 1

// 1 Spectaculator
{
    rgb(0x00,0x00,0x00), rgb(0x00,0x00,0xCE), rgb(0xCE,0x00,0x00), rgb(0xCE,0x00,0xCE), rgb(0x00,0xCA,0x00), rgb(0x00,0xCA,0xCE), rgb(0xCE,0xCA,0x00), rgb(0xCE,0xCA,0xCE),
    rgb(0x00,0x00,0x00), rgb(0x00,0x00,0xFF), rgb(0xFF,0x00,0x00), rgb(0xFF,0x00,0xFF), rgb(0x00,0xFB,0x00), rgb(0x00,0xFB,0xFF), rgb(0xFF,0xFB,0x00), rgb(0xFF,0xFB,0xFF)
},

#endif

    {
        // jussi ala-konni's
        rgb(0x00*4,0x00*4,0x00*4),rgb(0x00*4,0x00*4,0x28*4),rgb(0x30*4,0x00*4,0x00*4),rgb(0x30*4,0x00*4,0x28*4),rgb(0x00*4,0x2c*4,0x00*4),rgb(0x00*4,0x2c*4,0x28*4),rgb(0x30*4,0x2c*4,0x00*4),rgb(0x30*4,0x2c*4,0x28*4),
        rgb(0x00*4,0x00*4,0x00*4),rgb(0x00*4,0x00*4,0x37*4),rgb(0x3f*4,0x00*4,0x00*4),rgb(0x3f*4,0x00*4,0x37*4),rgb(0x00*4,0x3b*4,0x00*4),rgb(0x00*4,0x3b*4,0x37*4),rgb(0x3f*4,0x3b*4,0x00*4),rgb(0x3f*4,0x3b*4,0x37*4),
    },

    {
        // Richard Atkinson's colors (zx16/48/zx+ only)
//        rgb(  6,  8,  0),rgb( 13, 19,167),rgb(189,  7,  7),rgb(195, 18,175),rgb(  7,186, 12),rgb( 13,198,180),rgb(188,185, 20),rgb(194,196,188),
//        rgb(  6,  8,  0),rgb( 22, 28,176),rgb(206, 24, 24),rgb(220, 44,200),rgb( 40,220, 45),rgb( 54,239,222),rgb(238,235, 70),rgb(253,255,247),

        // above, with inverted gamma 2.2 (darker)
//        rgb(  1,   1,   0),   rgb(  2,   3,  88),   rgb(139,   1,   1),   rgb(145,   3,  96),
//        rgb(  1, 133,   2),   rgb(  2, 149, 119),   rgb(137, 134,   3),   rgb(144, 146, 135),
//        rgb(  1,   1,   0),   rgb(  4,   5,  98),   rgb(160,   4,   4),   rgb(179,   8, 139),
//        rgb(  7, 179,   8),   rgb( 11, 200, 173),   rgb(207, 202,  13),   rgb(249, 253, 230)

        // averaged between both variants
        rgb(4, 4, 0), rgb(8, 11, 128), rgb(164, 4, 4), rgb(170, 10, 136),
        rgb(4, 160, 7), rgb(8, 174, 150), rgb(162, 160, 12), rgb(169, 171, 162),
        rgb(4, 4, 0), rgb(13, 16, 137), rgb(183, 14, 14), rgb(200, 26, 170),
        rgb(24, 200, 26), rgb(32, 220, 198), rgb(222, 218, 42), rgb(251, 254, 238)
    },
    {   // sampled from signal sintez 2 capture at https://www.sinclaircollection.site/?page_id=484
        // then adjusted levels, lumas and hues. then major corrections
        rgb(  4,  0, 14),rgb( 12,  3,173),rgb(208, 24, 31),rgb(175, 32,241),
        rgb(  2,145, 81),rgb( 10,143,247),rgb(186,182, 12),rgb(182,179,198),
        rgb(  4,  0, 14),rgb( 13, 44,251),rgb(251, 40, 24),rgb(255, 64,230),
        rgb(  2,210,103),rgb( 10,219,255),rgb(225,225,  0),rgb(213,217,234),
    },

    {
        // adapted from amstrad cpc - https://www.cpcwiki.eu/index.php/CPC_Palette
        // https://www.grimware.org/doku.php/documentations/devices/gatearray

        // using my most compatible matches within CPC palette
        // rgb(000,000,000),rgb(000,000,128),rgb(128,000,000),rgb(128,000,128),
        // rgb(000,128,000),rgb(000,128,128),rgb(128,128,000),rgb(128,128,128),
        // rgb(000,000,000),rgb(000,000,255),rgb(255,000,000),rgb(255,000,255),
        // rgb(000,255,000),rgb(000,255,255),rgb(255,255,000),rgb(255,255,255),

        // CPC, more fun variant. i cheated on white7 btw, albeit would look better with 200,172,156
        rgb(   0,   0,   0),rgb(   0,   0, 128),rgb( 128,   0,   0),rgb( 128,   0, 128),rgb(   0, 128,   0),rgb(   0, 128, 255),rgb( 255, 128,   0),rgb( 172, 172, 172),
        rgb(   0,   0,   0),rgb(   0,   0, 255),rgb( 255,   0,   0),rgb( 255,   0, 128),rgb(   0, 255,   0),rgb(   0, 255, 255),rgb( 255, 255,   0),rgb( 255, 255, 255),
    },
{ 
    // cga
    //rgb(   0,   0,   0),rgb(   0,   0, 170),rgb( 170,   0,   0),rgb( 170,   0, 170),rgb(   0, 170,   0),rgb(   0, 170, 170),rgb( 170,  85,   0),rgb( 170, 170, 170),
    //rgb(  85,  85,  85),rgb(  85,  85, 255),rgb( 255,  85,  85),rgb( 255,  85, 255),rgb(  85, 255,  85),rgb(  85, 255, 255),rgb( 255, 255,  85),rgb( 255, 255, 255),

    // c1084s scanned
    //rgb(  46,  32,  33),rgb(  23,  47, 166),rgb( 162,  36,  27),rgb( 131,  23, 159),rgb(  49, 180,  30),rgb(  43, 147, 150),rgb( 152, 102,  58),rgb( 139, 145, 147),
    //rgb(  65,  53,  53),rgb(  17,  80, 210),rgb( 201,  65,  53),rgb( 188,  50, 211),rgb(  86, 229,  42),rgb(  24, 194, 203),rgb( 195, 183,  75),rgb( 188, 186, 188),

    // CGA->C1084s, adjusted @fixme: greens too bright
    rgb(  36,  18,  19),rgb(   0,  29, 173),rgb( 165,  15,   4),rgb( 125,   0, 159),rgb(  28, 190,   3),rgb(   7, 148, 151),rgb( 153,  90,  32),rgb( 152, 159, 163),
    rgb(  36,  18,  19),rgb(   0,  64, 222),rgb( 208,  44,  29),rgb( 187,  16, 216),rgb(  68, 245,  14),rgb(   0, 196, 207),rgb( 199, 184,  41),rgb( 180, 178, 180),
},
    {
        // adaptation from EGA64 palette - https://commons.wikimedia.org/wiki/File:EGA64_Full_Palette.png
        rgb(  0,  0,  0),rgb(  0,  0,170),rgb(170,  0,  0),rgb(170,  0, 85),
        rgb(  0,170, 85),rgb(  0,170,255),rgb(170,170,  0),rgb(170,170,170),
        rgb(  0,  0,  0),rgb(  0,  0,255),rgb(255,  0,  0),rgb(255,  0, 85),
        rgb(  0,255,  0),rgb(  0,255,255),rgb(255,255,  0),rgb(255,255,255),
    },
    {
        // adaptation from msx2 values. v9958 3-bit: 0,40,83,116,156,187,225,255
        rgb( 000, 000, 000),rgb( 000, 000, 156),rgb( 156, 000, 000),rgb( 156, 000, 156),rgb( 000, 156, 000),rgb( 000, 156, 156),rgb( 156, 156, 000),rgb( 156, 156, 156),
        rgb( 000, 000, 000),rgb( 000, 000, 225),rgb( 225, 000, 000),rgb( 225, 000, 225),rgb( 000, 225, 000),rgb( 000, 225, 225),rgb( 225, 225, 000),rgb( 225, 225, 225),
    },
#if 0
{ // https://lospec.com/palette-list/nec-p6
//        rgb(  16,  20,  16),rgb(   0,   0, 255),rgb( 255,   0,   0),rgb( 255,   0, 255),rgb(   0, 255,   0),rgb(   0, 174, 255),rgb( 255, 174,   0),rgb( 173, 174, 173),
//        rgb(   0,   0,   0),rgb( 173,   0, 255),rgb( 255,   0, 173),rgb(   0, 255, 173),rgb( 173, 255,   0),rgb(   0, 255, 255),rgb( 255, 255,   0),rgb( 255, 255, 255),
        // https://lospec.com/palette-list/nec-pc-6001-mk2
        //rgb(   0,   0,   0),rgb(   0,   0, 255),rgb( 255,   0,   0),rgb( 128,   0, 255),rgb(   0, 255,   0),rgb(   0, 255, 128),rgb( 255, 128,   0),rgb( 128, 128, 128),
        //rgb(   0,   0,   0),rgb(   0, 128, 255),rgb( 255,   0, 128),rgb( 255,   0, 255),rgb( 128, 255,   0),rgb(   0, 255, 255),rgb( 255, 255,   0),rgb( 255, 255, 255),
        // PC6001. fixed: increased brightness in white7
        rgb(   0,   0,   0),rgb(   0,   0, 255),rgb( 255,   0,   0),rgb( 128,   0, 255),rgb( 128, 255,   0),rgb(   0, 255, 128),rgb( 255, 128,   0),rgb( 230, 230, 230),
        rgb(   0,   0,   0),rgb(   0, 128, 255),rgb( 255,   0, 128),rgb( 255,   0, 255),rgb(   0, 255,   0),rgb(   0, 255, 255),rgb( 255, 255,   0),rgb( 255, 255, 255),

},
#endif
    {
        // gameboy. should be 4 shades, but we're doing 8 shades: many games are unplayable otherwise.
        rgb( 35-10, 84-10, 28-10),rgb( 35, 84, 28),rgb( 59-10,158-10, 59-10),rgb( 59,158, 59),rgb(124-10,186-10, 49-10),rgb(124,186, 49),rgb(174-10,255-10, 38-10),rgb(174,255, 38),
        rgb( 35-10, 84-10, 28-10),rgb( 35, 84, 28),rgb( 59-10,158-10, 59-10),rgb( 59,158, 59),rgb(124-10,186-10, 49-10),rgb(124,186, 49),rgb(174-10,255-10, 38-10),rgb(174,255, 38),
    },
    {
        // pcw-ish. take pc emulators > b/w version > green. expanded to 8 lumas for better vis (should be 4!)
        rgb(0,0x20,0x20*44/100), // there is no black in a pcw monitor afaik. use a dark green instead
        rgb(0,0x30,0x30*44/100), // boost G+B
        rgb(0,0x3E,0x3E*44/100),
        rgb(0,0x4C,0x4C*44/100),
        rgb(0,0x5A,0x5A*44/100),
        rgb(0,0x69,0x69*44/100),
        rgb(0,0x77,0x77*44/100),
        rgb(0,0x85,0x85*44/100),

        rgb(0,0x20,0x20*44/100), // there is no black in a pcw monitor afaik. use a dark green instead
        rgb(0,0x30,0x30*44/100), // boost G+B
        rgb(0,0x3E,0x3E*44/100),
        rgb(0,0x4C,0x4C*44/100),
        rgb(0,0x5A,0x5A*44/100),
        rgb(0,0x69,0x69*44/100),
        rgb(0,0x77,0x77*44/100),
        rgb(0,0x85,0x85*44/100),
    },
    {
        // amber-ish. take pc emulators > b/w version > orange. expanded to 8 lumas for better vis
        rgb(luma(0x00,0x30,0x00),luma(0x00,0x30,0x00)*44/100,0), // there is no black in an amber monitor afaik. use a dark orange instead
        rgb(luma(0x00,0x30,0x90),luma(0x00,0x30,0x90)*44/100,0), // boost G+B
        rgb(luma(0xD8,0x00,0x00),luma(0xD8,0x00,0x00)*44/100,0),
        rgb(luma(0xE0,0x00,0xE0),luma(0xE0,0x00,0xE0)*44/100,0),
        rgb(luma(0x00,0xE8,0x00),luma(0x00,0xE8,0x00)*44/100,0),
        rgb(luma(0x00,0xF0,0xF0),luma(0x00,0xF0,0xF0)*44/100,0),
        rgb(luma(0xF8,0xF8,0x00),luma(0xF8,0xF8,0x00)*44/100,0),
        rgb(luma(0xFF,0xFF,0xFF),luma(0xFF,0xFF,0xFF)*44/100,0),

        rgb(luma(0x00,0x30,0x00),luma(0x00,0x30,0x00)*44/100,0), // there is no black in an amber monitor afaik. use a dark orange instead
        rgb(luma(0x00,0x30,0x90),luma(0x00,0x30,0x90)*44/100,0), // boost G+B
        rgb(luma(0xD8,0x00,0x00),luma(0xD8,0x00,0x00)*44/100,0),
        rgb(luma(0xE0,0x00,0xE0),luma(0xE0,0x00,0xE0)*44/100,0),
        rgb(luma(0x00,0xE8,0x00),luma(0x00,0xE8,0x00)*44/100,0),
        rgb(luma(0x00,0xF0,0xF0),luma(0x00,0xF0,0xF0)*44/100,0),
        rgb(luma(0xF8,0xF8,0x00),luma(0xF8,0xF8,0x00)*44/100,0),
        rgb(luma(0xFF,0xFF,0xFF),luma(0xFF,0xFF,0xFF)*44/100,0),
    },

    {
        // noir: extremes are well defined. no many mid-colors
        #define filter(r,g,b) (0.2126 * r + 0.7152 * g + 0.0722 * b) // luma(r,g,b) // (r+g+b)/3
        #define sepia(r,g,b) rgb(filter(r,g,b),filter(r,g,b),filter(r,g,b))
        sepia(0x00,0x00,0x00),sepia(0xCD*.75,0x00,0x00),sepia(0xCD,0x00,0x00),sepia(0xCD,0x00,0xCD),
        sepia(0x00,0xCD,0x00),sepia(0x00,0xCD,0xCD),sepia(0xCD,0xCD,0x00),sepia(0xCD,0xCD,0xCD),
        sepia(0x00,0x00,0x00),sepia(0xFF*.75,0x00,0x00),sepia(0xFF,0x00,0x00),sepia(0xFF,0x00,0xFF),
        sepia(0x00,0xFF,0x00),sepia(0x00,0xFF,0xFF),sepia(0xFF,0xFF,0x00),sepia(0xFF,0xFF,0xFF),
        #undef sepia
        #undef filter
    },
    {
        // old tv set: decayed zinc vs phosphor diodes which tints closer to sepia
        #define filter(r,g,b) (0.2126 * r + 0.7152 * g + 0.0722 * b) // luma(r,g,b) // (r+g+b)/3
        #define X(x) ((byte)(((bool)x ? x/7.5 : 0.0)*255.0))
        #define sepia(Y) rgb(X(Y),X(Y)*.95,X(Y)*.80)
        sepia(0.25),sepia(1.25),sepia(2.25),sepia(3.25),sepia(4.25),sepia(5.25),sepia(6.25),sepia(7.25),
        sepia(0.25),sepia(1.50),sepia(2.50),sepia(3.50),sepia(4.50),sepia(5.50),sepia(6.50),sepia(7.50),
        #undef X
        #undef sepia
        #undef filter
    },
    {
        // Ilford FP4 Push filter
        rgb(0x0C,0x0C,0x0C),
        rgb(0x26,0x26,0x26),
        rgb(0x48,0x48,0x48),
        rgb(0x55,0x55,0x55),
        rgb(0x5F,0x5F,0x5F),
        rgb(0x69,0x69,0x69),
        rgb(0x8A,0x8A,0x8A),
        rgb(0xA8,0xA8,0xA8),
        rgb(0x0C,0x0C,0x0C),
        rgb(0x44,0x44,0x44),
        rgb(0x7D,0x7D,0x7D),
        rgb(0x91,0x91,0x91),
        rgb(0x9B,0x9B,0x9B),
        rgb(0xAF,0xAF,0xAF),
        rgb(0xC6,0xC6,0xC6),
        rgb(0xEB,0xEB,0xEB),
    },
// 4 B&W palette
{
    rgb(0x10,0x10,0x10), rgb(0x29,0x2d,0x29), rgb(0x4a,0x4d,0x4a), rgb(0x6b,0x6d,0x6b), rgb(0x7b,0x7d,0x7b), rgb(0x9c,0x9e,0x9c), rgb(0xbd,0xbe,0xbd), rgb(0xde,0xdf,0xde),
    rgb(0x10,0x10,0x10), rgb(0x31,0x31,0x31), rgb(0x5a,0x5d,0x5a), rgb(0x7b,0x7d,0x7b), rgb(0x9c,0x9e,0x9c), rgb(0xbd,0xbe,0xbd), rgb(0xe6,0xe3,0xe6), rgb(0xff,0xff,0xff)
},
    {
        // gray original, pc emulators, b/w version. c0/ff to luma Y, then rgb(y,y,y)
        // gray(0x00,0x00,0x00),gray(0x00,0x00,0xC0),gray(0xC0,0x00,0x00),gray(0xC0,0x00,0xC0),
        // gray(0x00,0xC0,0x00),gray(0x00,0xC0,0xC0),gray(0xC0,0xC0,0x00),gray(0xC0,0xC0,0xC0),
        // gray(0x00,0x00,0x00),gray(0x00,0x00,0xFF),gray(0xFF,0x00,0x00),gray(0xFF,0x00,0xFF),
        // gray(0x00,0xFF,0x00),gray(0x00,0xFF,0xFF),gray(0xFF,0xFF,0x00),gray(0xFF,0xFF,0xFF),

        // greylit new, D8/FF to luma Y to gamma 2.2
        // y = 0.299 * r + 0.587 * g + 0.114 * b; y = pow(y, 1/2.2); y *= 20
        rgb(0x00,0x00,0x00),rgb(0x56,0x56,0x56),rgb(0x85,0x85,0x85),rgb(0x9A,0x9A,0x9A),
        rgb(0xB5,0xB5,0xB5),rgb(0xC4,0xC4,0xC4),rgb(0xDA,0xDA,0xDA),rgb(0xE6,0xE6,0xE6),
        rgb(0x00,0x00,0x00),rgb(0x5D,0x5D,0x5D),rgb(0x8F,0x8F,0x8F),rgb(0xA6,0xA6,0xA6),
        rgb(0xC3,0xC3,0xC3),rgb(0xD3,0xD3,0xD3),rgb(0xEB,0xEB,0xEB),rgb(0xF8,0xF8,0xF8),
    },
    {
        // inverted gray original. pc emulators, b/w version, tv with inverted y+c signal. ^=0xFF >> 00->FF,FF->00,C0->63
        // gray(255,255,255),gray(255,255, 63),gray( 63,255,255),gray( 63,255, 63),
        // gray(255, 63,255),gray(255, 63, 63),gray( 63, 63,255),gray( 63, 63, 63),
        // gray(255,255,255),gray(255,255,000),gray(000,255,255),gray(000,255,000),
        // gray(255,000,255),gray(255,000,000),gray(000,000,255),gray(000,000,000),

        // gray new, D8/FF to luma Y to gamma 2.2. then xor^255
        gray3(0x80^0xFF),gray3(0x56^0xFF),gray3(0x85^0xFF),gray3(0x9A^0xFF),
        gray3(0xB5^0xFF),gray3(0xC4^0xFF),gray3(0xDA^0xFF),gray3(0xE6^0xFF),
        gray3(0x80^0xFF),gray3(0x5D^0xFF),gray3(0x8F^0xFF),gray3(0xA6^0xFF),
        gray3(0xC3^0xFF),gray3(0xD3^0xFF),gray3(0xEB^0xFF),gray3(0xF8^0xFF),
    },
    {
        // external. must be last entry
        0
    },
};

void palette_use(int palette, int bloom_cfg) {
#if 0
    memcpy(&ZXPalettes[palette][16], ZXPalette+16, sizeof(rgba) * (64-16) );
    memcpy(ZXPalette, ZXPalettes[palette], sizeof(rgba) * 64);
#else
    memcpy(ZXPalette, ZXPalettes[palette], sizeof(rgba) * 16);
#endif
    if(bloom_cfg) ZX_BLOOM = atoi(ZXPaletteNames[palette]);
}

int pal_loadbin(const void *palette, int size) {
    if( palette && size == 64 ) {
        if( writefile(".Spectral/Spectral.pal", palette, size) ) { // save for later
            rgba *pal32 = ZXPalettes[ (countof(ZXPalettes) - 1) ];
            memcpy(pal32, palette, 64);
            for( int i = 0; i < 16; ++i ) pal32[i] |= rgb(0,0,0); // set alpha
            return 1;
        }
    }
    if( palette && size == 48 ) {
        if( writefile(".Spectral/Spectral.pal", palette, size) ) { // save for later
            rgba *pal32 = ZXPalettes[ (countof(ZXPalettes) - 1) ];
            for( int i = 0; i < 16; ++i ) memcpy(pal32 + i, (byte*)palette + i*3, 3);
            for( int i = 0; i < 16; ++i ) pal32[i] |= rgb(0,0,0); // set alpha
            return 1;
        }
    }
    return 0;
}
int pal_load(const char *fname) {
    int size; char *data = readfile(fname, &size);
    int rc = pal_loadbin(data, size);
    if( data ) free(data);
    return rc;
}

int pick_palette_colors(Tigr *app, unsigned* pal16) {
    extern bool browser;

    int changed = 0;

    int bottom = 0;
    int offx = 0, offy = bottom ? _240 - 16*2 : 2, offy_bak = offy;
    
    struct mouse m = mouse();
    int mx = m.x, my = m.y, lmb = m.lb, rmb = m.rb;
    if( browser ) lmb = rmb = 0;
    if( bottom ) { int swap = rmb; rmb = lmb; lmb = swap; }

    static int selected1 = -1, selected2 = -1;

    // draw colors
    int _16 = _240 / countof(ZXPalettes); // ZX_ZOOM == 0 || ZX_ZOOM == 1 ? 16 : ZX_ZOOM == 1 ? 8 : 4; 
    int paloffx = _16 * 3;
    int inside = 0;
    int howmany = _240 / _16; if(howmany > countof(ZXPalettes)) howmany = countof(ZXPalettes);
    for( int h = 0; h < howmany; ++h ) {
        unsigned *pal16 = ZXPalettes[h];
        for( int c = 0; c < 16; ++c ) {
            if( c == 8 ) continue;
            int tbl[] = { 
                [0] = 0, [1] = 1, [2] = 3, [3] = 5, [4] = 7, [5] = 9, [6] = 11, [7] = 13, 
                [9] = 2, [10] = 4, [11] = 6, [12] = 8, [13] = 10, [14] = 12, [15] = 14, [8] = 15};
            int i = tbl[c];
            int offx = i*_16; offx += paloffx; // offx = _320/2-(16*_16)/2+offx;
            tigrFill(app, offx,offy+01,_16,_16, (TPixel){.rgba = pal16[(selected1 == c || selected2 == c  ) && ZXFlashFlag ? 15 : c  ]});
        }
        offy += _16;
    }

    // draw borders
    {
        int w = 15*_16;
        int offx = paloffx; ////int offx = _320/2-w/2+0*16;
        offy = ZX_PALETTE * _16 + _16 / 2 - 1;
        tigrLine(app, offx-1,offy+1,offx-1,offy+_16, (TPixel){.rgba = ui_colors[1]} );
        tigrLine(app, offx+w,offy+1,offx+w,offy+_16, (TPixel){.rgba = ui_colors[1]} );
        tigrLine(app, offx,offy+_16,offx+w,offy+_16, (TPixel){.rgba = ui_colors[1]} );
        tigrLine(app, offx,offy,offx+w,offy, (TPixel){.rgba = ui_colors[1]} );
    }

    // draw hovered color
    TPixel hovered = tigrGet(app,mx,my);
    if( mx >= paloffx && mx <= paloffx + _16*16 ) {
        // draw color rect
        tigrLine(app, mx-_16/2,my-_16/2,mx+_16/2,my-_16/2, (TPixel){.rgba = ui_colors[1]} );
        tigrLine(app, mx+_16/2,my-_16/2,mx+_16/2,my+_16/2, (TPixel){.rgba = ui_colors[1]} );
        tigrLine(app, mx+_16/2,my+_16/2,mx-_16/2,my+_16/2, (TPixel){.rgba = ui_colors[1]} );
        tigrLine(app, mx-_16/2,my+_16/2,mx-_16/2,my-_16/2, (TPixel){.rgba = ui_colors[1]} );

        if(lmb) {
            int tbl[] = { 
                [0] = 0, [1] = 1, [3] = 2, [5] = 3, [7] = 4, [9] = 5, [11] = 6, [13] = 7, 
                [2] = 9, [4] = 10, [6] = 11, [8] = 12, [10] = 13, [12] = 14, [14] = 15, [15] = 8 };

            int pal = (my-offy_bak)/_16;
            int col = (mx-paloffx)/_16; col = tbl[col];
            pal16[col] = ZXPalettes[pal][col];

            changed |= 1;
        }
    }

    if( bottom && tape_playing() ) offy -= 24;

    return changed;
}

int draw_palette(Tigr *app, unsigned* pal16, const char *name) {
    extern bool browser;

    int changed = 0;

    int bottom = 0;
    int offx = 0, offy = bottom ? _240 - 16*2 : 2;
    
    struct mouse m = mouse();
    int mx = m.x, my = m.y, lmb = m.lb, rmb = m.rb;
    if( browser ) lmb = rmb = 0;
    if( bottom ) { int swap = rmb; rmb = lmb; lmb = swap; }

    // draw borders
    {
        int w = 8*16;
        int offx = _320/2-w/2+0*16;
        tigrLine(app, offx-1,offy+1,offx-1,offy+32, (TPixel){.rgba = ui_colors[1]} );
        tigrLine(app, offx+w,offy+1,offx+w,offy+32, (TPixel){.rgba = ui_colors[1]} );
        tigrLine(app, offx,offy+32,offx+w,offy+32, (TPixel){.rgba = ui_colors[1]} );
        tigrLine(app, offx,offy,offx+w,offy, (TPixel){.rgba = ui_colors[1]} );
    }

    static int selected1 = -1, selected2 = -1;
//        printf("%d,%d\n", selected1, selected2);
    static int prev = 0; int lmb_down = lmb && prev <= 0; prev = lmb;

    // draw colors
    int inside = 0;
    for( int i = 0; i < 8; ++i ) {
        int offx = _320/2-(8*16)/2+i*16;

        tigrFill(app, offx,offy+01,16,16, (TPixel){.rgba = pal16[(selected1 == i   || selected2 == i  ) && ZXFlashFlag ? 15 : i  ]}),
        tigrFill(app, offx,offy+16,16,16, (TPixel){.rgba = pal16[(selected1 == i+8 || selected2 == i+8) && ZXFlashFlag ? 15 : i+8]});
        if( (my >= offy && my < offy+16*2) && (mx >= offx && mx < offx+16) ) {
            int index = i + 8 * (my >= offy+16);
            if( lmb_down ) {
                inside = 1;
                if( selected1 < 0 ) selected1 = index;
                else selected2 = index;
            }
            if(!browser) {
                if( rmb ) {
                    extern int cmdkey;
                    cmdkey = 'PAL0';
                }
                mouse_cursor(2); // hand
            }
        }
    }

    if( lmb_down && selected1 >= 0 ) {
        if( !inside ) {
            ZXBorderColor = selected1 & 7;
            selected1 = selected2 = -1;
        }
        else if( selected2 >= 0 ) {
            if( selected1 == selected2 ) {
                byte *entry = (byte*)(&pal16[selected1]);
                char *hexcolor = tinyfd_colorChooser(NULL,NULL,entry,entry); // #hex = pick(title,#hex,in,out)
                changed = 1;
            }
            else {
                unsigned copy = pal16[selected1];
                pal16[selected1] = pal16[selected2];
                pal16[selected2] = copy;
                changed = 1;
            }
            selected1 = selected2 = -1;
        }
    }

    TPixel hovered = tigrGet(app,mx,my);

    if( bottom && tape_playing() ) offy -= 24;

    if( name ) {
        int dims = ui_print(NULL, 0,0, NULL, name);
        int w = dims & 0xFFFF;
        ui_print(app, _320/2-w/2+offx/2,offy+8, ui_colors, name);
        offy += theFontH;
    }

    if( 1 ) {
        char hex[64];
        snprintf(hex, sizeof(hex), "#%02x%02x%02x", hovered.r, hovered.g, hovered.b);

        int dims = ui_print(NULL, 0,0, NULL, hex);
        int w = dims & 0xFFFF;
        ui_print(app, _320/2-w/2+offx/2,offy+8, ui_colors, hex);
        offy += theFontH;
    }

    ui_at(app, _320/2+offx/2-16*4-8,offy-theFontH+theFontH/2);
    if(ui_click(NULL,"<")) changed = 1, ZX_PALETTE = (ZX_PALETTE+(countof(ZXPalettes)-1)) % countof(ZXPalettes);
    ui_at(app, _320/2+offx/2+16*4,offy-theFontH+theFontH/2);
    if(ui_click(NULL,">")) changed = 1, ZX_PALETTE = (ZX_PALETTE+1) % countof(ZXPalettes);

    if( lmb ) {
        if( name ) printf("// %s\n", name);
        for(int j = 0; j < 16; ++j ) {
            TPixel p = (TPixel){.rgba = pal16[j]};
            printf("rgb(%4d,%4d,%4d),%s", p.r, p.g, p.b, !((j+1) % 8) ? "\n" : "" );
        }
    }

    if( key_pressed(TK_SHIFT) )
        changed |= pick_palette_colors(app, pal16); 

    return changed;
}


int draw_palettes(Tigr *app) {
    int changed = 0;
    return changed;
}

