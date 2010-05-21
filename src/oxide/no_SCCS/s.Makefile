h64461
s 00000/00000/00167
d D 2.1 93/02/03 15:27:50 suprem45 2 1
c Initial IV.GS release
e
s 00167/00000/00000
d D 1.1 93/02/03 15:27:03 suprem45 1 0
c date and time created 93/02/03 15:27:03 by suprem45
e
u
U
f e 0
t
T
I 1
# Makefile 3.1 9/14/87 10:54:26
#
# Makefile for suprem4 test version 
#
# change this for the port to other machines
WRKDIR  = ..
#
#
#define the standard libraries needed for life and the pursuit of happiness
LDFLAGS = 
#define the location to look for .h files
IDIR	= $(WRKDIR)/include
INCL    = -I$(WRKDIR)/include -I..
CFLAGS  = $(OPT) $(INCL) -D$(SYS) -DITERATE $(LOCAL)
CFILES  = oxrate.c oxgrow.c oxide_vel.c \
	  Oxidant.c erfit.c vert.c elast.c viscous.c mater.c\
	  FEbc.c FEconvert.c triox.c coeffox.c
OBJS    = oxrate.o oxgrow.o oxide_vel.o \
	  Oxidant.o erfit.o vert.o elast.o viscous.o mater.o\
	  FEbc.o FEconvert.o triox.o coeffox.o
SRCS    = $(CFILES) 
DEST	= $(WRKDIR)/lib/oxide.a

$(DEST) : $(OBJS) 
	$(AR) rv $(DEST) $(OBJS)
	$(RANLIB) $(DEST)

tags    : $(SRCS) 
	ctags $(SRCS) $(SUBSRC)

lint    : $(SRCS)
	lint $(CFLAGS) $(SRCS)

clean   : 
	rm -f $(OBJS) 

.c~.o	:
	$(GET) $(GFLAGS) $<
	$(CC) $(CFLAGS) $*.c

.c~.c	:
	$(GET) $(GFLAGS) $<

.h~.h	:
	$(GET) $(GFLAGS) $<

depend  : 
	sed '/^#LAST-HUMAN-LINE/q' Makefile > Makefile.new
	mkdep $(INCL) $(SRCS) 
	cat .depend >> Makefile.new
	mv Makefile Makefile.old; mv Makefile.new Makefile
	rm .depend

#LAST-HUMAN-LINE
oxrate.o: oxrate.c
oxrate.o: ../include/global.h
oxrate.o: ../include/constant.h
oxrate.o: ../include/geom.h
oxrate.o: ../include/dbaccess.h
oxrate.o: ../include/skel.h
oxrate.o: ../include/material.h
oxrate.o: ../include/impurity.h
oxgrow.o: oxgrow.c
oxgrow.o: ../include/global.h
oxgrow.o: ../include/constant.h
oxgrow.o: ../include/geom.h
oxgrow.o: ../include/dbaccess.h
oxgrow.o: ../include/skel.h
oxgrow.o: ../include/material.h
oxgrow.o: ../include/impurity.h
oxgrow.o: ../include/matrix.h
oxide_vel.o: oxide_vel.c
oxide_vel.o: ../include/global.h
oxide_vel.o: ../include/constant.h
oxide_vel.o: ../include/material.h
oxide_vel.o: ../include/impurity.h
oxide_vel.o: ../include/diffuse.h
oxide_vel.o: ../include/geom.h
oxide_vel.o: ../include/dbaccess.h
oxide_vel.o: ../include/skel.h
oxide_vel.o: ../include/FEgeom.h
Oxidant.o: Oxidant.c
Oxidant.o: ../include/global.h
Oxidant.o: ../include/constant.h
Oxidant.o: ../include/geom.h
Oxidant.o: ../include/dbaccess.h
Oxidant.o: ../include/skel.h
Oxidant.o: ../include/impurity.h
Oxidant.o: ../include/matrix.h
Oxidant.o: ../include/material.h
Oxidant.o: ../include/diffuse.h
Oxidant.o: ../include/FEgeom.h
Oxidant.o: ../include/FEmath.h
erfit.o: erfit.c
erfit.o: ../include/global.h
erfit.o: ../include/constant.h
erfit.o: ../include/geom.h
erfit.o: ../include/dbaccess.h
erfit.o: ../include/skel.h
erfit.o: ../include/material.h
erfit.o: ../include/impurity.h
vert.o: vert.c
vert.o: ../include/global.h
vert.o: ../include/constant.h
vert.o: ../include/geom.h
vert.o: ../include/dbaccess.h
vert.o: ../include/skel.h
vert.o: ../include/material.h
vert.o: ../include/impurity.h
vert.o: ../include/diffuse.h
elast.o: elast.c
elast.o: ../include/global.h
elast.o: ../include/constant.h
elast.o: ../include/geom.h
elast.o: ../include/dbaccess.h
elast.o: ../include/skel.h
elast.o: ../include/impurity.h
elast.o: ../include/material.h
viscous.o: viscous.c
viscous.o: ../include/global.h
viscous.o: ../include/constant.h
viscous.o: ../include/geom.h
viscous.o: ../include/dbaccess.h
viscous.o: ../include/skel.h
viscous.o: ../include/FEgeom.h
viscous.o: ../include/material.h
viscous.o: ../include/FEmath.h
mater.o: mater.c
mater.o: ../include/global.h
mater.o: ../include/constant.h
mater.o: ../include/geom.h
mater.o: ../include/dbaccess.h
mater.o: ../include/skel.h
mater.o: ../include/material.h
FEbc.o: FEbc.c
FEbc.o: ../include/constant.h
FEbc.o: ../include/geom.h
FEbc.o: ../include/dbaccess.h
FEbc.o: ../include/skel.h
FEbc.o: ../include/FEgeom.h
FEbc.o: ../include/material.h
FEconvert.o: FEconvert.c
FEconvert.o: ../include/constant.h
FEconvert.o: ../include/global.h
FEconvert.o: ../include/geom.h
FEconvert.o: ../include/dbaccess.h
FEconvert.o: ../include/skel.h
FEconvert.o: ../include/FEgeom.h
FEconvert.o: ../include/material.h
FEconvert.o: ../include/impurity.h
FEconvert.o: ../include/diffuse.h
triox.o: triox.c
triox.o: ../include/global.h
triox.o: ../include/constant.h
triox.o: ../include/FEgeom.h
triox.o: ../include/FEmath.h
triox.o: ../include/constant.h
triox.o: ../include/material.h
coeffox.o: coeffox.c
coeffox.o: ../include/global.h
coeffox.o: ../include/constant.h
coeffox.o: ../include/geom.h
coeffox.o: ../include/dbaccess.h
coeffox.o: ../include/skel.h
coeffox.o: ../include/impurity.h
coeffox.o: ../include/material.h
coeffox.o: ../include/diffuse.h
E 1
