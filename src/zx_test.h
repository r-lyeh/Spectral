// SAVE"" #audio #ear
// LOAD"" #audio #mic
// cobra's arc #audio #mic #ear
// parapshock @music #mic #beeper
// profanation @music #mic #beeper #issue2

// targetRenegade @menu #runahead #ay #audio #128
// taipan @menu #ay #save #128 #crash

// black lamp #128 #int
// afterburner #dsk #speedlock #weaksectors

// zxoom.z80 #disciple

// RANDOMIZE USR 5050 #cold #warm #z80

// jaws, platoon, aquaplane #border

// cauldron(silverbird) #used-to-crash #loader
// cauldron2 theEgg #press-a-key #loader

// slowglass, parapshock #crash #lack-of-INT

// mercs(samdisk), paperboy2 #offset-info #dsk

// blacklamp +3 (no AY)

// rom0 0x29D - wait for key in menu 128

// enter trdos from basic
//   34B3 CALL 1E99
//   34B6 LD HL,2D2B
//   34B9 PUSH HL
//   34BA PUSH BC
//   34BB RET   ;; SP [FF3C]==3D00

/* fdc timeout
alienstorm(erbe)
bonanza bros
dynasty wars
final fight
gauntlet iii
ghouls n ghosts
italia 1990
mercs
turbo out run
turrican
x-out
*/

/* weak sectors protection on DSKs without weak sectors
afterburner
airborne ranger
batman caped crusader
beyond the ice palace
buggy boy
daley thompson
guerrilla war
hopping mad
operation wolf[a]
outrun
overlander
target renegade
robocop
typhoon
wanderer
where time stood still
*/

// docs (verify dsk protections)
// https://simonowen.com/samdisk/sys_plus3/
// https://simonowen.com/blog/2009/03/21/further-edsk-extensions/
// https://simonowen.com/misc/extextdsk.txt
// http://www.cpctech.org.uk/docs/extdsk.html
// https://github.com/simonowen/samdisk/blob/main/src/types/dsk.cpp

// docs (pentagon-128)
// https://worldofspectrum.net/rusfaq/index.html
