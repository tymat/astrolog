/*
** Astrolog (Version 5.41G) File: data.c
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

#include "astrolog.h"


/*
******************************************************************************
** Global Variables.
******************************************************************************
*/

#ifdef __TURBOC__
extern unsigned _stklen = 0x4000;
#endif

US NPTR us = {

  /* Chart types */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  /* Chart suboptions */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  /* Table chart types */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  /* Main flags */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,

  /* Main subflags */
#ifdef SWITCHES
  fFalse,
#else
  fTrue,
#endif
  0, 0, 0, 0, 0, 0, 0,

  /* Rare flags */
#ifdef TRUENODE
  fTrue,
#else
  fFalse,
#endif
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  /* Value settings */
  0,
  0,
  0,
  DEFAULT_SYSTEM,
  DEFAULT_ASPECTS,
  oEar,
  0,
  1,
  0,
  0,
  0,
  DIVISIONS,
  SCREENWIDTH,
  0.0,
  DEFAULT_ZONE,
  DEFAULT_LONG,
  DEFAULT_LAT,

  /* Value subsettings */

  4, 5, cPart, 0.0, 365.2421988, 1, 1, 24, 0L, 0, BIODAYS};

IS NPTR is = {
  fFalse, fFalse, fFalse, fFalse, fFalse, fFalse, fFalse, fFalse,
  NULL, NULL, NULL, NULL, 0, 0, 0, 0.0, 0.0, 0.0,
  NULL, 0.0, 0.0, 0.0, 0.0, 0.0};

CI ciCore = {11, 19, 1971, 11.01, 0.0, 8.0, 122.20, 47.36, "", ""};
CI ciMain = {-1, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, "", ""};
CI ciTwin = {9, 11, 1991, 0.01, 0.0, 0.0, 122.20, 47.36, "", ""};
CI ciThre = {-1, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, "", ""};
CI ciFour = {-1, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, "", ""};
CI ciTran = {12, 31, 1999, 23.59, 0.0, 0.0, 0.0, 0.0, "", ""};
CI ciSave = {12, 21, 1998, 17.57, 0.0, 8.0, 122.20, 47.36, "", ""};
CP cp0, cp1, cp2;
#ifdef WIN
CP cp3;
int fCP3 = 0;
#endif


/*
******************************************************************************
** Global Arrays.
******************************************************************************
*/

real spacex[oNorm+1], spacey[oNorm+1], spacez[oNorm+1], force[objMax];
GridInfo FPTR *grid = NULL;
int starname[cStar+1], kObjA[objMax];
byte *szMacro[48];

int kObjA1[oNorm+1] = {0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   /* Planets  */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   /* Minors   */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,             /* Cusps    */
  0, 0, 0, 0, 0, 0, 0, 0, 0};                     /* Uranians */

/* Restriction status of each object, as specified with -R switch. */

byte ignore[objMax] = {0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   /* Planets  */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   /* Minors   */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,             /* Cusps    */
  0, 0, 0, 0, 0, 0, 0, 0, 0,                      /* Uranians */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* Stars    */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0};

/* Restriction of objects when transiting, as specified with -RT switch. */

byte ignore2[objMax] = {0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0,                   /* Planets  */
  0, 0, 0, 0, 0, 0, 1, 1, 1, 1,                   /* Minors   */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,             /* Cusps    */
  0, 0, 0, 0, 0, 0, 0, 0, 0,                      /* Uranians */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* Stars    */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0};

/* Restriction of objects when progressing, as specified with -RP switch. */
/* For dual comparison charts only, else normal ignore used.  V.A.        */

byte ignore3[objMax] = {0,
  0, 0, 0, 0, 0, 0, 0, 1, 1, 1,                   /* Planets  */
  1, 1, 1, 1, 1, 0, 1, 0, 1, 1,                   /* Minors   */
  0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1,             /* Cusps    */
  1, 1, 1, 1, 1, 1, 1, 1, 1,                      /* Uranians */
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  /* Stars    */
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1};

byte ignorez[4] = {0, 0, 0, 0};    /* Restrictions for -Zd chart events. */

/* Gauquelin sector plus zones, as specified with -Yl switch. */

byte pluszone[cSector+1] = {0,
  1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1};


/*
******************************************************************************
** Global Tables.
******************************************************************************
*/

CONST byte *szAppName = szAppNameCore;

CONST byte *szSignName[cSign+1] = {"",
  "Aries", "Taurus", "Gemini", "Cancer", "Leo",
  "Virgo", "Libra", "Scorpio",
  "Sagittarius", "Capricorn", "Aquarius", "Pisces"};

CONST byte *szSignAbbrev[cSign+1] = {"",
  "Ar", "Ta", "Ge", "Cn", "Le", "Vi", "Li", "Sc", "Sg", "Cp", "Aq", "Pi"};

CONST byte * ARR szSignEnglish[cSign+1] = {"",
  "Ram", "Bull", "Twins", "Crab", "Lion", "Virgin",
  "Scales", "Scorpion", "Archer", "Sea Goat", "Water Bearer", "Fishes"};

CONST byte * ARR szHouseTradition[cSign+1] = {"",
  "Personality", "Money", "Communication", "Home",
  "Children", "Servants", "Marriage", "Death",
  "Long Journeys Over Water", "Career", "Friends", "Troubles"};

byte * ARR szObjName[objMax] = {
  "Earth", "Sun", "Moon", "Mercury", "Venus", "Mars",       /* Planets   */
  "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto",
  "Chiron", "Ceres", "Pallas", "Juno", "Vesta",             /* Asteroids */
  "Node", "Lilith", "Fortune", "Vertex", "East Point",      /* Others    */
  "Ascendant", "2nd Cusp", "3rd Cusp", "IC",                /* Cusps     */
  "5th Cusp", "6th Cusp", "Descendant", "8th Cusp",
  "9th Cusp", "Midheaven", "11th Cusp", "12th Cusp",
  "Cupido", "Hades", "Zeus", "Kronos",                      /* Uranians  */
  "Apollon", "Admetos", "Vulkanus", "Poseidon", "Proserpina",

  /*  Stars  */

  "Achernar\0    ", "Polaris\0     ", "Zeta Retic.\0 ", "Alcyone\0     ",
  "Aldebaran\0   ", "Capella\0     ", "Rigel\0       ", "Bellatrix\0   ",
  "Elnath\0      ", "Alnilam\0     ", "Betelgeuse\0  ", "Menkalinan\0  ",
  "Mirzam\0      ", "Canopus\0     ", "Alhena\0      ", "Sirius\0      ",
  "Adhara\0      ", "Wezen\0       ", "Castor\0      ", "Procyon\0     ",
  "Pollux\0      ", "Suhail\0      ", "Avior\0       ", "Miaplacidus\0 ",
  "Alphard\0     ", "Regulus\0     ", "Dubhe\0       ", "Acrux\0       ",
  "Gacrux\0      ", "Mimosa\0      ", "Alioth\0      ", "Spica\0       ",
  "Alkaid\0      ", "Agena\0       ", "Arcturus\0    ", "Toliman\0     ",
  "Antares\0     ", "Shaula\0      ", "Sargas\0      ", "Kaus Austr.\0 ",
  "Vega\0        ", "Altair\0      ", "Peacock\0     ", "Deneb\0       ",
  "Alnair\0      ", "Fomalhaut\0   ", "Andromeda\0   ", "Alpheratz\0   ",
  "Algenib\0     ", "Schedar\0     ", "Mirach\0      ", "Alrischa\0    ",
  "Almac\0       ", "Algol\0       ", "Mintaka\0     ", "Wasat\0       ",
  "Acubens\0     ", "Merak\0       ", "Vindemiatrix\0", "Mizar\0       ",
  "Kochab\0      ", "Zuben Elgen.\0", "Zuben Escha.\0", "Alphecca\0    ",
  "Unuk Alhai\0  ", "Ras Alhague\0 ", "Albireo\0     ", "Alderamin\0   ",
  "Nashira\0     ", "Skat\0        ", "Scheat\0      ", "Markab\0      ", 
  "Gal.Center\0  ", "Apex\0        "};

byte * ARR szStarConName[cStar+1] = {"",
  "alEri\0   ", "alUMi\0   ", "zeRet\0   ", "etTau\0   ", "alTau\0   ", "alAur\0   ",
  "beOri\0   ", "gaOri\0   ", "beTau\0   ", "epOri\0   ", "alOri\0   ", "beAur\0   ",
  "beCMa\0   ", "alCar\0   ", "gaGem\0   ", "alCMa\0   ", "epCMa\0   ", "deCMa\0   ",
  "alGem\0   ", "alCMi\0   ", "beGem\0   ", "ga-2Vel\0 ", "epCar\0   ", "beCar\0   ",
  "alHya\0   ", "alLeo\0   ", "alUMa\0   ", "al-1Cru\0 ", "gaCru\0   ", "beCru\0   ",
  "epUMa\0   ", "alVir\0   ", "etUMa\0   ", "beCen\0   ", "alBoo\0   ", "alCen\0   ",
  "alSco\0   ", "laSco\0   ", "thSco\0   ", "epSgr\0   ", "alLyr\0   ", "alAql\0   ",
  "alPav\0   ", "alCyg\0   ", "alGru\0   ", "alPsA\0   ", "M31\0     ", "alAnd\0   ",
  "gaPeg\0   ", "alCas\0   ", "beAnd\0   ", "alPis\0   ", "ga-1And\0 ", "bePer\0   ",
  "deOri\0   ", "deGem\0   ", "alCnc\0   ", "beUMa\0   ", "epVir\0   ", "zeUMa\0   ",
  "beUMi\0   ", "al-2Lib\0 ", "beLib\0   ", "alCrB\0   ", "alSer\0   ", "alOph\0   ",
  "be-1Cyg\0 ", "alCep\0   ", "gaCap\0   ", "deAqr\0   ", "bePeg\0   ", "alPeg     ",
  "SgrA*\0   ", "HerA*\0   "};
    

CONST byte * ARR szSystem[cSystem] = {
  "Placidus", "Koch", "Equal", "Campanus", "Meridian",
  "Regiomontanus", "Porphyry", "Morinus", "Topocentric", "Alcabitius",
  "Equal (MC)", "Neo-Porphyry", "Whole", "Vedic", "Null"};

byte * ARR szAspectName[cAspect+1] = {"",
  "Conjunct", "Opposite", "Square", "Trine", "Sextile",
  "Inconjunct", "Semisextile", "Semisquare", "Sesquiquadrate",
  "Quintile", "Biquintile",
  "Semiquintile", "Septile", "Novile",
  "Binovile", "Biseptile", "Triseptile", "Quatronovile"};

byte * ARR szAspectAbbrev[cAspect+1] = {"",
  "Con", "Opp", "Squ", "Tri", "Sex",
  "Inc", "SSx", "SSq", "Ses", "Qui", "BQn",
  "SQn", "Sep", "Nov", "BNv", "BSp", "TSp", "QNv"};

byte * szAspectAbbrevCon = "Con";
byte * szAspectAbbrevOpp = "Opp";

CONST byte * ARR szAspectGlyph[cAspect+1] = {"",
  "Circle with extending line", "Two circles joined by line",
  "Quadrilateral", "Triangle", "Six pointed asterisk",
  "'K' rotated right", "'K' rotated left", "Acute angle",
  "Square with extending lines", "Letter 'Q'", "'+' over '-'",
  "'-' over '+'", "Number '7'", "Number '9'", "'9' under Roman 'II'",
  "'7' under Roman 'II'", "'7' under Roman 'III'", "'9' under Roman 'IV'"};

CONST byte *szAspectConfig[cAspConfig+1] = {"",
  "Stellium", "Grand Trine", "T-Square", "Yod", "Grand Cross", 
  "Cradle", "Mystic Rect", "Kite"};

CONST byte *szElem[4] = {"Fire", "Earth", "Air", "Water"};

CONST byte *szMode[3] = {"Cardinal", "Fixed", "Mutuable"};

CONST byte *szMonth[cSign+1] = {"",
  "January", "February", "March", "April", "May", "June",
  "July", "August", "September", "October", "November", "December"};

CONST byte *szDay[cWeek] = {"Sunday", "Monday", "Tuesday", "Wednesday",
  "Thursday", "Friday", "Saturday"};

CONST byte * ARR szZon[cZone] = {
  "HST", "HT", "H", "CAT", "AHS", "HDT", "AHD", "YST", "YT", "Y", "YDT",
  "PST", "PT", "P", "PDT", "PWT", "MST", "MT", "M", "MDT", "MWT",
  "CST", "CT", "C", "CDT", "CWT", "EST", "ET", "E", "EDT", "EWT",
  "AST", "AT", "A", "ADT", "AWT", "BST", "BT", "B", "BDT", "WAT",
  "GMT", "GT", "G", "WET", "CET", "EET", "UZ3", "UZ4",
  "IST", "IT", "I", "UZ5", "NST", "SST", "CCT", "JST", "JT", "J",
  "SAS", "GST", "UZ1", "NZT", "ZT", "Z", "IDL", "LMT", "LT", "L"};

CONST real ARR rZon[cZone] = {
  10.30, 10.30, 10.30, 10.0, 10.0, 9.30, 9.0, 9.0, 9.0, 9.0, 8.0,
  8.0, 8.0, 8.0, 7.0, 7.0, 7.0, 7.0, 7.0, 6.0, 6.0,
  6.0, 6.0, 6.0, 5.0, 5.0, 5.0, 5.0, 5.0, 4.0, 4.0,
  4.0, 4.0, 4.0, 3.0, 3.0, 3.0, 3.0, 3.0, 2.0, 1.0,
  0.0, 0.0, 0.0, 0.0, -1.0, -2.0, -4.0, -5.0,
  -5.30, -5.30, -5.30, -6.0, -6.30, -7.0, -8.0, -9.0, -9.0, -9.0,
  -9.30, -10.0, -11.0, -11.30, -11.30, -11.30, -12.0, 24.0, 24.0, 24.0};

CONST byte *szDir[4] = {"North", "East", "South", "West"};

CONST byte *szSuffix[cSign+1] = {"",
  "st", "nd", "rd", "th", "th", "th", "th", "th", "th", "th", "th", "th"};

real rAspAngle[cAspect+1] = {0,
  0.0, 180.0, 90.0, 120.0, 60.0, 150.0, 30.0, 45.0, 135.0, 72.0, 144.0,
  36.0, rDegMax/7.0, 40.0, 80.0, 720.0/7.0, 1080.0/7.0, 160.0};

real rAspOrb[cAspect+1] = {0,
  7.0, 7.0, 7.0, 7.0, 6.0, 3.0, 3.0, 3.0, 3.0, 2.0, 2.0,
  1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

real rObjOrb[oNorm+1] = {360.0,
  360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
  360.0, 360.0, 360.0, 360.0, 360.0, 3.0, 3.0, 360.0, 360.0, 3.0,
  360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
  360.0, 360.0, 360.0, 360.0, 360.0, 360.0,
  360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0, 360.0};

real rObjAdd[oNorm+1] = {0.0,
  1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

int ruler1[oNorm+1] = {0,
   5,  4,  3,  2,  1,  9, 10, 11, 12,  8,
  12,  2,  6,  7,  8, 11,  8, 12,  7,  1,
   1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
   7,  8,  5, 10,  9,  6,  1,  6,  9};
int ruler2[oNorm+1] = {0,
   0,  0,  6,  7,  8, 12, 11,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0};
int exalt[oNorm+1] = {0,
   1,  2, 11, 12, 10,  4,  7,  8,  9,  6,
   4,  4, 10,  5, 11,  6, 12,  9, 12, 10,
   5,  6,  7,  8,  9, 10, 11, 12,  1,  2,  3,  4,
   3,  6,  1,  9, 11,  8,  5,  3, 12};

  /* This array is the reverse of the ruler arrays:   */
  /* Here, given a sign, return what planet rules it. */
int rules[cSign+1] = {0,
  5, 4, 3, 2, 1, 3, 4, 10, 6, 7, 8, 9};
CONST byte * ARR szColor[cColor] = {"Black",
  "Maroon", "DkGreen", "Orange", "DkBlue", "Purple", "DkCyan", "LtGray",
  "Gray", "Red", "Green", "Yellow", "Blue", "Magenta", "Cyan", "White"};
int kMainA[9] = {kBlack, kWhite, kLtGray, kDkGray,
  kMaroon, kDkGreen, kDkCyan, kDkBlue, kMagenta};
int kRainbowA[8] = {kWhite,
  kRed, kOrange, kYellow, kGreen, kCyan, kBlue, kPurple};
int kElemA[4] = {kRed, kYellow, kGreen, kBlue};
int kAspA[cAspect+1] = {kWhite,
  kYellow, kBlue, kRed, kGreen, kCyan,
  kMagenta, kMagenta, kOrange, kOrange, kDkCyan, kDkCyan,
  kDkCyan, kMaroon, kPurple, kPurple, kMaroon, kMaroon, kPurple};

int kStar1 = kRed;
int kStar2 = kOrange;
int kStar3 = kMaroon;

#ifdef INTERPRETALT
byte NoPrint = 0;
byte InterpretAlt = 0;
#endif
byte NoPrintHeader = 0;

/* Influence information used by ChartInfluence() follows. The influence of */
/* a planet in its ruling or exalting sign or house is tacked onto the last */
/* two positions of the object and house influence array, respectively.     */


  /* The inherent strength of each planet - */
real rObjInf[oNorm+3] = {20,
  30, 25, 10, 10, 10, 10, 10, 10, 10, 10,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  20, 10, 10, 10, 10, 10, 10, 10, 10, 15, 10, 10,
  3, 3, 3, 3, 3, 3, 3, 3, 3,
  20, 10};

real ppower1[oNorm+1] = {0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0 ,0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0};

real ppower2[oNorm+1] = {0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0 ,0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0};

real ppowerTotal[oNorm+1] = {0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0 ,0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0};

  /* The inherent strength of each house - */
real rHouseInf[cSign+3]  = {0,
  20, 0, 0, 10, 0, 0, 5, 0, 0, 15, 0, 0,
  15, 5};

  /* The inherent strength of each aspect - */
real rAspInf[cAspect+1] = {0.0,
  1.0, 0.8, 0.8, 0.6, 0.6, 0.4, 0.4, 0.2, 0.2,
  0.2, 0.2, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};

  /* The inherent strength of each planet when transiting - */
real rTransitInf[oNorm+3] = {10,
  10, 4, 8, 9, 20, 30, 35, 40, 45, 50,
  30, 15, 15, 15, 15, 30,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  50, 50, 50, 50, 50, 50, 50, 50, 50};

CONST real rObjDist[oVes+1] = {149.59787, 0.0, 0.3844,
  57.91, 108.2, 227.94, 778.33, 1426.98, 2870.99, 4497.07, 5913.52,
  13.670*149.59787, 2.767*149.59787, 2.770*149.59787, 2.669*149.59787,
  2.361*149.59787};
CONST real rObjYear[oVes+1] = {1, 0, 27.32166/365.25,
  87.969/365.25, 224.701/365.25, 686.98/365.25, 11.8623,
  29.458, 84.01, 164.79, 248.54,
  51.0, 4.60, 4.61, 4.36, 3.63};
CONST real rObjDiam[oVes+1] = {12756.0, 1392000.0, 3476.0,
  4878.0, 12102.0, 6786.0, 142984.0, 120536.0, 51118.0, 49528.0, 2300.0,
  320.0, 955.0, 538.0, 226.0, 503.0};
CONST real rObjDay[oPlu+1] = {24.0, 30.0*24.0, 27.322*24.0,
  58.65*24.0, 243.01*24.0, 24.6229, 9.841, 10.233, 17.9, 19.2, 6.3872*24.0};
CONST real rObjMass[oPlu+1] = {1.0, 322946.0, 0.0123,
  0.0553, 0.8149, 0.1074, 317.938, 95.181, 14.531, 17.135, 0.0022};
CONST real rObjAxis[oPlu+1] = {23.5, 0.0, 6.7,
  2.0, 2.7, 25.19, 3.12, 26.73, 82.14, 29.6, 57.54};
CONST byte cSatellite[oPlu+1] = {1, 9, 0,
  0, 0, 2, 16, 18, 15, 8, 1};

#ifdef ARABIC
CONST AI ai[cPart] = {
  {"    02 01F ", "Fortune"},
  {"    01 02F ", "Spirit"},
  {"    06 S F ", "Victory"},
  {"    F  05F ", "Valor & Bravery"},
  {"    05 03Fh", "Mind & Administrators"},
  {"   h02r02Fh", "Property & Goods"},
  {"    06 07 h", "Siblings"},
  {"   j03 01Fh", "Death of Siblings"},
  {"    06 07Fh", "Death of Parents"},
  {"    07h02Fh", "Grandparents"},
  {"    06 03Fh", "Real Estate"},
  {"    07 06Fh", "Children & Life"},
  {"   R02 02 h", "Expected Birth"},
  {"    05 03 h", "Disease & Defects (1)"},
  {"    05 07 h", "Disease & Defects (2)"},
  {"   R07 07 h", "Captivity"},
  {"    02 03 h", "Servants"},
  {"   h07 04 h", "Partners"},
  {"   h08 02 h", "Death"},
  {"    05 07Fh", "Sickness & Murder"},
  {"    03 07Fh", "Danger, Violence & Debt"},
  {"   h09r09 h", "Journeys"},
  {"   105 07Fh", "Travel by Water"},
  {"    03 02Fh", "Faith, Trust & Belief"},
  {"    02 07Fh", "Deep Reflection"},
  {"    01 07Fh", "Understanding & Wisdom"},
  {"    06 01Fh", "Fame & Recognition"},
  {"    02 05Fh", "Rulers & Disassociation"},
  {"    07 01Fh", "Father, Fate & Karma"}, /* Combust */
  {"    F  07Fh", "Sudden Advancement"},
  {"    01 07 h", "Celebrity of Rank"},
  {"    07 05Fh", "Surgery & Accident"},
  {"    04 03Fh", "Merchants & Their Work"},
  {"    F  S Fh", "Merchandise (Exchange)"}, /* Moon */
  {"    02 04Fh", "Mother"},
  {"    S  F Fh", "Glory & Constancy"},
  {"    01 F Fh", "Honorable Acquaintances"},
  {"    06 F Fh", "Success"},
  {"    04 F Fh", "Worldliness"},
  {"    03 02 h", "Acquaintances"},
  {"    03 S  h", "Violence"},
  {"    01 03Fh", "Liberty of Person"},
  {"    04 06Fh", "Praise & Acceptance"},
  {"   h12r12 h", "Enmity"},
  {"    F  S  h", "Bad Luck"},
  {"    05 F F ", "Debilitated Bodies"},
  {"    02D  F ", "Boldness & Violence"},
  {"    S  03F ", "Trickery & Deceit"},
  {"   h03 05  ", "Necessities"},
  {"    03 F   ", "Realization of Needs"},
  {"    01 05F ", "Retribution"},
  {"    06 02  ", "Children (Male)"},
  {"    04 02  ", "Children (Female)"},
  {"    05 04  ", "Play & Variety"}, /* Change */
  {"    07 03 h", "Stability"},
  {"   h05 06Fh", "Speculation"},
  {"    03 04Fh", "Art"},
  {"   h05r05 h", "Sexual Attraction"},
  {"    10 04 h", "Sex Drive & Stimulation"},
  {"    05 01 h", "Passion"},
  {"    05 04 h", "Emotion & Affection"},
  {"   r08 07 h", "Most Perilous Year"},
  {"   h08 07 h", "Peril"},
  {"    09 08 h", "Occultism"},
  {"    03 01 h", "Commerce"},
  {"h09h03 04 h", "Marriage Contracts"},
  {"   h09r09 h", "Travel by Land"},
  {"    08H08 h", "Travel by Air"},
  {" 30 01 02Fh", "Destiny"},
  {" 30 02 01Fh", "Vocation & Status"},
  {"   019 01 h", "Honor, Nobility (Day)"},
  {"   033 02 h", "Honor, Nobility (Night)"},
  {"    10 01 h", "Organization"},
  {"    04h07 h", "Divorce"},
  {"    08 01 h", "Ostracism & Loss"},
  {"    02 08Fh", "Friends"},
  {"    07 01 h", "Tragedy & Brethren"},
  {"    02 10Fh", "Race (Consciousness)"},
  {"    02D02Fh", "Bondage & Slavery"},
  {"    F  09 h", "Imprisonment & Sorrow"},
  {"    04 08 h", "Perversion"},
  {"   h12 09 h", "Self-Undoing"},
  {"    09 01 h", "Treachery & Entrapment"},
  {"h12r12 09 h", "Bereavement"},
  {"    06h12 h", "Suicide (Yang)"},
  {"   h08 09 h", "Suicide (Yin)"},
  {"    06 09 h", "Depression"},
  {" 05 09 08 h", "Assassination (Yang)"},
  {"   r12 09 h", "Assassination (Yin)"},
  {"    09 06  ", "Cancer (Disease)"},
  {"    08 07  ", "Catastrophe"},
  {"    07 08  ", "Foolhardiness"},
  {"    03 05  ", "Release & Luck"},
  {"    06 03  ", "Benevolence & Assurance"},
  {"    03 06  ", "Hope & Sensitivity"},
  {"    03 07  ", "Aptness & Aloofness"},
  {"    08 09  ", "Charm & Personality"},
  {"    02 03F ", "Faith & Good Manners"},
  {"    01 03  ", "Temperament"},
  {"    04 03  ", "Security & Treasure"},
  {"    08 03  ", "Originality"},
  {"    03 08  ", "Eccentricity, Astrology"},
  {"    09 03  ", "Divination"},
  {"    03 09  ", "Intrusion"},
  {"    05 06  ", "Negotiation"},
  {"    06 05  ", "Discord & Controversy"},
  {"    05 08  ", "Coincidence"},
  {"    08 05  ", "Unpreparedness"},
  {"    05 09  ", "Popularity"},
  {"    09 05  ", "Misunderstanding"},
  {"    04 06  ", "Sentiment & Marriage"},
  {"    06 04  ", "Loneliness"},
  {"    04 07  ", "Success in Investment"},
  {"    07 04  ", "Frugality & Labor"},
  {"    08 04  ", "Wastefulness"},
  {"    04 09  ", "Vanity"},
  {"    09 04  ", "Corruptness"},
  {"    01 05  ", "Initiative"},
  {"    05 02F ", "Memory"},
  {"    04 01  ", "Love, Beauty & Peace"},
  {"    01 04  ", "Disinterest & Boredom"},
  {"    01 06  ", "Accomplishment"},
  {"    07 02F ", "Influence"},
  {"    06 01  ", "Increase & Impression"},
  {"    09 07  ", "Caution"},
  {"    07 09  ", "Timidity"},
  {"    08 06  ", "Entertainment"},
  {"    06 08  ", "Bequest"},
  {"    01 09  ", "Genius"},
  {"    02 09F ", "Revelation"},
  {"    09 02F ", "Delusion"},
  {"    08 02F ", "Misinterpretation"},
  {"    01 08  ", "Intellectuality"},
  {"    06 07 E", "Earth"},
  {"    04 02 E", "Water"},
  {"   r04 03 E", "Air & Wind"},
  {"    05 01 E", "Fire"},
  {"    07 05FE", "Clouds"},
  {"    04 02FE", "Rains"},
  {"    07 03FE", "Cold"},
  {"    06 01FC", "Wheat"},
  {"    06 02FC", "Barley & Meats"},
  {"    04 06FC", "Rice & Millet"},
  {"    07 06FC", "Corn"},
  {"    07 05FC", "Lentils, Iron, Pungents"},
  {"    05 07FC", "Beans & Onions"},
  {"    01 03FC", "Chick Peas"},
  {"    04 07FC", "Sesame & Grapes"},
  {"    03 04FC", "Sugar & Legumes"},
  {"    01 02FC", "Honey"},
  {"    02 05FC", "Oils"},
  {"    04 05FC", "Nuts & Flax"},
  {"    02 03FC", "Olives"},
  {"    05 07FC", "Apricots & Peaches"},
  {"    03 06FC", "Melons"},
  {"    05 02FC", "Salt"},
  {"    03 01FC", "Sweets"},
  {"    07 03FC", "Astrigents & Purgatives"},
  {"    04 03FC", "Silk & Cotton"},
  {"    05 07FC", "Purgatives (Bitter)"},
  {"    06 07FC", "Purgatives (Acid)"},
  {"    30D   H", "Secrets"},
  {"    02 03FH", "Information True/False"},
  {"    F D  FH", "Injury to Business"},
  {" 03 07 06 H", "Freedmen & Servants"},
  {" 02 07 06 H", "Employers"},
  {"   h07 04 H", "Marriage"},
  {"    06 01 H", "Time for Action/Success"},
  {"    07 01 H", "Time Occupied in Action"},
  {" 07 06 01 H", "Dismissal & Resignation"},
  {"    05 02 H", "Life/Death of Absentee"},
  {"    05 01 H", "Lost Animal (Light)"},
  {"    05 07 H", "Lost Animal (Dark)"},
  {"    03 05 H", "Lawsuit"},
  {"h08 05 02 H", "Decapitation"},
  {" 30 07 02 H", "Torture"},
  {" 02h04D   H", "Lost Objects"}
};
#endif /* ARABIC */

                          /* added by V.A. */
real StarOrb = 1.2;
real PartOrb = 1.0;
real CoeffComp = 2.5;
real CoeffPar = 6.0;
real CoeffTens = 1.0;
real PowerPar = 0.25;
real SolidAspect = 8.0;
real SConjunct = 0.0;
real SHarmon = 0.0;
real STension = 0.0;
real SSumma = 0.0;
real SChart = 0.0;
real PrimeOrb1 = 1.16666667;
real PrimeOrb2 = 1.16666667;
double deltaT;
int StarAspects = 1;
int PartAspects = 1;
int PartAspectsShow = 5;
int TensChart = 0;
int hRevers = 0;
int InfoFlag = 0;
int PrimeRest = 0;
int MonthFormat = fFalse;
byte StarRest = 0;
byte PolarMCflip = 0;
byte fEquator = 0;
byte forceSN = 0;
byte oscLilith = 0;
byte fNESW = 0;
byte fDisp = fTrue;

byte *szFileNameSearch = "                ";
byte szFilePathWin[cchSzMax] = "";

/* data.c */
