h45363
s 00216/00000/00000
d D 1.1 93/02/03 13:50:39 suprem45 1 0
c date and time created 93/02/03 13:50:39 by suprem45
e
u
U
f e 0
t
T
I 1
# Makefile 3.2 3/2/88 09:30:10
#
# Makefile for SUPREMIV 
#
#----------------------------------------
# change these lines when porting to other machines
#
# SYS should be one of {BSD, CONVEX, HPUX, CRAY}
# BSD - "vanilla" BSD4.2 system
# SYSV - vanilla system V unix
# CONVEX - like BSD but uses vectorizing Fortran sparse matrix routines
# HPUX - example of a non-BSD system
# CRAY - like HPUX, but uses vectorizing Fortran sparse matrix routines

SYS     = BSD

FC="f77"
CC=cc
AR=ar
RANLIB=ranlib
OPT=-g

# LOCAL is any global C compiler flags

LOCAL   = -DDEVICE -DHAVE_KSH
#----------------------------------------

#edit these to change the location of the include files
MANDIR = '-DMANLOC="/home/suprem45/rel.9130/man"'
KEYFILE = '-DKEYLOC="/home/suprem45/rel.9130/suprem.uk"'
MODELFILE = '-DMODELLOC="/home/suprem45/rel.9130/modelrc"'
IMP_FILE = '-DIMPLOC="/home/suprem45/rel.9130/implant/SUP4imp.dat"'

WRKDIR  = .
#
#
DEST	= suprem
INCL    = -I$(WRKDIR)/include

CFLAGS  = -c $(OPT) $(INCL) -D$(SYS) $(LOCAL) $(MANDIR) $(KEYFILE) $(MODELFILE) $(IMP_FILE)
LFLAGS  = -v -n -q $(INCL) -D$(SYS) $(LOCAL) $(MANDIR) $(KEYFILE) $(MODELFILE) $(IMP_FILE)
LDFLAGS =  -g $(LOCAL)
SUBLIB  = lib/diffuse.a lib/oxide.a lib/mesh.a \
	  lib/math.a lib/shell.a lib/check.a lib/implant.a \
	  lib/refine.a lib/libfe.a lib/device.a lib/imagetool.a \
	  lib/dbase.a lib/misc.a lib/geom.a lib/plot.a lib/readline.a
SUBSRC  = diffuse/*.c oxide/*.c mesh/*.c \
	  math/*.c shell/*.c check/*.c implant/*.c \
	  refine/*.c finel/*.c device/*.c \
	  imagetool/*.c dbase/*.c misc/*.c \
	  geom/*.c plot/*.c xsup/*.c
SCCS	= 
SRCS	= main.c bsd.c 
CFILES  = main.c bsd.c 
OBJS    = main.o bsd.o 
LSRCS   = main.c bsd.c $(SUBSRC)
GPDIR	= 
R3GR	= lib/xsup3.a -lX11
R4GR	= lib/xsup4.a -lXaw -lXmu -lXt -lXext -lX11 
GPGR	= lib/gpsup.a
LIBS    = $(R4GR) -ll -lm -ltermcap
SLIB	= 


makeall	: bin lib $(OBJS) incl bin/keyread suprem.uk suprem 

#1) Compiling on top of an executing copy fails on many systems.
#2) If the compile fails, this keeps a working copy around.
suprem 	: libs $(OBJS) 
	date
	time $(CC) $(LDFLAGS) $(OBJS) $(SUBLIB) $(SLIB) $(LIBS) -o suprem 

load :
	time $(CC) $(LDFLAGS) $(OBJS) $(SUBLIB) $(SLIB) $(LIBS) -o suprem 
	
lint    : 
	lint $(LFLAGS) $(LSRCS) lint.c | lint.awk > lint.list

libs    :
	cd shell; make SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd check; make SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd mesh; make SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd geom; make SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd plot; make SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd misc; make SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd math; make SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd diffuse; make SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd oxide; make SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd implant; make SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd refine; make SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd finel;  make SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd device; make SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd imagetool; make SYS=$(SYS) LOCAL='$(LOCAL)'  OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd dbase; make SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)
	cd xsupr4; make SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)

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

release:

bin	:
	mkdir bin
lib	:
	mkdir lib

incl 	:
	cd include; make

tags    : $(SRCS) $(SUBSRC)
	ctags $(SRCS) $(SUBSRC)

bin/keyread : 
	cd keyread; make SYS=$(SYS) LOCAL='$(LOCAL)' OPT='$(OPT)' CC=$(CC) \
		AR=$(AR) RANLIB=$(RANLIB)

suprem.uk : suprem.key 
	bin/keyread < suprem.key 

clean   : 
	rm -f $(OBJS)

cleanall :
	cd shell; make clean
	cd check; make clean
	cd mesh; make clean
	cd geom; make clean
	cd plot; make clean
	cd misc; make clean
	cd math; make clean
	cd diffuse; make clean
	cd oxide; make clean
	cd implant; make clean
	cd refine; make clean
	cd finel;  make clean
	cd device; make clean
	cd imagetool; make clean
	cd dbase; make clean
	cd xsupr4; make clean

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
E 1
