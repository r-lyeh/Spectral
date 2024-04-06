# Spectral <img src="src/img/noto_1f47b.png" width="5%" height="5%" />
Sinclair ZX Spectrum emulator from the 80s.

![image](https://github.com/r-lyeh/spectral/assets/35402248/24d23ee0-8203-4880-bb95-10e388a873c5)

## About
Spectral is an experimental emulator that I have been randomly assembling [since the pandemic days](https://twitter.com/r_rlyeh/status/1280964279903158273), inspired by my old fZX32 emulator.
Hardcore ZX users will find little value in this emulator right now, but I hope newbies may find its ease of use somehow appealing to try.
That being said, Spectral has a very compatible TAP/TZX loader and some other interesting features that provide me some fun in these days.
Code is highly experimental and prone to change in the future. I will keep altering/breaking things randomly for good sake.

## Features and wishlist
- [x] Single executable.
- [x] 16, 48/128, +2, +2A, +3/FDC.
- [x] Z80/Beeper/AY.
- [x] Issue 2/3.
- [x] ULA/ULA+.
- [x] Kempston mouse. <!-- @todo: AMX mouse.-->
- [x] Kempston/Fuller/Cursor joysticks.
- [x] RF experience (not physically accurate though).
- [x] TAP/TZX/DSK/Z80/SNA/ROM/IF2/SCR/ZIP support. <!-- @todo: tzx info on window title -->
- [x] Load games easily.
- [x] Game browser. <!-- @todo: rewrite this -->
- [x] Graphical tape browser.
- [x] 50Hz fps lock.
- [x] Run-a-head.
- [ ] Pentagon 128K + beta disk. SCL/TRD.
- [ ] Savestates. <!-- @todo: savefile spec -->
- [ ] Netplay.
- [ ] RZX support. <!-- @todo: rzx loadsave http://ramsoft.bbk.org.omegahg.com/rzxform.html -->
- [ ] ZXDB integration.
- [ ] Graphical User Interface.
- [ ] Cheats finder. POK support.
- [ ] Extra accurate Z80+AY backends. <!-- @todo: contended mem, contended ports, memptr, snow, Q, floating bus (+2a/+3) -->
- [ ] Cycle accurate (border, multicolor, etc).
- [ ] Gallery marquee. <!-- Flex. Tape cases. ZX catalog on demand. -->
- [ ] Shaders support. <!-- Barrel/CRT effects. -->
- [ ] Gamepad support. <!-- Invert joystick/mouse axes/buttons -->
- [ ] Portable.
- [ ] Optimized.
- [x] Unlicensed.

## Usage
- esc: game browser
- f1: speed boost (hold)
- f2: start/stop tape
- f3/f4: rewind/advance tape
- f5: restart
- f6: toggle input latency (runahead)
- f7: toggle tape polarity
- f8: toggle fast tape
- f9: tv rf mode
- f11/f12: quick save/load 
- alt+enter: fullscreen
- tab+cursors: joysticks

## Credits
- [x] Andre Weissflog, for SokolAudio and their lovely Z80/AY/Beeper single-header chip emulators (Zlib licensed).
- [x] Peter Sovietov, for their accurate AY chip emulator (MIT licensed).
- [x] Ulrich Doewich and Colin Pitrat, for their uPD765A floppy disk controller (GPL licensed).

## Unlicense
This software is released into the [public domain](https://unlicense.org/). Also dual-licensed as [0-BSD](https://opensource.org/licenses/0BSD) or [MIT (No Attribution)](https://github.com/aws/mit-0) for those countries where public domain is a concern (sigh). Any contribution to this repository is implicitly subjected to the same release conditions aforementioned.

## Links
- [Introduction to the ZX](https://en.wikipedia.org/wiki/ZX_Spectrum), ZX entry on Wikipedia.
- [SpecEmu](https://specemu.zxe.io/), my favourite ZX emulator on Windows.
- [SpectrumComputing](https://spectrumcomputing.co.uk/), [WorldOfSpectrum](https://worldofspectrum.net/), [ZXArt](https://zxart.ee/) and [ZXInfo](https://zxinfo.dk/): best online resources (imho).
- [Crash](https://archive.org/details/crash-magazine), [YourSinclair](https://archive.org/details/your-sinclair-magazine), [SinclairUser](https://archive.org/details/sinclair-user-magazine) and [MicroHobby(ES)](https://archive.org/details/microhobby-magazine): old paper magazines.
- [ZX database](https://github.com/zxdb/ZXDB), [game maps](https://maps.speccy.cz/), [game cheats](https://www.the-tipshop.co.uk/) and [game longplays](https://www.youtube.com/@rzxarchive).
- [YouTube](https://www.youtube.com/results?search_query=zx+spectrum&sp=CAI%253D), daily ZX videos.
