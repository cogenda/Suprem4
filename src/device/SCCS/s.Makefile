h19545
s 00000/00000/00147
d D 2.1 93/02/03 14:39:12 suprem45 2 1
c Initial IV.GS release
e
s 00147/00000/00000
d D 1.1 93/02/03 14:37:26 suprem45 1 0
c date and time created 93/02/03 14:37:26 by suprem45
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
INCL    = -I$(WRKDIR)/include -I..
CFLAGS  = $(OPT) $(INCL) -D$(SYS) -DDEBUG $(LOCAL)
LFLAGS  = $(INCL) -D$(SYS) -DDEBUG $(LOCAL)
CFILES  = contact.c dev_blkset.c device.c dev_prep.c Psi.c Electron.c Hole.c  \
	dev_solve.c circuit.c
OBJS    = contact.o dev_blkset.o device.o dev_prep.o Psi.o Electron.o Hole.o \
	dev_solve.o circuit.o
SRCS    = $(CFILES) 
DEST	= $(WRKDIR)/lib/device.a

$(DEST) : $(OBJS) 
	-rm $(DEST)
	$(AR) rv $(DEST) $(OBJS)
	$(RANLIB) $(DEST)

lint    : 
	lint $(LFLAGS) -DSTATIC_ALLOCATION_TIME $(SRCS) > lint.list

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

depend  : 
	sed '/^#LAST-HUMAN-LINE/q' Makefile > Makefile.new
	mkdep $(INCL) $(SRCS) 
	cat .depend >> Makefile.new
	mv Makefile Makefile.old; mv Makefile.new Makefile
	rm .depend
	
#LAST-HUMAN-LINE
contact.o: contact.c
contact.o: ../include/global.h
contact.o: ../include/constant.h
contact.o: ../include/geom.h
contact.o: ../include/dbaccess.h
contact.o: ../include/skel.h
contact.o: ../include/material.h
contact.o: ../include/device.h
dev_blkset.o: dev_blkset.c
dev_blkset.o: ../include/global.h
dev_blkset.o: ../include/constant.h
dev_blkset.o: ../include/geom.h
dev_blkset.o: ../include/dbaccess.h
dev_blkset.o: ../include/skel.h
dev_blkset.o: ../include/impurity.h
dev_blkset.o: ../include/device.h
dev_blkset.o: ../include/material.h
dev_blkset.o: ../include/diffuse.h
dev_blkset.o: ../include/matrix.h
device.o: device.c
device.o: ../include/global.h
device.o: ../include/constant.h
device.o: ../include/geom.h
device.o: ../include/dbaccess.h
device.o: ../include/skel.h
device.o: ../include/material.h
device.o: ../include/matrix.h
device.o: ../include/impurity.h
device.o: ../include/diffuse.h
device.o: ../include/device.h
dev_prep.o: dev_prep.c
dev_prep.o: ../include/global.h
dev_prep.o: ../include/constant.h
dev_prep.o: ../include/geom.h
dev_prep.o: ../include/dbaccess.h
dev_prep.o: ../include/skel.h
dev_prep.o: ../include/material.h
dev_prep.o: ../include/device.h
dev_prep.o: ../include/impurity.h
dev_prep.o: ../include/matrix.h
Psi.o: Psi.c
Psi.o: ../include/global.h
Psi.o: ../include/constant.h
Psi.o: ../include/geom.h
Psi.o: ../include/dbaccess.h
Psi.o: ../include/skel.h
Psi.o: ../include/impurity.h
Psi.o: ../include/material.h
Psi.o: ../include/matrix.h
Psi.o: ../include/diffuse.h
Psi.o: ../include/device.h
Electron.o: Electron.c
Electron.o: ../include/global.h
Electron.o: ../include/constant.h
Electron.o: ../include/geom.h
Electron.o: ../include/dbaccess.h
Electron.o: ../include/skel.h
Electron.o: ../include/impurity.h
Electron.o: ../include/material.h
Electron.o: ../include/device.h
Electron.o: ../include/matrix.h
Electron.o: ../include/diffuse.h
Hole.o: Hole.c
Hole.o: ../include/global.h
Hole.o: ../include/constant.h
Hole.o: ../include/geom.h
Hole.o: ../include/dbaccess.h
Hole.o: ../include/skel.h
Hole.o: ../include/impurity.h
Hole.o: ../include/material.h
Hole.o: ../include/device.h
dev_solve.o: dev_solve.c
dev_solve.o: ../include/global.h
dev_solve.o: ../include/constant.h
dev_solve.o: ../include/geom.h
dev_solve.o: ../include/dbaccess.h
dev_solve.o: ../include/skel.h
dev_solve.o: ../include/diffuse.h
dev_solve.o: ../include/impurity.h
dev_solve.o: ../include/material.h
dev_solve.o: ../include/matrix.h
dev_solve.o: ../include/device.h
circuit.o: circuit.c
circuit.o: ../include/global.h
circuit.o: ../include/constant.h
circuit.o: ../include/geom.h
circuit.o: ../include/dbaccess.h
circuit.o: ../include/skel.h
circuit.o: ../include/material.h
circuit.o: ../include/device.h
circuit.o: ../include/impurity.h
circuit.o: ../include/matrix.h
circuit.o: ../include/diffuse.h
E 1
