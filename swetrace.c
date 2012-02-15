#include "sweodef.h"
#include "swephexp.h"

void main()
{
  double tjd, t, nut, eps; int i, ipl, retc; long iflag;
  double armc, geolat, cusp[12], ascmc[10]; int hsys;
  double xx[6]; long iflgret;
  char s[AS_MAXCH], star[AS_MAXCH], serr[AS_MAXCH];

/*SWE_CLOSE*/
  swe_close();

/*SWE_SET_EPHE_PATH*/
  strcpy(s, ".;D:\WORK\ASTROLOG\DATFILES;D:\WORK\ASTROLOG\ASTR;C:\ASTROLOG;C:\ASTROLOG;C:\ASTROLOG");
  swe_set_ephe_path(s);
  printf("swe_set_ephe_path: path_in = ");  printf(s);
  	printf("\tpath_set = unknown to swetrace\n"); /* unknown to swetrace */

/*SWE_DELTAT*/
  tjd = 2451217.997210648; t = swe_deltat(tjd);
  printf("swe_deltat: %f\t%f\t\n", tjd, t);

/*SWE_CALC*/
  tjd = 2451217.997964243; ipl = 0; iflag = 256;
  iflgret = swe_calc(tjd, ipl, iflag, xx, serr);	/* xx = 282555430 */
  printf("swe_calc: %f\t%d\t%ld\t%f\t%f\t%f\t%f\t%f\t%f\t", 
	tjd, ipl, iflgret, xx[0], xx[1], xx[2], xx[3], xx[4], xx[5]);
  if (*serr != '\0') printf(serr); printf("\n");

/*SWE_DELTAT*/
  tjd = 2451217.997210648; t = swe_deltat(tjd);
  printf("swe_deltat: %f\t%f\t\n", tjd, t);

/*SWE_CALC*/
  tjd = 2451217.997964243; ipl = 1; iflag = 256;
  iflgret = swe_calc(tjd, ipl, iflag, xx, serr);	/* xx = 282555430 */
  printf("swe_calc: %f\t%d\t%ld\t%f\t%f\t%f\t%f\t%f\t%f\t", 
	tjd, ipl, iflgret, xx[0], xx[1], xx[2], xx[3], xx[4], xx[5]);
  if (*serr != '\0') printf(serr); printf("\n");

/*SWE_DELTAT*/
  tjd = 2451217.997210648; t = swe_deltat(tjd);
  printf("swe_deltat: %f\t%f\t\n", tjd, t);

/*SWE_CALC*/
  tjd = 2451217.997964243; ipl = 2; iflag = 256;
  iflgret = swe_calc(tjd, ipl, iflag, xx, serr);	/* xx = 282555430 */
  printf("swe_calc: %f\t%d\t%ld\t%f\t%f\t%f\t%f\t%f\t%f\t", 
	tjd, ipl, iflgret, xx[0], xx[1], xx[2], xx[3], xx[4], xx[5]);
  if (*serr != '\0') printf(serr); printf("\n");

/*SWE_DELTAT*/
  tjd = 2451217.997210648; t = swe_deltat(tjd);
  printf("swe_deltat: %f\t%f\t\n", tjd, t);

/*SWE_CALC*/
  tjd = 2451217.997964243; ipl = 3; iflag = 256;
  iflgret = swe_calc(tjd, ipl, iflag, xx, serr);	/* xx = 282555430 */
  printf("swe_calc: %f\t%d\t%ld\t%f\t%f\t%f\t%f\t%f\t%f\t", 
	tjd, ipl, iflgret, xx[0], xx[1], xx[2], xx[3], xx[4], xx[5]);
  if (*serr != '\0') printf(serr); printf("\n");

/*SWE_DELTAT*/
  tjd = 2451217.997210648; t = swe_deltat(tjd);
  printf("swe_deltat: %f\t%f\t\n", tjd, t);

/*SWE_CALC*/
  tjd = 2451217.997964243; ipl = 4; iflag = 256;
  iflgret = swe_calc(tjd, ipl, iflag, xx, serr);	/* xx = 282555430 */
  printf("swe_calc: %f\t%d\t%ld\t%f\t%f\t%f\t%f\t%f\t%f\t", 
	tjd, ipl, iflgret, xx[0], xx[1], xx[2], xx[3], xx[4], xx[5]);
  if (*serr != '\0') printf(serr); printf("\n");

/*SWE_DELTAT*/
  tjd = 2451217.997210648; t = swe_deltat(tjd);
  printf("swe_deltat: %f\t%f\t\n", tjd, t);

/*SWE_CALC*/
  tjd = 2451217.997964243; ipl = 5; iflag = 256;
  iflgret = swe_calc(tjd, ipl, iflag, xx, serr);	/* xx = 282555430 */
  printf("swe_calc: %f\t%d\t%ld\t%f\t%f\t%f\t%f\t%f\t%f\t", 
	tjd, ipl, iflgret, xx[0], xx[1], xx[2], xx[3], xx[4], xx[5]);
  if (*serr != '\0') printf(serr); printf("\n");

/*SWE_DELTAT*/
  tjd = 2451217.997210648; t = swe_deltat(tjd);
  printf("swe_deltat: %f\t%f\t\n", tjd, t);

/*SWE_CALC*/
  tjd = 2451217.997964243; ipl = 6; iflag = 256;
  iflgret = swe_calc(tjd, ipl, iflag, xx, serr);	/* xx = 282555430 */
  printf("swe_calc: %f\t%d\t%ld\t%f\t%f\t%f\t%f\t%f\t%f\t", 
	tjd, ipl, iflgret, xx[0], xx[1], xx[2], xx[3], xx[4], xx[5]);
  if (*serr != '\0') printf(serr); printf("\n");

/*SWE_DELTAT*/
  tjd = 2451217.997210648; t = swe_deltat(tjd);
  printf("swe_deltat: %f\t%f\t\n", tjd, t);

/*SWE_CALC*/
  tjd = 2451217.997964243; ipl = 7; iflag = 256;
  iflgret = swe_calc(tjd, ipl, iflag, xx, serr);	/* xx = 282555430 */
  printf("swe_calc: %f\t%d\t%ld\t%f\t%f\t%f\t%f\t%f\t%f\t", 
	tjd, ipl, iflgret, xx[0], xx[1], xx[2], xx[3], xx[4], xx[5]);
  if (*serr != '\0') printf(serr); printf("\n");

/*SWE_DELTAT*/
  tjd = 2451217.997210648; t = swe_deltat(tjd);
  printf("swe_deltat: %f\t%f\t\n", tjd, t);

/*SWE_CALC*/
  tjd = 2451217.997964243; ipl = 8; iflag = 256;
  iflgret = swe_calc(tjd, ipl, iflag, xx, serr);	/* xx = 282555430 */
  printf("swe_calc: %f\t%d\t%ld\t%f\t%f\t%f\t%f\t%f\t%f\t", 
	tjd, ipl, iflgret, xx[0], xx[1], xx[2], xx[3], xx[4], xx[5]);
  if (*serr != '\0') printf(serr); printf("\n");

/*SWE_DELTAT*/
  tjd = 2451217.997210648; t = swe_deltat(tjd);
  printf("swe_deltat: %f\t%f\t\n", tjd, t);

/*SWE_CALC*/
  tjd = 2451217.997964243; ipl = 10; iflag = 256;
  iflgret = swe_calc(tjd, ipl, iflag, xx, serr);	/* xx = 282555430 */
  printf("swe_calc: %f\t%d\t%ld\t%f\t%f\t%f\t%f\t%f\t%f\t", 
	tjd, ipl, iflgret,