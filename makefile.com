$ ! Astrolog (Version 5.41) File: makefile.com (VMS version)
$ !
$ ! IMPORTANT NOTICE: The graphics database and chart display routines
$ ! used in this program are Copyright (C) 1991-1996 by Walter D. Pullen
$ ! (Astara@msn.com, http://www.magitech.com/~cruiser1/astrolog.htm).
$ ! Permission is granted to freely use and distribute these routines
$ ! provided one doesn't sell, restrict, or profit from them in any way.
$ ! Modification is allowed provided these notices remain with any
$ ! altered or edited versions of the program.
$ !
$ ! This script originally created by Max Calvani.
$ !
$ set ver
$ define X11 decw$include
$ define lnk$library sys$library:vaxcrtl
$ CC ASTROLOG
$ CC CALC
$ CC CHARTS0
$ CC CHARTS1
$ CC CHARTS2
$ CC CHARTS3
$ CC DATA
$ CC DATA2
$ CC GENERAL
$ CC INTRPRET
$ CC INTRPALT
$ CC IO
$ CC MATRIX
$ CC XCHARTS0
$ CC XCHARTS1
$ CC XCHARTS2
$ CC XDATA
$ CC XDEVICE
$ CC XGENERAL
$ CC XSCREEN
$ link/exe=astrolog.exe -
	ASTROLOG.obj, -
	CALC.obj, -
	CHARTS0.obj, -
	CHARTS1.obj, -
	CHARTS2.obj, -
	CHARTS3.obj, -
	DATA.obj, -
	DATA2.obj, -
	GENERAL.obj, -
	INTRPRET.obj, -
	INTRPALT.obj, -
	IO.obj, -
	MATRIX.obj, -
	SWE_CALL.OBj, -
	SWEJPL.OBJ, -
	SWEMPLAN.OBj, -
	SWEPH.OBJ, -
	SWEDATE.OBJ, -
	SWEMMOON.OBj, -
	SWEPHLIB.OBj, -
	XCHARTS0.obj, -
	XCHARTS1.obj, -
	XCHARTS2.obj, -
	XDATA.obj, -
	XDEVICE.obj, -
	XGENERAL.obj, -
	XSCREEN.obj, -
	sys$input/opt
sys$share:decw$xlibshr/share
$ set nover
$ exit
