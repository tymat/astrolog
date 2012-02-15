/*
** Astrolog (Version 5.41G) File: calc.c
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

real ObjPrime[oNorm+1];
real StarPrime[cStar+1];
real Longit = 0.0;
real Latit = 0.0;

/*
******************************************************************************
** House Cusp Calculations.
******************************************************************************
*/


/* This is a subprocedure of ComputeInHouses(). Given a zodiac position,  */
/* return which of the twelve houses it falls in. Remember that a special */
/* check has to be done for the house that spans 0 degrees Aries.         */

int HousePlaceIn(rDeg)
real rDeg;
{
  int i = 0;

  rDeg = Mod(rDeg + 0.5/60.0/60.0);

  if ((hRevers < 2) || !PolarMCflip) {

    do {
      i++;
    } while (!(i >= cSign ||
        (rDeg >= chouse[i] && rDeg < chouse[Mod12(i+1)]) ||
        (chouse[i] > chouse[Mod12(i+1)] &&
        (rDeg >= chouse[i] || rDeg < chouse[Mod12(i+1)]))));
    return i;

  } else {

    do {
      i++;
    } while (!(i >= cSign ||
        (rDeg <= chouse[i] && rDeg > chouse[Mod12(i+1)]) ||
        (chouse[i] < chouse[Mod12(i+1)] &&
        (rDeg <= chouse[i] || rDeg > chouse[Mod12(i+1)]))));
    return i;
  }
}


/* For each object in the chart, determine what house it belongs in. */

void ComputeInHouses()
{
  int i;

  for (i = 0; i <= cObj; i++)
    inhouse[i] = HousePlaceIn(planet[i]);
}


/* This house system is just like the Equal system except that we start */
/* our 12 equal segments from the Midheaven instead of the Ascendant.   */

void HouseEqualMidheaven()
{
  int i;

  if (hRevers) {
    is.Asc = Mod(is.Asc -rDegHalf);
    if (PolarMCflip) {
      hRevers = 2;
      is.MC = Mod(is.MC - rDegHalf);
      }
  }

  for (i = 1; i <= cSign; i++) {
    if (hRevers && PolarMCflip)
      chouse[i] = Mod(is.MC-90.0-30.0*(real)(i-1));
    else
      chouse[i] = Mod(is.MC-270.0+30.0*(real)(i-1));
  }
}


/* Compute the cusp positions using the Alcabitius house system. */

void HouseAlcabitius()
{
  real rDecl, rSda, rSna, r, hr, hr2;
  int i;

  if (hRevers) {
    is.Asc = Mod(is.Asc - rDegHalf);
    hRevers = 2;
  }

  rDecl = RAsin(RSin(is.OB) * RSinD(is.Asc));
  r = -RTan(AA) * RTan(rDecl);
  rSda = DFromR(RAcos(r));
  rSna = rDegHalf - rSda;
  chouse[sLib] = DFromR(is.RA) - rSna;
  chouse[sSco] = DFromR(is.RA) - rSna*2.0/3.0;
  chouse[sSag] = DFromR(is.RA) - rSna/3.0;
  chouse[sCap] = DFromR(is.RA);
  chouse[sAqu] = DFromR(is.RA) + rSda/3.0;
  chouse[sPis] = DFromR(is.RA) + rSda*2.0/3.0;

  for (i = sLib; i <= sPis; i++) {
    hr = RFromD(chouse[i]);
    hr2 = RAtn(RTan(hr)/RCos(is.OB));
    if (hr2 < 0.0)
      hr2 += rPi;
    if (RSin(hr) < 0.0)
      hr2 += rPi;
    chouse[i] = DFromR(hr2);
    chouse[i] = Mod(chouse[i]+is.rSid);
  }
  for (i = sAri; i <= sVir; i++)
    chouse[i] = Mod(chouse[i+6]+rDegHalf);

  if (hRevers && PolarMCflip) {
    is.MC = Mod(is.MC - rDegHalf);
    for (i = 2; i <= 6; i++) {
      r = chouse[i];
      chouse[i] = chouse[14-i];
      chouse[14-i] = r;
    }
  }
}


/* This is a new house system similar in philosophy to Porphyry houses.   */
/* Instead of just trisecting the difference in each quadrant, we do a    */
/* smooth sinusoidal distribution of the difference around all the cusps. */

void HousePorphyryNeo()
{
  real delta;
  int i;

  if (hRevers) {
    is.Asc = Mod(is.Asc - rDegHalf);
    hRevers = 2;
  }

  if (hRevers && PolarMCflip) {
    is.MC = Mod(is.MC -rDegHalf);
    delta = (rDegQuad - MinDistance(is.MC, is.Asc))/4.0;
    chouse[sLib] = Mod(is.Asc+rDegHalf); chouse[sCap] = is.MC;
    chouse[sAqu] = Mod(chouse[sCap] - 30.0 + delta   + is.rSid);
    chouse[sPis] = Mod(chouse[sAqu] - 30.0 + delta*2 + is.rSid);
    chouse[sSag] = Mod(chouse[sCap] + 30.0 + delta   + is.rSid);
    chouse[sSco] = Mod(chouse[sSag] + 30.0 + delta*2 + is.rSid);
    for (i = sAri; i < sLib; i++)
      chouse[i] = Mod(chouse[i+6]-rDegHalf);

  } else {

    delta = (MinDistance(is.MC, is.Asc) - rDegQuad)/4.0;
    chouse[sLib] = Mod(is.Asc+rDegHalf); chouse[sCap] = is.MC;
    chouse[sAqu] = Mod(chouse[sCap] + 30.0 + delta   + is.rSid);
    chouse[sPis] = Mod(chouse[sAqu] + 30.0 + delta*2 + is.rSid);
    chouse[sSag] = Mod(chouse[sCap] - 30.0 + delta   + is.rSid);
    chouse[sSco] = Mod(chouse[sSag] - 30.0 + delta*2 + is.rSid);
    for (i = sAri; i < sLib; i++)
      chouse[i] = Mod(chouse[i+6]-rDegHalf);
  }
}


/* The "Whole" house system is like the Equal system with 30 degree houses, */
/* where the 1st house starts at zero degrees of the sign of the Ascendant. */

void HouseWhole()
{
  int i;

  if (hRevers) {
    is.Asc = Mod(is.Asc -rDegHalf);
    if (PolarMCflip)
      is.MC = Mod(is.MC - rDegHalf);
  }

  for (i = 1; i <= cSign; i++)
    chouse[i] = Mod((real)((SFromZ(is.Asc)-1)*30) + ZFromS(i));
}


/* The "Vedic" house system is like the Equal system except each house      */
/* starts 15 degrees earlier. The Asc falls in the middle of the 1st house. */

void HouseVedic()
{
  int i;

  for (i = 1; i <= cSign; i++)
    chouse[i] = Mod(is.Asc - 15.0 + ZFromS(i));
}

/* In "null" houses, the cusps are always fixed to start at their cor-    */
/* responding sign, i.e. the 1st house is always at 0 degrees Aries, etc. */

void HouseNull()
{
  int i;

  if (hRevers) {
    is.Asc = Mod(is.Asc -rDegHalf);
    is.MC = Mod(is.MC -rDegHalf);
  }

  for (i = 1; i <= cSign; i++)
    chouse[i] = Mod(ZFromS(i));
}


/* Calculate the house cusp positions, using the specified algorithm. */

void ComputeHouses(housesystem)
int housesystem;
{
  byte sz[cchSzDef];

  if (RAbs(AA) > RFromD(rDegQuad-rAxis) && housesystem < 2) {
    sprintf(sz,
      "The %s system of houses is not defined at extreme latitudes.",
      szSystem[housesystem]);
    PrintWarning(sz);
    AA = RSgn2(AA)*RFromD(rDegQuad-rAxis);
  }

  /* Flip the Ascendant if it falls in the wrong half of the zodiac. */
  if (MinDifference(is.MC, is.Asc) < 0.0 && housesystem == 13)
    is.Asc = Mod(is.Asc + rDegHalf);

  switch (housesystem) {
  case  1: HouseKoch();           break;
  case  2: HouseEqual();          break;
  case  3: HouseCampanus();       break;
  case  4: HouseMeridian();       break;
  case  5: HouseRegiomontanus();  break;
  case  6: HousePorphyry();       break;
  case  7: HouseMorinus();        break;
  case  8: HouseTopocentric();    break;
  case  9: HouseAlcabitius();     break;
  case 10: HouseEqualMidheaven(); break;
  case 11: HousePorphyryNeo();    break;
  case 12: HouseWhole();          break;
  case 13: HouseVedic();          break;
  case 14: HouseNull();           break;
  default: HousePlacidus();
  }
}


/*
******************************************************************************
** Star Position Calculations.
******************************************************************************
*/

/* This is used by the chart calculation routine to calculate the positions */
/* of the fixed stars. Since the stars don't move in the sky over time,     */
/* getting their positions is mostly just reading info from an array and    */
/* converting it to the correct reference frame. However, we have to add    */
/* in the correct precession for the tropical zodiac, and sort the final    */
/* index list based on what order the stars are supposed to be printed in.  */

void 
ComputeStars(SD)
real SD;
{
  int i, j;
  real x, y, z;
  real lon, lat, lonMC, latMC, lonz, latz, azi, alt;

#ifdef PLACALC
  bool HaveStarFile;
  real t_plac, r1, r2;

  HaveStarFile = fTrue;
  if (us.fPlacalc) {
    t_plac = is.T * 36525.0 + 2415020.0;
    szFileNameSearch = "fixstars.ast";
    if (!FileFind("")) {
      HaveStarFile = fFalse;
      if (us.fSeconds)
        ErrorEphem(szFileNameSearch, -1);
    }
  }
#endif /* PLACALC */

  /* If need, prepare to calculate Prime Vertical of stars. */

  if (us.nStar == 'p' || PrimeRest > 0) {
    lon = RFromD(Mod(Longit)); lat = RFromD(Latit);
    if (us.fEquator) {
      lonMC = RFromD(planet[oMC]);
      latMC = RFromD(planetalt[oMC]);
    } else {
      lonMC = RFromD(Tropical(planet[oMC]));
      latMC = RFromD(planetalt[oMC]);
      EclToEqu(&lonMC, &latMC);
    }
    if (PolarMCflip && hRevers)
      lonMC = Mod(lonMC+rPi);
  }

  /* Read in star positions. */

  for (i = 1; i <= cStar; i++) {
    x = rStarData[i*6-6]; y = rStarData[i*6-5]; z = rStarData[i*6-4];
    planet[oNorm+i] = RFromD(x*rDegMax/24.0+y*15.0/60.0+z*0.25/60.0);
    x = rStarData[i*6-3]; y = rStarData[i*6-2]; z = rStarData[i*6-1];
    if (x < 0.0) {
      neg(y); neg(z);
    } else if (fabs(x) == 0.0 && y < 0.0) {
      neg(z);
    }
    planetalt[oNorm+i] = RFromD(x+y/60.0+z/60.0/60.0);
    /* Convert to ecliptic zodiac coordinates. */
    EquToEcl(&planet[oNorm+i], &planetalt[oNorm+i]);
    planet[oNorm+i] = Mod(DFromR(planet[oNorm+i])+rEpoch2000+SD);
    planetalt[oNorm+i] = DFromR(planetalt[oNorm+i]);

#ifdef PLACALC
    if (us.fPlacalc && HaveStarFile) {
      if (FPlacalcStar(i, t_plac, &r1, &r2)) {
        planet[oNorm+i]    = Mod(r1 + is.rSid);
        planetalt[oNorm+i] = r2;
      }
    }
#endif
    ret[oNorm+i] = RFromD(rDegMax/26000.0/365.25);
    altret[oNorm+i] = 0.0;
    starname[i] = i;

    /* If sorting by prime vertical needed, compute it. */

    if (us.nStar == 'p' || PrimeRest > 0) {
      if (us.fEquator) {
        lonz = RFromD(planet[oNorm+i]);
        latz = RFromD(planetalt[oNorm+i]);
      } else {
        lonz = RFromD(Tropical(planet[oNorm+i]));
        latz = RFromD(planetalt[oNorm+i]);
        EclToEqu(&lonz, &latz);
      }
      lonz = RFromD(Mod(DFromR(lonMC-lonz+lon)));
      lonz = RFromD(Mod(DFromR(lonz-lon+rPiHalf)));
      if (!fEquator)
        EquToLocal(&lonz, &latz, rPiHalf-lat);
      azi = rDegMax-DFromR(lonz); alt = DFromR(latz);
      azi = RFromD(azi); alt = RFromD(alt);
      CoorXform(&azi, &alt, rPiHalf);
      azi = DFromR(azi); alt = DFromR(alt);
      StarPrime[i] = azi;
    }
  }
  if (PrimeRest > 0) {
    for (i = 1; i <= oNorm; i++) {
      if (us.fEquator) {
        lonz = RFromD(planet[i]);
        latz = RFromD(planetalt[i]);
      } else {
        lonz = RFromD(Tropical(planet[i]));
        latz = RFromD(planetalt[i]);
        EclToEqu(&lonz, &latz);
      }
      lonz = RFromD(Mod(DFromR(lonMC-lonz+lon)));
      lonz = RFromD(Mod(DFromR(lonz-lon+rPiHalf)));
      if (!fEquator)
        EquToLocal(&lonz, &latz, rPiHalf-lat);
      azi = rDegMax-DFromR(lonz); alt = DFromR(latz);
      azi = RFromD(azi); alt = RFromD(alt);
      CoorXform(&azi, &alt, rPiHalf);
      azi = DFromR(azi); alt = DFromR(alt);
      ObjPrime[i] = azi;
    }
  }

  /* Sort the index list if -Uz, -Ul, -Un, or -Ub switch in effect. */

  if (us.nStar > 1) for (i = 2; i <= cStar; i++) {
    j = i-1;

    /* Compare star names for -Un switch. */

    if (us.nStar == 'n') while (j > 0 && NCompareSz(
      szObjName[oNorm+starname[j]], szObjName[oNorm+starname[j+1]]) > 0) {
      SwapN(starname[j], starname[j+1]);
      j--;

    /* Compare star brightnesses for -Ub switch. */

    } else if (us.nStar == 'b') while (j > 0 &&
      rStarBright[starname[j]] > rStarBright[starname[j+1]]) {
      SwapN(starname[j], starname[j+1]);
      j--;

    /* Compare star zodiac locations for -Uz switch. */

    } else if (us.nStar == 'z') while (j > 0 &&
      planet[oNorm+starname[j]] > planet[oNorm+starname[j+1]]) {
      SwapN(starname[j], starname[j+1]);
      j--;

    /* Compare star declinations for -Ul switch. */

    } else if (us.nStar == 'l') while (j > 0 &&
      planetalt[oNorm+starname[j]] < planetalt[oNorm+starname[j+1]]) {
      SwapN(starname[j], starname[j+1]);
      j--;

    /* Compare star prime vertical for -Up switch. */

    } else if (us.nStar == 'p') while (j > 0 &&
      StarPrime[starname[j]] < StarPrime[starname[j+1]]) {
      SwapN(starname[j], starname[j+1]);
      j--;
    }
  }
}


/*
******************************************************************************
** Chart Calculation.
******************************************************************************
*/

/* Given a zodiac degree, transform it into its Decan sign, where each    */
/* sign is trisected into the three signs of its element. For example,    */
/* 1 Aries -> 3 Aries, 10 Leo -> 0 Sagittarius, 25 Sagittarius -> 15 Leo. */

real Decan(deg)
real deg;
{
  int sign;
  real unit;

  sign = SFromZ(deg);
  unit = deg - ZFromS(sign);
  sign = Mod12(sign + 4*((int)RFloor(unit/10.0)));
  unit = (unit - RFloor(unit/10.0)*10.0)*3.0;
  return ZFromS(sign)+unit;
}


/* Given a zodiac degree, transform it into its Navamsa position, where   */
/* each sign is divided into ninths, which determines the number of signs */
/* after a base element sign to use. Degrees within signs are unaffected. */

real Navamsa(deg)
real deg;
{
  int sign, sign2;
  real unit;

  sign = SFromZ(deg);
  unit = deg - ZFromS(sign);
  sign2 = Mod12(((sign-1 & 3)^(2*(sign-1 & 1)))*3+(int)(unit*0.3)+1);
  return ZFromS(sign2)+unit;
}


/* Transform spherical to rectangular coordinates in x, y, z. */

void SphToRec(r, azi, alt, rx, ry, rz)
real r, azi, alt, *rx, *ry, *rz;
{
  real rT;

  *rz = r *RSinD(alt);
  rT  = r *RCosD(alt);
  *rx = rT*RCosD(azi);
  *ry = rT*RSinD(azi);
}


#ifdef PLACALC
/* Compute the positions of the planets at a certain time using the Placalc */
/* accurate formulas and ephemeris. This will supersede the Matrix routine  */
/* values and is only called with the -b switch is in effect. Not all       */
/* objects or modes are available using this, but some additional values    */
/* such as Moon and Node velocities not available without -b are. (This is  */
/* the one place in Astrolog which calls the Placalc package functions.)    */

void ComputePlacalc(t)
real t;
{
  int i;
  real r1, r2, r3, r4, r5;

  /* We can compute the positions of Sun through Pluto, Chiron, the four */
  /* asteroids, Lilith, and the (true or mean) North Node using Placalc. */
  /* The other objects must be done elsewhere.                           */

      /* Note, that with central planet, other than Earth, Lilith isn't */
      /* calculated at all and we use South Node instead.          V.A. */

  for (i = oSun; i < uranHi; i++) {
    if ((ignore[i] && i > oMoo) || FBetween(i, oFor, cuspHi))
      continue;
    if (FPlacalcPlanet(i, t*36525.0+2415020.0, us.objCenter != oEar,
      &r1, &r2, &r3, &r4, &r5)) {

      /* Note that this can't compute charts with central planets other */
      /* than the Sun or Earth or relative velocities in current state. */

      /* In this case we use previously calculated velocities, also  */
      /* Node coordiantes, which aren't computed  here.        V.A.  */ 

      if (us.objCenter == oEar || (i != oNod && i!= oLil)) {
        planet[i]    = Mod(r1 + is.rSid);
        planetalt[i] = r2;
        ret[i]       = RFromD(r3);
      }
      altret[i]    = RFromD(r5);

      /* Compute x,y,z coordinates from azimuth, altitude, and distance. */

      SphToRec(r4, planet[i], planetalt[i],
        &spacex[i], &spacey[i], &spacez[i]);
    }
  }

  /* If heliocentric, move Earth position to object slot zero. */

  i = us.objCenter != oEar;
  if (i) {
    planet[oEar] = planet[oSun];
    planetalt[oEar] = planetalt[oSun];
    ret[oEar] = ret[oSun];
    spacex[oEar] = spacex[oSun];
    spacey[oEar] = spacey[oSun];
    spacez[oEar] = spacez[oSun];
  }
  spacex[i] = spacey[i] = spacez[i] =
    planet[i] = planetalt[i] = ret[i] = 0.0;
  if (us.objCenter < oMoo)
    return;

  /* If other planet centered, shift all positions as in Matrix formulas. */

  for (i = 0; i <= oVes; i++) if (!FIgnore(i)) {
    spacex[i] -= spacex[us.objCenter];
    spacey[i] -= spacey[us.objCenter];
    spacez[i] -= spacez[us.objCenter];
    ProcessPlanet(i, 0.0);
  }
  spacex[us.objCenter] = spacey[us.objCenter] = spacez[us.objCenter] =
    planet[us.objCenter] = planetalt[us.objCenter] = ret[us.objCenter] = 0.0;
}
#endif


/* This is probably the main routine in all of Astrolog. It generates a   */
/* chart, calculating the positions of all the celestial bodies and house */
/* cusps, based on the current chart information, and saves them for use  */
/* by any of the display routines.                                        */

real CastChart(fDate)
bool fDate;
{
  CI ci;
  real housetemp[cSign+1], Off = 0.0, vtx, j, l, m, ll, kk, alt1, alt2;
  real SunDirPos, SunDirection;
  int i, k, n, ii, mi;
  int ignoreT[objMax], ignoreT3[objMax];

  /* Hack: Time zone +/-24 means to have the time of day be in Local Mean */
  /* Time (LMT). This is done by making the time zone value reflect the   */
  /* logical offset from GMT as indicated by the chart's longitude value. */

  if (RAbs(ZZ) == 24.0)
    ZZ = DegToDec(DecToDeg(OO)/15.0);

  if (us.fProgress && !us.nRel && !us.fTransitInf && !us.fTransit && !us.fInDay) {
        is.JDp = MdytszToJulian(Mon,Day,Yea,Tim,Dst,Zon);
  } 

  if (us.fProgress && us.fSolarArc == 2) {
        us.fSolarArc = 0;
        ci = ciCore;
        Off = ProcessInput(fDate);
        ComputeVariables(&vtx);
        if (us.fGeodetic)               /* Check for -G geodetic chart. */
          is.RA = RFromD(Mod(-OO));

        for (mi = 0; mi <= cObj; mi++) {
          ignoreT[mi] = ignore[mi];
          ignoreT3[mi] = ignore3[mi];
          ignore[mi] = fTrue;
          ignore3[mi] = fTrue;
        }
        ignore[oSun] = fFalse;
        ignore3[oSun] = fFalse;
        ComputePlanets();
#ifdef PLACALC
       if (us.fPlacalc)
         ComputePlacalc(is.T);
#endif
        SunDirPos = planet[oSun];
        for (mi = 0; mi <= cObj; mi++) {
          ignore[mi] = ignoreT[mi];
          ignore3[mi] = ignoreT3[mi];
        }
        ignore[oSun] = fFalse;
        ignore3[oSun] = fFalse;
        ciCore = ci;
        us.fSolarArc = 2;
  }

  ci = ciCore;
  Longit = RealCoord(ciCore.lon); Latit = RealCoord(ciCore.lat);

  if (MM == -1) {

    /* Hack: If month is negative, then we know chart was read in through a  */
    /* -o0 position file, so the planet positions are already in the arrays. */

    is.MC = planet[oMC]; is.Asc = planet[oAsc];
  } else {

    for (i = 0; i <= cObj; i++) {
      planet[i] = planetalt[i] = 0.0;    /* On ecliptic unless we say so.  */
      ret[i] = 1.0;                      /* Direct until we say otherwise. */
    }
    Off = ProcessInput(fDate);
    ComputeVariables(&vtx);
    if (us.fGeodetic)               /* Check for -G geodetic chart. */
      is.RA = RFromD(Mod(-OO));
    is.MC  = CuspMidheaven();       /* Calculate our Ascendant & Midheaven. */
    is.Asc = CuspAscendant();

    hRevers = 0;
    if (RAbs(AA) > RFromD(rDegQuad - rAxis)) {
       if (Mod(is.Asc - is.MC) > rDegHalf) {
          hRevers = 1;
       }
    }

    ComputeHouses(us.nHouseSystem); /* Go calculate house cusps. */

    /* Go calculate planet, Moon, and North Node positions. */

    ComputePlanets();
    if (!ignore[oMoo] || !ignore[oNod] || !ignore[oSou] || !ignore[oFor]) {
      ComputeLunar(&planet[oMoo], &planetalt[oMoo],
        &planet[oNod], &planetalt[oNod]);
      ret[oNod] = -1.0;
    }

    /* Compute more accurate ephemeris positions for certain objects. */

#ifdef PLACALC
    if (us.fPlacalc)
      ComputePlacalc(is.T);
#endif
    if (!us.fPlacalc || us.objCenter != oEar || forceSN) {
      planet[oSou] = Mod(planet[oNod]+rDegHalf);
      ret[oSou] = ret[oNod] = RFromD(-0.053);
      ret[oMoo] = RFromD(12.5);
    }

    /* Calculate position of Part of Fortune. */

    j = planet[oMoo]-planet[oSun];
    if (us.nArabicNight < 0 ||
      (us.nArabicNight == 0 && HousePlaceIn(planet[oSun]) < sLib))
      neg(j);
    j = RAbs(j) < rDegQuad ? j : j - RSgn(j)*rDegMax;
    planet[oFor] = Mod(j+is.Asc);

    /* Fill in "planet" positions corresponding to house cusps. */

    planet[oVtx] = vtx; planet[oEP] = CuspEastPoint();
    for (i = 1; i <= cSign; i++)
      planet[cuspLo + i - 1] = chouse[i];
    if (!us.fHouseAngle) {
      planet[oAsc] = is.Asc; planet[oMC] = is.MC;
      planet[oDes] = Mod(is.Asc + rDegHalf);
      planet[oNad] = Mod(is.MC + rDegHalf);
    }
    for (i = oFor; i <= cuspHi; i++)
      ret[i] = RFromD(rDegMax);
  }

  /* Go calculate star positions if -U switch in effect. */

  if (us.nStar)
    ComputeStars(us.fSidereal ? 0.0 : -Off);

  /* Transform ecliptic to equatorial coordinates if -sr in effect. */

  if (us.fEquator) {
    for (i = 0; i <= cObj; i++) if (!ignore[i]) {
      planet[i]    = RFromD(Tropical(planet[i]));
      planetalt[i] = RFromD(planetalt[i]);
      EclToEqu(&planet[i], &planetalt[i]);
      planet[i]    = DFromR(planet[i]);
      planetalt[i] = DFromR(planetalt[i]);
    }
    for (i = 1; i <= cSign; i++) {
      l = 0.0;
      chouse[i] = RFromD(Tropical(chouse[i]));
      EclToEqu(&chouse[i], &l);
      chouse[i] = DFromR(chouse[i]);
    }
       /* Recalculate equatorial position of Part of Fortune. */
/*
    j = planet[oMoo]-planet[oSun];
    if (us.nArabicNight < 0 ||
      (us.nArabicNight == 0 && HousePlaceIn(planet[oSun]) < sLib))
      neg(j);
    j = RAbs(j) < rDegQuad ? j : j - RSgn(j)*rDegMax;
    planet[oFor] = Mod(j+planet[oAsc]);
*/
  }

  /* Alternative star restrictions - by ecliptic and prime vertical */

  if (us.nStar) {
    if (StarRest && !PrimeRest) {
      for (i = starLo; i <= starHi; i++)
        ignore[i] = 1;

      if (!us.fParallel) {
        for (i = starLo; i <= starHi; i++) {
          for (k = 1; k <= uranHi; k++) if (!ignore[k]) {
            for (n = 1; n <= StarAspects; n++) {
              l = MinDistance(planet[i], planet[k]);
              m = l - rAspAngle[n];
              if (RAbs(m) < GetOrb(i,k,n))
                 ignore [i] = 0;
            }
          }
        }
      } else {
        for (i = starLo; i <= starHi; i++) {
          for (k = 1; k <= uranHi; k++) if (!ignore[k] && FThing(k)) {
            ll = RFromD(planet[k]); alt1 = RFromD(planetalt[k]);
            EclToEqu(&ll, &alt1); alt1 = DFromR(alt1);
            ll = RFromD(planet[i]); alt2 = RFromD(planetalt[i]);
            EclToEqu(&ll, &alt2); alt2 = DFromR(alt2);
            kk = RAbs(alt1 - alt2);
            if (kk < GetOrb(i, k, aCon))
              ignore[i] = 0;
            else if (StarAspects > 1) {
              kk = RAbs(alt2) - RAbs(alt1);
              if (RAbs(kk) < GetOrb(i, k, aCon))
                ignore[i] = 0;
            }
          }
        }
      }
    }
    if (PrimeRest) {
      for (i = 1; i <= cStar; i++) {
        ii = starLo+i-1;
        ignore[ii] = 1;
        if (PrimeRest != 2) {
          for (k = 1; k <= uranHi; k++) if (!ignore[k] && FThing(k)) {
            l = StarPrime[i] - ObjPrime[k];
            if (RAbs(l) < PrimeOrb1)
              ignore [ii] = 0;
          }
        }
        if (PrimeRest > 1) {
          for (k = 0; k <= 8; k++) {
            l = StarPrime[i] - k*45.0;
            if (RAbs(l) < PrimeOrb2)
              ignore [ii] = 0;
          }
        }
      }
    }
  }

  /* Now, we may have to modify the base positions we calculated above  */
  /* based on what type of chart we are generating.                     */

  /* Bugfix by VA. When "degree per day/month" charts are calculatd,    */
  /* is.JDp - is.JD gives wrong difference in time, because is.JD is    */
  /* calculated for midnigth of birthday (time is set to 00:00:00 GMT), */
  /* so the birthtime isn't used and time difference (in days) is off   */
  /* just by birthtime which gives error in positions up to 10 arcsec.  */
  /* When is.JD below is replaced by JulianDayFromTime(is.T) which is   */
  /* real Julian Day for birthtime, time difference is still off, this  */
  /* time exactly by 12 hours, because Julian Day starts from midday,   */
  /* not from midnight (as in case of is.JDp) and in subroutine         */
  /* ProcessInput() time is.T is shifted appropriately. (Just compare   */
  /* calculation of is.T in named routine with reverse caclulation      */
  /* JulianDayFromTime(is.T) and see this shift by 0.5 day). Therefore  */
  /* we have substract here the same 12 ours (or 0.5 days).             */

  if (us.fProgress && us.fSolarArc) {  /* Are we doing -p0 solar arc chart? */

    if (us.fSolarArc == 1) {
      for (i = 0; i <= cObj; i++)
/*        planet[i] = Mod(planet[i] + (is.JDp - is.JD) / us.rProgDay);  */
        planet[i] = Mod(planet[i] + (is.JDp - JulianDayFromTime(is.T) - 0.5) / us.rProgDay);
      for (i = 1; i <= cSign; i++)
/*        chouse[i]  = Mod(chouse[i]  + (is.JDp - is.JD) / us.rProgDay);  */
        chouse[i]  = Mod(chouse[i]  + (is.JDp - JulianDayFromTime(is.T) - 0.5) / us.rProgDay);
    }
    if (us.fSolarArc == 2) {
      SunDirection = SunDirPos - planet[oSun];
      for (i = 0; i <= cObj; i++)
        planet[i] = Mod(planet[i] + SunDirection);
      for (i = 1; i <= cSign; i++)
        chouse[i]  = Mod(chouse[i]  + SunDirection);

      for (mi = 0; mi <= cObj; mi++) {
        ignore[mi] = ignoreT[mi];
        ignore3[mi] = ignoreT3[mi];
      }
    }
  }
  if (us.nHarmonic > 1)            /* Are we doing a -x harmonic chart?     */
    for (i = 0; i <= cObj; i++)
      planet[i] = Mod(planet[i] * (real)us.nHarmonic);
  if (us.objOnAsc) {
    if (us.objOnAsc > 0)           /* Is -1 put on Ascendant in effect?     */
      j = planet[us.objOnAsc]-is.Asc;
    else                           /* Or -2 put object on Midheaven switch? */
      j = planet[-us.objOnAsc]-is.MC;
    for (i = 1; i <= cSign; i++)   /* If so, rotate the houses accordingly. */
      chouse[i] = Mod(chouse[i]+j);
  }

  /* Check to see if we are -F forcing any objects to be particular values. */

  for (i = 0; i <= cObj; i++)
    if (force[i] != 0.0) {
      planet[i] = force[i]-rDegMax;
      planetalt[i] = ret[i] = 0.0;
    }

  ComputeInHouses();        /* Figure out what house everything falls in. */

  /* If -f domal chart switch in effect, switch planet and house positions. */

  if (us.fFlip) {
    for (i = 0; i <= cObj; i++) {
      k = inhouse[i];
      inhouse[i] = SFromZ(planet[i]);
      planet[i] = ZFromS(k)+MinDistance(chouse[k], planet[i]) /
        MinDistance(chouse[k], chouse[Mod12(k+1)])*30.0;
    }
    for (i = 1; i <= cSign; i++) {
      k = HousePlaceIn(ZFromS(i));
      housetemp[i] = ZFromS(k)+MinDistance(chouse[k], ZFromS(i)) /
        MinDistance(chouse[k], chouse[Mod12(k+1)])*30.0;
    }
    for (i = 1; i <= cSign; i++)
      chouse[i] = housetemp[i];
  }

  /* If -3 decan chart switch in effect, edit planet positions accordingly. */

  if (us.fDecan) {
    for (i = 0; i <= cObj; i++)
      planet[i] = Decan(planet[i]);
    ComputeInHouses();
  }

  /* If -9 navamsa chart switch in effect, edit positions accordingly. */

  if (us.fNavamsa) {
    for (i = 0; i <= cObj; i++)
      planet[i] = Navamsa(planet[i]);
    ComputeInHouses();
  }

  ciCore = ci;
  return is.T;
}


/* Calculate the position of each planet with respect to the Gauquelin      */
/* sectors. This is used by the sector charts. Fill out the planet position */
/* array where one degree means 1/10 the way across one of the 36 sectors.  */

void CastSectors()
{
  int source[MAXINDAY], type[MAXINDAY], occurcount, division, div,
    i, j, s1, s2, ihouse, fT;
  real time[MAXINDAY], rgalt1[objMax], rgalt2[objMax],
    azi1, azi2, alt1, alt2, lon, lat, mc1, mc2, d, k;
  byte EquT, MCpolarT;

  /* If the -l0 approximate sectors flag is set, we can quickly get rough   */
  /* positions by having each position be the location of the planet as     */
  /* mapped into Placidus houses. The -f flip houses flag does this for us. */

  if (us.fSectorApprox) {
    ihouse = us.nHouseSystem; us.nHouseSystem = 0;
    not(us.fFlip);
    CastChart(fTrue);
    not(us.fFlip);
    us.nHouseSystem = ihouse;
    return;
  }

  /* If not approximating sectors, then they need to be computed the formal */
  /* way: based on a planet's nearest rising and setting times. The code    */
  /* below is similar to ChartInDayHorizon() accessed by the -Zd switch.    */

  fT = us.fSidereal; us.fSidereal = fFalse;
  EquT = us.fEquator; us.fEquator = fFalse;
  MCpolarT = PolarMCflip; PolarMCflip = fFalse;
  lon = RFromD(Mod(Lon)); lat = RFromD(Lat);
  division = us.nDivision * 4;
  occurcount = 0;

  /* Start scanning from 18 hours before to 18 hours after the time of the */
  /* chart in question, to find the closest rising and setting times.      */

  ciCore = ciMain; ciCore.tim -= 18.0;
  if (ciCore.tim < 0.0) {
    ciCore.tim += 24.0;
    ciCore.day--;
  }
  CastChart(fTrue);
  mc2 = RFromD(planet[oMC]); k = RFromD(planetalt[oMC]);
  EclToEqu(&mc2, &k);
  cp2 = cp0;
  for (i = 1; i <= cObj; i++) {
    rgalt2[i] = planetalt[i];
  }

  /* Loop through 36 hours, dividing it into a certain number of segments. */
  /* For each segment we get the planet positions at its endpoints.        */

  for (div = 1; div <= division; div++) {
    ciCore = ciMain;
    ciCore.tim = DecToDeg(ciCore.tim) - 18.0 + 36.0*(real)div/(real)division;
    if (ciCore.tim < 0.0) {
      ciCore.tim += 24.0;
      ciCore.day--;
    } else if (ciCore.tim >= 24.0) {
      ciCore.tim -= 24.0;
      ciCore.day++;
    }
    ciCore.tim = DegToDec(ciCore.tim);
    CastChart(fTrue);
    mc1 = mc2;
    mc2 = RFromD(planet[oMC]); k = RFromD(planetalt[oMC]);
    EclToEqu(&mc2, &k);
    cp1 = cp2; cp2 = cp0;
    for (i = 1; i <= cObj; i++) {
      rgalt1[i] = rgalt2[i]; rgalt2[i] = planetalt[i];
    }

    /* During our segment, check to see if each planet rises or sets. */

    for (i = 0; i <= cObj; i++) if (!FIgnore(i) && FThing(i)) {
      EclToHorizon(&azi1, &alt1, cp1.obj[i], rgalt1[i], lon, lat, mc1);
      EclToHorizon(&azi2, &alt2, cp2.obj[i], rgalt2[i], lon, lat, mc2);
      j = 0;
      if ((alt1 > 0.0) != (alt2 > 0.0)) {
        d = RAbs(alt1)/(RAbs(alt1)+RAbs(alt2));
        k = Mod(azi1 + d*MinDifference(azi1, azi2));
        j = 1 + (MinDistance(k, rDegHalf) < rDegQuad);
      }
      if (j && occurcount < MAXINDAY) {
        source[occurcount] = i;
        type[occurcount] = j;
        time[occurcount] = 36.0*((real)(div-1)+d)/(real)division*60.0;
        occurcount++;
      }
    }
  }

  /* Sort each event in order of time when it happens during the day. */

  for (i = 1; i < occurcount; i++) {
    j = i-1;
    while (j >= 0 && time[j] > time[j+1]) {
      SwapN(source[j], source[j+1]);
      SwapN(type[j], type[j+1]);
      SwapR(&time[j], &time[j+1]);
      j--;
    }
  }

  /* Now fill out the planet array with the appropriate sector location. */

  for (i = 1; i <= cObj; i++) if (!ignore[i] && FThing(i)) {
    planet[i] = 0.0;
    /* Search for the first rising or setting event of our planet. */
    for (s2 = 0; s2 < occurcount && source[s2] != i; s2++)
      ;
    if (s2 == occurcount)
      {
LFail:
      /* If we failed to find a rising/setting bracket around our time, */
      /* automatically restrict that planet so it doesn't show up.      */
      ignore[i] = fTrue;
      continue;
      }
LRetry:
    /* One rising or setting event was found. Now search for the next one. */
    s1 = s2;
    for (s2 = s1 + 1; s2 < occurcount && source[s2] != i; s2++)
      ;
    if (s2 == occurcount)
      goto LFail;
    /* Reject the two events if either (1) they're both the same, i.e. both */
    /* rising or both setting, or (2) they don't bracket the chart's time.  */
    if (type[s2] == type[s1] || time[s1] > 18.0*60.0 || time[s2] < 18.0*60.0)
      goto LRetry;
    /* Cool, we've found our rising/setting bracket. The sector position is */
    /* the proportion the chart time is between the two event times.        */
    planet[i] = (18.0*60.0 - time[s1])/(time[s2] - time[s1])*rDegHalf;
    if (type[s1] == 2)
      planet[i] += rDegHalf;
    planet[i] = Mod(rDegMax - planet[i]);
  }

  /* Save Gauquelin's cp0 to cp2 for graphics infoborder.  */

  cp2 = cp0;

  /* Restore original chart info as we've overwritten it. */

  ciCore = ciMain;
  us.fSidereal = fT;
  us.fEquator = EquT;
  PolarMCflip = MCpolarT;
}


/*
******************************************************************************
** Aspect Calculations.
******************************************************************************
*/

/* Set up the aspect/midpoint grid. Allocate memory for this array, if not */
/* already done. Allocation is only done once, first time this is called.  */

bool FEnsureGrid()
{
  if (grid != NULL)
    return fTrue;
  grid = (GridInfo FPTR *)PAllocate(sizeof(GridInfo), fFalse, "grid");
  return grid != NULL;
}


/* Indicate whether some aspect between two objects should be shown. */

bool FAcceptAspect(obj1, asp, obj2)
int obj1, asp, obj2;
{
  int fSupp;

  if (ignorea(asp))    /* If the aspect restricted, reject immediately. */
    return fFalse;

  if (us.nRel > rcDual && FCusp(obj1) && FCusp(obj2))
    return fFalse;

  if (FStar(obj1) && FStar(obj2))
    return fFalse;

  if (us.fSmartCusp) {

    /* Allow only conjunctions to the minor house cusps. */

    if ((FMinor(obj1) || FMinor(obj2)) && asp > aCon)
      return fFalse;

    /* Prevent any simultaneous aspects to opposing angle cusps,     */
    /* e.g. if conjunct one, don't be opposite the other; if trine   */
    /* one, don't sextile the other; don't square both at once, etc. */

    fSupp = (asp == aOpp || asp == aSex || asp == aSSx || asp == aSes);
    if ((FAngle(obj1) || FAngle(obj2)) &&
      (fSupp || (asp == aSqu &&
      (obj1 == oDes || obj2 == oDes || obj1 == oNad || obj2 == oNad))))
      return fFalse;

    /* Prevent any simultaneous aspects to the North and South Node. */

    if (fSouthNode) {
      if (((obj1 == oNod || obj2 == oNod) && fSupp) ||
        ((obj1 == oSou || obj2 == oSou) && (fSupp || asp == aSqu)))
        return fFalse;
    }
  }
  return fTrue;
}


/* This is a subprocedure of FCreateGrid() and FCreateGridRelation().   */
/* Given two planets, determine what aspect, if any, is present between */
/* them, and save the aspect name and orb in the specified grid cell.   */

void GetAspect(planet1, planet2, ret1, ret2, i, j)
real *planet1, *planet2, *ret1, *ret2;
int i, j;
{
  int k;
  real l, m;

  grid->v[i][j] = grid->n[i][j] = 0;
  l = MinDistance(planet2[i], planet1[j]);
  for (k = us.nAsp; k >= 1; k--) {
    if (!FAcceptAspect(i, k, j))
      continue;
    m = l-rAspAngle[k];
    if (RAbs(m) < GetOrb(i, j, k)) {
      grid->n[i][j] = k;

      /* If -ga switch in effect, then change the sign of the orb to    */
      /* correspond to whether the aspect is applying or separating.    */
      /* To do this, we check the velocity vectors to see if the        */
      /* planets are moving toward, away, or are overtaking each other. */

      if (us.fAppSep)
        m = RSgn2(ret1[j]-ret2[i])*
          RSgn2(MinDifference(planet2[i], planet1[j]))*RSgn2(m)*RAbs(m);
      grid->v[i][j] = (int)(m*60.0);
    }
  }
}


/* Very similar to GetAspect(), this determines if there is a parallel or */
/* contraparallel aspect between the given two planets, and stores the    */
/* result as above. The settings and orbs for conjunction are used for    */
/* parallel and those for opposition are used for contraparallel.         */

void GetParallel(planet1, planet2, planetalt1, planetalt2,
     ret1, ret2, altret1, altret2, i, j)
real *planet1, *planet2, *planetalt1, *planetalt2, *ret1, *ret2,
     *altret1, *altret2;
int i, j;
{
  int k;
  real l, alt1, alt2, latdiri, latdirj;

  l = RFromD(planet1[j]); alt1 = RFromD(planetalt1[j]);
  EclToEqu(&l, &alt1); alt1 = DFromR(alt1);
  l = RFromD(planet2[i]); alt2 = RFromD(planetalt2[i]);
  EclToEqu(&l, &alt2); alt2 = DFromR(alt2);
  grid->v[i][j] = grid->n[i][j] = 0;
  for (k = Min(us.nAsp, aOpp); k >= 1; k--) {
    if (!FAcceptAspect(i, k, j))
      continue;
    l = (k == aCon ? RAbs(alt1 - alt2) : RAbs(alt2) - RAbs(alt1));
    if (RAbs(l) < GetOrb(i, j, k)) {
      grid->n[i][j] = k;
#ifdef PLACALC
      if (us.fAppSep && us.fPlacalc) {
        latdiri = ret2[i]*RCosD(planet2[i])*RSinD(rAxis)
                + altret2[i]*RCosD(rAxis - RAbs(planetalt2[i] - alt2));
        latdirj = ret1[j]*RCosD(planet1[j])*RSinD(rAxis)
                + altret1[j]*RCosD(rAxis - RAbs(planetalt1[j] - alt1));
        l = RSgn2(latdirj - latdiri) * RSgn2(alt1 - alt2) * RSgn2(l) * RAbs(l);
      }
#endif                                       /*  PLACALC  */
      grid->v[i][j] = (int)(l*60.0);
    }
  }
}


/* Fill in the aspect grid based on the aspects taking place among the */
/* planets in the present chart. Also fill in the midpoint grid.       */

bool FCreateGrid(fFlip)
bool fFlip;
{
  int i, j, k;
  real l;

  if (!FEnsureGrid())
    return fFalse;
  for (j = 0; j <= cObj; j++) if (!FIgnore(j))
    for (i = 0; i <= cObj; i++) if (!FIgnore(i))

      /* The parameter 'flip' determines what half of the grid is filled in */
      /* with the aspects and what half is filled in with the midpoints.    */

      if (fFlip ? i > j : i < j) {
        if (us.fParallel)
          GetParallel(planet, planet, planetalt, planetalt, ret, ret, altret, altret, i, j);
        else
          GetAspect(planet, planet, ret, ret, i, j);
      } else if (fFlip ? i < j : i > j) {
        l = Mod(Midpoint(planet[i], planet[j])); k = (int)l;  /* Calculate */
        grid->n[i][j] = k/30+1;                               /* midpoint. */
        grid->v[i][j] = (int)((l-(real)(k/30)*30.0)*60.0);
      } else {
        grid->n[i][j] = SFromZ(planet[j]);
        grid->v[i][j] = (int)(planet[j]-(real)(grid->n[i][j]-1)*30.0);
      }
  return fTrue;
}


/* This is similar to the previous function; however, this time fill in the */
/* grid based on the aspects (or midpoints if 'acc' set) taking place among */
/* the planets in two different charts, as in the -g -r0 combination.       */

bool FCreateGridRelation(fMidpoint)
bool fMidpoint;
{
  int i, j, k;
  real l;

  if (!FEnsureGrid())
    return fFalse;
  for (j = 0; j <= cObj; j++) if (!FIgnore(j) || !FIgnore2(j) || !FIgnore3(j))
    for (i = 0; i <= cObj; i++) if (!FIgnore(i) || !FIgnore2(i) || !FIgnore3(i))
      if (!fMidpoint) {
        if (us.fParallel)
          GetParallel(cp1.obj, cp2.obj, cp1.alt, cp2.alt, cp1.dir, cp2.dir, cp1.altdir, cp2.altdir, i, j);
        else
          GetAspect(cp1.obj, cp2.obj, cp1.dir, cp2.dir, i, j);
      } else {
        l = Mod(Midpoint(cp2.obj[i], cp1.obj[j])); k = (int)l; /* Calculate */
        grid->n[i][j] = k/30+1;                                /* midpoint. */
        grid->v[i][j] = (int)((l-(real)(k/30)*30.0)*60.0);
      }
  return fTrue;
}


/*
******************************************************************************
** Other Calculations.
******************************************************************************
*/

/* Fill out tables based on the number of unrestricted planets in signs by  */
/* element, signs by mode, as well as other values such as the number of    */
/* objects in yang vs. yin signs, in various house hemispheres (north/south */
/* and east/west), and the number in first six signs vs. second six signs.  */
/* This is used by the -v chart listing and the sidebar in graphics charts. */

void CreateElemTable(pet)
ET *pet;
{
  int i, s;

  ClearB((lpbyte)pet, (int)sizeof(ET));
  for (i = 0; i <= cObj; i++) if (!FIgnore(i)) {
    pet->coSum++;
    s = SFromZ(planet[i]);
    pet->coSign[s-1]++;
    pet->coElemMode[(s-1)&3][(s-1)%3]++;
    pet->coElem[(s-1)&3]++; pet->coMode[(s-1)%3]++;
    pet->coYang += (s & 1);
    pet->coLearn += (s < sLib);
    if (!FCusp(i)) {
      pet->coHemi++;
      s = inhouse[i];
      pet->coHouse[s-1]++;
      pet->coModeH[(s-1)%3]++;
      pet->coMC += (s >= sLib);
      pet->coAsc += (s < sCan || s >= sCap);
    }
  }
  pet->coYin   = pet->coSum  - pet->coYang;
  pet->coShare = pet->coSum  - pet->coLearn;
  pet->coDes   = pet->coHemi - pet->coAsc;
  pet->coIC    = pet->coHemi - pet->coMC;
}

/* Added by V.Abramov. Recalculates CI longitude and latitude  */
/* to correct decimal form. That's bugfix - two first decimal  */
/* digits in longitude and latitude in CI are just value of    */
/* minutes in coordinates. 30deg30' latitude means 30.5 degree */
/* but is saved in form 30.30, and even more, transformed to   */
/* rad. So decimal part of latitude and longitude can't be     */
/* used directly for caclulations.                             */

real RealCoord(icoord)
real icoord;
{
  real coord, cfloor, cfract;
  int lon, neg;
  coord = icoord;
  lon = 0; neg = 0;
  if (coord > 180.0) {
    coord = coord - 360.0;
    lon = 1;
  }
  if (coord < 0.0) {
    coord = -coord;
    neg = 1;
  }
  cfloor = RFloor(coord);
  cfract = RFract(coord);
  cfract = cfract / 0.6;
  coord = cfloor + cfract;
  if (neg)
    coord = -coord;
  if (lon)
    coord = 360.0 - coord;
  return coord;
}

/* calc.c */
