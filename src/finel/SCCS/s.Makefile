h59573
s 00000/00000/00072
d D 2.1 93/02/03 14:56:27 suprem45 2 1
c Initial IV.GS release
e
s 00072/00000/00000
d D 1.1 93/02/03 14:53:09 suprem45 1 0
c date and time created 93/02/03 14:53:09 by suprem45
e
u
U
f e 0
t
T
I 1
# %M% %I%   %G% %U%
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
CFLAGS  = $(INCL) -D$(SYS) -DITERATE $(LOCAL) -DNO_F77
CFLAGSO = $(OPT) -c $(INCL) -D$(SYS) -DITERATE $(LOCAL)
SCCS    = 
CFILES  = FEassmb.c FEsolve.c tri6.c tri7.c\
	  tomcal.c interface.r toms508.f
OBJS    = FEassmb.o FEsolve.o tri6.o tri7.o\
	  tomcal.o interface.o toms508.o
SRCS    = $(CFILES) 
DEST	= $(WRKDIR)/lib/libfe.a

$(DEST) : $(OBJS) 
	$(AR) rv $(DEST) $(OBJS)
	$(RANLIB) $(DEST)

tags    : $(SRCS) 
	etags $(SRCS) $(SUBSRC)

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

.r.f    :
	ratfor -C -6c < $*.r > $*.f

csr_symlu.o:
	$(CC) $(CFLAGSO) csr_symlu.c

depend  : 
	sed '/^#LAST-HUMAN-LINE/q' Makefile > Makefile.new
	mkdep $(INCL) $(SRCS) 
	cat .depend >> Makefile.new
	mv Makefile Makefile.old; mv Makefile.new Makefile
	rm .depend

#LAST-HUMAN-LINE
FEassmb.o: FEassmb.c
FEassmb.o: ../include/constant.h
FEassmb.o: ../include/FEgeom.h
FEsolve.o: FEsolve.c
FEsolve.o: ../include/constant.h
FEsolve.o: ../include/global.h
FEsolve.o: ../include/FEgeom.h
FEsolve.o: ../include/FEmath.h
tri6.o: tri6.c
tri7.o: tri7.c
tomcal.o: tomcal.c
tomcal.o: ../include/global.h
E 1
