h35484
s 00000/00000/00131
d D 2.1 93/02/03 15:29:28 suprem45 2 1
c Initial IV.GS release
e
s 00131/00000/00000
d D 1.1 93/02/03 15:28:43 suprem45 1 0
c date and time created 93/02/03 15:28:43 by suprem45
e
u
U
f e 0
t
T
I 1
# Makefile 3.1 9/14/87 10:55:49
#
# Makefile for the SupremIV plot software.
#
# change this for port to other machines
WRKDIR  = ..
#
# set up destination directories and sources
DESTLIB = $(WRKDIR)/lib/plot.a
INCL 	= -I$(WRKDIR)/include

CFLAGS	= $(OPT) $(INCL) -D$(SYS) $(LOCAL)
HFILES  = 
SRCS    = contour.c do_1d.c intersect.c matedge.c gplot.c \
	material.c option.c plot.c plot_1d.c plot_2d.c print_1d.c select.c
OBJS    = contour.o do_1d.o intersect.o matedge.o gplot.o \
	material.o option.o plot.o plot_1d.o plot_2d.o print_1d.o select.o

$(DESTLIB) : $(OBJS)
	$(AR) rv $(DESTLIB) $(OBJS)
	$(RANLIB) $(DESTLIB)

tags    : $(SRCS)
	ctags $(SRCS)

clean   : 
	rm -f $(OBJS) 

version : 
	prs -d":Dt: :F:\n:C:\n\n" $(SCCS)

# change the default get line
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
contour.o: contour.c
contour.o: ../include/global.h
contour.o: ../include/constant.h
contour.o: ../include/geom.h
contour.o: ../include/dbaccess.h
contour.o: ../include/skel.h
contour.o: ../include/plot.h
contour.o: ../include/material.h
do_1d.o: do_1d.c
do_1d.o: ../include/global.h
do_1d.o: ../include/constant.h
do_1d.o: ../include/geom.h
do_1d.o: ../include/dbaccess.h
do_1d.o: ../include/skel.h
do_1d.o: ../include/plot.h
do_1d.o: ../include/material.h
intersect.o: intersect.c
intersect.o: ../include/global.h
intersect.o: ../include/constant.h
matedge.o: matedge.c
matedge.o: ../include/global.h
matedge.o: ../include/constant.h
matedge.o: ../include/geom.h
matedge.o: ../include/dbaccess.h
matedge.o: ../include/skel.h
matedge.o: ../include/material.h
gplot.o: gplot.c
material.o: material.c
material.o: ../include/global.h
material.o: ../include/constant.h
material.o: ../include/dbaccess.h
material.o: ../include/geom.h
material.o: ../include/skel.h
option.o: option.c
option.o: ../include/global.h
plot.o: plot.c
plot.o: ../include/global.h
plot.o: ../include/constant.h
plot.o: ../include/skel.h
plot.o: ../include/dbaccess.h
plot.o: ../include/geom.h
plot.o: ../include/plot.h
plot_1d.o: plot_1d.c
plot_1d.o: ../include/global.h
plot_1d.o: ../include/constant.h
plot_1d.o: ../include/geom.h
plot_1d.o: ../include/dbaccess.h
plot_1d.o: ../include/skel.h
plot_1d.o: ../include/plot.h
plot_1d.o: ../include/material.h
plot_2d.o: plot_2d.c
plot_2d.o: ../include/global.h
plot_2d.o: ../include/constant.h
plot_2d.o: ../include/impurity.h
plot_2d.o: ../include/material.h
plot_2d.o: ../include/geom.h
plot_2d.o: ../include/dbaccess.h
plot_2d.o: ../include/skel.h
plot_2d.o: ../include/regrid.h
plot_2d.o: ../include/plot.h
print_1d.o: print_1d.c
print_1d.o: ../include/global.h
print_1d.o: ../include/constant.h
print_1d.o: ../include/geom.h
print_1d.o: ../include/dbaccess.h
print_1d.o: ../include/skel.h
print_1d.o: ../include/plot.h
print_1d.o: ../include/material.h
select.o: select.c
select.o: ../include/global.h
select.o: ../include/constant.h
select.o: ../include/dbaccess.h
select.o: ../include/geom.h
select.o: ../include/skel.h
select.o: ../include/expr.h
select.o: ../include/material.h
select.o: ../include/impurity.h
select.o: ../include/diffuse.h
select.o: ../include/plot.h
select.o: ../include/device.h
select.o: ../include/sysdep.h
E 1
