#define NUM_FDI_DRIVES 4
#define BYTE_TYPE_DEFINED

/** EMULib Emulation Library *********************************/
/**                                                         **/
/**                        FDIDisk.h                        **/
/**                                                         **/
/** This file declares routines to load, save, and access   **/
/** disk images in various formats. The internal format is  **/
/** always .FDI. See FDIDisk.c for the actual code.         **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 2007-2021                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/
#ifndef FDIDISK_H
#define FDIDISK_H
#ifdef __cplusplus
extern "C" {
#endif
                              /* SaveFDI() result:           */
#define FDI_SAVE_FAILED    0  /* Failed saving disk image    */
#define FDI_SAVE_TRUNCATED 1  /* Truncated data while saving */
#define FDI_SAVE_PADDED    2  /* Padded data while saving    */
#define FDI_SAVE_OK        3  /* Succeeded saving disk image */

                           /* Supported disk image formats:  */
#define FMT_AUTO   0       /* Determine format automatically */                   
#define FMT_IMG    1       /* ZX Spectrum disk               */             
#define FMT_MGT    2       /* ZX Spectrum disk, same as .DSK */             
#define FMT_TRD    3       /* ZX Spectrum TRDOS disk         */
#define FMT_FDI    4       /* Generic FDI image              */ 
#define FMT_SCL    5       /* ZX Spectrum TRDOS disk         */
#define FMT_HOBETA 6       /* ZX Spectrum HoBeta disk        */
#define FMT_MSXDSK 7       /* MSX disk                       */          
#define FMT_CPCDSK 8       /* CPC disk                       */          
#define FMT_SF7000 9       /* Sega SF-7000 disk              */ 
#define FMT_SAMDSK 10      /* Sam Coupe disk                 */    
#define FMT_ADMDSK 11      /* Coleco Adam disk               */  
#define FMT_DDP    12      /* Coleco Adam tape               */  
#define FMT_SAD    13      /* Sam Coupe disk                 */
#define FMT_DSK    14      /* Generic raw disk image         */

#define SEEK_DELETED (0x40000000)

#define DataFDI(D) ((D)->Data+(D)->Data[10]+((int)((D)->Data[11])<<8))

#ifndef BYTE_TYPE_DEFINED
#define BYTE_TYPE_DEFINED
typedef unsigned char byte;
#endif

/** FDIDisk **************************************************/
/** This structure contains all disk image information and  **/
/** also the result of the last SeekFDI() call.             **/
/*************************************************************/
typedef struct
{
  byte Format;     /* Original disk format (FMT_*) */
  int  Sides;      /* Sides per disk */
  int  Tracks;     /* Tracks per side */
  int  Sectors;    /* Sectors per track */
  int  SecSize;    /* Bytes per sector */

  byte *Data;      /* Disk data */
  int  DataSize;   /* Disk data size */

  byte Header[6];  /* Current header, result of SeekFDI() */
  byte Verbose;    /* 1: Print debugging messages */
} FDIDisk;

/** InitFDI() ************************************************/
/** Clear all data structure fields.                        **/
/*************************************************************/
void InitFDI(FDIDisk *D);

/** EjectFDI() ***********************************************/
/** Eject disk image. Free all allocated memory.            **/
/*************************************************************/
void EjectFDI(FDIDisk *D);

/** NewFDI() *************************************************/
/** Allocate memory and create new .FDI disk image of given **/
/** dimensions. Returns disk data pointer on success, 0 on  **/
/** failure.                                                **/
/*************************************************************/
byte *NewFDI(FDIDisk *D,int Sides,int Tracks,int Sectors,int SecSize);

/** FormatFDI() ***********************************************/
/** Allocate memory and create new standard disk image for a **/
/** given format. Returns disk data pointer on success, 0 on **/
/** failure.                                                 **/
/**************************************************************/
byte *FormatFDI(FDIDisk *D,int Format);

/** LoadFDI() ************************************************/
/** Load a disk image from a given file, in a given format  **/
/** (see FMT_* #defines). Guess format from the file name   **/
/** when Format=FMT_AUTO. Returns format ID on success or   **/
/** 0 on failure. When FileName=0, ejects the disk freeing  **/
/** memory and returns 0.                                   **/
/*************************************************************/
int LoadFDI(FDIDisk *D,const char *FileName,int Format);

/** SaveFDI() ************************************************/
/** Save a disk image to a given file, in a given format    **/
/** (see FMT_* #defines). Use the original format when      **/
/** when Format=FMT_AUTO. Returns FDI_SAVE_OK on success,   **/
/** FDI_SAVE_PADDED if any sectors were padded,             **/
/** FDI_SAVE_TRUNCATED if any sectors were truncated,       **/
/** FDI_SAVE_FAILED (0) if failed.                          **/
/*************************************************************/
int SaveFDI(FDIDisk *D,const char *FileName,int Format);

/** SeekFDI() ************************************************/
/** Seek to given side/track/sector. Returns sector address **/
/** on success or 0 on failure.                             **/
/*************************************************************/
byte *SeekFDI(FDIDisk *D,int Side,int Track,int SideID,int TrackID,int SectorID);

/** LinearFDI() **********************************************/
/** Seek to given sector by its linear number. Returns      **/
/** sector address on success or 0 on failure.              **/
/*************************************************************/
byte *LinearFDI(FDIDisk *D,int SectorN);

#ifdef __cplusplus
}
#endif
#endif /* FDIDISK_H */

/** EMULib Emulation Library *********************************/
/**                                                         **/
/**                        FDIDisk.c                        **/
/**                                                         **/
/** This file contains routines to load, save, and access   **/
/** disk images in various formats. The internal format is  **/
/** always .FDI. See FDIDisk.h for declarations.            **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 2007-2021                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/
//#include "FDIDisk.h"
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <ctype.h>

#ifdef ZLIB
//#include <zlib.h>
#endif

#define IMAGE_SIZE(Fmt) \
  (Formats[Fmt].Sides*Formats[Fmt].Tracks*    \
   Formats[Fmt].Sectors*Formats[Fmt].SecSize)

#define FDI_SIDES(P)      ((P)[6]+((int)((P)[7])<<8))
#define FDI_TRACKS(P)     ((P)[4]+((int)((P)[5])<<8))
#define FDI_DIR(P)        ((P)+(P)[12]+((int)((P)[13])<<8)+14)
#define FDI_DATA(P)       ((P)+(P)[10]+((int)((P)[11])<<8))
#define FDI_INFO(P)       ((P)+(P)[8]+((int)((P)[9])<<8))
#define FDI_SECTORS(T)    ((T)[6])
#define FDI_TRACK(P,T)    (FDI_DATA(P)+(T)[0]+((int)((T)[1])<<8)+((int)((T)[2])<<16)+((int)((T)[3])<<24))
#define FDI_SECSIZE(S)    (SecSizes[(S)[3]<=4? (S)[3]:4])
#define FDI_SECTOR(P,T,S) (FDI_TRACK(P,T)+(S)[5]+((int)((S)[6])<<8))

static const struct { int Sides,Tracks,Sectors,SecSize; } Formats[] =
{
  { 2,80,16,256 }, /* Dummy format */
  { 2,80,10,512 }, /* FMT_IMG can be 256 */
  { 2,80,10,512 }, /* FMT_MGT can be 256 */
  { 2,80,16,256 }, /* FMT_TRD    - ZX Spectrum TRDOS disk */
  { 2,80,10,512 }, /* FMT_FDI    - Generic FDI image */
  { 2,80,16,256 }, /* FMT_SCL    - ZX Spectrum TRDOS disk */
  { 2,80,16,256 }, /* FMT_HOBETA - ZX Spectrum HoBeta disk */
  { 2,80,9,512 },  /* FMT_MSXDSK - MSX disk */
  { 2,80,9,512 },  /* FMT_CPCDSK - CPC disk */
  { 1,40,16,256 }, /* FMT_SF7000 - Sega SF-7000 disk */
  { 2,80,10,512 }, /* FMT_SAMDSK - Sam Coupe disk */
  { 1,40,8,512 },  /* FMT_ADMDSK - Coleco Adam disk */
  { 1,32,16,512 }, /* FMT_DDP    - Coleco Adam tape */
  { 2,80,10,512 }, /* FMT_SAD    - Sam Coupe disk */
  { 2,80,9,512 }   /* FMT_DSK    - Assuming 720kB MSX format */
};

static const int SecSizes[] =
{ 128,256,512,1024,4096,0 };

static const char FDIDiskLabel[] =
"Disk image created by EMULib (C)Marat Fayzullin";

static const byte TRDDiskInfo[] =
{
  0x01,0x16,0x00,0xF0,0x09,0x10,0x00,0x00,
  0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
  0x20,0x00,0x00,0x64,0x69,0x73,0x6B,0x6E,
  0x61,0x6D,0x65,0x00,0x00,0x00,0x46,0x55
};

static int stricmpn(const char *S1,const char *S2,int Limit)
{
  for(;*S1&&*S2&&Limit&&(toupper(*S1)==toupper(*S2));++S1,++S2,--Limit);
  return(Limit? toupper(*S1)-toupper(*S2):0);
}

/** InitFDI() ************************************************/
/** Clear all data structure fields.                        **/
/*************************************************************/
void InitFDI(FDIDisk *D)
{
  D->Format   = 0;
  D->Data     = 0;
  D->DataSize = 0;
  D->Sides    = 0;
  D->Tracks   = 0;
  D->Sectors  = 0;
  D->SecSize  = 0;
}

/** EjectFDI() ***********************************************/
/** Eject disk image. Free all allocated memory.            **/
/*************************************************************/
void EjectFDI(FDIDisk *D)
{
  if(D->Data) free(D->Data);
  InitFDI(D);
}

/** NewFDI() *************************************************/
/** Allocate memory and create new .FDI disk image of given **/
/** dimensions. Returns disk data pointer on success, 0 on  **/
/** failure.                                                **/
/*************************************************************/
byte *NewFDI(FDIDisk *D,int Sides,int Tracks,int Sectors,int SecSize)
{
  byte *P,*DDir;
  int I,J,K,L,N;

  /* Find sector size code */
  for(L=0;SecSizes[L]&&(SecSizes[L]!=SecSize);++L);
  if(!SecSizes[L]) return(0);

  /* Allocate memory */
  K = Sides*Tracks*Sectors*SecSize+sizeof(FDIDiskLabel);
  I = Sides*Tracks*(Sectors+1)*7+14;
  if(!(P=(byte *)malloc(I+K))) return(0);
  memset(P,0x00,I+K);

  /* Eject previous disk image */
  EjectFDI(D);

  /* Set disk dimensions according to format */
  D->Format   = FMT_FDI;
  D->Data     = P;
  D->DataSize = I+K;
  D->Sides    = Sides;
  D->Tracks   = Tracks;
  D->Sectors  = Sectors;
  D->SecSize  = SecSize;

  /* .FDI magic number */
  memcpy(P,"FDI",3);
  /* Disk description */
  memcpy(P+I,FDIDiskLabel,sizeof(FDIDiskLabel));
  /* Write protection (1=ON) */
  P[3]  = 0;
  P[4]  = Tracks&0xFF;
  P[5]  = Tracks>>8;
  P[6]  = Sides&0xFF;
  P[7]  = Sides>>8;
  /* Disk description offset */
  P[8]  = I&0xFF;
  P[9]  = I>>8;
  I    += sizeof(FDIDiskLabel);
  /* Sector data offset */
  P[10] = I&0xFF;
  P[11] = I>>8;
  /* Track directory offset */
  P[12] = 0;
  P[13] = 0;

  /* Create track directory */
  for(J=K=0,DDir=P+14;J<Sides*Tracks;++J,K+=Sectors*SecSize)
  {
    /* Create track entry */
    DDir[0] = K&0xFF;
    DDir[1] = (K>>8)&0xFF;
    DDir[2] = (K>>16)&0xFF;
    DDir[3] = (K>>24)&0xFF;
    /* Reserved bytes */
    DDir[4] = 0;
    DDir[5] = 0;
    DDir[6] = Sectors;
    /* For all sectors on a track... */
    for(I=N=0,DDir+=7;I<Sectors;++I,DDir+=7,N+=SecSize)
    {
      /* Create sector entry */
      DDir[0] = J/Sides;
      DDir[1] = J%Sides;
      DDir[2] = I+1;
      DDir[3] = L;
      /* CRC marks and "deleted" bit (D00CCCCC) */
      DDir[4] = (1<<L);
      DDir[5] = N&0xFF;
      DDir[6] = N>>8;
    }
  }

  /* Done */
  return(FDI_DATA(P));
}

#ifdef ZLIB
#define fopen(N,M)      (FILE *)gzopen(N,M)
#define fclose(F)       gzclose((gzFile)(F))
#define fread(B,L,N,F)  gzread((gzFile)(F),B,(L)*(N))
#define fwrite(B,L,N,F) gzwrite((gzFile)(F),B,(L)*(N))
#define fgets(B,L,F)    gzgets((gzFile)(F),B,L)
#define fseek(F,O,W)    gzseek((gzFile)(F),O,W)
#define rewind(F)       gzrewind((gzFile)(F))
#define fgetc(F)        gzgetc((gzFile)(F))
#define ftell(F)        gztell((gzFile)(F))
#endif

/** LoadFDI() ************************************************/
/** Load a disk image from a given file, in a given format  **/
/** (see FMT_* #defines). Guess format from the file name   **/
/** when Format=FMT_AUTO. Returns format ID on success or   **/
/** 0 on failure. When FileName=0, ejects the disk freeing  **/
/** memory and returns 0.                                   **/
/*************************************************************/
int LoadFDI(FDIDisk *D,const char *FileName,int Format)
{
  byte Buf[256],*P,*DDir;
  const char *T;
  int J,I,K,L,N;
  FILE *F;

  /* If just ejecting a disk, drop out */
  if(!FileName) { EjectFDI(D);return(0); }

  /* If requested automatic format recognition... */
  if(!Format)
  {
    /* Recognize disk image format */
    T = strrchr(FileName,'\\');
    T = T? T:strrchr(FileName,'/');
    T = T? T+1:FileName;
    T = strchr(T,'.');
    Format = !T? 0
           : !stricmpn(T,".FDI",4)? FMT_FDI
           : !stricmpn(T,".IMG",4)? FMT_IMG
           : !stricmpn(T,".MGT",4)? FMT_MGT
           : !stricmpn(T,".TRD",4)? FMT_TRD
           : !stricmpn(T,".SCL",4)? FMT_SCL
           : !stricmpn(T,".DSK",4)? FMT_DSK
           : !stricmpn(T,".DDP",4)? FMT_DDP
           : !stricmpn(T,".SAD",4)? FMT_SAD
           : !stricmpn(T,".$",2)?   FMT_HOBETA
           : 0;

    /* Try loading by extension, ignore generic raw images for now */
    if(Format&&(Format!=FMT_DSK)&&(Format!=FMT_MGT)&&(J=LoadFDI(D,FileName,Format)))
      return(J);

    /* Try loading by magic number... */

    /* Starts with "FDI" */
    if(LoadFDI(D,FileName,FMT_FDI)) return(FMT_FDI);

    /* Starts with "SINCLAIR" */
    if(LoadFDI(D,FileName,FMT_SCL)) return(FMT_SCL);

    /* Starts with "Aley's disk backup" */
    if(LoadFDI(D,FileName,FMT_SAD)) return(FMT_SAD);

    /* Starts with "MV - CPC" or "EXTENDED CPC DSK File" */
    if(LoadFDI(D,FileName,FMT_CPCDSK)) return(FMT_CPCDSK);

    /* Starts with 0xE9 or 0xEB, with some other constraints */
    if(LoadFDI(D,FileName,FMT_MSXDSK)) return(FMT_MSXDSK);

    /* Try loading as a generic raw disk image */
    return(LoadFDI(D,FileName,FMT_DSK));
  }

  /* Open file and find its size */
  if(!(F=fopen(FileName,"rb"))) return(0);
#ifdef ZLIB
  for(J=0;(I=fread(Buf,1,sizeof(Buf),F));J+=I);
#else
  if(fseek(F,0,SEEK_END)<0) { fclose(F);return(0); }
  if((J=ftell(F))<=0)       { fclose(F);return(0); }
#endif
  rewind(F);

  switch(Format)
  {
    case FMT_FDI: /* If .FDI format... */
      /* Allocate memory and read file */
      if(!(P=(byte *)malloc(J))) { fclose(F);return(0); }
      if(fread(P,1,J,F)!=J)      { free(P);fclose(F);return(0); }
      /* Verify .FDI format tag */
      if(memcmp(P,"FDI",3))      { free(P);fclose(F);return(0); }
      /* Eject current disk image */
      EjectFDI(D);
      /* Read disk dimensions */
      D->Sides   = FDI_SIDES(P);
      D->Tracks  = FDI_TRACKS(P);
      D->Sectors = 0;
      D->SecSize = 0;
      /* Check number of sectors and sector size */
      for(J=FDI_SIDES(P)*FDI_TRACKS(P),DDir=FDI_DIR(P);J;--J)
      {
        /* Get number of sectors */
        I=FDI_SECTORS(DDir);
        /* Check that all tracks have the same number of sectors */
        if(!D->Sectors) D->Sectors=I; else if(D->Sectors!=I) break;
        /* Check that all sectors have the same size */
        for(DDir+=7;I;--I,DDir+=7)
          if(!D->SecSize) D->SecSize=FDI_SECSIZE(DDir);
          else if(D->SecSize!=FDI_SECSIZE(DDir)) break;
        /* Drop out if the sector size is not uniform */
        if(I) break;
      }
      /* If no uniform sectors or sector size, set them to zeros */
      if(J) D->Sectors=D->SecSize=0;
      break;

    case FMT_DSK: /* If generic raw disk image... */
    case FMT_MGT: /* ZX Spectrum .MGT is similar to .DSK */
      /* Try a few standard geometries first */
      I = J==IMAGE_SIZE(FMT_MSXDSK)? FMT_MSXDSK /* 737280 bytes */
        : J==IMAGE_SIZE(FMT_ADMDSK)? FMT_ADMDSK /* 163840 bytes */
        : J==IMAGE_SIZE(FMT_SAMDSK)? FMT_SAMDSK /* 819200 bytes */
        : J==IMAGE_SIZE(FMT_TRD)?    FMT_TRD    /* 655360 bytes */
        : J==IMAGE_SIZE(FMT_DDP)?    FMT_DDP    /* 262144 bytes */
        : J==IMAGE_SIZE(FMT_SF7000)? FMT_SF7000 /* 163840 bytes (!) */
        : J==IMAGE_SIZE(FMT_MGT)?    FMT_MGT    /* 819200 bytes (!) */
        : 0;
      /* If a standard geometry found... */
      if(I)
      {
        /* Create a new disk image */
        P = FormatFDI(D,Format=I);
        if(!P) { fclose(F);return(0); }
        /* Read disk image file (ignore short reads!) */
        fread(P,1,IMAGE_SIZE(I),F);
        /* Done */
        P = D->Data;
        break;
      }
      /* Try finding matching geometry */
      for(K=1,P=0;!P&&(K<=2);K<<=1)
        for(I=40;!P&&(I<=80);I<<=1)
          for(N=8;!P&&(N<=16);++N)
            for(L=256;!P&&(L<=512);L<<=1)
              if(J==K*I*N*L)
              {
                /* Create a new disk image */
                P = NewFDI(D,K,I,N,L);
                if(!P) { fclose(F);return(0); }
                /* Read disk image file (ignore short reads!) */
                fread(P,1,J,F);
                /* Done */
                P = D->Data;
              }
      break;

    case FMT_MSXDSK: /* If MSX .DSK format... */
      /* Read header */
      if(fread(Buf,1,32,F)!=32) { fclose(F);return(0); }
      /* Check magic number */
      if((Buf[0]!=0xE9)&&(Buf[0]!=0xEB)) { fclose(F);return(0); }
      /* Check media descriptor */
      if(Buf[21]<0xF8) { fclose(F);return(0); }
      /* Compute disk geometry */
      K = Buf[26]+((int)Buf[27]<<8);       /* Heads   */
      N = Buf[24]+((int)Buf[25]<<8);       /* Sectors */
      L = Buf[11]+((int)Buf[12]<<8);       /* SecSize */
      I = Buf[19]+((int)Buf[20]<<8);       /* Total S.*/
      I = K&&N? I/K/N:0;                   /* Tracks  */
      /* Number of heads CAN BE WRONG */
      K = I&&N&&L? J/I/N/L:0;
      /* Create a new disk image */
      P = NewFDI(D,K,I,N,L);
      if(!P) { fclose(F);return(0); }
      /* Make sure we do not read too much data */
      I = K*I*N*L;
      J = J>I? I:J;
      /* Read disk image file (ignore short reads!) */
      rewind(F);
      fread(P,1,J,F);
      /* Done */
      P = D->Data;
      break;

    case FMT_CPCDSK: /* If Amstrad CPC .DSK format... */
      /* Read header (first track is next) */
      if(fread(Buf,1,256,F)!=256) { fclose(F);return(0); }
      /* Check magic string */
      if(memcmp(Buf,"MV - CPC",8)&&memcmp(Buf,"EXTENDED CPC DSK File",21))
      { fclose(F);return(0); }
      /* Compute disk geometry */
      I = Buf[48];                   /* Tracks  */
      K = Buf[49];                   /* Heads   */
      N = Formats[Format].Sectors;   /* Maximal number of sectors */
      L = Buf[50]+((int)Buf[51]<<8); /* Track size + 0x100 */
      /* Extended CPC .DSK format lists sizes by track */
      if(!L)
        for(J=0;J<I;++J)
          if(L<((int)Buf[J+52]<<8)) L=(int)Buf[J+52]<<8;
      /* Maximal sector size */  
      L = (L-0x100+N-1)/N;
      /* Round up to the next power of two */
      for(J=1;J<L;J<<=1);
//printf("Tracks=%d, Heads=%d, Sectors=%d, SectorSize=%d<%d\n",I,K,N,L,J);
      if(D->Verbose && (L!=J))
        printf("LoadFDI(): Adjusted %d-byte CPC disk sectors to %d bytes.\n",L,J);
      L = J;
      /* Check geometry */
      if(!K||!N||!L||!I) { fclose(F);return(0); }
      /* Create a new disk image */
      if(!NewFDI(D,K,I,N,L)) { fclose(F);return(0); }
      /* Sectors-per-track and bytes-per-sector may vary */
      D->Sectors = 0;
      D->SecSize = 0;
      /* We are rewriting .FDI directory and data */
      DDir = FDI_DIR(D->Data);
      P    = FDI_DATA(D->Data);
      /* Skip to the first track info block */
      fseek(F,0x100,SEEK_SET);
      /* Read tracks */
      for(I*=K;I;--I)
      {
        /* Read track header */
        if(fread(Buf,1,0x18,F)!=0x18) break;
        /* Check magic string */
        if(memcmp(Buf,"Track-Info\r\n",12)) break;
        /* Compute track geometry */
        N = Buf[21];             /* Sectors */
        L = Buf[20];             /* SecSize */
        J = P-FDI_DATA(D->Data); /* Data offset */
        /* Create .FDI track entry */
        DDir[0] = J&0xFF;
        DDir[1] = (J>>8)&0xFF;
        DDir[2] = (J>>16)&0xFF;
        DDir[3] = (J>>24)&0xFF;
        DDir[4] = 0;
        DDir[5] = 0;
        DDir[6] = N;
        /* Read sector headers */
        for(DDir+=7,J=N,K=0;J&&(fread(Buf,8,1,F)==8);DDir+=7,--J,K+=SecSizes[L])
        {
          /* Create .FDI sector entry */
          DDir[0] = Buf[0];
          DDir[1] = Buf[1];
          DDir[2] = Buf[2];
          DDir[3] = Buf[3];
//          DDir[4] = (1<<L)|(~Buf[4]&0x80);
          DDir[4] = (1<<L)|((Buf[5]&0x40)<<1);
          DDir[5] = K&0xFF;
          DDir[6] = K>>8;
        }
        /* Seek to the track data */
        if(fseek(F,0x100-0x18-8*N,SEEK_CUR)<0) break;
        /* Read track data */
        if(fread(P,1,K,F)!=K) break; else P+=K;
      }
      /* Done */
      P = D->Data;
      break;

    case FMT_SAD: /* If Sam Coupe .SAD format... */
      /* Read header */
      if(fread(Buf,1,22,F)!=22) { fclose(F);return(0); }
      /* Check magic string */
      if(memcmp(Buf,"Aley's disk backup",18)) { fclose(F);return(0); }
      /* Compute disk geometry */
      K = Buf[18];     /* Heads       */
      I = Buf[19];     /* Tracks      */
      N = Buf[20];     /* Sectors     */
      L = Buf[21]*64;  /* Sector size */
      /* Check geometry */
      if(!K||!N||!L||!I) { fclose(F);return(0); }
      /* Create a new disk image */
      P = NewFDI(D,K,I,N,L);
      if(!P) { fclose(F);return(0); }
      /* Make sure we do not read too much data */
      I = K*I*N*L;
      J = J-22;
      J = J>I? I:J;
      /* Read disk image file (ignore short reads!) */
      fread(P,1,J,F);
      /* Done */
      P = D->Data;
      break;

    case FMT_IMG: /* If .IMG format... */
      /* Create a new disk image */
      P = FormatFDI(D,Format);
      if(!P) { fclose(F);return(0); }
      /* Read disk image file track-by-track */
      K = Formats[Format].Tracks;
      L = Formats[Format].Sectors*Formats[Format].SecSize;
      I = Formats[Format].Tracks*Formats[Format].Sides;
      for(J=0;J<I;++J)
        if(fread(P+L*2*(J%K)+(J>=K? L:0),1,L,F)!=L) break;
      /* Done */
      P = D->Data;
      break;

    case FMT_SCL: /* If .SCL format... */
      /* @@@ NEED TO CHECK CHECKSUM AT THE END */
      /* Read header */
      if(fread(Buf,1,9,F)!=9) { fclose(F);return(0); }
      /* Verify .SCL format tag and the number of files */
      if(memcmp(Buf,"SINCLAIR",8)||(Buf[8]>128)) { fclose(F);return(0); }
      /* Create a new disk image */
      P = FormatFDI(D,Format);
      if(!P) { fclose(F);return(0); }
      /* Compute the number of free sectors */
      I = D->Sides*D->Tracks*D->Sectors;
      /* Build directory, until we run out of disk space */
      for(J=0,K=D->Sectors,DDir=P;(J<Buf[8])&&(K<I);++J)
      {
        /* Read .SCL directory entry */
        if(fread(DDir,1,14,F)!=14) break;
        /* Compute sector and track */
        DDir[14] = K%D->Sectors;
        DDir[15] = K/D->Sectors;
        /* Next entry */
        K       += DDir[13];
        DDir    += 16;
      }
      /* Skip over remaining directory entries */
      if(J<Buf[8]) fseek(F,(Buf[8]-J)*14,SEEK_CUR);
      /* Build disk information */
      memset(P+J*16,0,D->Sectors*D->SecSize-J*16);
      memcpy(P+0x08E2,TRDDiskInfo,sizeof(TRDDiskInfo));
      strncpy((char *)P+0x08F5,"SPECCY",8);
      P[0x8E1] = K%D->Sectors;  /* First free sector */
      P[0x8E2] = K/D->Sectors;  /* Track it belongs to */
      P[0x8E3] = 0x16 + (D->Tracks>40? 0:1) + (D->Sides>1? 0:2);
      P[0x8E4] = J;             /* Number of files */
      P[0x8E5] = (I-K)&0xFF;    /* Number of free sectors */
      P[0x8E6] = (I-K)>>8;
      /* Read data */
      for(DDir=P;J;--J,DDir+=16)
      {
        /* Determine data offset and size */
        I = (DDir[15]*D->Sectors+DDir[14])*D->SecSize;
        N = DDir[13]*D->SecSize;
        /* Read .SCL data (ignore short reads!) */
        fread(P+I,1,N,F);
      }
      /* Done */
      P = D->Data;
      break;

    case FMT_HOBETA: /* If .$* format... */
      /* Create a new disk image */
      P = FormatFDI(D,Format);
      if(!P) { fclose(F);return(0); }
      /* Read header */
      if(fread(P,1,17,F)!=17) { fclose(F);return(0); }
      /* Determine data offset and size */
      I = D->Sectors*D->SecSize;
      N = P[13]+((int)P[14]<<8);
      /* Build disk information */
      memset(P+16,0,I-16);
      memcpy(P+0x08E2,TRDDiskInfo,sizeof(TRDDiskInfo));
      strncpy((char *)P+0x08F5,"SPECCY",8);
      K        = D->Sectors+N;
      J        = D->Sectors*D->Tracks*D->Sides-K;
      P[0x8E1] = K%D->Sectors;  /* First free sector */
      P[0x8E2] = K/D->Sectors;  /* Track it belongs to */
      P[0x8E3] = 0x16 + (D->Tracks>40? 0:1) + (D->Sides>1? 0:2);
      P[0x8E4] = 1;             /* Number of files */
      P[0x8E5] = J&0xFF;        /* Number of free sectors */
      P[0x8E6] = J>>8;
      N        = N*D->SecSize;  /* N is now in bytes */
      /* Read data (ignore short reads!) */
      fread(P+I,1,N,F);
      /* Compute and check checksum */
      for(L=I=0;I<15;++I) L+=P[I];
      L = ((L*257+105)&0xFFFF)-P[15]-((int)P[16]<<8);
      if(L) { /* @@@ DO SOMETHING BAD! */ }
      /* Place file at track #1 sector #0, limit its size to 255 sectors */
      P[13] = P[14]? 255:P[13];
      P[14] = 0;
      P[15] = 1;
      P[16] = 0;
      /* Done */
      P = D->Data;
      break;

    case FMT_SF7000: /* If SF-7000 .SF format...  */
    case FMT_SAMDSK: /* If Sam Coupe .DSK format... */
    case FMT_ADMDSK: /* If Coleco Adam .DSK format... */
    case FMT_DDP:    /* If Coleco Adam .DDP format... */
    case FMT_TRD:    /* If ZX Spectrum .TRD format... */
      /* Must have exact size, unless it is a .TRD */
      if((Format!=FMT_TRD) && (J!=IMAGE_SIZE(Format))) { fclose(F);return(0); }
      /* Create a new disk image */
      P = FormatFDI(D,Format);
      if(!P) { fclose(F);return(0); }
      /* Read disk image file (ignore short reads!) */
      fread(P,1,IMAGE_SIZE(Format),F);
      /* Done */
      P = D->Data;
      break;

    default:
      /* Format not recognized */
      return(0);
  }

  if(D->Verbose)
    printf(
      "LoadFDI(): Loaded '%s', %d sides x %d tracks x %d sectors x %d bytes\n",
      FileName,D->Sides,D->Tracks,D->Sectors,D->SecSize
    );

  /* Done */
  fclose(F);
  D->Data   = P;
  D->Format = Format;
  return(Format);
}

#ifdef ZLIB
#undef fopen
#undef fclose
#undef fread
#undef fwrite
#undef fseek
#undef ftell
#undef rewind
#endif

/** SaveDSKData() ********************************************/
/** Save uniform disk data, truncating or adding zeros as   **/
/** needed. Returns FDI_SAVE_OK on success, FDI_SAVE_PADDED **/
/** if any sectors were padded, FDI_SAVE_TRUNCATED if any   **/
/** sectors were truncated, FDI_SAVE_FAILED if failed.      **/
/*************************************************************/
static int SaveDSKData(FDIDisk *D,FILE *F,int Sides,int Tracks,int Sectors,int SecSize)
{
  int J,I,K,Result;

  Result = FDI_SAVE_OK;

  /* Scan through all tracks, sides, sectors */
  for(J=0;J<Tracks;++J)
    for(I=0;I<Sides;++I)
      for(K=0;K<Sectors;++K)
      {
        /* Seek to sector and determine actual sector size */
        byte *P = SeekFDI(D,I,J,I,J,K+1);
        int   L = D->SecSize<SecSize? D->SecSize:SecSize;
        /* Write sector */
        if(!P||!L||(fwrite(P,1,L,F)!=L)) return(FDI_SAVE_FAILED);
        /* Pad sector to SecSize, if needed */
        if((SecSize>L)&&fseek(F,SecSize-L,SEEK_CUR)) return(FDI_SAVE_FAILED);
        /* Update result */
        L = SecSize>L? FDI_SAVE_PADDED:SecSize<L? FDI_SAVE_TRUNCATED:FDI_SAVE_OK;
        if(L<Result) Result=L;
      }

  /* Done */
  return(Result);
}

/** SaveIMGData() ********************************************/
/** Save uniform disk data, truncating or adding zeros as   **/
/** needed. Returns FDI_SAVE_OK on success, FDI_SAVE_PADDED **/
/** if any sectors were padded, FDI_SAVE_TRUNCATED if any   **/
/** sectors were truncated, FDI_SAVE_FAILED if failed.      **/
/*************************************************************/
static int SaveIMGData(FDIDisk *D,FILE *F,int Sides,int Tracks,int Sectors,int SecSize)
{
  int J,I,K,Result;

  Result = FDI_SAVE_OK;

  /* Scan through all sides, tracks, sectors */
  for(I=0;I<Sides;++I)
    for(J=0;J<Tracks;++J)
      for(K=0;K<Sectors;++K)
      {
        /* Seek to sector and determine actual sector size */
        byte *P = SeekFDI(D,I,J,I,J,K+1);
        int   L = D->SecSize<SecSize? D->SecSize:SecSize;
        /* Write sector */
        if(!P||!L||(fwrite(P,1,L,F)!=L)) return(FDI_SAVE_FAILED);
        /* Pad sector to SecSize, if needed */
        if((SecSize>L)&&fseek(F,SecSize-L,SEEK_CUR)) return(FDI_SAVE_FAILED);
        /* Update result */
        L = SecSize>L? FDI_SAVE_PADDED:SecSize<L? FDI_SAVE_TRUNCATED:FDI_SAVE_OK;
        if(L<Result) Result=L;
      }

  /* Done */
  return(Result);
}

/** SaveFDI() ************************************************/
/** Save a disk image to a given file, in a given format    **/
/** (see FMT_* #defines). Use the original format when      **/
/** when Format=FMT_AUTO. Returns FDI_SAVE_OK on success,   **/
/** FDI_SAVE_PADDED if any sectors were padded,             **/
/** FDI_SAVE_TRUNCATED if any sectors were truncated,       **/
/** FDI_SAVE_FAILED (0) if failed.                          **/
/*************************************************************/
int SaveFDI(FDIDisk *D,const char *FileName,int Format)
{
  byte S[256];
  int I,J,K,C,L,Result;
  FILE *F;
  byte *P,*T;

  /* Must have a disk to save */
  if(!D->Data) return(0);

  /* Use original format if requested */
  if(!Format) Format=D->Format;

  /* Open file for writing */
  if(!(F=fopen(FileName,"wb"))) return(0);

  /* Assume success */
  Result = FDI_SAVE_OK;

  /* Depending on the format... */
  switch(Format)
  {
    case FMT_FDI:
      /* This is the native format in which data is stored in memory */
      if(fwrite(D->Data,1,D->DataSize,F)!=D->DataSize)
      { fclose(F);unlink(FileName);return(0); }
      break;

    case FMT_IMG:
      /* Check the number of tracks and sides */
      if((FDI_TRACKS(D->Data)!=Formats[Format].Tracks)||(FDI_SIDES(D->Data)!=Formats[Format].Sides))
      { fclose(F);unlink(FileName);return(0); }
      /* Write out the data, in sides/tracks/sectors order */
      Result = SaveIMGData(D,F,Formats[Format].Sides,Formats[Format].Tracks,Formats[Format].Sectors,Formats[Format].SecSize);
      if(!Result) { fclose(F);unlink(FileName);return(0); }
      break;

    case FMT_SF7000:
    case FMT_SAMDSK:
    case FMT_ADMDSK:
    case FMT_DDP:
    case FMT_TRD:
      /* Check the number of tracks and sides */
      if((FDI_TRACKS(D->Data)!=Formats[Format].Tracks)||(FDI_SIDES(D->Data)!=Formats[Format].Sides))
      { fclose(F);unlink(FileName);return(0); }
      /* Write out the data, in tracks/sides/sectors order */
      Result = SaveDSKData(D,F,Formats[Format].Sides,Formats[Format].Tracks,Formats[Format].Sectors,Formats[Format].SecSize);
      if(!Result) { fclose(F);unlink(FileName);return(0); }
      break;

    case FMT_MSXDSK:
    case FMT_DSK:
    case FMT_MGT:
      /* Must have uniform tracks */
      if(!D->Sectors || !D->SecSize) { fclose(F);unlink(FileName);return(0); }
      /* Write out the data, in tracks/sides/sectors order */
      Result = SaveDSKData(D,F,FDI_SIDES(D->Data),FDI_TRACKS(D->Data),D->Sectors,D->SecSize);
      if(!Result) { fclose(F);unlink(FileName);return(0); }
      break;

    case FMT_SAD:
      /* Must have uniform tracks with "even" sector size */
      if(!D->Sectors || !D->SecSize || (D->SecSize&0x3F))
      { fclose(F);unlink(FileName);return(0); }
      /* Fill header */
      memset(S,0,sizeof(S));
      strcpy((char *)S,"Aley's disk backup");
      S[18] = FDI_SIDES(D->Data);
      S[19] = FDI_TRACKS(D->Data);
      S[20] = D->Sectors;
      S[21] = D->SecSize>>6;
      /* Write header */
      if(fwrite(S,1,22,F)!=22) { fclose(F);unlink(FileName);return(0); }
      /* Write out the data, in tracks/sides/sectors order */
      Result = SaveDSKData(D,F,S[18],S[19],S[20],S[21]*64);
      if(!Result) { fclose(F);unlink(FileName);return(0); }
      break;

    case FMT_SCL:
      /* Get data pointer */
      T=FDI_DATA(D->Data);
      /* Check tracks, sides, sectors, and the TR-DOS magic number */
      if((FDI_SIDES(D->Data)!=Formats[Format].Sides)
       ||(FDI_TRACKS(D->Data)!=Formats[Format].Tracks)
       ||(D->Sectors!=Formats[Format].Sectors)
       ||(D->SecSize!=Formats[Format].SecSize)
       ||(T[0x8E3]!=0x16)
      ) { fclose(F);unlink(FileName);return(0); }
      /* Write header */
      strcpy((char *)S,"SINCLAIR");
      S[8]=T[0x8E4];
      if(fwrite(S,1,9,F)!=9) { fclose(F);unlink(FileName);return(0); }
      for(C=I=0;I<9;++I) C+=S[I];
      /* Write directory entries */
      for(J=0,P=T;J<T[0x8E4];++J,P+=16)
      {
        if(fwrite(P,1,14,F)!=14) { fclose(F);unlink(FileName);return(0); }
        for(I=0;I<14;++I) C+=P[I];
      }
      /* Write files */
      for(J=0,P=T;J<T[0x8E4];++J,P+=16)
      {
        /* Determine data offset and size */
        K = (P[15]*D->Sectors+P[14])*D->SecSize;
        I = P[13]*D->SecSize;
        /* Write data */
        if(fwrite(T+K,1,I,F)!=I)
        { fclose(F);unlink(FileName);return(0); }
        /* Compute checksum */
        for(L=K,I+=K;L<I;++L) C+=T[L];
      }
      /* Write checksum */
      S[0] = C&0xFF;
      S[1] = (C>>8)&0xFF;
      S[2] = (C>>16)&0xFF;
      S[3] = (C>>24)&0xFF;
      if(fwrite(S,1,4,F)!=4) { fclose(F);unlink(FileName);return(0); }
      /* Done */
      break;

    case FMT_HOBETA:
      /* Get data pointer */
      T=FDI_DATA(D->Data);
      /* Check tracks, sides, sectors, and the TR-DOS magic number */
      if((FDI_SIDES(D->Data)!=Formats[Format].Sides)
       ||(FDI_TRACKS(D->Data)!=Formats[Format].Tracks)
       ||(D->Sectors!=Formats[Format].Sectors)
       ||(D->SecSize!=Formats[Format].SecSize)
       ||(T[0x8E3]!=0x16)
      ) { fclose(F);unlink(FileName);return(0); }
      /* Look for the first file */
      for(J=0,P=T;(J<T[0x8E4])&&!P[0];++J,P+=16);
      /* If not found, drop out */
      if(J>=T[0x8E4]) { fclose(F);unlink(FileName);return(0); }      
      /* Copy header */
      memcpy(S,P,14);
      /* Get single file address and size */
      I = P[13]*D->SecSize;
      P = T+(P[14]+D->Sectors*P[15])*D->SecSize;
      /* Compute checksum and build header */
      for(C=J=0;J<14;++J) C+=P[J];
      S[14] = 0;
      C     = (C+S[14])*257+105;
      S[15] = C&0xFF;
      S[16] = (C>>8)&0xFF;
      /* Write header */
      if(fwrite(S,1,17,F)!=17) { fclose(F);unlink(FileName);return(0); }
      /* Write file data */
      if(fwrite(P,1,I,F)!=I) { fclose(F);unlink(FileName);return(0); }
      /* Done */
      break;

    default:
      /* Can't save this format for now */
      fclose(F);
      unlink(FileName);
      return(0);
  }

  /* Done */
  fclose(F);
  return(Result);
}

/** SeekFDI() ************************************************/
/** Seek to given side/track/sector. Returns sector address **/
/** on success or 0 on failure.                             **/
/*************************************************************/
byte *SeekFDI(FDIDisk *D,int Side,int Track,int SideID,int TrackID,int SectorID)
{
  byte *P,*T;
  int J,Deleted;

  /* Have to have disk mounted */
  if(!D||!D->Data) return(0);

  /* May need to search for deleted sectors */
  Deleted = (SectorID>=0) && (SectorID&SEEK_DELETED)? 0x80:0x00;
  if(Deleted) SectorID&=~SEEK_DELETED;

  switch(D->Format)
  {
    case FMT_TRD:
    case FMT_DSK:
    case FMT_SCL:
    case FMT_FDI:
    case FMT_MGT:
    case FMT_IMG:
    case FMT_DDP:
    case FMT_SAD:
    case FMT_CPCDSK:
    case FMT_SAMDSK:
    case FMT_ADMDSK:
    case FMT_MSXDSK:
    case FMT_SF7000:
      /* Track directory */
      P = FDI_DIR(D->Data);
      /* Find current track entry */
      for(J=Track*D->Sides+Side%D->Sides;J;--J) P+=(FDI_SECTORS(P)+1)*7;
      /* Find sector entry */
      for(J=FDI_SECTORS(P),T=P+7;J;--J,T+=7)
        if((T[0]==TrackID)||(TrackID<0))
          if((T[1]==SideID)||(SideID<0))
            if(((T[2]==SectorID)&&((T[4]&0x80)==Deleted))||(SectorID<0))
              break;
      /* Fall out if not found */
      if(!J) return(0);
      /* FDI stores a header for each sector */
      D->Header[0] = T[0];
      D->Header[1] = T[1];
      D->Header[2] = T[2];
      D->Header[3] = T[3]<=3? T[3]:3;
      D->Header[4] = T[4];
      D->Header[5] = 0x00;
      /* FDI has variable sector numbers and sizes */
      D->Sectors   = FDI_SECTORS(P);
      D->SecSize   = FDI_SECSIZE(T);
      return(FDI_SECTOR(D->Data,P,T));
  }

  /* Unknown format */
  return(0);
}

/** LinearFDI() **********************************************/
/** Seek to given sector by its linear number. Returns      **/
/** sector address on success or 0 on failure.              **/
/*************************************************************/
byte *LinearFDI(FDIDisk *D,int SectorN)
{
  if(!D->Sectors || !D->Sides || (SectorN<0)) return(0);
  else
  {
    int Sector = SectorN % D->Sectors;
    int Track  = SectorN / D->Sectors / D->Sides;
    int Side   = (SectorN / D->Sectors) % D->Sides;
    return(SeekFDI(D,Side,Track,Side,Track,Sector+1));
  }
}

/** FormatFDI() ***********************************************/
/** Allocate memory and create new standard disk image for a **/
/** given format. Returns disk data pointer on success, 0 on **/
/** failure.                                                 **/
/**************************************************************/
byte *FormatFDI(FDIDisk *D,int Format)
{
  if((Format<0) || (Format>=sizeof(Formats)/sizeof(Formats[0]))) return(0);
  
  return(NewFDI(D,
    Formats[Format].Sides,
    Formats[Format].Tracks,
    Formats[Format].Sectors,
    Formats[Format].SecSize
  ));
}

/** EMULib Emulation Library *********************************/
/**                                                         **/
/**                         WD1793.h                        **/
/**                                                         **/
/** This file contains emulation for the WD1793/2793 disk   **/
/** controller produced by Western Digital. See WD1793.c    **/
/** for implementation.                                     **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 2005-2021                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/
#ifndef WD1793_H
#define WD1793_H

//#include "FDIDisk.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WD1793_KEEP    0
#define WD1793_INIT    1
#define WD1793_EJECT   2

#define WD1793_COMMAND 0
#define WD1793_STATUS  0
#define WD1793_TRACK   1
#define WD1793_SECTOR  2
#define WD1793_DATA    3
#define WD1793_SYSTEM  4
#define WD1793_READY   4

#define WD1793_IRQ     0x80
#define WD1793_DRQ     0x40

                           /* Common status bits:               */
#define F_BUSY     0x01    /* Controller is executing a command */
#define F_READONLY 0x40    /* The disk is write-protected       */
#define F_NOTREADY 0x80    /* The drive is not ready            */

                           /* Type-1 command status:            */
#define F_INDEX    0x02    /* Index mark detected               */
#define F_TRACK0   0x04    /* Head positioned at track #0       */
#define F_CRCERR   0x08    /* CRC error in ID field             */
#define F_SEEKERR  0x10    /* Seek error, track not verified    */
#define F_HEADLOAD 0x20    /* Head loaded                       */

                           /* Type-2 and Type-3 command status: */
#define F_DRQ      0x02    /* Data request pending              */
#define F_LOSTDATA 0x04    /* Data has been lost (missed DRQ)   */
#define F_ERRCODE  0x18    /* Error code bits:                  */
#define F_BADDATA  0x08    /* 1 = bad data CRC                  */
#define F_NOTFOUND 0x10    /* 2 = sector not found              */
#define F_BADID    0x18    /* 3 = bad ID field CRC              */
#define F_DELETED  0x20    /* Deleted data mark (when reading)  */
#define F_WRFAULT  0x20    /* Write fault (when writing)        */

#define C_DELMARK  0x01
#define C_SIDECOMP 0x02
#define C_STEPRATE 0x03
#define C_VERIFY   0x04
#define C_WAIT15MS 0x04
#define C_LOADHEAD 0x08
#define C_SIDE     0x08
#define C_IRQ      0x08
#define C_SETTRACK 0x10
#define C_MULTIREC 0x10

#define S_DRIVE    0x03
#define S_RESET    0x04
#define S_HALT     0x08
#define S_SIDE     0x10
#define S_DENSITY  0x20

#ifndef BYTE_TYPE_DEFINED
#define BYTE_TYPE_DEFINED
typedef unsigned char byte;
#endif

#pragma pack(4)
typedef struct
{
  int  Rsrvd1[4]; /* Reserved, do not touch */

  byte R[5];      /* Registers */
  byte Drive;     /* Current disk # */
  byte Side;      /* Current side # */
  byte Track[4];  /* Current track # */
  byte LastS;     /* Last STEP direction */
  byte IRQ;       /* 0x80: IRQ pending, 0x40: DRQ pending */
  byte Wait;      /* Expiration counter */
  byte Cmd;       /* Last command */
                  
  int  WRLength;  /* Data left to write */
  int  RDLength;  /* Data left to read */
  int  Rsrvd2;    /* Reserved, do not touch */
                  
  byte Verbose;   /* 1: Print debugging messages */

  /*--- Save1793() will save state above this line ----*/

  byte *Ptr;        /* Pointer to data */
  FDIDisk *Disk[4]; /* Disk images */
} WD1793;
#pragma pack()

/** Reset1793() **********************************************/
/** Reset WD1793. When Disks=WD1793_INIT, also initialize   **/
/** disks. When Disks=WD1793_EJECT, eject inserted disks,   **/
/** freeing memory.                                         **/
/*************************************************************/
void Reset1793(register WD1793 *D,FDIDisk *Disks,register byte Eject);

/** Read1793() ***********************************************/
/** Read value from a WD1793 register A. Returns read data  **/
/** on success or 0xFF on failure (bad register address).   **/
/*************************************************************/
byte Read1793(register WD1793 *D,register byte A);

/** Write1793() **********************************************/
/** Write value V into WD1793 register A. Returns DRQ/IRQ   **/
/** values.                                                 **/
/*************************************************************/
byte Write1793(register WD1793 *D,register byte A,register byte V);

/** Save1793() ***********************************************/
/** Save WD1793 state to a given buffer of given maximal    **/
/** size. Returns number of bytes saved or 0 on failure.    **/
/*************************************************************/
unsigned int Save1793(const register WD1793 *D,byte *Buf,unsigned int Size);

/** Load1793() ***********************************************/
/** Load WD1793 state from a given buffer of given maximal  **/
/** size. Returns number of bytes loaded or 0 on failure.   **/
/*************************************************************/
unsigned int Load1793(register WD1793 *D,byte *Buf,unsigned int Size);

#ifdef __cplusplus
}
#endif
#endif /* WD1793_H */

/** EMULib Emulation Library *********************************/
/**                                                         **/
/**                         WD1793.c                        **/
/**                                                         **/
/** This file contains emulation for the WD1793/2793 disk   **/
/** controller produced by Western Digital. See WD1793.h    **/
/** for declarations.                                       **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 2005-2021                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/
//#include "WD1793.h"
//#include <stdio.h>
//#include <string.h>

/** Reset1793() **********************************************/
/** Reset WD1793. When Disks=WD1793_INIT, also initialize   **/
/** disks. When Disks=WD1793_EJECT, eject inserted disks,   **/
/** freeing memory.                                         **/
/*************************************************************/
void Reset1793(register WD1793 *D,FDIDisk *Disks,register byte Eject)
{
  int J;

  D->R[0]     = 0x00;
  D->R[1]     = 0x00;
  D->R[2]     = 0x00;
  D->R[3]     = 0x00;
  D->R[4]     = S_RESET|S_HALT;
  D->Drive    = 0;
  D->Side     = 0;
  D->LastS    = 0;
  D->IRQ      = 0;
  D->WRLength = 0;
  D->RDLength = 0;
  D->Wait     = 0;
  D->Cmd      = 0xD0;
  D->Rsrvd2   = 0;

  /* For all drives... */
  for(J=0;J<4;++J)
  {
    /* Reset drive-dependent state */
    D->Disk[J]   = Disks? &Disks[J]:0;
    D->Track[J]  = 0;
    D->Rsrvd1[J] = 0;
    /* Initialize disk structure, if requested */
    if((Eject==WD1793_INIT)&&D->Disk[J])  InitFDI(D->Disk[J]);
    /* Eject disk image, if requested */
    if((Eject==WD1793_EJECT)&&D->Disk[J]) EjectFDI(D->Disk[J]);
  }
}

/** Save1793() ***********************************************/
/** Save WD1793 state to a given buffer of given maximal    **/
/** size. Returns number of bytes saved or 0 on failure.    **/
/*************************************************************/
unsigned int Save1793(const register WD1793 *D,byte *Buf,unsigned int Size)
{
  unsigned int N = (const byte *)&(D->Ptr) - (const byte *)D;
  if(N>Size) return(0);
  memcpy(Buf,D,N);
  return(N);
}

/** Load1793() ***********************************************/
/** Load WD1793 state from a given buffer of given maximal  **/
/** size. Returns number of bytes loaded or 0 on failure.   **/
/*************************************************************/
unsigned int Load1793(register WD1793 *D,byte *Buf,unsigned int Size)
{
  unsigned int N = (const byte *)&(D->Ptr) - (const byte *)D;
  if(N>Size) return(0);
  memcpy(D,Buf,N);
  return(N);
}

/** Read1793() ***********************************************/
/** Read value from a WD1793 register A. Returns read data  **/
/** on success or 0xFF on failure (bad register address).   **/
/*************************************************************/
byte Read1793(register WD1793 *D,register byte A)
{
  switch(A)
  {
    case WD1793_STATUS:
      A=D->R[0];
      /* If no disk present, set F_NOTREADY */
      if(!D->Disk[D->Drive]||!D->Disk[D->Drive]->Data) A|=F_NOTREADY;
      if((D->Cmd<0x80)||(D->Cmd==0xD0))
      {
        /* Keep flipping F_INDEX bit as the disk rotates (Sam Coupe) */
        D->R[0]=(D->R[0]^F_INDEX)&(F_INDEX|F_BUSY|F_NOTREADY|F_READONLY|F_TRACK0);
      }
      else
      {
        /* When reading status, clear all bits but F_BUSY and F_NOTREADY */
        D->R[0]&=F_BUSY|F_NOTREADY|F_READONLY|F_DRQ;
      }
      return(A);
    case WD1793_TRACK:
    case WD1793_SECTOR:
      /* Return track/sector numbers */
      return(D->R[A]);
    case WD1793_DATA:
      /* When reading data, load value from disk */
      if(!D->RDLength)
      { if(D->Verbose) printf("WD1793: EXTRA DATA READ\n"); }
      else
      {
        /* Read data */
        D->R[A]=*D->Ptr++;
        /* Decrement length */
        if(--D->RDLength)
        {
          /* Reset timeout watchdog */
          D->Wait=255;
          /* Advance to the next sector as needed */
          if(!(D->RDLength&(D->Disk[D->Drive]->SecSize-1))) ++D->R[2];
        }
        else
        {
          /* Read completed */
          if(D->Verbose) printf("WD1793: READ COMPLETED\n");
          D->R[0]&= ~(F_DRQ|F_BUSY);
          D->IRQ  = WD1793_IRQ;
        }
      }
      return(D->R[A]);
    case WD1793_READY:
      /* After some idling, stop read/write operations */
      if(D->Wait)
        if(!--D->Wait)
        {
          if(D->Verbose) printf("WD1793: COMMAND TIMED OUT\n");
          D->RDLength=D->WRLength=0;
          D->R[0] = (D->R[0]&~(F_DRQ|F_BUSY))|F_LOSTDATA;
          D->IRQ  = WD1793_IRQ;
        }
      /* Done */
      return(D->IRQ);
  }

  /* Bad register */
  return(0xFF);
}

/** Write1793() **********************************************/
/** Write value V into WD1793 register A. Returns DRQ/IRQ   **/
/** values.                                                 **/
/*************************************************************/
byte Write1793(register WD1793 *D,register byte A,register byte V)
{
  int J;

  switch(A)
  {
    case WD1793_COMMAND:
      /* Reset interrupt request */
      D->IRQ=0;
      /* If it is FORCE-IRQ command... */
      if((V&0xF0)==0xD0)
      {
        if(D->Verbose) printf("WD1793: FORCE-INTERRUPT (%02Xh)\n",V);
        /* Reset any executing command */
        D->RDLength=D->WRLength=0;
        D->Cmd=0xD0;
        /* Either reset BUSY flag or reset all flags if BUSY=0 */
        if(D->R[0]&F_BUSY) D->R[0]&=~F_BUSY;
        else               D->R[0]=(D->Track[D->Drive]? 0:F_TRACK0)|F_INDEX;
        /* Cause immediate interrupt if requested */
        if(V&C_IRQ) D->IRQ=WD1793_IRQ;
        /* Done */
        return(D->IRQ);
      }
      /* If busy, drop out */
      if(D->R[0]&F_BUSY) break;
      /* Reset status register */
      D->R[0]=0x00;
      D->Cmd=V;
      /* Depending on the command... */
      switch(V&0xF0)
      {
        case 0x00: /* RESTORE (seek track 0) */
          if(D->Verbose) printf("WD1793: RESTORE-TRACK-0 (%02Xh)\n",V);
          D->Track[D->Drive]=0;
          D->R[0] = F_INDEX|F_TRACK0|(V&C_LOADHEAD? F_HEADLOAD:0);
          D->R[1] = 0;
          D->IRQ  = WD1793_IRQ;
          break;

        case 0x10: /* SEEK */
          if(D->Verbose) printf("WD1793: SEEK-TRACK %d (%02Xh)\n",D->R[3],V);
          /* Reset any executing command */
          D->RDLength=D->WRLength=0;
          D->Track[D->Drive]=D->R[3];
          D->R[0] = F_INDEX
                  | (D->Track[D->Drive]? 0:F_TRACK0)
                  | (V&C_LOADHEAD? F_HEADLOAD:0);
          D->R[1] = D->Track[D->Drive];
          D->IRQ  = WD1793_IRQ;
          break;

        case 0x20: /* STEP */
        case 0x30: /* STEP-AND-UPDATE */
        case 0x40: /* STEP-IN */
        case 0x50: /* STEP-IN-AND-UPDATE */
        case 0x60: /* STEP-OUT */
        case 0x70: /* STEP-OUT-AND-UPDATE */
          if(D->Verbose) printf("WD1793: STEP%s%s (%02Xh)\n",
            V&0x40? (V&0x20? "-OUT":"-IN"):"",
            V&0x10? "-AND-UPDATE":"",
            V
          );
          /* Either store or fetch step direction */
          if(V&0x40) D->LastS=V&0x20; else V=(V&~0x20)|D->LastS;
          /* Step the head, update track register if requested */
          if(V&0x20) { if(D->Track[D->Drive]) --D->Track[D->Drive]; }
          else ++D->Track[D->Drive];
          /* Update track register if requested */
          if(V&C_SETTRACK) D->R[1]=D->Track[D->Drive];
          /* Update status register */
          D->R[0] = F_INDEX|(D->Track[D->Drive]? 0:F_TRACK0);
// @@@ WHY USING J HERE?
//                  | (J&&(V&C_VERIFY)? 0:F_SEEKERR);
          /* Generate IRQ */
          D->IRQ  = WD1793_IRQ;          
          break;

        case 0x80:
        case 0x90: /* READ-SECTORS */
          if(D->Verbose) printf("WD1793: READ-SECTOR%s %c:%d:%d:%d (%02Xh)\n",V&0x10? "S":"",D->Drive+'A',D->Side,D->R[1],D->R[2],V);
          /* Seek to the requested sector */
          D->Ptr=SeekFDI(
            D->Disk[D->Drive],D->Side,D->Track[D->Drive],
            V&C_SIDECOMP? !!(V&C_SIDE):D->Side,D->R[1],D->R[2]
          );
          /* If seek successful, set up reading operation */
          if(!D->Ptr)
          {
            if(D->Verbose) printf("WD1793: READ ERROR\n");
            D->R[0]     = (D->R[0]&~F_ERRCODE)|F_NOTFOUND;
            D->IRQ      = WD1793_IRQ;
          }
          else
          {
            D->RDLength = D->Disk[D->Drive]->SecSize
                        * (V&0x10? (D->Disk[D->Drive]->Sectors-D->R[2]+1):1);
            D->R[0]    |= F_BUSY|F_DRQ;
            D->IRQ      = WD1793_DRQ;
            D->Wait     = 255;
          }
          break;

        case 0xA0:
        case 0xB0: /* WRITE-SECTORS */
          if(D->Verbose) printf("WD1793: WRITE-SECTOR%s %c:%d:%d:%d (%02Xh)\n",V&0x10? "S":"",'A'+D->Drive,D->Side,D->R[1],D->R[2],V);
          /* Seek to the requested sector */
          D->Ptr=SeekFDI(
            D->Disk[D->Drive],D->Side,D->Track[D->Drive],
            V&C_SIDECOMP? !!(V&C_SIDE):D->Side,D->R[1],D->R[2]
          );
          /* If seek successful, set up writing operation */
          if(!D->Ptr)
          {
            if(D->Verbose) printf("WD1793: WRITE ERROR\n");
            D->R[0]     = (D->R[0]&~F_ERRCODE)|F_NOTFOUND;
            D->IRQ      = WD1793_IRQ;
          }
          else
          {
            D->WRLength = D->Disk[D->Drive]->SecSize
                        * (V&0x10? (D->Disk[D->Drive]->Sectors-D->R[2]+1):1);
            D->R[0]    |= F_BUSY|F_DRQ;
            D->IRQ      = WD1793_DRQ;
            D->Wait     = 255;
          }
          break;

        case 0xC0: /* READ-ADDRESS */
          if(D->Verbose) printf("WD1793: READ-ADDRESS (%02Xh)\n",V);
          /* Read first sector address from the track */
          if(!D->Disk[D->Drive]) D->Ptr=0;
          else
            for(J=0;J<256;++J)
            {
              D->Ptr=SeekFDI(
                D->Disk[D->Drive],
                D->Side,D->Track[D->Drive],
                D->Side,D->Track[D->Drive],J
              );
              if(D->Ptr) break;
            }
          /* If address found, initiate data transfer */
          if(!D->Ptr)
          {
            if(D->Verbose) printf("WD1793: READ-ADDRESS ERROR\n");
            D->R[0]    |= F_NOTFOUND;
            D->IRQ      = WD1793_IRQ;
          }
          else
          {
            D->Ptr      = D->Disk[D->Drive]->Header;
            D->RDLength = 6;
            D->R[0]    |= F_BUSY|F_DRQ;
            D->IRQ      = WD1793_DRQ;
            D->Wait     = 255;
          }
          break;

        case 0xE0: /* READ-TRACK */
          if(D->Verbose) printf("WD1793: READ-TRACK %d (%02Xh) UNSUPPORTED!\n",D->R[1],V);
          break;

        case 0xF0: /* WRITE-TRACK */
          if(D->Verbose) printf("WD1793: WRITE-TRACK %d (%02Xh) UNSUPPORTED!\n",D->R[1],V);
          break;

        default: /* UNKNOWN */
          if(D->Verbose) printf("WD1793: UNSUPPORTED OPERATION %02Xh!\n",V);
          break;
      }
      break;

    case WD1793_TRACK:
    case WD1793_SECTOR:
      if(!(D->R[0]&F_BUSY)) D->R[A]=V;
      break;

    case WD1793_SYSTEM:
// @@@ Too verbose
//      if(D->Verbose) printf("WD1793: Drive %c, %cD side %d\n",'A'+(V&S_DRIVE),V&S_DENSITY? 'D':'S',V&S_SIDE? 0:1);
      /* Reset controller if S_RESET goes up */
      if((D->R[4]^V)&V&S_RESET) Reset1793(D,D->Disk[0],WD1793_KEEP);
      /* Set disk #, side #, ignore the density (@@@) */
      D->Drive = V&S_DRIVE;
      D->Side  = !(V&S_SIDE);
      /* Save last written value */
      D->R[4]  = V;
      break;

    case WD1793_DATA:
      /* When writing data, store value to disk */
      if(!D->WRLength)
      { if(D->Verbose) printf("WD1793: EXTRA DATA WRITE (%02Xh)\n",V); }
      else
      {
        /* Write data */
        *D->Ptr++=V;
        /* Decrement length */
        if(--D->WRLength)
        {
          /* Reset timeout watchdog */
          D->Wait=255;
          /* Advance to the next sector as needed */
          if(!(D->WRLength&(D->Disk[D->Drive]->SecSize-1))) ++D->R[2];
        }
        else
        {
          /* Write completed */
          if(D->Verbose) printf("WD1793: WRITE COMPLETED\n");
          D->R[0]&= ~(F_DRQ|F_BUSY);
          D->IRQ  = WD1793_IRQ;
        }
      }
      /* Save last written value */
      D->R[A]=V;
      break;
  }

  /* Done */
  return(D->IRQ);
}
