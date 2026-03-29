// fixme regressions: untouchables(stop tape), tai-pan(stop tape), that's the spirit,

#define has_turbo_rom() (rom_patches & TURBO_PATCH)
#define loading_from_rom() ((PC(cpu) & 0xFF00) == 0x0500) // PC(cpu) < 0x4000; // && GET_MAPPED_ROMBANK() == GET_BASIC_ROMBANK();

#define ALT_TIMINGS    0 // 1 for the wip timings
#define PAUSE_EXTEND   (loading_from_rom() || !tape_has_turbo) //(PC(cpu) < 0x4000 && !tape_has_turbo) // 1 to extend short tape pauses (fixes mostly wrong pauses in .tap tapes)
// would fix: coliseum.tap, JmenoRuze.tap, MoonAndThePirates.tap, doctum.tap, doctum.tzx
// would break: hysteria, roadblasters, thefury; they break when extra pause is added
// indirectly related: bleepload+turborom (jaws,blacklamp)
#define PAUSE_REMOVE   (mic_count(q) >= 3000) // loading_from_rom() && !tape_has_turbo) // mic_count(q) >= 1000) // /*tape_has_turbo ? 0 : loading_from_rom()*/) //(PC(cpu) < 0x4000 && !tape_has_turbo) // 1 to remove large tape pauses (speeds up tape loading)
// would optimize: nautilus.tzx,
// would break: myla di'kaich, tt racer 48, moonlighter,
// could be optimized? eg, tape_has_turbo ? count_pauses() > 8000 : loading_from_rom()

enum { PILOT = 2168, DELAY_HEADER = 8063, DELAY_DATA = 3223, SYNC1 = 667, SYNC2 = 735, ZERO = 855, ONE = 1710, END_MS = 1000, COUNT_PER_MS = 3547 }; // 3500 };
enum { LEVEL_FLIP, LEVEL_KEEP, LEVEL_LOW, LEVEL_HIGH }; // polarity
enum { LEAD_SILENCE = 100 }; // MS

#pragma pack(push, 1)    // mem fit
struct tape_block {
    unsigned count : 14; // SpecialProgram11-ITA-bootleg.tzx uses 8469. pauses are 3500, pilots use 2168, and turbo loaders tend to shorten this value
    unsigned units : 18; // most blocks specifies pulses. pa(u)se, st(o)p blocks use millisecond units though
    char level;          // polarity level: flip(0), keep(1), low(2), high(3). could be packed in 2 bits
    char debug;          // could be packed into 3 bits: pi(l)ot,sy(n)c,da(t)a,pa(u)se,st(o)p
};
#pragma pack(pop)

typedef int static_assert_tape_block[sizeof(struct tape_block) == 6 ? 1:-1];

#define VOC_DEFINES \
int             mic,mic_on; /* these two belong to zx.h */ \
byte            tape_type; \
uint64_t        tape_tstate; \
int             tape_level; \
int             tape_has_turbo, tape_num_stops, voc_len, voc_pos, voc_count, voc_units; \
int             tape_issue2; \
int             tape_counter; \
struct tape_block q;
VOC_DEFINES

char tape_preview[1000+1];
struct tape_block* voc;

// --- voc rendering

#define tape_push(d,l,c,u) do { assert((u)>=0); \
    voc[voc_len++] = ((struct tape_block){c,u,l,2[d] }); } while(0)
//#define tape_pop(numbits) do { int bits = (numbits); if( bits <= voc_len ) voc_len -= bits; else voc_len = 0; } while(0) 

void tape_render_polarity(unsigned level) {
    // MASK(+), Basil(+), ForbiddenPlanetV1(-), ForbiddenPlanetV2(-), Wizball(pzxtools).tap(+), LoneWolf3SideA128(+), LoneWolf3SideB48(+), KoronisRift48(+)
    tape_level = level;
}
void tape_render_pilot(float count, float pulse) { // Used to be x1.0250 for longer pilots
    if( pulse != PILOT ) tape_issue2 |= ((unsigned)count) & 1; // detect tape polarity: odd/even. exclude standard pilots
#if !ALT_TIMINGS
    pulse *= 1.05; // Italy 1990 (Winners Edition), Express Raider
#endif
    // Untouchables(Hitsquad), Lightforce, ATF, TT Racer, Explorer(EDS)
    tape_push("pilot", tape_level, count, pulse); // pi(l)ot
}
void tape_render_sync(float pulse) {
    // Italy 1990 (Winners Edition)
    tape_push("sync", tape_level, 1, pulse); // sy(n)c1 or sy(n)c2
}
void tape_render_stop(void) {
    // OddiTheViking, Untouchables(Hitsquad), BatmanTheMovie, ExpressRaider
    tape_push("stop", LEVEL_LOW, COUNT_PER_MS, 1); // st(o)p
    ++tape_num_stops;
}
void tape_render_pause2(unsigned pause_ms, int level) { // Used to be x1.03 for longer pauses
    // Barbarian(Melbourne), Hijack128(EDS), Italy1990(Winners), Dogfight2187, HudsonHawk
    // pauses for decompression: JmenoRuze.tap, pause command in basic: MoonAndThePirates.tap
#if !ALT_TIMINGS
    // pause_ms *= (pause_ms == END_MS) * 1.5;
    // pause_ms *= 1.03;
#endif
    // this version improves tape_preview[] accuracy
    for(unsigned i = 0; i < pause_ms; ++i) tape_push("pause", level, COUNT_PER_MS, 1); // pa(u)se
}
void tape_render_pause(unsigned pause_ms) {
    tape_render_pause2(pause_ms, LEVEL_LOW);
}
void tape_render_bit(int bit, int states_per_bit) { // rate 79 for 44100, 158 for 22050
#if ALT_TIMINGS
    states_per_bit = 1;
#endif
    tape_push("piLot", bit ? LEVEL_HIGH : LEVEL_LOW, 1, states_per_bit);
}
void tape_render_data(byte *data, unsigned bytes, unsigned bits, unsigned zero, unsigned one, int bitrepeat) {
    // keep 2nd byte in safe place
    if(tape_type == 0xFF && bytes > 1) tape_type = data[1];
    // render bits
    for( ; bytes-- > 0; ++data ) for( int i = 0; i < 8; ++i ) {
        tape_push("data", tape_level, bitrepeat, ((*data) & (1<<(7-i)) ? one : zero)); // da(t)a
    }
    // truncate excess bits within last byte if needed
    voc_len -= 8 - bits; // tape_pop(8 - bits); // 
}

void tape_render_full(byte *data, unsigned bytes, unsigned bits, float pilot_len, unsigned pilot, unsigned sync1, unsigned sync2, unsigned zero, unsigned one, unsigned pause) {
    tape_render_pilot(pilot_len, pilot);
    tape_render_sync(sync1);
    tape_render_sync(sync2);
    tape_render_data(data, bytes, bits, zero, one, 2);
    tape_render_pause(pause);
}
void tape_render_standard(byte *data, unsigned bytes, float pilot_len) {
    tape_render_full(data, bytes, 8, pilot_len, PILOT, SYNC1, SYNC2, ZERO, ONE, END_MS);
}

const byte *TAP_sof, *TAP_pof, *TAP_eof; // start,position,eof of binary contents (FlashLoad)

void tape_reset(void) {
    memset(tape_preview, 0, sizeof(tape_preview));

    voc = realloc(voc, sizeof(struct tape_block) * 0x1800000);
    voc_len = 0;
    voc_pos = voc_count = voc_units = 0;

    tape_has_turbo = 0;
    tape_num_stops = 0;
    tape_issue2 = 0;
    tape_type = 0xFF;
    tape_counter = 0;

    TAP_sof = TAP_pof = TAP_eof = 0;

    mic = 0;
    mic_on = 0;

    tape_level = LEVEL_FLIP;
    tape_tstate = 0;

    memset(&q, 0, sizeof(struct tape_block));

    tape_render_pause(LEAD_SILENCE); // give tape some leading silence (stabilizes tape levels during rewinds)
}

void tape_finish() {
#if 1 // PAUSE_REMOVE
    // trim ending silences. see: abusimbelprofanation(gremlin) 16000ms nipper2(kixx) 23910ms
    for( int i = voc_len; --i >= 0; )
        if( !strchr("uo", voc[i].debug) ) break; // pa(u)se st(o)p
            else voc[i].units = 1, voc[i].debug = 'o';
#endif

    // write a terminator
    tape_render_stop();

    // we could trim extremely large gaps now (>13s). however, we are missing a good heuristic to discern
    // between real multiload pauses (gauntlet.tzx) and bad tapes (jacknipper2(kixx).tzx). disabled for now.
    // idea: do not trim gaps on tape if any stop-tape blocks are found. wont work for tap/csw files, though.
    if(0)
    for( int i = 0; i < voc_len; ++i )
        if( voc[i].debug == 'u' && voc[i].units > 5000 )
            voc[i].units = 5000;

    // @fixme: highlight .bas programs, side Bs, glue blocks and tzx groups
    // create tape preview in 2 steps
    // 1) any kind of noise is a dotted line (==1)
    // 2) ensure silences (==0) are clearly blank over dots from step 1.
    int tape_color = 1;
    for( int i = 0; i <= 1000; ++i ) tape_preview[i] = tape_color;
    for( unsigned pos = 0; pos < voc_len; ++pos ) {
        unsigned pct = pos * 1000.0 / (voc_len + !voc_len);

        tape_color += voc[pos].debug == 'o', tape_color -= 6 * (tape_color > 6);
        tape_preview[pct] = tape_color;

        int silence = 2 * !!strchr("uo", voc[pos].debug) + !!strchr("l", voc[pos].debug); // 2-3 blanks for pa(u)se, st(o)p, pi(l)ots
        for( int i = 0; i < silence; ++i ) if(pct - i * (pct >= i) >= 0) tape_preview[pct - i * (pct >= i)] = 0;
//        int specials = 2 * !!strchr("ln", voc[pos].debug); // pi(l)ot and sy(n)c tones
    }
}

// --- mic reading

struct tape_block mic_read_tapeblock(int voc_pos) {
    struct tape_block q = voc[ voc_pos ];

    // patch pilots to be consistently odd/even across the whole tape blocks (see: Mask(IBSA).tzx)
    // fixes: mask(ibsa)(-)(+)
    // improves: forbiddenPlanet(-), lonewolf3(+) in any issue2/3 combination
    // keeps: koronisRift(-)
    // breaks: wizball.tap(pzxtools)
    if( 0 )
    if( q.debug == 'l' /* && tape_issue2 */ )
        q.count = issue2 ? q.count | 1 : q.count & ~1;

    // Tape compensation for 128k/+2/+2a/+3 models. Canonical value would be 70908/69888 (1%)
    // Used to be 1.025 or 1.03 for me, though. not turborom friendly
#if !ALT_TIMINGS
    float scale = ZX > 48 && !ZX_PENTAGON ? 70908 / 69888. : 1.0;
    //q.units *= scale;
#endif

    // convert normal to turbo block, if needed
    if( has_turbo_rom() && loading_from_rom() ) {
        /**/ if( q.debug == 'l' ) { // pi(l)ot
            q.count *= 2.00; // extra count. this is only needed in standard loads if turborom is present and we're loading basic blocks like 720º or Cauldron(Silverbird) with small pauses
            IF_TURBOROM_FASTER_EDGES(q.units -= 358);          // ROMHACK $5e7 x16 faster edges (OK)
            IF_TURBOROM_FASTER_PILOTS_AND_PAUSES(q.count /= 6); // ROMHACK $571 x6 faster pilots/pauses (OK)
        }
        else if( q.debug == 'n' ) { // sy(n)c
            IF_TURBOROM_FASTER_EDGES(q.units -= 358);  // ROMHACK $5e7 x16 faster edges (OK)
        }
        else if( q.debug == 't' ) { // da(t)a
            IF_TURBOROM_HALF_BITS(q.count /= 2);           // ROMHACK $5ca 50% eliminate dupe bits of data
            IF_TURBOROM_FASTER_EDGES(q.units -= 358);  // ROMHACK $5e7 x16 faster edges (OK)
            IF_TURBOROM_TURBO(q.units /= ROMHACK_TURBO);  // ROMHACK $5a5 turbo loader (OK)
        }
        else if( q.debug == 'u' ) { // pa(u)se
            #if 0 // removed on v06/v07 because of spirits.tzx
            IF_TURBOROM_FASTER_EDGES(q.units -= 358);          // ROMHACK $5e7 x16 faster edges (OK)
            #endif
            #if 0 // removed on v104 because of Afterburner.tzx and many TopoSoft games
            IF_TURBOROM_FASTER_PILOTS_AND_PAUSES(q.units /= 6); // ROMHACK $571 x6 faster pilots/pauses (OK)
            #endif
        }
    }

    return q;
}

unsigned mic_count(struct tape_block q) { // count number of matching blocks from current tape pos
    unsigned count = voc_pos;
    while( count < voc_len && !memcmp(&voc[count], &q, sizeof(struct tape_block)) ) count++;
    return count - voc_pos;
}

byte mic_read(uint64_t tstates) {
    mic_on *= voc_pos < voc_len; // stop tape at end of tape

    extern uint64_t ticks;
    int diff2 = ticks - tstates;

    extern uint64_t ticks; tstates = ticks;
    int diff = tstates - tape_tstate;
    tape_tstate = tstates;

    // there are tapes with much shorter pauses than needed. these games usually display a fancy animation
    // or intro which takes a few seconds to finish, meanwhile the tape motor will progress quickly thru the
    // current short pause block. by the time the intros are done, the tape lead could have read the next
    // pilot and sync tones, and it may be wrongly positioned within next data tones block. the load is a failure
    // at that point because it was never meant to be played like that. the proper way to fix this would be
    // to edit all the wrong .tap/.tzx files on the internet. since that is insane, i guess we will do like
    // most other emulators and playtzx tools do: try to fix pathological bad tapes with extremely short pauses.
    // note that emulators with rom trap loading wont ever probably notice this issue at all since they
    // are cheating by skipping the whole tape loading routines and doing a memcpy() transfer instead.
    //
    // * notably wrong authored tapes: Doctum.tzx, Coliseum.tap, Barbarian(melbourne) (1s pause block vs +10s intro time).
    // - related: JmenoRuze.tap, MoonAndThePirates.tap, (not accounting for decompression or additional pauses)
    // - related: TheEgg, Cauldron2.tap, Diver(2004) (failure depends on how much time the user takes to press a key)
    // checkme: swords & sorcery,
    // * beware of extending silences, these games are sensitive:
    // - may fail during load: Hysteria, road blaster

    // while we're at it, there are also games with extremely large pause blocks that could be better trimmed.
    // many games add unnecessary large pauses in between data blocks with no user intervention at all.
    //
    // * beware of eating silences, these games are sensitive:
    // - may fail at the very end: Alien8, HeadOverHeels(speedlock), Rambo(HitSquad)...
    // - may fail during load: LoneWolf3, Jaws, Renegade(HitSquad), WTSS
    // - may get stuck before screen$: BookOfTheDeadPart1(CRL/GDB), Cauldron2(Silverbird), DarkStar(pzx) ...

    if( diff2 < diff ) diff = diff2;
    if( diff < 0 ) diff = 0;

    if( mic_on )
    {
        extern int tape_hz;
        int been_reading = tape_hz > 300 && diff < 60;

        static int been_hz;
        been_hz = (been_hz + 1) * !!been_reading;

        if( !been_reading && strchr("uo", q.debug) && PAUSE_EXTEND ) {
            diff = 1;
        }

        else

        if( been_hz > 10 && q.debug == 'u' && PAUSE_REMOVE ) {
#if 1
            int before = voc_pos;
            while( voc_pos < voc_len && !memcmp(&voc[voc_pos], &q, sizeof(struct tape_block)) ) voc_pos++;
            voc_pos -= 2 * (voc_pos > 1);
            if( voc_pos < before ) voc_pos = before;
            // printf("pause %d -> %d; [-1]%c [0]%c [1]=%c [2]=%c\n", before, voc_pos, voc[voc_pos-1].debug, voc[voc_pos].debug, voc[voc_pos+1].debug, voc[voc_pos+2].debug);
#else
            int before = voc_pos;
            int len = mic_count(q);
            voc_pos += len / 2;
            q = voc[voc_pos];
            // printf("pause %d -> %d; [-1]%c [0]%c [1]=%c [2]=%c\n", before, voc_pos, voc[voc_pos-1].debug, voc[voc_pos].debug, voc[voc_pos+1].debug, voc[voc_pos+2].debug);
#endif
        }
    }

    if( mic_on ) { // if tape not stopped
        // debug
        // printf("%c [%d / %d][%d / %d][%d / %d] += %d\n", voc[voc_pos].debug,voc_pos,voc_len, voc_count,q.count, voc_units,q.units, diff);

        // fsm0, reset line
        enum { LEVEL = 64 }; // low(0), high(64). high(+) by default; however, +2 models got tape polarity inverted(-) [see: LA6324 quad op-amp (IC302)]
        if( !q.units ) {
            mic = LEVEL ^ 64;
            q = mic_read_tapeblock(voc_pos);
        }

        // fsm2, advance clock. the three counters do work like a clock cycle hh:mm:ss
        // hh:position in voc[], mm:sub-count, ss:sub-units
        for( int inc = min(diff, q.units); diff > 0; diff -= inc, inc = min(diff, q.units) )
        if( (voc_units += inc) >= q.units ) {
            voc_units -= q.units;

            /**/ if( q.level == LEVEL_FLIP ) mic ^= 64;
            else if( q.level == LEVEL_HIGH ) mic = LEVEL;
            else if( q.level == LEVEL_LOW )  mic = LEVEL ^ 64;

            if( ++voc_count >= q.count ) {
                voc_count = 0;
                voc_pos ++;
                voc_pos -= voc_pos >= voc_len;

                // transfer new block
                q = mic_read_tapeblock(voc_pos);
                if( q.debug == 'o' ) return mic_on = 0, mic; // st(o)p
            }
        }
    }

    return mic;
}

// --- tap loading

int tap_load(const void *fp, int siz) {
    if( memcmp(fp, "\x13\x00", 2) ) return 0;

    tape_reset();

    if(ZX_FLASHLOAD) {
    TAP_sof = TAP_pof = fp; // used for flashload
    TAP_eof = TAP_sof + siz; // used for flashload
    //return 1;
    }

    byte *begin=(byte *)fp, *pos = begin, *end=begin+siz;
    for( int block = 0; pos < end; ++block ) {
        //length in bytes
        unsigned bytes = ((pos[1]<<8) | pos[0]);
        if(bytes <= 2) break; // fix bad jet-pac.tap

        #if 0 // we dont hotpatch media contents anymore. we do hotpatch memory contents now instead.
        // redo checksum. not needed unless tape contents have been hot patched
        byte *checksum = pos + 2 + bytes - 1; *checksum = 0;
        for( unsigned i = 0; i < (bytes - 1); ++i ) *checksum ^= pos[2+i];
        #endif

        printf("tap.block %03d (%s) %u bytes\n",block,pos[2] ? "HEAD":"DATA", bytes - 2);

        // data(2s) or header(5s) block?
        tape_render_standard(pos+2, bytes, pos[2] < 128 ? DELAY_HEADER : DELAY_DATA);
        pos += 2 + bytes;
    }

    tape_finish();
    return 1;
}

// --- tape utils

#define tape_level() tape_level_()
int tape_level_() {
    if(ZX_FLASHLOAD) if(TAP_sof) return 0;
    return !!mic;
}
int tape_inserted() {
    if(ZX_FLASHLOAD) if(TAP_sof) return TAP_pof < TAP_eof;
    return voc_len > LEAD_SILENCE; // (!!voc_len)
}
int tape_seeki(int at) {
    if(ZX_FLASHLOAD) if(TAP_sof) return 0;
    return voc_pos = voc && voc_len && at >= 0 && at < voc_len ? at : voc_pos;
}
int tape_seekf(float at) {
    if(ZX_FLASHLOAD) if(TAP_sof) return 0;
    return voc_pos = voc && voc_len && at >= 0 && at <= 1 ? at * (voc_len - 1) : voc_pos;
}
int tape_peek() {
    if(ZX_FLASHLOAD) if(TAP_sof) return 'u';
    return voc_pos < voc_len ? voc[voc_pos].debug : ' ';
}
float tape_tellf() {
    if(ZX_FLASHLOAD) if(TAP_sof) return (TAP_pof - TAP_sof) / (float)(TAP_eof - TAP_sof);
    return voc_pos / (float)(voc_len+!voc_len);
}
int tape_playing() {
    if(ZX_FLASHLOAD) if(TAP_sof) return mic_on && (TAP_pof > TAP_sof);
    return (mic_on && voc_len);
}
int tape_feeding() {
    return tape_playing() && !strchr("uo", tape_peek());
}
void tape_play(int);
void tape_stop() {
    tape_play(0);
}


/*
int tape_play(on) {
    return mic_on = !!(on);
}
*/
void tape_play(int on) {
    mic_on = !!on;
    if( on && tape_inserted() ) {
        if( voc_pos < (voc_len-1) && strchr("uo",q.debug) ) {
            extern uint64_t tape_ticks;
            mic_read(tape_ticks);
            q = mic_read_tapeblock(++voc_pos);
            voc_count = voc_units = 0;
        }
    }
}

void tape_next() {
    tape_play(0);
    if( voc_len ) {
        while(voc_pos < voc_len && 'l' == voc[voc_pos].debug) voc_pos++; // skip pi(l)ot; current, if any
        while(voc_pos < voc_len && 'l' != voc[voc_pos].debug) voc_pos++; // find pi(l)ot
        voc_pos -= (voc_pos >= voc_len), voc_count = voc_units = 0;
        tape_tstate = 0;
    }
}
void tape_prev() {
    tape_play(0);
    if( voc_len ) {
        while((unsigned)voc_pos < voc_len && 'l' == voc[voc_pos].debug) voc_pos--; // skip pi(l)ot; current, if any
        while((unsigned)voc_pos < voc_len && 'l' != voc[voc_pos].debug) voc_pos--; // find pi(l)ot
        while((unsigned)voc_pos < voc_len && 'l' == voc[voc_pos].debug) voc_pos--; // skip pi(l)ot
        voc_pos += (voc_pos < 0), voc_count = voc_units = 0;
        tape_tstate = 0;
    }
}
void tape_rewind() {
    mic = 0;
    mic_on = tape_inserted();

    voc_pos = voc_count = voc_units = 0;
    tape_level = LEVEL_FLIP;
    tape_tstate = 0;
}

#define VOC_IMPORT \
    IMPORT(mic); \
    IMPORT(mic_on); \
    IMPORT(tape_type); \
    IMPORT(tape_tstate); \
    IMPORT(tape_level); \
    IMPORT(voc_len); \
    IMPORT(tape_has_turbo); \
    IMPORT(tape_num_stops); \
    IMPORT(tape_issue2); \
    IMPORT(tape_counter); \
    IMPORT(voc_pos); \
    IMPORT(voc_count); \
    IMPORT(voc_units); \
    IMPORT(q);

#define VOC_EXPORT \
    EXPORT(mic); \
    EXPORT(mic_on); \
    EXPORT(tape_type); \
    EXPORT(tape_tstate); \
    EXPORT(tape_level); \
    EXPORT(voc_len); \
    EXPORT(tape_has_turbo); \
    EXPORT(tape_num_stops); \
    EXPORT(tape_issue2); \
    EXPORT(tape_counter); \
    EXPORT(voc_pos); \
    EXPORT(voc_count); \
    EXPORT(voc_units); \
    EXPORT(q);

// @fixme
// - [ ] devices: enterprise,
// - [ ] rom: macadam bumper,
// - [ ] bad tapes? lonewolf128.tap, krakatoa.tzx, st dragon (kixx).tzx
//
// test [* missing ** crash]
// [x] loaders: *black arrow, black tiger, blood brothers/deflektor(erbe), bobby bearing, joe blade 2, fairlight,
// [x] loaders: fighting warrior, moonstrike, *rigel's revenge, trap door, travel trashman,
// [x] loaders: *return of bart bear, *ballbreaker, cobra, critical mass,
// [x] loaders: **freddy hardest, gunrunner/nebulus/zynaps, *joe 128, indy last crusade, *locomotion
// [x] loaders: lode runner, *podraz 32, saboteur, **wizball.tap, manic miner (?), scooby doo, splat v2,
// [x] loaders: star wars, technician ted, three weeks in paradise 48k, uridium/firelord, xeno, ik+,
// [x] loaders: spirits, *song in 3 lines,
// [x] loaders: aspargpmaster(grandprixmaster)(dinamic), capitantrueno
// [x] loaders: soldieroffortune, blacklamp, bubble bobble, flying shark
// [x] loaders: lone wolf 3
// [x] loaders: donkey kong (erbe), cobra(erbe)
// [x] loaders: astro marine corps, rescue atlantis
// [x] loaders: hunt for red october
// [x] loaders: buffalo bill
//
//
// refs:
// - http://ramsoft.bbk.org.omegahg.com/mtzxman.html#HISTORY
// - https://www.alessandrogrussu.it/loading/schemes/Schemes.html
// - http://newton.sunderland.ac.uk/~specfreak/Schemes/schemes.html
// - https://faqwiki.zxnet.co.uk/wiki/Loading_routine_%22cores%22
//
// [-/+] Failures | Loader Name | Games | Total (-) 9 tape errors, (+) 9 tape errors, (!) fails, (^C) ok but fails with turborom, (?) could not test anymore
// [ / ] _unknown_                Myla Di'Kaich
// [ / ] _unknown_                Twister ^C
// [ / ] _unknown_                Zanthrax.tzx
// [ / ] Biturbo                  Special Program, Playgames
// [ / ] NN:Hollywood Poker     ? Hollywood Poker
// [ / ] Odeload/UnilODE          Trivial Pursuit (HitSquad), (Erbe), ... Genus Edition Question Tape Side A, Trivial Pursuit Baby Boomer Edition Question Tape side B, Trivial Pursuit Young Players Edition Question Tape
// [0/0] _unknown_                Falcon Patrol 2
// [0/0] Activision               Time Scanner ^C, Pac-Land, Dynamite Dux, Ninja Spirit
// [0/0] Alkatraz                 720, Artura, Legend of Kage, bobby bearing, Brave starr, fairlight 128k, fairlight 2 128k, hate, rolling thunder, Alien Syndrome 48 (most Kixx and US Gold releases)
// [0/0] Alkatraz 2               Strider 2 (US Gold) ^C, Gauntlet 3, Outrun Europa
// [0/0] BleepLoad                Black Lamp, Jaws, Sentinel 48, Bubble Bobble, I,Ball; Brainstorm (Firebird), Earthlight, Flying Shark, Kinetik, The Plot, Rick Dangerous, Soldier of Fortune, Thrust II 48, Ninja Scooter, Zolyx
// [0/0] Busy soft                Jet-Story, Double Dash, Kliatba noci, Quadrax
// [0/0] Cyberlode 1.1            Cauldron (Silverbird), Antiriad 48 (Silverbird)
// [0/0] Digital Integration  ^C  TT Racer 48 (4 Aces) ^C, ATF 48, Tomahawk (4 Aces)
// [0/0] Dinaload                 Capitan Trueno, Grand Prix Master, Satan, Cosmic Sheriff, Freddy Hardest En Manhattan Sur, Michel Futbol Master
// [0/0] EDOS                 ^C  Beyond Ice Palace (EDOS), Bomb Jack 2 (EDOS), Street Cred Football (EDOS)
// [0/0] Elite                    Kokotoni Wilf 48, Dukes of Hazzard 48,
// [0/0] Excelerator              Loads of Midnight, Last Mohican, Jack the Ripper
// [0/0] Flash Load               Dan Dare, Great Fire of London, Cliff Hanger 48, Strangeloop,
// [0/0] FTL                      Thundercats, Supertrux
// [0/0] Gargoyle                 Heavy on the Magick, Lightforce (Rack-It)
// [0/0] Richlock                 Hydrofool (Rack-It), Light Force 48, Light Force (Rack-It), 
// [0/0] Gremlin              ^C  Metabolis, Monty on the Run, DeathWish3, Grumpy Gumphrey Super Sleuth, Way of the Tiger, Bounder, Jack the Nipper, Avenger, Future Knight
// [0/0] Gremlin 2                Basil the Mouse Detective (10 Great Games 2), Mask, Mask (10 Great Games 2)
// [0/0] Haxpoc-Lock              Star Wars (Domark)
// [0/0] Hyper-Loading            LaserWARp, Air Traffic Control
// [0/0] Injectaload              Book of the Dead (CRL), Ninja Hamster, Outcast, (3D Game Maker?)
// [0/0] Jet-Load                 Prelude ^C, Classroom Chaos, Dungeon Dare, The Greatest Show on Earth
// [0/0] Jon North              ? YS Poke Tapes
// [0/0] LazerLoad 48             Macadam Bumper (PSS) 48 ^C
// [0/0] Lerm                   ? Lerm Microdrive 1 - Side A, Lerm Microdrive 1 - Side B, Lerm Tape Copier 6, Lerm Tape Copier 7 48
// [0/0] LoadA-Game               Joe Blade 2
// [0/0] Micromega                Kentilla 48 ^C, Braxx Bluff, Jasper, A Day in the Life, Glass(BugByte),
// [0/0] Microprose             * Times of Lore ^C, Xenophobe
// [0/0] Microsphere              Skool Daze, Sky Ranger, Back to Skool
// [0/0] Mikro-Gen                Automania, Pyjamarama, Witch's Cauldron, Everyone's a Wally, Herbert's Dummy Run, Shadow of the Unicorn, Three Weeks in Paradise (both), Battle of the Planets, Equinox, Stainless Steel, Frost Byte, Cop-Out
// [0/0] Movieload              * Moon Strike 48 ^C
// [0/0] NN:Moonlighter           Moonlighter
// [0/0] NN:Roller Coaster        Roller Coaster
// [0/0] NN:Worldcup              World Cup Football 48 ^C
// [0/0] Novaload 48            * Swords & Sorcery 48, Covenant
// [0/0] Nu-Load Ninety One       The Hunt for Red October(Grandslam)
// [0/0] ODEload                  Sailing, Sailing (Mastertronic), Sailing (Mastertronic Plus), Trivial Pursuit Genus Edition Question Tape side B, 
// [0/0] Paul Owens System        Batman the Movie, The Untouchables, Chase HQ, Rainbow Islands, Cabal, Operation Thunderbolt, Red Heat (latest Ocean games)
// [0/0] Players 1                Xanthius, Deviants, Fernandez Must Die, Fox Fights Back, Tomcat, Street Gang, Task Force, Street Cred' Football, Spooked, Cobra Force, Prohibition, Denizen, Elven Warrior, Saigon Combat Unit, Joe Blade 3, Prison Riot
// [0/0] Players 2                Joe Blade, Metal Army ^C, Andy Capp, Tetris, Thing!, Tanium, Shanghai Karate, Sword Slayer, PowerPlay
// [0/0] Poliload (DinamicLoader) Astro Marine Corps, Rescate Atlantida (Rescue From Atlantis)
// [0/0] PowerLoad 48             Confuzion, Death Wake, Dynamite Dan, Arena, Boulderdash, SpyVsSpy, Tantalus
// [0/0] Proxima Software     ^C  Orion 48, Inferno 48,
// [0/0] Rapid                    Travel With Trashman, Zombie Zombie
// [0/0] Really-quite-fast-loader Gary Lineker's Superskills
// [0/0] SearchLoader             The Final Matrix, Ranarama ^C, City Slicker
// [0/0] Sentient                 Tai-Pan, A Question of Scruples, Guerrilla War
// [0/0] SetoLoad               ? SL-multi-test.tzx
// [0/0] Softlock                 Elite 48k (Firebird), Rasputin 48k, Chimera 48k, Skyfox, PHM Pegasus, Cylu 48k, Impossible Mission 2
// [0/0] Software Projects        BC's Quest for Tires 48 ^C, Learning with Leeper
// [0/0] Speedlock 1              Batman, Alien 8, Rambo 48, Beach Head, Blue Max 48, Cyclone 48, Decathlon 48, Gilligan's Gold 48, Match Day 48, Mikie 48, NOMAD 48, Spy Hunter 48, Yie Ar Kung Fu 48
// [0/0] Speedlock 1/2 Hybrid     Highlander
// [0/0] Speedlock 2              Head Over Heels 128k, Alien Highway, Enduro Racer, The Great Escape, Green Beret, Tarzan, Yie Ar Kung Fu 2 48
// [0/0] Speedlock 3         ^C ! Triaxos, Leviathan, Dogfight 2187
// [0/0] Speedlock 4            ! Athena 128, Mutants, Tai-Pan, Wizball, Road Runner, The Ninja Warriors, Slap Fight,
// [0/0] Speedlock 5              Road Blasters, Hysteria, Action Force 2, Outrun, Gryzor, Phantom Club, Slaine
// [0/0] Speedlock 6              The Fury, Platoon (Release1), Super Hang-On (Proein), MatchDay II (TheHitSquad), Vixen
// [0/0] Speedlock 7              The Addams Family, Arkanoid 2/1, Batman The Caped Crusader, Target Renegade, WTSS, Vindicator, Robocop2, Typhoon, Simpsons, TimeMachine (most TheHitSquad releases)
// [0/0] Speedlock 8?             Robocop 3
// [0/0] Speedlock Associates     Dan Dare 2
// [0/0] Sprite Novaload          Theatre Europe
// [0/0] The Edge                 Starbike ^C, That's the Spirit, Psytraxx, Brian Bloodaxe
// [0/0] Uniloader                Batty, IkariWarriors ^C, Bomb Jack 2
// [0/0] ZetaLoad                 Wec Le Mans (Imagine) [1]
// [0/0] Zydroload                The Light Corridor, North & South, Magic Johnson,
// [0/0] Multiload                Blood Brothers, Deflektor, 
// [] Elite                       Bomb Jack, Commando,
// [] Animagic                    Bronx, Cyberbig, Mortadelo y Filemon 2,
// [] Audiogenic                  Lone Wolf: Mirror of Death,
// [] Tynesoft                    Buffalo Bill's Wild West Show,
// [] Dinamic                     Saimazoom, Abu Simbel Profanation, Camelot Warriors 48 (Ariola)
// [] Topo                        Spirits, Tuareg, Coliseum (Kixx), Gunsmoke, Mundo Perdido,
// [] Hewson                      Cybernoid, Marauder, Nebulus, Exolon, Gunrunner 48, Impossaball,
// [] Stephen Crow                Firelord, Starquake, Uridium, Wizard's Lair
// [] Piranha                     Trap Door,
// [] Tiny's Loader               Canyon Warrior,
// [] CEZ team                    Cannon Bubble,
// [] Busy Soft                   Cesta Bojovnika,
// [0/0] Choice ?                 Tank, Beach Volley (ERBE),
// [] ERBE                  ^C    Conquestador,
// []                             Donkey Kong (Erbe), Bazooka Bill (Erbe/IBSA), Cobra (Erbe/IBSA), Conquestador (Erbe), 
// [] The Sales Curve             St Dragon, Continental Circus,
// [] Mirrorsoft                  Mean Streak,
// [] Customs                     Abadia del Crimen
// [] Customs                     Crusader, Cyborg Terminator 2, Darius+, DarkSide, Forbidden Planet, Freddy Hardest, Frontiers, FullThrottle, Gary Lineker's Superskills, Gimme Bright, 
// Academy(Pim), action reflex, airbone ranger, amazing rocketeer,
// ano gaia, apb. apulija13, archon, aspar, basil, bad dream (proxima),
// batman 1 r1, beach volley, bedlam, blood valley, 
// bomb fusion, boovie, bounty bob jansoft, New York Warriors, Grell and Falla, Hall of the Things, Hamte Damte, 
// trap door, , SAS Operation ThunderFlash, Turbo The Tortoise, Empire Strikes Back, Lode Runner, Mickey Mouse, Micronaut One,
// Dynamite Dan 2, Explorer 48 (EDS), Hong Kong Phooey, Ice Temple, International Kickboxing, Iron Soldier, Kings Valley, 
// Night Hunter, Koronis Rift, La Balada del Duende 48, Loco, Wonderboy, Wrestle Crazy, Boovie, Ninja Grannies, 
// RocknRoll (Rainbow Arts), Rommels Revenge, R-Type, Barry McGuigan Championship Boxing, Aliens (eds)[a2],
// Camel Trophy 86, Double Dragon (DroSoft), Dustin (MediumCase), Eliminator (PlayersSoftware), Days of Thunder, 
// Dragons Of Flame (Kixx), Cobra's Arc, Welcome To Hollywood
// [] Custom (square)             Defender of the Crown, Rocky Horror Show, BMX Ninja, Room Ten, Aftermath, Defender of the Crown (different), Cybex, Bugsy,
// [] CRL                         Doctor What, Jack The Ripper,
// [] Durell                      Saboteur
// [] BrainSport, Hijack (1986)(electric dreams software)

// [5/0] Polarity (+)             Lone Wolf 3, Basil Mouse Detective, Mask, DarkStar, Wizball.tap (PZXTools)
// [0/2] Polarity (-)             Forbidden Planet (V1,V2), lonewolf 3 48
//                                try also: hudson hawk,starbike
// [6/0] Longer pilots (x1.0250)  The Untouchables (TheHitSquad), Dogfight 2187, ATF, TT Racer, Lightforce, Magmax, Shockway Rider
// [2/0] Shorter syncs            Italy 1990 (Winners Edition), 
// [3/0] Longer pauses (x1.03)    hijack (EDS) 128, italy 1990 winners, dogfight 2187

// unknown
// [0/0] Forbidden Planet (Zeppelin) ^C
// tapes with likely stop-the-tape issues:
// [0/0] the munsters.tzx
// large tapes that need a smart tape handler:
// [0/0] Gauntlet 2(Kixx) ^C
// [0/0] TurboTheTortoise ^C
// [0/0] X-Out (Kixx)
// [0/0] Karnov (1988)(ElectricDreamsSoftware)(48-128k)
// [0/0] SlySpy(TheHitSquad)
// [0/0] YourSinclair(Issue65)
// [0/0] Crash(Issue91)


// bad dumps?
// [1/1] st dragon(kixx).tzx
// [1/1] Freddy Hardest - Part 1.tzx
// [1/1] Krakatoa (Paxman) (with rom trap loading only?)

// problematic tapes:
// [1/1] Macadam Bumper (EreInformatique).tzx (wont load in 128/+2 models. works only in 48/+3 computers in 48k mode)
// [0/0] IronLord [0]
// [0/0] Killer III ^C [SHIFT]

// ---
// [0/0] strategic defence initiative (1998)(activision)(speedlock 4).tzx
// [0/0] Dizzy 3 - Fantasy World Dizzy (1989)(Codemasters).tzx
// [0/0] L'Abbaye des Morts ^C
// [0/0] Isotopia 128 [SIDE B]
// [0/0] StarStrike II (Cobra).tzx
// [0/0] DanDare
// [0/0] Gutz
// [0/0] hudson hawk.tzx
// [0/0] TaiChi Tortoise
// [0/0] Short Circuit 128k (TheHitSquad).tzx (non-2A+)
// [0/0] StormFinch
// [0/0] StormLord 2 Deliverance.tzx
// [0/0] Street Hassle (1988)(MelbourneHouse).tzx
// [0/0] Viz The Computer Game
// [0/0] Ghostbusters (128k)(Speedlock2)
// [0/0] Gyron Arena
// 48k only
// [0/0] StarFarce 48k
// [0/0] World Cup Football 48k
// [0/0] Runestone 48k
// [0/0] Pinball 48k
// [0/0] AlterEgo 48k
// [0/0] TurboEsprit 48k
// [0/0] Orion 48k ^C
// [0/0] GhostHunters 48k
// [0/0] fred's fan factory 48
// extra bits:
// [0/0] hercules (byteback) ^C

// extra loaders:
// [0/0] Temple of Terror
// [0/0] Leader Board Par 3
// [0/0] StarControl.tzx
// [0/0] Switchblade ^C
// [0/0] Marauder ^C
// [0/0] 1942 ^C
// [0/0] Vectron 3D ^C
// [0/0] Wrestling Superstars (1993)(Codemasters) ^C
// [0/0] Halloween ^C
// [0/0] LotusEspritTurboChallenge ^C

// [0/0] SplATTR

// speedlock
// [0/0] time machine(activision).tzx
// [0/0] star paws.tzx
// [0/0] skull & crossbones (thehitsquad).tzx
// [0/0] the ninja warriors.tzx

// no: bloodwych

// try: tk90x turbo timings (R.G.)
// PILOT = 1408, n_pilot = 4835, guess = 4835 / (5/2) = 1934, SYNC1 = 397, SYNC2 = 317, ZERO = 325, ONE = 649, END_MS = 318,

// [?][?][?][ ] turborom
// [?][?][?][x] turborom+coliseum.tap

// tapes that require cpu-driven ticks:
// [ ][ ][ ][ ] 1942.tzx(with trainer)
// [ ][ ][ ][ ] nosferatu(alternativeltd)
// [x][*][ ][ ] express raider ; needs longer pilots
// [*][*][*][ ] Italy1990(Winners) ; needs longer pilots
// [x][ ][x][*] ForbiddenPlanetV1(-), ForbiddenPlanetV2(-) (before loading screen)

// games that needs tape stopped while cpu is busy
// [ ][x][ ][ ] gauntlet.tzx
// [ ][x][ ][ ] mythhistoryinthemaking(kixx).tzx
// [ ][ ][ ][ ] hudsown hawk.tzx
// [ ][x][ ][ ] cauldron2.tap (press-any-key)
// [ ][x][ ][ ] EggThe (press-any-key)
// [x][x][*][*] cauldron(silverbird)

// pauses
// [ ][x][ ][ ] doctum (intro)
// [ ][ ][ ][ ] Barbarian(MelbourneHouse).tzx (intro)
// [ ][x][ ][ ] coliseum.tap (intro)
// [ ][x][ ][ ] jmeno ruze.tap (decompression)
// [ ][x][ ][ ] moon and the pirates.tap (decompression)
// [ ][ ][ ][ ] hijack (EDS) 128
// [ ][x][*][ ] italy 1990 winners
// [ ][x][x][!] dogfight 2187
// [ ][ ][ ][ ] hudson hawk

// issue2/3 loading issues
// [x][x][ ][ ] Wizball(pzxtools).tap(+)
// [ ][ ][ ][ ] LoneWolf3SideA128(+), LoneWolf3SideB48(+)
// [ ][ ][ ][ ] MASK(+), Basil(+)
// [ ][x][ ][ ] KoronisRift(+)
// [x][ ][x][x] MASK(IBSA)(+)(-)
// [?][?][*][x] ForbiddenPlanet(-)

// issue2/3 keyboard issues
// [ ][ ][ ][ ] abusimbel(gremlin) i2 + polarity level at end-of-tape
// [ ][ ][ ][ ] spynads issue2
// [ ][ ][ ][ ] rasputin 48k issue2

// 70908/69888 compensation
// [x][x][x][ ] La Abadia del Crimen (5ExitosOpera)

// auto-stop is broken
// [ ][ ][ ][ ] basil(+)
// [ ][ ][ ][ ] abadia
// [ ][ ][ ][ ] untouchables(hitsquad)
// [ ][ ][ ][ ] jack2(kixx)
// [ ][ ][ ][ ] hijack (final stop)
// [ ][x][x][!] dogfight

// Crashes
// [x][ ][ ][ ] abadiadelcrimen.tzx crashes towards end of tape
// [?][ ][ ][ ] Untouchables (hitsquad)

// pilots
// [?][x][ ][ ] Untouchables (hitsquad)
// [?][ ][ ][ ] Lightforce
// [?][ ][ ][ ] ATF 48
// [?][ ][ ][ ] TT Racer 48
// [?][ ][ ][ ] Explorer (EDS) 48

// stop
// [?][ ][ ][ ] oddi the viking
// [?][x][ ][ ] untouchables hitsquad
// [?][ ][ ][ ] batman the movie hitsquad

// edos: tapes use additional non-standard padding bytes in headers (19->29), making the EAR routine to loop pretty badly
// [?][?][?][x] streecreedfootball(edos).tzx
// [?][?][?][x] beyondtheicepalace(edos).tzx
// [?][?][?][x] elvenwarrior(edos).tzx
