/*
** Astrolog (Version 5.41F) File: xdevice.c
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
/*
******************************************************************************
** Bitmap File Routines.
******************************************************************************
*/

/* Write the bitmap array to a previously opened file in a format that   */
/* can be read in by the Unix X commands bitmap and xsetroot. The 'mode' */
/* parameter defines how much white space is put in the file.            */

void WriteXBitmap(file, name, mode)
FILE *file;
byte *name, mode;
{
  int x, y, i, temp = 0;
  _int value;

  fprintf(file, "#define %s_width %d\n" , name, gs.xWin);
  fprintf(file, "#define %s_height %d\n", name, gs.yWin);
  fprintf(file, "static %s %s_bits[] = {",
    mode != 'V' ? "char" : "short", name);
  for (y = 0; y < gs.yWin; y++) {
    x = 0;
    do {

      /* Process each row, eight columns at a time. */

      if (y + x > 0)
        fprintf(file, ",");
      if (temp == 0)
        fprintf(file, "\n%s",
          mode == 'N' ? "  " : (mode == 'C' ? " " : ""));
      value = 0;
      for (i = (mode != 'V' ? 7 : 15); i >= 0; i--)
        value = (value << 1) + (!(FBmGet(gi.bm, x+i, y)^
          (gs.fInverse*15))^gs.fInverse && (x + i < gs.xWin));
      if (mode == 'N')
        putc(' ', file);
      fprintf(file, "0x");
      if (mode == 'V')
        fprintf(file, "%c%c",
          ChHex(value >> 12), ChHex((value >> 8) & 15));
      fprintf(file, "%c%c",
        ChHex((value >> 4) & 15), ChHex(value & 15));
      temp++;

      /* Is it time to skip to the next line while writing the file yet? */

      if ((mode == 'N' && temp >= 12) ||
          (mode == 'C' && temp >= 15) ||
          (mode == 'V' && temp >= 11))
        temp = 0;
      x += (mode != 'V' ? 8 : 16);
    } while (x < gs.xWin);
  }
  fprintf(file, "};\n");
}


/* Write the bitmap array to a previously opened file in a simple boolean    */
/* Ascii rectangle, one char per pixel, where '#' represents an off bit and  */
/* '-' an on bit. The output format is identical to the format generated by  */
/* the Unix bmtoa command, and it can be converted into a bitmap with atobm. */

void WriteAscii(file)
FILE *file;
{
  int x, y, i;

  for (y = 0; y < gs.yWin; y++) {
    for (x = 0; x < gs.xWin; x++) {
      i = FBmGet(gi.bm, x, y);
      if (gs.fColor)
        putc(ChHex(i), file);
      else
        putc(i ? '-' : '#', file);
    }
    putc('\n', file);
  }
}


/* Write the bitmap array to a previously opened file in the bitmap format  */
/* used in Microsoft Windows for its .bmp extension files. This is a pretty */
/* efficient format, only requiring a small header, and one bit per pixel   */
/* for monochrome graphics, or four bits per pixel for full color.          */

void WriteBmp(file)
FILE *file;
{
  int x, y;
  dword value;

  /* Note that we sometimes only write a part of the full bitmap to disk   */
  /* during the call, as done when the bitmap is being generated in bands. */

  if (gi.yBand == 0 || gi.yOffset + gi.yBand >= gs.yWin) {
    /* BitmapFileHeader */
    PutByte('B'); PutByte('M');
    PutLong(14+40 + (gs.fColor ? 64 : 8) +
      (long)4*gs.yWin*((gs.xWin-1 >> (gs.fColor ? 3 : 5))+1));
    PutWord(0); PutWord(0);
    PutLong(14+40 + (gs.fColor ? 64 : 8));
    /* BitmapInfo / BitmapInfoHeader */
    PutLong(40);
    PutLong(gs.xWin); PutLong(gs.yWin);
    PutWord(1); PutWord(gs.fColor ? 4 : 1);
    PutLong(0 /*BI_RGB*/); PutLong(0);
    PutLong(0); PutLong(0);
    PutLong(0); PutLong(0);
    /* RgbQuad */
    if (gs.fColor)
      for (x = 0; x < 16; x++) {
        PutByte(RGBB(rgbbmp[x])); PutByte(RGBG(rgbbmp[x]));
        PutByte(RGBR(rgbbmp[x])); PutByte(0);
      }
    else {
      PutLong(0);
      PutByte(255); PutByte(255); PutByte(255); PutByte(0);
    }
  }
  /* Data */
  for (y = (gi.yBand ? Min(gi.yBand, gs.yWin - gi.yOffset) : gs.yWin) - 1;
    y >= 0; y--) {
    value = 0;
    for (x = 0; x < gs.xWin; x++) {
      if ((x & (gs.fColor ? 7 : 31)) == 0 && x > 0) {
        PutLong(value);
        value = 0;
      }
      if (gs.fColor)
        value |= (dword)FBmGet(gi.bm, x, y) << ((x & 7 ^ 1) << 2);
      else
        if (FBmGet(gi.bm, x, y))
          value |= (dword)1 << (x & 31 ^ 7);
    }
    PutLong(value);
  }
}


/* Begin the work of creating a graphics file. Prompt for a filename if */
/* need be, and if valid, create the file and open it for writing.      */

void BeginFileX()
{
  byte line[cchSzDef];

  if (us.fNoWrite)
    return;
#ifndef WIN
  if (gi.szFileOut == NULL && (
#ifdef PS
    gi.fEps ||
#endif
    gs.fMeta || (gs.fBitmap && gs.chBmpMode == 'B'))) {
    sprintf(line, "(It is recommended to specify an extension of '.%s'.)\n",
      gs.fBitmap ? "bmp" :
#ifdef PS
      (gi.fEps ? "eps" : "wmf")
#else
      "wmf"
#endif
      );
    PrintSzScreen(line);
  }
#endif /* WIN */
  loop {
    if (gi.szFileOut == NULL) {
      sprintf(line, "Enter name of file to write %s to",
        gs.fBitmap ? "bitmap" : (gs.fPS ? "PostScript" : "metafile"));
      InputString(line, line);
      gi.szFileOut = line;
    }
    gi.file = fopen(gi.szFileOut, gs.fPS ? "w" : "wb");
    if (gi.file != NULL)
      break;
    else {
      PrintWarning("Couldn't create output file.");
      gi.szFileOut = NULL;
    }
  }
}


/* Finish up the work of creating a graphics file. This basically consists */
/* of just calling the appropriate routine to actually write the data in   */
/* memory to a file for bitmaps and metafiles, although for PostScript we  */
/* just close file as we were already writing while creating the chart.    */

void EndFileX()
{
  byte sz[cchSzDef];

  if (gs.fBitmap) {
    if (gi.yBand) {
      sprintf(sz, "Writing part %d of chart bitmap to file.",
        gi.yOffset / gi.yBand + 1);
      PrintNotice(sz);
    } else
      PrintNotice("Writing chart bitmap to file.");
    if (gs.chBmpMode == 'B')
      WriteBmp(gi.file);
    else if (gs.chBmpMode == 'A')
      WriteAscii(gi.file);
    else
      WriteXBitmap(gi.file, gi.szFileOut, gs.chBmpMode);
  }
#ifdef PS
  else if (gs.fPS)
    PsEnd();
#endif
#ifdef META
  else {
    PrintNotice("Writing metafile to disk.");
    WriteMeta(gi.file);
  }
#endif
  if (!gs.fBitmap || gi.yOffset == 0) {
    fclose(gi.file);
    gi.yBand = 0;
  }
#ifdef WIN
  if (wi.wCmd == cmdSaveWallTile || wi.wCmd == cmdSaveWallCenter) {
    WriteProfileString("Desktop", "TileWallpaper",
      wi.wCmd == cmdSaveWallTile ? "1" : "0");
    SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, gi.szFileOut,
      SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
    wi.wCmd = 0;
  }
#endif
}


#ifdef PS
/*
******************************************************************************
** PostScript File Routines.
******************************************************************************
*/

/* Table of PostScript header alias lines used by the program. */

CONST byte FPTR szPsFunctions[] =
"/languagelevel where{pop languagelevel}{1}ifelse\
 2 lt{\n\
/sf{exch findfont exch\
 dup type/arraytype eq{makefont}{scalefont}ifelse setfont}bind def\n\
/rf{gsave newpath\n\
4 -2 roll moveto\
 dup 0 exch rlineto exch 0 rlineto neg 0 exch rlineto closepath\n\
fill grestore}bind def\n\
/rc{newpath\n\
4 -2 roll moveto\
 dup 0 exch rlineto exch 0 rlineto neg 0 exch rlineto closepath\n\
clip newpath}bind def\n\
}{/sf/selectfont load def/rf/rectfill load def\
/rc/rectclip load def}ifelse\n\
/center{0 begin gsave dup 4 2 roll\
 translate newpath 0 0 moveto\
 false charpath flattenpath pathbbox\
 /URy exch def/URx exch def/LLy exch def/LLx exch def\
 URx LLx sub 0.5 mul LLx add neg URy LLy sub 0.5 mul LLy add neg\
 0 0 moveto rmoveto\
 show grestore end}bind def\n\
/center load 0 4 dict put\n\
/c{setrgbcolor}bind def\n\
/d{moveto 0 0 rlineto}bind def\n\
/l{4 2 roll moveto lineto}bind def\n\
/t{lineto}bind def\n\
/el{newpath matrix currentmatrix 5 1 roll translate scale\
 0 0 1 0 360 arc setmatrix stroke}bind def\n";


/* Write a command to flush the PostScript buffer. */

void PsStrokeForce()
{
  if (gi.cStroke > 0) {              /* render any existing path */
    fprintf(gi.file, "stroke\n");
    gi.cStroke = 0;
    gi.xPen = -1;                    /* Invalidate PolyLine cache */
  }
}


/* Indicate that a certain number of PostScript commands have been done. */

void PsStroke(n)
int n;
{
  gi.cStroke += n;
  if (gi.cStroke > 2000)    /* Whenever we reach a certain limit, flush. */
    PsStrokeForce();
}


/* Set the type of line end to be used by PostScript commands. If linecap */
/* is true, then the line ends are rounded, otherwise they are squared.   */

void PsLineCap(fLineCap)
bool fLineCap;
{
  if (fLineCap != gi.fLineCap) {
    PsStrokeForce();
    fprintf(gi.file, "%d setlinecap\n", fLineCap);
    gi.fLineCap = fLineCap;
  }
}


/* Set the dash length to be used by PostScript line commands. */

void PsDash(dashoff)
int dashoff;
{
  if (dashoff != gi.nDash) {
    PsStrokeForce();
    if (dashoff)
      fprintf(gi.file, "[%d %d", PSMUL, dashoff * PSMUL);
    else
      fprintf(gi.file, "[");
    fprintf(gi.file, "]0 setdash\n");
    gi.nDash = dashoff;
  }
}


/* Set a linewidth size to be used by PostScript figure primitive commands. */

void PsLineWidth(linewidth)
int linewidth;
{
  if ((real)linewidth != gi.rLineWid) {
    PsStrokeForce();
    fprintf(gi.file, "%d setlinewidth\n", linewidth);
    gi.rLineWid = (real)linewidth;
  }
}


/* Set a system font and size to be used by PostScript text commands. */

void PsFont(psfont)
int psfont;
{
  int z;

  if (psfont != gi.nFont && gs.fFont) {
    if (psfont <= 2) {
      z = psfont == 1 ? 32*PSMUL : 23*PSMUL;
      fprintf(gi.file, "/Astro[%d 0 0 -%d 0 0]sf\n", z, z);
    } else if (psfont == 3) {
      z = 26*PSMUL;
      fprintf(gi.file, "/Times-Roman[%d 0 0 -%d 0 0]sf\n", z, z);
    } else {
      z = 10*PSMUL;
      fprintf(gi.file, "/Courier[%d 0 0 -%d 0 0]sf\n", z, z);
    }
    gi.nFont = psfont;
  }
}


/* Prompt the user for the name of a file to write the PostScript file to */
/* (if not already specified), open it, and write out file header info.   */

void PsBegin()
{
  fprintf(gi.file, "%%!PS-Adobe-2.0");
  if (gi.fEps)
    fprintf(gi.file, " EPSF-2.0");
  fprintf(gi.file, "\n%%%%Title: %s\n", gi.szFileOut);
  fprintf(gi.file, "%%%%Creator: %s %s\n", szAppName, szVersionCore);
  fprintf(gi.file, "%%%%CreationDate: %s\n", szDateCore);
  if (gi.fEps) {
    fprintf(gi.file, "%%%%BoundingBox: 0 0 %d %d\n", gs.xWin, gs.yWin);
    fprintf(gi.file, "%%%%EndComments\n");
    fprintf(gi.file, "%%%%BeginSetup\n");
    fprintf(gi.file, szPsFunctions, 6 * PSMUL, 6 * PSMUL);
    fprintf(gi.file, "%%%%EndSetup\n");
    fprintf(gi.file, "0 0 %d %d rc\n", gs.xWin, gs.yWin);
  } else {
    fprintf(gi.file, "%%%%Pages: 1 1\n");
    fprintf(gi.file, "%%%%DocumentFonts: (atend)\n");
    fprintf(gi.file, "%%%%BoundingBox: %d %d %d %d\n", PSGUTTER, PSGUTTER,
      (int)(gs.xInch*72.0+rRound)-PSGUTTER,
      (int)(gs.yInch*72.0+rRound)-PSGUTTER);
    fprintf(gi.file, "%%%%EndComments\n");
    fprintf(gi.file, "%%%%BeginProcSet: common\n");
    fprintf(gi.file, szPsFunctions, 6 * PSMUL, 6 * PSMUL);
    fprintf(gi.file, "%%%%EndProcSet\n");
    fprintf(gi.file, "%%%%Page: 1 1\n");
  }
  PsFont(2);
  fprintf(gi.file, "gsave\n");
  PsLineWidth(gi.nPenWid/2);
  gi.xPen = -1;
  PrintNotice("Creating PostScript chart file.");
}


/* Write out trailing information to the PostScript file and close it. */

void PsEnd()
{
  PsStrokeForce();
  if (gi.fEps)
    fprintf(gi.file, "%%%%EOF\n");
  else {
    fprintf(gi.file, "showpage\n");
    fprintf(gi.file, "%%%%PageTrailer\n");
    fprintf(gi.file, "%%%%Trailer\n");
    fprintf(gi.file, "%%%%DocumentFonts: Times-Roman\n");
    if (gs.fFont) {
      fprintf(gi.file, "%%%%+ Courier\n");
      fprintf(gi.file, "%%%%+ Astro\n");
    }
  }
  fclose(gi.file);
}
#endif /* PS */


#ifdef META
/*
******************************************************************************
** Metafile Routines.
******************************************************************************
*/

/* Output one 16 bit or 32 bit value into the metafile buffer stream. */

void MetaWord(w)
word w;
{
  byte sz[cchSzDef];

  if ((hpbyte)gi.pwMetaCur - gi.bm >= gi.cbMeta) {
    sprintf(sz, "Metafile would be more than %ld bytes.", gi.cbMeta);
    PrintError(sz);
    Terminate(tcFatal);
  }
  *gi.pwMetaCur = w;
  gi.pwMetaCur++;
}

void MetaLong(l)
long l;
{
  MetaWord(WLo(l));
  MetaWord(WHi(l));
}


/* Output any necessary metafile records to make the current actual     */
/* settings of line color, fill color, etc, be those that we know are   */
/* desired. This is generally called by the primitives routines before  */
/* any figure record is actually written into a metafile. We wait until */
/* the last moment before changing any settings to ensure that we don't */
/* output any unnecessary records, e.g. two select colors in a row.     */

void MetaSelect()
{
  if (gi.kiLineDes != gi.kiLineAct) {
    MetaSelectObject(gi.kiLineDes);
    gi.kiLineAct = gi.kiLineDes;
  }
  if (gi.kiFillDes != gi.kiFillAct) {
    MetaSelectObject(16*4 + gi.kiFillDes);
    gi.kiFillAct = gi.kiFillDes;
  }
  if (gi.nFontDes != gi.nFontAct) {
    MetaSelectObject(16*5 + gi.nFontDes);
    gi.nFontAct = gi.nFontDes;
  }
  if (gi.kiTextDes != gi.kiTextAct) {
    MetaTextColor(rgbbmp[gi.kiTextDes]);
    gi.kiTextAct = gi.kiTextDes;
  }
  if (gi.nAlignDes != gi.nAlignAct) {
    MetaTextAlign(gi.nAlignDes);
    gi.nAlignAct = gi.nAlignDes;
  }
  gi.xPen = -1;    /* Invalidate PolyLine cache */
}


/* Output initial metafile header information into our metafile buffer. */
/* We also setup and create all pen, brush, and font objects that may   */
/* possibly be used in the generation and playing of the picture.       */

void MetaInit()
{
  int i, j, k;

  gi.pwMetaCur = (word HPTR *)gi.bm;
  /* Placable Metaheader */
  MetaLong(0x9AC6CDD7L);
  MetaWord(0);      /* Not used */
  MetaWord(0); MetaWord(0);
  MetaWord(gs.xWin); MetaWord(gs.yWin);
  MetaWord(gs.xWin/6);                     /* Units per inch */
  MetaLong(0L);     /* Not used */
  MetaWord(0x9AC6 ^ 0xCDD7 ^ gs.xWin ^ gs.yWin ^ gs.xWin/6);  /* Checksum */
  /* Metaheader */
  MetaWord(1);                      /* Metafile type                    */
  MetaWord(9);                      /* Size of header in words          */
  MetaWord(0x300);                  /* Windows version                  */
  MetaLong(0L);                     /* Size of entire metafile in words */
  MetaWord(16*5+1+(gs.fFont>0)*4);  /* Number of objects in metafile    */
  MetaLong(17L);                    /* Size of largest record in words  */
  MetaWord(0);                      /* Not used                         */
  /* Setup */
  MetaEscape(17);
  MetaLong(LFromBB('A', 's', 't', 'r'));  /* "Astr" */
  MetaWord(4);                            /* Creator */
  MetaLong(14L);                          /* Bytes in string */
  MetaLong(LFromBB('A', 's', 't', 'r'));  /* "Astr" */
  MetaLong(LFromBB('o', 'l', 'o', 'g'));  /* "olog" */
  MetaLong(LFromBB(' ', '5', '.', '4'));  /* " 5.4" */
  MetaWord(WFromBB('0', 0));              /* "0"    */
  MetaSaveDc();
  MetaWindowOrg(0, 0);
  MetaWindowExt(gs.xWin, gs.yWin);
  MetaBkMode(1 /* Transparent */);
  /* Colors */
  for (j = 1; j <= 4; j++)
    for (i = 0; i < 16; i++) {
      k = j <= 1 ? gi.nPenWid : 0;
      MetaCreatePen(j <= 2 ? 0 : j-2 /* PS_SOLID; PS_DASH; PS_DOT */,
        k, rgbbmp[i]);
    }
  for (i = 0; i < 16; i++) {
    MetaCreateBrush(0 /* BS_SOLID */, rgbbmp[i]);
  }
  MetaCreateBrush(1 /* BS_NULL */, 0L);
  /* Fonts */
  if (gs.fFont) {
    MetaCreateFont(5, 0, -8*gi.nScale, 2 /* Symbol Charset */);
    MetaWord(WFromBB(1 /* Draft */, 1 | 0x10 /* Fixed | Roman */));
    MetaLong(LFromBB('W', 'i', 'n', 'g'));
    MetaLong(LFromBB('d', 'i', 'n', 'g'));
    MetaWord(WFromBB('s', 0));

    MetaCreateFont(8, 0, -6*gi.nScale, 0 /* Ansi Charset */);
    MetaWord(WFromBB(0 /* Default */, 2 | 0x10 /* Variable | Roman */));
    MetaLong(LFromBB('T', 'i', 'm', 'e'));
    MetaLong(LFromBB('s', ' ', 'N', 'e'));
    MetaLong(LFromBB('w', ' ', 'R', 'o'));
    MetaLong(LFromBB('m', 'a', 'n', 0));

    MetaCreateFont(6, 6*METAMUL, 10*METAMUL, 0 /* Ansi Charset */);
    MetaWord(WFromBB(1 /* Draft */, 1 | 0x30 /* Fixed | Modern */));
    MetaLong(LFromBB('C', 'o', 'u', 'r'));
    MetaLong(LFromBB('i', 'e', 'r', ' '));
    MetaLong(LFromBB('N', 'e', 'w', 0));

    MetaCreateFont(8, 0, -11*gi.nScale, 0 /* Ansi Charset */);
    MetaWord(WFromBB(0 /* Default */, 2 | 0 /* Variable | Don't Care */));
    MetaLong(LFromBB('A', 's', 't', 'r'));
    MetaLong(LFromBB('o', '-', 'S', 'e'));
    MetaLong(LFromBB('m', 'i', 'B', 'o'));
    MetaLong(LFromBB('l', 'd', 0, 0));
  }
}


/* Output trailing records to indicate the end of the metafile and then */
/* actually write out the entire buffer to the specifed file.           */

void WriteMeta(file)
FILE *file;
{
  word HPTR *w;
#if FALSE
  int i;

  for (i = 16*5+1+(gs.fFont>0)*4; i >= 0; i--) {
    MetaDeleteObject(i);
  }
#endif
  MetaRestoreDc();
  MetaRecord(3, 0);    /* End record */
  *(long HPTR *)(gi.bm + 22 + 6) =
    ((long)((hpbyte)gi.pwMetaCur - gi.bm) - 22) / 2;
  for (w = (word HPTR *)gi.bm; w < gi.pwMetaCur; w++) {
    PutWord(*w);
  }
}
#endif /* META */


#ifdef MOUSE
#ifdef PC
/*
******************************************************************************
** Mouse Routines.
******************************************************************************
*/

static union REGS dosreg;


/* Setup and initialize the PC graphics mouse, returning the number of    */
/* buttons available, or zero for no mouse at all. Passed in is the pixel */
/* size of the screen the mouse pointer is to be contained within.        */

int MouseInit(x, y)
int x, y;
{
  int dx, cBtn;

  if (!gs.fMouse)
    return 0;
  dosreg.x.ax = 0;
  int86(0x33, &dosreg, &dosreg);
  if (!(gs.fMouse = dosreg.x.ax))
    return 0;
  cBtn = dosreg.x.bx;
  dx = x - 1;
  dosreg.x.ax = 7;
  dosreg.x.cx = 0;
  dosreg.x.dx = dx;
  int86(0x33, &dosreg, &dosreg);
  dx = y - 1;
  dosreg.x.ax = 8;
  dosreg.x.cx = 0;
  dosreg.x.dx = dx;
  int86(0x33, &dosreg, &dosreg);
  return cBtn;
}


/* Turn on or hide the PC graphics mouse pointer. */

void MouseShow(fShow)
bool fShow;
{
  int ax;

  if (!gs.fMouse)
    return;
  ax = fShow ? 1 : 2;
  dosreg.x.ax = ax;
  int86(0x33, &dosreg, &dosreg);
}


/* Fill out the current status of the mouse: its horizontal and vertical  */
/* position, and button press status. We return false if the mouse hasn't */
/* changed any from the last call (assuming that the previous values are  */
/* stored in the input parameters) or if the mouse isn't active at all.   */

bool MouseStatus(x, y, btn)
int *x, *y, *btn;
{
  int bx, cx, dx, fChange;

  if (!gs.fMouse)
    return fFalse;
  dosreg.x.ax = 3;
  int86(0x33, &dosreg, &dosreg);
  bx = dosreg.x.bx;
  cx = dosreg.x.cx;
  dx = dosreg.x.dx;
  fChange = (cx != *x || dx != *y || bx != *btn);
  *x = cx;
  *y = dx;
  *btn = bx;
  return fChange;
}
#endif /* PC */
#endif /* MOUSE */
#endif /* GRAPH */

/* xdevice.c */
