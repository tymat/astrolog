/*
** Astrolog (Version 5.41G) File: general.c
**
** Code changed by Valentin Abramov
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

real rOne = 0.999999999;
real rFractal = 0.0;

/*
******************************************************************************
** General Procedures.
******************************************************************************
*/

/* Swap two floating point values. */

void SwapR(d1, d2)
real *d1, *d2;
{
  real temp;

  temp = *d1; *d1 = *d2; *d2 = temp;
}


/* Return the length of a string (not counting the null terminator). */

int CchSz(sz)
CONST byte *sz;
{
  int i;

  for (i = 0; *sz++; i++)
    ;
  return i;
}


/* Compare two strings. Return 0 if they are equal, a positive value if  */
/* the first string is greater, and a negative if the second is greater. */

int NCompareSz(s1, s2)
CONST byte *s1, *s2;
{
  while (*s1 && *s1 == *s2)
    s1++, s2++;
  return *s1 - *s2;
}


/* Set a given number of bytes to zero given a starting pointer. */

void ClearB(pb, cb)
lpbyte pb;
int cb;
{
  while (cb-- > 0)
    *pb++ = 0;
}


/* Copy a given number of bytes from one location to another. */

void CopyRgb(pbSrc, pbDst, cb)
byte *pbSrc, *pbDst;
int cb;
{
  while (cb-- > 0)
    *pbDst++ = *pbSrc++;
}


/* Determine the sign of a number: -1 if value negative, +1 if value */
/* positive, and 0 if it's zero.                                     */

real RSgn(r)
real r;
{
  return r == 0.0 ? 0.0 : RSgn2(r);
}


/* Given an x and y coordinate, return the angle formed by a line from the */
/* origin to this coordinate. This is just converting from rectangular to  */
/* polar coordinates; however, we don't determine the radius here.         */

real Angle(x, y)
real x, y;
{
  real a;

  if (x != 0.0) {
    if (y != 0.0)
      a = RAtn(y/x);
    else
      a = x < 0.0 ? rPi : 0.0;
  } else
    a = y < 0.0 ? -rPiHalf : rPiHalf;
  if (a < 0.0)
    a += rPi;
  if (y < 0.0)
    a += rPi;
  return a;
}


/* Modulus function for floating point values, where we bring the given */
/* parameter to within the range of 0 to 360.                           */

real Mod(d)
real d;
{
  if (d >= rDegMax)        /* In most cases, our value is only slightly */
    d -= rDegMax;          /* out of range, so we can test for it and   */
  else if (d < 0.0)        /* avoid the more complicated arithmetic.    */
    d += rDegMax;
  if (d >= 0 && d < rDegMax)
    return d;
  return (d - RFloor(d/rDegMax)*rDegMax);
}


/* Another modulus function, this time for the range of 0 to 2 Pi. */

real ModRad(r)
real r;
{
  while (r >= rPi2)    /* We assume our value is only slightly out of       */
    r -= rPi2;         /* range, so test and never do any complicated math. */
  while (r < 0.0)
    r += rPi2;
  return r;
}


/* Integer division - like the "/" operator but always rounds result down. */

long Dvd(x, y)
long x, y;
{
  long z;

  if (y == 0)
    return x;
  z = x / y;
  if (((x >= 0) == (y >= 0)) || x-z*y == 0)
    return z;
  return z - 1;
}


/*
******************************************************************************
** General Astrology Procedures.
******************************************************************************
*/

/* A similar modulus function: convert an integer to value from 1..12. */

int Mod12(i)
int i;
{
  while (i > cSign)
    i -= cSign;
  while (i < 1)
    i += cSign;
  return i;
}


/* Convert an inputed fractional degrees/minutes value to a true decimal   */
/* degree quantity. For example, the user enters the decimal value "10.30" */
/* to mean 10 degrees and 30 minutes; this will return 10.5, i.e. 10       */
/* degrees and 30 minutes expressed as a floating point degree value.      */

real DecToDeg(d)
real d;
{
  return RSgn(d)*(RFloor(RAbs(d))+RFract(RAbs(d))*100.0/60.0);
}


/* This is the inverse of the above function. Given a true decimal value */
/* for a zodiac degree, adjust it so the degrees are in the integer part */
/* and the minute expressed as hundredths, e.g. 10.5 degrees -> 10.30    */

real DegToDec(d)
real d;
{
  return RSgn(d)*(RFloor(RAbs(d))+RFract(RAbs(d))*60.0/100.0);
}


/* Return the shortest distance between two degrees in the zodiac. This is  */
/* normally their difference, but we have to check if near the Aries point. */

real MinDistance(deg1, deg2)
real deg1, deg2;
{
  real i;

  i = RAbs(deg1-deg2);
  return i < rDegHalf ? i : rDegMax - i;
}


/* This is just like the above routine, except the min distance value  */
/* returned will either be positive or negative based on whether the   */
/* second value is ahead or behind the first one in a circular zodiac. */

real MinDifference(deg1, deg2)
real deg1, deg2;
{
  real i;

  i = deg2 - deg1;
  if (RAbs(i) < rDegHalf)
    return i;
  return RSgn(i)*(RAbs(i) - rDegMax);
}


/* Return the degree of the midpoint between two zodiac positions, making */
/* sure we return the true midpoint closest to the positions in question. */

real Midpoint(deg1, deg2)
real deg1, deg2;
{
  real mid;

  mid = (deg1+deg2)/2.0;
  return MinDistance(deg1, mid) < rDegQuad ? mid : Mod(mid+rDegHalf);
}


/* Given a planet and sign, determine whether: The planet rules the sign, */
/* the planet has its fall in the sign, the planet exalts in the sign, or */
/* is debilitated in the sign; and return an appropriate character.       */

byte Dignify(obj, sign)
int obj, sign;
{
  if (obj > oNorm)
    return ' ';
  if (ruler1[obj] == sign || ruler2[obj] == sign)
    return 'R';
  if (ruler1[obj] == Mod12(sign+6) || ruler2[obj] == Mod12(sign+6))
    return 'F';
  if (exalt[obj] == sign)
    return 'e';
  if (exalt[obj] == Mod12(sign+6))
    return 'd';
  return '-';
}


/* Determine the number of days in a particular month. The year is needed, */
/* too, because we have to check for leap years in the case of February.   */

int DayInMonth(month, year)
int month, year;
{
  int d;

  if (month == mSep || month == mApr || month == mJun || month == mNov)
    d = 30;
  else if (month != mFeb)
    d = 31;
  else {
    d = 28;
    if (year % 4 == 0 &&
      (year % 100 != 0 || year % 400 == 0 || year <= yeaJ2G))
      d++;
  }
  return d;
}


/* Return the actual number of days in a particular month. Normally, this  */
/* is the same as the above routine which determines the index of the last */
/* day of the month, but the values can differ when changing between       */
/* calendar systems (Julian to Gregorian) in which one can jump over days. */

int DaysInMonth(month, year)
int month, year;
{
  int d;

  d = DayInMonth(month, year);
  if (year == yeaJ2G && month == monJ2G)
    d -= (dayJ2G2 - dayJ2G1 - 1);
  return d;
}


/* Return the day of the week (Sunday is 0) of the specified given date. */

int DayOfWeek(month, day, year)
int month, day, year;
{
  int d;

  d = (int)((MdyToJulian(month, day, year) + 1) % 7);
  return d < 0 ? d+7 : d;
}


/* Given a day, and the month and year it falls in, add a number of days    */
/* to it and return the new day index. As month changes are not checked for */
/* here, this is mostly just adding the offset to the day; however we need  */
/* to check for calendar changes for when days in a month may be skipped.   */

int AddDay(month, day, year, delta)
int month, day, year, delta;
{
  int d;

  d = day + delta;
  if (year == yeaJ2G && month == monJ2G) {     /* Check for Julian to  */
    if (d > dayJ2G1 && d < dayJ2G2)            /* Gregorian crossover. */
      d += NSgn(delta)*(dayJ2G2-dayJ2G1-1);
  }
  return d;
}


/* Given an aspect and two objects making that aspect with each other,   */
/* return the maximum orb allowed for such an aspect. Normally this only */
/* depends on the aspect itself, but some objects require narrow orbs,   */
/* and some allow wider orbs, so check for these cases.                  */

real GetOrb(obj1, obj2, asp)
int obj1, obj2, asp;
{
  real orb, r;

  orb = rAspOrb[asp];
  r = obj1 > oNorm ? StarOrb : rObjOrb[obj1];
  orb = Min(orb, r);
  r = obj2 > oNorm ? StarOrb : rObjOrb[obj2];
  orb = Min(orb, r);
  if (obj1 <= oNorm && obj2 <= oNorm) {
    orb += rObjAdd[obj1];
    orb += rObjAdd[obj2];
  if (us.fParallel)
    orb = orb / CoeffPar;
  else if (us.nRel <= rcDual || us.fTransitInf || us.fTransitInf)
    orb = orb /CoeffComp;
  }
  return orb;
}


/*
******************************************************************************
** String Procedures.
******************************************************************************
*/

/* Exit the program, and do any cleanup necessary. Note that if we had     */
/* a non-fatal error, and we are in the -Q loop mode, then we won't        */
/* actually terminate the program, but drop back to the command line loop. */

void Terminate(tc)
int tc;
{
  byte sz[cchSzDef];

  if (us.fNoQuit)
    return;
  if (tc == tcForce) {
    is.S = stdout;
    AnsiColor(kWhite);
    sprintf(sz, "\n%s %s exited.\n", szAppName, szVersionCore);
    PrintSz(sz);
  }
  if (tc == tcError && us.fLoop)
    return;
  if (us.fAnsiColor) {
    sprintf(sz, "%c[0m", chEscape);    /* Get out of any Ansi color mode. */
    PrintSz(sz);
  }
  exit(abs(tc));
}


/* Print a string on the screen. A seemingly simple operation, however we */
/* keep track of what column we are printing at after each newline so we  */
/* can automatically clip at the appropriate point, and we keep track of  */
/* the row we are printing at, so we may prompt before screen scrolling.  */

void PrintSz(sz)
CONST byte *sz;
{
  byte szInput[cchSzDef], *pch;
#ifndef WIN
  int fT;
#endif

  for (pch = (byte *)sz; *pch; pch++) {
    if (*pch != '\n') {
      is.cchCol++;
      if (us.fClip80 && is.cchCol >= us.nScreenWidth)  /* Clip if need be. */
        continue;
    } else {
      is.cchRow++;
      is.cchCol = 0;
    }
#ifdef WIN
    if (is.S == stdout) {
      if ((byte)*pch >= ' ') {
        szInput[0] = *pch; szInput[1] = chNull;
        TextOut(wi.hdc, (is.cchCol - 1 - wi.xScroll * 10) * wi.xChar + 4,
          (is.cchRow - wi.yScroll * 10) * wi.yChar, szInput, 1);
      }
    } else
#endif
    putc(*pch, is.S);
#ifndef WIN
    if (*pch == '\n' && is.S == stdout &&
      us.nScrollRow > 0 && is.cchRow >= us.nScrollRow) {

      /* If we've printed 'n' rows, stop and wait for a line to be entered. */

      fT = us.fAnsiColor;
      us.fAnsiColor = fFalse;
      InputString("Press return to continue scrolling", szInput);
      us.fAnsiColor = fT;
      is.cchRow = 0;

      /* One can actually give a few simple commands before hitting return. */

      if (szInput[0] == '.' || szInput[0] == 'q')
        Terminate(tcForce);
      else if (szInput[0] == '8')
        not(us.fClip80);
      else if (szInput[0] == 'Q')
        us.nScrollRow = 0;
      else if (szInput[0] == 'k') {
        if (us.fAnsiColor)
          AnsiColor(kDefault);
        not(us.fAnsiColor); not(us.fAnsiChar);
      }
    }
#else
    if (*pch == '\n' && is.S == stdout && wi.hdcPrint != hdcNil &&
      is.cchRow >= us.nScrollRow) {

      /* If writing to the printer, start a new page when appropriate. */

      is.cchRow = 0;
      EndPage(wi.hdcPrint);
      StartPage(wi.hdcPrint);
      /* StartPage clobbers all the DC settings */
      SetMapMode(wi.hdcPrint, MM_ANISOTROPIC);      /* For SetViewPortExt */
      SetViewportOrg(wi.hdcPrint, 0, 0);
      SetViewportExt(wi.hdcPrint, GetDeviceCaps(wi.hdcPrint, HORZRES),
        GetDeviceCaps(wi.hdcPrint, VERTRES));
      SetWindowOrg(wi.hdcPrint, 0, 0);
      SetWindowExt(wi.hdcPrint, wi.xClient, wi.yClient);
      SetBkMode(wi.hdcPrint, TRANSPARENT);
      SelectObject(wi.hdcPrint, wi.hfont);
    }
#endif
  }
}


/* Print a single character on the screen. */

void PrintCh(ch)
byte ch;
{
  byte sz[2];

  sz[0] = ch; sz[1] = chNull;    /* Treat char as a string of length one. */
  PrintSz(sz);                   /* Then call above to print the string.  */
}


/* Print a string on the screen. Unlike the normal PrintSz(), here we still */
/* go to the standard output even if text is being sent to a file with -os. */

void PrintSzScreen(sz)
byte *sz;
{
  FILE *fileT;

  fileT = is.S;
  is.S = stdout;
  PrintSz(sz);
  is.S = fileT;
}


/* Print a general user message given a string. This is just like the */
/* warning displayer below just that we print in a different color.   */

void PrintNotice(sz)
byte *sz;
{
#ifndef WIN
  /* Notice messages are ignored in the Windows version. */
  AnsiColor(kYellow);
  fprintf(stderr, "%s\n", sz);
  AnsiColor(kDefault);
#endif
}


/* Print a warning message given a string. This is called in non-fatal  */
/* cases where we return to normal execution after printing the string. */

void PrintWarning(sz)
byte *sz;
{
#ifndef WIN
  AnsiColor(kRed);
  fprintf(stderr, "%s\n", sz);
  AnsiColor(kDefault);
#else
  byte szT[cchSzDef];

  sprintf(szT, "%s Warning", szAppName);
  MessageBox(wi.hwndMain, sz, szT, MB_ICONSTOP);
#endif
}


/* Print an error message. This is called in more serious cases which halt */
/* running of the current chart sequence, which can terminate the program  */
/* but isn't a fatal error in that we can still fall back to the -Q loop.  */

void PrintError(sz)
byte *sz;
{
#ifndef WIN
  AnsiColor(kRed);
  fprintf(stderr, "%s: %s\n", szAppName, sz);
  Terminate(tcError);
  AnsiColor(kDefault);
#else
  byte szT[cchSzDef];

  sprintf(szT, "%s Error", szAppName);
  MessageBox(wi.hwndMain, sz, szT, MB_ICONEXCLAMATION);
#endif
}


/* Simplification for a commonly printed error message. */

void ErrorArgc(szOpt)
byte *szOpt;
{
  byte sz[cchSzDef];

  sprintf(sz, "Too few options to switch %c%s", chSwitch, szOpt);
  PrintError(sz);
}


/* Another simplification for a commonly printed error message. */

void ErrorValN(szOpt, nVal)
byte *szOpt;
int nVal;
{
  byte sz[cchSzDef];

  sprintf(sz, "Value %d passed to switch %c%s out of range.\n",
    nVal, chSwitch, szOpt);
  PrintError(sz);
}


/* Yet another place to print a type of error message. */

void ErrorArgv(szOpt)
byte *szOpt;
{
  byte sz[cchSzDef];

  sprintf(sz, "The switch %c%s is not allowed now.\n", chSwitch, szOpt);
  PrintError(sz);
}


/* Still another place to print a type of error message. */

void ErrorSwitch(szOpt)
byte *szOpt;
{
  byte sz[cchSzDef];

  sprintf(sz, "Unknown switch '%s'", szOpt);
  PrintError(sz);
}


#ifdef PLACALC
/* Print error messages dealing with ephemeris file access. */

void ErrorEphem(sz, l)
byte *sz;
long l;
{
  byte szT[cchSzDef];

  if (l < 0)
    sprintf(szT, "Ephemeris file %s not found.\n", sz);
  else
    sprintf(szT, "Ephemeris file %s is damaged at position %ld.\n", sz, l);
  is.fNoEphFile = fTrue;
  PrintWarning(szT);
}
#endif


/* A simple procedure used throughout Astrolog: Print a particular */
/* character on the screen 'n' times.                              */

void PrintTab(ch, cch)
byte ch;
int cch;
{
  int i;

  for (i = 0; i < cch; i++)
    PrintCh(ch);
}


/* Set an Ansi or MS Windows text color. */

void AnsiColor(k)
int k;
{
  byte sz[cchSzDef];
  int cchSav;

#ifdef WIN
  if (is.S == stdout) {
    if (k < 0)
      k = kLtGray;
    SetTextColor(wi.hdc,
      (COLORREF)rgbbmp[us.fAnsiColor ? k : (gs.fInverse ? kBlack : kLtGray)]);
    return;
  }
#endif

  /* Special case: If we are passed the value Reverse, and Ansi color is */
  /* not only on but set to a value > 1, then enter reverse video mode.  */

  if (!us.fAnsiColor || (k == kReverse && us.fAnsiColor < 2))
    return;
  cchSav = is.cchCol;
  is.cchCol = 0;
  sprintf(sz, "%c[", chEscape);
  PrintSz(sz);
  if (k == kDefault)
    PrintCh('0');
  else if (k == kReverse) {
    PrintCh('7');
  } else {
    sprintf(sz, "%c;%d", k > 7 ? '1' : '0', 30 + (k & 7));
    PrintSz(sz);
  }
  PrintCh('m');
  is.cchCol = cchSav;
}


/* Print a zodiac position on the screen. This basically just prints the */
/* string returned from SzZodiac() below, except we take care of color.  */

void PrintZodiac(deg)
real deg;
{
  AnsiColor(kElemA[(int)(deg / 30.0) & 3]);
  PrintSz(SzZodiac(deg));
  AnsiColor(kDefault);
}


/* Given a zodiac position, return a string containing it as it's */
/* formatted for display to the user.                             */

byte *SzZodiac(deg)
real deg;
{
  static byte szZod[11];
  int sign, d, m;
  real s;

  switch (us.nDegForm) {
  case 0:

    /* Normally, we format the position in degrees/sign/minutes format: */

    deg = Mod(deg + (is.fSeconds ? rRound/60.0/60.0 : rRound/60.0));
    sign = (int)deg / 30;
    d = (int)deg - sign*30;
    m = (int)(RFract(deg)*60.0);
    sprintf(szZod, "%2d%c%c%c%02d", d, chSig3(sign + 1), m);
    if (is.fSeconds) {
      s = RFract(deg)*60.0; s = RFract(s)*60.0;
      sprintf(&szZod[7], "'%02d\"", (int)s);
    }
    break;

  case 1:
    /* However, if -sh switch in effect, get position in hours/minutes: */

    deg = Mod(deg + (is.fSeconds ? rRound/4.0/60.0 : rRound/4.0));
    d = (int)deg / 15;
    m = (int)((deg - (real)d*15.0)*4.0);
    sprintf(szZod, "%2dh,%02dm", d, m);
    if (is.fSeconds) {
      s = RFract(deg)*4.0; s = RFract(s)*60.0;
      sprintf(&szZod[7], ",%02ds", (int)s);
    }
    break;

  default:
    /* Otherwise, if -sd in effect, format position as a simple degree: */

    sprintf(szZod, is.fSeconds ? "%11.7f" : "%7.3f", deg);
    break;
  }
  return szZod;
}


/* This is similar to formatting a zodiac degree, but here we return a */
/* string of a (signed) declination value in degrees and minutes.      */

byte *SzAltitude(deg)
real deg;
{
  static byte szAlt[10];
  int d, m, f;
  real s;
  byte ch;

  f = deg < 0.0;
  deg = RAbs(deg) + (is.fSeconds ? rRound/60.0/60.0 : rRound/60.0);
  d = (int)deg;
  m = (int)(RFract(deg)*60.0);
  ch = us.fAnsiChar > 1 ? 248 : chDeg1;
  sprintf(szAlt, "%c%2d%c%02d'", f ? '-' : '+', d, ch, m);
  if (is.fSeconds) {
    s = RFract(deg)*60.0; s = RFract(s)*60.0;
    sprintf(&szAlt[7], "%02d\"", (int)s);
  }
  return szAlt;
}


/* Here we return a string simply expressing the given value as degrees */
/* and minutes (and sometimes seconds) in the 0 to 360 degree circle.   */

byte *SzDegree(deg)
real deg;
{
  static byte szPos[11];
  int d, m;
  real s;

  deg = RAbs(deg) + (is.fSeconds ? rRound/60.0/60.0 : rRound/60.0);
  d = (int)deg;
  m = (int)(RFract(deg)*60.0);
  sprintf(szPos, "%3d%c%02d'", d, chDeg1, m);
  if (is.fSeconds) {
    s = RFract(deg)*60.0; s = RFract(s)*60.0;
    sprintf(&szPos[7], "%02d\"", (int)s);
  }
  return szPos;
}


/* Another string formatter, here we return a date string given a month,    */
/* day, and year. We format with the day or month first based on whether    */
/* the "European" date variable is set or not. The routine also takes a     */
/* parameter to indicate how much the string should be abbreviated, if any. */

byte *SzDate(mon, day, yea, nFormat)
int mon, day, yea, nFormat;
{
  static byte szDat[20];

  if (us.fEuroDate) {
    switch (nFormat) {
    case  2: sprintf(szDat, "%2d %c%c%c%5d", day, chMon3(mon), yea); break;
    case  1: sprintf(szDat, "%d %s %d", day, szMonth[mon], yea);     break;
    case -1: sprintf(szDat, "%2d-%2d-%2d", day, mon, abs(yea)%100);  break;
    default: sprintf(szDat, "%2d-%2d-%4d", day, mon, yea);           break;
    }
  } else {
    switch (nFormat) {
    case  3: sprintf(szDat, "%c%c%c %2d, %d", chMon3(mon), day, yea); break;
    case  2: sprintf(szDat, "%c%c%c %2d%5d", chMon3(mon), day, yea);  break;
    case  1: sprintf(szDat, "%s %d, %d", szMonth[mon], day, yea);     break;
    case -1: sprintf(szDat, "%2d/%2d/%2d", mon, day, abs(yea)%100);   break;
    default: sprintf(szDat, "%2d/%2d/%4d", mon, day, yea);            break;
    }
  }
  return szDat;
}


/* Return a string containing the given time expressed as an hour and */
/* minute quantity. This is formatted in 24 hour or am/pm time based  */
/* on whether the "European" time format flag is set or not.          */

byte *SzTime(hr, min, sec)
int hr, min, sec;
{
  static byte szTim[12];

  while (min >= 60) {
    min -= 60;
    hr++;
  }
  while (hr < 0)
    hr += 24;
  while (hr >= 24)
    hr -= 24;
  if (us.fEuroTime) {
    if (sec == -1)
      sprintf(szTim, "%2d:%02d", hr, min);
    else
      sprintf(szTim, "%2d:%02d:%02d", hr, min, sec);
  } else {
    if (sec == -1)
      sprintf(szTim, "%2d:%02d%cm", Mod12(hr), min, hr < 12 ? 'a' : 'p');
    else
      sprintf(szTim, "%2d:%02d:%02d%cm", Mod12(hr), min, sec,hr < 12 ? 'a' : 'p');
  }
  return szTim;
}


/* This just determines the correct hour and minute and calls the above. */

byte *SzTim(tim)
real tim;
{
  int hr, min, sec;
  real rMin;

  hr = NFloor(tim);
  rMin = RFract(RAbs(tim)) * 100.0;
  min = (int)(rMin + rRound / 600.0);
  rFractal = RFract(rMin);
  if (rFractal > rOne)
    rFractal = rSmall;
  sec = (int)(60.0*rFractal + rRound);
  return SzTime(hr, min, sec);
/*  return SzTime(NFloor(tim), (int)(RFract(RAbs(tim))*100.0+rRound/600.0)); */
}


/* Return a string containing the given time zone, given as a real value     */
/* having the hours before GMT in the integer part and minutes fractionally. */

byte *SzZone(zon)
real zon;
{
  static byte szZon[7];

  sprintf(szZon, "%c%d:%02d", zon > 0.0 ? '-' : '+', (int)RAbs(zon),
    (int)(RFract(RAbs(zon))*100.0+rRound/60.0));
  return szZon;
}


/* Nicely format the given longitude and latitude locations and return    */
/* them in a string. Various parts of the program display a chart header, */
/* and this allows the similar computations to be coded only once.        */

byte *SzLocation(lon, lat)
real lon, lat;
{
  static byte szLoc[32];
  int mini, minj, seci, secj;
  real rMin;
  byte ch1, ch2, ch3;

  rMin = RFract(RAbs(lon)) * 100.0;
  mini = (int)(rMin + rRound / 600.0);
  rFractal = RFract(rMin);
  if (rFractal > rOne)
    rFractal = rSmall;
  seci = (int)(60.0 * rFractal + rRound);

  rMin = RFract(RAbs(lat)) * 100.0;
  minj = (int)(rMin + rRound / 600.0);
  rFractal = RFract(rMin);
  if (rFractal > rOne)
    rFractal = rSmall;
  secj = (int)(60.0 * rFractal + rRound);

  ch1 = us.fAnsiChar > 1 ? 248 : chDeg1;
  ch2 = us.fAnsiChar > 1 ?  39 : chMin1;
  ch3 = us.fAnsiChar > 1 ?  34 : chSec1;
  if (us.fAnsiChar) {
    if (us.fAnsiChar != 3) {
      sprintf(szLoc, "%3.0f%c%02d%c%02d%c%c%3.0f%c%02d%c%02d%c%c",
        RFloor(RAbs(lon)), ch1, mini, ch2, seci, ch3, lon < 0.0 ? 'E' : 'W',
        RFloor(RAbs(lat)), ch1, minj, ch2, secj, ch3, lat < 0.0 ? 'S' : 'N');
    } else {
      sprintf(szLoc, "%3.0f%c%02d%c%02d%3.0f%c%02d%c%02d",
        RFloor(RAbs(lon)), lon < 0.0 ? 'E' : 'W', mini, ch2, seci,
        RFloor(RAbs(lat)), lat < 0.0 ? 'S' : 'N', minj, ch2, secj);
    }
  } else {
    sprintf(szLoc, "%3.0f%c%02d%c%02d%c%3.0f%c%02d%c%02d%c",
      RFloor(RAbs(lon)), ch1, mini, ch2, seci, lon < 0.0 ? 'E' : 'W',
      RFloor(RAbs(lat)), ch1, minj, ch2, secj, lat < 0.0 ? 'S' : 'N');
  }
  return szLoc;
}


#ifdef TIME
/* Compute the date and time it is right now as the program is running      */
/* using the computer's internal clock. We do this by getting the number    */
/* of seconds which have passed since January 1, 1970 and going from there. */
/* The time return value filled is expressed in the given zone parameter.   */

void GetTimeNow(mon, day, yea, tim, zon)
int *mon, *day, *yea;
real *tim, zon;
{
  dword curtimer;
  int min, sec;
  real hr;

  time(&curtimer);
  sec = (int)(curtimer % 60);
  curtimer = curtimer / 60 + us.lTimeAddition;
  min = (int)(curtimer % 60);
  curtimer /= 60;
#ifdef MAC
  curtimer += 8;
#endif
  hr = (real)(curtimer % 24) - zon;
  curtimer /= 24;
  while (hr < 0.0) {
    curtimer--;
    hr += 24.0;
  }
  while (hr >= 24.0) {
    curtimer++;
    hr -= 24.0;
  }
  curtimer += ldTime;  /* Number of days between 1/1/1970 and 1/1/4713 BC. */
  JulianToMdy((real)curtimer, mon, day, yea);
  *tim = hr + (real)min / 100.0 + (real)sec / 6000.0;
}
#endif /* TIME */


#ifdef PCG
/* Map one character value to another. This is used in processing special  */
/* keys and alt key combinations, which are read in from the keyboard as a */
/* zero immediately followed by some value. This converts that value into  */
/* something more useful to process and deal with.                         */

int NFromAltN(nAlt)
int nAlt;
{
  /* Map number pad keys to the numbers characters they correspond to. */
  if (nAlt == 82)
    return '0';
  else if (FBetween(nAlt, 79, 81))
    return '1' + nAlt - 79;
  else if (FBetween(nAlt, 75, 77))
    return '4' + nAlt - 75;
  else if (FBetween(nAlt, 71, 73))
    return '7' + nAlt - 71;

  /* Map F1 through F12 function keys to the values 201-212. */
  else if (FBetween(nAlt, 59, 68))
    return 201 + nAlt - 59;
  else if (FBetween(nAlt, 133, 134))
    return 211 + nAlt - 133;

  /* Map Shift+F1 through Shift+F12 keys to the values 213-224. */
  else if (FBetween(nAlt, 84, 93))
    return 213 + nAlt - 84;
  else if (FBetween(nAlt, 135, 136))
    return 223 + nAlt - 135;

  /* Map Control+F1 through Control+F12 keys to the values 225-236. */
  else if (FBetween(nAlt, 94, 103))
    return 225 + nAlt - 94;
  else if (FBetween(nAlt, 137, 138))
    return 235 + nAlt - 137;

  /* Map Alt+F1 through Alt+F12 keys to the values 237-248. */
  else if (FBetween(nAlt, 104, 113))
    return 237 + nAlt - 104;
  else if (FBetween(nAlt, 139, 140))
    return 247 + nAlt - 139;

  return chNull;
}
#endif


/* Given a string representing the complete pathname to a file, strip off    */
/* all the path information leaving just the filename itself. This is called */
/* by the main program to determine the name of the Astrolog executable.     */

byte *ProcessProgname(szPath)
byte *szPath;
{
  byte *b, *c, *e;

  b = c = szPath;
  while (*c) {
#ifdef PC
    *c = ChUncap(*c);    /* Because DOS filenames are case insensitive. */
#endif
    c++;
  }
  e = c;
  while (c > b && *c != '.')
    c--;
  if (c > b)
    *c = 0;
  else
    c = e;
  while (c > b && *c != chDirSep)
    c--;
  if (c > b)
    szPath = c+1;
  return szPath;
}


/* Given a string, return a pointer to a persistent version of it, where  */
/* 'persistent' means its contents won't be invalidated when the stack    */
/* frame changes. Strings such as macros, et al, need to be in their own  */
/* space and can't just be local variables in a function reading them in. */

byte *SzPersist(szSrc)
byte *szSrc;
{
  byte szT[cchSzDef], *szNew;
  int cb;

  /* Some strings such as outer level command line parameter arguments */
  /* already persist, so we can just return the same string passed in. */
  if (is.fSzPersist)
    return szSrc;

  /* Otherwise we make a copy of the string in the local heap and use it. */
  cb = CchSz(szSrc)+1;
  AllocateNear(szNew, cb);
  if (szNew == NULL) {
    sprintf(szT, "%s: Not enough near memory for string (%d bytes).",
      szAppName, cb);
    PrintWarning(szT);
  } else
    CopyRgb((byte *)szSrc, (byte *)szNew, cb);
  return szNew;
}


/* This is Astrolog's memory allocation routine, returning a pointer given */
/* a size, a flag for if it is a more than 64K huge allocation, and a      */
/* string to use when printing an error if the allocation fails.           */

lpbyte PAllocate(lcb, fHuge, szType)
long lcb;
bool fHuge;
byte *szType;
{
  byte szT[cchSzDef];
  lpbyte lp;

  if (fHuge)
    AllocateHuge(lp, lcb);
  else
    AllocateFar(lp, (int)lcb);
#ifdef PC
  /* For PC's the array better not cross a segment boundary. */
  if (lp && !fHuge && WHi(WLo(lp) + lcb) > 0)
    lp = NULL;
#endif
  if (lp == NULL && szType) {
    sprintf(szT, "%s: Not enough memory for %s (%ld bytes).",
      szAppName, szType, lcb);
    PrintWarning(szT);
  }
  return lp;
}

/* general.c */
