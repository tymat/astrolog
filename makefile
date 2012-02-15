# Astrolog (Version 5.41E) File: Makefile (Unix version)
#
# IMPORTANT NOTICE: The graphics database and chart display routines
# used in this program are Copyright (C) 1991-1996 by Walter D. Pullen
# (Astara@msn.com, http://www.magitech.com/~cruiser1/astrolog.htm).
# Permission is granted to freely use and distribute these routines
# provided one doesn't sell, restrict, or profit from them in any way.
# Modification is allowed provided these notices remain with any
# altered or edited versions of the program.
#
# First created 11/21/1991.
#
# This Makefile is included only for convenience. One could easily compile
# Astrolog on a Unix system by hand with the command:
# % cc -c -O *.c; cc -o astrolog *.o -lm -lX11
# Generally, all that needs to be done to compile once astrolog.h has been
# edited, is compile each source file, and link them together with the math
# library, and if applicable, the main X library.
#
NAME = astrolog
OBJ = astrolog.o data.o data2.o general.o io.o\
 calc.o matrix.o charts0.o charts1.o charts2.o charts3.o\
 intrpret.o intrpalt.o\
 swe_call.o swejpl.o swemplan.o sweph.o\
 swedate.o swemmoon.o swephlib.o\
 xdata.o xgeneral.o xdevice.o xcharts0.o xcharts1.o xcharts2.o xscreen.o
# If you don't have X windows, delete the "-lX11" part from the line below:
# LIBS = -lm -lX11
# with Debian Linux and X windows worked 
# LIBS = -lm -L/usr/X11R6/lib -lX11
LIBS= -lm
#
CFLAGS = -O -DHPUX_SOURCE
#
# CFLAGS from 5.41 original sources
# CFLAGS = -g -Aa -DHPUX_SOURCE
#
# Planetery ephemeris are built into code. To drop them and use external file, 
# use additional flag -DPAIR_SWEPH
#
astrolog:: $(OBJ)
	cc -o $(NAME) $(OBJ) $(LIBS)
	strip $(NAME)
#
