// ref: http://clrhome.org/table/
// ref: http://www.z80.info/z80sflag.htm
// ref: https://gist.github.com/drhelius/8497817
// ei, stays queued and lasts for 2 instructions

#include <string.h>

    // registers
  //enum {B,C,D,E,H,L,A,F,IXH,IXL,IYH,IYL,S,P,B_,C_,D_,E_,H_,L_,A_,F_,I,R};
  enum {C,B,E,D,L,H,F,A,IXL,IXH,IYL,IYH,P,S,C_,B_,E_,D_,L_,H_,F_,A_,R,I};
  enum {BC,DE,HL,AF,IX,IY,SP,BC_,DE_,HL_,AF_,IR};

    // flags
    // S:sign, Z:zero, 5:result bit5 (undoc), H:halfcarry, 3:result bit3 (undoc), P/V:parity/two-complement signed overflow, N:was addition 0/was subtraction 1, C:carry
    enum { fs = 128, fz = 64, f5 = 32, fh = 16, f3 = 8, fp = 4, fn = 2, fc = 1 };

struct z80 {
    unsigned char *ram;
    unsigned char *rom;
    unsigned short pc, di, im, swap, if1, if2, xy, lr;
    unsigned int   t, imm; // unsigned char imm
    union {
//      struct { unsigned char  b,c, d,e, h,l, a,f, ixh,ixl, iyh,iyl, s,p, b_,c_, d_,e_, h_,l_, a_,f_, i,r; }; 
        struct { unsigned char  c,b, e,d, l,h, f,a, ixl,ixh, iyl,iyh, p,s, c_,b_, e_,d_, l_,h_, f_,a_, r,i; }; 
        struct { unsigned short bc,  de,  hl,  af,  ix,      iy,      sp,  bc_,   de_,   hl_,   af_,   ir; };  
        unsigned char  reg8[28];
        unsigned short reg16[13+3]; // +3: extras for (HL)_mirror, (IX)_mirror, (IY)_mirror
    };
} z;

// unroll 7 horizontal byte opcodes: b,c,d,e,h,l,a
#define FULL7(X,opcode,bytes,cycles,altcycles,pcadj,fn8,fmt,...) \
    X(opcode+0x00,bytes,cycles,altcycles,pcadj,fn8(B),fmt " b",__VA_ARGS__) \
    X(opcode+0x01,bytes,cycles,altcycles,pcadj,fn8(C),fmt " c",__VA_ARGS__) \
    X(opcode+0x02,bytes,cycles,altcycles,pcadj,fn8(D),fmt " d",__VA_ARGS__) \
    X(opcode+0x03,bytes,cycles,altcycles,pcadj,fn8(E),fmt " e",__VA_ARGS__) \
    X(opcode+0x04,bytes,cycles,altcycles,pcadj,fn8(H),fmt " h",__VA_ARGS__) \
    X(opcode+0x05,bytes,cycles,altcycles,pcadj,fn8(L),fmt " l",__VA_ARGS__) \
    X(opcode+0x07,bytes,cycles,altcycles,pcadj,fn8(A),fmt " a",__VA_ARGS__)

// unroll 8 horizontal byte opcodes: b,c,d,e,h,l,(hl),a
#define FULL(X,opcode,bytes,cycles,altcycles,pcadj,fn8,fmt,...) \
    FULL7(X,opcode,bytes,cycles,altcycles,pcadj,fn8,fmt,__VA_ARGS__) \
    X(opcode+0x06,bytes,cycles,altcycles,pcadj,fn8(27),fmt " (hl)",__VA_ARGS__)

// unroll 4 vertical word opcodes: bc,de,hl,af
#define BCAF(X,opcode,bytes,cycles,altcycles,pcadj,fn16,fmt,...) \
    X(opcode+0x00,bytes,cycles,altcycles,pcadj,fn16(BC),fmt " bc",__VA_ARGS__) \
    X(opcode+0x10,bytes,cycles,altcycles,pcadj,fn16(DE),fmt " de",__VA_ARGS__) \
    X(opcode+0x20,bytes,cycles,altcycles,pcadj,fn16(HL),fmt " hl",__VA_ARGS__) \
    X(opcode+0x30,bytes,cycles,altcycles,pcadj,fn16(AF),fmt " af",__VA_ARGS__)

// unroll 4 vertical word opcodes: bc,de,hl,sp
#define BCSP(X,opcode,bytes,cycles,altcycles,pcadj,fn16,fmt,...) \
    X(opcode+0x00,bytes,cycles,altcycles,pcadj,fn16(BC),fmt " bc",__VA_ARGS__) \
    X(opcode+0x10,bytes,cycles,altcycles,pcadj,fn16(DE),fmt " de",__VA_ARGS__) \
    X(opcode+0x20,bytes,cycles,altcycles,pcadj,fn16(HL),fmt " hl",__VA_ARGS__) \
    X(opcode+0x30,bytes,cycles,altcycles,pcadj,fn16(SP),fmt " sp",__VA_ARGS__)

// unroll 3 vertical byte opcodes: b,d,h
#define BDH(X,opcode,bytes,cycles,altcycles,pcadj,fn8,fmt,...) \
    X(opcode+0x00,bytes,cycles,altcycles,pcadj,fn8(B),fmt " b",__VA_ARGS__) \
    X(opcode+0x10,bytes,cycles,altcycles,pcadj,fn8(D),fmt " d",__VA_ARGS__) \
    X(opcode+0x20,bytes,cycles,altcycles,pcadj,fn8(H),fmt " h",__VA_ARGS__)

// unroll 4 vertical byte opcodes: b,d,h,(hl)
#define BDHP(X,opcode,bytes,cycles,altcycles,pcadj,fn8,fmt,...) \
    BDH(X,opcode,bytes,cycles,altcycles,pcadj,fn8,fmt,__VA_ARGS__) \
    X(opcode+0x30,bytes,cycles,altcycles,pcadj,fn8(27),fmt " (hl)",__VA_ARGS__)

// unroll 4 vertical byte opcodes: c,e,l,a
#define CELA(X,opcode,bytes,cycles,altcycles,pcadj,fn8,fmt,...) \
    X(opcode+0x00,bytes,cycles,altcycles,pcadj,fn8(C),fmt " c",__VA_ARGS__) \
    X(opcode+0x10,bytes,cycles,altcycles,pcadj,fn8(E),fmt " e",__VA_ARGS__) \
    X(opcode+0x20,bytes,cycles,altcycles,pcadj,fn8(L),fmt " l",__VA_ARGS__) \
    X(opcode+0x30,bytes,cycles,altcycles,pcadj,fn8(A),fmt " a",__VA_ARGS__)

// unroll 4 vertical conditions: z,c,p(e),n(-)
#define ZCPM(X,opcode,bytes,cycles,altcycles,pcadj,fn0,else0,fmt,...) \
    X(opcode+0x00,bytes,cycles,altcycles,pcadj,( z.f & fz ? fn0() : else0()),fmt " z",__VA_ARGS__) \
    X(opcode+0x10,bytes,cycles,altcycles,pcadj,( z.f & fc ? fn0() : else0()),fmt " c",__VA_ARGS__) \
    X(opcode+0x20,bytes,cycles,altcycles,pcadj,( z.f & fp ? fn0() : else0()),fmt " p",__VA_ARGS__) /*pe*/ \
    X(opcode+0x30,bytes,cycles,altcycles,pcadj,( z.f & fs ? fn0() : else0()),fmt " m",__VA_ARGS__) /*m*/

// unroll 4 vertical negative conditions: nz,nc,p(o),p(+)
#define NZCPM(X,opcode,bytes,cycles,altcycles,pcadj,fn0,else0,fmt,...) \
    X(opcode+0x00,bytes,cycles,altcycles,pcadj,( z.f & fz ? else0() : fn0()),fmt " z", __VA_ARGS__) \
    X(opcode+0x10,bytes,cycles,altcycles,pcadj,( z.f & fc ? else0() : fn0()),fmt " c", __VA_ARGS__) \
    X(opcode+0x20,bytes,cycles,altcycles,pcadj,( z.f & fp ? else0() : fn0()),fmt " p", __VA_ARGS__) /*po*/ \
    X(opcode+0x30,bytes,cycles,altcycles,pcadj,( z.f & fs ? else0() : fn0()),fmt " m", __VA_ARGS__) /*p*/

// unroll single opcode 4 times
#define DUPE(X,opcode,bytes,cycles,altcycles,pcadj,block,fmt,...) \
    X(opcode+0x00,bytes,cycles,altcycles,pcadj,block,fmt,__VA_ARGS__) \
    X(opcode+0x10,bytes,cycles,altcycles,pcadj,block,fmt,__VA_ARGS__) \
    X(opcode+0x20,bytes,cycles,altcycles,pcadj,block,fmt,__VA_ARGS__) \
    X(opcode+0x30,bytes,cycles,altcycles,pcadj,block,fmt,__VA_ARGS__)

#define ENDIAN(h) (unsigned short)( ((unsigned short)(h) << 8) | ((unsigned short)(h) >> 8) )

#define NOP(...)    (z.pc+=0)
#define PCINC3(...) (z.pc+=3)

#define PTR8(p)  ((unsigned char*)&z.ram[p])
#define LD8(i)   (*PTR8(z.pc+i))
#define LD8i(i)  ((signed char)LD8(i))
#define PTR16(p) ((unsigned short*)&z.ram[p])
#define LD16(i)  (*PTR16(z.pc+i))

#define SETPTR16(addr,val)  (z.imm=(addr), (z.imm >= 0x4000 ? *PTR16(z.imm) = (val) : (val)))
#define SETPTR8(addr,val)   (z.imm=(addr), (z.imm >= 0x4000 ?  *PTR8(z.imm) = (val) : (val)))

#define LDXY(x,y) z.reg8[x] = z.reg8[y]
#define LDB(y)    LDXY(B,y)
#define LDC(y)    LDXY(C,y)
#define LDD(y)    LDXY(D,y)
#define LDE(y)    LDXY(E,y)
#define LDH(y)    LDXY(H,y)
#define LDL(y)    LDXY(L,y)
#define LDP(y)    SETPTR8(z.hl, z.reg8[y]) // PTR16
#define LDA(y)    LDXY(A,y)
#define LDX(x)    (z.reg8[x]=*PTR8(z.pc+1))
#define LDXX(xx)  (z.reg16[xx]=*PTR16(z.pc+1))

// Flags: (0)clear, (1)set, (-)not affected, (?)undefined
//                                     CPY| SET|
#define F01(r,nc) (z.t=(r),(z.f=         0|   0|   FLAG_S|FLAG_Z|FLAG_5|FLAG_H|FLAG_3|FLAG_V|            (nc)),z.t) // SZ5H3VNC   ADD/ADC/SUB/SBC
#define F02(r)    (z.t=(r),(z.f=         0|0x02|   FLAG_S|FLAG_Z|FLAG_5|FLAG_H|FLAG_3|FLAG_V|       0 |FLAG_C),z.t) // SZ5H3V1C              CP X  F5 and F3 are copied from the operand, not the result
#define F03(r,nc) (z.t=(r),(z.f=(z.f&0x11)|   0|   FLAG_S|FLAG_Z|FLAG_5|0/*FLAG_H*/|FLAG_3|FLAG_V|       (nc)),z.t) // SZ5H3VN-         INC/DEC X  16 bit additions are done in two steps:  First the two lower bytes are added, the two higher bytes.
#define F04(r)    (z.t=(r),(z.f=(z.f&0xC4)|   0|       0 |    0 |FLAG_5|FLAG_h|FLAG_3|    0 |       0 |FLAG_c),z.t) // --5H3-0C            ADD XX  F5,H,F3 from higher bytes addition
#define F05(r,nc) (z.t=(r),(z.f=         0|   0|   FLAG_S|FLAG_Z|FLAG_5|FLAG_h|FLAG_3|FLAG_V|            (nc)),z.t) // SZ5H3VNC        ADC/SBC XX  F5,H,F3 from higher bytes addition
#define F06(r)    (z.t=(r),(z.f=         0|0x10|   FLAG_S|FLAG_Z|FLAG_5|    0 |FLAG_3|FLAG_P|       0 |    0 ),z.t) // SZ513P00             AND X
#define F07(r)    (z.t=(r),(z.f=         0|   0|   FLAG_S|FLAG_Z|FLAG_5|    0 |FLAG_3|FLAG_P|       0 |    0 ),z.t) // SZ503P00          OR/XOR X
#define F08(r)    (z.t=(r),(z.f=(z.f&0xC4)|   0|       0 |    0 |FLAG_5|    0 |FLAG_3|    0 |       0 |FLAG_C),z.t) // --503-0C RLCA/RLA/RRCA/RRA
#define F09(r)    (z.t=(r),(z.f=         0|   0|   FLAG_S|FLAG_Z|FLAG_5|    0 |FLAG_3|FLAG_P|       0 |FLAG_C),z.t) // SZ503P0C   RLC/RL/RRC/RR X
#define F10(r)    (z.t=(r),(z.f=         0|   0|   FLAG_S|FLAG_Z|FLAG_5|    0 |FLAG_3|FLAG_P|       0 |FLAG_C),z.t) // SZ503P0C SLA/SLL/SRA/SRL X  SLL is like SLA except b0 gets set
#define F11(r)    (z.t=(r),(z.f=(z.f&0x01)|   0|   FLAG_S|FLAG_Z|FLAG_5|    0 |FLAG_3|FLAG_P|       0 |    0 ),z.t) // SZ503P0-           RRD/RLD  Flags set on result in A
#define F12(r,b)  (z.t=(r),(z.f=(z.f&0x01)|0x10|(z.t?z.t&128:0)|(z.t?0:!z.t)<<6|FLAG_5|    0 |FLAG_3|(z.t?0:!z.t)<<2|    0 |    0 ),z.t) // SZ513X0-           BIT n,X  PV as Z, S set only if n=7 and b7 of r set. Behaves much like AND r,2^n
//#define F12(r,b)  (z.t=(r),(z.f=(z.f&0x01)|0x10|FLAG_s(b)|FLAG_Z|FLAG_5|    0 |FLAG_3|(!z.t)<<2|    0 |    0 ),z.t) // SZ513X0-           BIT n,X  PV as Z, S set only if n=7 and b7 of r set. Behaves much like AND r,2^n
//#define F12(r,b)  (z.t=(r),(z.f=(z.f&0x01)|0x10|FLAG_s(b)|FLAG_P<<4|FLAG_5|    0 |FLAG_3|FLAG_P|    0 |    0 ),z.t) // SZ513X0-           BIT n,X  PV as Z, S set only if n=7 and b7 of r set. Behaves much like AND r,2^n
#define F13(r)    (z.t=(r),(z.f=(z.f&0xC4)|   0|       0 |    0 |FLAG_5|FLAG_H|FLAG_3|    0 |       0 |FLAG_C),z.t) // --5H3-0C               CCF  C=1-C, H as old C. F5, F3 from A register
#define F14(r)    (z.t=(r),(z.f=(z.f&0xC4)|0x01|       0 |    0 |FLAG_5|    0 |FLAG_3|    0 |       0 |    0 ),z.t) // --503-01               SCF  F5, F3 from A register
#define F15(r)    (z.t=(r),(z.f=(z.f&0xC5)|0x12|       0 |    0 |FLAG_5|    0 |FLAG_3|    0 |       0 |    0 ),z.t) // --513-1-               CPL  F5, F3 from A register
#define F16(r)    (z.t=(r),(z.f=         0|0x02|   FLAG_S|FLAG_Z|FLAG_5|FLAG_H|FLAG_3|FLAG_V|  FLAG_N |FLAG_C),z.t) // SZ5H3V1C               NEG  A=0-A (Zaks gets C wrong)
#define F17(r)    (z.t=(r),(z.f=(z.f&0x02)|   0|   FLAG_S|FLAG_Z|FLAG_5|FLAG_H|FLAG_3|FLAG_P|       0 |FLAG_C),z.t) // SZ5H3P-C               DAA  H from internal correction, C for cascade BCD
#define F18(r)    (z.t=(r),(z.f=(z.f&0x01)|   0|   FLAG_S|FLAG_Z|FLAG_5|    0 |FLAG_3|z.if2<<2|     0 |    0 ),z.t) // SZ503V0-     LD A,R/LD A,I  PV as IFF2 [yaze doesn't affect F?]
#define F19(r)    (z.t=(r),(z.f=(z.f&0xC1)|   0|       0 |    0 |FLAG_5|    0 |FLAG_3|FLAG_p|       0 |    0 ),z.t) // --503V0- LDI/LDIR/LDD/LDIR  PV set if BC not 0. F5 is bit 1 of (transferred byte + A). F3 is bit 3 of (transferred byte + A)
#define F20(r)    (z.t=(r),(z.f=(z.f&0x01)|0x02|   FLAG_S|FLAG_Z|FLAG_5|FLAG_H|FLAG_3|FLAG_p|       0 |    0 ),z.t) // SZ5H3V1- CPI/CPIR/CPD/CPDR  PV set if BC not 0. S,Z,H from (A - (HL) ) as in CP (HL). F3 is bit 3 of (A - (HL) - H), H as in F after instruction. F5 is bit 1 of (A - (HL) - H), H as in F after instruction
#define F21(r)    (z.t=(r),(z.f=(z.f&0x01)|   0|   FLAG_S|FLAG_Z|FLAG_5|    0 |FLAG_3|FLAG_P|       0 |    0 ),z.t) // SZ503P0-          IN X,(C)  Also true for IN F,(C)
#define F22(r)    (z.t=(r),(z.f=         0|   0|   FLAG_S|FLAG_Z|FLAG_5|    0 |FLAG_3|    0 |       0 |    0 ),z.t) // SZ5?3??? INI/INIR/IND/INDR  Flags affected as in DEC B
#define F00(r)    (z.t=(r))                                                                                         // --------        All others  Except for POP AF and EX AF,AF', of course..

#if 1
#elif 0
/* evaluate S+Z flags */
#define _SZ(val) ((val&0xFF)?(val&Z80_SF):Z80_ZF)
/* evaluate SZYXCH flags */
#define _SZYXCH(acc,val,res) (_SZ(res)|(res&(Z80_YF|Z80_XF))|((res>>8)&Z80_CF)|((acc^val^res)&Z80_HF))
/* evaluate flags for 8-bit adds */
#define _ADD_FLAGS(acc,val,res) (_SZYXCH(acc,val,res)|((((val^acc^0x80)&(val^res))>>5)&Z80_VF))
/* evaluate flags for 8-bit subs */
#define _SUB_FLAGS(acc,val,res) (Z80_NF|_SZYXCH(acc,val,res)|((((val^acc)&(res^acc))>>5)&Z80_VF))
/* evaluate flags for 8-bit compare */
#define _CP_FLAGS(acc,val,res) (Z80_NF|(_SZ(res)|(val&(Z80_YF|Z80_XF))|((res>>8)&Z80_CF)|((acc^val^res)&Z80_HF))|((((val^acc)&(res^acc))>>5)&Z80_VF))
/* evaluate flags for LD A,I and LD A,R */
#define _SZIFF2_FLAGS(val) ((_G_F()&Z80_CF)|_SZ(val)|(val&(Z80_YF|Z80_XF))|((r2&_BIT_IFF2)?Z80_PF:0))
BIT (r?(r&Z80_SF):(Z80_ZF|Z80_PF));
#endif

#define FLAG_S          (fs*!!(z.t&0x80))
#define FLAG_Z          (fz*!z.t)
#define FLAG_5   0//  (f5*!!(z.t&0x10))
#define FLAG_H   (fh&(z.a  ^ (z.imm&255) ^ (z.t&255))) // accum,value,result
#define FLAG_h          (fh&((z.hl ^ z.imm ^ z.t)>>8))
#define FLAG_3   0//  (f3*!!(z.t&0x04))
#define FLAG_P          (fp*!!(FLAG_PARITY[z.t&0xff]-'0'))
#define FLAG_V   (fp*!!((signed)(z.t&255) < -128 || (signed)(z.t&255) > 127))
#define FLAG_N          (fn)
#define FLAG_C          (fc*!!(z.t&~0x00ff))  // carry/borrow
#define FLAG_c          (fc*!!(z.t&~0xffff))  // carry/borrow
#define FLAG_p     (FLAG_P * !!z.bc) // special case
#define FLAG_s(b7) (FLAG_S * ((b7==7) && !!(z.t & (1<<7)))) // special case: S set only if n=7 and b7 of r set

        // P/V - Parity or Overflow
        // Parity set if even number of bits set
        // Overflow set if the 2-complement result does not fit in the register
        static const char FLAG_PARITY[256] = 
            "1001011001101001""0110100110010110"
            "0110100110010110""1001011001101001"
            "0110100110010110""1001011001101001"
            "1001011001101001""0110100110010110"
            "0110100110010110""1001011001101001"
            "1001011001101001""0110100110010110"
            "1001011001101001""0110100110010110"
            "0110100110010110""1001011001101001";

            // "1001011001101001""0110100110010110" #define PARITY(p) P[16*(P[16 + (p/16)]-'0') + p%16]


#define INCXX(xx)       (++z.reg16[xx])
#define DECXX(xx)       (--z.reg16[xx])
#define INCX(x)         (z.imm = z.reg8[x], F03(z.reg8[x] = z.imm + 1,FLAG_C))
#define DECX(x)         (z.imm = z.reg8[x], F03(z.reg8[x] = z.imm - 1,FLAG_C|FLAG_N))
#define DEC_IYP         (z.imm = *PTR8(z.reg16[IY]+LD8(1)), F03(*PTR8(z.reg16[IY]+LD8(1)) = z.imm - 1,FLAG_C|FLAG_N))

#define IMMADD(i)       (z.imm = (i), z.a = F01(z.a + z.imm           ,FLAG_C)) // add val to a
#define IMMADC(i)       (z.imm = (i), z.a = F01(z.a + z.imm + (z.f&fc),FLAG_C)) // add val and carry flag to a
#define IMMSUB(i)       (z.imm = (i), z.a = F01(z.a - z.imm           ,FLAG_C|FLAG_N)) // sub val from a
#define IMMSBC(i)       (z.imm = (i), z.a = F01(z.a - z.imm - (z.f&fc),FLAG_C|FLAG_N)) // sub val and carry flag from a
#define IMMAND(i)       (z.imm = (i), z.a = F06(z.a & z.imm                  )) // bitwise AND on a and val
#define IMMXOR(i)       (z.imm = (i), z.a = F07(z.a ^ z.imm                  )) // bitwise XOR on a and val
#define IMMOR(i)        (z.imm = (i), z.a = F07(z.a | z.imm                  )) // bitwise OR on a and val
#define IMMCP(i)        (z.imm = (i), F02(z.a - z.imm)) // update flags after subtracting val from a
#define IMMBIT(i,n)     (z.imm = (i), F12(z.imm & (1<<n),n))

#define ADDX(x)         IMMADD(z.reg8[x])
#define ADCX(x)         IMMADC(z.reg8[x])
#define SUBX(x)         IMMSUB(z.reg8[x])
#define SBCX(x)         IMMSBC(z.reg8[x])
#define ANDX(x)         IMMAND(z.reg8[x])
#define XORX(x)         IMMXOR(z.reg8[x])
#define ORX(x)          IMMOR(z.reg8[x])
#define CPX(x)          IMMCP(z.reg8[x])

#define NEG(x)          (z.imm = z.a, z.a = 0 - F16(z.imm))
#define ADDHL(xx)       (z.imm = z.reg16[xx], z.hl = F04(z.hl + z.imm))
#define ADCHL(xx)       (z.imm = z.reg16[xx], z.hl = F05(z.hl + z.imm + (z.f&fc),FLAG_c))
#define SBCHL(xx)       (z.imm = z.reg16[xx], z.hl = F05(z.hl - z.imm - (z.f&fc),FLAG_c|FLAG_N))

#define PUSH(v)         SETPTR16(z.sp-=2, v)
#define POP(v)          (v = (z.sp+=2,*PTR16(z.sp-2)))
#define PUSHXX(xx)      PUSH(z.reg16[xx])
#define POPXX(xx)       POP(z.reg16[xx])

#define CALL()          (PUSH(z.pc+3), z.pc = *PTR16(z.pc+1))
#define RET()           (z.pc = *PTR16(z.sp)-1, z.sp+=2)
#define JP()            (z.pc = *PTR16(z.pc+1))

#define SWAP(xx,yy)     (z.swap = (xx), (xx) = (yy), (yy) = z.swap)

#ifndef INP
#define INP(p)          0
#endif
#ifndef OUTP
#define OUTP(p,b)       (void)0
#endif

#define BIT0(x)         IMMBIT(z.reg8[x], 0)
#define BIT1(x)         IMMBIT(z.reg8[x], 1)
#define BIT2(x)         IMMBIT(z.reg8[x], 2)
#define BIT3(x)         IMMBIT(z.reg8[x], 3)
#define BIT4(x)         IMMBIT(z.reg8[x], 4)
#define BIT5(x)         IMMBIT(z.reg8[x], 5)
#define BIT6(x)         IMMBIT(z.reg8[x], 6)
#define BIT7(x)         IMMBIT(z.reg8[x], 7)

#define RES0(x)         (z.reg8[x]&= ~0x01)
#define RES1(x)         (z.reg8[x]&= ~0x02)
#define RES2(x)         (z.reg8[x]&= ~0x04)
#define RES3(x)         (z.reg8[x]&= ~0x08)
#define RES4(x)         (z.reg8[x]&= ~0x10)
#define RES5(x)         (z.reg8[x]&= ~0x20)
#define RES6(x)         (z.reg8[x]&= ~0x40)
#define RES7(x)         (z.reg8[x]&= ~0x80)

#define SET0(x)         (z.reg8[x] |= 0x01)
#define SET1(x)         (z.reg8[x] |= 0x02)
#define SET2(x)         (z.reg8[x] |= 0x04)
#define SET3(x)         (z.reg8[x] |= 0x08)
#define SET4(x)         (z.reg8[x] |= 0x10)
#define SET5(x)         (z.reg8[x] |= 0x20)
#define SET6(x)         (z.reg8[x] |= 0x40)
#define SET7(x)         (z.reg8[x] |= 0x80)

#define RL(x)           F09(z.reg8[x] = (z.reg8[x]<<1) | (z.f&fc) )
#define RR(x)           F09(z.reg8[x] = (z.reg8[x]>>1) | (z.f << 7))
#define RLC(x)          F09(z.reg8[x] = (z.reg8[x]<<1) | (z.reg8[x]>>7) )
#define RRC(x)          F09(z.reg8[x] = (z.reg8[x]>>1) | (z.reg8[x]<<7) )
#define SLA(x)          F10(z.reg8[x] <<= 1)
#define SRA(x)          F10(z.reg8[x] = (z.reg8[x] & 0x80) | (z.reg8[x]>>1))
#define SLL(x)          F10(z.reg8[x] = 1|(z.reg8[x] << 1))
#define SRL(x)          F10(z.reg8[x] >>= 1)

#define LD_X_IYP(x)     (z.reg8[x]=*PTR8(z.iy+z.xy))
#define LD_IYP_X(x)     SETPTR8(z.iy+z.xy, z.reg8[x])
#define LD_IMMP_XX(xx)  SETPTR16(z.pc+1, z.reg16[xx])
#define LD_XX_IMMP(xx)  (z.reg16[xx] = *PTR16(z.pc+1))

#define LDI     F19((SETPTR8(z.de, *PTR8(z.hl)), z.de++, z.hl++, z.bc--))
#define CPI     F20((IMMCP(*PTR8(z.hl)), z.hl++, z.bc--))
#define INI     F22((FIXME("bc->c"),SETPTR8(z.hl, INP(z.bc)), z.hl++, z.b--))
#define OUTI        (FIXME("bc->c"),OUTP(z.bc,*PTR8(z.hl)), z.hl++, z.b--) 
#define LDD_    F19((SETPTR8(z.de, *PTR8(z.hl)), z.de--, z.hl--, z.bc--))
#define CPD     F20((IMMCP(*PTR8(z.hl)), z.hl--, z.bc--))
#define IND     F22((FIXME("bc->c"),SETPTR8(z.hl, INP(z.bc)), z.hl--, z.b--))
#define OUTD        (FIXME("bc->c"),OUTP(z.bc,*PTR8(z.hl)), z.hl--, z.b--)

#define INX(x)  F21(z.reg8[x]=INP(z.bc))
#define OUTX(x)     OUTP(z.bc,z.reg8[x])

#define RETN        (z.if1 = z.if2, z.pc = *PTR16(z.sp), z.sp += 2)
#define RETI        (FIXME("raise"),RETN)

#define DAA     F17(z.a|=(z.a&0x0F)>9||(z.f&fh)?0x06:(z.a&0xF0)>0x90||(z.f&fc)?0x60:0)

#define RLD         assert(!"RLD") //F11
#define RRD         assert(!"RRD") //F11

#define IM(x)       (++z.r, z.im = (x), (z.di||!z.if1 ? 0 : ((z.if1=z.if2=0), PUSH(z.pc+1), z.pc=(z.im==1?0x38:*PTR8(z.ir))-1)))
// 0, exec(read8(bus)), 1, rst38, 2, rst(read8(bus)<<8)
//IM2: PC = READ_16(((zuint16)I << 8) | (zuint16)(INT_DATA & 0xFF));

/*  X(opcode, flags, format, bytes, cycles, alt cycles, pcadj, code...) */

#define LIST_DD(X) \
        X(0x23,     1,10,0,     0,INCX(IXH),                                                         "inc ixh", 0) 

#define LIST_DDCB(X) ;
#define LIST_FD(X) \
        X(0x21,     3,14,0,     0,z.reg16[IY]=LD16(1),                                               "ld iy,#%04X",LD16(1)) \
        X(0x35,     2,23,0,     0,DEC_IYP,                                                           "dec (iy+#%02X)",LD8(1)) \
FULL7(  X,0x70,     2,19,0,     0,LD_IYP_X,                                                          "ld (iy+#%02X),X",LD8(1)) \
        X(0x36,     3,19,0,     0,SETPTR8(z.reg16[IY]+LD8(1),LD8(2)),                                "ld (iy+#%02X),#%02X",LD8(1),LD8(2)) \
        X(0x86,     2,19,0,     0,(z.reg8[A]+=F01(*PTR8(z.reg16[IY]+LD8(1)),0)),                     "add a,(iy+#%02X)",LD8(1)) \
        X(0xA6,     2,19,0,     0,(z.reg8[A]&=F06(*PTR8(z.reg16[IY]+LD8(1)))),                       "and (iy+#%02X)",LD8(1)) \
BDH(    X,0x46,     2,19,0,     0,LD_X_IYP,                                                          "ld X,(iy+#%02x)",LD8(1)) 

#define LIST_FDCB(X) \
        X(0x46,     1,23,0,     0,IMMBIT(*PTR8(z.reg16[IY]+z.xy),0),                                 "bit 0,(iy+#%02x)",z.xy) \
        X(0x4E,     1,23,0,     0,IMMBIT(*PTR8(z.reg16[IY]+z.xy),1),                                 "bit 1,(iy+#%02x)",z.xy) \
        X(0x56,     1,23,0,     0,IMMBIT(*PTR8(z.reg16[IY]+z.xy),2),                                 "bit 2,(iy+#%02x)",z.xy) \
        X(0x5E,     1,23,0,     0,IMMBIT(*PTR8(z.reg16[IY]+z.xy),3),                                 "bit 3,(iy+#%02x)",z.xy) \
        X(0x66,     1,23,0,     0,IMMBIT(*PTR8(z.reg16[IY]+z.xy),4),                                 "bit 4,(iy+#%02x)",z.xy) \
        X(0x6E,     1,23,0,     0,IMMBIT(*PTR8(z.reg16[IY]+z.xy),5),                                 "bit 5,(iy+#%02x)",z.xy) \
        X(0x76,     1,23,0,     0,IMMBIT(*PTR8(z.reg16[IY]+z.xy),6),                                 "bit 6,(iy+#%02x)",z.xy) \
        X(0x7E,     1,23,0,     0,IMMBIT(*PTR8(z.reg16[IY]+z.xy),7),                                 "bit 7,(iy+#%02x)",z.xy) \
        X(0x86,     1,23,0,     0,SETPTR8(z.reg16[IY]+z.xy,*PTR8(z.reg16[IY]+z.xy)&~  1),            "res 0,(iy+#%02x)",z.xy) \
        X(0x8E,     1,23,0,     0,SETPTR8(z.reg16[IY]+z.xy,*PTR8(z.reg16[IY]+z.xy)&~  2),            "res 1,(iy+#%02x)",z.xy) \
        X(0x96,     1,23,0,     0,SETPTR8(z.reg16[IY]+z.xy,*PTR8(z.reg16[IY]+z.xy)&~  4),            "res 2,(iy+#%02x)",z.xy) \
        X(0x9E,     1,23,0,     0,SETPTR8(z.reg16[IY]+z.xy,*PTR8(z.reg16[IY]+z.xy)&~  8),            "res 3,(iy+#%02x)",z.xy) \
        X(0xA6,     1,23,0,     0,SETPTR8(z.reg16[IY]+z.xy,*PTR8(z.reg16[IY]+z.xy)&~ 16),            "res 4,(iy+#%02x)",z.xy) \
        X(0xAE,     1,23,0,     0,SETPTR8(z.reg16[IY]+z.xy,*PTR8(z.reg16[IY]+z.xy)&~ 32),            "res 5,(iy+#%02x)",z.xy) \
        X(0xB6,     1,23,0,     0,SETPTR8(z.reg16[IY]+z.xy,*PTR8(z.reg16[IY]+z.xy)&~ 64),            "res 6,(iy+#%02x)",z.xy) \
        X(0xBE,     1,23,0,     0,SETPTR8(z.reg16[IY]+z.xy,*PTR8(z.reg16[IY]+z.xy)&~128),            "res 7,(iy+#%02x)",z.xy) \
        X(0xC6,     1,23,0,     0,SETPTR8(z.reg16[IY]+z.xy,*PTR8(z.reg16[IY]+z.xy)|  1),             "set 0,(iy+#%02x)",z.xy) \
        X(0xCE,     1,23,0,     0,SETPTR8(z.reg16[IY]+z.xy,*PTR8(z.reg16[IY]+z.xy)|  2),             "set 1,(iy+#%02x)",z.xy) \
        X(0xD6,     1,23,0,     0,SETPTR8(z.reg16[IY]+z.xy,*PTR8(z.reg16[IY]+z.xy)|  4),             "set 2,(iy+#%02x)",z.xy) \
        X(0xDE,     1,23,0,     0,SETPTR8(z.reg16[IY]+z.xy,*PTR8(z.reg16[IY]+z.xy)|  8),             "set 3,(iy+#%02x)",z.xy) \
        X(0xE6,     1,23,0,     0,SETPTR8(z.reg16[IY]+z.xy,*PTR8(z.reg16[IY]+z.xy)| 16),             "set 4,(iy+#%02x)",z.xy) \
        X(0xEE,     1,23,0,     0,SETPTR8(z.reg16[IY]+z.xy,*PTR8(z.reg16[IY]+z.xy)| 32),             "set 5,(iy+#%02x)",z.xy) \
        X(0xF6,     1,23,0,     0,SETPTR8(z.reg16[IY]+z.xy,*PTR8(z.reg16[IY]+z.xy)| 64),             "set 6,(iy+#%02x)",z.xy) \
        X(0xFE,     1,23,0,     0,SETPTR8(z.reg16[IY]+z.xy,*PTR8(z.reg16[IY]+z.xy)|128),             "set 7,(iy+#%02x)",z.xy) 

#define LIST_ED(X) \
DUPE(   X,0x44,     1,4,0,      0,NEG(A),                                   "neg", 0) \
DUPE(   X,0x4C,     1,4,0,      0,NEG(A),                                   "neg", 0) \
        X(0x47,     1,9,0,      0,(z.i=z.a,++z.r),                          "ld i,a", 0) \
        X(0x57,     1,9,0,      0,F18(z.a=z.i),                             "ld a,i", 0) \
        X(0x4F,     1,9,0,      0,(z.r=z.a),                                "ld r,a", 0) \
        X(0x5F,     1,9,0,      0,F18(z.a=z.r),                             "ld a,r", 0) \
\
BCSP(   X,0x43,     3,20,0,     0,LD_IMMP_XX,                               "ld (#%04X),",LD16(1)) \
BCSP(   X,0x4B,     3,20,0,     0,LD_XX_IMMP,                               "ld XX,(#%04X),",LD16(1)) \
BCSP(   X,0x42,     1,15,0,     0,SBCHL,                                    "sbc hl,", 0) \
BCSP(   X,0x4A,     1,15,0,     0,ADCHL,                                    "abc hl,", 0) \
\
DUPE(   X,0x45,     1,14,0,     0,RETN,                                     "retn", 0) \
        X(0x5D,     1,14,0,     0,RETN,                                     "retn", 0) \
        X(0x6D,     1,14,0,     0,RETN,                                     "retn", 0) \
        X(0x7D,     1,14,0,     0,RETN,                                     "retn", 0) \
        X(0x46,     1,8,0,      0,IM(0),                                    "im 0", 0) \
        X(0x66,     1,8,0,      0,IM(0),                                    "im 0", 0) \
        X(0x56,     1,8,0,      0,IM(1),                                    "im 1", 0) \
        X(0x76,     1,8,0,      0,IM(1),                                    "im 1", 0) \
        X(0x5E,     1,8,0,      0,IM(2),                                    "im 2", 0) \
        X(0x7E,     1,8,0,      0,IM(2),                                    "im 2", 0) \
        X(0x4E,     1,8,0,      0,IM(-1),                                   "im 01", 0) \
        X(0x6E,     1,8,0,      0,IM(-1),                                   "im 01", 0) \
\
        X(0xA0,     1,16, 0,    0,LDI,                                      "ldi", 0) \
        X(0xB0,     1,16,21,    0,{do LDI; while(z.bc);},                   "ldir", 0) \
        X(0xA1,     1,16, 0,    0,CPI,                                      "cpi", 0) \
        X(0xB1,     1,16,21,    0,{do CPI; while(z.bc && !(z.f&fz));},      "cpir", 0) \
        X(0xA2,     1,16, 0,    0,INI,                                      "ini", 0) \
        X(0xB2,     1,16,21,    0,{do INI; while(z.b);},                    "inir", 0) \
        X(0xA3,     1,16, 0,    0,OUTI,                                     "outi", 0) \
        X(0xB3,     1,16,21,    0,{do OUTI; while(z.b);},                   "otir", 0) \
\
        X(0xA8,     1,16, 0,    0,LDD_,                                     "ldd", 0) \
        X(0xB8,     1,16,21,    0,{do LDD_; while(z.bc);},                  "lddr", 0) \
        X(0xA9,     1,16, 0,    0,CPD,                                      "cpd", 0) \
        X(0xB9,     1,16,21,    0,{do CPD; while(z.bc && !(z.f&fz));},      "cpdr", 0) \
        X(0xAA,     1,16, 0,    0,IND,                                      "ind", 0) \
        X(0xBA,     1,16,21,    0,{do IND; while(z.b);},                    "indr", 0) \
        X(0xAB,     1,16, 0,    0,OUTD,                                     "outd", 0) \
        X(0xBB,     1,16,21,    0,{do OUTD; while(z.b);},                   "otdr", 0) \
\
BDH(    X,0x40,     1,12,0,     0,INX,                                      "in X,(c)", 0) \
CELA(   X,0x48,     1,12,0,     0,INX,                                      "in X,(c)", 0) \
        X(0x70,     1,12,0,     0,INP(z.bc),                                "in (c)", 0) \
BDH(    X,0x41,     1,12,0,     0,OUTX,                                     "out (c),X", 0) \
CELA(   X,0x49,     1,12,0,     0,OUTX,                                     "out (c),X", 0) \
        X(0x71,     1,12,0,     0,OUTP(z.bc,0),                             "out (c),0", 0) 


#define LIST_CB(X) \
FULL(   X,0x00,     2,8,0,      0,RLC,                                      "rlc", 0) \
FULL(   X,0x08,     2,8,0,      0,RRC,                                      "rrc", 0) \
FULL(   X,0x10,     2,8,0,      0,RL,                                       "rl", 0) \
FULL(   X,0x18,     2,8,0,      0,RR,                                       "rr", 0) \
FULL(   X,0x20,     2,8,0,      0,SLA,                                      "sla", 0) \
FULL(   X,0x28,     2,8,0,      0,SRA,                                      "sra", 0) \
FULL(   X,0x30,     2,8,0,      0,SLL,                                      "sll", 0) \
FULL(   X,0x38,     2,8,0,      0,SRL,                                      "srl", 0) \
FULL(   X,0x40,     2,8,0,      0,BIT0,                                     "bit 0,", 0) \
FULL(   X,0x48,     2,8,0,      0,BIT1,                                     "bit 1,", 0) \
FULL(   X,0x50,     2,8,0,      0,BIT2,                                     "bit 2,", 0) \
FULL(   X,0x58,     2,8,0,      0,BIT3,                                     "bit 3,", 0) \
FULL(   X,0x60,     2,8,0,      0,BIT4,                                     "bit 4,", 0) \
FULL(   X,0x68,     2,8,0,      0,BIT5,                                     "bit 5,", 0) \
FULL(   X,0x70,     2,8,0,      0,BIT6,                                     "bit 6,", 0) \
FULL(   X,0x78,     2,8,0,      0,BIT7,                                     "bit 7,", 0) \
FULL(   X,0x80,     2,8,0,      0,RES0,                                     "res 0,", 0) \
FULL(   X,0x88,     2,8,0,      0,RES1,                                     "res 1,", 0) \
FULL(   X,0x90,     2,8,0,      0,RES2,                                     "res 2,", 0) \
FULL(   X,0x98,     2,8,0,      0,RES3,                                     "res 3,", 0) \
FULL(   X,0xA0,     2,8,0,      0,RES4,                                     "res 4,", 0) \
FULL(   X,0xA8,     2,8,0,      0,RES5,                                     "res 5,", 0) \
FULL(   X,0xB0,     2,8,0,      0,RES6,                                     "res 6,", 0) \
FULL(   X,0xB8,     2,8,0,      0,RES7,                                     "res 7,", 0) \
FULL(   X,0xC0,     2,8,0,      0,SET0,                                     "set 0,", 0) \
FULL(   X,0xC8,     2,8,0,      0,SET1,                                     "set 1,", 0) \
FULL(   X,0xD0,     2,8,0,      0,SET2,                                     "set 2,", 0) \
FULL(   X,0xD8,     2,8,0,      0,SET3,                                     "set 3,", 0) \
FULL(   X,0xE0,     2,8,0,      0,SET4,                                     "set 4,", 0) \
FULL(   X,0xE8,     2,8,0,      0,SET5,                                     "set 5,", 0) \
FULL(   X,0xF0,     2,8,0,      0,SET6,                                     "set 6,", 0) \
FULL(   X,0xF8,     2,8,0,      0,SET7,                                     "set 7,", 0) 

#define LIST_MAIN(X) \
        X(0x00,     1,4,0,      0,{},                                       "nop", 0) \
        X(0xF3,     1,4,0,      0,z.di=1,                                   "di", 0) \
        X(0xFB,     1,4,0,      0,z.di=0,                                   "ei", 0) \
        X(0x37,     1,4,0,      0,F14(z.f|=fc),                             "scf", 0) \
        X(0x3F,     1,4,0,      0,F13(z.f&=~fc),                            "ccf", 0) \
        X(0xC7,     1,11,0,    -1,(PUSH(z.pc+1),z.pc=0x00),                 "rst #00", 0) \
        X(0xD7,     1,11,0,    -1,(PUSH(z.pc+1),z.pc=0x10),                 "rst #10", 0) \
        X(0xE7,     1,11,0,    -1,(PUSH(z.pc+1),z.pc=0x20),                 "rst #20", 0) \
        X(0xF7,     1,11,0,    -1,(PUSH(z.pc+1),z.pc=0x30),                 "rst #30", 0) \
        X(0xCF,     1,11,0,    -1,(PUSH(z.pc+1),z.pc=0x08),                 "rst #08", 0) \
        X(0xDF,     1,11,0,    -1,(PUSH(z.pc+1),z.pc=0x18),                 "rst #18", 0) \
        X(0xEF,     1,11,0,    -1,(PUSH(z.pc+1),z.pc=0x28),                 "rst #28", 0) \
        X(0xFF,     1,11,0,    -1,(PUSH(z.pc+1),z.pc=0x38),                 "rst #38", 0) \
\
BCSP(   X,0x03,     1,6,0,      0,INCXX,                                    "inc", 0) \
BDHP(   X,0x04,     1,4,0,      0,INCX,                                     "inc", 0) \
CELA(   X,0x0C,     1,4,0,      0,INCX,                                     "inc", 0) \
BCSP(   X,0x0B,     1,6,0,      0,DECXX,                                    "dec", 0) \
BDHP(   X,0x05,     1,4,0,      0,DECX,                                     "dec", 0) \
CELA(   X,0x0D,     1,4,0,      0,DECX,                                     "dec", 0) \
FULL(   X,0x40,     1,4,0,      0,LDB,                                      "ld b,", 0) \
FULL(   X,0x48,     1,4,0,      0,LDC,                                      "ld c,", 0) \
FULL(   X,0x50,     1,4,0,      0,LDD,                                      "ld d,", 0) \
FULL(   X,0x58,     1,4,0,      0,LDE,                                      "ld e,", 0) \
FULL(   X,0x60,     1,4,0,      0,LDH,                                      "ld h,", 0) \
FULL(   X,0x68,     1,4,0,      0,LDL,                                      "ld l,", 0) \
FULL(   X,0x70,     1,4,0,      0,LDP,                                      "ld (hl),", 0) \
FULL(   X,0x78,     1,4,0,      0,LDA,                                      "ld a,", 0) \
\
BCSP(   X,0x01,     3,10,0,     0,LDXX,                                     "ld XX,#%04X",LD16(1)) \
BDHP(   X,0x06,     2,7,10,     0,LDX,                                      "ld X,#%02X",LD8(1)) \
CELA(   X,0x0E,     2,7,0,      0,LDX,                                      "ld X,#%02X",LD8(1)) \
\
BCSP(   X,0x09,     1,11,0,     0,ADDHL,                                    "add hl,", 0) \
FULL(   X,0x80,     1,4,0,      0,ADDX,                                     "add a,", 0) \
FULL(   X,0x88,     1,4,0,      0,ADCX,                                     "adc a,", 0) \
FULL(   X,0x90,     1,4,0,      0,SUBX,                                     "sub", 0) \
FULL(   X,0x98,     1,4,0,      0,SBCX,                                     "sbc a,", 0) \
FULL(   X,0xA0,     1,4,0,      0,ANDX,                                     "and", 0) \
FULL(   X,0xA8,     1,4,0,      0,XORX,                                     "xor", 0) \
FULL(   X,0xB0,     1,4,0,      0,ORX,                                      "or", 0) \
FULL(   X,0xB8,     1,4,0,      0,CPX,                                      "cp", 0) \
\
        X(0xC6,     2,7,0,      0,IMMADD(*PTR8(z.pc+1)),                    "add a,#%02X",LD8(1)) \
        X(0xD6,     2,7,0,      0,IMMSUB(*PTR8(z.pc+1)),                    "sub #%02X",LD8(1)) \
        X(0xE6,     2,7,0,      0,IMMAND(*PTR8(z.pc+1)),                    "and #%02X",LD8(1)) \
        X(0xF6,     2,7,0,      0,IMMOR(*PTR8(z.pc+1)),                     "or #%02X",LD8(1)) \
        X(0xCE,     2,7,0,      0,IMMADC(*PTR8(z.pc+1)),                    "adc a,#%02X",LD8(1)) \
        X(0xDE,     2,7,0,      0,IMMSBC(*PTR8(z.pc+1)),                    "sbc a,#%02X",LD8(1)) \
        X(0xEE,     2,7,0,      0,IMMXOR(*PTR8(z.pc+1)),                    "xor #%02X",LD8(1)) \
        X(0xFE,     2,7,0,      0,IMMCP(*PTR8(z.pc+1)),                     "cp #%02X",LD8(1)) \
\
BCAF(   X,0xC5,     1,11,0,     0,PUSHXX,                                   "push", 0) \
BCAF(   X,0xC1,     1,10,0,     0,POPXX,                                    "pop", 0) \
\
NZCPM(  X,0xC2,     3,10,0,    -3,JP,PCINC3,                                "jp nX,#%04X",LD16(1)) \
 ZCPM(  X,0xCA,     3,10,0,    -3,JP,PCINC3,                                "jp X,#%04X",LD16(1)) \
        X(0xC3,     3,10,0,    -3,z.pc=*PTR16(z.pc+1),                      "jp #%04X",LD16(1)) \
        X(0xE9,     1,4,0,     -1,z.pc=*PTR16(z.hl),                        "jp (hl)", 0) \
\
NZCPM(  X,0xC0,     1,11,5,     0,RET,NOP,                                  "ret nX", 0) \
 ZCPM(  X,0xC8,     1,11,5,     0,RET,NOP,                                  "ret X", 0) \
        X(0xC9,     1,10,0,     0,RET(),                                    "ret", 0) \
\
NZCPM(  X,0xC4,     3,17,10,    0,CALL,NOP,                                 "call nX,#%X",LD16(1)) \
 ZCPM(  X,0xCC,     3,17,10,    0,CALL,NOP,                                 "call X,#%X",LD16(1)) \
        X(0xCD,     3,17,0,    -3,CALL(),                                   "call #%04X",LD16(1)) \
\
        X(0xF9,     1,6,0,      0,z.sp=z.hl,                                "ld sp,hl", 0) \
\
        X(0x08,     1,4,0,      0,SWAP(z.af,z.af_),                         "ex af,af'", 0) \
        X(0xEB,     1,4,0,      0,SWAP(z.de,z.hl),                          "ex de,hl", 0) \
        X(0xE3,     1,19,0,     0,(z.t=*PTR16(z.sp),SETPTR16(z.sp,z.hl),z.hl=z.t),      "ex (sp),hl", 0) \
        X(0xD9,     1,4,0,      0,{SWAP(z.bc,z.bc_);SWAP(z.de,z.de_);SWAP(z.hl,z.hl_);},    "exx", 0) \
\
        X(0xD3,     2,11,0,     0,OUTP(*PTR8(z.pc+1),z.a),                  "out #%02X,a",LD8(1)) \
        X(0xDB,     2,11,0,     0,z.a = INP((z.a<<8)|*PTR16(z.pc+1)),       "in a,#%02X",LD8(1)) \
\
        X(0x2F,     1,4,0,      0,(z.a^=0xFF),                              "cpl", 0) \
        X(0x07,     1,4,0,      0,RLC(A),                                   "rlca", 0) \
        X(0x17,     1,4,0,      0,RL(A),                                    "rla", 0) \
        X(0x0F,     1,4,0,      0,RRC(A),                                   "rrca", 0) \
        X(0x1F,     1,4,0,      0,RR(A),                                    "rra", 0) \
\
        X(0x18,     2,12,0,     0,z.pc+=LD8i(1),                            "jr %+d",LD8i(1), 0) \
        X(0x28,     2,12,7,     0,z.pc+=z.f&fz?LD8i(1):0,                   "jr z,%+d",LD8i(1), 0) \
        X(0x38,     2,12,7,     0,z.pc+=z.f&fc?LD8i(1):0,                   "jr c,%+d",LD8i(1), 0) \
        X(0x20,     2,12,7,     0,z.pc+=z.f&fz?0:LD8i(1),                   "jr nz,%+d",LD8i(1), 0) \
        X(0x30,     2,12,7,     0,z.pc+=z.f&fc?0:LD8i(1),                   "jr nc,%+d",LD8i(1), 0) \
        X(0x10,     2,13,8,     0,z.pc+=--z.b?LD8i(1):0,                    "djnz %+d",LD8i(1), 0) \
\
        X(0x02,     1, 7,0,     0,SETPTR8(z.bc,z.a),                        "ld (bc),a", 0) \
        X(0x0A,     1, 7,0,     0,z.a=*PTR8(z.bc),                          "ld a,(bc)", 0) \
        X(0x12,     1, 7,0,     0,SETPTR8(z.de,z.a),                        "ld (de),a", 0) \
        X(0x1A,     1, 7,0,     0,z.a=*PTR8(z.de),                          "ld a,(de)", 0) \
        X(0x22,     3,16,0,     0,SETPTR16(LD16(1),z.hl),                   "ld (#%04X),hl",LD16(1)) \
        X(0x2A,     3,16,0,     0,z.hl=*PTR16(LD16(1)),                     "ld hl,(#%04X)",LD16(1)) \
        X(0x32,     3,13,0,     0,SETPTR8(LD16(1),z.a),                     "ld (#%04X),a",LD16(1)) \
        X(0x3A,     3,13,0,     0,z.a=*PTR8(LD16(1)),                       "ld a,(#%04X)",LD16(1)) \
\
        X(0x27,     1,4, 0,     0,DAA,                                      "daa", 0) 

int dis(int offs) {
    const char *prefix = "";
    char *ret = 0, *opc = 0, len = 1;
    unsigned z_pc_bak = z.pc;

#define UNDEFINED default: ret = va("%02x", z.ram[z.pc]), opc = va("--"); break;
#define X(opcode,bytes,cycles,altcycles,pcadj,code,...) break; case opcode: \
        /**/ if(bytes == 1) len = bytes, ret = va("%02x", z.ram[z.pc]), opc = va(__VA_ARGS__); \
        else if(bytes == 2) len = bytes, ret = va("%02x%02x", z.ram[z.pc], z.ram[z.pc+1]), opc = va(__VA_ARGS__); \
        else if(bytes == 3) len = bytes, ret = va("%02x%02x%02x", z.ram[z.pc], z.ram[z.pc+1], z.ram[z.pc+2]), opc = va(__VA_ARGS__); \
        else switch(0) { UNDEFINED };

    z.pc += offs;
    if(z.ram) {
        switch(z.ram[z.pc]) { 
            UNDEFINED LIST_MAIN(X)
            break; case 0xED: prefix = "ed";                          switch(z.ram[++z.pc]) { UNDEFINED LIST_ED(X) }
            break; case 0xCB: prefix = "cb";                          switch(z.ram[++z.pc]) { UNDEFINED LIST_CB(X) }
            break; case 0xDD: prefix = "dd";                          switch(z.ram[++z.pc]) { UNDEFINED LIST_DD(X)
            break; case 0xCB: prefix = "ddcb"; z.xy = z.ram[++z.pc];  switch(z.ram[++z.pc]) { UNDEFINED LIST_DDCB(X) }
            }
            break; case 0xFD: prefix = "fd";                          switch(z.ram[++z.pc]) { UNDEFINED LIST_FD(X)
            break; case 0xCB: prefix = "fdcb"; z.xy = z.ram[++z.pc];  switch(z.ram[++z.pc]) { UNDEFINED LIST_FDCB(X) }
            }
        }
    }

#undef X
#undef UNDEFINED

    if(opc && strstr(opc,"XX")) if(replace(opc, "XX", opc + strlen(opc) - 2)) opc[ strlen(opc) - 2 ] = 0;
    if(opc && strstr(opc,"X")) {
        // handle (hl) case, common otherwise
        if(opc[strlen(opc)-1] == ')') {
            char *x = strstr(opc, "X");
            char *t = va("%.*s(hl)%.*s",(int)(x - opc),opc,(int)(strlen(opc) - (int)(x-opc)-4-1), x+1);
            opc = t;
        } else {
            if(replace(opc, "X", opc + strlen(opc) - 1)) opc[ strlen(opc) - 1 ] = 0;
        }
    }

    char *regs = va("%c%c%c%c%c%c%c%c im%dei%d [%04x] af(%04x'%04x)bc(%04x'%04x)de(%04x'%04x)hl(%04x'%04x)ixy(%04x'%04x)ir(%04x)",
        z.f&fs?'S':'.',z.f&fz?'Z':'.',z.f&f5?'5':'.',z.f&fh?'H':'.',z.f&f3?'3':'.',z.f&fp?'P':'.',z.f&fn?'N':'.',z.f&fc?'C':'.',
        z.im,!z.di,ENDIAN(*PTR16(z.hl)),//ENDIAN(*PTR16(z.ix)),ENDIAN(*PTR16(z.iy)),
        z.af,z.af_,z.bc,z.bc_,z.de,z.de_,z.hl,z.hl_,z.ix,z.iy,z.ir
    );
    char disasm[32] = {0}; sprintf(disasm, "%s%*.s%s  %-15s", prefix, 8-(int)strlen(prefix)-(int)strlen(ret), "", ret, opc?opc:"");
    printf("#%04X:%04X %s %s\n", z_pc_bak+offs, z.sp, ret ? disasm : va("-----"), ret ? regs : "");

    z.pc = z_pc_bak;
    return len;
}
