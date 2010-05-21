h60321
s 00000/00000/00068
d D 2.1 93/02/03 14:59:53 suprem45 2 1
c Initial IV.GS release
e
s 00068/00000/00000
d D 1.1 93/02/03 14:58:25 suprem45 1 0
c date and time created 93/02/03 14:58:25 by suprem45
e
u
U
f e 0
t
T
I 1
# Makefile 3.1 9/14/87 10:39:29
#
# Makefile for the SupremIV parameter parser and checker.  
#
# change this for port to other machines
WRKDIR  = ..
#
# set up destination directories and sources
DESTLIB = $(WRKDIR)/lib/geom.a
INCL 	= -I$(WRKDIR)/include

CFLAGS	= $(OPT) $(INCL) -D$(SYS) $(LOCAL)
HFILES  = 
SRCS    = limits.c misc.c tnabor.c 
CFILES  = limits.c misc.c tnabor.c 
OBJS	= limits.o misc.o tnabor.o 

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
limits.o: limits.c
limits.o: ../include/global.h
limits.o: ../include/constant.h
limits.o: ../include/geom.h
limits.o: ../include/dbaccess.h
limits.o: ../include/skel.h
misc.o: misc.c
misc.o: ../include/global.h
misc.o: ../include/constant.h
misc.o: ../include/geom.h
misc.o: ../include/dbaccess.h
misc.o: ../include/skel.h
tnabor.o: tnabor.c
tnabor.o: ../include/global.h
tnabor.o: ../include/constant.h
tnabor.o: ../include/geom.h
tnabor.o: ../include/dbaccess.h
tnabor.o: ../include/skel.h
tnabor.o: ../include/regrid.h
E 1
