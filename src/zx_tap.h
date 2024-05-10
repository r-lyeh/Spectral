// fixme!
// - [ ] voc renderer is mem hungry
// - [ ] auto-tape (taps): madmix2, p47, gauntlet, batman caped (glue block)
// - [ ] auto-tape / press key: the egg,
// - [x] extra pause: jmeno ruze, explorer, hijack, italy 90
// - [ ] stop the tape blocks: diver, doctum, 
// - [ ] tape errors: express raider,
// - [ ] devices: enterprise,
// - [ ] rom: macadam bumper,
// - [ ] bad tapes? lonewolf128.tap, krakatoa.tzx, st dragon (kixx).tzx

// refs:
// - http://ramsoft.bbk.org.omegahg.com/mtzxman.html#HISTORY
// - https://www.alessandrogrussu.it/loading/schemes/Schemes.html
// - http://newton.sunderland.ac.uk/~specfreak/Schemes/schemes.html
// - https://faqwiki.zxnet.co.uk/wiki/Loading_routine_%22cores%22
//
// [-/+] Failures | Loader Name | Games | Total (-) 9 tape errors, (+) 9 tape errors
// [ / ] _unknown_                Myla Di'Kaich
// [ / ] _unknown_                Twister
// [ / ] _unknown_                Zanthrax
// [ / ] Biturbo                  Special Program, Playgames
// [ / ] NN:Hollywood Poker       Hollywood Poker
// [ / ] Odeload/UnilODE          Trivial Pursuit (HitSquad), (Erbe), ... Genus Edition Question Tape Side A, Trivial Pursuit Baby Boomer Edition Question Tape side B, Trivial Pursuit Young Players Edition Question Tape
// [0/0] _unknown_                Falcon Patrol 2
// [0/0] Activision               Time Scanner ^C, Pac-Land, Dynamite Dux, Ninja Spirit
// [0/0] Alkatraz                 720, Artura, Legend of Kage, bobby bearing, Brave starr, fairlight 128k, fairlight 2 128k, hate, rolling thunder, Alien Syndrome 48 (most Kixx and US Gold releases)
// [0/0] Alkatraz 2               Outrun Europa, Gauntlet 3, Strider 2 (US Gold) ^C
// [0/0] BleepLoad                Black Lamp, Bubble Bobble, I,Ball; Brainstorm (Firebird), Earthlight, Flying Shark, Jaws, Kinetik, The Plot, Rick Dangerous, Soldier of Fortune, Thrust II 48, Ninja Scooter, Zolyx
// [0/0] Busy soft                Jet-Story, Double Dash, Kliatba noci, Quadrax
// [0/0] Cyberlode 1.1            Cauldron (Silverbird), Antiriad 48 (Silverbird)
// [0/0] Digital Integration      ATF 48, TT Racer 48 (4 Aces), Tomahawk (4 Aces)
// [0/0] Dinaload                 Capitan Trueno, Grand Prix Master, Satan, Cosmic Sheriff, Freddy Hardest En Manhattan Sur, Michel Futbol Master
// [0/0] EDOS                     Beyond Ice Palace (EDOS), Bomb Jack 2 (EDOS), Street Cred Football (EDOS)
// [0/0] Elite                    Kokotoni Wilf 48, Dukes of Hazzard 48,
// [0/0] Excelerator              Loads of Midnight, Last Mohican, Jack the Ripper
// [0/0] Flash Load               Dan Dare, Great Fire of London, Cliff Hanger 48, Strangeloop,
// [0/0] FTL                      Thundercats, Supertrux
// [0/0] Gargoyle                 Heavy on the Magick, Lightforce (Rack-It)
// [0/0] Gremlin                  Metabolis, Monty on the Run, DeathWish3, Grumpy Gumphrey Super Sleuth, Way of the Tiger, Bounder, Jack the Nipper, Avenger, Future Knight
// [0/0] Gremlin 2                Basil the Mouse Detective (10 Great Games 2), Mask, Mask (10 Great Games 2)
// [0/0] Haxpoc-Lock              Star Wars (Domark)
// [0/0] Hyper-Loading            LaserWARp, Air Traffic Control
// [0/0] Injectaload              Book of the Dead (CRL), Ninja Hamster, Outcast, (3D Game Maker?)
// [0/0] Jet-Load                 Classroom Chaos, Dungeon Dare, Prelude, The Greatest Show on Earth
// [0/0] Jon North                YS Poke Tapes
// [0/0] LazerLoad 48             Macadam Bumper (PSS) 48 ^C
// [0/0] Lerm                     Lerm Microdrive 1 - Side A, Lerm Microdrive 1 - Side B, Lerm Tape Copier 6, Lerm Tape Copier 7 48
// [0/0] LoadA-Game               Joe Blade 2
// [0/0] Micromega                Braxx Bluff, Kentilla 48, Jasper, A Day in the Life, Glass(BugByte),
// [0/0] Microprose               Times of Lore, Xenophobe
// [0/0] Microsphere              Skool Daze, Sky Ranger, Back to Skool
// [0/0] Mikro-Gen                Automania, Pyjamarama, Witch's Cauldron, Everyone's a Wally, Herbert's Dummy Run, Shadow of the Unicorn, Three Weeks in Paradise (both), Battle of the Planets, Equinox, Stainless Steel, Frost Byte, Cop-Out
// [0/0] Movieload                Moonstrike 48
// [0/0] NN:Moonlighter           Moonlighter
// [0/0] NN:Roller Coaster        Roller Coaster
// [0/0] NN:Worldcup              World Cup Football 48
// [0/0] Novaload 48              Swords & Sorcery 48, Covenant
// [0/0] Nu-Load Ninety One       The Hunt for Red October
// [0/0] ODEload                  Trivial Pursuit Genus Edition Question Tape side B, Sailing, Sailing (Mastertronic), Sailing (Mastertronic Plus)
// [0/0] Paul Owens System        Batman the Movie, Cabal, Operation Thunderbolt, The Untouchables, Chase HQ, Rainbow Islands, Red Heat (latest Ocean games)
// [0/0] Players 1                Xanthius, Deviants, Fernandez Must Die, Fox Fights Back, Tomcat, Street Gang, Task Force, Street Cred' Football, Spooked, Cobra Force, Prohibition, Denizen, Elven Warrior, Saigon Combat Unit, Joe Blade 3, Prison Riot
// [0/0] Players 2                Joe Blade, Andy Capp, Tetris, Thing!, Tanium, Shanghai Karate, Metal Army, Sword Slayer, PowerPlay
// [0/0] Poliload (DinamicLoader) Astro Marine Corps, Rescate Atlantida (Rescue From Atlantis)
// [0/0] PowerLoad 48             Arena, Boulderdash, Confuzion, Deathwake, Dynamite Dan, SpyVsSpy, Tantalus
// [0/0] Proxima Software         Orion 48, Inferno 48,
// [0/0] Rapid                    Travel With Trashman, Zombie Zombie
// [0/0] Really-quite-fast-loader Gary Lineker's Superskills
// [0/0] Richlock                 Light Force 48, Light Force (Rack-It), Hydrofool (Rack-It), 
// [0/0] SearchLoader             The Final Matrix, Ranarama ^C, City Slicker
// [0/0] Sentient                 Tai-Pan, A Question of Scruples, Guerrilla War
// [0/0] SetoLoad                 SL-multi-test.tzx
// [0/0] Softlock                 Chimera 48k, Rasputin 48k, Skyfox, PHM Pegasus, Elite 48k (Firebird), Cylu 48k, Impossible Mission 2
// [0/0] Software Projects        BC's Quest for Tires 48k, Learning with Leeper
// [0/0] Speedlock 1              Alien 8, Batman, Beach Head, Blue Max 48, Cyclone 48, Decathlon 48, Gilligan's Gold 48, Match Day 48, Mikie 48, NOMAD 48, Rambo 48, Spy Hunter 48, Yie Ar Kung Fu 48
// [0/0] Speedlock 1/2 Hybrid     Highlander
// [0/0] Speedlock 2              Alien Highway, Enduro Racer, The Great Escape, Green Beret, Head Over Heels 128k, Tarzan, Yie Ar Kung Fu 2 48
// [0/0] Speedlock 3              Leviathan [ENTER], Dogfight 2187, Triaxos
// [0/0] Speedlock 4              Athena 128, Road Runner, Mutants, The Ninja Warriors, Slap Fight, Tai-Pan, Wizball
// [0/0] Speedlock 5              Road Blasters, Action Force 2, Outrun, Hysteria, Gryzor, Phantom Club, Slaine
// [0/0] Speedlock 6              The Fury, Platoon (Release1), Super Hang-On (Proein), MatchDay II (TheHitSquad), Vixen
// [0/0] Speedlock 7              The Addams Family, Arkanoid 1/2, Batman The Caped Crusader, Target Renegade, WTSS, Vindicator, Robocop2, Typhoon, Simpsons, TimeMachine (most TheHitSquad releases)
// [0/0] Speedlock 8 ?            Robocop 3
// [0/0] Speedlock Associates     Dan Dare 2
// [0/0] Sprite Novaload          Theatre Europe
// [0/0] The Edge                 Starbike, That's the Spirit, Psytraxx, Brian Bloodaxe
// [0/0] Uniloader                Batty ^C, IkariWarriors ^C, Bomb Jack 2
// [0/0] ZetaLoad                 Wec Le Mans (Imagine) [1]
// [0/0] Zydroload                The Light Corridor, North & South, Magic Johnson,
// [0/0] Multiload                Blood Brothers, Deflektor, 
// []                             Donkey Kong (Erbe), Bazooka Bill (Erbe/IBSA), Cobra (Erbe/IBSA), Conquestador (Erbe), 
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
// [] ERBE                        Conquestador,
// [] The Sales Curve             St Dragon, Continental Circus,
// [] Mirrorsoft                  Mean Streak,
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
// Dragons Of Flame (Kixx), Cobra's Arc,
// [] Custom (square)             Defender of the Crown, Rocky Horror Show, BMX Ninja, Room Ten, Aftermath, Defender of the Crown (different), Cybex, Bugsy,
// [] CRL                         Doctor What, Jack The Ripper,
// [] Durell                      Saboteur
// [] BrainSport, Hijack (1986)(electric dreams software)

// [5/0] Polarity (+)             Lone Wolf 3, Basil Mouse Detective, Mask, DarkStar, Wizball.tap (PZXTools)
// [0/2] Polarity (-)             Forbidden Planet (V1,V2),
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

// ---
// tzx(csw) starbike1, starbike2
// tzx(gdb) basil great mouse, bc quest for tires, book of the dead part 1 (crl), dan dare 2 mekon, world cup carnival
// tzx(flow) hollywood poker, bubble bobble (the hit squad)
// tzx(noise) leaderboard par 3, tai-pan, wizball
// tzx(voc) catacombs of balachor
// tzx(???) bathyscape

#define voc_pos RAW_fsm

float DELAY_PER_MS = 3500; // 69888/20; // 3500; // (ZX_TS*50.06/1000) // (ZX_TS*50.6/1000)
enum { PILOT = 2168, DELAY_HEADER = 8063, DELAY_DATA = 3223, SYNC1 = 667, SYNC2 = 735, ZERO = 855, ONE = 1710, END_MS = 1000 };

#define AZIMUTH_DEFAULT 1.002501 // + 1.02631; // + 1.0450; // 0.9950; // 0.9950; // 0.9937f; // 0.9937 1.05 both fixes lonewolf+hijack(1986)(electric dreams software).tzx
float azimuth = AZIMUTH_DEFAULT;

byte        mic,mic_on,mic_has_tape;
int         mic_last_block;
const char *mic_last_type = "";
char        mic_preview[320+1];
int         mic_invert_polarity = 0;
int         mic_low;

int         RAW_repeat, RAW_pulse;
int         RAW_tstate_prev, RAW_tstate_curr, RAW_tstate_goal;
int         RAW_blockinfo;
uint64_t    RAW_fsm;

struct mic_queue_type {
    const char *debug;
    int count;
    int pulse;
    int block;
} *mic_queue = 0, *mic_turbo = 0;
int mic_queue_rd = 0, mic_queue_wr = 0;
bool mic_queue_has_turbo;

byte *mic_data = 0; // do not free
int mic_datalen = 0;

byte *voc = 0;
unsigned int voclen = 0;
unsigned int voccap = 1;

void voc_init(int reserve) {
    voc = realloc(voc, voccap = reserve);
    voclen = 0;
}
void voc_push(byte x, int count) {
    if( (voclen+count) >= voccap ) {
        voccap = (voclen+count) * 1.75;
        voc = realloc(voc, voccap);
        if (!voc) exit(-printf("Out of mem %d bytes\n", voccap));
    }
    memset(&voc[voclen], x, count);
    voclen += count;
}


void mic_block_info(int number) {
    RAW_blockinfo = number;
    mic_has_tape = 1;
}

void mic_render_pilot(float count, float pulse) {
    // longer pilots (x1.0250) fixes: untouchables (hitsquad), lightforce, ATF, TT Racer, Explorer (EDS)...
    pulse *= 1.0250;
    // PILOT
    mic_queue[mic_queue_wr].debug = "pilot";
    mic_queue[mic_queue_wr].block = RAW_blockinfo;
    mic_queue[mic_queue_wr].count = count;
    mic_queue[mic_queue_wr++].pulse = pulse; assert(pulse > 0);
}
void mic_render_sync(float pulse) {
    // SYNC1 or SYNC2 (usually)
    mic_queue[mic_queue_wr].debug = "sync";
    mic_queue[mic_queue_wr].block = RAW_blockinfo;
    mic_queue[mic_queue_wr].count = 1;
    mic_queue[mic_queue_wr++].pulse = pulse; assert(pulse > 0);
}
void mic_render_data(byte *data, unsigned bytes, unsigned bits, unsigned zero, unsigned one) {
    // COPY
    mic_data = realloc(mic_data, (mic_datalen += bytes));
    memcpy(mic_data + mic_datalen - bytes, data, bytes);
    // DATA
    for( ; bytes-- > 0; ++data ) for( int i = 0; i < 8; ++i ) {
    mic_queue[mic_queue_wr].debug = "data";
    mic_queue[mic_queue_wr].block = RAW_blockinfo;
    mic_queue[mic_queue_wr].count = 2;
    mic_queue[mic_queue_wr++].pulse = ((*data) & (1<<(7-i)) ? one : zero);
    }
    // truncate latest bits
    mic_queue_wr -= 8 - bits;
}

void mic_render_pause(unsigned pause_ms) {
    // fix a few .tap files that should be .tzx instead: jmeno ruze.tap, moon and the pirates.tap ...
    // .tap files have a fixed pause length of 1000ms (1s). some games need some more time than 1s
    // to process the last loaded block (like when decompressing). thus, during this processing, real
    // speccies will keep their tapes running, and for when CPU is ready, tape may have advanced over
    // the next leading tones too much as to break the loading process.
    // note: emulators with rom trap loading will never issue this.
    pause_ms += (pause_ms == END_MS) ? 1500 : 0; // give extra time to some .tap files; this gap could belong to .tzx files too, but it wont matter in this case

    // fix hijack (EDS) 128, italy 1990 winners, dogfight 2187
    pause_ms *= 1.03; // 70908 vs 69888

    // @fixme: stretch longer pauses if turborom is enabled (x6?) see: 1942.tzx + turborom, or topo games

    // END PILOT
    if(pause_ms) {
    mic_queue[mic_queue_wr].debug = "pause1";
    mic_queue[mic_queue_wr].block = RAW_blockinfo;
    mic_queue[mic_queue_wr].count = 1;
    mic_queue[mic_queue_wr++].pulse = DELAY_PER_MS;

    mic_queue[mic_queue_wr].debug = "pause";
    mic_queue[mic_queue_wr].block = RAW_blockinfo;
    mic_queue[mic_queue_wr].count = 1;
    mic_queue[mic_queue_wr++].pulse = pause_ms * DELAY_PER_MS;
    }
}

void mic_render_stop(void) { // REV
    mic_render_pause(1);
//    return;

    // oddi the viking
    // untouchables hitsquad
    // batman the movie
    // express raider

    mic_queue[mic_queue_wr].debug = "stop";
    mic_queue[mic_queue_wr].block = RAW_blockinfo;
    mic_queue[mic_queue_wr].count = 1;
    mic_queue[mic_queue_wr++].pulse = DELAY_PER_MS; //0;
}

void mic_render_full(byte *data, unsigned bytes, unsigned bits, float pilot_len, unsigned pilot, unsigned sync1, unsigned sync2, unsigned zero, unsigned one, unsigned pause) {
    mic_render_pilot(pilot_len, pilot);
    mic_render_sync(sync1);
    mic_render_sync(sync2);
    mic_render_data(data, bytes, bits, zero, one);
    mic_render_pause(pause);
}

void mic_render_standard(byte *data, unsigned bytes, float pilot_len) {
    mic_render_full(data, bytes, 8, pilot_len, PILOT, SYNC1, SYNC2, ZERO, ONE, END_MS);
}


// static int autotape_counter, autotape_indicators;

byte ReadMIC(int tstates) {
    if(mic_queue_rd >= mic_queue_wr) return mic_on = 0, mic; // end of tape

    unsigned pc = PC(cpu);
#if 1
    bool rom1 = ZX==48 || (ZX == 128 && (page128 & 16));
    bool loading_from_rom = rom1 && (pc >= 0x04C2 && pc < 0x09F4); // (pc == 0x562 || pc == 0x5f1);
    if(!mic_on) {
        if( loading_from_rom && mic_has_tape ) {
            mic_on = 1;
        }
        return 0;
    }
#else

// gather machine stats
static int ts_prev; int ts = tstates - ts_prev; ts += ts < 0 ? TS : 0; ts_prev = tstates;
static unsigned pc_avg, pc0, pc1, pc2; pc0 = pc1; pc1 = pc2; pc2 = pc;
static unsigned ts_avg, ts0, ts1, ts2; ts0 = ts1; ts1 = ts2; ts2 = ts_prev;
static unsigned last_fe_;

autotape_indicators = 0;

bool rom1 = ZX==48 || (ZX == 128 && (page128 & 16));
bool loading_from_rom = rom1 && (pc >= 0x04C2 && pc < 0x09F4); // (pc == 0x562 || pc == 0x5f1);
autotape_indicators |= 0x10000 * !!(loading_from_rom && mic_has_tape);
autotape_indicators |= 0x01000 * !!((last_fe_&~64) == (last_fe&~64));    // keyboard pressed? border changed? beeper started to change? 
autotape_indicators |= 0x00100 * !!(abs(pc-pc_avg) < 0x1000);        // large jump in pc? changed from loading routine to game likely?
autotape_indicators |= 0x00010 * !!(abs(ts-ts_avg) > 100);           // changed timing? did we stop polling EAR/MIC?
autotape_indicators |= 0x00001 * !!(mic_queue[mic_queue_rd].debug[2] != 'u' && mic_queue[mic_queue_rd].debug[2] != 'o'); // pa(u)se st(o)p
                                                // changed ay rw/ro?
                                                // changed 1/7ffd rw/ro? page128
                                                // changed mouse rw/ro?
                                                // changed kempston rw/ro?
                                                // changed fdc rw/ro?


last_fe_ = last_fe;
pc_avg = (pc0+pc1+pc2)/3;
ts_avg = (ts0+ts1+ts2)/3;

// auto-tape on
    if(!mic_on) {
        if( loading_from_rom && mic_has_tape ) {
            autotape_counter = INT_MAX;
        }
    }

#if 0
// auto-tape off
    if(mic_on) {  
        // increase aritmetically (slow), decrease geometrically (fast)
        if((autotape_indicators&1
            + autotape_indicators&0x10
            + autotape_indicators&0x100
            + autotape_indicators&0x1000)
         < 2) autotape_counter = (autotape_counter * 0.95) - 1; // stop tape if at least 2 indicators found
    }
#endif

if(autotape_counter>100) autotape_counter = 100;
if(autotape_counter<0) autotape_counter = 0;

if(autotape_counter<25) mic_on=0;
if(autotape_counter>75) mic_on=1;

#endif

#if 1
    if( !mic_on ) {
        return mic;
    }
#endif

#if 0
    //
    if(patched_rom) {
        bool turbo_loading = false; // /*mic_queue[mic_queue_rd].bits != 8 ||*/ (mic_queue[mic_queue_rd].level && mic_queue[mic_queue_rd].pulse > PILOT);
        if( turbo_loading || !loading_from_rom ) {
            puts("Turbo loading detected, reverting turborom patch");
            void rom_patch(int);
            rom_patch(0);
            loading_from_rom = 0;
        }
    }
#endif

    bool rom_turbo = patched_rom && loading_from_rom;
    struct mic_queue_type *queue = rom_turbo ? mic_turbo : mic_queue;

    mic_last_type = queue[mic_queue_rd].debug;
    mic_last_block = queue[mic_queue_rd].block;

    // needed?
    // if(queue[mic_queue_rd].pulse<=0) return mic_on = 0; // stop the tape
repeat:;
    if( RAW_fsm == 0 ) {

    mic_low = mic_invert_polarity ? 0 : 64; // <-- polarity sensitive: 64: fixes hudson hawk,starbike, 0: fixes lonewolf,basil,mask,
    mic = mic_low;
    voc_init(128 * 1024 * 1024); // @fixme: 128 mib seems excessive
    for( int i = 0; i < mic_queue_wr; ++i ) {
        int count = queue[i].count;
        for( int j = 0; j < count; ++j ) {
            mic ^= 64;
            if( !strcmp(queue[i].debug, "pause") ) mic = mic_low;
            int pulse = queue[i].pulse; assert(pulse > 0); // * (rom_turbo ? 1.0 : azimuth);
//if( !strcmp(queue[i].debug, "pilot") ) pulse *= 1.0250; // longer pilots: breaks: italy90, fixes: untouchables (hitsquad), dogfight 2187, lightforce, ATF, TT Racer
//if( !strcmp(queue[i].debug, "sync") ) pulse -= 2; // shorter syncs: fix italy 1990 (winners), hijack (1986)(electric dreams software)
            voc_push( mic<<1 | queue[i].debug[2], (pulse / 4) + !!(pulse % 4));
        }
        // if( (i+1)==mic_queue_wr || !(i%100000) ) printf("%d/%d,%d bytes\r", i, mic_queue_wr, voclen);
    }
    printf("%d last mic\n", mic);



        RAW_fsm = 4;
        RAW_tstate_prev = tstates;
        RAW_pulse = 1;
    }
    if( RAW_fsm ) {
        int diff=tstates-RAW_tstate_prev;
        RAW_tstate_prev=tstates;
        RAW_fsm+=diff>=0?diff:69888-diff; // ZX_TS, max tstates

        mic_on = (RAW_fsm/4) < voclen;
        mic = mic_on ? (voc[RAW_fsm/4]>>1)&64 : mic; // 0
    }

    // stop tape if needed
    if(mic_on)
    if(RAW_pulse <= 0
        || strchr(/*!mic_queue_has_turbo ? "uo" :*/ "o", voc[RAW_fsm/4]&0x7f) // pa(u)se st(o)p
        ) {
        mic_on = 0;
        // advance tape for upcoming resume event
        mic_queue_rd++;
    }

    return mic;
}

void mic_finish() {
#if 0
    mic_render_stop();
    mic_queue[mic_queue_wr].debug = "";
    mic_queue[mic_queue_wr].pulse = 0;
#else
    mic_render_pause(1000);
#endif

    // convert normal to turbo
    for( int i = 0; i < mic_queue_wr; ++i ) {
        mic_turbo[i] = mic_queue[i];

        /**/ if(!strcmp(mic_turbo[i].debug, "pilot")) {
            IF_ROMHACK_FASTER_EDGES(mic_turbo[i].pulse -= 358);          // ROMHACK $5e7 x16 faster edges (OK)
            IF_ROMHACK_FASTER_PILOTS_AND_PAUSES(mic_turbo[i].count /= 6); // ROMHACK $571 x6 faster pilots/pauses (OK)
        }
        else if(!strcmp(mic_turbo[i].debug, "sync")) {
            IF_ROMHACK_FASTER_EDGES(mic_turbo[i].pulse -= 358);  // ROMHACK $5e7 x16 faster edges (OK)
        }
        else if(!strcmp(mic_turbo[i].debug, "data")) {
            IF_ROMHACK_HALF_BITS(mic_turbo[i].count /= 2);           // ROMHACK $5ca 50% eliminate dupe bits of data
            IF_ROMHACK_FASTER_EDGES(mic_turbo[i].pulse -= 358);  // ROMHACK $5e7 x16 faster edges (OK)
            IF_ROMHACK_TURBO(mic_turbo[i].pulse /= ROMHACK_TURBO);  // ROMHACK $5a5 turbo loader (OK)
        }
        else if(!strcmp(mic_turbo[i].debug, "pause")) {
            //commented because of spirits.tzx
            IF_ROMHACK_FASTER_EDGES(mic_turbo[i].pulse -= 358);          // ROMHACK $5e7 x16 faster edges (OK)
            IF_ROMHACK_FASTER_PILOTS_AND_PAUSES(mic_turbo[i].pulse /= 6); // ROMHACK $571 x6 faster pilots/pauses (OK)
        }
    }
}

void mic_reset() {
    mic_queue = realloc(mic_queue, sizeof(struct mic_queue_type) * (0x100000 * 8 + 4) );
    mic_turbo = realloc(mic_turbo, sizeof(struct mic_queue_type) * (0x100000 * 8 + 4) );
    mic_queue_rd = mic_queue_wr = 0;

    mic_on=0;
    mic=0;

    RAW_fsm = 0;
    RAW_repeat = 0, RAW_pulse = 0;
    RAW_tstate_prev=0, RAW_tstate_curr=0, RAW_tstate_goal = 0;
    RAW_blockinfo = 0;

    mic_last_type = "";
    mic_has_tape = 0;
    mic_last_block = 0;
    mic_queue_has_turbo = 0;
    mic_datalen = 0;

    memset(mic_preview, 0, sizeof(mic_preview));
}

int tap_load(void *fp, int siz) {
    mic_reset();

    if( memcmp(fp, "\x13\x00", 2) ) return 0;

    byte *begin=(byte *)fp, *pos = begin, *end=begin+siz;
    for( int block = 0; pos < end; ++block ) {
        //length in bytes
        unsigned bytes = ((pos[1]<<8) | pos[0]);
        if(bytes <= 2) break; // fix bad jet-pac.tap

        printf("tap.block %003d (%s) %u bytes\n",block,pos[2] ? "HEAD":"DATA", bytes - 2);

        //data(2s) or header(5s) block?
        mic_block_info(block);
        mic_render_standard(pos+2, bytes, pos[2] < 128 ? DELAY_HEADER : DELAY_DATA);
        pos += 2 + bytes;

        // create tape preview
        unsigned pct = (1.f * _320 * (unsigned)(pos - begin)) / ((unsigned)(end - begin));
        mic_preview[pct] |= 1;
    }

    mic_finish();
    return 1;
}

void tap_prev() { // @fixme: needs to render voc section again
    while(mic_queue_rd &&  strcmp(mic_queue[mic_queue_rd].debug,"pause")) mic_queue_rd--;
    while(mic_queue_rd && !strcmp(mic_queue[mic_queue_rd].debug,"pause")) mic_queue_rd--;
    while(mic_queue_rd &&  strcmp(mic_queue[mic_queue_rd].debug,"pause")) mic_queue_rd--;
}
void tap_next() { // @fixme: needs to render voc section again
#if 1
    while(mic_queue_rd && !strcmp(mic_queue[mic_queue_rd+1].debug,mic_queue[mic_queue_rd].debug)) mic_queue_rd++;
#else
    printf("%d: %s\n", mic_queue_rd, mic_queue[mic_queue_rd].debug);
    mic_on = 0;
    while(!strchr("uo", mic_queue[mic_queue_rd].debug[2])) mic_queue_rd++; // pa(u)se st(o)p
    printf("%d: %s\n", mic_queue_rd, mic_queue[mic_queue_rd].debug);
#endif
}
