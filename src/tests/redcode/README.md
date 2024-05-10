# Z80 XCF Flavor

![](https://zxe.io/software/Z80_XCF_Flavor/assets/images/zx-spectrum-badge.svg)
[![](https://github.com/redcode/Z80_XCF_Flavor/actions/workflows/build.yml/badge.svg)](https://github.com/redcode/Z80_XCF_Flavor/actions/workflows/build.yml)

This is a test for the [ZX Spectrum](https://en.wikipedia.org/wiki/ZX_Spectrum) that detects the [Z80 CPU](https://en.wikipedia.org/wiki/Zilog_Z80) type. It is based on the behavior of the undocumented flags in the `ccf` and `scf` instructions.

## Background

In 2012, [Patrik Rak](https://https://github.com/raxoft) [deciphered](https://worldofspectrum.org/forums/discussion/41704) the behavior of YF and XF in the `ccf` and `scf` instructions, confirming that their values correspond, respectively, to bits 5 and 3 of the result of <code>(Q&nbsp;^&nbsp;F)&nbsp;|&nbsp;A</code>. This discovery applies to all Zilog Z80 models, both NMOS and CMOS.

The Q "register", or let's call it the Q factor, serves as an abstraction for a set of latches within the ALU where the CPU constructs the updated values of the flags. Put simply, after an instruction that does not alter the flags (including <code>ex&nbsp;af,af'</code> and <code>pop&nbsp;af</code>), Q equals `0`. Conversely, after an instruction that modifies the flags, Q equals the new value of the F register. Strangely, during the execution of the `ccf` and `scf` instructions, Q appears to leak partially, impacting YF and XF as seen in the aforementioned formula. Whether this leakage is intentional on Zilog's part, perhaps for verification or debugging purposes, remains a mystery. What we do know is that this behavior is not consistent across any Z80 model, regardless of the manufacturer. Many tested CPUs exhibit instability in the values of YF and XF during these instructions. Interestingly, the same CPU may demonstrate stability in some machines and instability in others, suggesting that external factors such as temperature, voltage, or clock signal may be at play.

In 2018, according to the [results](https://stardot.org.uk/forums/viewtopic.php?p=211042#p211042) of his tests, [David Banks](https://github.com/hoglet67) [stated](https://github.com/hoglet67/Z80Decoder/wiki/Undocumented-Flags) that NEC NMOS models simply get the value of both flags from the A register, whereas ST CMOS models use the formula discovered by Patrik for YF but obtain the value of XF from A. However, doubts have arisen regarding the existence of the ST CMOS behavior described by David Banks. Subsequent tests conducted by others indicate that these models behave the same as Zilog's in terms of `ccf` and `scf`. Therefore, Banks' results could have been influenced by the unstable behavior of the undocumented flags during these instructions or, in any case, might only apply to specific ST CMOS models.

## Test Details

<img src="https://zxe.io/software/Z80_XCF_Flavor/assets/images/readme-screenshot-1.5.gif" width="368" height="312" align="right">

Z80 XCF Flavor displays the values of YF and XF after the execution of `ccf` and `scf` for every possible combination of states of Q, F, and A. The left column represents the values of each factor (`1` is used to indicate that bits 5 and 3 in the factor are set to 1, due to the limited space on the ZX Spectrum screen). The central columns labeled "Any Zilog", "NEC NMOS", and "ST CMOS" show the reference values of YF and XF on those CPU variants for both `ccf` and `scf`. Finally, the two columns on the right display the values of YF and XF obtained on the host CPU for `ccf` and `scf` separately.

The program exists to BASIC after printing the results. To run it again, type `RUN 2` or <code>RANDOMIZE&nbsp;USR&nbsp;32768</code>.

> [!IMPORTANT]
> It is essential to note that this program does not conduct an exhaustive check of how each Z80 instruction affects Q. Instead, it uses the minimum necessary to test each combination of states. Its purpose is to detect real hardware behavior, not to validate the complete Q implementation of an emulator. For the latter, it is recommended to use the `z80ccf.tap` tape included in Patrik Rak's [Zilog Z80 CPU Test Suite](https://github.com/raxoft/z80test).

## Build

You will need [SjASMPlus](https://github.com/z00m128/sjasmplus) to assemble the source code.

To build the tape image and a snapshot in SNA format, type `make` or <code>sjasmplus&nbsp;"Z80&nbsp;XCF&nbsp;Flavor.asm"</code>.

## Thanks

Many thanks to the following people (in chronological order):

* **Mark Woodmass**, **Stuart Brady** and **Ian Greenway** for their initial research on the `ccf/scf` instructions.
* **Patrik Rack** for cracking the flag behavior of the `ccf/scf` instructions and writting tests.
* **David Banks** for his additional discoveries on the NEC NMOS and ST CMOS variants.
* **azesmbog** for discovering that the behavior of YF and XF is unstable during the `ccf/scf` instructions.
* **Peter Helcmanovsky** and **César Nicolás-González** for writting tests and for their help.
* **Ricardo Martínez Cantero** for validating the test on real hardware.

<sup>See the [README](https://github.com/redcode/Z80#thanks) of the [Z80](https://github.com/redcode/Z80) library for a complete list of bibliographical references.</sup>

## License

Copyright © 2022-2024 Manuel Sainz de Baranda y Goñi.  

<img src="https://zxe.io/software/Z80_XCF_Flavor/assets/images/gplv3.svg" width="160" align="right">

This program is [free software](https://www.gnu.org/philosophy/free-sw.html): you can redistribute it and/or modify it under the terms of the [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0.en.html) as published by the [Free Software Foundation](https://www.fsf.org), either version 3 of the License, or (at your option) any later version.

**This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE**. See the GNU General Public License for more details.

You should have received a [copy](COPYING) of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
