h42290
s 00000/00000/00213
d D 2.1 93/02/03 15:34:23 suprem45 2 1
c Initial IV.GS release
e
s 00213/00000/00000
d D 1.1 93/02/03 15:33:25 suprem45 1 0
c date and time created 93/02/03 15:33:25 by suprem45
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
SYS = BSD
IDIR	= $(WRKDIR)/include
INCL    = -I$(WRKDIR)/include -I..
CFLAGS  = $(OPT) $(INCL) -D$(SYS) $(LOCAL)
CFILES  = deposit.c etch.c grid.c hinit.c \
	rect_tri.c skel.c sp_edge.c triang.c tridata.c \
	triheur.c dbase.c offset.c surface.c bfill.c rate.c interp.c \
	deloop.c string_etch.c string_off.c
OBJS  = deposit.o etch.o grid.o hinit.o \
	rect_tri.o skel.o sp_edge.o triang.o tridata.o \
	triheur.o dbase.o offset.o surface.o bfill.o rate.o interp.o \
	deloop.o string_etch.o string_off.o
SRCS    = $(CFILES) 
DEST	= $(WRKDIR)/lib/refine.a

$(DEST) : $(OBJS) 
	-rm $(DEST)
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
deposit.o: deposit.c
deposit.o: ../include/global.h
deposit.o: ../include/constant.h
deposit.o: ../include/geom.h
deposit.o: ../include/dbaccess.h
deposit.o: ../include/skel.h
deposit.o: ../include/material.h
deposit.o: ../include/impurity.h
deposit.o: ../include/regrid.h
deposit.o: ../include/refine.h
deposit.o: ../include/poly.h
etch.o: etch.c
etch.o: ../include/global.h
etch.o: ../include/constant.h
etch.o: ../include/geom.h
etch.o: ../include/dbaccess.h
etch.o: ../include/skel.h
etch.o: ../include/regrid.h
etch.o: ../include/material.h
etch.o: ../include/impurity.h
etch.o: ../include/refine.h
etch.o: ../include/skel.h
grid.o: grid.c
grid.o: ../include/global.h
grid.o: ../include/constant.h
grid.o: ../include/dbaccess.h
grid.o: ../include/geom.h
grid.o: ../include/skel.h
grid.o: ../include/skel.h
hinit.o: hinit.c
hinit.o: ../include/global.h
hinit.o: ../include/constant.h
hinit.o: ../include/skel.h
hinit.o: ../include/dbaccess.h
hinit.o: ../include/geom.h
rect_tri.o: rect_tri.c
rect_tri.o: ../include/global.h
rect_tri.o: ../include/constant.h
rect_tri.o: ../include/dbaccess.h
rect_tri.o: ../include/geom.h
rect_tri.o: ../include/skel.h
rect_tri.o: ../include/skel.h
skel.o: skel.c
skel.o: ../include/global.h
skel.o: ../include/constant.h
skel.o: ../include/geom.h
skel.o: ../include/dbaccess.h
skel.o: ../include/skel.h
skel.o: ../include/regrid.h
skel.o: ../include/material.h
skel.o: ../include/impurity.h
skel.o: ../include/refine.h
skel.o: ../include/skel.h
sp_edge.o: sp_edge.c
sp_edge.o: ../include/global.h
sp_edge.o: ../include/constant.h
sp_edge.o: ../include/geom.h
sp_edge.o: ../include/dbaccess.h
sp_edge.o: ../include/skel.h
sp_edge.o: ../include/impurity.h
sp_edge.o: ../include/material.h
sp_edge.o: ../include/skel.h
triang.o: triang.c
triang.o: ../include/global.h
triang.o: ../include/constant.h
triang.o: ../include/dbaccess.h
triang.o: ../include/geom.h
triang.o: ../include/skel.h
tridata.o: tridata.c
tridata.o: ../include/global.h
tridata.o: ../include/constant.h
tridata.o: ../include/dbaccess.h
tridata.o: ../include/geom.h
tridata.o: ../include/skel.h
tridata.o: ../include/skel.h
triheur.o: triheur.c
triheur.o: ../include/global.h
triheur.o: ../include/constant.h
triheur.o: ../include/dbaccess.h
triheur.o: ../include/geom.h
triheur.o: ../include/skel.h
triheur.o: ../include/skel.h
dbase.o: dbase.c
dbase.o: ../include/global.h
dbase.o: ../include/constant.h
dbase.o: ../include/dbaccess.h
dbase.o: ../include/geom.h
dbase.o: ../include/skel.h
dbase.o: ../include/skel.h
dbase.o: ../include/material.h
dbase.o: ../include/regrid.h
offset.o: offset.c
offset.o: ../include/global.h
offset.o: ../include/constant.h
offset.o: ../include/geom.h
offset.o: ../include/dbaccess.h
offset.o: ../include/skel.h
offset.o: ../include/material.h
offset.o: ../include/impurity.h
offset.o: ../include/regrid.h
offset.o: ../include/refine.h
surface.o: surface.c
surface.o: ../include/global.h
surface.o: ../include/constant.h
surface.o: ../include/geom.h
surface.o: ../include/dbaccess.h
surface.o: ../include/skel.h
surface.o: ../include/material.h
surface.o: ../include/impurity.h
surface.o: ../include/regrid.h
surface.o: ../include/refine.h
bfill.o: bfill.c
bfill.o: ../include/global.h
bfill.o: ../include/constant.h
bfill.o: ../include/dbaccess.h
bfill.o: ../include/geom.h
bfill.o: ../include/skel.h
bfill.o: ../include/skel.h
rate.o: rate.c
rate.o: ../include/global.h
rate.o: ../include/constant.h
rate.o: ../include/geom.h
rate.o: ../include/dbaccess.h
rate.o: ../include/skel.h
rate.o: ../include/material.h
rate.o: ../include/impurity.h
rate.o: ../include/regrid.h
rate.o: ../include/refine.h
interp.o: interp.c
interp.o: ../include/global.h
interp.o: ../include/constant.h
interp.o: ../include/geom.h
interp.o: ../include/dbaccess.h
interp.o: ../include/skel.h
interp.o: ../include/regrid.h
interp.o: ../include/material.h
interp.o: ../include/impurity.h
interp.o: ../include/refine.h
interp.o: ../include/skel.h
deloop.o: deloop.c
deloop.o: ../include/global.h
deloop.o: ../include/constant.h
deloop.o: ../include/dbaccess.h
deloop.o: ../include/geom.h
deloop.o: ../include/skel.h
deloop.o: ../include/refine.h
string_etch.o: string_etch.c
string_etch.o: ../include/constant.h
string_etch.o: ../include/material.h
string_etch.o: ../include/refine.h
string_etch.o: ../include/global.h
string_off.o: string_off.c
string_off.o: ../include/global.h
string_off.o: ../include/constant.h
string_off.o: ../include/refine.h
E 1
