/*
** Astrolog (Version 5.41G) File: intrpalt.c
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
*/

/***********************************************************************/
/* This module is created by Valentin Abramov (valja@tartu.customs.ee) */
/* and includes routines for alternative interpretation of Astrolog    */
/* natal charts and natal-transit comparison charts.                   */
/***********************************************************************/ 

#include "astrolog.h"

#ifdef INTERPRET
#ifdef INTERPRETALT

#include <string.h>

byte * szHouseFile P((int));
byte * szPlanetFile P((int, bool));
byte * szAspectFile P((int, bool));
void FieldWordI P((byte *));
bool FProcessInterpretFile P((byte *, FILE *, int, int));

byte SaveStars[objMax] = {0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0,                /* Planets  */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                /* Minors   */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,          /* Cusps    */
  0, 0, 0, 0, 0, 0, 0, 0, 0,                   /* Uranians */
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,     /* Stars    */
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1};


/* Three simple routines, which builds names of interpretation files. */
/* That saves memory, wich would be needed to describe hundreds file  */
/* names. There are four different files for every planet (position   */
/* and aspects, same for transits), also house files.                 */

byte *szHouseFile(i)
int i;
{
  byte szFileName[16], szFileNumber[8];

  sprintf (szFileNumber, "%i", i);
  sprintf (szFileName, "house");
  strcat (szFileName, szFileNumber);
  strcat (szFileName, ".dat");
  return szFileName;
}


byte *szPlanetFile(i, trflag)
int i;
bool trflag;
{
  byte szFileName[16], szFileNumber[8];

  sprintf (szFileNumber, "%i", i);
  if (i < 10)
    sprintf (szFileName, "pl0");
  else
    sprintf (szFileName, "pl");
  strcat (szFileName, szFileNumber);
  if (trflag)
    strcat (szFileName, "t.dat");
  else
    strcat (szFileName, ".dat");
  return szFileName;
}

byte *szAspectFile(i, trflag)
int i;
bool trflag;
{
  byte szFileName[16], szFileNumber[8];

  sprintf (szFileNumber, "%i", i);
  if (i < 10)
    sprintf (szFileName, "pl0");
  else
    sprintf (szFileName, "pl");
  strcat (szFileName, szFileNumber);
  if (trflag)
    strcat (szFileName, "ta.dat");
  else
    strcat (szFileName, "a.dat");
  return szFileName;
}

void FieldWordI(sz)
byte *sz;
{
  static byte line[cchSzMax];
  static int cursor = 0;
  int i, j;

  if (sz == NULL) {
    line[cursor] = 0;
    PrintSz(line); PrintL();
    cursor = 0;
    return;
  }
  if (cursor)
    line[cursor++] = ' ';
  for (i = 0; (line[cursor] = sz[i]); i++, cursor++)
    ;

  while (cursor >= us.nScreenWidth-1) {
    for (i = us.nScreenWidth-1; line[i] != ' '; i--)
      ;
    line[i] = 0;
    PrintSz(line); PrintL();
    for (j = 0; (line[j] = line[i+j+1]) != 0; j++)
      ;
    cursor -= (i+1);
  }
}

/* This is generic file processing routine, which handles chart          */
/* interpretation files. Given a file name or a file handle and two      */
/* indexes of necessary interpretation part, run through interpretation  */
/* data file and print out current part of interpretation                */

bool FProcessInterpretFile(szFile, file, j, k)
byte *szFile;
FILE *file;
int j, k;
{
  byte szLine[cchSzMax], ch, chLocal[2];
  int i, iind;
#ifndef WIN
  char chh;
#endif

  if (file == NULL)
    file = FileOpen(szFile, 0);
  if (file == NULL)
    return fFalse;

  /* All files have to begin with the -@ switch file type identifier. */
  ch = (byte)getc(file); ungetc(ch, file);
  if (ch != '@') {
    sprintf(szLine,
      "The command file '%s' is not in any valid format (character %d).",
      szFile, (int)ch);
    PrintWarning(szLine);
    fclose(file);
    return fFalse;
  }

/* Look for indexes of interpretation we need.  */ 
/* Every pair of indexes begin at '&'           */

  loop {
#ifdef WIN
    while (!feof(file) && (ch = (byte)getc(file)) != '&')
#else
    while (!feof(file) && (chh = getc(file)) != '&')
#endif
    ;
    if (feof(file)){
      fclose(file);
      return fTrue;
    }
    chLocal[0] = (byte)getc(file);
    chLocal[1] = (byte)getc(file);
    iind = atoi(chLocal);
    if (iind == j) {               /* Found 1st index */
#ifdef WIN
      while (!feof(file) && (ch = (byte)getc(file)) <= ' ')
#else
      while (!feof(file) && (chh = getc(file)) <= ' ')
#endif
      ;
      if (feof(file)){
        fclose(file);
        return fTrue;
      }
#ifdef WIN
      chLocal[0] = ch;
#else
      chLocal[0] = (byte)chh;
#endif
      chLocal[1] = (byte)getc(file);
      iind = atoi(chLocal);
      if (iind == k){              /* Found 2nd index */
        while (!feof(file) && (ch = (byte)getc(file)) < ' ')
        ;
        if (feof(file) || (ch == '&')){
          fclose(file);
          return fTrue;
        }
        ungetc(ch, file);
        break;
      }
    }
  }

/* Print out current inpretation part up to index of next part  "&"  */

  loop {
    while (!feof(file) && (ch = (byte)getc(file)) <= '\n')
      ;
    if (feof(file)) {
      FieldWordI(NULL);
      break;
    }
    for (szLine[0] = ch, i = 1; i < cchSzMax && !feof(file) &&
      (szLine[i] = (byte)getc(file)) > '\n' && szLine[i] != '&' ; i++)
      ;
    if (szLine[0] == '&') {
      szLine[0] = chNull;
      FieldWordI(NULL);
      break;
    }
    if (szLine[i] == '&') {
      szLine[i] = chNull;
      FieldWordI(szLine);
      FieldWordI(NULL);
      break;
    }
    if (szLine[i] <= '\n') {
      szLine[i] = chNull;
     FieldWordI(szLine);
    }
  }
  fclose(file);
  return fTrue;
}


/*
***************************************************************************
** Natal Chart interpretation routine
***************************************************************************
*/

void NatalInterpret()
{
  byte sz[cchSzDef], c;
  int i, j, k, l, m, n, jj, kk, SaveStarFlag, DispSS, DispSH;
  real ip, jp, p;

  SaveStarFlag = us.nStar;
  for(i = starLo; i <= starHi; i++)
     SaveStars[i] = ignore[i];

  if(!us.fInfluence) {
    NoPrint = 1;
    ChartInfluence();
    NoPrint = 0;
  }
      
  us.fAppSep = fTrue;
  us.nStar = SaveStarFlag;
  if (!FCreateGrid(fFalse))
    return;

  SaveStarFlag = us.nStar;  us.nStar = fFalse;

/*****************************************************************************/
/* PART 1: Planet's positions and aspects.                                   */
/* For every object print position and calculated powers: position and total */
/* and meaning of current object, aspects of current object with others and  */
/* interpretataion, also interpretation of object in sign and House.         */
/*****************************************************************************/

if (InterpretAlt != 4 && InterpretAlt != 6) {
    sprintf(sz, "   ****** PLANETS AND ASPECTS ******"); PrintSz(sz);
    PrintL2();

  for (i = 1; i <= cObjInt; i++) {
    if (ignore[i])
      continue;

    AnsiColor(kObjA[i]);
    l = SFromZ(planet[i]); c = Dignify(i, l);
    sprintf(sz, "%s%s%s%s in %s", ret[i] < 0.0 ? "Retrograde " : "",
      i == oNod ? "North " : (i == oFor ? "Part of " : ""), szObjName[i],
      i == 13 ? " Athena" : "", szSignName[l]);
      PrintSz(sz);
    if (i < cuspLo || i> cuspHi){
      sprintf(sz, " and %d%s House.", inhouse[i], szSuffix[inhouse[i]]);
      PrintSz(sz);
    }
      PrintL();
    sprintf(sz, "Powers -   position:%6.2f   total:%6.2f",ppower1[i], ppowerTotal[i]);
    PrintSz(sz);
    PrintL();

/*        PRINT OUT description of planet 'i'   */
    if(InterpretAlt != 1) {
      AnsiColor(kDefault);
      FProcessInterpretFile(szPlanetFile(i, fFalse), NULL, 0, 0);
    }

/* Print aspects of planets with orb and aspect power      */

    for (j = 1; j <= cObjInt; j++) {
      if (ignore[j] || i == j)
        continue;
    if (i > j) {
       k = grid->n[j][i];
       m = grid->v[j][i];
    } else {
       k = grid->n[i][j];
       m = grid->v[i][j];
    }
    if (k < 1 || k > cAspectInt)
      continue;

    ip = i <= oNorm ? ppower1[i] : 2.5;
    jp = j <= oNorm ? ppower1[j] : 2.5;
    p = (rAspInf[k]*(sqrt)(ip*jp)*
        (1.0-RAbs((real)(m))/60.0/GetOrb(i,j,k))*500.0);

    PrintAspect(i, SFromZ(planet[i]), (int)RSgn(ret[i]), k,
      j, SFromZ(planet[j]), (int)RSgn(ret[j]), 'a');
    sprintf(sz, " - orb: %c%d%c%02d'",
      us.fAppSep ? (m < 0 ? 'a' : 's') : (m < 0 ? '-' : '+'),
      abs(m)/60, chDeg1, abs(m)%60); PrintSz(sz);
    AnsiColor(kDkGreen);
    sprintf(sz, " - power:%6.2f\n", p/500.0); PrintSz(sz);

    if(InterpretAlt != 1) {
      AnsiColor(kDefault);
/*      PRINT INTERPRETATION OF CURRENT ASPECT           */
/*      first planet "i", second one "j",  aspect "k"    */

      if (i < j)
         FProcessInterpretFile(szAspectFile(i, fFalse), NULL, j, k);
      else
         FProcessInterpretFile(szAspectFile(j, fFalse), NULL, i, k);
      }
    }
    if (InterpretAlt != 1) {
    AnsiColor(kSignA(l));
    sprintf(sz, "%s%s%s%s in %s:\n", ret[i] < 0.0 ? "Retrograde " : "",
      i == oNod ? "North " : (i == oFor ? "Part of " : ""), szObjName[i],
      i == 13 ? " Athena" : "", szSignName[l]); PrintSz(sz);
/*     PRINT INTERPRETATION OF PLANET IN SIGN */
/*     planet "i", sign "l"                   */
      AnsiColor(kDefault);
      FProcessInterpretFile(szPlanetFile(i, fFalse), NULL, 1, l);
    }

    if (i < cuspLo || i > cuspHi) {
    if (InterpretAlt != 1) {
    AnsiColor(kSignA(inhouse[i]));
    sprintf(sz, "%s%s%s%s in %d%s House:\n", ret[i] < 0.0 ? "Retrograde " : "",
      i == oNod ? "North " : (i == oFor ? "Part of " : ""), szObjName[i],
      i == 13 ? " Athena" : "", inhouse[i], szSuffix[inhouse[i]]);
    PrintSz(sz);
/*     PRINT INTERPRETATION OF PLANET IN HOUSE */
/*     planet "i" , house "inhouse[i]"         */

      AnsiColor(kDefault);
      FProcessInterpretFile(szPlanetFile(i, fFalse), NULL, 2, inhouse[i]);
      }
    }
    PrintL();
  }
PrintL();
}

/**************************************************************************/
/*  PART 2: Houses                                                        */
/*  For every house print out relative power, meaning of house, position  */
/*  of house cusp with interpretation, position of situative dispositor   */
/*  in sign in house with interpretation, also position of natural        */
/*  dispositor with interpretations.                                      */
/**************************************************************************/

if (InterpretAlt != 3) {

  AnsiColor(kWhite);
  sprintf(sz, "   ****** HOUSES ******"); PrintSz(sz);
  PrintL2();

  for (n = 1; n <= cSign; n++) {
    AnsiColor(kSignA(n));
    sprintf(sz,"%d%s House:\n", n, szSuffix[n]);
    PrintSz(sz);

    if(InterpretAlt >= 2) {
      AnsiColor(kDefault);
      FProcessInterpretFile(szHouseFile(n), NULL, 0, 0);
    }

    l = SFromZ(chouse[n]);
    AnsiColor(kSignA(l));
    sprintf(sz, "House Cusp. in %s\n", szSignName[l]);
    PrintSz(sz);

    m = rules[SFromZ(chouse[n])];
        AnsiColor(kObjA[m]);
    if (m == (int) rules[n])
      sprintf(sz, "Natural and Situative Dispositor %s, total power %6.2f:\n",
          szObjName[m], ppowerTotal[m]);
    else
      sprintf(sz, "Situative Dispositor %s, total power %6.2f:\n",
          szObjName[m], ppowerTotal[m]);
      PrintSz(sz);

    if(InterpretAlt >= 2) {
      AnsiColor(kDefault);
      FProcessInterpretFile(szHouseFile(n), NULL, 1, l);
    }

/* Meanings of placement of dispositors in signs and houses.  */

    if((InterpretAlt == 1) || (InterpretAlt >= 5)) {
      l = SFromZ(planet[m]);
      DispSS = l;
      AnsiColor(kSignA(l));
      sprintf(sz, "%d%s House dispositor in %s:\n", 
        n, szSuffix[n], szSignName[l]); PrintSz(sz);
    
      if(InterpretAlt != 1) {
        AnsiColor(kDefault);
        FProcessInterpretFile(szHouseFile(n), NULL, 2, l);
      }
    }

    if((InterpretAlt ==1) || (InterpretAlt >= 5)) {
      l = inhouse[m];
      DispSH = l;
      AnsiColor(kSignA(l));
      sprintf(sz, "%d%s House dispositor in %d%s House:\n",
        n, szSuffix[n], l, szSuffix[l]); PrintSz(sz);
    
      if(InterpretAlt != 1) {
        AnsiColor(kDefault);
        FProcessInterpretFile(szHouseFile(n), NULL, 3, l);
      }
    }

    if (m != (int)rules[n]) {
      AnsiColor(kObjA[m]);
       sprintf(sz, "Natural Dispositor %s, total power %6.2f\n",
           szObjName[rules[n]], ppowerTotal[rules[n]]);
      PrintSz(sz);


    if((InterpretAlt ==1) || (InterpretAlt >= 5)) {
      l = SFromZ(planet[rules[n]]);
      AnsiColor(kSignA(l));
      sprintf(sz, "%d%s House dispositor in %s:\n",
        n, szSuffix[n], szSignName[l]); PrintSz(sz);
    }
    if(InterpretAlt >= 5) {
      AnsiColor(kDefault);
      if (l == DispSS)
        FProcessInterpretFile(szHouseFile(n), NULL, 0, 1);
      else
        FProcessInterpretFile(szHouseFile(n), NULL, 2, l);
    }

    if((InterpretAlt ==1) || (InterpretAlt >= 5)) {
      l = inhouse[rules[n]];
      AnsiColor(kSignA(l));
      sprintf(sz, "%d%s House dispositor in %d%s House:\n",
        n, szSuffix[n], l, szSuffix[l]); PrintSz(sz);
      }
      if(InterpretAlt >= 5) {
        AnsiColor(kDefault);
        if (l == DispSH)
        FProcessInterpretFile(szHouseFile(n), NULL, 0, 1);
        else
          FProcessInterpretFile(szHouseFile(n), NULL, 3, l);
      }
    }

    for (i = 1; i <= cObjInt; i++) {
      if (ignore[i] || FCusp(i))
        continue;
      if ((int)(inhouse[i]) == n) {
         AnsiColor(kObjA[i]);
         sprintf(sz,"In %d%s House %s, total power %6.2f:\n",
                 n, szSuffix[n], szObjName[i], ppowerTotal[i]);
         PrintSz(sz);
/*  INTERPRETATION of planet (i) in house (n)            */

        if((InterpretAlt == 2) || (InterpretAlt >= 4)) {
           AnsiColor(kDefault);
           FProcessInterpretFile(szPlanetFile(i, fFalse), NULL, 2, n);
        }
      }
    }
  PrintL();
  }
PrintL();
}

/***********************************************************/
/* PART 3: Fixed Stars                                     */
/* For every Fixed Star conjuct with some allowed thing,   */
/* print out interpretation.                               */
/***********************************************************/

  us.nStar = SaveStarFlag;

  if (!us.nStar)
     return;

  AnsiColor(kWhite);
  sprintf(sz, "   ****** FIXED STARS ******"); PrintSz(sz);
  PrintL2();

  for (i = starLo; i <= starHi; i++)
    ignore[i] = SaveStars[i];

  for (i = starLo; i <= starHi; i++) if (!ignore[oNorm+starname[i-oNorm]]) {
    j = oNorm+starname[i-oNorm];
    AnsiColor(kObjA[j]);
    sprintf(sz, "%-10.10s: ", szObjName[j]); PrintSz(sz);
    PrintZodiac(planet[j]);
    PrintSz("  ");
    PrintAltitude(planetalt[j]);
    k = inhouse[j];
    AnsiColor(kSignA(k));
    sprintf(sz, "  [%2d%s house]", k, szSuffix[k]); PrintSz(sz);
    AnsiColor(kDefault);
    sprintf(sz, "  %s %7.7s  Star #%2d: %5.2f\n",
      us.fSeconds ? " " : "", szStarConName[j-oNorm], i-oNorm, rStarBright[j-oNorm]);
    PrintSz(sz);

    for (jj = 1; jj <= cObjInt; jj++) {
      if (ignore[jj])
        continue;
      kk = grid->n[jj][j];
      m = grid->v[jj][j];
      if (kk < 1 || kk > 5)
        continue;
      PrintAspect(j, SFromZ(planet[j]), (int)RSgn(ret[j]), kk,
        jj, SFromZ(planet[jj]), (int)RSgn(ret[jj]), 'a');
      AnsiColor(kDefault);
      sprintf(sz, " - orb: %c%d%c%02d'\n",
        us.fAppSep ? (m < 0 ? 'a' : 's') : (m < 0 ? '-' : '+'),
        abs(m)/60, chDeg1, abs(m)%60); PrintSz(sz);
    }

    if(InterpretAlt > 1) {
      FProcessInterpretFile("stars.dat", NULL, 0, j-oNorm);
    PrintL();
    }
  }
}

/*
***************************************************************************
** Transit Chart interpretation routine
***************************************************************************
*/

void TransitInterpret()
{
  byte sz[cchSzDef], c;
  int i, j, k, l, m, SaveStarFlag;
  real ip, jp, p;

  SaveStarFlag = us.nStar;
  for(i = starLo; i <= starHi; i++)
    SaveStars[i] = ignore[i];
  us.nStar = fFalse;
  us.fAppSep = fTrue;

  if (!FCreateGridRelation(fFalse))
    return;

/************************************************************************ */
/* PART 1: Transiting Planet's positions in houses and aspects.           */
/* For every transiting object print general meaning of planet transits,  */
/* position in House with interpretation, aspects with natal planets with */
/* powers and interpretataion.                                            */
/**************************************************************************/

  PrintL();

    sprintf(sz, "   ****** TRANSITING PLANETS AND THEIR ASPECTS ******");
    PrintSz(sz);
    PrintL2();

  for (i = 1; i <= cObjInt; i++) {
    if (ignore2[i])
      continue;

    AnsiColor(kObjA[i]);
    l = SFromZ(cp2.obj[i]); c = Dignify(i, l);
    sprintf(sz, "%s%s%s%s in %s", ret[i] < 0.0 ? "Retrograde " : "",
      i == oNod ? "North " : (i == oFor ? "Part of " : ""), szObjName[i],
      i == 13 ? " Athena" : "", szSignName[l]);
      PrintSz(sz);
    if (i < cuspLo || i> cuspHi){
      sprintf(sz, " and %d%s House.", inhouse[i], szSuffix[inhouse[i]]);
      PrintSz(sz);
    }
    PrintL();
    if (InterpretAlt != 1) {
      AnsiColor(kDefault);
      FProcessInterpretFile(szPlanetFile(i, fTrue), NULL, 0, 0);
      FProcessInterpretFile(szPlanetFile(i, fTrue), NULL, 1, inhouse[i]);
    }

/* Print aspects of planets with orb and aspect power   */

    for (j = 1; j <= cObjInt; j++) if (!ignore[j]) {
      k = grid->n[i][j];
      m = grid->v[i][j];
      if (k < 1 || k > cAspectInt)
        continue;

      ip = i <= oNorm ? rTransitInf[i] : 2.5;
      jp = j <= oNorm ? ppower1[j] : 2.5;
      p = (rAspInf[k]*(sqrt)(ip*jp)*
          (1.0-RAbs((real)(m))/60.0/GetOrb(i,j,k))*500.0);

      PrintAspect(i, SFromZ(cp2.obj[i]), (int)RSgn(cp2.dir[i]), k,
        j, SFromZ(cp1.obj[j]), (int)RSgn(cp1.dir[j]), 'a');
      sprintf(sz, " - orb: %c%d%c%02d'",
        us.fAppSep ? (m < 0 ? 'a' : 's') : (m < 0 ? '-' : '+'),
        abs(m)/60, chDeg1, abs(m)%60); PrintSz(sz);
      AnsiColor(kDkGreen);
      sprintf(sz, " - power:%6.2f\n", p/500.0); PrintSz(sz);

/* Print out aspect interpretation (if needed)   */

      if (InterpretAlt != 1) {
        AnsiColor(kDefault);
        FProcessInterpretFile(szAspectFile(i, fTrue), NULL, j, k);
      }
    }
    PrintL();
  }
  us.nStar = SaveStarFlag;
  for(i = starLo; i <= starHi; i++)
    ignore[i] = SaveStars[i];
}
#endif /* INTERPRETALT */
#endif /* INTERPRET */
