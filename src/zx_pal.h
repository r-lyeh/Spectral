// contention: http://www.zxdesign.info/memContRevision.shtml
// 171,0,77 <3
// 146,89,40 <3

#define luma(r,g,b) ((byte)((r)*0.299+(g)*0.587+(b)*0.114))
#define gray(r,g,b) rgb(luma(r,g,b),luma(r,g,b),luma(r,g,b))
#define hex(rrggbb) rgb((0x##rrggbb)>>24,(0x##rrggbb&0xff00)>>16,(0x##rrggbb&0xff))

// palette used by ZX_PLAYER
enum { ZX_PLAYER_PALETTE = 14 }; // vivid

// first 2-digit number configures ZX_BLOOM
const char *ZXPaletteNames[] = {
    "00" "Spectral",
    "00" "Modern",
    "00" "Reborn",
    "00" "Remix\n",
    "00" "Merlot",
    "00" "Fantasy",
    "00" "Hue",
    "00" "Dream",
    "00" "Skin\n",
    "00" "Pico8",
    "00" "Petit",
    "00" "Bringer\n",
    "40" "Gradients", // best with ZX_BLOOM=40 ?
    "00" "Atkinson", // probably best with ZX_BLOOM=20
    "00" "Vivid",
    "00" "Konni",
    "00" "Sintez2\n",
    "40" "CPC",
    "00" "EGA",
    "00" "Gameboy",
    "20" "PCW", // best with ZX_BLOOM=20
    "90" "Amber", // best with ZX_BLOOM=90
    "20" "Gray", // best with ZX_BLOOM=20
    "20" "Negative\n", // best with ZX_BLOOM=20
    "00" "External", // must be last entry
};

rgba ZXPalettes[][64] = {

    // two sections each, 16 regular colors in total. 64 entries for ulaplus, though
    // normal: black,blue,red,pink,green,cyan,yellow,white
    // bright: black,blue,red,pink,green,cyan,yellow,white

    {
        // spectral palette. note: no pure black
        // use 0xAB if used with bloom; 0xC0 otherwise
        rgb(0x12,0x10,0x12),rgb(0x00,0x00,0xC0),rgb(0xC0,0x00,0x00),rgb(0xC0,0x00,0xC0),
        rgb(0x00,0xC0,0x00),rgb(0x00,0xC0,0xC0),rgb(0xC0,0xC0,0x00),rgb(0xC0,0xC0,0xC0),
        rgb(0x12,0x10,0x12),rgb(0x00,0x00,0xFF),rgb(0xFF,0x00,0x00),rgb(0xFF,0x00,0xFF),
        rgb(0x00,0xFF,0x00),rgb(0x00,0xFF,0xFF),rgb(0xFF,0xFF,0x00),rgb(0xFF,0xFF,0xFF),
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
        // Merlot
        rgb(  0,  0, 24),rgb(  0, 30,118),rgb(140,  0, 63),rgb(200,  0,  0),rgb( 13,136, 29),rgb(  0,152,196),rgb(250,169,  1),rgb(169,169,169),
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
        // Gradients (from specemu; probably chev's), needs to be used with gloom so it burns the colors and increases luma
        rgb(000,000,000),rgb(000,000,143),rgb(143,000,000),rgb(143,000,143),
        rgb(000,143,000),rgb(000,143,143),rgb(143,143,000),rgb(143,143,143),
        rgb(000,000,000),rgb(000,000,255),rgb(217,000,000),rgb(217,000,255),
        rgb(000,217,000),rgb(000,217,255),rgb(217,217,000),rgb(217,217,255),
    },
    {
        // Richard Atkinson's colors (zx16/48/zx+ only)
        rgb(  6,  8,  0),rgb( 13, 19,167),rgb(189,  7,  7),rgb(195, 18,175),rgb(  7,186, 12),rgb( 13,198,180),rgb(188,185, 20),rgb(194,196,188),
        rgb(  6,  8,  0),rgb( 22, 28,176),rgb(206, 24, 24),rgb(220, 44,200),rgb( 40,220, 45),rgb( 54,239,222),rgb(238,235, 70),rgb(253,255,247),
    },
    {
        // Vivid: what most pc emulators use
        // note: C0->D7 seems fine too
        // note: D8->FF is another option. see @Polyducks: "A true-to-hardware palette. Please note that the luminesence of the ZX Spectrum is dictated by the voltage output of the hardware (85% voltage for non-bright, 100% for bright) - so instead of using E/F values as dictated in the wikipedia article in the hex for non-bright/bright, the colours are instead D8/FF (D8 being 85% of FF). For example, non-bright red is given as EE0000 in the article - instead it has been portrayed here as D80000 to give as close to hardware output as possible on modern screens."
        rgb(0x00,0x00,0x00),rgb(0x00,0x00,0xC0),rgb(0xC0,0x00,0x00),rgb(0xC0,0x00,0xC0),
        rgb(0x00,0xC0,0x00),rgb(0x00,0xC0,0xC0),rgb(0xC0,0xC0,0x00),rgb(0xC0,0xC0,0xC0),
        rgb(0x00,0x00,0x00),rgb(0x00,0x00,0xFF),rgb(0xFF,0x00,0x00),rgb(0xFF,0x00,0xFF),
        rgb(0x00,0xFF,0x00),rgb(0x00,0xFF,0xFF),rgb(0xFF,0xFF,0x00),rgb(0xFF,0xFF,0xFF),
    },
    {
        // jussi ala-konni's
        rgb(0x00*4,0x00*4,0x00*4),rgb(0x00*4,0x00*4,0x28*4),rgb(0x30*4,0x00*4,0x00*4),rgb(0x30*4,0x00*4,0x28*4),rgb(0x00*4,0x2c*4,0x00*4),rgb(0x00*4,0x2c*4,0x28*4),rgb(0x30*4,0x2c*4,0x00*4),rgb(0x30*4,0x2c*4,0x28*4),
        rgb(0x00*4,0x00*4,0x00*4),rgb(0x00*4,0x00*4,0x37*4),rgb(0x3f*4,0x00*4,0x00*4),rgb(0x3f*4,0x00*4,0x37*4),rgb(0x00*4,0x3b*4,0x00*4),rgb(0x00*4,0x3b*4,0x37*4),rgb(0x3f*4,0x3b*4,0x00*4),rgb(0x3f*4,0x3b*4,0x37*4),
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

        // using the most compatible matches within CPC palette
        rgb(000,000,000),rgb(000,000,128),rgb(128,000,000),rgb(128,000,128),
        rgb(000,128,000),rgb(000,128,128),rgb(128,128,000),rgb(128,128,128),
        rgb(000,000,000),rgb(000,000,255),rgb(255,000,000),rgb(255,000,255),
        rgb(000,255,000),rgb(000,255,255),rgb(255,255,000),rgb(255,255,255),
    },
    {
        // adapted from EGA64 palette - https://commons.wikimedia.org/wiki/File:EGA64_Full_Palette.png
        rgb(  0,  0,  0),rgb(  0,  0,170),rgb(170,  0,  0),rgb(170,  0, 85),
        rgb(  0,170, 85),rgb(  0,170,255),rgb(170,170,  0),rgb(170,170,170),
        rgb(  0,  0,  0),rgb(  0,  0,255),rgb(255,  0,  0),rgb(255,  0, 85),
        rgb(  0,255,  0),rgb(  0,255,255),rgb(255,255,  0),rgb(255,255,255),
    },
    {
        // gameboy. should be 4 shades, but we're doing 8 shades: many games are unplayable otherwise.
        rgb( 35-10, 84-10, 28-10),rgb( 35, 84, 28),rgb( 59-10,158-10, 59-10),rgb( 59,158, 59),rgb(124-10,186-10, 49-10),rgb(124,186, 49),rgb(174-10,255-10, 38-10),rgb(174,255, 38),
        rgb( 35-10, 84-10, 28-10),rgb( 35, 84, 28),rgb( 59-10,158-10, 59-10),rgb( 59,158, 59),rgb(124-10,186-10, 49-10),rgb(124,186, 49),rgb(174-10,255-10, 38-10),rgb(174,255, 38),
    },
    {
        // pcw-ish. take pc emulators > b/w version > green. limited to 8 lumas for better vis (should be 4!)
        rgb(0,luma(0x00,0x20,0x00),luma(0x00,0x20,0x00)*44/100), // there is no black in a pcw monitor afaik. use a dark green instead
        rgb(0,luma(0x00,0x20,0xEA),luma(0x00,0x20,0xEA)*44/100), // boost G+B
        rgb(0,luma(0xD0,0x00,0x00),luma(0xD0,0x00,0x00)*44/100),
        rgb(0,luma(0xD0,0x00,0xD0),luma(0xD0,0x00,0xD0)*44/100),
        rgb(0,luma(0x00,0xD0,0x00),luma(0x00,0xD0,0x00)*44/100),
        rgb(0,luma(0x00,0xD0,0xD0),luma(0x00,0xD0,0xD0)*44/100),
        rgb(0,luma(0xD0,0xD0,0x00),luma(0xD0,0xD0,0x00)*44/100),
        rgb(0,luma(0xD0,0xD0,0xD0),luma(0xD0,0xD0,0xD0)*44/100),

        rgb(0,luma(0x00,0x20,0x00),luma(0x00,0x20,0x00)*44/100), // there is no black in a pcw monitor afaik. use a dark green instead
        rgb(0,luma(0x00,0x20,0xEA),luma(0x00,0x20,0xEA)*44/100), // boost G+B
        rgb(0,luma(0xD0,0x00,0x00),luma(0xD0,0x00,0x00)*44/100),
        rgb(0,luma(0xD0,0x00,0xD0),luma(0xD0,0x00,0xD0)*44/100),
        rgb(0,luma(0x00,0xD0,0x00),luma(0x00,0xD0,0x00)*44/100),
        rgb(0,luma(0x00,0xD0,0xD0),luma(0x00,0xD0,0xD0)*44/100),
        rgb(0,luma(0xD0,0xD0,0x00),luma(0xD0,0xD0,0x00)*44/100),
        rgb(0,luma(0xD0,0xD0,0xD0),luma(0xD0,0xD0,0xD0)*44/100),
    },
    {
        // amber-ish. take pc emulators > b/w version > orange. limited to 8 lumas for better vis
        rgb(luma(0x00,0x20,0x00),luma(0x00,0x20,0x00)*44/100,0), // there is no black in an amber monitor afaik. use a dark orange instead
        rgb(luma(0x00,0x20,0xEA),luma(0x00,0x20,0xEA)*44/100,0), // boost G+B
        rgb(luma(0xD0,0x00,0x00),luma(0xD0,0x00,0x00)*44/100,0),
        rgb(luma(0xD0,0x00,0xD0),luma(0xD0,0x00,0xD0)*44/100,0),
        rgb(luma(0x00,0xD0,0x00),luma(0x00,0xD0,0x00)*44/100,0),
        rgb(luma(0x00,0xD0,0xD0),luma(0x00,0xD0,0xD0)*44/100,0),
        rgb(luma(0xD0,0xD0,0x00),luma(0xD0,0xD0,0x00)*44/100,0),
        rgb(luma(0xD0,0xD0,0xD0),luma(0xD0,0xD0,0xD0)*44/100,0),

        rgb(luma(0x00,0x20,0x00),luma(0x00,0x20,0x00)*44/100,0), // there is no black in an amber monitor afaik. use a dark orange instead
        rgb(luma(0x00,0x20,0xEA),luma(0x00,0x20,0xEA)*44/100,0), // boost G+B
        rgb(luma(0xD0,0x00,0x00),luma(0xD0,0x00,0x00)*44/100,0),
        rgb(luma(0xD0,0x00,0xD0),luma(0xD0,0x00,0xD0)*44/100,0),
        rgb(luma(0x00,0xD0,0x00),luma(0x00,0xD0,0x00)*44/100,0),
        rgb(luma(0x00,0xD0,0xD0),luma(0x00,0xD0,0xD0)*44/100,0),
        rgb(luma(0xD0,0xD0,0x00),luma(0xD0,0xD0,0x00)*44/100,0),
        rgb(luma(0xD0,0xD0,0xD0),luma(0xD0,0xD0,0xD0)*44/100,0),
    },
    {
        // pc emulators, b/w version
        gray(0x00,0x00,0x00),gray(0x00,0x00,0xC0),gray(0xC0,0x00,0x00),gray(0xC0,0x00,0xC0),
        gray(0x00,0xC0,0x00),gray(0x00,0xC0,0xC0),gray(0xC0,0xC0,0x00),gray(0xC0,0xC0,0xC0),
        gray(0x00,0x00,0x00),gray(0x00,0x00,0xFF),gray(0xFF,0x00,0x00),gray(0xFF,0x00,0xFF),
        gray(0x00,0xFF,0x00),gray(0x00,0xFF,0xFF),gray(0xFF,0xFF,0x00),gray(0xFF,0xFF,0xFF),
    },
    {
        // pc emulators, b/w version, tv with inverted y+c signal. ^=0xFF >> 00->FF,FF->00,C0->63
        gray(255,255,255),gray(255,255, 63),gray( 63,255,255),gray( 63,255, 63),
        gray(255, 63,255),gray(255, 63, 63),gray( 63, 63,255),gray( 63, 63, 63),
        gray(255,255,255),gray(255,255,000),gray(000,255,255),gray(000,255,000),
        gray(255,000,255),gray(255,000,000),gray(000,000,255),gray(000,000,000),
    },
    {
        // external. must be last entry
        0
    },
};

void palette_use(int palette) {
#if 0
    memcpy(&ZXPalettes[palette][16], ZXPalette+16, sizeof(rgba) * (64-16) );
    memcpy(ZXPalette, ZXPalettes[palette], sizeof(rgba) * 64);
#else
    memcpy(ZXPalette, ZXPalettes[palette], sizeof(rgba) * 16);
#endif
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

int draw_palette(Tigr *app, unsigned* pal16, const char *name) {
    extern bool browser;

    int changed = 0;

    int bottom = 0;
    int offx = 0, offy = bottom ? _240 - 16*2 : 0;
    
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

    // draw colors
    for( int i = 0; i < 8; ++i ) {
        int offx = _320/2-(8*16)/2+i*16;

        tigrFill(app, offx,offy+01,16,16, (TPixel){.rgba = pal16[i]}),
        tigrFill(app, offx,offy+16,16,16, (TPixel){.rgba = pal16[i+8]});
        if( (my >= offy && my < offy+16*2) && (mx >= offx && mx < offx+16) ) {
            int index = i + 8 * (my >= offy+16);
            if( lmb ) {
                if( key_pressed(TK_SHIFT) ) {
                    ZXBorderColor = i;
                } else {
                    byte *entry = (byte*)(&pal16[index]);
                    char *hexcolor = tinyfd_colorChooser(NULL,NULL,entry,entry); // #hex = pick(title,#hex,in,out)
                    changed = 1;
                }
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

    if( 0 )
    if( lmb ) {
        for(int i = 0; i < 16; ++i ) {
            TPixel p = (TPixel){.rgba = pal16[i]};
            printf("rgb(%3d,%3d,%3d),%s", p.r, p.g, p.b, !((i+1) % 8) ? "\n" : "" );
        }
    }

    return changed;
}

