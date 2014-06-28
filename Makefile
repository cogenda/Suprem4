#----------------------------------------------------------------------
#  Top level Makefile for Suprem-IV.gs
#
#  Written by Stephen E. Hansen (hansen@gloworm.stanford.edu)
#		Tue Feb  2 09:02:13 PST 1993
#
#----------------------------------------------------------------------

# change these lines when porting to other machines
#
# The SYS variable should be changed to reflect the appropriate system
# type.  BSD and SYSV are the most common.  There are several changes
# that may also be appropriate on vector hardware.
#
# SYS should be one of {BSD, CONVEX, HPUX, CRAY}
# BSD - "vanilla" BSD4.2 system
# SYSV - vanilla system V unix
# CONVEX - like BSD but uses vectorizing Fortran sparse matrix routines
# HPUX - example of a non-BSD system
# CRAY - like HPUX, but uses vectorizing Fortran sparse matrix routines
SYS     = SYSV

#.. This is only useful if you want the bin, data, and help directories
#.. located immediately below HERE.  Otherwise set BINDIR, DATADIR, and
#.. HELPDIR to the explicit paths.  Make sure before you start that you
#.. have permission to write into the appropriate directories.

#.. Where are we?
HERE	= $(SUPREM4_HOME)

#.. What are we going to call it and where does all this go?
SUPREM	= suprem
#BINDIR	= /usr/local/bin
BINDIR	= $(HERE)/bin

#.. Edit these to change the location of the help and data files.
#.. The user man override these locations with the appropriate
#.. envrionment variable (e.g. MANDIR, KEYFILE, MODELFILE, IMPFILE).
#HELPDIR = /usr/local/lib/suprem45/help
#DATADIR = /usr/local/lib/suprem45/data
HELPDIR = $(HERE)/help
DATADIR = $(HERE)/data

#.. Which C and Fortran Compiler?
#CC	= cc
CC	= gcc -m32
#FC	= f77
FC	= gfortran -m32

#.. What optimization level for the compilers?
COPT	= -O1
FOPT	= -O1
#COPT	= -g
#FOPT	= -g

#.. One of two graphics options is supported and must be chosen at
#.. compile time.  One is for X windows, (specific to X11R4), which
#.. supports postscript, hpgl, and idraw for hardcopy.  The other
#.. package is for gplot, the graphics package from Stanford which
#.. supports a wide variety of terminals and harcopy devices,
#.. including X windows and Sunview.

#.. If you want the X11 plotting uncomment the following.
#GRLIBS	= -lXaw -lXmu -lXt -lXext -lX11
#GRINC	=
#GRARC	= $$(LIBDEST)/xsupr4.a
#GRSRCS	= xsupr4/Graph.c xsupr4/app1.c xsupr4/draw.c xsupr4/help.c \
#	xsupr4/hpgl.c xsupr4/idraw.c xsupr4/interface.c xsupr4/ps.c \
#	xsupr4/xgX.c
#GROBJS	= xsupr4/Graph.o xsupr4/app1.o xsupr4/draw.o xsupr4/help.o \
#	xsupr4/hpgl.o xsupr4/idraw.o xsupr4/interface.o xsupr4/ps.o \
#	xsupr4/xgX.o


#.. Otherwise if you want the gplot plotting package then use this.
#.. Although you need to change the /usr/local/.. to wherever the
#.. package include and archive files are.
#GRLIBS	= -L/icl/lib -lgplot -lhigh2
#GRINC	= -I/icl/include
#GRLIBS        = -L$(PISCES_HOME)/icl/lib -lgplot -lhigh2
#GRINC = -I$(PISCES_HOME)/icl/include

#GRARC	= $$(LIBDEST)/gpsup.a
#GRSRCS	= gpsup/gpcover.c
#GROBJS	= gpsup/gpcover.o


#or disable the plot function by use fake gp functions
GRARC	= $$(LIBDEST)/gpfake.a
GRSRCS	= gpsup/gpfake.c
GROBJS	= gpsup/gpfake.o

# SysV uses termlib
#LIBS	= $(GRLIBS) -ly -ll -lm -ltermlib
# BSD uses termcap
#LIBS	= $(GRLIBS) -ly -ll -lm -ltermcap
#It seems termcap does not be required by linux
LIBS  = $(GRLIBS) -lm


#.. SysV users take note!  In src/Makefile.proto, down where the
#.. archive files are ranlib'd, is some real crufty stuff.  This is to
#.. get around a bug in ranlib that shows up mainly when running gmake
#.. with the multi-job flag.
#.. BSD folks need this.
RANLIB  = ranlib
#.. SysV folks who don't have ranlib should to do this.
#RANLIB  = \#

MAKE	= make
#MAKE	= gmake

#--- You shouldn't need to mess with anything below here ---
#----------------------------------------------------------------------
#LOCDEFS = -DDEVICE -DHAVE_KSH -D$(SYS)
LOCDEFS	= -DDEVICE -D$(SYS) -DNO_F77

nogo:
	@echo
	@echo " You should first edit the Makefile file in this directory to"
	@echo " reflect your local system and destination directories.  Once"
	@echo " you have done that, type \"make depend install\" in this"
	@echo " directory."
	@echo

depend:
	cp src/Makefile.proto src/Makefile;  cd src;	$(MAKE) depend

install:
	cd src; $(MAKE) 'SYS=$(SYS)' 'SUPREM=$(SUPREM)' \
		'SUPREM=$(SUPREM)' 'BINDIR=$(BINDIR)' \
		'HELPDIR=$(HELPDIR)' 'DATADIR=$(DATADIR)' \
		'CC=$(CC)' 'FC=$(FC)' 'COPT=$(COPT)' 'FOPT=$(FOPT)' \
		'GRARC=$(GRARC)' 'GRINC=$(GRINC)' \
		'GRSRCS=$(GRSRCS)' 'GROBJS=$(GROBJS)' \
		'LIBS=$(LIBS)' 'LOCDEFS=$(LOCDEFS)' 'RANLIB=$(RANLIB)' \
		install

clean:
	cd src; make clean
