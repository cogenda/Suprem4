h41538
s 00000/00000/00061
d D 2.1 93/02/03 15:03:12 suprem45 2 1
c Initial IV.GS release
e
s 00061/00000/00000
d D 1.1 93/02/03 15:02:39 suprem45 1 0
c date and time created 93/02/03 15:02:39 by suprem45
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
DESTLIB = $(WRKDIR)/lib/gpsup.a
INCL 	= -I$(WRKDIR)/include

CFLAGS	= $(OPT) $(INCL) -D$(SYS) $(LOCAL)
HFILES  = 
SRCS    = gplot.c
OBJS    = gplot.o

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

depend	:
	cp Makefile Makefile.new
	mkdep -f Makefile.new $(INCL) $(SRCS)
	mv Makefile Makefile.old; mv Makefile.new Makefile

#depend  : 
#	sed '/^#LAST-HUMAN-LINE/q' Makefile > Makefile.new
#	mkdep $(INCL) $(SRCS) 
#	cat .depend >> Makefile.new
#	mv Makefile Makefile.old; mv Makefile.new Makefile
#	rm .depend

#LAST-HUMAN-LINE
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

gplot.o: gplot.c /usr/include/stdio.h /usr/include/math.h
gplot.o: /usr/include/floatingpoint.h /usr/include/sys/ieeefp.h
gplot.o: ../include/gplot.h

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
E 1
