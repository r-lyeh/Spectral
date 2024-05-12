# Spectral <img src="src/res/img/noto_1f47b.png" width="5%" height="5%" />
[![](https://github.com/r-lyeh/Spectral/actions/workflows/build.yml/badge.svg)](https://github.com/r-lyeh/Spectral/actions/workflows/build.yml)

Sinclair ZX Spectrum emulator from the 80s.

![image](https://github.com/r-lyeh/spectral/assets/35402248/8ae5f8d4-0a7c-41ee-9112-2e86bacdb262)

## About
Spectral is an experimental emulator that I have been randomly assembling [since the pandemic days](https://twitter.com/r_rlyeh/status/1280964279903158273), inspired by my old fZX32 emulator.
Hardcore ZX users will find little value in this emulator right now, but I hope newbies may find its ease of use somehow appealing to try.
That being said, Spectral has a very compatible TAP/TZX loader and some other interesting features that provide me some fun in these days.
Code is highly experimental and prone to change in the future. I will keep altering/breaking things randomly for good sake.

## Features and wishlist
- [x] Single executable.
- [x] Z80. Z80 Disassembler.
- [x] 16, 48/128, +2, +2A, +3/FDC.
- [x] Issue 2/3.
- [x] ULA/ULA+.
- [x] Beeper/AY.
- [x] Kempston mouse. <!-- @todo: AMX mouse.-->
- [x] Kempston/Fuller/Cursor joysticks.
- [x] RF/CRT experience (not physically accurate though).
- [x] TAP/TZX/DSK/Z80/SNA/ROM/IF2/SCR/ZIP support. <!-- @todo: tzx info on window title -->
- [x] SCR/PNG screenshots.
- [x] Load games easily. Turbo loads.
- [x] Game browser. <!-- @todo: rewrite this -->
- [x] Graphical tape browser.
- [x] 50Hz fps lock.
- [x] Run-a-head.
- [x] POK support.  <!-- @todo: cheats finder --> 
- [x] Gunstick, Lightgun. <!-- Cheetah Defender Lightgun, Magnum Light Phaser, Stack Light Rifle -->
- [x] External shaders support.
- [x] Internal savestates.
- [ ] Cycle accurate (border, multicolor, etc).
- [ ] Extra accurate Z80 backend. <!-- @todo: contended mem, contended ports, memptr, snow, Q, floating bus (+2a/+3) -->
- [ ] Pentagon 128K + beta disk. SCL/TRD.
- [ ] RZX support. <!-- @todo: rzx loadsave http://ramsoft.bbk.org.omegahg.com/rzxform.html -->
- [ ] ZXDB integration.
- [ ] Graphical User Interface. <!-- mouse driven -->
- [ ] Gallery marquee. <!-- Flex. Tape cases. ZX catalog on demand. -->
- [ ] Gamepad support. <!-- Invert joystick/mouse axes/buttons -->
- [ ] MP3s.
- [ ] Netplay.
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
- f8: toggle tape speed
- f9: toggle tv/rf
- f11/f12: quick save/load 
- alt+enter: fullscreen
- tab+cursors: joysticks

## Credits
Andre Weissflog, for their many single-header libraries! (Zlib licensed). Peter Sovietov, for their accurate AY chip emulator (MIT licensed). Ulrich Doewich and Colin Pitrat, for their uPD765A floppy disk controller (GPL licensed). lalaoopybee, for their lovely tube shader. Andrew Owen and Geoff Wearmouth for their custom ROMs. Santiago Romero, Philip Kendall, James McKay for their FOSS emulators. All the ZX community!

## Unlicense
This software is released into the [public domain](https://unlicense.org/). Also dual-licensed as [0-BSD](https://opensource.org/licenses/0BSD) or [MIT (No Attribution)](https://github.com/aws/mit-0) for those countries where public domain is a concern (sigh). Any contribution to this repository is implicitly subjected to the same release conditions aforementioned.

## Links
- [Introduction to the ZX](https://en.wikipedia.org/wiki/ZX_Spectrum), ZX entry on Wikipedia.
- [SpecEmu](https://specemu.zxe.io/), my favourite ZX emulator on Windows.
- [SpectrumComputing](https://spectrumcomputing.co.uk/), [WorldOfSpectrum](https://worldofspectrum.net/), [ZXArt](https://zxart.ee/) and [ZXInfo](https://zxinfo.dk/) are best online resources (imho).
- [Crash](https://archive.org/details/crash-magazine), [YourSinclair](https://archive.org/details/your-sinclair-magazine), [SinclairUser](https://archive.org/details/sinclair-user-magazine) and [MicroHobby(ES)](https://archive.org/details/microhobby-magazine) are great old paper magazines.
- [ZX database](https://github.com/zxdb/ZXDB), [game maps](https://maps.speccy.cz/), [game cheats](https://www.the-tipshop.co.uk/) and [game longplays](https://www.youtube.com/@rzxarchive).
- [Daily ZX videos](https://www.youtube.com/results?search_query=zx+spectrum&sp=CAI%253D), on YouTube.
