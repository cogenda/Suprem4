h28956
s 00000/00000/00054
d D 2.1 93/02/03 15:04:48 suprem45 2 1
c Initial IV.GS release
e
s 00054/00000/00000
d D 1.1 93/02/03 15:03:59 suprem45 1 0
c date and time created 93/02/03 15:03:59 by suprem45
e
u
U
f e 0
t
T
I 1

#	Makefile		Version 1.3	
#	Last Modification:  8/11/89		

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
INCL    = -I$(WRKDIR)/include -I.. -I.
CFLAGS  = $(OPT)  $(INCL) -D$(SYS) -DITERATE $(LOCAL)
CFILES  = fill_grid.c make_grid.c makeframe.c image_write.c window.c
OBJS    = fill_grid.o make_grid.o makeframe.o image_write.o window.o 
SRCS    = $(CFILES) 
DEST	= $(WRKDIR)/lib/imagetool.a

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
E 1
