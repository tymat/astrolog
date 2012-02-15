/*
** Astrolog (Version 5.41F) File: xcharts1.c
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

#include "astrolog.h"

#ifdef GRAPH

void DrawCalendar P((int, int, int, int, int));

/*
******************************************************************************
** Single Chart Graphics Routines.
******************************************************************************
*/

/* Draw a wheel chart, in which the 12 signs and houses are delineated, and  */
/* the planets are inserted in their proper places. This is the default      */
/* graphics chart to generate, as is done when the -v or -w (or no) switches */
/* are included with -X. Draw the aspects in the middle of chart, too.       */

void XChartWheel()
{
  real xsign[cSign+1], xhouse[cSign+1], xplanet[objMax], symbol[objMax];
  int cx, cy, i, j, k, first, n;
  real unitx, unity, argum, argum1, value, valt, l, m, p, ip, jp;
  real power1[oNorm+1];
  real ccc=2.0;

  /* Set up variables and temporarily automatically decrease the horizontal */
  /* chart size to leave room for the sidebar if that mode is in effect.    */

  if (gs.fText && !us.fVelocity)
    gs.xWin -= xSideT;
  cx = gs.xWin/2 - 1; cy = gs.yWin/2 - 1;
  unitx = (real)cx; unity = (real)cy;
  gi.rAsc = gs.objLeft ? planet[abs(gs.objLeft)]+rDegQuad*(gs.objLeft < 0) :
    chouse[1];
  if (us.fVedic)
    gi.rAsc = gs.objLeft ? (gs.objLeft < 0 ? 120.0 : -60.0)-gi.rAsc : 0.0;

  if (hRevers == 2)
    gi.rAsc = chouse[7];

  /* Fill out arrays with the angular degree on the circle of where to */
  /* place each object, cusp, and sign glyph based on the chart mode.  */

  if (gi.nMode == gWheel) {
    for (i = 1; i <= cSign; i++)
      xhouse[i] = PZ(chouse[i]);
  } else {
    gi.rAsc -= chouse[1];
    for (i = 1; i <= cSign; i++)
      xhouse[i] = PZ(ZFromS(i));
  }
  for (i = 1; i <= cSign; i++)
    xsign[i] = PZ(HousePlaceInX(ZFromS(i)));
  for (i = 0; i <= cObj; i++)
    xplanet[i] = PZ(HousePlaceInX(planet[i]));

  /* Go draw the outer sign and house rings. */

  DrawWheel(xsign, xhouse, cx, cy, unitx, unity, gi.rAsc,
    0.65, 0.70, 0.75, 0.80, 0.875);

  for (i = 0; i <= cObj; i++)    /* Figure out where to put planet glyphs. */
    symbol[i] = xplanet[i];
  FillSymbolRing(symbol, 1.0);

  /* For each planet, draw a small dot indicating where it is, and then */
  /* a line from that point to the planet's glyph.                      */

  DrawSymbolRing(symbol, xplanet, ret, cx, cy, unitx, unity,
    0.50, 0.52, 0.56, 0.60);

  if(!FCreateGrid(fFalse))
     return;
  for (i = 0; i <= oNorm; i++)
    power1[i] = rObjInf[i];
  if(us.nRel <= 0) {
     PlanetPPower();
     for (i = 0; i <= oNorm; i++)
       power1[i] = ppower1[i];
   }
   STension = SHarmon = SConjunct = SSumma = SChart = 0.0;

/* Added by V.A. Additional graphics output - tension/harmony chart. */

  if (TensChart != 0) {
    DrawColor(kWhite);
    for (i = 0; i < 180; i++) {
      value = 0.25;
      argum = (real)(i)*2.0;
      argum = PZ(HousePlaceInX(argum));
      DrawPoint(cx+POINT1(unitx, value, PX(argum)),
        cy+POINT1(unity, value, PY(argum)));
    }
    DrawColor(kWhite);
    for (i = 0; i < 3600; i++) {
      value = 0.0;
      if (TensChart <= 1)
        first = aOpp;
      else
        first = aCon;

      argum = (real)(i)/10.0;
      argum1 = PZ(HousePlaceInX(argum));
      for (j = 1; j <= cObj; j++) if (!ignore[j]) {
        l = MinDistance(argum, planet[j]);
        for (k = first; k <= us.nAsp; k++) {
          m = RAbs(l - rAspAngle[k]);
          if (m < GetOrb(1,j,k)){
             if (FCusp(j) && (k!=1 || k!=4))
                continue;
             valt = (sqrt)(20.0*ppower1[j])*rAspInf[k]*
                    (1.0 - m/GetOrb(1,j,k)) ;
             if ((k==aOpp || k==aSqu || k==aSSq || k==aSes ) && (TensChart <= 1))
                valt = - valt;
             value = value + CoeffTens*valt/120.0;
          }
        } 
      }
        
      if (TensChart == 1)
         value = 0.25 + value;
      if (TensChart == -1)
         value = 0.25 - value;   
      if (value < 0.0)
          value = 0.0;
      if (value > 0.6)
          value = 0.6;

      DrawPoint(cx+POINT1(unitx, value, PX(argum1)),
        cy+POINT1(unity, value, PY(argum1))); 
    }
  } else {

  /* End V.A. addition */


  /* Draw lines connecting planets which have aspects between them. */
  /* Now in bonus mode dashenness of aspect lines depends of absolute */
  /* power of aspect, not only of orb. V.A. */

    for (j = cObj; j >= 1; j--)
      for (i = j-1; i >= 0; i--)
        if (grid->n[i][j] && FProper(i) && FProper(j)) {
          k = grid->n[i][j];
          ip = i <= oNorm ? power1[i] : 2.5;
          jp = j <= oNorm ? power1[j] : 2.5;
          p = (rAspInf[k]*(sqrt)(ip*jp)*
            (1.0-RAbs((real)(grid->v[i][j]))/60.0/GetOrb(i, j, k)));
          if (us.fParallel)
            p = p * PowerPar;

        DrawColor(kAspB[grid->n[i][j]]);
        if (!gs.fAlt) {
           DrawDash(cx+POINT1(unitx, 0.48, PX(xplanet[i])),
             cy+POINT1(unity, 0.48, PY(xplanet[i])),
             cx+POINT1(unitx, 0.48, PX(xplanet[j])),
             cy+POINT1(unity, 0.48, PY(xplanet[j])),
          (int)(RAbs((real)(grid->v[i][j]))/(GetOrb(i,j,grid->n[i][j]))/15.0));
        } else {
           if (p/SolidAspect <= 0.05)
              n = 20;
           else
              n = (int)(SolidAspect/p);
           DrawDash(cx+POINT1(unitx, 0.48, PX(xplanet[i])),
             cy+POINT1(unity, 0.48, PY(xplanet[i])),
             cx+POINT1(unitx, 0.48, PX(xplanet[j])),
             cy+POINT1(unity, 0.48, PY(xplanet[j])),
             n);
        }
      }
  }
  for (j = cObj; j >= 1; j--)
    for (i = j-1; i >= 0; i--)
      if (grid->n[i][j] && FProper(i) && FProper(j)) {
        k = grid->n[i][j];
        ip = i <= oNorm ? power1[i] : 2.5;
        jp = j <= oNorm ? power1[j] : 2.5;
        p = (rAspInf[k]*(sqrt)(ip*jp)*
          (1.0-RAbs((real)(grid->v[i][j]))/60.0/GetOrb(i, j, k)));
        if (us.fParallel)
          p = p * PowerPar ;
        if (FCusp(i) && FCusp(j))
           continue;
        if (k == aOpp || k == aSqu || k == aSSq || k == aSes)
           STension -= ccc*p;
        if (k == aSex || k == aTri || k == aSSx || k == aInc)
           SHarmon += ccc*p;
        if (k == aCon)
           SConjunct += ccc*p;
        SSumma += ccc*p;
      }
      SChart = SHarmon + STension;

  /* Go draw sidebar with chart information and positions if need be. */

  DrawInfo();
}


/* Draw an astro-graph chart on a map of the world, i.e. the draw the     */
/* Ascendant, Descendant, Midheaven, and Nadir lines corresponding to the */
/* time in the chart. This chart is done when the -L switch is combined   */
/* with the -X switch.                                                    */

void XChartAstroGraph()
{
  real planet1[objMax], planet2[objMax],
    end1[cObj*2+1], end2[cObj*2+1],
    symbol1[cObj*2+1], symbol2[cObj*2+1],
    lon, longm, x, y, z, ad, oa, am, od, dm, lat;
  int unit = gi.nScale, fStroke, lat1 = -60, lat2 = 75, y1, y2, xold1, xold2,
    i, j, k, l;

  lon = RealCoord(Lon);

  /* Erase top and bottom parts of map. We don't draw the astro-graph lines */
  /* above certain latitudes, and this gives us room for glyph labels, too. */

  y1 = (91-lat1)*gi.nScale;
  y2 = (91-lat2)*gi.nScale;
  DrawColor(gi.kiOff);
  DrawBlock(0, 1, gs.xWin-1, y2-1);
  DrawBlock(0, y1+1, gs.xWin-1, gs.yWin-2);
  DrawColor(gi.kiLite);
  DrawDash(0, gs.yWin/2, gs.xWin-2, gs.yWin/2, 4);    /* Draw equator. */
  DrawColor(gi.kiOn);
  DrawLine(1, y2, gs.xWin-2, y2);
  DrawLine(1, y1, gs.xWin-2, y1);
  for (i = 1; i <= cObj*2; i++)
    end1[i] = end2[i] = -rLarge;

  /* Draw small hatches every 5 degrees along edges of world map. */

  DrawColor(gi.kiLite);
  for (i = lat1; i <= lat2; i += 5) {
    j = (91-i)*gi.nScale;
    k = (2+(i%10 == 0)+2*(i%30 == 0))*gi.nScaleT;
    DrawLine(1, j, k, j);
    DrawLine(gs.xWin-2, j, gs.xWin-1-k, j);
  }
  for (i = -180; i < 180; i += 5) {
    j = (180-i)*gi.nScale;
    k = (2+(i%10 == 0)+2*(i%30 == 0)+(i%90 == 0))*gi.nScaleT;
    DrawLine(j, y2+1, j, y2+k);
    DrawLine(j, y1-1, j, y1-k);
  }
  if (us.fLatitudeCross) {
    DrawColor(kRainbowB[7]);
    i = (int)((91.0-Lat)*(real)gi.nScale);
    DrawLine(0, i, gs.xWin-1, i);
  }

#ifdef MATRIX
  /* Calculate zenith locations of each planet. */

  for (i = 1; i <= cObj; i++) if (!ignore[i] || i == oMC) {
    planet1[i] = RFromD(Tropical(i == oMC ? is.MC : planet[i]));
    planet2[i] = RFromD(planetalt[i]);
    EclToEqu(&planet1[i], &planet2[i]);
  }

  /* Draw the Midheaven lines and zenith location markings. */

  if (lon < 0.0)
    lon += rDegMax;
  for (i = 1; i <= cObj; i++) if (FProper(i)) {
    x = planet1[oMC]-planet1[i];
    if (x < 0.0)
      x += 2.0*rPi;
    if (x > rPi)
      x -= 2.0*rPi;
    z = lon+DFromR(x);
    if (z > rDegHalf)
      z -= rDegMax;
    j = (int)(Mod(rDegHalf-z+gs.nRot)*(real)gi.nScale);
    DrawColor(kElemB[eEar]);
    DrawLine(j, y1+unit*4, j, y2-unit*1);
    end2[i*2-1] = (real)j;
    y = DFromR(planet2[i]);
    k = (int)((91.0-y)*(real)gi.nScale);
    if (FBetween((int)y, lat1, lat2)) {
      DrawColor(gi.kiLite);
      DrawBlock(j-gi.nScaleT, k-gi.nScaleT, j+gi.nScaleT, k+gi.nScaleT);
      DrawColor(gi.kiOff);
      DrawBlock(j, k, j, k);
    }

    /* Draw Nadir lines assuming we aren't in bonus chart mode. */

    if (!gs.fAlt) {
      j += 180*gi.nScale;
      if (j > gs.xWin-2)
        j -= (gs.xWin-2);
      end1[i*2-1] = (real)j;
      DrawColor(kElemB[eWat]);
      DrawLine(j, y1+unit*2, j, y2-unit*2);
    }
  }

  /* Now, normally, unless we are in bonus chart mode, we will go on to draw */
  /* the Ascendant and Descendant lines here.                                */

  longm = RFromD(Mod(DFromR(planet1[oMC])+lon));
  if (!gs.fAlt) for (i = 1; i <= cObj; i++) if (FProper(i)) {
    xold1 = xold2 = -1000;

    /* Hack: Normally we draw the Ascendant and Descendant line segments  */
    /* simultaneously. However, for the PostScript and metafile stroke    */
    /* graphics, this will case the file to get inordinately large due to */
    /* the constant thrashing between the Asc and Desc colors. Hence for  */
    /* these charts only, we'll do two passes for Asc and Desc.           */
    fStroke = gs.fPS || gs.fMeta;
    for (l = 0; l <= fStroke; l++)

    for (lat = (real)lat1; lat <= (real)lat2;
      lat += 1.0/(real)(gi.nScale/gi.nScaleT)) {

      /* First compute and draw the current segment of Ascendant line. */

      j = (int)((91.0-lat)*(real)gi.nScale);
      ad = RTan(planet2[i])*RTan(RFromD(lat));
      if (ad*ad > 1.0)
        ad = rLarge;
      else {
        ad = RAsin(ad);
        oa = planet1[i]-ad;
        if (oa < 0.0)
          oa += 2.0*rPi;
        am = oa-rPiHalf;
        if (am < 0.0)
          am += 2.0*rPi;
        z = longm-am;
        if (z < 0.0)
          z += 2.0*rPi;
        if (z > rPi)
          z -= 2.0*rPi;
        z = DFromR(z);
        k = (int)(Mod(rDegHalf-z+gs.nRot)*(real)gi.nScale);
        if (!fStroke || !l) {
          DrawColor(kElemB[eFir]);
          DrawWrap(xold1, j+gi.nScaleT, k, j, 1, gs.xWin-2);
          if (lat == (real)lat1) {                          /* Line segment */
            DrawLine(k, y1, k, y1+unit*4);                  /* pointing to  */
            end2[i*2] = (real)k;                            /* Ascendant.   */
          }
        } else if (lat == (real)lat1)
          end2[i*2] = (real)k;
        xold1 = k;
      }

      /* The curving Ascendant and Descendant lines actually touch at low or */
      /* high latitudes. Sometimes when we start out, a particular planet's  */
      /* lines haven't appeared yet, i.e. we are scanning at a latitude      */
      /* where our planet's lines don't exist. If this is the case, then     */
      /* when they finally do start, draw a thin horizontal line connecting  */
      /* the Ascendant and Descendant lines so they don't just start in      */
      /* space. Note that these connected lines aren't labeled with glyphs.  */

      if (ad == rLarge) {
        if (xold1 >= 0) {
          if (!fStroke || !l) {
            DrawColor(gi.kiGray);
            DrawWrap(xold1, j+1, xold2, j+1, 1, gs.xWin-2);
          }
          lat = rDegQuad;
        }
      } else {

      /* Then compute and draw corresponding segment of Descendant line. */

        od = planet1[i]+ad;
        dm = od+rPiHalf;
        z = longm-dm;
        if (z < 0.0)
          z += 2.0*rPi;
        if (z > rPi)
          z -= 2.0*rPi;
        z = DFromR(z);
        k = (int)(Mod(rDegHalf-z+gs.nRot)*(real)gi.nScale);
        if (xold2 < 0 && lat > (real)lat1 && (!fStroke || l)) {
          DrawColor(gi.kiGray);
          DrawWrap(xold1, j, k, j, 1, gs.xWin-2);
        }
        if (!fStroke || l) {
          DrawColor(kElemB[eAir]);
          DrawWrap(xold2, j+gi.nScaleT, k, j, 1, gs.xWin-2);
          if (lat == (real)lat1)                            /* Line segment */
            DrawLine(k, y1, k, y1+unit*2);                  /* pointing to  */
        }                                                   /* Descendant.  */
        xold2 = k;
      }
    }
#endif /* MATRIX */

    /* Draw segments pointing to top of Ascendant and Descendant lines. */

    if (ad != rLarge) {
      DrawColor(kElemB[eFir]);
      DrawLine(xold1, y2, xold1, y2-unit*1);
      DrawColor(kElemB[eAir]);
      DrawLine(k, y2, k, y2-unit*2);
      end1[i*2] = (real)k;
    }
  }

  DrawColor(kMainB[8]);
  i = (int)((181.0-Lon)*(real)gi.nScale);
  j = (int)((91.0-Lat)*(real)gi.nScale);
  if (us.fLatitudeCross)
    DrawSpot(i, j);
  else
    DrawPoint(i, j);

  /* Determine where to draw the planet glyphs. We have four sets of each    */
  /* planet - each planet's glyph appearing in the chart up to four times -  */
  /* one for each type of line. The Midheaven and Ascendant lines are always */
  /* labeled at the bottom of the chart, while the Nadir and Midheaven lines */
  /* at the top. Therefore we need to place two sets of glyphs, twice.       */

  for (i = 1; i <= cObj*2; i++) {
    symbol1[i] = end1[i];
    symbol2[i] = end2[i];
  }
  FillSymbolLine(symbol1);
  FillSymbolLine(symbol2);

  /* Now actually draw the planet glyphs. */

  for (i = 1; i <= cObj*2; i++) {
    j = (i+1)/2;
    if (FProper(j)) {
      if ((gi.xTurtle = (int)symbol1[i]) > 0 && gs.fLabel) {
        DrawColor(ret[j] < 0.0 ? gi.kiGray : gi.kiOn);
        DrawDash((int)end1[i], y2-unit*2, (int)symbol1[i], y2-unit*4,
          (ret[i] < 0.0 ? 1 : 0) - gs.fColor);
        DrawObject(j, gi.xTurtle, y2-unit*10);
      }
      if ((gi.xTurtle = (int)symbol2[i]) > 0) {
        DrawColor(ret[j] < 0.0 ? gi.kiGray : gi.kiOn);
        DrawDash((int)end2[i], y1+unit*4, (int)symbol2[i], y1+unit*8,
          (ret[i] < 0.0 ? 1 : 0) - gs.fColor);
        DrawObject(j, gi.xTurtle, y1+unit*14);
        DrawTurtle(szDrawObject[i & 1 ? oMC : oAsc], (int)symbol2[i],
          y1+unit*24-gi.nScaleT);
      }
    }
  }
}


/* Draw an aspect and midpoint grid in the window, with planets labeled down */
/* the diagonal. This chart is done when the -g switch is combined with the  */
/* -X switch. The chart always has a certain number of cells; hence based    */
/* how the restrictions are set up, there may be blank columns and rows,     */
/* or else only the first number of unrestricted objects will be included.   */

void XChartGrid()
{
  byte sz[cchSzDef];
  int unit, siz, x, y, i, j, k;
  KI c;

  unit = CELLSIZE*gi.nScale; siz = gs.nGridCell*unit;
  if (!FCreateGrid(gs.fAlt))
    return;

  /* Loop through each cell in each row and column of grid. */

  for (y = 1, j = oEar-1; y <= gs.nGridCell; y++) {
    do {
      j++;
    } while (!FProper(j) && j <= cObj);
    DrawColor(gi.kiGray);
    DrawDash(0, y*unit, siz, y*unit, !gs.fColor);
    DrawDash(y*unit, 0, y*unit, siz, !gs.fColor);
    if (j <= cObj) for (x = 1, i = oEar-1; x <= gs.nGridCell; x++) {
      do {
        i++;
      } while (!FProper(i) && i <= cObj);
      if (i <= cObj) {
        gi.xTurtle = x*unit-unit/2;
        gi.yTurtle = y*unit-unit/2 -
          (gi.nScale/gi.nScaleT > 2 ? 5*gi.nScaleT : 0);
        k = grid->n[i][j];

        /* If this is an aspect cell, draw glyph of aspect in effect. */

        if (gs.fAlt ? x > y : x < y) {
          if (k) {
            DrawColor(c = kAspB[k]);
            DrawAspect(k, gi.xTurtle, gi.yTurtle);
          }

        /* If this is a midpoint cell, draw glyph of sign of midpoint. */

        } else if (gs.fAlt ? x < y : x > y) {
          DrawColor(c = kSignB(grid->n[i][j]));
          DrawSign(grid->n[i][j], gi.xTurtle, gi.yTurtle);

        /* For cells on main diagonal, draw glyph of planet. */

        } else {
          DrawColor(gi.kiLite);
          DrawEdge((y-1)*unit, (y-1)*unit, y*unit, y*unit);
          DrawObject(i, gi.xTurtle, gi.yTurtle);
        }

        /* When the scale size is 300+, we can print text in each cell: */

        if (gi.nScale/gi.nScaleT > 2 && gs.fLabel) {
          k = abs(grid->v[i][j]);

          /* For the aspect portion, print the orb in degrees and minutes. */

          if (gs.fAlt ? x > y : x < y) {
            if (grid->n[i][j])
              sprintf(sz, "%c%d %02d'", k != grid->v[i][j] ? (us.fAppSep ?
                'a' : '-') : (us.fAppSep ? 's' : '+'), k/60, k%60);
            else
              sprintf(sz, "");

          /* For the midpoint portion, print the degrees and minutes. */

          } else if (gs.fAlt ? x < y : x > y)
            sprintf(sz, "%2d %02d'", k/60, k%60);

          /* For the main diagonal, print degree and sign of each planet. */

          else {
            c = kSignB(grid->n[i][j]);
            sprintf(sz, "%c%c%c %02d", chSig3(grid->n[i][j]), k);
          }
          DrawColor(c);
          DrawSz(sz, x*unit-unit/2, y*unit-3*gi.nScaleT, dtBottom);
        }
      }
    }
  }
}


/* Draw the local horizon, and draw in the planets where they are at the */
/* time in question, as done when the -Z is combined with the -X switch. */

void XChartHorizon()
{
  real lat, lonz[objMax], latz[objMax], azi[objMax], alt[objMax];
  int x[objMax], y[objMax], m[objMax], n[objMax],
    cx, cy, unit, x1, y1, x2, y2, xs, ys, i, j, k, l;
  byte sz[2];
  real lonMC, lon1, lat1, alt1, azi1;
  int xx1, yy1;

  unit = Max(12, 6*gi.nScale);
  x1 = unit; y1 = unit; x2 = gs.xWin-1-unit; y2 = gs.yWin-1-unit;
  unit = 12*gi.nScale;
  xs = x2-x1; ys = y2-y1; cx = (x1+x2)/2; cy = (y1+y2)/2;

  /* Make a slightly smaller rectangle within the window to draw the planets */
  /* in. Make segments on all four edges marking 5 degree increments.        */

  DrawColor(gi.kiLite);
  for (i = 5; i < 180; i += 5) {
    j = y1+(int)((real)i*(real)ys/rDegHalf);
    k = (2+(i%10 == 0)+2*(i%30 == 0))*gi.nScaleT;
    DrawLine(x1+1, j, x1+1+k, j);
    DrawLine(x2-1, j, x2-1-k, j);
  }
  sz[1] = chNull;
  for (i = 5; i < nDegMax; i += 5) {
    j = x1+(int)((real)i*(real)xs/rDegMax);
    k = (2+(i%10 == 0)+2*(i%30 == 0))*gi.nScaleT;
    DrawLine(j, y1+1, j, y1+1+k);
    DrawLine(j, y2-1, j, y2-1-k);
    if (i % 90 == 0) {
      *sz = *szDir[i/90 & 3];
      DrawSz(sz, j, y1-2*gi.nScaleT, dtBottom);
    }
  }

  /* Draw vertical lines dividing our rectangle into four areas. In our     */
  /* local space chart, the middle line represents due south, the left line */
  /* due east, the right line due west, and the edges due north. A fourth   */
  /* horizontal line divides that which is above and below the horizon.     */

  DrawColor(gi.kiGray);
  DrawDash(cx, y1, cx, y2, 1);
  DrawDash((cx+x1)/2, y1, (cx+x1)/2, y2, 1);
  DrawDash((cx+x2)/2, y1, (cx+x2)/2, y2, 1);
  DrawColor(gi.kiOn);
  DrawEdge(x1, y1, x2, y2);
  DrawDash(x1, cy, x2, cy, 1);

  /* Calculate the local horizon coordinates of each planet. First convert */
  /* zodiac position and declination to zenith longitude and latitude.     */

  lat = RFromD(RealCoord(Lat));
  for (i = 0; i <= cObj; i++) if (!ignore[i] || i == oMC) {
    if (us.fEquator) {
      lonz[i] = RFromD(planet[i]);
      latz[i] = RFromD(planetalt[i]);
    } else {
      lonz[i] = RFromD(Tropical(planet[i]));
      latz[i] = RFromD(planetalt[i]);
      EclToEqu(&lonz[i], &latz[i]);
    }
  }
  if (PolarMCflip && hRevers)
    lonMC = Mod(lonz[oMC]+rPi);
  else
    lonMC = lonz[oMC];

  if (!fEquator)
    for (i = 0; i < 180; i++) {
      lon1 = RFromD(2*(real)i);
      lat1 = 0.0;
      lon1 = RFromD(Mod(DFromR(lonMC-lon1+rPiHalf)));
      EquToLocal(&lon1, &lat1, rPiHalf-lat);
      azi1 = rDegMax-DFromR(lon1); alt1 = DFromR(lat1);
      xx1 = x1+(int)((real)xs*(Mod(rDegQuad-azi1))/rDegMax+rRound);
      yy1 = y1+(int)((real)ys*(rDegQuad-alt1)/rDegHalf+rRound);
      DrawColor(kLtGray);
      DrawPoint(xx1, yy1);
    }
  for (i = 0; i < 180; i++) {
    lon1 = 2*(real)i;
    lat1 = 0.0;
    lon1 = RFromD(Tropical(lon1));
    EclToEqu(&lon1, &lat1);
    lon1 = RFromD(Mod(DFromR(lonMC-lon1+rPiHalf)));
    if (!fEquator)
      EquToLocal(&lon1, &lat1, rPiHalf-lat);
    azi1 = rDegMax-DFromR(lon1); alt1 = DFromR(lat1);
    xx1 = x1+(int)((real)xs*(Mod(rDegQuad-azi1))/rDegMax+rRound);
    yy1 = y1+(int)((real)ys*(rDegQuad-alt1)/rDegHalf+rRound);
    DrawColor(kDkGray);
    if (RFract((real)i/15.0) <= 0.0 && i != 0)
      DrawColor(kWhite);
    if (!i)
      DrawColor(kRed);
    if (i == 90)
      DrawColor(kGreen);
    DrawPoint(xx1, yy1);
  }

  for (i = 0; i <= cObj; i++) if (FProper(i)) {
    lonz[i] = RFromD(Mod(DFromR(lonMC-lonz[i]+rPiHalf)));
    if (!fEquator)
      EquToLocal(&lonz[i], &latz[i], rPiHalf-lat);
    azi[i] = rDegMax-DFromR(lonz[i]); alt[i] = DFromR(latz[i]);
    x[i] = x1+(int)((real)xs*(Mod(rDegQuad-azi[i]))/rDegMax+rRound);
    y[i] = y1+(int)((real)ys*(rDegQuad-alt[i])/rDegHalf+rRound);
    m[i] = x[i]; n[i] = y[i]+unit/2;
  }

  /* As in the DrawGlobe() routine, we now determine where to draw the   */
  /* glyphs in relation to the actual points, so that the glyphs aren't  */
  /* drawn on top of each other if possible. Again, we assume that we'll */
  /* put the glyph right under the point, unless there would be some     */
  /* overlap and the above position is better off.                       */

  for (i = 0; i <= cObj; i++) if (FProper(i)) {
    k = l = gs.xWin+gs.yWin;
    for (j = 1; j < i; j++) if (FProper(j)) {
      k = Min(k, abs(m[i]-m[j])+abs(n[i]-n[j]));
      l = Min(l, abs(m[i]-m[j])+abs(n[i]-unit-n[j]));
    }
    if (k < unit || l < unit)
      if (k < l)
        n[i] -= unit;
  }
  for (i = cObj; i >= 0; i--) if (FProper(i))    /* Draw planet's glyph. */
    DrawObject(i, m[i], n[i]);
  for (i = cObj; i >= 0; i--) if (FProper(i)) {
    DrawColor(kObjB[i]);
    if (!gs.fAlt)
      DrawPoint(x[i], y[i]);      /* Draw small or large dot */
    else                          /* near glyph indicating   */
      DrawSpot(x[i], y[i]);       /* exact local location.   */
  }
}


/* Draw the local horizon, and draw in the planets where they are at the  */
/* time in question. This chart is done when the -Z0 is combined with the */
/* -X switch. This is an identical function to XChartHorizon(); however,  */
/* that routine's chart is entered on the horizon and meridian. Here we   */
/* center the chart around the center of the sky straight up from the     */
/* local horizon, with the horizon itself being an encompassing circle.   */

void XChartHorizonSky()
{
  real lat, rx, ry, s,
    lonz[objMax], latz[objMax], azi[objMax], alt[objMax];
  int x[objMax], y[objMax], m[objMax], n[objMax],
    cx, cy, unit, x1, y1, x2, y2, xs, ys, i, j, k, l;
  real lonMC, coef;
  int ccx, ccy;

  unit = Max(12, 6*gi.nScale);
  x1 = unit; y1 = unit; x2 = gs.xWin-1-unit; y2 = gs.yWin-1-unit;
  unit = 12*gi.nScale;
  xs = x2-x1; ys = y2-y1; cx = (x1+x2)/2; cy = (y1+y2)/2;

  /* Draw a circle in window to indicate horizon line, lines dividing   */
  /* the window into quadrants to indicate n/s and w/e meridians, and   */
  /* segments on these lines marking 5 degree increments.               */

  coef = 1.8;
  DrawColor(gi.kiGray);
  DrawDash(cx, y1, cx, y2, 1);
  DrawDash(x1, cy, x2, cy, 1);
  DrawColor(gi.kiLite);
  for (i = -160; i <= 160; i += 5) {
    k = (2+(i/10*10 == i ? 1 : 0)+(i/30*30 == i ? 2 : 0))*gi.nScaleT;
    s = 1.0/(rDegQuad*coef);
    j = cy+(int)(s*ys/2*i);
    DrawLine(cx-k, j, cx+k, j);
    j = cx+(int)(s*xs/2*i);
    DrawLine(j, cy-k, j, cy+k);
  }
  DrawSz(gs.fAlt ? "Z" : "N", cx, y1-2*gi.nScaleT, dtBottom);
  DrawSz("E", x1/2, cy+2*gi.nScaleT, dtCent);
  DrawSz("W", (gs.xWin+x2)/2, cy+2*gi.nScaleT, dtCent);
  if (!gs.fText)
    DrawSz(gs.fAlt ? "N" : "S", cx, gs.yWin-3*gi.nScaleT, dtBottom);
  rx = xs/2/coef; ry = ys/2/coef;
  DrawColor(gi.kiOn);
  DrawEdge(x1, y1, x2, y2);
  DrawCircle(cx, cy, (int)rx, (int)ry);
  for (i = 0; i < nDegMax; i += 5) {
    k = (2+(i/10*10 == i ? 1 : 0)+(i/30*30 == i ? 2 : 0))*gi.nScaleT;
    DrawLine(cx+(int)((rx-k)*RCosD((real)i)), cy+(int)((ry-k)*RSinD((real)i)),
      cx+(int)((rx+k)*RCosD((real)i)), cy+(int)((ry+k)*RSinD((real)i)));
  }

   /* Draw outhern dashed circle, which represents nadir point, which is */
   /* now on screen in corners, because I rescaled picture. V.A.         */

  rx = xs/coef; ry = ys/coef;
  for (i = 0; i < nDegMax; i ++) {
    ccx = (int)(rx*RCosD((real)i)) + cx;
    ccy = (int)(ry*RSinD((real)i)) + cy;
    if (ccx < x1 || ccx > x2 || ccy < y1 || ccy > y2)
      continue; 
    DrawPoint(ccx, ccy);
  }
  for (i = 0; i < nDegMax; i += 5) {
    if (i < 27 || FBetween(i, 62, 118) || FBetween(i, 152,208) ||
        FBetween(i, 242, 298) || i > 333)
      continue;
    k = (1+(i/10*10 == i ? 1 : 0)+(i/30*30 == i ? 1 : 0))*gi.nScaleT;
    DrawLine(cx+(int)((rx-k)*RCosD((real)i)), cy+(int)((ry-k)*RSinD((real)i)),
      cx+(int)((rx+k)*RCosD((real)i)), cy+(int)((ry+k)*RSinD((real)i)));
  }
  rx = xs/2/coef; ry = ys/2/coef;

  /* Calculate the local horizon coordinates of each planet. First convert */
  /* zodiac position and declination to zenith longitude and latitude.     */

  lat = RFromD(RealCoord(Lat));
  for (i = 0; i <= cObj; i++) if (!ignore[i] || i == oMC) {
    if (us.fEquator) {
      lonz[i] = RFromD(planet[i]);
      latz[i] = RFromD(planetalt[i]);
    } else {
      lonz[i] = RFromD(Tropical(planet[i]));
      latz[i] = RFromD(planetalt[i]);
      EclToEqu(&lonz[i], &latz[i]);
    }
  }
  if (PolarMCflip && hRevers)
    lonMC = Mod(lonz[oMC]+rPi);
  else
    lonMC = lonz[oMC];

  for (i = 0; i <= cObj; i++) if (FProper(i) && i != oMC) {
    lonz[i] = RFromD(Mod(DFromR(lonMC-lonz[i]+rPiHalf)));
    if (!fEquator)
      EquToLocal(&lonz[i], &latz[i], rPiHalf-lat);
    azi[i] = rDegMax-DFromR(lonz[i]);

            /* In bonus mode convert from altitude/azimuth coordinates to */
            /* prime vertical coordinates that we'll print instead.       */

    if (gs.fAlt) {
        alt[i] = DFromR(latz[i]);
        azi[i] = RFromD(azi[i]); alt[i] = RFromD(alt[i]);
        CoorXform(&azi[i], &alt[i], rPiHalf);
        azi[i] = rDegMax - DFromR(azi[i]);
        alt[i] = rDegQuad + DFromR(alt[i]);
    } else {
        alt[i] = rDegQuad-DFromR(latz[i]);
    }
    s = alt[i]/rDegQuad;
    x[i] = cx+(int)(rx*s*RCosD(rDegHalf+azi[i])+rRound);
    y[i] = cy+(int)(ry*s*RSinD(rDegHalf+azi[i])+rRound);
    if (!FOnWin(x[i], y[i]))
      x[i] = -1000;
    m[i] = x[i]; n[i] = y[i]+unit/2;
  }

  /* As in the DrawGlobe() routine, we now determine where to draw the   */
  /* glyphs in relation to the actual points, so that the glyphs aren't  */
  /* drawn on top of each other if possible. Again, we assume that we'll */
  /* put the glyph right under the point, unless there would be some     */
  /* overlap and the above position is better off.                       */

  for (i = 0; i <= cObj; i++) if (FProper(i)) {
    k = l = gs.xWin+gs.yWin;
    for (j = 0; j < i; j++) if (FProper(j)) {
      k = Min(k, abs(m[i]-m[j])+abs(n[i]-n[j]));
      l = Min(l, abs(m[i]-m[j])+abs(n[i]-unit-n[j]));
    }
    if (k < unit || l < unit)
      if (k < l)
        n[i] -= unit;
  }
  for (i = cObj; i >= 0; i--) if (m[i] >= x1 && FProper(i))  /* Draw glyph. */
    DrawObject(i, m[i], n[i]);
  for (i = cObj; i >= 0; i--) if (x[i] >= y1 && FProper(i)) {
    DrawColor(kObjB[i]);
    DrawSpot(x[i], y[i]);
  }
}


/* Draw a chart depicting an aerial view of the solar system in space, with */
/* all the planets drawn around the Sun, and the specified central planet   */
/* in the middle, as done when the -S is combined with the -X switch.       */

void XChartOrbit()
{
  int x[objMax], y[objMax], m[objMax], n[objMax],
    cx = gs.xWin / 2, cy = gs.yWin / 2, unit, x1, y1, x2, y2, i, j, k, l;
  real sx, sy, sz = 30.0, xp, yp, a;

  unit = Max(gs.fText*12, 6*gi.nScale);
  x1 = unit; y1 = unit; x2 = gs.xWin-1-unit; y2 = gs.yWin-1-unit;
  unit = 12*gi.nScale;

  /* Determine the scale of the chart. For a scale size of 400+, make the */
  /* graphic 1 AU in radius (just out to Earth's orbit). For 300, make    */
  /* the chart 6 AU in radius (enough for inner planets out to asteroid   */
  /* belt). For a scale of 200, make window 30 AU in radius (enough for   */
  /* planets out to Neptune). For scale of 100, make it 90 AU in radius   */
  /* (enough for all planets including the orbits of the uranians.)       */

  if (gi.nScale/gi.nScaleT < 2)
    sz = 90.0;
  else if (gi.nScale/gi.nScaleT == 3)
    sz = 6.0;
  else if (gi.nScale/gi.nScaleT > 3)
    sz = 1.0;
  sx = (real)(cx-x1)/sz; sy = (real)(cy-y1)/sz;
  for (i = 0; i <= oNorm; i++) if (FProper(i)) {
    xp = spacex[i]; yp = spacey[i];
    x[i] = cx-(int)(xp*sx); y[i] = cy+(int)(yp*sy);
    m[i] = x[i]; n[i] = y[i]+unit/2;
  }

  /* As in the DrawGlobe() routine, we now determine where to draw the   */
  /* glyphs in relation to the actual points, so that the glyphs aren't  */
  /* drawn on top of each other if possible. Again, we assume that we'll */
  /* put the glyph right under the point, unless there would be some     */
  /* overlap and the above position is better off.                       */

  for (i = 0; i <= oNorm; i++) if (FProper(i)) {
    k = l = gs.xWin+gs.yWin;
    for (j = 0; j < i; j++) if (FProper(j)) {
      k = Min(k, abs(m[i]-m[j])+abs(n[i]-n[j]));
      l = Min(l, abs(m[i]-m[j])+abs(n[i]-unit-n[j]));
    }
    if (k < unit || l < unit)
      if (k < l)
        n[i] -= unit;
  }

  /* Draw the 12 sign boundaries from the center body to edges of screen. */

  a = Mod(DFromR(Angle(spacex[oJup], spacey[oJup]))-planet[oJup]);
  DrawColor(gi.kiGray);
  for (i = 0; i < cSign; i++) {
    k = cx+2*(int)((real)cx*RCosD((real)i*30.0+a));
    l = cy+2*(int)((real)cy*RSinD((real)i*30.0+a));
    DrawClip(cx, cy, k, l, x1, y1, x2, y2, 1);
  }
  DrawColor(gi.kiLite);
  DrawEdge(x1, y1, x2, y2);
  for (i = oNorm; i >= 0; i--)
    if (FProper(i) && FInRect(m[i], n[i], x1, y1, x2, y2))
      DrawObject(i, m[i], n[i]);
  for (i = oNorm; i >= 0; i--)
    if (FProper(i) && FInRect(x[i], y[i], x1, y1, x2, y2)) {
      DrawColor(kObjB[i]);
      if (!gs.fAlt || i > oNorm)
        DrawPoint(x[i], y[i]);      /* Draw small or large dot */
      else                          /* near glyph indicating   */
        DrawSpot(x[i], y[i]);       /* exact orbital location. */
    }
}


/* Draw a chart showing the 36 Gauquelin sectors, with all the planets    */
/* positioned in their appropriate sector (and at the correct fracton     */
/* across the sector) as done when the -l is combined with the -X switch. */

void XChartSector()
{
  real xplanet[objMax], symbol[objMax];
  byte sz[3];
  int cx, cy, i, j, k;
  real unitx, unity, px, py, temp;
  byte ignoreT[objMax+1];

  for (i = 1; i <= objMax; i++)
    ignoreT[i] = ignore[i];

  if (gs.fText && !us.fVelocity)
    gs.xWin -= xSideT;
  cx = gs.xWin/2 - 1; cy = gs.yWin/2 - 1;
  unitx = (real)cx; unity = (real)cy;

  /* Draw lines across the whole chart at the four angles. */

  DrawColor(gi.kiLite);
  DrawDash(cx+POINT1(unitx, 0.99, PX(0.0)),
           cy+POINT1(unity, 0.99, PY(0.0)),
           cx+POINT1(unitx, 0.99, PX(180.0)),
           cy+POINT1(unity, 0.99, PY(180.0)), !gs.fColor);
  DrawDash(cx+POINT1(unitx, 0.99, PX(90.0)),
           cy+POINT1(unity, 0.99, PY(90.0)),
           cx+POINT1(unitx, 0.99, PX(270.0)),
           cy+POINT1(unity, 0.99, PY(270.0)), !gs.fColor);

  /* Draw circles and radial lines delineating the 36 sectors. */

  DrawColor(gi.kiOn);
  for (i = 0; i < nDegMax; i += 10) {
    px = PX((real)i); py = PY((real)i);
    DrawLine(cx+POINT1(unitx, 0.81, px), cy+POINT1(unity, 0.81, py),
      cx+POINT2(unitx, 0.95, px), cy+POINT2(unity, 0.95, py));
  }
  DrawCircle(cx, cy, (int)(unitx*0.95+rRound), (int)(unity*0.95+rRound));
  DrawCircle(cx, cy, (int)(unitx*0.81+rRound), (int)(unity*0.81+rRound));

  /* Label the 36 sectors, with plus zones in red and normal in dark green. */

  k = pluszone[cSector];
  for (i = 1; i <= cSector; i++) {
    j = pluszone[i];
    DrawColor(j ? kRainbowB[1] : kMainB[5]);
    sprintf(sz, "%d", i);
    DrawSz(sz, cx+POINT1(unitx, 0.88, PX((real)(i*10+175)))+
      (FBetween(i, 12, 19) ? -(gi.nScale/* *gi.nScaleT*/) : 0),
      cy+POINT1(unity, 0.88, PY((real)(i*10+175)))+(gi.nScale/* *gi.nScaleT*/),
      dtCent | dtScale);
    sprintf(sz, "%c", j ? '+' : '-');
    DrawSz(sz, cx+POINT1(unitx, 0.97, PX((real)(i*10+175))),
      cy+POINT1(unity, 0.97, PY((real)(i*10+175)))+gi.nScaleT*2, dtCent);
    if (j != k) {
      DrawColor(gi.kiGray);
      DrawDash(cx, cy, cx+POINT2(unitx, 0.81, PX((real)(i*10+170))),
        cy+POINT2(unity, 0.81, PY((real)(i*10+170))), 1);
    }
    k = j;
  }

  if (!gs.fAlt && !FCreateGrid(fFalse))
      return;
  CastSectors();    /* Go compute the planets' sector positions. */

  for (i = 0; i <= cObj; i++)    /* Figure out where to put planet glyphs. */
    symbol[i] = xplanet[i] = Mod(rDegHalf - planet[i]);
  FillSymbolRing(symbol, 1.0);

  /* For each planet, draw a small dot indicating where it is, and then */
  /* a line from that point to the planet's glyph.                      */

  for (i = cObj; i >= 0; i--) if (FProper(i)) {
    if (gs.fLabel) {
      temp = symbol[i];
      DrawColor(ret[i] < 0.0 ? gi.kiGray : gi.kiOn);
      DrawDash(cx+POINT1(unitx, 0.67, PX(xplanet[i])),
        cy+POINT1(unity, 0.67, PY(xplanet[i])),
        cx+POINT1(unitx, 0.71, PX(temp)),
        cy+POINT1(unity, 0.71, PY(temp)),
        (ret[i] < 0.0 ? 1 : 0) - gs.fColor);
      DrawObject(i, cx+POINT1(unitx, 0.75, PX(temp)),
        cy+POINT1(unity, 0.75, PY(temp)));
    } else
      DrawColor(kObjB[i]);
    DrawPoint(cx+POINT1(unitx, 0.65, PX(xplanet[i])),
      cy+POINT1(unity, 0.65, PY(xplanet[i])));
  }

  /* Draw lines connecting planets which have aspects between them. */

  if (!gs.fAlt) {                /* Don't draw aspects in bonus mode. */
    for (j = cObj; j >= 1; j--)
      for (i = j-1; i >= 0; i--)
        if (grid->n[i][j] && FProper(i) && FProper(j)) {
          DrawColor(kAspB[grid->n[i][j]]);
          DrawDash(cx+POINT1(unitx, 0.63, PX(xplanet[i])),
            cy+POINT1(unity, 0.63, PY(xplanet[i])),
            cx+POINT1(unitx, 0.63, PX(xplanet[j])),
            cy+POINT1(unity, 0.63, PY(xplanet[j])),
            abs(grid->v[i][j]/60/2));
        }
  }
  CastChart(fTrue);     /* restore cp0 for infoborder */
  DrawInfo();
  for (i = 1; i <= objMax; i++)
    ignore[i] = ignoreT[i];
  CastChart(fTrue);      /* restore normal restriction */
}

/* Draw an arrow from one point to another, a line with an arrowhead at the */
/* ending point. The size of the arrowhead is based on current scale size,  */
/* and the line segment is actually shorter and doesn't touch either        */
/* endpoint by the same amount. This is used by XChartDispositor() below.   */

void DrawArrow(x1, y1, x2, y2)
int x1, y1, x2, y2;
{
  real r, s, a;

  r = DFromR(Angle((real)(x2-x1), (real)(y2-y1)));
  s = (real)(gi.nScale*8);
  x1 += (int)(s*RCosD(r)); y1 += (int)(s*RSinD(r));    /* Shrink line by    */
  x2 -= (int)(s*RCosD(r)); y2 -= (int)(s*RSinD(r));    /* the scale amount. */
  s = (real)(gi.nScale)*4.5;
  DrawLine(x1, y1, x2, y2);                            /* Main segment. */
  for (a = -1.0; a <= 1.0; a += 2.0)
    DrawLine(x2, y2, x2 + (int)(s*RCosD(r + a*135.0)), /* The two arrow     */
      y2 + (int)(s*RSinD(r + a*135.0)));               /* head line pieces. */
}

/* Draw dispositor graphs for the 10 main planets, as done when the -j is   */
/* combined with the -X switch. Four graphs are drawn, one in each screen   */
/* quadrant. A dispositor graph may be based on the sign or house position, */
/* and the planets may be arranged in a hierarchy or a wheel format.        */

void XChartDispositor()
{
  int oDis[oNorm+1], dLev[oNorm+1], cLev[oNorm+1], xo[oNorm+1], yo[oNorm+1];
  real xCirc[oNorm+1], yCirc[oNorm+1];
  byte sz[cchSzDef];
  byte SaveRest[oNorm+1];
  real jj;
  int xLev, yLev, xSub, ySub, cx0, cy0, cx, cy, i, j, k;
  int Rulers = 12;
  byte SaveUra, SaveCentr;

  for (i = 1; i <= cSign-1; i++)
    for (j = i+1; j <= cSign; j++)
      if ((rules[i] == rules[j]) && (i != j))
         Rulers -= 1;

  for (i = 1; i <= oNorm; i++)
     SaveRest[i] = ignore[i];
  for (i = 1; i <= oNorm; i++)
     if(RulerPlanet(i))
       ignore[i] = 0;
  SaveUra = us.fUranian;
  SaveCentr = us.objCenter;
  us.objCenter = oEar;
  us.fUranian = fTrue;
  CastChart(fTrue);

  /* Set up screen positions of the 10 planets for the wheel graphs. */
  /* Now number of planets can be up to 12.  (V.A.)                  */

  jj = rDegHalf;
  cx0 = gs.xWin / 3; cy0 = gs.yWin / 2;
  for (i = 1; i <= oNorm; i++) if(RulerPlanet(i)) {
    xCirc[i] = (real)cx0*0.4*RCosD(jj);
    yCirc[i] = (real)cy0*0.35*RSinD(jj);
    jj -= rDegMax/(real)Rulers;
  }

  /* Loop over the two basic dispositor types: sign based and house based. */

  for (xSub = 0; xSub <= 2; xSub++) {
    cx = xSub * cx0 + cx0 / 2 ;

    /* For each planet, get its dispositor planet for current graph type. */

    for (i = 1; i <= oNorm; i++) {
      dLev[i] = 1;
      if (xSub == 0)                                  
        oDis[i] = rules[SFromZ(planet[i])];           
      if (xSub == 1)                                   
           oDis[i] = rules[inhouse[i]];                  
      if (xSub == 2)
        oDis[i] = rules[SFromZ(chouse[inhouse[i]])];
    }

    /* Determine the final dispositors (including mutual reception loops). */

    do {
      j = fFalse;
      for (i = 1; i <= oNorm; i++)
        cLev[i] = fFalse;
      for (i = 1; i <= oNorm; i++)
        if (dLev[i] && RulerPlanet(i))
          cLev[oDis[i]] = fTrue;
      for (i = 1; i <= oNorm; i++)    /* A planet isn't a final dispositor */
        if (dLev[i] && !cLev[i] && RulerPlanet(i)) {    /* if nobody is pointing to it.      */
          dLev[i] = 0;
          j = fTrue;
        }
    } while (j);

    for (i = 1; i <= oNorm; i++)
       if (!RulerPlanet(i))
          dLev[i] = 0;

    /* Determine the level of each planet, i.e. how many times you have to */
    /* jump to your dispositor before reaching a final, with finals == 1.  */

    do {
      j = fFalse;
      for (i = 1; i <= oNorm; i++) if(RulerPlanet(i)) {
        if (!dLev[i]) {
          if (!dLev[oDis[i]])
            j = fTrue;
          else                              /* If my dispositor already has */
            dLev[i] = dLev[oDis[i]] + 1;    /* a level, mine is one more.   */
        }
      }
    } while (j);

    /* Count the number of planets at each dispositor level. */

    for (i = 1; i <= oNorm; i++)
      cLev[i] = 0;
    for (i = 1; i <= oNorm; i++)
      cLev[dLev[i]]++;

    /* Count the number of levels total, and max planets on any one level. */

    xLev = yLev = 0;
    for (i = 1; i <= oNorm; i++)
      if (cLev[i]) {
        yLev = i;
        if (cLev[i] > xLev)
          xLev = cLev[i];
      }

    /* Loop over our two dispositor display formats: hierarchy and wheel. */

    for (ySub = 0; ySub <= 1; ySub++) {
      cy = ySub * cy0 + cy0 / 2;
      sprintf(sz, "%s dispositor %s.", xSub ? "House" : "Sign",
        ySub ? "wheel" : "hierarchy");
      DrawColor(gi.kiLite);
      DrawSz(sz, cx, ySub * cy0 + 3*gi.nScaleT, dtTop);

      if (ySub) {

        /* Draw a graph in wheel format. */

        for (i = 1; i <= oNorm; i++) if(RulerPlanet(i)) {
          DrawObject(i, cx + (int)xCirc[i], cy + (int)yCirc[i]);
          j = oDis[i];
          if (j != i) {
            if (dLev[i] < 2)
              DrawColor(gi.kiOn);
            else
              DrawColor(kObjB[i]);
            DrawArrow(cx + (int)xCirc[i], cy + (int)yCirc[i],
              cx + (int)xCirc[j], cy + (int)yCirc[j]);
          }
          if (!gs.fAlt && (j == i || dLev[i] < 2)) {
            DrawColor(j == i ? gi.kiOn : gi.kiGray);
            DrawCircle(cx + (int)xCirc[i], cy + (int)yCirc[i],
              7*gi.nScale, 7*gi.nScale);
          }
        }
      } else {

        /* For level hierarchies, first figure out the screen coordinates    */
        /* for each planet, based on its level, total levels, and max width. */

        for (i = 1; i <= oNorm; i++) if(RulerPlanet(i)) {
          yo[i] = cy0*(dLev[i]*2-1)/(yLev*2);
          k = 0;
          for (j = 1; j < i; j++) if(RulerPlanet(j)) {
            if (dLev[i] == dLev[j])
              k = j;
          }
          if (k)
            xo[i] = xo[k] + cx0/xLev;    /* One right of last one on level. */
          else
            xo[i] = cx - ((cx0/xLev)*(cLev[dLev[i]]-1)/2);
        }

        /* Draw graph in level hierarchy format. */

        for (i = 1; i <= oNorm; i++) if(RulerPlanet(i)) {
          DrawObject(i, xo[i], yo[i]);
          j = oDis[i];
          if (j != i) {
            if (dLev[i] < 2) {
              if (abs(xo[i] - xo[j]) < cx0/xLev*3/2) {
                DrawColor(gi.kiOn);
                DrawArrow(xo[i], yo[i], xo[j], yo[j]);
              }
              DrawColor(gi.kiGray);
            } else {
              DrawColor(kObjB[i]);
              DrawArrow(xo[i], yo[i], xo[j], yo[j]);
            }
          } else
            DrawColor(gi.kiOn);
          if (!gs.fAlt && dLev[i] < 2)
            DrawCircle(xo[i], yo[i], 7*gi.nScale, 7*gi.nScale);
        }
      }
    }
  }

  /* Draw boundary lines between the four separate dispositor graphs. */

  if (gs.fBorder) {
    DrawColor(gi.kiLite);
    DrawBlock(cx0, 0, cx0, gs.yWin - 1);
    DrawBlock(2*cx0, 0, 2*cx0, gs.yWin - 1);
    DrawBlock(0, cy0, gs.xWin, cy0);
  }
  for (i = 1; i <= oNorm; i++)
    ignore[i] = SaveRest[i];
  us.fUranian = SaveUra;
  us.objCenter = SaveCentr;
  CastChart(fTrue);
}


/* Draw a graphical calendar for a given month, with numbers in boxes,  */
/* scaled to fit within the given bounds. This is used for single month */
/* -K switch images and is called 12 times for a full year -Ky image.   */

void DrawCalendar(mon, X1, Y1, X2, Y2)
int mon, X1, Y1, X2, Y2;
{
  byte sz[cchSzDef];
  int day, cday, dayHi, cweek, xunit, yunit, xs, ys, x1, y1, x, y, s;

  xs = X2 - X1; ys = Y2 - Y1;
  day = DayOfWeek(mon, 1, Yea);    /* Day of week of 1st of month.     */
  cday = DaysInMonth(mon, Yea);    /* Count of days in the month.      */
  dayHi = DayInMonth(mon, Yea);    /* Number of last day in the month. */
  cweek = us.fCalendarYear ? 6 : (day + cday + 6) / 7;   /* Week rows. */
  xunit = xs/8;                    /* Hor. pixel size of each day box. */
  yunit = ys/(cweek+2);            /* Ver. pixel size of each day box. */
  x1 = X1 + (xs - xunit*7) / 2;    /* Blank space to left of calendar. */
  y1 = Y1 + yunit*3/2;             /* Blank space to top of calendar.  */

  /* Print the month and year in big letters at top of chart. */

  DrawColor(gi.kiOn);
  sprintf(sz, "%s, %d", szMonth[mon], Yea);
  s = gi.nScale;
  gi.nScale = Min((yunit*3/2-yFont*s) / yFont, xs/9/*CchSz(sz)*/ / xFont);
  gi.nScale = Max(gi.nScale-1, 1);
  DrawSz(sz, X1 + xs/2, Y1 + (yunit*3/2-yFont*s)/2, dtCent | dtScale);
  gi.nScale = s;

  /* Draw the grid of boxes for the days. */

  for (x = 0; x <= cWeek; x++) {

    /* Print days of week at top of each column (abbreviated if need be). */

    if (x < cWeek) {
      if (xunit / (xFont*gi.nScale) < 9)
        sprintf(sz, "%c%c%c", chDay3(x));
      else
        sprintf(sz, "%s", szDay[x]);
      DrawColor(kRainbowB[3]);
      DrawSz(sz, x1 + x*xunit + xunit/2, y1 - s*3, dtBottom | dtScale);
      DrawColor(kRainbowB[5]);
    }
    DrawLine(x1 + x*xunit, y1, x1 + x*xunit, y1 + cweek*yunit);
  }
  for (y = 0; y <= cweek; y++)
    DrawLine(x1, y1 + y*yunit, x1 + 7*xunit, y1 + y*yunit);

  /* Actually draw the day numbers in their appropriate boxes. */

  x = day; y = 0;
  for (day = 1; day <= dayHi; day = AddDay(mon, day, Yea, 1)) {
    sprintf(sz, gs.fText ? "%2d" : "%d", day);
    DrawColor(day == Day && mon == Mon && gs.fLabel ? kRainbowB[4] :
      (x <= 0 || x >= cWeek-1 ? kRainbowB[1] : gi.kiLite));
    if (!gs.fAlt)
      DrawSz(sz, x1 + x*xunit + s*2, y1 + y*yunit + s*4,
        dtLeft | dtTop | dtScale);
    else
      DrawSz(sz, x1 + x*xunit + xunit/2,
        y1 + y*yunit + yunit/2 + gi.nScale, dtCent | dtScale);
    if (++x >= cWeek) {
      x = 0;
      y++;
    }
  }
}


/* Draw a graphical calendar on the screen for the chart month or entire */
/* year, as done when the -K or -Ky is combined with the -X switch.      */

void XChartCalendar()
{
  int xs, ys, xunit, yunit, x1, y1, x, y;

  if (!us.fCalendarYear) {
    DrawCalendar(Mon, 0, 0, gs.xWin, gs.yWin);
    return;
  }

  /* Determine the best sized rectangle of months to draw the year in based */
  /* on the chart dimensions: Either do 6x2 months, or 4x3, 3x4, or 2x6.    */

  if (gs.xWin > gs.yWin) {
    if (gs.xWin > gs.yWin * 3) {
      xs = 6; ys = 2;
    } else {
      xs = 4; ys = 3;
    }
  } else {
    if (gs.yWin > gs.xWin * 2) {
      xs = 2; ys = 6;
    } else {
      xs = 3; ys = 4;
    }
  }
  xunit = gs.xWin / xs; yunit = gs.yWin / ys;
  x1 = (gs.xWin - xunit*xs) / 2;
  y1 = (gs.yWin - yunit*ys) / 2;
  for (y = 0; y < ys; y++)
    for (x = 0; x < xs; x++) {
      DrawCalendar(y * xs + x + 1, x1 + x*xunit, y1 + y*yunit,
        x1 + (x+1)*xunit, y1 + (y+1)*yunit);
    }
}
#endif /* GRAPH */

/* xcharts1.c */
