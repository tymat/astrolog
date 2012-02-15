/*
** Astrolog (Version 5.41G) File: astrolog.h
**
** Code changed by Valentin Abramov (valja@tartu.customs.ee)
**
** IMPORTANT NOTICE: The graphics database and chart display routines
** used in this program are Copyright (C) 1991-1998 by Walter D. Pullen
** (Astara@msn.com, http://www.magitech.com/~cruiser1/astrolog.htm).
** Permission is granted to freely use and distribute these routines
** provided one doesn't sell, restrict, or profit from them in any way.
** Modification is allowed provided these notices remain with any
** altered or edited versions of the program.
**
** The main planetary calculation routines used in this program have
** been Copyrighted and the core of this program is basically a
** conversion to C of the routines created by James Neely as listed in
** Michael Erlewine's 'Manual of Computer Programming for Astrologers',
** available from Matrix Software. The copyright gives us permission to
** use the routines for personal use but not to sell them or profit from
** them in any way.
**
** The PostScript code within the core graphics routines are programmed
** and Copyright (C) 1992-1993 by Brian D. Willoughby
** (brianw@sounds.wa.com). Conditions are identical to those above.
**
** The extended accurate ephemeris databases and formulas are from the
** calculation routines in the library SWISS EPHEMERIS and are programmed and
** copyright 1998 by Astrodienst AG.
** The use of that source code is subject to
** the Swiss Ephemeris Public License, available at 
** http://www.astro.ch/swisseph. This copyright notice must not be 
** changed or removed by any user of this program.
**
** Initial programming 8/28,30, 9/10,13,16,20,23, 10/3,6,7, 11/7,10,21/1991.
** X Window graphics initially programmed 10/23-29/1991.
** PostScript graphics initially programmed 11/29-30/1992.
** Last code change made 12/20/1998.
** Modifications from version 5.40 to 5.41 are by Alois Treindl.
*/

/*
** TO COMPILE: For most systems, especially Unix, DOS, and Macs, the only
** changes that should need to be made to the code are to edit or comment the
** #define's below to equal the particulars of your own system and locale:
**
** SYSTEM SECTION: These settings describe platform and hardware specifics.
** They are all required to be set properly or the program likely won't
** compile or run. Some of these are technically optional and can be
** commented out even if your system would normally support them, e.g.
** the X11 graphics can be disabled even if you are running X windows.
*/

/*#define PC /* Comment out this #define if you have a Unix, Mac, or other */
           /* system that isn't a generic PC running DOS or MS Windows.  */

/*#define MAC /* Comment out this #define if you're not compiling for a Mac. */

/*#define X11 /* Comment out this #define if you don't have X windows, or */
            /* else have them and don't wish to compile in X graphics.  */

/*#define WIN /* Comment out this #define if you don't have MS Windows, or */
            /* else have them but want to compile a DOS version instead. */

/*#define MSG /* Comment out this #define if you don't have access to the     */
            /* Microsoft C PC graphics library as in graph.h, or you do and */
            /* have a PC and just don't wish to compile these graphics in.  */

/*#define BGI /* Comment out this #define if you don't have access to the  */
            /* Borland C BGI graphics library for PC's in graphics.h, or */
            /* you do and just don't wish to compile such graphics in.   */

/*#define MACG /* Comment out this #define if you don't have a Mac, or else  */
             /* have one and don't wish to compile in Mac screen graphics. */

/*#define MOUSE /* Comment out this #define if you don't have a mouse, or    */
              /* don't wish to compile in mouse tracking features. This is */
              /* only valid if X11, WIN, MSG, BGI, or MACG above are set.  */

#define TIME /* Comment out this #define if your compiler can't take the  */
             /* calls to the 'time' or 'localtime' functions as in time.h */

#define SWITCHES /* Comment out this #define if your system can not handle */
                 /* parameters on the command line (such as Mac's).        */

#define ENVIRON /* Comment out this #define if your system doesn't have  */
                /* environment variables or can't compile calls to them. */

/*#define ATOF /* Comment out this #define if you have a system in which  */
             /* 'atof' and related functions aren't defined in stdio.h, */
             /* such as most PC's, Linux, VMS compilers, and NeXT's.    */

#define PROTO /* Comment out this #define if you have an older compiler   */
              /* which doesn't allow full Ansi function prototypes. This  */
              /* is for programmers only and has no effect on executable. */

/*
** FEATURES SECTION: These settings describe features that are always
** available to be compiled into the program no matter what platform or
** hardware is available. Their settings are always optional.
*/

#define GRAPH /* Comment out this #define if you don't want any graphics   */
              /* in the program. This switch allows at least generation of */
              /* bitmap files and must be set if any of the more advanced  */
              /* graphics feature additions are also compiled in.          */

#define PLACALC /* Comment out this #define if you don't want the more */
                /* accurate calculation features and formulas to be    */
                /* compiled into the program (as accessed with -b).    */
                /* The old macro name is still used even with this version
                   of Astrolog uses the Swiss Ephemetis package. */

#define PS /* Comment out this #define if you don't want the ability to */
           /* generate charts in the PostScript graphics format.        */

#define META /* Comment out this #define if you don't want the ability to  */
             /* generate charts in the MS Windows metafile picture format. */

#define INTERPRET /* Comment out this #define if you don't want the ability */
                  /* to display interpretations of the various chart types. */

#define ARABIC /* Comment out this #define if you don't want any chart     */
               /* lists that include Arabic parts included in the program. */

#define CONSTEL /* Comment out this #define if you don't want any of the */
                /* astronomical constellation charts in the program.     */

#define BIORHYTHM /* Comment out this #define if you don't want the    */
                  /* non-astrological biorhythm charts in the program. */

#define INTERPRETALT        /*   V.A.     */

/*
** DATA CONFIGURATION SECTION: These settings describe particulars of
** your own location and where the program looks for certain info. It is
** recommended that these values be changed appropriately, although the
** program will still run if they are left alone.
*/

#ifndef PC
#define DEFAULT_DIR "~/astrolog"
#define EPHE_DIR "~/astrolog/ephe"
#else
#define DEFAULT_DIR "C:\\ASTROLOG"
#define EPHE_DIR DEFAULT_DIR
#endif
  /* Change this string to directory path program should look in for the  */
  /* astrolog.dat default file, if one is not in the current directory or */
  /* in the dirs indicated by environment variables. For PC systems, use  */
  /* two backslashes instead of one forward one to divide subdirectories. */

#define CHART_DIR DEFAULT_DIR
  /* This string is the directory the program looks in for chart info    */
  /* files (-i switch) if not in the current directory. This is normally */
  /* the default dir above but may be changed to be somewhere else.      */

  /* This string is the directory the program looks in for the ephemeris */
  /* files as accessed with the -b switch. This is normally the default  */
  /* dir above but may be changed to be somewhere else.                  */

#define DEFAULT_LONG -26.43 /* Change these values to the longitude west    */
#define DEFAULT_LAT   58.23 /* and latitude north of your current location. */
                            /* Use negative values for east/southern areas. */

#define DEFAULT_ZONE -2.00 /* Change this number to the time zone of your */
                          /* current location in hours before (west of)  */
                          /* GMT. Use negative values for eastern zones. */

/*
** OPTIONAL CONFIGURATION SECTION: Although not necessary, one may like
** to change some of the values below: These constants affect some of
** the default parameters and other such things.
*/

#define DEFAULT_SYSTEM 0 /* Normally, Placidus houses are used (unless the */
                         /* user specifies otherwise). If you want a       */
                         /* different default system, change this number   */
                         /* to a value from 0..9 (values same as in -c).   */

#define DEFAULT_ASPECTS 5 /* Default number of aspects to use in charts. */

#define DIVISIONS 48 /* Greater numbers means more accuracy but slower  */
                     /* calculation, of exact aspect and transit times. */

#define DEFAULT_INFOFILE "astrolog.dat"
  /* Name of file to look in for default program parameters (which will */
  /* override the compile time values here, if the file exists).        */

#define ENVIRONALL "ASTROLOG"
#define ENVIRONVER "ASTR"
  /* Name of environment variables to look in for chart, ephemeris, and  */
  /* default files. The second name is a version specific variable which */
  /* gets the current version appended to it before it is accessed.      */

#define WHEELCOLS 15   /* Affects width of each house in wheel display.    */
#define WHEELROWS 11   /* Max no. of objects that can be in a wheel house. */
#define SCREENWIDTH 80 /* Number of columns to print interpretations in.   */
#define MONTHSPACE 3   /* Number of spaces between each calendar column.   */
#define MAXINDAY 150   /* Max number of aspects or transits displayable.   */
#define MAXCROSS 750   /* Max number of latitude crossings displayable.    */
#define CREDITWIDTH 74 /* Number of text columns in the -Hc credit screen. */
#define MAXSWITCHES 32 /* Max number of switch parameters per input line.  */
#define PSGUTTER 9     /* Points of white space on PostScript page edge.   */


#ifdef GRAPH           /* For graphics, this char affects how bitmaps are */
#ifndef PC             /* written. 'N' is written like with the 'bitmap'  */
#define BITMAPMODE 'C' /* program, 'C' is compacted somewhat (files have  */
#else                  /* less spaces), and 'V' is compacted even more.   */
#define BITMAPMODE 'B' /* 'A' means write as rectangular Ascii text file. */
#endif                 /* 'B' means write as Windows bitmap (.bmp) file.  */

#ifdef MSG
#define DEFHIRESMODE _MAXRESMODE /* 'High-resolution' PC graphics mode. */
#define DEFLORESMODE _ERESCOLOR  /* 'Flicker-free' PC graphics mode.    */
#endif
#ifdef BGI
#define DEFHIRESMODE 0 /* Zero or less means a 'hi-res' mode of driver. */
#define DEFLORESMODE 1 /* One or more means a 'lo-res' mode of driver.  */
#endif
#endif /* GRAPH */

/*#define TRUENODE /* Comment out this #define if you'd prefer the 'Node'    */
                 /* object to refer to the Mean North Node of the Moon by  */
                 /* default as opposed to the True North Node of the Moon. */

/*
** By the time you reach here and the above values are customized as
** desired, Astrolog is ready to be compiled! Be sure to similarly
** change the values in the astrolog.dat file, which will override any
** corresponding compile time values here. Don't change any of the
** values in the section below unless you know what you're doing.
*/

#ifdef GRAPH
#define BITMAPX    2730 /* Maximum window size allowed */
#define BITMAPY    2730
#define BITMAPX1    180 /* Minimum window size allowed */
#define BITMAPY1    180
#define DEFAULTX    480 /* Default window size */
#define DEFAULTY    480
#define SIDESIZE    160 /* Size of wheel chart information sidebar.    */
#define MAXMETA 200000L /* Max bytes allowed in a metafile.            */
#define METAMUL      12 /* Metafile coordinate to chart pixel ratio.   */
#define PSMUL        11 /* PostScript coordinate to chart pixel ratio. */
#define CELLSIZE     14 /* Size for each cell in the aspect grid.      */
#define DEGINC        2 /* Number of degrees per segment for circles.  */
#define DEFORB      7.0 /* Min distance glyphs can be from each other. */
#define MAXSCALE    400 /* Max scale factor as passed to -Xs swtich.   */
#define TILTSTEP  11.25 /* Degrees to change when pressing '[' or ']'. */
#endif /* GRAPH */

#define BIODAYS      14 /* Days to include in graphic biorhythms.      */

#define chH    (char)(us.fAnsiChar ? 196 : '-')    /* Ansi and Ascii       */
#define chV    (char)(us.fAnsiChar ? 179 : '|')    /* characters used to   */
#define chC    (char)(us.fAnsiChar ? 197 : '|')    /* display text charts. */
#define chNW   (char)(us.fAnsiChar ? 218 : '+')
#define chNE   (char)(us.fAnsiChar ? 191 : '+')
#define chSW   (char)(us.fAnsiChar ? 192 : '+')
#define chSE   (char)(us.fAnsiChar ? 217 : '+')
#define chJN   (char)(us.fAnsiChar ? 193 : '-')
#define chJS   (char)(us.fAnsiChar ? 194 : '-')
#define chJW   (char)(us.fAnsiChar ? 180 : '|')
#define chJE   (char)(us.fAnsiChar ? 195 : '|')
#define chDeg0 (char)(us.fAnsiChar ? 248 : ' ')
#define chDeg1 (char)(us.fAnsiChar ? 248 : ':')
#define chMin1 (char)(us.fAnsiChar ?  39 : ':')
#define chSec1 (char)(us.fAnsiChar ?  34 : ' ')


/*
** One shouldn't ever need to change anything below this line to compile.
*/

#define ASTROLOG
#define MATRIX
/*#define BETA /* Uncomment to compile in beta message on startup. */
#include <stdio.h>
#ifndef ATOF
#include <stdlib.h>
#endif
#include <math.h>
#ifdef PC
#include <malloc.h>
#endif
#ifdef TIME
#include <time.h>
#endif

#ifdef X11
#define ISG
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif
#ifdef WIN
#define ISG
#include "windows.h"
#include "commdlg.h"
#include "print.h"
#include "resource.h"
#endif
#ifdef MSG
#define ISG
#define PCG
#include <graph.h>
#include <conio.h>
#endif
#ifdef BGI
#define ISG
#define PCG
#include <graphics.h>
#include <conio.h>
#endif
#ifdef MACG
#define ISG
#endif

#ifdef PS
#define STROKE
#endif
#ifdef META
#define STROKE
#endif
#ifdef MOUSE
#ifdef PC
#include <dos.h>
#endif
#endif /* MOUSE */


/*
** Make sure only legal combinations of the graphics options are active.
*/

#ifdef MAC
#ifdef SWITCHES
"If 'MAC' is defined 'SWITCHES' must not be as well"
#endif
#ifdef ENVIRON
"If 'MAC' is defined 'ENVIRON' must not be as well"
#endif
#endif /* MAC */

#ifdef X11
#ifndef GRAPH
"If 'X11' is defined 'GRAPH' must be too"
#endif
#ifdef WIN
"If 'X11' is defined 'WIN' must not be as well"
#endif
#ifdef MSG
"If 'X11' is defined 'MSG' must not be as well"
#endif
#ifdef BGI
"If 'X11' is defined 'BGI' must not be as well"
#endif
#ifdef MACG
"If 'X11' is defined 'MACG' must not be as well"
#endif
#ifdef PC
"If 'X11' is defined 'PC' must not be as well"
#endif
#endif /* X11 */

#ifdef WIN
#ifndef GRAPH
"If 'WIN' is defined 'GRAPH' must be too"
#endif
#ifdef X11
"If 'WIN' is defined 'X11' must not be as well"
#endif
#ifdef MSG
"If 'WIN' is defined 'MSG' must not be as well"
#endif
#ifdef BGI
"If 'WIN' is defined 'BGI' must not be as well"
#endif
#ifdef MACG
"If 'WIN' is defined 'MACG' must not be as well"
#endif
#ifndef PC
"If 'WIN' is defined 'PC' must be too"
#endif
#endif /* WIN */

#ifdef MSG
#ifndef GRAPH
"If 'MSG' is defined 'GRAPH' must be too"
#endif
#ifdef X11
"If 'MSG' is defined 'X11' must not be as well"
#endif
#ifdef WIN
"If 'MSG' is defined 'WIN' must not be as well"
#endif
#ifdef BGI
"If 'MSG' is defined 'BGI' must not be as well"
#endif
#ifdef MACG
"If 'MSG' is defined 'MACG' must not be as well"
#endif
#ifndef PC
"If 'MSG' is defined 'PC' must be too"
#endif
#endif /* MSG */

#ifdef BGI
#ifndef GRAPH
"If 'BGI' is defined 'GRAPH' must be too"
#endif
#ifdef X11
"If 'BGI' is defined 'X11' must not be as well"
#endif
#ifdef WIN
"If 'BGI' is defined 'WIN' must not be as well"
#endif
#ifdef MSG
"If 'BGI' is defined 'MSG' must not be as well"
#endif
#ifdef MACG
"If 'BGI' is defined 'MACG' must not be as well"
#endif
#ifndef PC
"If 'BGI' is defined 'PC' must be too"
#endif
#endif /* BGI */

#ifdef MACG
#ifndef GRAPH
"If 'MACG' is defined 'GRAPH' must be too"
#endif
#ifndef MAC
"If 'MACG' is defined 'MAC' must be too"
#endif
#ifdef X11
"If 'MACG' is defined 'X11' must not be as well"
#endif
#ifdef WIN
"If 'MACG' is defined 'WIN' must not be as well"
#endif
#ifdef MSG
"If 'MACG' is defined 'MSG' must not be as well"
#endif
#ifdef BGI
"If 'MACG' is defined 'BGI' must not be as well"
#endif
#ifdef PC
"If 'MACG' is defined 'PC' must not be as well"
#endif
#endif /* MACG */

#ifdef MOUSE
#ifdef GRAPH
#ifndef ISG
"If 'MOUSE' is defined 'X11', 'WIN', 'MSG', 'BGI', or 'MACG' must be too"
#endif
#endif /* GRAPH */
#endif /* MOUSE */

#ifdef PS
#ifndef GRAPH
"If 'PS' is defined 'GRAPH' must be too"
#endif
#endif /* PS */

#ifdef META
#ifndef GRAPH
"If 'META' is defined 'GRAPH' must be too"
#endif
#endif /* META */

/*
******************************************************************************
** Program Constants.
******************************************************************************
*/

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif
#define fFalse FALSE
#define fTrue  TRUE

#define szAppNameCore "Astrolog"
#define szVersionCore "5.41G"
#define szDateCore    "Nov 1999"
#define szAddressCore \
  "Astara@msn.com - http://www.magitech.com/~cruiser1/astrolog.htm"
#define szNowCore     "now"
#define szTtyCore     "tty"
#define szSetCore     "set"
#define szNulCore     "nul"

#define cchSzDef   80
#define cchSzMax   255
#define nDegMax    360
#define nDegHalf   180
#define yeaJ2G     1582
#define monJ2G     mOct
#define dayJ2G1    4
#define dayJ2G2    15

#define rPi        3.14159265358979323846
#define rPi2       (rPi*2.0)
#define rPiHalf    (rPi/2.0)
#define rDegMax    360.0
#define rDegHalf   180.0
#define rDegQuad   90.0
#define rDegRad    (rDegHalf/rPi)
#define rEpoch2000 (-24.736467)
/* #define rAxis      23.44578889 */
#define rAxis      DFromR(is.OB)
#define rSmall     (1.7453E-09)
#define rLarge     10000.0
#define rRound     0.5

#define chNull     '\0'
#define chEscape   '\33'
#define chBell     '\7'
#define chReturn   '\r'
#define chTab      '\t'
#define chDelete   '\b'
#define chBreak    '\3'
#define chRet      'R'

/* Array index limits */

#define cSign      12
#define cObj       115
#define cObjInt    uranHi
#define objMax     (cObj+1)
#define cAspect    18
#define cAspectInt 11
#define cSystem    15
#define cCnstl     88
#define cZone      69
#define cSector    36
#define cPart      177
#define cAspConfig 8
#define cWeek      7
#define cColor     16
#define xFont      6
#define yFont      10
#define xFontT     (xFont*gi.nScaleT)
#define yFontT     (yFont*gi.nScaleT)
#define xSideT     (SIDESIZE*gi.nScaleT)

/* Object array index values */

#define cPlanet oVes
#define cThing  oLil
#define oMain   10
#define oCore   20
#define cUran   9
#define cStar   74
#define cuspLo  21
#define cuspHi  32
#define uranLo  33
#define uranHi  41
#define oNorm   uranHi
#define starLo  42
#define starHi  cObj
#define starMaxName 12
#define starMaxNName 8

/* Month index values */

#define mJan 1
#define mFeb 2
#define mMar 3
#define mApr 4
#define mMay 5
#define mJun 6
#define mJul 7
#define mAug 8
#define mSep 9
#define mOct 10
#define mNov 11
#define mDec 12

/* Elements */

#define eFir 0
#define eEar 1
#define eAir 2
#define eWat 3

/* Zodiac signs */

#define sAri 1
#define sTau 2
#define sGem 3
#define sCan 4
#define sLeo 5
#define sVir 6
#define sLib 7
#define sSco 8
#define sSag 9
#define sCap 10
#define sAqu 11
#define sPis 12

/* Objects */

#define oEar 0
#define oSun 1
#define oMoo 2
#define oMer 3
#define oVen 4
#define oMar 5
#define oJup 6
#define oSat 7
#define oUra 8
#define oNep 9
#define oPlu 10
#define oChi 11
#define oCer 12
#define oVes 15
#define oNod 16
#define oLil 17
#define oSou 17
#define oFor 18
#define oVtx 19
#define oEP  20
#define oAsc 21
#define oNad 24
#define oDes 27
#define oMC  30
#define oOri (starLo-1+10)
#define oAnd (starLo-1+47)

/* Aspects */

#define aDir -2
#define aSig -1
#define aCon 1
#define aOpp 2
#define aSqu 3
#define aTri 4
#define aSex 5
#define aInc 6
#define aSSx 7
#define aSSq 8
#define aSes 9
#define aQui 10
#define aBQn 11

/* Biorhythm cycle constants */

#define brPhy 23.0
#define brEmo 28.0
#define brInt 33.0

/* Relationship chart modes */

#define rcSynastry   1
#define rcComposite  2
#define rcMidpoint   3
#define rcDifference 4
#define rcBiorhythm  5
#define rcDual      -1
#define rcTriWheel  -2
#define rcQuadWheel -3
#define rcTransit   -4
#define rcProgress  -5

/* Aspect configurations */

#define acS  1
#define acGT 2
#define acTS 3
#define acY  4
#define acGC 5
#define acC  6
#define acMR 7
#define acK  8

/* Graphics chart modes */

#define gWheel      1
#define gHouse      2
#define gGrid       3
#define gHorizon    4
#define gOrbit      5
#define gSector     6
#define gCalendar   7
#define gDisposit   8
#define gAstroGraph 9
#define gEphemeris 10
#define gWorldMap  11
#define gGlobe     12
#define gPolar     13
#define gBiorhythm 14
#ifdef WIN
#define gAspect    15
#define gMidpoint  16
#define gArabic    17
#define gSign      18
#define gObject    19
#define gHelpAsp   20
#define gConstel   21
#define gPlanet    22
#define gMeaning   23
#define gSwitch    24
#define gObscure   25
#define gKeystroke 26
#define gCredit    27
#define gRising    28
#define gTraTraHit 29
#define gTraTraInf 30
#define gTraNatHit 31
#define gTraNatInf 32
#endif

/* Colors */

#define kReverse -2
#define kDefault -1
#define kBlack   0
#define kMaroon  1
#define kDkGreen 2
#define kOrange  3
#define kDkBlue  4
#define kPurple  5
#define kDkCyan  6
#define kLtGray  7
#define kDkGray  8
#define kRed     9
#define kGreen   10
#define kYellow  11
#define kBlue    12
#define kMagenta 13
#define kCyan    14
#define kWhite   15
#define kNull    16

/* Arabic parts */

#define apFor 0
#define apSpi 1

/* Draw text formatting flags */

#define dtCent   0x0
#define dtLeft   0x1
#define dtBottom 0x2
#define dtErase  0x4
#define dtScale  0x8
#define dtTop    0x10

/* User string parse modes */

#define pmMon     1
#define pmDay     2
#define pmYea     3
#define pmTim     4
#define pmDst     5
#define pmZon     6
#define pmLon     7
#define pmLat     8
#define pmObject  9
#define pmAspect 10
#define pmSystem 11
#define pmSign   12
#define pmColor  13

/* PC mouse flags */

#define mfLeft   0x01
#define mfRight  0x02
#define mfMiddle 0x04

/* Termination codes */

#define tcError -1
#define tcOK    0
#define tcFatal 1
#define tcForce 2

#ifndef _ZRES256COLOR
#define _ZRES256COLOR 263
#endif


/*
******************************************************************************
** Macro Functions.
******************************************************************************
*/

#define BLo(w) ((byte)(w))
#define BHi(w) ((byte)((word)(w) >> 8 & 0xFF))
#define WLo(l) ((word)(dword)(l))
#define WHi(l) ((word)((dword)(l) >> 16 & 0xFFFF))
#define WFromBB(bLo, bHi) ((word)BLo(bLo) | (word)((byte)(bHi)) << 8)
#define LFromWW(wLo, wHi) ((dword)WLo(wLo) | (dword)((word)(wHi)) << 16)
#define LFromBB(b1, b2, b3, b4) LFromWW(WFromBB(b1, b2), WFromBB(b3, b4))
#ifndef WIN
#define RGB(bR, bG, bB) \
  ((dword)((byte)(bR) | (word)(bG)<<8 | (dword)(byte)(bB)<<16))
#endif
#define RGBR(l) BLo(l)
#define RGBG(l) BHi(l)
#define RGBB(l) ((byte)((dword)(l) >> 16 & 0xFF))
#define ChHex(n) (char)((n) < 10 ? '0' + (n) : 'a' + (n) - 10)
#define VgaFromEga(x) NMultDiv((x), 480, 350)
#define VgaFromCga(x) NMultDiv((x), 480, 200)

#define Max(v1, v2) ((v1) > (v2) ? (v1) : (v2))
#define Min(v1, v2) ((v1) < (v2) ? (v1) : (v2))
#define NSgn(n) ((n) < 0 ? -1 : (n) > 0)
#define RSgn2(r) ((r) < 0.0 ? -1.0 : 1.0)
#define FBetween(v, v1, v2) ((v) >= (v1) && (v) <= (v2))
#define RFract(r) ((r) - RFloor(r))
#define ChCap(ch) ((ch) >= 'a' && (ch) <= 'z' ? (ch) - 'a' + 'A' : (ch))
#define ChUncap(ch) (FCapCh(ch) ? (ch) - 'A' + 'a' : (ch))
#define FCapCh(ch) ((ch) >= 'A' && (ch) <= 'Z')
#define FNumCh(ch) ((ch) >= '0' && (ch) <= '9')
#define NMultDiv(n1, n2, n3) ((int)((long)(n1) * (n2) / (n3)))
#define Ratio(v1, v2, v3) ((v1) + ((v2) - (v1)) * (v3))
#define ZFromS(s) ((real)(((s)-1)*30))
#define SFromZ(r) (((int)(r))/30+1)
#define RFromD(r) ((r)/rDegRad)
#define DFromR(r) ((r)*rDegRad)
#define GFromO(o) ((rDegMax - planet[o])/10.0)
#define RAbs(r) fabs(r)
#define RFloor(r) floor(r)
#define NFloor(r) ((int)RFloor(r))
#define RSqr(r) sqrt(r)
#define RSin(r) sin(r)
#define RCos(r) cos(r)
#ifndef PC
#define RTan(r) tan(r)
#else
/* The tangent function in some PC compilers returns a result the negative */
/* of what it should be when doing emulation for a non-math chip machine.  */
#define RTan(r) (sin(r)/cos(r))
#endif
#define RAtn(r) atan(r)
#define RAsin(r) asin(r)
#define RAcos(r) acos(r)
#define RSinD(r) RSin(RFromD(r))
#define RCosD(r) RCos(RFromD(r))
#define NSinD(nR, nD) ((int)((real)(nR)*RSinD((real)nD)))
#define NCosD(nR, nD) ((int)((real)(nR)*RCosD((real)nD)))

#define FItem(obj)    FBetween(obj, 0, cObj)
#define FNorm(obj)    FBetween(obj, 0, oNorm)
#define FCusp(obj)    FBetween(obj, cuspLo, cuspHi)
#define FAngle(obj)   (FCusp(obj) && (obj)%3 == 0)
#define FMinor(obj)   (FCusp(obj) && (obj)%3 != 0)
#define FUranian(obj) FBetween(obj, uranLo, uranHi)
#define FStar(obj)    FBetween(obj, starLo, starHi)
#define FObject(obj)  ((obj) <= oVes || (obj) >= uranLo)
#define FThing(obj)   ((obj) <= cThing || (obj) >= uranLo)
#define FHelio(obj)   (FNorm(obj) && FObject(obj) && (obj) != oMoo)
#define FAspect(asp)  FBetween(asp, 1, us.nAsp)
#define FSector(s)    FBetween(s, 1, cSector)
#define ChDst(dst)    (dst == 0.0 ? 'S' : (dst == 1.0 ? 'D' : 'A'))
#define ChDashF(f)    (f ? '=' : '_')
#define SzNumF(f)     (f ? "1 " : "0 ")
#define DayInYearHi(yea) (365-28+DayInMonth(2, yea))
#define FChSwitch(ch) \
  ((ch) == '-' || (ch) == '/' || (ch) == '_' || (ch) == '=' || (ch) == ':')

#define FValidMon(mon) FBetween(mon, 1, 12)
#define FValidDay(day, mon, yea) ((day) >= 1 && (day) <= DayInMonth(mon, yea))
#define FValidYea(yea) FBetween(yea, -20000, 20000)
#define FValidTim(tim) ((tim) > -2.0 && (tim) < 24.0 && \
  RFract(RAbs(tim)) < 0.60)
#define FValidDst(dst) FValidZon(dst)
#define FValidZon(zon) FBetween(zon, -24.0, 24.0)
#define FValidLon(lon) FBetween(lon, -rDegHalf, rDegHalf)
#define FValidLat(lat) FBetween(lat, -rDegQuad, rDegQuad)
#define FValidAspect(asp) FBetween(asp, 0, cAspect)
#define FValidSystem(n) FBetween(n, 0, cSystem-1)
#define FValidDivision(n) FBetween(n, 1, 2880)
#define FValidOffset(r) FBetween(r, -rDegMax, rDegMax)
#define FValidCenter(obj) \
  (FBetween(obj, oEar, uranHi) && FObject(obj) && (obj) != oMoo)
#define FValidHarmonic(n) FBetween(n, 1, 30000)
#define FValidWheel(n) FBetween(n, 1, WHEELROWS)
#define FValidAstrograph(n) (n > 0 && 160%n == 0)
#define FValidPart(n) FBetween(n, 1, cPart)
#define FValidBioday(n) FBetween(n, 1, 199)
#define FValidScreen(n) FBetween(n, 20, 200)
#define FValidMacro(n) FBetween(n, 1, 48)
#define FValidTextrows(n) ((n) == 25 || (n) == 43 || (n) == 50)
#define FValidGlyphs(n) FBetween(n, 0, 2223)
#define FValidGrid(n) FBetween(n, 1, cObj)
#define FValidScale(n) (FBetween(n, 100, MAXSCALE) && (n)%100 == 0)
#define FValidGraphx(x) (FBetween(x, BITMAPX1, BITMAPX) || (x) == 0)
#define FValidGraphy(y) (FBetween(y, BITMAPY1, BITMAPY) || (y) == 0)
#define FValidRotation(n) FBetween(n, 0, nDegMax-1)
#define FValidTilt(n) FBetween(n, -rDegQuad, rDegQuad)
#define FValidColor(n) FBetween(n, 0, cColor - 1)
#define FValidBmpmode(ch) \
  ((ch) == 'N' || (ch) == 'C' || (ch) == 'V' || (ch) == 'A' || (ch) == 'B')
#define FValidTimer(n) FBetween(n, 1, 32000)
#define FValidStarAspects(n) FBetween(n, 0, 18)
#define FValidStarOrb(r) FBetween(r, 0.2, 4.0)
#define FValidCoeffTens(r) FBetween(r, 0.2, 4.0)
#define FValidCoeffComp(r) FBetween(r, rSmall, 1000.0)

#define chSig3(A) szSignName[A][0], szSignName[A][1], szSignName[A][2]
#define chObj3(A) szObjName[A][0], szObjName[A][1], szObjName[A][2]
#define chMon3(A) szMonth[A][0], szMonth[A][1], szMonth[A][2]
#define chDay3(A) szDay[A][0], szDay[A][1], szDay[A][2]
#define kSignA(s) kElemA[(s)-1 & 3]
#define kSignB(s) kElemB[(s)-1 & 3]
#define kModeA(m) kElemA[m <= 1 ? m : eWat]
#define szPerson  (ciMain.nam[0] ? ciMain.nam : "This person")
#define szPerson0 (ciMain.nam[0] ? ciMain.nam : "the person")
#define szPerson1 (ciMain.nam[0] ? ciMain.nam : "Person1")
#define szPerson2 (ciTwin.nam[0] ? ciTwin.nam : "Person2")
#define FIgnore(i) (ignore[i] || (i) == us.objCenter)
#define FIgnore2(i) (ignore2[i] || (i) == us.objCenter)
#define FIgnore3(i) (ignore3[i] || (i) == us.objCenter)
#define fSouthNode (!us.fPlacalc || us.objCenter != oEar || forceSN)
#define fNoTimeOrSpace (Mon == -1)

#define loop for (;;)
#define not(V) V = !(V)
#define neg(V) V = -(V)
#define PrintL() PrintCh('\n')
#define PrintL2() PrintSz("\n\n")
#define PrintF(sz) fprintf(file, sz)
#define PrintFSz() fprintf(file, sz)
#define SwapN(n1, n2) (n1)^=(n2)^=(n1)^=(n2)
#define FSwitchF(f) ((((f) | fOr) & !fAnd) ^ fNot)
#define SwitchF(f) f = FSwitchF(f)
#define SwitchF2(f) f = (((f) | (fOr || fNot)) & !fAnd)
#define SetCI(ci, M, D, Y, T, S, Z, O, A) \
  ci.mon = M; ci.day = D; ci.yea = Y; \
  ci.tim = T; ci.dst = S; ci.zon = Z; ci.lon = O; ci.lat = A

#ifndef PC
#define chDirSep '/'
#define chSwitch '-'
#define CONST
#define NPTR
#define FPTR
#define HPTR
#define AllocateNear(p, cb) (p) = (void *)malloc(cb)
#define AllocateFar(p, cb)  AllocateNear(p, cb)
#define AllocateHuge(p, cb) AllocateFar(p, cb)
#define DeallocateNear(p) free(p)
#define DeallocateFar(p)  DeallocateNear(p)
#define DeallocateHuge(p) DeallocateFar(p)
#ifndef MAC
#define ldTime 2440588L
#else
#define ldTime 2416481L
#endif
#define ARR
#else /* PC */
#define chDirSep '\\'
#define chSwitch '/'
#define CONST const
#define EXPORT _export
#ifndef WIN
#define NPTR near
#define FPTR far
#else
#define NPTR
#define FPTR
#endif
#define HPTR huge
#define AllocateNear(p, cb) (p) = malloc(cb)
#define AllocateFar(p, cb)  (p) = _fmalloc(cb)
#define AllocateHuge(p, cb) (p) = halloc(cb, sizeof(byte))
#define DeallocateNear(p) free(p)
#define DeallocateFar(p)  _ffree(p)
#define DeallocateHuge(p) hfree(p)
#define ldTime 2440588L
#ifndef __TURBOC__
#define ARR
#else
#define ARR far
#endif
#endif /* PC */

#ifdef GRAPH
#ifdef WIN
#define API FAR PASCAL
#define hdcNil ((HDC)NULL)
#endif
#ifdef MSG
#define xPcScreen gi.cfg.numxpixels
#define yPcScreen gi.cfg.numypixels
#define FValidResmode(n) FBetween(n, _MAXRESMODE, _ZRES256COLOR)
#define FEgaRes(res) (res == _ERESNOCOLOR || res == _ERESCOLOR)
#define FCgaRes(res) (res == _HRESBW || res == _HRES16COLOR)
#define PcMoveTo(x, y) _moveto(x, y)
#define PcLineTo(x, y) _lineto(x, y)
#define PcSetTextRows(n) _settextrows(gs.nTextRows)
#endif
#ifdef BGI
#define xPcScreen (getmaxx()+1)
#define yPcScreen (getmaxy()+1)
#define FValidResmode(n) FBetween(n, -500, 500)
#define FEgaRes(res) (yPcScreen == 350)
#define FCgaRes(res) (yPcScreen == 200)
#define PcMoveTo(x, y) moveto(x, y)
#define PcLineTo(x, y) lineto(x, y)
#define PcSetTextRows(n) if ((n) > 25) textmode(C4350)
#endif


/*(!(us.nRel == rcTransit ? ignore2[i] : ignore[i]) && i != us.objCenter) */
/* Should an object in the outer wheel be restricted? */
#define FProper2(i) \
(!(us.nRel == rcProgress ? ignore3[i] : (us.nRel == rcTransit ? ignore2[i] : ignore[i])) && i != us.objCenter)

/* Are particular coordinates on the chart? */
#define FInRect(x, y, x1, y1, x2, y2) \
  ((x) >= (x1) && (x) < (x2) && (y) >= (y1) && (y) < (y2))
#define FOnWin(X, Y) FInRect((X), (Y), 0, 0, gs.xWin, gs.yWin)

/* Get a coordinate based on chart radius, a fraction, and (co)sin value. */
#define POINT1(U, R, S) ((int)(((U)+1.4)*(R)*(S)))
#define POINT2(U, R, S) ((int)(((U)-0.3)*(R)*(S)))

/* Determine (co)sin factors based on zodiac angle and chart orientation. */
#define PX(A) RCosD(A)
#define PY(A) RSinD(A)
#define PZ(A) PlaceInX(A)

/* Compute Mollewide projection in pixel scale given latitude. */
#define NMollewide(y) \
  ((int)(RSqr((real)(180L*nScl*180L*nScl - 4L*(y)*nScl*(y)*nScl))+rRound))

/* Do settings indicate the current chart should have the info sidebar? */
#define fSidebar ((gi.nMode == gWheel || gi.nMode == gHouse || \
  gi.nMode == gSector) && gs.fText && !us.fVelocity)

/* Is the current chart most properly displayed as a square graphic? */
#define fSquare \
  (gi.nMode == gWheel || gi.nMode == gHouse || gi.nMode == gGrid || \
  (gi.nMode == gHorizon && us.fPrimeVert) || gi.nMode == gDisposit || \
  gi.nMode == gOrbit || gi.nMode == gGlobe || gi.nMode == gPolar)

/* Does the current chart have to be displayed in a map rectangle? */
#define fMap \
  (gi.nMode == gAstroGraph || gi.nMode == gWorldMap)

/* Do settings indicate the current chart should have an outer border? */
#define fDrawBorder \
  ((gs.fBorder || gi.nMode == gGrid) && gi.nMode != gGlobe && \
  gi.nMode != gPolar && (gi.nMode != gWorldMap || !gs.fMollewide))

/* Do settings indicate current chart should have chart info at its bottom? */
#define fDrawText \
  (gs.fText && gi.nMode != gCalendar && gi.nMode != gWorldMap && \
  gi.nMode != gGlobe && gi.nMode != gPolar && ((gi.nMode != gWheel && \
  gi.nMode != gHouse && gi.nMode != gSector) || us.fVelocity))
#endif /* GRAPH */


/*
******************************************************************************
** Type Definitions.
******************************************************************************
*/

#define byte  unsigned char
#define word  unsigned short
#define dword unsigned long
#define word4 long
#define real  double
#define _char unsigned char
#define _int  unsigned int
#define bool  int
#define _bool char
#define lpbyte byte FPTR *
#define hpbyte byte HPTR *
#define lpreal real FPTR *

typedef struct _GridInfo {
  byte  n[objMax][objMax];
  short v[objMax][objMax];
} GridInfo;

typedef struct _CrossInfo {
  real lat[MAXCROSS];
  real lon[MAXCROSS];
  int obj1[MAXCROSS];
  int obj2[MAXCROSS];
} CrossInfo;

#ifdef GRAPH
#define KV unsigned long
#define KI int
#endif /* GRAPH */

typedef struct _UserSettings {

  /* Chart types */
  _bool fListing;       /* -v */
  _bool fWheel;         /* -w */
  _bool fGrid;          /* -g */
  _bool fAspList;       /* -a */
  _bool fMidpoint;      /* -m */
  _bool fHorizon;       /* -Z */
  _bool fOrbit;         /* -S */
  _bool fSector;        /* -l */
  _bool fInfluence;     /* -j */
  _bool fAstroGraph;    /* -L */
  _bool fCalendar;      /* -K */
  _bool fInDay;         /* -d */
  _bool fInDayInf;      /* -D */
  _bool fEphemeris;     /* -E */
  _bool fTransit;       /* -t */
  _bool fTransitInf;    /* -T */

  /* Chart suboptions */
  _bool fVelocity;      /* -v0 */
  _bool fWheelReverse;  /* -w0 */
  _bool fGridConfig;    /* -g0 */
  _bool fAppSep;        /* -ga */
  _bool fParallel;      /* -gp */
  _bool fAspSummary;    /* -a0 */
  _bool fMidSummary;    /* -m0 */
  _bool fMidAspect;     /* -ma */
  _bool fPrimeVert;     /* -Z0 */
  _bool fHorizonSearch; /* -Zd */
  _bool fSectorApprox;  /* -l0 */
  _bool fInfluenceSign; /* -j0 */
  _bool fLatitudeCross; /* -L0 */
  _bool fCalendarYear;  /* -Ky */
  _bool fInDayMonth;    /* -dm */
  _bool fArabicFlip;    /* -P0 */

  /* Table chart types */
  _bool fCredit;        /* -Hc */
  _bool fSwitch;        /* -H  */
  _bool fSwitchRare;    /* -Y  */
  _bool fKeyGraph;      /* -HX */
  _bool fSign;          /* -HC */
  _bool fObject;        /* -HO */
  _bool fAspect;        /* -HA */
  _bool fConstel;       /* -HF */
  _bool fOrbitData;     /* -HS */
  _bool fMeaning;       /* -HI */

  /* Main flags */
  _bool fLoop;        /* -Q */
  _bool fSidereal;    /* -s */
  _bool fCusp;        /* -C */
  _bool fUranian;     /* -u */
  _bool fProgress;    /* Are we doing a -p progressed chart?           */
  _bool fInterpret;   /* Is -I interpretation switch in effect?        */
  _bool fDecan;       /* -3 */
  _bool fFlip;        /* -f */
  _bool fGeodetic;    /* -G */
  _bool fVedic;       /* -J */
  _bool fNavamsa;     /* -9 */
  _bool fPlacalc;     /* -b */
  _bool fWriteFile;   /* -o */
  _bool fAnsiColor;   /* -k */
  _bool fGraphics;    /* -X */

  /* Main subflags */
  _bool fNoSwitches;
  _bool fLoopInit;    /* -Q0 */
  _bool fSeconds;     /* -b0 */
  _bool fPlacalcAst;  /* -ba */
  _bool fEquator;     /* -sr */
  _bool fSolarArc;    /* -p0 */
  _bool fWritePos;    /* -o0 */
  _bool fAnsiChar;    /* -k0 */

  /* Rare flags */
  _bool fTrueNode;    /* -Yn */
  _bool fEuroDate;    /* -Yd */
  _bool fEuroTime;    /* -Yt */
  _bool fSmartCusp;   /* -YC */
  _bool fClip80;      /* -Y8 */
  _bool fWriteOld;    /* -Yo */
  _bool fHouseAngle;  /* -Yc */
  _bool fIgnoreSign;  /* -YR0 */
  _bool fIgnoreDir;   /* -YR0 */
  _bool fNoWrite;     /* -0o */
  _bool fNoRead;      /* -0i */
  _bool fNoQuit;      /* -0q */
  _bool fNoGraphics;  /* -0X */

  /* Value settings */
  int   nEphemYears;  /* -Ey */
  int   nArabic;      /* -P */
  int   nRel;         /* What relationship chart are we doing, if any? */
  int   nHouseSystem; /* -c */
  int   nAsp;         /* -A */
  int   objCenter;    /* -h */
  int   nStar;        /* -U */
  int   nHarmonic;    /* Harmonic chart value passed to -x switch.     */
  int   objOnAsc;     /* House value passed to -1 or -2 switch.        */
  int   dayDelta;     /* -+, -- */
  int   nDegForm;     /* -s */
  int   nDivision;    /* -d */
  int   nScreenWidth; /* -I */
  real  dstDef;       /* -z0 */
  real  zonDef;       /* -z  */
  real  lonDef;       /* -l  */
  real  latDef;       /* -l  */

  /* Value subsettings */
  int   nWheelRows;      /* Number of rows per house to use for -w wheel. */
  int   nAstroGraphStep; /* Latitude step rate passed to -L switch.       */
  int   nArabicParts;    /* Arabic parts to include value passed to -P.   */
  real  rZodiacOffset;   /* Position shifting value passed to -s switch.  */
  real  rProgDay;        /* Progression day value passed to -pd switch.   */
  int   nRatio1;         /* Chart ratio factors passed to -rc or -rm.     */
  int   nRatio2;
  int   nScrollRow;      /* -YQ */
  long  lTimeAddition;   /* -Yz */
  int   nArabicNight;    /* -YP */
  int   nBioday;         /* -Yb */
} US;

typedef struct _InternalSettings {
  _bool fHaveInfo;    /* Do we need to prompt user for chart info?         */
  _bool fProgress;    /* Are we doing a chart involving progression?       */
  _bool fReturn;      /* Are we doing a transit chart for returns?         */
  _bool fMult;        /* Have we already printed at least one text chart?  */
  _bool fSeconds;     /* Do we print locations to nearest second?          */
  _bool fSzPersist;   /* Are parameter strings persistent when processing? */
  _bool fSzInteract;  /* Are we in middle of chart so some setting fixed?  */
  _bool fNoEphFile;   /* Have we already had a ephem file not found error? */
  char *szProgName;   /* The name and path of the executable running.      */
  char *szFileScreen; /* The file to send text output to as passed to -os. */
  char *szFileOut;    /* The output chart filename string as passed to -o. */
  char **rgszComment; /* Points to any comment strings after -o filename.  */
  int cszComment;     /* The number of strings after -o that are comments. */
  int cchCol;         /* The current column text charts are printing at.   */
  int cchRow;         /* The current row text charts have scrolled to.     */
  real rSid;          /* Sidereal offset degrees to be added to locations. */
  real JD;            /* Fractional Julian day for current chart.          */
  real JDp;           /* Julian day that a progressed chart indicates.     */
  FILE *S;  /* File to write text to.   */
  real T;   /* Julian time for chart.   */
  real MC;  /* Midheaven at chart time. */
  real Asc; /* Ascendant at chart time. */
  real RA;  /* Right ascension at time. */
  real OB;  /* Obliquity of ecliptic.   */
} IS;

typedef struct _ChartInfo {
  int mon;   /* Month            */
  int day;   /* Day              */
  int yea;   /* Year             */
  real tim;  /* Time in hours    */
  real dst;  /* Daylight offset  */
  real zon;  /* Time zone        */
  real lon;  /* Longitude        */
  real lat;  /* Latitude         */
  byte *nam; /* Name for chart   */
  byte *loc; /* Name of location */
} CI;

typedef struct _ChartPositions {
  real obj[objMax];   /* The zodiac positions.    */
  real alt[objMax];   /* Ecliptic declination.    */
  real dir[objMax];   /* Retrogradation velocity. */
  real altdir[objMax];  /* Speed in declination  */
  real cusp[cSign+1]; /* House cusp positions.    */
  byte house[objMax]; /* House each object is in. */
} CP;

#ifdef GRAPH
typedef struct _GraphicsSettings {
  _bool fBitmap;    /* Are we creating a bitmap file (-Xb set).         */
  _bool fPS;        /* Are we generating a PostScript file (-Xp set).   */
  _bool fMeta;      /* Are we generating a metafile graphic (-XM set).  */
  _bool fColor;     /* Are we drawing a color chart (-Xm not set).      */
  _bool fInverse;   /* Are we drawing in reverse video (-Xr set).       */
  _bool fRoot;      /* Are we drawing on the X11 background (-XB set).  */
  _bool fText;      /* Are we printing chart info on chart (-XT set).   */
  _bool fFont;      /* Are we simulating fonts in charts (-XM0 set).    */
  _bool fAlt;       /* Are we drawing in alternate mode (-Xi set).      */
  _bool fBorder;    /* Are we drawing borders around charts (-Xu set).  */
  _bool fLabel;     /* Are we labeling objects in charts (-Xl not set). */
  _bool fJetTrail;  /* Are we not clearing screen on updates (-Xj set). */
  _bool fMouse;     /* Are we not considering PC mouse inputs.          */
  _bool fConstel;   /* Are we drawing maps as constellations (-XF set). */
  _bool fMollewide; /* Are we drawing maps scaled correctly (-XW0 set). */
  _bool fPrintMap;  /* Are we printing globe names on draw (-XP0 set).  */
  int xWin;         /* Current size of graphic chart (-Xw).      */
  int yWin;
  int nAnim;        /* Current animation mode, if any (-Xn).     */
  int nScale;       /* Current character scale factor (-Xs).     */
  int objLeft;      /* Current object to place on Asc (-X1).     */
  int nTextRows;    /* Numb. of rows to set text screen to (-V). */
  int nRot;         /* Current rotation degree of globe.         */
  real rTilt;       /* Current vertical tilt of rotating globe.  */
  char chBmpMode;   /* Current bitmap file type (-Xb).           */
  int nOrient;      /* PostScript paper orientation indicator.   */
  real xInch;       /* PostScript horizontal paper size inches.  */
  real yInch;       /* PostScript vertical paper size inches.    */
  char *szDisplay;  /* Current X11 display name (-Xd).           */
  int nGridCell;    /* Number of cells in -g grids (-Yg).        */
  int nGlyphs;      /* Settings for what gylphs to use (-YG).    */
#ifdef PCG
  int nResHi;       /* 'High-resolution' graphics mode. */
  int nResLo;       /* 'Flicker-free' graphics mode.    */
#endif
} GS;

typedef struct _GraphicsInternal {
  int nMode;            /* Current type of chart to create.           */
  _bool fMono;          /* Is this a monochrome monitor.              */
  int kiCur;            /* Current color drawing with.                */
  hpbyte bm;            /* Pointer to allocated memory.               */
  int cbBmpRow;         /* Horizontal size of bitmap array in memory. */
  char *szFileOut;      /* Current name of bitmap file (-Xo).         */
  FILE *file;           /* Actual file handle writing graphics to.    */
  int yBand;            /* Vertical offset to current bitmap band.    */
  real rAsc;            /* Degree to be at left edge in wheel charts. */
  _bool fFile;          /* Are we making a graphics file.             */
  int nScale;           /* Scale ratio, i.e. percentage / 100.        */
  int nScaleT;          /* Relative scale to draw chart text at.      */
  int nPenWid;          /* Pen width to use when creating metafiles.  */
  KI kiOn;              /* Foreground color. */
  KI kiOff;             /* Background color. */
  KI kiLite;            /* Hilight color.    */
  KI kiGray;            /* A "dim" color.    */
  int xOffset;          /* Viewport origin.                           */
  int yOffset;
  int xTurtle;          /* Current coordinates of drawing pen.        */
  int yTurtle;
  int xPen;             /* Cached coordinates where last line ended.  */
  int yPen;
#ifdef X11
  Display *disp;        /* The opened X11 display (-Xd). */
  GC gc, pmgc;
  Pixmap pmap, icon;
  Window wind, root;
  int screen;
  int depth;            /* Number of active color bits.  */
#endif
#ifdef PS               /* Variables used by the PostScript generator. */
  _bool fEps;           /* Are we doing Encapsulated PostScript.    */
  int cStroke;          /* Number of items drawn without fluahing.  */
  _bool fLineCap;       /* Are line ends rounded instead of square. */
  int nDash;            /* How much long are dashes in lines drawn. */
  int nFont;            /* What system font are we drawing text in. */
  real rLineWid;        /* How wide are lines, et al, drawn with.   */
#endif
#ifdef META             /* Variables used by the metafile generator.  */
  word HPTR *pwMetaCur; /* Current mem position when making metafile. */
  word HPTR *pwPoly;    /* Position for start of current polyline.    */
  long cbMeta;
  KI kiLineAct;         /* Desired and actual line color. */
  KI kiLineDes;
  KI kiFillAct;         /* Desired and actual fill color. */
  KI kiFillDes;
  int nFontAct;         /* Desired and actual text font.  */
  int nFontDes;
  KI kiTextAct;         /* Desired and actual text color. */
  KI kiTextDes;
  int nAlignAct;        /* Desired/actual text alignment. */
  int nAlignDes;
#endif
#ifdef MSG              /* MS graphics PC specific global variables.  */
  int nRes;             /* Current graphics mode.                     */
  struct videoconfig cfg;          /* State of current graphics mode. */
#endif
#ifdef BGI              /* Borland BGI PC specific global variables.  */
  int nRes;             /* Current graphics mode.              */
  _bool fLoaded;        /* Have we loaded the graphics driver. */
  int nDriver;          /* Current graphics driver.            */
  int nGraph;           /* Current mode within driver.         */
  int nPages;           /* Number of pages in current mode.    */
  int nPageCur;         /* The current page bring drawn in.    */
#endif
#ifdef MACG
  WindowPtr wpAst;
  Rect rcDrag;
  Rect rcBounds;
#endif
} GI;
#endif /* GRAPH */

typedef struct _ArabicInfo {
  char *form;                 /* The formula to calculate it. */
  char *name;                 /* The name of the Arabic part. */
} AI;

typedef struct _ElementTable {
  int coSum;            /* Total objects considered.          */
  int coHemi;           /* Number that can be in hemispheres. */
  int coSign[cSign];    /* Number of objects in each sign.    */
  int coHouse[cSign];   /* Number of objects in each house.   */
  int coElemMode[4][3]; /* Objects in each elem/mode combo.   */
  int coElem[4];        /* Object in each element.            */
  int coMode[3];        /* Objects in each sign mode.         */
  int coModeH[3];       /* Objects in each house mode.        */
  int coYang;           /* Objects in Fire/Air signs.         */
  int coYin;            /* Objects in Earth/Water signs.      */
  int coLearn;          /* Objects in first six signs.        */
  int coShare;          /* Objects in last six signs.         */
  int coAsc;            /* Objects in Eastern houses.         */
  int coDes;            /* Objects in Western houses.         */
  int coMC;             /* Objects in Southern houses.        */
  int coIC;             /* Objects in Northern houses.        */
} ET;

typedef struct _OrbitalElements {
  real ma0, ma1, ma2; /* Mean anomaly.           */
  real ec0, ec1, ec2; /* Eccentricity.           */
  real sma;           /* Semi-major axis.        */
  real ap0, ap1, ap2; /* Argument of perihelion. */
  real an0, an1, an2; /* Ascending node.         */
  real in0, in1, in2; /* Inclination.            */
} OE;

#ifdef WIN
#define nScrollDiv 24
#define nScrollPage 6
#define cchSzMaxFile 128
#define szFileTempCore "ASTROLOG.TMP"

typedef struct _WindowInternal {
  HWND hinst;     /* Instance of the Astrolog window class.    */
  HWND hwndMain;  /* The outer created frame window.           */
  HWND hwnd;      /* The current window being dealt with.      */
  HMENU hmenu;    /* The Astrolog main menu bar.               */
  HACCEL haccel;  /* Keyboard accelerator or shortcut table.   */
  HDC hdc;        /* The current DC bring drawn upon.          */
  HDC hdcPrint;   /* The current DC being printed upon.        */
  HWND hwndAbort; /* Window of the printing abort dialog.      */
  HPEN hpen;      /* Pen with the current line color.          */
  HBRUSH hbrush;  /* Fill if any with the current color.       */
  HFONT hfont;    /* Font of current text size being printed.  */
  UINT nTimer;    /* Identifier for the animation timer.       */
  short xScroll;  /* Horizontal & vertical scrollbar position. */
  short yScroll;
  short xClient;  /* Horizontal & vertical window size. */
  short yClient;
  int xChar;      /* Horizontal & vertical font character size. */
  int yChar;
  int xMouse;     /* Horitontal & vertical mouse position. */
  int yMouse;
  WORD wCmd;      /* The currently invoked menu command.        */
  int nMode;      /* New chart type to switch to if any.        */
  bool fMenu;     /* Do we need to repaint the menu bar?        */
  bool fMenuAll;  /* Do we need to redetermine all menu checks? */
  bool fRedraw;   /* Do we need to redraw the screen?           */
  bool fCast;     /* Do we need to recast the chart positions?  */
  bool fAbort;    /* Did the user cancel printing in progress?  */
  int nDlgChart;  /* Which chart to set in Open or Info dialog. */

  /* Window User settings. */
  bool fPause;       /* Is animation paused?                   */
  bool fBuffer;      /* Are we drawing updates off screen?     */
  bool fHourglass;   /* Bring up hourglass cursor on redraws?  */
  bool fChartWindow; /* Does chart change cause window resize? */
  bool fWindowChart; /* Does window resize cause chart change? */
  bool fNoUpdate;    /* Do we not automatically update screen? */
  KI kiPen;          /* The current pen scribble color.        */
  int nDir;          /* Animation step factor and direction.   */
  UINT nTimerDelay;  /* Milliseconds between animation draws.  */
} WI;
#endif

#include "extern.h"

/* astrolog.h */
