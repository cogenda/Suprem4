h21705
s 00099/00205/00028
d D 2.1 93/02/04 13:20:54 suprem45 2 1
c Initial IV.GS release
e
s 00233/00000/00000
d D 1.1 93/02/03 15:45:25 suprem45 1 0
c date and time created 93/02/03 15:45:25 by suprem45
e
u
U
f e 0
t
T
I 1
D 2
# Makefile 3.2 3/2/88 09:30:10
E 2
I 2
#----------------------------------------------------------------------
#  Top level Makefile for Suprem-IV.gs
E 2
#
D 2
# Makefile for SUPREMIV 
E 2
I 2
#  Written by Stephen E. Hansen (hansen@gloworm.stanford.edu)
#		Tue Feb  2 09:02:13 PST 1993
E 2
#
D 2
#----------------------------------------
E 2
I 2
#----------------------------------------------------------------------

E 2
# change these lines when porting to other machines
#
I 2
# The SYS variable should be changed to reflect the appropriate system
# type.  BSD and SYSV are the most common.  There are several changes
# that may also be appropriate on vector hardware.
#
E 2
# SYS should be one of {BSD, CONVEX, HPUX, CRAY}
# BSD - "vanilla" BSD4.2 system
# SYSV - vanilla system V unix
# CONVEX - like BSD but uses vectorizing Fortran sparse matrix routines
# HPUX - example of a non-BSD system
# CRAY - like HPUX, but uses vectorizing Fortran sparse matrix routines
D 2

E 2
SYS     = BSD

D 2
FC="f77"
CC=gcc
AR=ar
RANLIB=ranlib
OPT=-g
E 2
I 2
#.. This is only useful if you want the bin, data, and help directories
#.. located immediately below HERE.  Otherwise set BINDIR, DATADIR, and
#.. HELPDIR to the explicit paths.  Make sure before you start that you
#.. have permission to write into the appropriate directories.
E 2

D 2
# LOCAL is any global C compiler flags
E 2
I 2
#.. Where are we?
HERE	= /home/suprem45/rel.9305
E 2

D 2
LOCAL   = -DDEVICE -DHAVE_KSH
#----------------------------------------
E 2
I 2
#.. What are we going to call it and where does all this go?
SUPREM	= suprem
#BINDIR	= /usr/local/bin
BINDIR	= $(HERE)/bin
E 2

D 2
#edit these to change the location of the include files
MANDIR = '-DMANLOC="/home/suprem45/rel.9130/man"'
KEYFILE = '-DKEYLOC="/home/suprem45/rel.9130/suprem.uk"'
MODELFILE = '-DMODELLOC="/home/suprem45/rel.9130/modelrc"'
IMP_FILE = '-DIMPLOC="/home/suprem45/rel.9130/implant/SUP4imp.dat"'
#MANDIR = '-DMANLOC="/home/suprem45/man"'
#KEYFILE = '-DKEYLOC="/home/suprem45/lib/suprem.uk"'
#MODELFILE = '-DMODELLOC="/home/suprem45/lib/modelrc"'
#IMP_FILE = '-DIMPLOC="/home/suprem45/lib/SUP4imp.dat"'
E 2
I 2
#.. Edit these to change the location of the help and data files.
#.. The user man override these locations with the appropriate
#.. envrionment variable (e.g. MANDIR, KEYFILE, MODELFILE, IMPFILE).
#HELPDIR = /usr/local/lib/suprem45/help
#DATADIR = /usr/local/lib/suprem45/data
HELPDIR = $(HERE)/help
DATADIR = $(HERE)/data
E 2

D 2
WRKDIR  = .
#
#
DEST	= suprem
INCL    = -I$(WRKDIR)/include
E 2
I 2
#.. Which C and Fortran Compiler?
CC	= cc
#CC	= gcc
FC	= f77
E 2

D 2
JOBS	= -j 4
MAKE	= gmake $(JOBS)
CFLAGS  = -c $(OPT) $(INCL) -D$(SYS) $(LOCAL) $(MANDIR) $(KEYFILE) $(MODELFILE) $(IMP_FILE)
LFLAGS  = -v -n -q $(INCL) -D$(SYS) $(LOCAL) $(MANDIR) $(KEYFILE) $(MODELFILE) $(IMP_FILE)
LDFLAGS =  -g $(LOCAL)
SUBLIB  = lib/diffuse.a lib/oxide.a lib/mesh.a \
	  lib/math.a lib/shell.a lib/check.a lib/implant.a \
	  lib/refine.a lib/libfe.a lib/device.a lib/imagetool.a \
	  lib/dbase.a lib/misc.a lib/geom.a lib/plot.a \
	  /icl/lib/libedline.a
#	  lib/readline.a
SUBSRC  = diffuse/*.c oxide/*.c mesh/*.c \
	  math/*.c shell/*.c check/*.c implant/*.c \
	  refine/*.c finel/*.c device/*.c \
	  imagetool/*.c dbase/*.c misc/*.c \
	  geom/*.c plot/*.c xsup/*.c gpsup/*.c
SCCS	= 
SRCS	= main.c bsd.c pa.c
CFILES  = main.c bsd.c pa.c
OBJS    = main.o bsd.o pa.o
LSRCS   = main.c bsd.c ps.o $(SUBSRC)
GPDIR	= 
R3GR	= lib/xsup3.a -lX11
R4GR	= lib/xsup4.a -lXaw -lXmu -lXt -lXext -lX11 
GPGR	= lib/gpsup.a -L/icl/lib -lgplot -lhigh2
LIBS    = $(GPGR) -ll -lm -ltermcap
#LIBS    = $(R4GR) -ll -lm -ltermcap
#LIBS    = $(R4GR) -ll -lm -ltermcap /usr/lib/debug/malloc.o
SLIB	= 
E 2
I 2
#.. What optimization level for the compilers?
COPT	= -O3
FOPT	= -O2
#COPT	= -g
#FOPT	= -g
E 2

I 2
#.. One of two graphics options is supported and must be chosen at
#.. compile time.  One is for X windows, (specific to X11R4), which
#.. supports postscript, hpgl, and idraw for hardcopy.  The other
#.. package is for gplot, the graphics package from Stanford which
#.. supports a wide variety of terminals and harcopy devices,
#.. including X windows and Sunview.
E 2

D 2
makeall	: bin lib $(OBJS) incl bin/keyread suprem.uk suprem 
E 2
I 2
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
E 2

D 2
#1) Compiling on top of an executing copy fails on many systems.
#2) If the compile fails, this keeps a working copy around.
suprem 	: libs $(OBJS) 
	date
	time $(CC) $(LDFLAGS) $(OBJS) $(SUBLIB) $(SLIB) $(LIBS) -o suprem 
E 2

D 2
load :
	time $(CC) $(LDFLAGS) $(OBJS) $(SUBLIB) $(SLIB) $(LIBS) -o suprem 
E 2
I 2
#.. Otherwise if you want the gplot plotting package then use this.
#.. Although you need to change the /usr/local/.. to wherever the
#.. package include and archive files are.
GRLIBS	= -L/usr/local/lib -lgplot -lhigh2
GRINC	= -I/usr/local/include
GRARC	= $$(LIBDEST)/gpsup.a
GRSRCS	= gpsup/gpcover.c
GROBJS	= gpsup/gpcover.o
E 2

D 2
purify :
	purify $(CC) $(LDFLAGS) $(OBJS) $(SUBLIB) $(SLIB) $(LIBS) -o suprem 
	
lint    : 
	lint $(LFLAGS) $(LSRCS) lint.c | lint.awk > lint.list
E 2
I 2
# SysV uses termlib
#LIBS	= $(GRLIBS) -ly -ll -lm -ltermlib
# BSD uses termcap
LIBS	= $(GRLIBS) -ly -ll -lm -ltermcap
E 2

D 2
libs    :
	cd shell; $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd check; $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd mesh; $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd geom; $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd plot; $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd misc; $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd math; $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd diffuse; $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd oxide; $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd implant; $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd refine; $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd finel;  $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd device; $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd imagetool; $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd dbase; $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd xsupr4; $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd gpsup; $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
E 2
I 2
#.. SysV users take note!  In src/Makefile.proto, down where the 
#.. archive files are ranlib'd, is some real crufty stuff.  This is to
#.. get around a bug in ranlib that shows up mainly when running gmake
#.. with the multi-job flag.
#.. BSD folks need this.
RANLIB  = ranlib
#.. SysV folks who don't have ranlib should to do this.
#RANLIB  = \#
E 2

D 2
tape    :
	tar -cvf tape main.c bsd.c Makefile modelrc suprem.key README 
	tar -rvf tape include/*.[chrfly] include/Makefile
	tar -rvf tape shell/*.[chrfly] shell/Makefile
	tar -rvf tape check/*.[chrfly] check/Makefile
	tar -rvf tape mesh/*.[chrfly] mesh/Makefile
	tar -rvf tape geom/*.[chrfly] geom/Makefile
	tar -rvf tape plot/*.[chrfly] plot/Makefile
	tar -rvf tape misc/*.[chrfly] misc/Makefile
	tar -rvf tape math/*.[chrfly] math/Makefile
	tar -rvf tape diffuse/*.[chrfly] diffuse/Makefile
	tar -rvf tape oxide/*.[chrfly] oxide/Makefile
	tar -rvf tape implant/*.[chrfly] implant/Makefile implant/SUP4imp.dat
	tar -rvf tape refine/*.[chrfly] refine/Makefile
	tar -rvf tape finel/*.[chrfly]  finel/Makefile
	tar -rvf tape device/*.[chrfly] device/Makefile
	tar -rvf tape imagetool/*.[chrfly] imagetool/Makefile
	tar -rvf tape dbase/*.[chrfly] dbase/Makefile
	tar -rvf tape xsupr4/*.[chrfly] xsupr4/Makefile
	tar -rvf tape xsupr3/*.[chrfly] xsupr3/Makefile
	tar -rvf tape gpsup/*.[chrfly] 
	tar -rvf tape keyread/*.[chrfly] keyread/Makefile
	tar -rvf tape examples
E 2
I 2
#MAKE	= gmake -j4
#MAKE	= gmake
E 2

D 2
release:

bin	:
	mkdir bin
lib	:
	mkdir lib

incl 	:
	cd include; $(MAKE)

tags    : $(SRCS) $(SUBSRC)
	ctags $(SRCS) $(SUBSRC)

bin/keyread : 
	cd keyread; $(MAKE) SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)

suprem.uk : suprem.key 
	bin/keyread < suprem.key 
E 2
I 2
#--- You shouldn't need to mess with anything below here ---
#----------------------------------------------------------------------
#LOCDEFS = -DDEVICE -DHAVE_KSH -D$(SYS)
LOCDEFS	= -DDEVICE -D$(SYS) -DNO_F77
E 2

D 2
clean   : 
	rm -f $(OBJS)
E 2
I 2
nogo:
	@echo
	@echo " You should first edit the Makefile file in this directory to"
	@echo " reflect your local system and destination directories.  Once"
	@echo " you have done that, type \"make depend install\" in this"
	@echo " directory."
	@echo
E 2

D 2
cleanall :
	cd shell; $(MAKE) clean
	cd check; $(MAKE) clean
	cd mesh; $(MAKE) clean
	cd geom; $(MAKE) clean
	cd plot; $(MAKE) clean
	cd misc; $(MAKE) clean
	cd math; $(MAKE) clean
	cd diffuse; $(MAKE) clean
	cd oxide; $(MAKE) clean
	cd implant; $(MAKE) clean
	cd refine; $(MAKE) clean
	cd finel;  $(MAKE) clean
	cd device; $(MAKE) clean
	cd imagetool; $(MAKE) clean
	cd dbase; $(MAKE) clean
	cd xsupr4; $(MAKE) clean
	cd gpsup; $(MAKE) clean
E 2
I 2
depend:
	cd src; $(MAKE) depend
E 2

D 2
depend  : 
	sed '/^#LAST-HUMAN-LINE/q' Makefile > Makefile.new
	mkdep $(INCL) $(SRCS) 
	cat .depend >> Makefile.new
	mv Makefile Makefile.old; mv Makefile.new Makefile
	rm .depend
	
#LAST-HUMAN-LINE
main.o: main.c
main.o: include/global.h
main.o: include/check.h
main.o: include/constant.h
main.o: include/dbaccess.h
main.o: include/geom.h
main.o: include/skel.h
main.o: include/diffuse.h
main.o: include/impurity.h
main.o: include/defect.h
main.o: include/key.h
main.o: include/material.h
main.o: include/matrix.h
main.o: include/plot.h
main.o: include/regrid.h
main.o: include/shell.h
main.o: include/implant.h
main.o: include/sysdep.h
main.o: include/expr.h
main.o: include/refine.h
main.o: include/poly.h
bsd.o: bsd.c
jacob.o: jacob.c
jacob.o: include/constant.h
jacob.o: include/global.h
jacob.o: include/geom.h
jacob.o: include/dbaccess.h
jacob.o: include/skel.h
jacob.o: include/impurity.h
jacob.o: include/matrix.h
jacob.o: include/diffuse.h
pa.o: pa.c
E 2
I 2
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
E 2
E 1
