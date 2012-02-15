/*
** Astrolog (Version 5.41G) File: xcharts0.c
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


#ifdef GRAPH
/*
******************************************************************************
** Subchart Graphics Routines.
******************************************************************************
*/

/* Given a string, draw it on the screen using the given color. The       */
/* position of the text is based the saved positions of where we drew the */
/* text the last time the routine was called, being either directly below */
/* in the same column or in the same row just to the right. This is used  */
/* by the sidebar drawing routine to print a list of text on the chart.   */

int DrawPrint(sz, m, n)
byte *sz;
int m, n;

{
  static int x0, x, y;

  if (sz == NULL) {    /* Null string means just initialize position. */
    x0 = x = m; y = n;
    return y;
  }
  if (y >= gs.yWin-1)  /* Don't draw if we've scrolled off the chart bottom. */
    return y;
  DrawColor(m);
  DrawSz(sz, x, y, dtLeft | dtBottom);

  /* If the second parameter is TRUE, we stay on the same line, otherwise */
  /* when FALSE we go to the next line at the original column setting.    */

  if (n)
    x += CchSz(sz)*xFont*gi.nScaleT;
  else {
    x = x0;
    n = y;
    y += yFont*gi.nScaleT;
  }
  return y;
}


/* Print text showing the chart information and house and planet positions */
/* of a chart in a "sidebar" to the right of the chart in question. This   */
/* is always done for the -v and -w graphic wheel charts unless the -v0    */
/* switch flag is also set, in which case none of the stuff here is done.  */

void DrawInfo()
{
  byte sz[cchSzDef];
  ET et;
  CI ciT;
  int i, y, a, s;
  real r, deg, se;
  int kJDcolor;
  int de, mi;
  byte ch = 248, nT;

#ifdef INTERPRET
  int tot, abo, lef;

  /* Hack: Just for fun, if interpretation is active (which normally has  */
  /* no effect whatsoever on graphics) we'll decorate the chart a little. */

  if (us.fInterpret) {
    if (us.nScreenWidth & 1) {

      /* If screenwidth value is odd, draw a moire pattern in each corner. */

      abo = gs.yWin/(us.nScreenWidth/10);
      lef = gs.xWin/(us.nScreenWidth/10);
      for (y = 0; y <= 1; y++)
        for (i = 0; i <= 1; i++)
          for (s = 0; s <= 1; s++)
            for (a = 1; a < (s ? lef : abo)*2; a++) {
              DrawColor(a & 1 ? gi.kiGray : gi.kiOff);
              DrawLine(i ? gs.xWin-1-lef : lef, y ? gs.yWin-1-abo : abo,
                s ? (i ? gs.xWin-1-a : a) : i*(gs.xWin-1),
                s ? y*(gs.yWin-1) : (y ? gs.yWin-1-a : a));
            }
    } else {

      /* If screenwidth is even, draw spider web lines in each corner. */

      DrawColor(gi.kiGray);
      tot = us.nScreenWidth*3/20;
      abo = gs.yWin/4;
      lef = gs.xWin/4;
      for (y = 0; y <= 1; y++)
        for (i = 0; i <= 1; i++)
          for (a = 1; a < tot; a++)
            DrawLine(i*(gs.xWin-1), y ? (gs.yWin-1-a*abo/tot) : a*abo/tot,
              i ? gs.xWin-1-lef+a*lef/tot : lef-a*lef/tot, y*(gs.yWin-1));
    }
  }
#endif
  if (!gs.fText || us.fVelocity)    /* Don't draw sidebar if */
    return;                         /* -v0 flag is set.      */

  a = us.fAnsiChar;
  us.fAnsiChar = (!gs.fFont || (!gs.fMeta && !gs.fPS)) << 1;
  DrawColor(gi.kiLite);
  if (gs.fBorder)
    DrawLine(gs.xWin-1, 0, gs.xWin-1, gs.yWin-1);
  gs.xWin += xSideT;
  DrawPrint(NULL, gs.xWin-xSideT+xFontT-gi.nScaleT, yFont*7/5*gi.nScaleT);

  /* Print chart header and setting information. */

  sprintf(sz, "%s %s", szAppName, szVersionCore);
  DrawPrint(sz, gi.kiOn, fFalse);

  if (gs.fAlt) {
     sprintf(sz, "Bonus Mode");
     DrawPrint(sz, gi.kiLite, fFalse);
  }
  if (us.nRel < 0) {
    if ((us.nRel != rcTriWheel) && (us.nRel != rcQuadWheel))
      sprintf(sz, "Inner ring:");
    else
      sprintf(sz, "Outer ring:");
    DrawPrint(sz, gi.kiLite, fFalse);
  }
  if (us.fProgress && !us.nRel) {
      ciT = ciMain;
      ciMain = ciCore;
  }
  if (*ciMain.nam && (us.nRel >= rcDifference || us.nRel <= 0))
    DrawPrint(ciMain.nam, gi.kiLite, fFalse);

  kJDcolor = gi.kiOn;

  if (fNoTimeOrSpace || us.nRel == rcMidpoint || us.nRel == rcComposite
                     || us.nRel == rcSynastry) {
    if (fNoTimeOrSpace) {
      sprintf(sz, "No time or space");
      kJDcolor = 0;
    }
    else if (us.nRel == rcMidpoint) {
      sprintf(sz, "Time and space Midpoint");
      kJDcolor = gi.kiOn;
    }
    else if (us.nRel == rcComposite) {
      sprintf(sz, "Composite chart");
      kJDcolor = 0;
    }
    else if (us.nRel == rcSynastry) {
      sprintf(sz, "Synastry.  Houses:");
      kJDcolor = kYellow;
    }
    DrawPrint(sz, gi.kiLite, fFalse);
  }

  if (!fNoTimeOrSpace) {
    sprintf(sz, "%c%c%c %s", chDay3(DayOfWeek(Mon, Day, Yea)),
      SzDate(Mon, Day, Yea, fTrue));
    DrawPrint(sz, gi.kiOn, fFalse);
    DrawPrint(SzTim(Tim), gi.kiOn, fTrue);
    sprintf(sz, " (%cT %s GMT)", ChDst(Dst), SzZone(Zon));
    DrawPrint(sz, gi.kiOn, fFalse);
  }
  if (*ciMain.loc && us.nRel != rcMidpoint)
    DrawPrint(ciMain.loc, gi.kiLite, fFalse);

  if (Mon != -1)
    DrawPrint(SzLocation(Lon, Lat), gi.kiLite, fFalse);

  if (us.fProgress && !us.nRel)
    ciMain = ciT;

  if (us.nRel < 0 || us.nRel == rcSynastry || us.nRel == rcComposite 
                  || us.fProgress) {
    if (us.nRel < 0 || us.nRel == rcComposite || us.nRel == rcSynastry) {
      ciT = ciMain;
      ciMain = ciTwin;
    }
    if (us.nRel == rcDual) {
    sprintf(sz, "Outer ring:");
    kJDcolor = kYellow;
    DrawPrint(sz, gi.kiLite, fFalse);
    if (*ciMain.nam)
      DrawPrint(ciMain.nam, gi.kiLite, fFalse);
    }
    if (us.nRel == rcTransit) {
      sprintf(sz, "Outer ring - Transit:");
      DrawPrint(sz, gi.kiLite, fFalse);
      kJDcolor = kYellow;
    }
    if (us.nRel == rcProgress) {
      if (us.fSolarArc) {
        if (us.fSolarArc == 1) {
          sprintf(sz, "Degree per Year/Month -");
          DrawPrint(sz, gi.kiLite, fFalse);
        }
        if (us.fSolarArc == 2) {
          sprintf(sz, "Solar Arc -");
          DrawPrint(sz, gi.kiLite, fFalse);
        }
        sprintf(sz, "outer ring directed to:");
        kJDcolor = kYellow;
      } else {
        sprintf(sz, "Outer ring progressed to:");
        kJDcolor = kGreen;
      }
      DrawPrint(sz, gi.kiLite, fFalse);
    }
    if (us.nRel == rcTriWheel || us.nRel == rcQuadWheel) {
      sprintf(sz, "Second ring:");
      DrawPrint(sz, gi.kiLite, fFalse);
    }
    if (us.fProgress && !us.nRel) {
      if (us.fSolarArc) {
        if (us.fSolarArc == 1) {
          sprintf(sz, "Degree per Year/Month -");
          DrawPrint(sz, gi.kiLite, fFalse);
        }
        if (us.fSolarArc == 2) {
          sprintf(sz, "Solar Arc -");
          DrawPrint(sz, gi.kiLite, fFalse);
        }
        sprintf(sz, "chart directed to:");
        kJDcolor = kYellow;
      } else {
        sprintf(sz, "Chart progressed to:");
        kJDcolor = kGreen;
      }
      DrawPrint(sz, gi.kiLite, fFalse);
    }
    if (us.nRel == rcSynastry) {
      sprintf(sz, "Planets:");
      DrawPrint(sz, gi.kiLite, fFalse);
    }
    sprintf(sz, "%c%c%c %s", chDay3(DayOfWeek(Mon, Day, Yea)),
      SzDate(Mon, Day, Yea, fTrue));
    DrawPrint(sz, kYellow, fFalse);
    DrawPrint(SzTim(Tim), kYellow, fTrue);
    sprintf(sz, " (%cT %s GMT)", ChDst(Dst), SzZone(Zon));
    DrawPrint(sz, kYellow, fFalse);
    if (us.nRel != rcProgress) {  
      if (*ciMain.loc)
        DrawPrint(ciMain.loc, gi.kiLite, fFalse);
      DrawPrint(SzLocation(Lon, Lat), gi.kiLite, fFalse);
    }
    if (us.nRel < 0 || us.nRel == rcComposite || us.nRel == rcSynastry)
      ciMain = ciT;
  }

  sprintf(sz, "%s houses.", szSystem[us.nHouseSystem]);
  DrawPrint(sz, gi.kiLite, fFalse);
  sprintf(sz, "%s, %s.", us.fSidereal ? "Sidereal" : "Tropical",
    us.objCenter == oSun ? "Heliocentric" :
    (us.objCenter == oEar ? "Geocentric" : szObjName[us.objCenter]));
  DrawPrint(sz, gi.kiLite, fFalse);

  if (kJDcolor) {
    if ((us.fProgress || us.nRel == rcProgress) && us.fSolarArc) {
      sprintf(sz, "Julian Day = %11.4f", is.JDp - 0.5);
    } else {
      sprintf(sz, "Julian Day = %11.4f", JulianDayFromTime(is.T));
    }
    DrawPrint(sz, kJDcolor, fFalse);
/*
  sprintf(sz, "Obliquity = %9.6f", DFromR(is.OB));
  DrawPrint(sz, gi.kiLite, fFalse);
*/
    deg = DFromR(is.OB) + rRound / 3600.0;
    de = (int)deg;
    mi = (int)(RFract(deg)*60.0);
    se = RFract(deg)*60.0;  se = RFract(se)*60.0;
    sprintf(sz, "Obliquity = %2d%c%02d'%02d\"", de, ch, mi, (int)se);
    DrawPrint(sz, gi.kiLite, fFalse);

    nT = us.fEuroTime; us.fEuroTime = fTrue;
    sprintf(sz, "Sidereal time: %s", SzTim(DegToDec(DFromR(is.RA)*(24.0/rDegMax))));
    DrawPrint(sz, gi.kiLite, fFalse);
    us.fEuroTime = nT;
  }

#ifdef PLACALC
  if (kJDcolor && us.fPlacalc) {
    sprintf(sz, "DeltaT = %8.4f", deltaT * 86400.0);
    DrawPrint(sz, gi.kiLite, fFalse);
  }
#endif

  if (us.fSidereal) {
    deg = RAbs(is.rSid) + rRound / 3600.0;
    de = (int)(deg*RSgn2(is.rSid));
    mi = (int)(RFract(deg)*60.0);
    se = RFract(deg)*60.0;  se = RFract(se)*60.0;
    sprintf(sz, "Ayanamsha = %2d%c%02d'%02d\"", de, ch, mi, (int)se);
    DrawPrint(sz, gi.kiLite, fFalse);
/*
    sprintf(sz, "Ayanamsha = %9.6f", Offset);
    DrawPrint(sz, gi.kiLite, fFalse);
*/
  }

  if(InfoFlag && !us.fSector) {
     DrawPrint("", gi.kiLite, fFalse);
     sprintf(sz, "Harm = "); DrawPrint(sz, kAspB[4], fTrue);
     sprintf(sz, "%+4.1f ", SHarmon);DrawPrint(sz, gi.kiLite, fTrue);
     sprintf(sz, "Conj = "); DrawPrint(sz, kAspB[1], fTrue);
     sprintf(sz, "%4.1f ", SConjunct); DrawPrint(sz, gi.kiLite, fFalse);
     sprintf(sz, "Tens = "); DrawPrint(sz, kAspB[3], fTrue);
     sprintf(sz, "%4.1f ", STension);DrawPrint(sz, gi.kiLite, fTrue);
     sprintf(sz, "Total = "); DrawPrint(sz, kAspB[2], fTrue);
     sprintf(sz, "%4.1f ", SSumma);DrawPrint(sz, gi.kiLite, fFalse);
     sprintf(sz, "Summ = "); DrawPrint(sz, gi.kiLite, fTrue);
     sprintf(sz, "%+4.1f ", SChart);
     if (SChart > 75.0)
        DrawPrint(sz, kAspB[4], fFalse);
     else if (SChart > 25.0)
        DrawPrint(sz, kAspB[5], fFalse);
     else if (SChart > -25.0)
        DrawPrint(sz, kAspB[1], fFalse);
     else if (SChart > -75.0)
        DrawPrint(sz, kAspB[6], fFalse);
     else
        DrawPrint(sz, kAspB[3], fFalse);
  }

  /* Print house cusp positions. */

  DrawPrint("", gi.kiLite, fFalse);
  for (i = 1; i <= cSign; i++) {
    sprintf(sz, "%2d%s house: ", i, szSuffix[i]);
    y = DrawPrint(sz, kSignB(i), fTrue);
    if (!is.fSeconds && (gs.nScale == 100 ||
      !gs.fFont || !gi.fFile || gs.fBitmap) && y < gs.yWin-1) {
      s = gi.nScale;
      gi.nScale = gi.nScaleT;
      DrawSign(SFromZ(chouse[i]), gs.xWin-12*gi.nScaleT,
        y-(yFont/2-1)*gi.nScaleT);
      gi.nScale = s;
    }
    DrawPrint(SzZodiac(chouse[i]), kSignB(SFromZ(chouse[i])), fFalse);
  }

  /* Print planet positions. */

  DrawPrint("", gi.kiLite, fFalse);
  for (i = 0; i <= oNorm; i++) if (FProper2(i) && (!FCusp(i)||(i==21)||(i==30))) {
    sprintf(sz, is.fSeconds ? "%3.3s: " : "%4.4s: ", szObjName[i]);
    DrawPrint(sz, kObjB[i], fTrue);
    y = DrawPrint(SzZodiac(planet[i]), kSignB(SFromZ(planet[i])), fTrue);
    if (!is.fSeconds && i < starLo && gi.nMode != gSector && (gs.nScale ==
      100 || !gs.fFont || !gi.fFile || gs.fBitmap) && y < gs.yWin-1) {
      s = gi.nScale;
      gi.nScale = gi.nScaleT;
      DrawObject(i, gs.xWin-12*gi.nScaleT, y-(yFont/2-1)*gi.nScaleT);
      gi.nScale = s;
    }
    sprintf(sz, "%c ", ret[i] < 0.0 ? chRet : ' ');
    DrawPrint(sz, gi.kiOn, fTrue);
    if (gi.nMode != gSector || !is.fSeconds) {
      if (FThing(i)) {
        is.fSeconds = fFalse;
        DrawPrint(SzAltitude(planetalt[i]), gi.kiLite, fTrue);
        is.fSeconds = us.fSeconds;
      } else
        DrawPrint("       ", gi.kiOn, fTrue);
    }
    if (gi.nMode == gSector) {
      r = ((rDegMax - cp2.obj[i])/10.0); s = (int)r + 1;
      if (!is.fSeconds)
        sprintf(sz, " %2d", s);
      else
        sprintf(sz, "%6.3f%c", r + 1.0, pluszone[s] ? '+' : '-');
      DrawPrint(sz, pluszone[s] ? kRainbowB[1] : kMainB[5], fFalse);
    } else
      DrawPrint("", gi.kiOn, fFalse);
  }

  /* Print star positions. */

  for (i = starLo; i <= starHi; i++) if (FProper2(i)) {
    s = oNorm+starname[i-oNorm];
    sprintf(sz, is.fSeconds ? "%3.3s: " : "%4.4s: ", szObjName[s]);
    DrawPrint(sz, kObjB[s], fTrue);
    DrawPrint(SzZodiac(planet[s]), kSignB(SFromZ(planet[s])), fTrue);
    DrawPrint("  ", gi.kiOn, fTrue);
    if (gi.nMode != gSector || !is.fSeconds)
      DrawPrint(SzAltitude(planetalt[s]), gi.kiLite, fTrue);
    if (gi.nMode == gSector) {
      r = GFromO(s); s = (int)r + 1;
      if (!is.fSeconds)
        sprintf(sz, " %2d", s);
      else
        sprintf(sz, "%6.3f%c", r + 1.0, pluszone[s] ? '+' : '-');
      DrawPrint(sz, pluszone[s] ? kRainbowB[1] : kMainB[5], fFalse);
    } else
      DrawPrint("", gi.kiOn, fFalse);
  }

  /* Print element table information. */

  DrawPrint("", gi.kiLite, fFalse);
  CreateElemTable(&et);
  sprintf(sz, "Fire: %d, Earth: %d,", et.coElem[eFir], et.coElem[eEar]);
  DrawPrint(sz, gi.kiLite, fFalse);
  sprintf(sz, "Air : %d, Water: %d", et.coElem[eAir], et.coElem[eWat]);
  DrawPrint(sz, gi.kiLite, fFalse);
  sprintf(sz, "Car: %d, Fix: %d, Mut: %d",
    et.coMode[0], et.coMode[1], et.coMode[2]);
  DrawPrint(sz, gi.kiLite, fFalse);
  sprintf(sz, "Yang: %d, Yin: %d", et.coYang, et.coYin);
  DrawPrint(sz, gi.kiLite, fFalse);
  sprintf(sz, "M: %d, N: %d, A: %d, D: %d",
    et.coMC, et.coIC, et.coAsc, et.coDes);
  DrawPrint(sz, gi.kiLite, fFalse);
  sprintf(sz, "Ang: %d, Suc: %d, Cad: %d",
    et.coModeH[0], et.coModeH[1], et.coModeH[2]);
  DrawPrint(sz, gi.kiLite, fFalse);
  sprintf(sz, "Learn: %d, Share: %d", et.coLearn, et.coShare);
  DrawPrint(sz, gi.kiLite, fFalse);
  us.fAnsiChar = a;
/*  if (us.nRel == rcTransit)
    ciMain = ciT; */
}


/* This is a subprocedure of XChartWheel() and XChartWheelRelation(). Draw  */
/* the outer sign and house rings for a wheel chart at the specified zodiac */
/* locations and at the given radius values.                                */

void DrawWheel(xsign, xhouse, cx, cy, unitx, unity, asc, r1, r2, r3, r4, r5)
real *xsign, *xhouse;
int cx, cy;
real unitx, unity, asc, r1, r2, r3, r4, r5;
{
  int i, sc;
  real px, py, temp;

  /* Draw Ascendant/Descendant and Midheaven/Nadir lines across whole chart. */

  DrawColor(gi.kiLite);
  DrawDash(cx+POINT1(unitx, 0.99, PX(xhouse[sAri])),
           cy+POINT1(unity, 0.99, PY(xhouse[sAri])),
           cx+POINT1(unitx, 0.99, PX(xhouse[sLib])),
           cy+POINT1(unity, 0.99, PY(xhouse[sLib])), !gs.fColor);
  DrawDash(cx+POINT1(unitx, 0.99, PX(xhouse[sCap])),
           cy+POINT1(unity, 0.99, PY(xhouse[sCap])),
           cx+POINT1(unitx, 0.99, PX(xhouse[sCan])),
           cy+POINT1(unity, 0.99, PY(xhouse[sCan])), !gs.fColor);

  /* Draw small five or one degree increments around the zodiac sign ring. */

  for (i = 0; i < nDegMax; i++) {
    temp = PZ(HousePlaceInX((real)i));
    px = PX(temp); py = PY(temp);
    DrawColor(i%5 ? gi.kiGray : gi.kiOn);
    DrawDash(cx+POINT1(unitx, r3, px), cy+POINT1(unity, r3, py),
      cx+POINT2(unitx, r4, px), cy+POINT2(unity, r4, py),
      ((!gs.fColor || gs.fPS || gs.fMeta) && i%5)*2);
  }

  /* Draw circles for the zodiac sign and house rings. */

  DrawColor(gi.kiOn);
  DrawCircle(cx, cy, (int)(unitx*0.95+rRound), (int)(unity*0.95+rRound));
  DrawCircle(cx, cy, (int)(unitx*r4+rRound), (int)(unity*r4+rRound));
  DrawCircle(cx, cy, (int)(unitx*r3+rRound), (int)(unity*r3+rRound));
  DrawCircle(cx, cy, (int)(unitx*r1+rRound), (int)(unity*r1+rRound));

  /* Draw the glyphs for the signs and houses themselves. */

  for (i = 1; i <= cSign; i++) {
    temp = xsign[i];
    DrawColor(gi.kiOn);
    DrawLine(cx+POINT2(unitx, 0.95, PX(temp)),    /* Draw lines separating */
      cy+POINT2(unity, 0.95, PY(temp)),           /* each sign and house   */
      cx+POINT1(unitx, r4, PX(temp)),             /* from each other.      */
      cy+POINT1(unity, r4, PY(temp)));
    DrawLine(cx+POINT2(unitx, r3, PX(xhouse[i])),
      cy+POINT2(unity, r3, PY(xhouse[i])),
      cx+POINT1(unitx, r1, PX(xhouse[i])),
      cy+POINT1(unity, r1, PY(xhouse[i])));
    if (gs.fColor && i%3 != 1) {                             /* Lines from */
      DrawColor(gi.kiGray);                                  /* each house */
      DrawDash(cx, cy, cx+POINT2(unitx, r1, PX(xhouse[i])),  /* to center  */
        cy+POINT2(unity, r1, PY(xhouse[i])), 1);             /* of wheel.  */
    }
    temp = Midpoint(temp, xsign[Mod12(i+1)]);
    DrawColor(kSignB(i));
    DrawSign(i, cx+POINT1(unitx, r5, PX(temp)),
      cy+POINT1(unity, r5, PY(temp)));
    if (fDisp) {                          /* Draw dispositor's glyph. VA. */
      sc = gi.nScale;
      temp = Midpoint(xsign[i], temp);
      if (gi.nScale > 1)
        gi.nScale -= 1;
      DrawObject(rules[i], cx+POINT1(unitx, r5, PX(temp)),
        cy+POINT1(unity, r5, PY(temp)));
      gi.nScale = sc;
    }
    DrawColor(kSignB(i));
    temp = Midpoint(xhouse[i], xhouse[Mod12(i+1)]);
    DrawHouse(i, cx+POINT1(unitx, r2, PX(temp)),
      cy+POINT1(unity, r2, PY(temp)));
    if (fDisp) {
      sc = gi.nScale;
      temp = Midpoint(xhouse[i], temp);
      if (gi.nScale > 1)
        gi.nScale -= 1;
      DrawObject(rules[i], cx+POINT1(unitx, r2, PX(temp)),
        cy+POINT1(unity, r2, PY(temp)));
      gi.nScale = sc;
    }
  }
}


/* Another subprocedure of XChartWheel() and XChartWheelRelation(). Draw */
/* a set of planets in a wheel chart, drawing each glyph and a line from */
/* it to a dot indicating the planet's actual location.                  */

void DrawSymbolRing(symbol, xplanet, dir, cx, cy, unitx, unity, r1, r2, r3, r4)
real *symbol, *xplanet, *dir;
int cx, cy;
real unitx, unity, r1, r2, r3, r4;
{
  int i;
  real temp;

  for (i = cObj; i >= 0; i--) if (FProper(i)) {
    if (gs.fLabel) {
      temp = symbol[i];
      DrawColor(dir[i] < 0.0 ? gi.kiGray : gi.kiOn);
      DrawDash(cx+POINT1(unitx, r2, PX(xplanet[i])),
        cy+POINT1(unity, r2, PY(xplanet[i])),
        cx+POINT1(unitx, r3, PX(temp)),
        cy+POINT1(unity, r3, PY(temp)),
        (ret[i] < 0.0 ? 1 : 0) - gs.fColor);
      DrawObject(i, cx+POINT1(unitx, r4, PX(temp)),
        cy+POINT1(unity, r4, PY(temp)));
    } else
      DrawColor(kObjB[i]);
    DrawSpot(cx+POINT1(unitx, r1, PX(xplanet[i])),
      cy+POINT1(unity, r1, PY(xplanet[i])));
  }
}


/*
******************************************************************************
** Map Chart Routines.
******************************************************************************
*/

/* Another stream reader, this one is used by the globe drawing routine: */
/* for the next body of land/water, return its name (and color), its     */
/* longitude and latitude, and a vector description of its outline.      */

bool FReadWorldData(nam, loc, lin)
byte FPTR **nam, FPTR **loc, FPTR **lin;
{
  static byte FPTR **psz = (byte FPTR **)szWorldData;
  int i;

  *loc = *psz++;
  *lin = *psz++;
  *nam = *psz++;
  if (*loc[0]) {
    if (gs.fPrintMap && gi.fFile) {
      i = **nam - '0';
      AnsiColor(i ? kRainbowA[i] : kMainA[7]);
      PrintSz(*nam+1); PrintL();
    }
    return fTrue;
  }
  psz = (byte FPTR **)szWorldData;  /* Reset stream when no data left. */
  return fFalse;
}


/* Given longitude and latitude values on a globe, return the window        */
/* coordinates corresponding to them. In other words, project the globe     */
/* onto the view plane, and return where our coordinates got projected to,  */
/* as well as whether our location is hidden on the back side of the globe. */

bool FGlobeCalc(x1, y1, u, v, cx, cy, rx, ry, deg)
real x1, y1;
int *u, *v, cx, cy, rx, ry, deg;
{
  real j, siny1;

  /* Compute coordinates for a general globe invoked with -XG switch. */

  if (gi.nMode == gGlobe) {
    x1 = Mod(x1+(real)deg);    /* Shift by current globe rotation value. */
    if (gs.rTilt != 0.0) {
      /* Do another coordinate shift if the globe's equator is tilted any. */
      x1 = RFromD(x1); y1 = RFromD(rDegQuad-y1);
      CoorXform(&x1, &y1, RFromD(gs.rTilt));
      x1 = Mod(DFromR(x1)); y1 = rDegQuad-DFromR(y1);
    }
    *v = cy + (int)((real)ry*-RCosD(y1)-rRound);
    *u = cx + (int)((real)rx*-RCosD(x1)*RSinD(y1)-rRound);
    return x1 > rDegHalf;
  }

  /* Compute coordinates for a polar globe invoked with -XP switch. */

  siny1 = RSinD(y1);
  j = gs.fAlt ? rDegQuad+x1+deg : 270.0-x1-deg;
  *v = cy + (int)(siny1*(real)ry*RSinD(j)-rRound);
  *u = cx + (int)(siny1*(real)rx*RCosD(j)-rRound);
  return gs.fAlt ? y1 < rDegQuad : y1 > rDegQuad;
}


/* Draw one "Ley line" on the world map, based coordinates given in terms of */
/* longitude and vertical fractional distance from the center of the earth.  */

void DrawLeyLine(l1, f1, l2, f2)
real l1, f1, l2, f2;
{
  l1 = Mod(l1); l2 = Mod(l2);

  /* Convert vertical fractional distance to a corresponding coordinate. */

  f1 = rDegQuad-RAsin(f1)/rPiHalf*rDegQuad;
  f2 = rDegQuad-RAsin(f2)/rPiHalf*rDegQuad;
  DrawWrap((int)(l1*(real)gi.nScale+rRound),
           (int)(f1*(real)gi.nScale+rRound),
           (int)(l2*(real)gi.nScale+rRound),
           (int)(f2*(real)gi.nScale+rRound), 0, gs.xWin-1);
}


/* Draw the main set of planetary Ley lines on the map of the world. This */
/* consists of drawing an icosahedron and then a dodecahedron lattice.    */

void DrawLeyLines(deg)
int deg;
{
  real off = (real)deg, phi, h, h1, h2, r, i;

  phi = (RSqr(5.0)+1.0)/2.0;                   /* Icosahedron constants. */
  h = 1.0/(phi*2.0-1.0);
  DrawColor(kMainB[6]);
  for (i = off; i < rDegMax+off; i += 72.0) {  /* Draw icosahedron edges. */
    DrawLeyLine(i, h, i+72.0, h);
    DrawLeyLine(i-36.0, -h, i+36.0, -h);
    DrawLeyLine(i, h, i, 1.0);
    DrawLeyLine(i+36.0, -h, i+36.0, -1.0);
    DrawLeyLine(i, h, i+36.0, -h);
    DrawLeyLine(i, h, i-36.0, -h);
  }
  r = 1.0/RSqr(3.0)/phi/RCos(RFromD(54.0));    /* Dodecahedron constants. */
  h2 = RSqr(1.0-r*r); h1 = h2/(phi*2.0+1.0);
  DrawColor(kMainB[4]);
  for (i = off; i < rDegMax+off; i += 72.0) {  /* Draw docecahedron edges. */
    DrawLeyLine(i-36.0, h2, i+36.0, h2);
    DrawLeyLine(i, -h2, i+72.0, -h2);
    DrawLeyLine(i+36.0, h2, i+36.0, h1);
    DrawLeyLine(i, -h2, i, -h1);
    DrawLeyLine(i+36.0, h1, i+72.0, -h1);
    DrawLeyLine(i+36.0, h1, i, -h1);
  }
}


/* This major routine draws all of Astrolog's map charts. This means       */
/* either the world map or the constellations, in either rectangular or    */
/* globe hemisphere form. The rectangular chart may also be done in a      */
/* Mollewide projection, for six total combinations. We shift the chart by */
/* specified rotational and tilt values, and may draw on the chart each    */
/* planet at its zenith position on Earth or location in constellations.   */

void DrawMap(fSky, fGlobe, deg)
bool fSky, fGlobe;
int deg;
{
  byte *nam, *loc, *lin, chCmd;
  int X[objMax], Y[objMax], M[objMax], N[objMax],
    cx = gs.xWin/2, cy = gs.yWin/2, rx, ry, lon, lat, unit = 12*gi.nScale,
    x, y, xold, yold, m, n, u, v, i, j, k, l, nScl = gi.nScale;
  bool fNext = fTrue, fCan;
  real planet1[objMax], planet2[objMax], x1, y1, rT;
#ifdef CONSTEL
  byte *pch;
  bool fBlank;
  int isz = 0, nC, xT, yT, xDelta, yDelta, xLo, xHi, yLo, yHi;
#endif

  /* Set up some variables. */
  rx = cx-1; ry = cy-1;
  if (fGlobe)
    fCan = (gs.rTilt == 0.0 && gi.nMode != gPolar);

#ifdef CONSTEL
  /* Draw a dot grid for large rectangular constellation charts. */
  if (fSky && !fGlobe && !gs.fMollewide && gi.nScale/gi.nScaleT > 2)
    for (yT = 5; yT < nDegHalf; yT += 5)
      for (xT = 5; xT <= nDegMax; xT += 5) {
        DrawColor(xT % 15 == 0 && yT % 10 == 0 ? gi.kiOn : gi.kiGray);
        x = xT+deg;
        if (x > nDegMax)
          x -= nDegMax;
        DrawPoint(x*nScl, yT*nScl);
      }
#endif

  loop {

    /* Get the next chunk of data to process. Get the starting position, */
    /* map it to the screen, and set the drawing color appropriately.    */

    if (fNext) {
      fNext = fFalse;

      /* For constellations, get data for the next constellation shape. */

      if (fSky) {
#ifdef CONSTEL
        isz++;
        if (isz > cCnstl)
          break;
        DrawColor(gs.fAlt && gi.nMode != gPolar && (gi.nMode != gWorldMap ||
          !gs.fMollewide) ? kMainB[7] : kRainbowB[6]);
        pch = (byte *)szDrawConstel[isz];
        lon = nDegMax -
          (((pch[2]-'0')*10+(pch[3]-'0'))*15+(pch[4]-'0')*10+(pch[5]-'0'));
        lat = 90-((pch[6] == '-' ? -1 : 1)*((pch[7]-'0')*10+(pch[8]-'0')));
        pch += 9;
        xLo = xHi = xT = xold = x = lon;
        yLo = yHi = yT = yold = y = lat;
        nC = 0;
        if (fGlobe) {
          FGlobeCalc((real)x, (real)y, &m, &n, cx, cy, rx, ry, deg);
          k = l = fTrue;
        } else {
          xold += deg;
          x += deg;
        }
#else
        ;
#endif

      /* For world maps, get data for the next coastline piece. */

      } else {
        if (!FReadWorldData(&nam, &loc, &lin))
          break;
        i = nam[0]-'0';
        DrawColor((!fGlobe && gi.nMode == gAstroGraph) ? gi.kiOn :
          (gi.nMode == gGlobe && gs.fAlt) ? gi.kiGray :
          (i ? kRainbowB[i] : kMainB[7]));
        lon = (loc[0] == '+' ? 1 : -1)*
          ((loc[1]-'0')*100 + (loc[2]-'0')*10 + (loc[3]-'0'));
        lat = (loc[4] == '+' ? 1 : -1)*((loc[5]-'0')*10 + (loc[6]-'0'));
        if (fGlobe) {
          x = 180-lon;
          y = 90-lat;
          FGlobeCalc((real)x, (real)y, &m, &n, cx, cy, rx, ry, deg);
          k = l = fTrue;
        } else {
          xold = x = 181-lon+deg;
          yold = y = 90-lat;
        }
      }
    }

    /* Get the next unit from the string to draw on the screen as a line. */

    if (fSky) {

      /* For constellations we have a cache of how long we should keep    */
      /* going in the previous direction, as say "u5" for up five should  */
      /* move our pointer up five times without advancing string pointer. */

#ifdef CONSTEL
      if (nC <= 0) {
        if (!(chCmd = *pch)) {
          fNext = fTrue;
          if (gs.fText) {

            /* If we've reached the end of current constellation, compute */
            /* the center location in it based on lower and upper bounds  */
            /* we've maintained, and print the name of the constel there. */

            xT = xLo + (xHi - xLo)*(szDrawConstel[isz][0]-'1')/8;
            yT = yLo + (yHi - yLo)*(szDrawConstel[isz][1]-'1')/8;
            if (xT < 0)
              xT += nDegMax;
            else if (xT > nDegMax)
              xT -= nDegMax;
            if (fGlobe) {
              if (FGlobeCalc((real)xT, (real)yT, &x, &y, cx, cy, rx, ry, deg))
                continue;
            } else {
              xT += deg;
              if (xT > nDegMax)
                xT -= nDegMax;
              if (gs.fMollewide)
                x = 180*nScl + NMultDiv(xT-180, NMollewide(yT-91), 180L);
              else
                x = xT*nScl;
              y = yT*nScl;
            }
            DrawColor(gs.fAlt && gi.nMode != gPolar && (gi.nMode !=
              gWorldMap || !gs.fMollewide) ? gi.kiGray : kMainB[5]);
            DrawSz(szCnstlAbbrev[isz], x, y, dtCent);
          }
          continue;
        }
        pch++;

        /* Get the next direction and distance from constellation string. */

        if (fBlank = (chCmd == 'b'))
          chCmd = *pch++;
        xDelta = yDelta = 0;
        switch (chCmd) {
        case 'u': yDelta = -1; break;    /* Up    */
        case 'd': yDelta =  1; break;    /* Down  */
        case 'l': xDelta = -1; break;    /* Left  */
        case 'r': xDelta =  1; break;    /* Right */
        case 'U': yDelta = -1; nC = (yT-1)%10+1;    break;  /* Up until    */
        case 'D': yDelta =  1; nC = 10-yT%10;       break;  /* Down until  */
        case 'L': xDelta = -1; nC = (xT+599)%15+1;  break;  /* Left until  */
        case 'R': xDelta =  1; nC = 15-(xT+600)%15; break;  /* Right until */
        default: PrintError("Bad draw.");             /* Shouldn't happen. */
        }
        if (chCmd >= 'a')
          nC = NFromPch(&pch);    /* Figure out how far to draw. */
      }
      nC--;
      xT += xDelta; x += xDelta;
      yT += yDelta; y += yDelta;
      if (fBlank) {
        xold = x; yold = y;    /* We occasionally want to move the pointer */
        l = fFalse;            /* without drawing the line on the screen.  */
        continue;
      }
      if (xT < xLo)         /* Maintain our bounding rectangle for this */
        xLo = xT;           /* constellation if we crossed over it any. */
      else if (xT > xHi)
        xHi = xT;
      if (yT < yLo)
        yLo = yT;
      else if (yT > yHi)
        yHi = yT;
#else
      ;
#endif

    } else {

      /* Get the next unit from the much simpler world map strings. */

      if (!(chCmd = *lin)) {
        fNext = fTrue;
        continue;
      }
      lin++;

      /* Each unit is exactly one character in the coastline string. */

      if (chCmd == 'L' || chCmd == 'H' || chCmd == 'G')
        x--;
      else if (chCmd == 'R' || chCmd == 'E' || chCmd == 'F')
        x++;
      if (chCmd == 'U' || chCmd == 'H' || chCmd == 'E')
        y--;
      else if (chCmd == 'D' || chCmd == 'G' || chCmd == 'F')
        y++;
    }

    /* Transform map coordinates to screen coordinates and draw a line. */

    while (x >= nDegMax)    /* Take care of coordinate wrap around. */
      x -= nDegMax;
    while (x < 0)
      x += nDegMax;
    if (abs(x-xold) > nDegHalf)
      xold = x;

    if (fGlobe) {

      /* For globes, we have to go do a complicated transformation, and not */
      /* draw when we're hidden on the back side of the sphere. We're smart */
      /* and try to only do the slow stuff when we know we'll be visible.   */

      if (fCan) {
        k = x+deg;
        if (k >= nDegMax)
          k -= nDegMax;
        k = (k <= 180);
      }
      if (k && !FGlobeCalc((real)x, (real)y, &u, &v, cx, cy, rx, ry, deg)) {
        if (l)
          DrawLine(m, n, u, v);
        m = u; n = v;
        l = fTrue;
      } else
        l = fFalse;
    } else {

      /* Rectangular maps are much simpler, with screen coordinates      */
      /* proportional to internal coords. For the Mollewide projection   */
      /* we have to apply a factor to the horizontal positioning though. */

      if (gs.fMollewide && gi.nMode != gAstroGraph)
        DrawLine(180*nScl + NMultDiv(xold-180,
          NMollewide(yold-91), 180L), (yold-1)*nScl,
          180*nScl + NMultDiv(x-180, NMollewide(y-91), 180L), (y-1)*nScl);
      else
        DrawLine(xold*nScl, (yold-1)*nScl, x*nScl, (y-1)*nScl);
      xold = x; yold = y;
    }
  }

  /* Draw the outline of the map, either a circle around globes or a */
  /* Mollewide type ellipse for that type of rectangular chart.      */

  DrawColor(gi.kiOn);
  if (!fGlobe) {
    if (gs.fMollewide && gi.nMode != gAstroGraph)
      if (!gs.fAlt)
        for (j = -1; j <= 1; j += 2)
          for (xold = 0, y = 89; y >= 0; y--, xold = x)
            for (x = NMollewide(y), i = -1; i <= 1; i += 2)
              {
              DrawLine(180*nScl + i*xold - (i==1), (90+j*(y+1))*nScl - (j==1),
                180*nScl + i*x - (i==1), (90+j*y)*nScl - (j==1));
              }
  } else
    DrawEllipse(0, 0, gs.xWin-1, gs.yWin-1);

  /* Now, if we are in an appropriate bonus chart mode, draw each planet at */
  /* its zenith or visible location on the globe or map, if not hidden.     */

  if (!gs.fAlt || (gi.nMode != gGlobe &&
    (!fSky || gi.nMode != gWorldMap || gs.fMollewide)))
    return;
  rT = gs.fConstel ? rDegHalf - (fGlobe ? 0.0 : (real)deg) : Lon;
  if (rT < 0.0)
    rT += rDegMax;
  for (i = 1; i <= cObj; i++) {
    planet1[i] = RFromD(Tropical(planet[i]));
    planet2[i] = RFromD(planetalt[i]);
    EclToEqu(&planet1[i], &planet2[i]);    /* Calculate zenith long. & lat. */
  }

  /* Compute screen coordinates of each object, if it's even visible. */

  for (i = 1; i <= cObj; i++) if (FProper(i)) {
    if (fSky)
      x1 = planet1[i];
    else
      x1 = planet1[oMC]-planet1[i];
    if (x1 < 0.0)
      x1 += 2.0*rPi;
    if (x1 > rPi)
      x1 -= 2.0*rPi;
    x1 = Mod(rDegHalf-rT-DFromR(x1));
    y1 = rDegQuad-DFromR(planet2[i]);
    if (fGlobe) {
      X[i] = FGlobeCalc(x1, y1, &u, &v, cx, cy, rx, ry, deg) ? -1000 : u;
      Y[i] = v;
    } else {
      X[i] = (int)(x1 * (real)nScl);
      Y[i] = (int)(y1 * (real)nScl);
    }
    M[i] = X[i]; N[i] = Y[i]+unit/2;
  }

  /* Now that we have the coordinates of each object, figure out where to   */
  /* draw the glyphs. Again we try not to draw glyphs on top of each other. */

  for (i = 1; i <= cObj; i++) if (FProper(i)) {
    k = l = gs.xWin+gs.yWin;

    /* For each planet, we draw the glyph either right over or right under */
    /* the actual zenith location point. So, find out the closest distance */
    /* of any other planet assuming we place ours at both possibilities.   */

    for (j = 1; j < i; j++) if (FProper(j)) {
      k = Min(k, abs(M[i]-M[j])+abs(N[i]-N[j]));
      l = Min(l, abs(M[i]-M[j])+abs(N[i]-unit-N[j]));
    }

    /* Normally, we put the glyph right below the actual point. If however  */
    /* another planet is close enough to have their glyphs overlap, and the */
    /* above location is better, then we'll draw the glyph above instead.   */

    if (k < unit || l < unit)
      if (k < l)
        N[i] -= unit;
  }
  for (i = cObj; i >= 1; i--) if (X[i] >= 0 && FProper(i))      /* Draw the */
    DrawObject(i, M[i], N[i]);                                  /* glyphs.  */
  for (i = cObj; i >= 1; i--) if (X[i] >= 0 && FProper(i)) {
    DrawColor(kObjB[i]);
    DrawSpot(X[i], Y[i]);
  }
}


/* Create a chart in the window based on the current graphics chart mode. */
/* This is the main dispatch routine for all of the program's graphics.   */

void DrawChartX()
{
  byte sz[cchSzDef];
  int i;
  bool fT;

  gi.nScale = gs.nScale/100;

  if (gs.fBitmap || gs.fMeta)
    PrintNotice("Creating graphics chart in memory.");
  DrawClearScreen();
#ifdef CONSTEL
  fT = gs.fConstel;
#else
  fT = fFalse;
#endif
  switch (gi.nMode) {
  case gWheel:
  case gHouse:
    if (us.nRel > rcDual)
      XChartWheel();

    else if (us.nRel == rcTriWheel || us.nRel == rcQuadWheel)
      XChartWheelThreeFour();

    else
      XChartWheelRelation();
    break;
  case gGrid:
    if (us.nRel > rcDual)
      XChartGrid();
    else
      XChartGridRelation();
    break;
  case gHorizon:
    if (us.fPrimeVert)
      XChartHorizonSky();
    else
      XChartHorizon();
    break;
  case gOrbit:
    XChartOrbit();
    break;
  case gSector:
    XChartSector();
    break;
  case gDisposit:
    XChartDispositor();
    break;
  case gAstroGraph:
    DrawMap(fFalse, fFalse, gs.nRot);  /* First draw map of world.           */
    XChartAstroGraph();                /* Then draw astro-graph lines on it. */
    break;
  case gCalendar:
    XChartCalendar();
    break;
  case gEphemeris:
    XChartEphemeris();
    break;
  case gWorldMap:
    DrawMap(fT, fFalse, gs.nRot);           /* First draw map of world. */
    if (!fT && gs.fAlt && !gs.fMollewide)   /* Then maybe Ley lines.    */
      DrawLeyLines(gs.nRot);
    break;
  case gGlobe:
  case gPolar:
    DrawMap(fT, fTrue, gs.nRot);
    break;
#ifdef BIORHYTHM
  case gBiorhythm:
    XChartBiorhythm();
    break;
#endif
  }

  /* Print text showing chart information at bottom of window. */

  DrawColor(gi.kiLite);
  if (fDrawText) {
    if (fNoTimeOrSpace)
      sprintf(sz, "(No time or space)");
    else if (us.nRel == rcComposite)
      sprintf(sz, "(Composite)");
    else {
      fT = us.fAnsiChar;
      us.fAnsiChar = (!gs.fFont || (!gs.fMeta && !gs.fPS)) << 1;
      i = DayOfWeek(Mon, Day, Yea);
      sprintf(sz, "%c%c%c %s %s (%cT %s GMT) %s", chDay3(i),
        SzDate(Mon, Day, Yea, 2), SzTim(Tim), ChDst(Dst),
        SzZone(Zon), SzLocation(Lon, Lat));
      us.fAnsiChar = fT;
    }
    DrawSz(sz, gs.xWin/2, gs.yWin-3*gi.nScaleT, dtBottom | dtErase);
  }

  /* Draw a border around the chart if the mode is set and appropriate. */

  if (fDrawBorder)
    DrawEdgeAll();
}
#endif /* GRAPH */

/* xcharts0.c */
