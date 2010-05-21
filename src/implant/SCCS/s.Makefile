h63384
s 00000/00000/00135
d D 2.1 93/02/03 15:10:29 suprem45 2 1
c Initial IV.GS release
e
s 00135/00000/00000
d D 1.1 93/02/03 15:08:27 suprem45 1 0
c date and time created 93/02/03 15:08:27 by suprem45
e
u
U
f e 0
t
T
I 1
#
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
INCL    = -I$(WRKDIR)/include -I.. -I.
CFLAGS  = $(OPT) $(INCL) -D$(SYS) $(LOCAL)
CFILES	= fgtmom.c imp_gtprs.c imp_lat.c imp_prson.c imp_qeqv.c \
	imp_qpgauss.c imp_qpprs.c imp_setprs.c imp_vert.c imp_zeqv.c implant.c \
	pearson.c damage.c surf.c
OBJS  	= fgtmom.o imp_gtprs.o imp_lat.o imp_prson.o imp_qeqv.o \
	imp_qpgauss.o imp_qpprs.o imp_setprs.o imp_vert.o imp_zeqv.o implant.o \
	pearson.o damage.o surf.o
SRCS    = $(CFILES) 
DEST	= $(WRKDIR)/lib/implant.a

$(DEST) : $(OBJS) 
	$(AR) rv $(DEST) $(OBJS)
	$(RANLIB) $(DEST)

tags    : $(SRCS) 
	ctags $(SRCS) $(SUBSRC)

clean   : 
	rm -f $(OBJS) 

.c~.o	:
	$(GET) $(GFLAGS) $<
	$(CC) $(CFLAGS) $*.c

.c~.c	:
	$(GET) $(GFLAGS) $<

.h~.h	:
	$(GET) $(GFLAGS) $<

lint	:
	lint $(INCL) -D$(SYS) -DITERATE $(CFILES)

depend  : 
	sed '/^#LAST-HUMAN-LINE/q' Makefile > Makefile.new
	mkdep $(INCL) $(SRCS) 
	cat .depend >> Makefile.new
	mv Makefile Makefile.old; mv Makefile.new Makefile
	rm .depend
	
#LAST-HUMAN-LINE
fgtmom.o: fgtmom.c
fgtmom.o: ../include/sysdep.h
imp_gtprs.o: imp_gtprs.c
imp_gtprs.o: ../include/global.h
imp_gtprs.o: ../include/constant.h
imp_gtprs.o: ../include/implant.h
imp_lat.o: imp_lat.c
imp_lat.o: ../include/global.h
imp_lat.o: ../include/constant.h
imp_lat.o: ../include/implant.h
imp_prson.o: imp_prson.c
imp_prson.o: ../include/global.h
imp_prson.o: ../include/constant.h
imp_prson.o: ../include/implant.h
imp_qeqv.o: imp_qeqv.c
imp_qeqv.o: ../include/global.h
imp_qeqv.o: ../include/constant.h
imp_qeqv.o: ../include/implant.h
imp_qpgauss.o: imp_qpgauss.c
imp_qpgauss.o: ../include/global.h
imp_qpgauss.o: ../include/constant.h
imp_qpgauss.o: ../include/implant.h
imp_qpprs.o: imp_qpprs.c
imp_qpprs.o: ../include/global.h
imp_qpprs.o: ../include/constant.h
imp_qpprs.o: ../include/implant.h
imp_setprs.o: imp_setprs.c
imp_setprs.o: ../include/constant.h
imp_setprs.o: ../include/global.h
imp_setprs.o: ../include/impurity.h
imp_setprs.o: ../include/material.h
imp_setprs.o: ../include/implant.h
imp_vert.o: imp_vert.c
imp_vert.o: ../include/global.h
imp_vert.o: ../include/constant.h
imp_vert.o: ../include/implant.h
imp_zeqv.o: imp_zeqv.c
imp_zeqv.o: ../include/global.h
imp_zeqv.o: ../include/constant.h
imp_zeqv.o: ../include/implant.h
implant.o: implant.c
implant.o: ../include/global.h
implant.o: ../include/constant.h
implant.o: ../include/geom.h
implant.o: ../include/dbaccess.h
implant.o: ../include/skel.h
implant.o: ../include/impurity.h
implant.o: ../include/material.h
implant.o: ../include/implant.h
implant.o: ../include/matrix.h
implant.o: ../include/defect.h
implant.o: ../include/expr.h
pearson.o: pearson.c
pearson.o: ../include/global.h
pearson.o: ../include/constant.h
pearson.o: ../include/geom.h
pearson.o: ../include/dbaccess.h
pearson.o: ../include/skel.h
pearson.o: ../include/impurity.h
pearson.o: ../include/material.h
pearson.o: ../include/implant.h
damage.o: damage.c
damage.o: ../include/sysdep.h
damage.o: ../include/constant.h
damage.o: ../include/global.h
damage.o: ../include/geom.h
damage.o: ../include/dbaccess.h
damage.o: ../include/skel.h
damage.o: ../include/material.h
damage.o: ../include/implant.h
damage.o: ../include/impurity.h
surf.o: surf.c
surf.o: ../include/sysdep.h
surf.o: ../include/constant.h
surf.o: ../include/global.h
surf.o: ../include/geom.h
surf.o: ../include/dbaccess.h
surf.o: ../include/skel.h
surf.o: ../include/material.h
surf.o: ../include/implant.h
surf.o: ../include/refine.h
E 1
