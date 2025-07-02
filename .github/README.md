<p align="center">
<picture>
    <source media="(prefers-color-scheme: dark)" srcset="../src/res/logo/bar4.png">
    <img alt="Project Logo" src="../src/res/logo/bar1.png" width="600">
</picture>
<picture>
    <source media="(prefers-color-scheme: dark)" srcset="../src/res/logo/logowbg.png">
    <img alt="Project Logo" src="../src/res/logo/logobw.png" width="8%" height="8%">
</picture><br/>
</p>


Spectral is an experimental Sinclair ZX Spectrum emulator from the 80s, which has been randomly assembled [since the pandemic days](https://twitter.com/r_rlyeh/status/1280964279903158273). Accuracy and performance are long-term goals, but the primary focus is just having fun with this thing. Hardcore ZX users will find little value in this emulator right now, but I hope newbies may find its ease of use somehow appealing to try.
That being said, Spectral has a very compatible TAP/TZX loader and some other interesting features that provide me some fun in these days.
Code is highly experimental and prone to change in the future. I will keep altering/breaking things randomly for good sake.

<p align="center">
<img src="https://github.com/r-lyeh/Spectral/assets/35402248/8d8ee594-fafd-4538-993f-9840bf9fc245"/><br/>
<img src="https://github.com/user-attachments/assets/05de75a6-ae3c-4da8-9896-8cc89efbb5e6"/><br/>
<img src="https://github.com/user-attachments/assets/fcc20345-c619-48df-af26-e37c8af99a6c"/>
</p>

# Roadmap and features
- [ ] Extra accurate Z80 backend. <!-- @todo: memptr, snow, Q, floating bus (+2a/+3) -->
- [ ] RZX support. <!-- @todo: rzx loadsave http://ramsoft.bbk.org.omegahg.com/rzxform.html -->
- [ ] Netplay.
- [ ] Optimized.
- [x] Unlicensed.
- [x] Single executable.
- [x] Cycle stepped Z80. Z80 Disassembler. 3.5/7/14 MHz.
- [x] Cycle stepped ULA/ULA+ ultrawide/multicolor graphics. 25/30/50/60Hz fps lock.
- [x] 16, 48, 128, +2, +2A, +3, Pentagon128 models.
- [x] Issue 2/3 keyboards. Modern native/host keyboard layout while in BASIC.
- [x] Beeper/AY chips. Turbosound/Turbo-AY (Pentagon only). Waveforms.
- [x] Kempston mouse. <!-- @todo: AMX mouse.-->
- [x] Kempston/Fuller/Cursor/Sinclair joysticks. Gamepad support (Windows, Linux). <!-- @todo: invert joystick/mouse axes/buttons -->
- [x] RF/CRT experience (not physically accurate though).
- [x] TAP/TZX/PZX/CSW tapes. Z80/SNA/SZX snaps. ROM/IF2 roms.
- [x] DSK/EDSK/TRD/SCL/FDI/MGT/IMG/HOBETA disks.
- [x] SCR/PNG screenshots. MP4/MPG video recordings (no sound). <!-- @todo: ulaplus screenshots -->
- [x] ZIP/RAR/GZ archives.
- [x] AY tunes player.
- [x] MP3s. Side-B bonus tracks.
- [x] Nec µ765/Betadisk interfaces.
- [x] Auto load games. Auto play/stop tape. TurboROM.
- [x] Graphical tape browser.
- [x] Improved input latency via Run-Ahead.
- [x] POK support. <!-- @todo: cheats finder; useful? --> 
- [x] Gunstick, Lightgun. Mikro-plus. [Lenslok](https://www.youtube.com/watch?v=GN_vPGQ4BNM). <!-- Cheetah Defender Lightgun, Magnum Light Phaser, Stack Light Rifle -->
- [x] External shaders support.
- [x] Internal savestates.
- [x] Graphical User Interface.
- [x] Portable: Windows, Linux, MacOS.
- [x] Embedded ZXDB. ZXDB Browser. ZXDB Gallery. <!-- @todo: 3d tape cases. -->
- [x] Can translate game menus from some languages into English.
- [x] Games can be appended to executable and get a standalone game viewer.
- [x] Co-op for kids: Horace, a companion screenmate.

# Downloads
Download both source code and binary releases from [these links](https://github.com/r-lyeh/Spectral/releases).

Alternatively, you can build the emulator yourself:
- Windows users double click `MAKE.bat` file.
- Linux/MacOS users can run `sh MAKE.bat` instead.

# Usage
Spectral can be configured with a mouse.

Here are some keyboard shortcuts, though:
- ESC: Game browser
- ALT+ENTER: Fullscreen
- TAB+CURSORS (or GAMEPAD): Joystick

Hold any F1..F12 key for 2 seconds to redefine it.

# Credits
Andre Weissflog, for their many single-header libraries! (Zlib licensed). Peter Sovietov and wermipls, for their accurate AY chip emulator (MIT licensed). Ulrich Doewich and Colin Pitrat, for their uPD765A floppy disk controller (GPL licensed). Marat Fayzullin for their WD1793/FDI controllers (Proprietary). Sean Middleditch for their gamepad code (MIT licensed). Sergey Bulba for their ay2sna tool (Public Domain). Potapov Vsevolod Viktorovich for their rusfaq website. Andrew Owen and Geoff Wearmouth for their custom ROMs. Simon Owen for their DSK technical websites. Santiago Romero, Philip Kendall, James McKay for their FOSS emulators. Damian Vila for their BESCII truetype font (CC-1.0). lalaoopybee, for their lovely tube shader. Günter Woigk, Juan Carlos González Amestoy and David Colmenero for their floppy sound recordings. Sean Middleditch for their gamepad library (MIT). joric for their irc client (PD). David Reid and Lieff for their mp3 decoding library (PD). Steve John for their waveform display code (MIT). Simon Owen for their LensKey source code. The ZXDB devs. The ZX Spectrum Discord folks. All the ZX community!

Special thanks to zjoYkileR, ZXGuesser, Beginner, arjun, ref and LaesQ! :D

# Unlicense
This software is released into the [public domain](https://unlicense.org/). Also dual-licensed as [0-BSD](https://opensource.org/licenses/0BSD) or [MIT (No Attribution)](https://github.com/aws/mit-0) for those countries where public domain is a concern (sigh). Any contribution to this repository is implicitly subjected to the same release conditions aforementioned.

# Privacy policy
This program will not transfer any information to other networked systems unless specifically requested by the user or the person installing or operating it.

# Code signing policy
Free code signing provided by [SignPath.io](https://about.signpath.io), certificate by [SignPath Foundation](https://signpath.org).

# Links
- [Introduction to the ZX Spectrum](https://en.wikipedia.org/wiki/ZX_Spectrum), entry on Wikipedia.
- [SpecEmu](https://specemu.zxe.io/), my favourite ZX emulator on Windows. Also, see [other alternatives](https://alternativeto.net/software/spectral/).
- [SpectrumComputing](https://spectrumcomputing.co.uk/), [WorldOfSpectrum](https://worldofspectrum.net/), [ZXArt](https://zxart.ee/), [Virtual TRDOS](https://vtrd.in/) and [ZXInfo](https://zxinfo.dk/) are the best online resources (imho).
- [Crash](https://archive.org/details/crash-magazine), [YourSinclair](https://archive.org/details/your-sinclair-magazine), [SinclairUser](https://archive.org/details/sinclair-user-magazine) and [MicroHobby(ES)](https://archive.org/details/microhobby-magazine) are great old paper magazines.
- [ZXDB](https://github.com/zxdb/ZXDB), [game maps](https://maps.speccy.cz/), [game cheats](https://www.the-tipshop.co.uk/), [RZX](https://worldofspectrum.net/RZXformat.html) [replays](https://www.rzxarchive.co.uk/) and [game longplays](https://www.youtube.com/@SpectrumComputing).
- [Daily ZX videos](https://www.youtube.com/results?search_query=zx+spectrum&sp=CAI%253D), on YouTube.

[![](https://github.com/r-lyeh/Spectral/actions/workflows/build.yml/badge.svg)](https://github.com/r-lyeh/Spectral/actions/workflows/build.yml) <a href="https://discord.gg/UpB7nahEFU"><img alt="Discord" src="https://img.shields.io/discord/354670964400848898?color=5865F2&label=Chat&logo=discord&logoColor=white"/></a>
