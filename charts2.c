/*
** Astrolog (Version 5.41G) File: charts2.c
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


/*
******************************************************************************
** Dual Chart Display Routines.
******************************************************************************
*/

/* Print out an aspect (or midpoint if -g0 switch in effect) grid of a      */
/* relationship chart. This is similar to the ChartGrid() routine; however, */
/* here we have both axes labeled with the planets for the two charts in    */
/* question, instead of just a diagonal down the center for only one chart. */

void ChartGridRelation()
{
  byte sz[cchSzDef];
  int i, j, k, tot = cObj, temp, m;
  byte ignoreT[objMax];

  PrintHeader();     /* Display chart info */

#ifdef INTERPRET
  if (us.fInterpret && !us.fGridConfig) {
    InterpretGridRelation();
    return;
  }
#endif

  if (us.nRel == rcTransit) {
    for (m = 0; m <= cObj; m++)
      ignoreT[m] = ignore2[m];
  } else if (us.nRel == rcProgress) {
    for (m = 0; m <= cObj; m++)
      ignoreT[m] = ignore3[m];
  } else {
    for (m = 0; m<= cObj; m++)
      ignoreT[m] = ignore[m];
  }

  PrintSz(" 2>");
  for (temp = 0, i = 0; i <= cObj; i++) if (!ignoreT[i] && i != us.objCenter) {
    PrintCh(chV);
    AnsiColor(kObjA[i]);
    sprintf(sz, "%c%c%c", chObj3(i)); PrintSz(sz);
    AnsiColor(kDefault);
    temp++;
  }
  PrintSz("\n1  ");
  for (i = 0; i <= tot; i++) if (!ignoreT[i] && i != us.objCenter) {
    PrintCh(chV);
    AnsiColor(kSignA(SFromZ(cp2.obj[i])));
    sprintf(sz, "%2d%c", (int)cp2.obj[i] % 30, chDeg0); PrintSz(sz);
    AnsiColor(kDefault);
  }
  PrintSz("\nV  ");
  for (i = 0; i <= tot; i++) if (!ignoreT[i] && i != us.objCenter) {
    PrintCh(chV);
    temp = SFromZ(cp2.obj[i]);
    AnsiColor(kSignA(temp));
    sprintf(sz, "%c%c%c", chSig3(temp)); PrintSz(sz);
    AnsiColor(kDefault);
  }
  PrintL();
  for (j = 0; j <= cObj; j++) if (!ignore[j] && j != us.objCenter)
    for (k = 1; k <= 4; k++) {
      if (k < 2)
        PrintTab(chH, 3);
      else if (k == 2) {
        AnsiColor(kObjA[j]);
        sprintf(sz, "%c%c%c", chObj3(j)); PrintSz(sz);
      } else {
        temp = SFromZ(cp1.obj[j]);
        AnsiColor(kSignA(temp));
        if (k == 3)
          sprintf(sz, "%2d%c", (int)cp1.obj[j] - (temp-1)*30, chDeg0);
        else
          sprintf(sz, "%c%c%c", chSig3(temp));
        PrintSz(sz);
      }
      if (k > 1)
        AnsiColor(kDefault);
      for (i = 0; i <= tot; i++) if (!ignoreT[i] && i != us.objCenter) {
        PrintCh((byte)(k < 2 ? chC : chV));
        temp = grid->n[i][j];
        if (k > 1) {
          if (i == j)
            AnsiColor(kReverse);
          AnsiColor(us.fGridConfig ? kSignA(temp) :
            kAspA[temp]);
        }
        if (k < 2)
          PrintTab(chH, 3);
        else if (k == 2) {
          if (us.fGridConfig)
            sprintf(sz, "%c%c%c", chSig3(temp));
          else
            sprintf(sz, "%s", temp ? szAspectAbbrev[temp] : "   ");
          PrintSz(sz);
        } else if (k == 3) {
          if (us.fGridConfig) {
            sprintf(sz, "%2d%c", grid->v[i][j]/60, chDeg0); PrintSz(sz);
          } else
            if (grid->n[i][j]) {
              if (grid->v[i][j] < 6000)
                sprintf(sz, "%c%2d", us.fAppSep ?
                  (grid->v[i][j] < 0 ? 'a' : 's') :
                  (grid->v[i][j] < 0 ? '-' : '+'), abs(grid->v[i][j])/60);
              else
                sprintf(sz, "%3d", abs(temp)/60);
              PrintSz(sz);
            } else
              PrintSz("   ");
        } else {
          if (grid->n[i][j]) {
            sprintf(sz, "%02d'", abs(grid->v[i][j])%60); PrintSz(sz);
          } else
            PrintSz("   ");
        }
        AnsiColor(kDefault);
      }
      PrintL();
    }
}


/* Display all aspects between objects in the relationship comparison chart, */
/* one per line, in sorted order based on the total "power" of the aspects,  */
/* as specified with the -r0 -a switch combination.                          */

void ChartAspectRelation()
{
  int ca[cAspect + 1], co[objMax];
  byte sz[cchSzDef];
  int pcut = 30000, icut, jcut, phi, ihi, jhi, ahi, p, i, j, k, count = 0, m;
  real ip, jp, rPowSum = 0.0;

  byte ignoreT[objMax];
  real power2[oNorm+1];

  PrintHeader();     /* Display chart info */
  PrintL();

  for (m = 0; m <= cObj; m++)
    ignoreT[m] = FIgnore(m);

  for (i = 0; i <= oNorm; i++)
     power2[i] = rObjInf[i];

  if (us.nRel == rcTransit) {
    for (m = 0; m <= cObj; m++)
      ignoreT[m] = FIgnore2(m);
    for (i = 0; i <= oNorm; i++)
      power2[i] = rTransitInf[i];
  } else {
    if (us.nRel == rcDual)
      for (i = 0; i <= oNorm; i++)
        power2[i] = ppower2[i];
    if (us.nRel == rcProgress) {
      for (m = 0; m <= cObj; m++)
        ignoreT[m] = FIgnore3(m);
      for (i = 0; i<= oNorm; i++)
        power2[i] = ppower1[i];
    }
  }


  ClearB((lpbyte)ca, (cAspect + 1)*(int)sizeof(int));
  ClearB((lpbyte)co, objMax*(int)sizeof(int));
  loop {
    phi = -1;

    /* Search for the next most powerful aspect in the aspect grid. */

    for (i = 0; i <= cObj; i++) if (!ignoreT[i])
      for (j = 0; j <= cObj; j++) if (!FIgnore(j))
        if (k = grid->n[i][j]) {
          ip = i <= oNorm ? power2[i] : 2.5;
          jp = j <= oNorm ? ppower1[j] : 2.5;
          p = (int)(rAspInf[k]*(sqrt)(ip*jp)*
            (1.0-RAbs((real)(grid->v[i][j]))/60.0/GetOrb(i, j, k))*500.0);
          if (us.fParallel)
            p = p / 4;
          if ((p < pcut || (p == pcut && (i > icut ||
            (i == icut && j > jcut)))) && p > phi) {
            ihi = i; jhi = j; phi = p; ahi = k;
          }
        }
    if (phi < 0)    /* Exit when no less powerful aspect found. */
      break;
    pcut = phi; icut = ihi; jcut = jhi;
    count++;                              /* Display the current aspect.   */
#ifdef INTERPRET
    if (us.fInterpret) {                  /* Interpret it if -I in effect. */
      InterpretAspectRelation(jhi, ihi);
      continue;
    }
#endif
    rPowSum += (real)phi/500.0;
    ca[ahi]++;
    co[jhi]++; co[ihi]++;
    sprintf(sz, "%3d: ", count); PrintSz(sz);
    PrintAspect(jhi, SFromZ(cp1.obj[jhi]), (int)RSgn(cp1.dir[jhi]), ahi,
      ihi, SFromZ(cp2.obj[ihi]), (int)RSgn(cp2.dir[ihi]), 'A');
    k = grid->v[ihi][jhi];
    AnsiColor(k < 0 ? kWhite : kLtGray);
    sprintf(sz, "- orb: %c%d,%02d'",
      us.fAppSep ? (k < 0 ? 'a' : 's') : (k < 0 ? '-' : '+'),
      abs(k)/60, abs(k)%60); PrintSz(sz);
    AnsiColor(kDkGreen);
    sprintf(sz, " - power:%6.2f\n", (real)phi/500.0); PrintSz(sz);
    AnsiColor(kDefault);
  }

  PrintAspectSummary(ca, co, count, rPowSum);
}


/* Display locations of all midpoints between objects in the relationship */
/* comparison chart, one per line, in sorted zodiac order from zero Aries */
/* onward, as specified with the -r0 -m switch combination.               */

void ChartMidpointRelation()
{
  int cs[cSign + 1];
  byte sz[cchSzDef];
  int mcut = -1, icut, jcut, mlo, ilo, jlo, m, i, j, count = 0, l;
  long lSpanSum = 0;
  byte ignoreT[objMax];

  if (us.nRel == rcTransit) {
    for (l = 0; l <= cObj; l++)
      ignoreT[l] = FIgnore2(l);
  } else if (us.nRel == rcProgress) {
    for (l = 0; l <= cObj; l++)
      ignoreT[l] = FIgnore3(l);
  } else {
    for (l = 0; l <= cObj; l++)
      ignoreT[l] = FIgnore(l);
  }

  ClearB((lpbyte)cs, (cSign + 1)*(int)sizeof(int));
  loop {
    mlo = 21600;

    /* Search for the next closest midpoint farther down in the zodiac. */

    for (i = 0; i <= cObj; i++) if (!FIgnore(i))
      for (j = 0; j <= cObj; j++) if (!ignoreT[j]) {
        m = (grid->n[j][i]-1)*30*60 + grid->v[j][i];
        if ((m > mcut || (m == mcut && (i > icut ||
          (i == icut && j > jcut)))) && m < mlo) {
          ilo = i; jlo = j; mlo = m;
        }
      }
    if (mlo >= 21600)    /* Exit when no midpoint farther in zodiac found. */
      break;
    mcut = mlo; icut = ilo; jcut = jlo;
    count++;                               /* Display the current midpoint. */
#ifdef INTERPRET
    if (us.fInterpret) {                   /* Interpret it if -I in effect. */
      InterpretMidpointRelation(ilo, jlo);
      continue;
    }
#endif
    cs[mlo/60/30+1]++;
    sprintf(sz, "%4d: ", count); PrintSz(sz);
    PrintZodiac((real)mlo/60.0);
    PrintCh(' ');
    PrintAspect(ilo, SFromZ(cp1.obj[ilo]), (int)RSgn(cp1.dir[ilo]), 0,
      jlo, SFromZ(cp2.obj[jlo]), (int)RSgn(cp2.dir[jlo]), 'M');
    AnsiColor(kDefault);
    m = (int)(MinDistance(cp1.obj[ilo], cp2.obj[jlo])*60.0);
    lSpanSum += m;
    sprintf(sz, "-%4d%c%02d' degree span.\n", m/60, chDeg1, m%60);
    PrintSz(sz);
  }

  PrintMidpointSummary(cs, count, lSpanSum);
}


/* Calculate any of the various kinds of relationship charts. This involves */
/* computing and storing the planet and house positions for the "core" and  */
/* "second" charts, and then combining them in the main single chart in the */
/* proper manner, e.g. for synastry, composite, time space midpoint charts. */

void CastRelation()
{
  byte ignoreT[objMax];
  int i, saveRev;
  real ratio, t1, t2, t;
  real ppowerT[oNorm+1];
  CI ciT;

#ifdef WIN
  if (MM == -1) {
    cp3 = cp0;
    fCP3 = 1;
  }
#endif

  /* Cast the first chart. */

  ciMain = ciCore;
  t1 = CastChart(fTrue);
  cp1 = cp0;
  saveRev = hRevers;
  if(us.nRel == rcTransit || us.nRel == rcProgress)
    for (i = 1; i <= cObj; i++) {
      cp1.dir[i] = 0.0;
      if (i > oNorm)
        continue;
      cp1.altdir[i] = 0.0;
    }

#ifdef GRAPH              /* Struct GS is defined with gaphics.  */
  if (!gs.nAnim) {
#endif
    if (!FCreateGrid(fFalse))
      return;
    PlanetPPower();
#ifdef GRAPH
  }
#endif  

  /* Cast the second chart. */

  ciCore = ciTwin;
  if (us.nRel == rcTransit) {
    for (i = 0; i <= cObj; i++) {
      ignoreT[i] = ignore[i];
      if (us.fSector || us.fAstroGraph)
        ignore[i] = ignore[i] && ignore2[i];
      else
        ignore[i] = ignore2[i];
    }
  } else if (us.nRel == rcProgress) {
    us.fProgress = fTrue;
    is.JDp = MdytszToJulian(MM, DD, YY, TT, SS, ZZ);
    ciCore = ciMain;
    for (i = 0; i <= cObj; i++) {
      ignoreT[i] = ignore[i];
      if (us.fSector || us.fAstroGraph)
        ignore[i] = ignore[i] && ignore3[i];
      else
        ignore[i] = ignore3[i];
    }
  }
  t2 = CastChart(fTrue);
  if (us.nRel == rcTransit) {
    for (i = 0; i <= cObj; i++) {
      ignore2[i] = ignore[i];
      ignore[i] = ignoreT[i];
    }
  } else if (us.nRel == rcProgress) {
    us.fProgress = fFalse;
    for (i = 0; i <= cObj; i++) {
      ignore3[i] = ignore[i];
      ignore[i] = ignoreT[i];
    }
  }
  cp2 = cp0;

  if (us.nRel == rcDual) {
    if (!FCreateGrid(fFalse))
      return;
    for (i = 0; i <= oNorm; i++)
      ppowerT[i] = ppower1[i];
    PlanetPPower();
    for (i = 0; i <= oNorm; i++) {
      ppower2[i] = ppower1[i];
      ppower1[i] = ppowerT[i];
    }
  }

  hRevers = saveRev;
  ciCore = ciMain;

  /* Now combine the two charts based on what relation we are doing.   */
  /* For the standard -r synastry chart, use the house cusps of chart1 */
  /* and the planets positions of chart2.                              */

  ratio = (real)us.nRatio1 / ((real)(us.nRatio1 + us.nRatio2));
  if (us.nRel <= rcSynastry) {
    for (i = 1; i <= cSign; i++)
      chouse[i] = cp1.cusp[i];

  /* For the -rc composite chart, take the midpoints of the planets/houses. */

  } else if (us.nRel == rcComposite) {
    for (i = 0; i <= cObj; i++) {
      planet[i] = Ratio(cp1.obj[i], cp2.obj[i], ratio);
      if (RAbs(cp2.obj[i] - cp1.obj[i]) > rDegHalf)
        planet[i] = Mod(planet[i] + rDegMax*ratio);
      planetalt[i] = Ratio(cp1.alt[i], cp2.alt[i], ratio);
      ret[i] = Ratio(cp1.dir[i], cp2.dir[i], ratio);
    }
    for (i = 1; i <= cSign; i++) {
      chouse[i] = Ratio(cp1.cusp[i], cp2.cusp[i], ratio);
      if (RAbs(cp2.cusp[i] - cp1.cusp[i]) > rDegHalf)
        chouse[i] = Mod(chouse[i] + rDegMax*ratio);
    }

    /* Make sure we don't have any 180 degree errors in house cusp    */
    /* complement pairs, which may happen if the cusps are far apart. */

    for (i = 1; i <= cSign; i++)
      if (MinDistance(chouse[sCap], Mod(chouse[i]-ZFromS(i+3))) > rDegQuad)
        chouse[i] = Mod(chouse[i]+rDegHalf);
    for (i = 1; i <= cSign; i++)
      if (RAbs(MinDistance(chouse[i], planet[oAsc - 1 + i])) > rDegQuad)
        planet[oAsc - 1 + i] = Mod(planet[oAsc - 1 + i]+rDegHalf);

  /* For the -rm time space midpoint chart, calculate the midpoint time and */
  /* place between the two charts and then recast for the new chart info.   */

  } else if (us.nRel == rcMidpoint) {
    ciT = ciCore;
    is.T = Ratio(t1, t2, ratio);
    t = (is.T*36525.0)+rRound; is.JD = RFloor(t)+2415020.0;
    TT = RFract(t)*24.0;
    ZZ = Ratio(DecToDeg(Zon), DecToDeg(ciTwin.zon), ratio);
    SS = Ratio(DecToDeg(Dst), DecToDeg(ciTwin.dst), ratio);
    TT -= ZZ - SS;
    if (TT < 0.0) {
      TT += 24.0; is.JD -= 1.0;
    }
    JulianToMdy(is.JD, &MM, &DD, &YY);
    OO = Ratio(DecToDeg(Lon), DecToDeg(ciTwin.lon), ratio);
    if (RAbs(ciTwin.lon-Lon) > rDegHalf)
      OO = Mod(OO+rDegMax*ratio);
    AA = Ratio(DecToDeg(Lat), DecToDeg(ciTwin.lat), ratio);
    TT = DegToDec(TT); SS = DegToDec(SS); ZZ = DegToDec(ZZ);
    OO = DegToDec(OO); AA = DegToDec(AA);
    ciMain = ciCore;
    CastChart(fTrue);
    ciCore = ciT;

  /* There are a couple of non-astrological charts, which only require the */
  /* number of days that have passed between the two charts to be done.    */

  } else
    is.JD = RAbs(t2-t1)*36525.0;
  ComputeInHouses();

#ifdef WIN
  if (fCP3) {
    cp0 = cp3;
    fCP3 = 0;
  }
#endif

}


/*
******************************************************************************
** Other Chart Display Routines.
******************************************************************************
*/

/* Given two objects and an aspect between them, or an object and a sign  */
/* that it's entering, print if this is a "major" event, such as a season */
/* change or major lunar phase. This is called from the ChartInDay()      */
/* searching and influence routines. Do an interpretation if need be too. */

void PrintInDay(source, aspect, dest)
int source, aspect, dest;
{
  if (aspect == aSig) {
    if (source == oSun) {
      AnsiColor(kWhite);
      if (dest == 1)
        PrintSz(" (Vernal Equinox)");     /* If the Sun changes sign, */
      else if (dest == 4)                 /* then print out if this   */
        PrintSz(" (Summer Solstice)");    /* is a season change.      */
      else if (dest == 7)
        PrintSz(" (Autumnal Equinox)");
      else if (dest == 10)
        PrintSz(" (Winter Solstice)");
    }
  } else if ((aspect > 0) && !us.fParallel) {
    if (source == oSun && dest == oMoo) {
      if (aspect <= aSqu)
        AnsiColor(kWhite);
      if (aspect == aCon)
        PrintSz(" (New Moon)");     /* Print out if the present */
      else if (aspect == aOpp)      /* aspect is a New, Full,   */
        PrintSz(" (Full Moon)");    /* or Half Moon.            */
      else if (aspect == aSqu)
        PrintSz(" (Half Moon)");
    }
  }
  PrintL();

#ifdef INTERPRET
  if (us.fInterpret)
    InterpretInDay(source, aspect, dest);
#endif
  AnsiColor(kDefault);
}


/* Given two objects and an aspect (or one object, and an event such as a */
/* sign or direction change) display the configuration in question. This  */
/* is called by the many charts which list aspects among items, such as   */
/* the -a aspect lists, -m midpoint lists, -d aspect in day search and    */
/* influence charts, and -t transit search and influence charts.          */

void PrintAspect(obj1, sign1, ret1, asp, obj2, sign2, ret2, chart)
int obj1, sign1, ret1, asp, obj2, sign2, ret2;
byte chart;
{
  byte sz[cchSzDef];

  AnsiColor(kObjA[obj1]);
  if (chart == 't' || chart == 'T')
    PrintSz("trans ");
  else if (chart == 'e' || chart == 'u' || chart == 'U')
    PrintSz("progr ");
  sprintf(sz, "%7.7s", szObjName[obj1]); PrintSz(sz);
  AnsiColor(kSignA(sign1));
  sprintf(sz, " %c%c%c%c%c",
    ret1 > 0 ? '(' : (ret1 < 0 ? '[' : '<'), chSig3(sign1),
    ret1 > 0 ? ')' : (ret1 < 0 ? ']' : '>')); PrintSz(sz);
  AnsiColor(asp > 0 ? kAspA[asp] : kWhite);
  PrintCh(' ');
  if (asp == aSig)
    sprintf(sz, "-->");                        /* Print a sign change. */
  else if (asp == aDir)
    sprintf(sz, "S/%c", obj2 ? chRet : 'D');   /* Print a direction change. */
  else if (asp == 0)
    sprintf(sz, chart == 'm' ? "&" : "with");
  else
    sprintf(sz, "%s", szAspectAbbrev[asp]);    /* Print an aspect. */
  PrintSz(sz);
  if (asp != aDir)
    PrintCh(' ');
  if (chart == 'A')
    PrintSz("with ");
  if (asp == aSig) {
    AnsiColor(kSignA(obj2));
    sprintf(sz, "%s", szSignName[obj2]); PrintSz(sz);
  } else if (asp >= 0) {
    AnsiColor(kSignA(sign2));
    if (chart == 't' || chart == 'u' || chart == 'T' || chart == 'U')
      PrintSz("natal ");
    sprintf(sz, "%c%c%c%c%c ",
      ret2 > 0 ? '(' : (ret2 < 0 ? '[' : '<'), chSig3(sign2),
      ret2 > 0 ? ')' : (ret2 < 0 ? ']' : '>')); PrintSz(sz);
    AnsiColor(kObjA[obj2]);
    sprintf(sz, "%.10s", szObjName[obj2]); PrintSz(sz);
  }
  if (chart == 'D' || chart == 'T' || chart == 'U' ||
    chart == 'a' || chart == 'A' || chart == 'm' || chart == 'M')
    PrintTab(' ', 10-CchSz(szObjName[obj2]));
}


/* Based on the given chart information, display all the aspects taking   */
/* place in the chart, as specified with the -D switch. The aspects are   */
/* printed in order of influence determined by treating them as happening */
/* outside among transiting planets, such that rare outer planet aspects  */
/* are given more power than common ones among inner planets. (This is    */
/* almost identical to the -a list, except the influences are different.) */

void ChartInDayInfluence()
{
  int source[MAXINDAY], aspect[MAXINDAY], dest[MAXINDAY];
  real power[MAXINDAY];
  byte sz[cchSzDef];
  int occurcount = 0, i, j, k, l, m;

  /* Go compute the aspects in the chart. */

  i = us.fAppSep;
  us.fAppSep = fTrue;     /* We always want applying vs. separating orbs. */
  FCreateGrid(fFalse);
  us.fAppSep = i;

  /* Search through the grid and build up the list of aspects. */

  for (j = 1; j <= cObj; j++) {
    if (FIgnore(j))
      continue;
    for (i = 0; i < j; i++) {
      if (FIgnore(i) || (k = grid->n[i][j]) == 0 || occurcount >= MAXINDAY)
        continue;
      source[occurcount] = i; aspect[occurcount] = k; dest[occurcount] = j;
      l = grid->v[i][j];
      power[occurcount] =
        (sqrt)((i <= oNorm ? rTransitInf[i] : 2.5)*
               (j <= oNorm ? rTransitInf[j] : 2.5))*
        rAspInf[k]*(1.0-(real)abs(l)/60.0/GetOrb(i, j, k));
      occurcount++;
    }
  }

  /* Sort aspects by order of influence. */

  for (i = 1; i < occurcount; i++) {
    j = i-1;
    while (j >= 0 && power[j] < power[j+1]) {
      SwapN(source[j], source[j+1]);
      SwapN(aspect[j], aspect[j+1]);
      SwapN(dest[j], dest[j+1]);
      SwapR(&power[j], &power[j+1]);
      j--;
    }
  }

  /* Now display each aspect line. */

  for (i = 0; i < occurcount; i++) {
    sprintf(sz, "%3d: ", i+1); PrintSz(sz);
    j = source[i]; k = aspect[i]; l = dest[i];
    PrintAspect(
      j, SFromZ(planet[j]), (int)RSgn(ret[j]), k,
      l, SFromZ(planet[l]), (int)RSgn(ret[l]), 'D');
    m = grid->v[j][l];
    AnsiColor(m < 0 ? kWhite : kLtGray);
    sprintf(sz, "- %s%2d%c%02d'", m < 0 ? "app" : "sep",
      abs(m)/60, chDeg1, abs(m)%60); PrintSz(sz);
    AnsiColor(kDkGreen);
    sprintf(sz, " - power:%6.2f", power[i]); PrintSz(sz);
    PrintInDay(j, k, l);
  }
  if (occurcount == 0)
    PrintSz("Empty transit aspect list.\n");
}


/* Given an arbitrary day, determine what aspects are made between this */
/* transiting chart and the given natal chart, as specified with the -T */
/* switch, and display the transits in order sorted by influence.       */

void ChartTransitInfluence(fProg)
bool fProg;
{
  int source[MAXINDAY], aspect[MAXINDAY], dest[MAXINDAY];
  real power[MAXINDAY];
  byte ignoreT[objMax];
  byte sz[cchSzDef];
  int occurcount = 0, fProgress = us.fProgress, i, j, k, l, m;

  PrintHeader();     /* Display chart info */

  PlanetPPower();

  /* Cast the natal and transiting charts as with a relationship chart. */

#ifdef WIN
  if (MM == -1) {
    cp3 = cp0;
    fCP3 = 1;
  }
#endif

  cp1 = cp0;
  for (i = 0; i <= cObj; i++) {
    ignoreT[i] = ignore[i]; ignore[i] = ignore2[i];
  }
/*  SetCI(ciCore, ciTran.mon, ciTran.day, ciTran.yea, Tim,
    Dst, Zon, Lon, Lat);  */
  ciCore = ciTran;
  if (us.fProgress = fProg) {
    is.JDp = MdytszToJulian(MM, DD, YY, TT, SS, ZZ);
    ciCore = ciMain;
  }
  CastChart(fTrue);
  cp2 = cp0;
  for (i = 0; i <= cObj; i++) {
    ignore[i] = ignoreT[i];
  }

  /* Do a relationship aspect grid to get the transits. We have to make and */
  /* restore three changes to get it right for this chart. (1) We make the  */
  /* natal planets have zero velocity so applying vs. separating is only a  */
  /* function of the transiter. (2) We force applying vs. separating orbs   */
  /* regardless if -ga or -ma is in effect or not. (3) Finally we tweak the */
  /* main restrictions to allow for transiting objects not restricted.      */

  for (i = 0; i <= cObj; i++) {
    ret[i] = cp1.dir[i];
    cp1.dir[i] = 0.0;
    ignoreT[i] = ignore[i];
    ignore[i] = ignore[i] && ignore2[i];
  }
  i = us.fAppSep; us.fAppSep = fTrue;
  FCreateGridRelation(fFalse);
  us.fAppSep = i;
  for (i = 0; i <= cObj; i++) {
    cp1.dir[i] = ret[i];
    ignore[i] = ignoreT[i];
  }

  /* Loop through the grid, and build up a list of the valid transits. */

  for (i = 0; i <= oNorm; i++) {
    if (FIgnore2(i))
      continue;
    for (j = 0; j <= cObj; j++) {
      if (FIgnore(j) || (is.fReturn && i != j) || (k = grid->n[i][j]) == 0 ||
        occurcount >= MAXINDAY)
        continue;
      source[occurcount] = i; aspect[occurcount] = k; dest[occurcount] = j;
      l = grid->v[i][j];
      power[occurcount] = rAspInf[k]*
        (sqrt)((us.fProgress ? ppower1[i] : rTransitInf[i])*
        (j <= oNorm ? ppower1[j] : 2.5))*
        (1.0-(real)abs(l)/60.0/GetOrb(i, j, k));
      occurcount++;
    }
  }

  /* After all transits located, sort them by their total power. */

  for (i = 1; i < occurcount; i++) {
    j = i-1;
    while (j >= 0 && power[j] < power[j+1]) {
      SwapN(source[j], source[j+1]);
      SwapN(aspect[j], aspect[j+1]);
      SwapN(dest[j], dest[j+1]);
      SwapR(&power[j], &power[j+1]);
      j--;
    }
  }

  /* Now loop through list and display each transit in effect at the time. */

  for (i = 0; i < occurcount; i++) {
    k = aspect[i];
    l = source[i];
    sprintf(sz, "%3d: ", i+1); PrintSz(sz);
    j = SFromZ(cp2.obj[l]);
    PrintAspect(l, j, (int)RSgn(cp2.dir[l]), k,
      dest[i], SFromZ(cp1.obj[dest[i]]), (int)RSgn(cp1.dir[dest[i]]),
      (byte)(fProg ? 'U' : 'T'));
    m = grid->v[l][dest[i]];
    AnsiColor(m < 0 ? kWhite : kLtGray);
    sprintf(sz, "- %s%2d%c%02d'", m < 0 ? "app" : "sep",
      abs(m)/60, chDeg1, abs(m)%60); PrintSz(sz);
    AnsiColor(kDkGreen);
    sprintf(sz, " - power:%6.2f", power[i]); PrintSz(sz);
    if (k == aCon && l == dest[i]) {    /* Print a small "R" for returns. */
      AnsiColor(kWhite);
      PrintSz(" R");
    }
    PrintL();
#ifdef INTERPRET
    if (us.fInterpret)
      InterpretTransit(l, k, dest[i]);
#endif
    AnsiColor(kDefault);
  }
  if (occurcount == 0)
    PrintSz("Empty transit list.\n");

#ifdef WIN
  if (fCP3) {
    cp0 = cp3;
    fCP3 = 0;
  }
#endif

  us.fProgress = fProgress;
  ciCore = ciMain;
  CastChart(fTrue);
}


/* Given the zodiac location of a planet in the sky and its declination,   */
/* and a location on the Earth, compute the azimuth and altitude of where  */
/* on the local horizon sky the planet would appear to one at the given    */
/* location. A reference MC position at Greenwich is also needed for this. */

void EclToHorizon(azi, alt, obj, objalt, lon, lat, mc)
real *azi, *alt, obj, objalt, lon, lat, mc;
{
  real lonz, latz;

  lonz = RFromD(obj); latz = RFromD(objalt);
  EclToEqu(&lonz, &latz);
  lonz = RFromD(Mod(DFromR(mc-lonz+lon)));
  lonz = RFromD(Mod(DFromR(lonz-lon+rPiHalf)));
  EquToLocal(&lonz, &latz, rPiHalf-lat);
  *azi = rDegMax-DFromR(lonz); *alt = DFromR(latz);
}


/* Display a calendar for the given month in the chart, as specified with  */
/* with the -K switch. When color is on, the title is white, weekends are  */
/* highlighted in red, and the specific day in the chart is colored green. */

void ChartCalendarMonth()
{
  byte sz[cchSzDef];
  int i, j, k;

  AnsiColor(kWhite);
  PrintTab(' ', 16-CchSz(szMonth[Mon]) >> 1);
  sprintf(sz, "%s%5d\n", szMonth[Mon], Yea); PrintSz(sz);
  for (i = 0; i < cWeek; i++) {
    sprintf(sz, "%c%c%c", szDay[i][0], szDay[i][1], i < cWeek-1 ? ' ' : '\n');
    PrintSz(sz);
  }
  j = DayOfWeek(Mon, 1, Yea);
  AnsiColor(kDefault);
  for (i = 0; i < j; i++) {
    if (i == 0)
      AnsiColor(kRainbowA[1]);
    PrintSz("-- ");
    if (i == 0)
      AnsiColor(kDefault);
  }
  k = DayInMonth(Mon, Yea);
  for (i = 1; i <= k; i = AddDay(Mon, i, Yea, 1)) {
    if (i == (int)Day)
      AnsiColor(kRainbowA[4]);
    else if (j == 0 || j == cWeek-1)
      AnsiColor(kRainbowA[1]);
    sprintf(sz, "%2d", i); PrintSz(sz);
    if (j == 0 || j == cWeek-1 || i == Day)
      AnsiColor(kDefault);
    if (j < cWeek-1) {
      j++;
      PrintCh(' ');
    } else {
      j = 0;
      PrintL();
    }
  }
  while (j > 0 && j < cWeek) {
    if (j == cWeek-1)
      AnsiColor(kRainbowA[1]);
    j++;
    sprintf(sz, "--%c", j < cWeek ? ' ' : '\n'); PrintSz(sz);
  }
  AnsiColor(kDefault);
}


/* Display a calendar for the entire year given in the chart, as specified */
/* with the -Ky switch. This is just like twelve of the individual month   */
/* calendars above displayed together, with same color highlights and all. */

void ChartCalendarYear()
{
  byte sz[cchSzDef];
  int r, w, c, m, d, dy, p[3], l[3], n[3];

  dy = DayOfWeek(1, 1, Yea);
  for (r = 0; r < 4; r++) {     /* Loop over one set of three months */
    AnsiColor(kWhite);
    for (c = 0; c < 3; c++) {
      m = r*3+c+1;
      PrintTab(' ', 16-CchSz(szMonth[m]) >> 1);
      sprintf(sz, "%s%5d", szMonth[m], Yea); PrintSz(sz);
      if (c < 2)
        PrintTab(' ', 20 + MONTHSPACE -
          (16-CchSz(szMonth[m]) >> 1) - CchSz(szMonth[m]) - 5);
    }
    PrintL();
    for (c = 0; c < 3; c++) {
      for (d = 0; d < cWeek; d++) {
        sprintf(sz, "%c%c%c", szDay[d][0], szDay[d][1],
          d < cWeek-1 || c < 2 ? ' ' : '\n'); PrintSz(sz);
      }
      if (c < 2)
        PrintTab(' ', MONTHSPACE-1);
      m = r*3+c+1;
      p[c] = dy % cWeek;
      l[c] = DayInMonth(m, Yea);
      n[c] = 0;
      dy += DaysInMonth(m, Yea);
    }
    for (w = 0; w < cWeek-1; w++) {    /* Loop over one set of week rows */
      for (c = 0; c < 3; c++) {        /* Loop over one week in a month  */
        m = r*3+c+1;
        d = 0;
        if (w == 0)
          while (d < p[c]) {
            if (d == 0)
              AnsiColor(kRainbowA[1]);
            PrintSz("-- ");
            if (d == 0)
              AnsiColor(kDefault);
            d++;
          }
        AnsiColor(kDefault);
        while (d < cWeek && n[c] < l[c]) {
          n[c] = AddDay(m, n[c], Yea, 1);
          if (n[c] == Day && m == Mon)
            AnsiColor(kRainbowA[4]);
          else if (d == 0 || d == cWeek-1)
            AnsiColor(kRainbowA[1]);
          sprintf(sz, "%2d%c", n[c], d < cWeek-1 || c < 2 ? ' ' : '\n');
          PrintSz(sz);
          if (d == 0 || d == cWeek-1 || (n[c] == Day && m == Mon))
            AnsiColor(kDefault);
          d++;
        }
        while (d < cWeek) {
          if (d == 0 || d == cWeek-1)
            AnsiColor(kRainbowA[1]);
          sprintf(sz, "--%c", d < cWeek-1 || c < 2 ? ' ' : '\n'); PrintSz(sz);
          if (d == 0)
            AnsiColor(kDefault);
          d++;
        }
        if (c < 2)
          PrintTab(' ', MONTHSPACE-1);
      }
    }
    if (r < 3)
      PrintL();
  }
  AnsiColor(kDefault);
}


/* Display either a biorhythm chart or the time difference in various units */
/* between two charts, i.e. two types of relationship "charts" that aren't  */
/* related in any way to planetary positions, as specified by either the    */
/* -rb or -rd switches, respectively.                                       */

void DisplayRelation()
{
  byte sz[cchSzDef];
  int i;
#ifdef BIORHYTHM
  int j;
  real k, l;
#endif

  /* If we are calculating the difference between two dates, then display */
  /* the value and return, as with the -rd switch.                        */

  if (us.nRel == rcDifference) {
    PrintSz("Differences between the dates in the two charts:\n");
    for (i = 1; i <= 7; i++) {
      AnsiColor(kRainbowA[i]);
      switch (i) {
      case 1: sprintf(sz, "Years  : %.0f", is.JD/365.25);      break;
      case 2: sprintf(sz, "Months : %.0f", is.JD/(365.25/12)); break;
      case 3: sprintf(sz, "Weeks  : %.0f", is.JD/7.0);         break;
      case 4: sprintf(sz, "Days   : %.0f", is.JD);             break;
      case 5: sprintf(sz, "Hours  : %.0f", is.JD*24.0);        break;
      case 6: sprintf(sz, "Minutes: %.0f", is.JD*24.0*60.0);   break;
      case 7: sprintf(sz, "Seconds: %.0f", is.JD*24.0*3600.0); break;
      }
      PrintSz(sz);
      PrintL();
    }
    AnsiColor(kDefault);
    return;
  }

#ifdef BIORHYTHM
  /* If we are doing a biorhythm (-rb switch), then we'll calculate it for */
  /* someone born on the older date, at the time of the younger date. Loop */
  /* through the week preceeding and following the date in question.       */

  is.JD = RFloor(is.JD + rRound);
  for (is.JD -= (real)(us.nBioday/2), i = -us.nBioday/2; i <= us.nBioday/2;
    i++, is.JD += 1.0) {
    if (i == 0)
      AnsiColor(kWhite);
    else if (i == 1)
      AnsiColor(kDefault);
    j = abs(i);
    sprintf(sz, "T%c%d%sDay%c:", i < 0 ? '-' : '+', j,
      j < 10 ? " " : "", j != 1 ? 's' : ' '); PrintSz(sz);
    for (j = 1; j <= 3; j++) {
      PrintCh(' ');
      switch (j) {
      case 1: k = brPhy; AnsiColor(kRed);   PrintSz("Physical");     break;
      case 2: k = brEmo; AnsiColor(kBlue);  PrintSz("Emotional");    break;
      case 3: k = brInt; AnsiColor(kGreen); PrintSz("Intellectual"); break;
      }
      AnsiColor(i ? kDefault : kWhite);

      /* The biorhythm calculation is below. */

      l = RBiorhythm(is.JD, k);
      sprintf(sz, " at %c%3.0f%%", l < 0.0 ? '-' : '+', RAbs(l)); PrintSz(sz);

      /* Print smiley face, medium face, or sad face based on current cycle. */

      AnsiColor(kPurple);
      sprintf(sz, " :%c", l > 50.0 ? ')' : (l < -50.0 ? '(' : '|'));
      PrintSz(sz);
      AnsiColor(i ? kDefault : kWhite);
      if (j < 3)
        PrintCh(',');
    }
    PrintL();
  }
#endif /* BIORHYTHM */
}

/* charts2.c */
