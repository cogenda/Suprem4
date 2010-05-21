h58448
s 00000/00000/00115
d D 2.1 93/02/03 15:19:41 suprem45 2 1
c Initial IV.GS release
e
s 00115/00000/00000
d D 1.1 93/02/03 15:18:38 suprem45 1 0
c date and time created 93/02/03 15:18:38 by suprem45
e
u
U
f e 0
t
T
I 1
# Makefile 3.2 10/27/87 09:06:14
#
# Makefile for suprem4 test version 
#
# change this for port to other machines
WRKDIR  = ..
#
#define the standard libraries needed for life and the pursuit of happiness
LIBS    = 
#define the location to look for .h files
INCL    = -I$(WRKDIR)/include
CFLAGS  = $(OPT) $(INCL) -D$(SYS) $(LOCAL) -DINITIAL -DPREELIM
VFLAGS  = -O $(INCL) -D$(SYS) $(LOCAL) -DINITIAL -DPREELIM
FFLAGS	= -O2
#update these next two lines to reflect what we are currently working on
SCCS    = s.min_fill.c s.symb.c s.generate.c s.gauss.c s.solblk.c \
	  s.sip.c s.iccg.c s.mxv.c s.blklu.c
CFILES  = min_fill.c symb.c generate.c genrow.c coldata.c
VFILES	= solblk.c mxv.c blklu.c  vector.f
OBJS    = min_fill.o symb.o generate.o solblk.o \
	  mxv.o blklu.o genrow.o coldata.o vector.o
SRCS    = $(CFILES) $(HFILES) $(VFILES)
DEST	= $(WRKDIR)/lib/math.a

$(DEST)	: $(OBJS) 
	$(AR) rv $(DEST) $(OBJS)
	$(RANLIB) $(DEST)

tags    : $(SRCS) 
	ctags $(SRCS) $(SUBSRC)

clean   : 
	rm -f $(OBJS)

version : 
	prs -d":Dt: :F:\n:C:\n\n" $(SCCS)

# set up the vector compilier
#.c.o	:
#	@-for file in $(VFILES); 	\
#	do \
#	    if [ "$*.c" = "$$file" ]; \
#	    then \
#		    vector="TRUE";\
#	    fi; \
#	done; \
#	if [ "$$vector" = "TRUE" ]; \
#	then \
#	    echo "$(VC) -c $(VFLAGS) $*.c";	\
#	    $(VC) -c $(VFLAGS) $*.c;	\
#	else	\
#	    echo "$(CC) -c $(CFLAGS) $*.c";	\
#	    $(CC) -c $(CFLAGS) $*.c;	\
#	fi;


depend  : 
	sed '/^#LAST-HUMAN-LINE/q' Makefile > Makefile.new
	mkdep $(INCL) $(SRCS) 
	cat .depend >> Makefile.new
	mv Makefile Makefile.old; mv Makefile.new Makefile
	rm .depend

#LAST-HUMAN-LINE
min_fill.o: min_fill.c
min_fill.o: ../include/global.h
min_fill.o: ../include/constant.h
min_fill.o: ../include/geom.h
min_fill.o: ../include/dbaccess.h
min_fill.o: ../include/skel.h
symb.o: symb.c
symb.o: ../include/global.h
symb.o: ../include/constant.h
symb.o: ../include/geom.h
symb.o: ../include/dbaccess.h
symb.o: ../include/skel.h
symb.o: ../include/diffuse.h
symb.o: ../include/impurity.h
symb.o: ../include/material.h
symb.o: ../include/matrix.h
symb.o: ../include/FEmath.h
symb.o: ../include/poly.h
generate.o: generate.c
generate.o: ../include/global.h
genrow.o: genrow.c
genrow.o: ../include/global.h
genrow.o: ../include/constant.h
genrow.o: ../include/geom.h
genrow.o: ../include/dbaccess.h
genrow.o: ../include/skel.h
genrow.o: ../include/matrix.h
coldata.o: coldata.c
coldata.o: ../include/global.h
coldata.o: ../include/constant.h
coldata.o: ../include/matrix.h
solblk.o: solblk.c
solblk.o: ../include/global.h
solblk.o: ../include/constant.h
solblk.o: ../include/sysdep.h
solblk.o: ../include/diffuse.h
solblk.o: ../include/impurity.h
solblk.o: ../include/matrix.h
mxv.o: mxv.c
mxv.o: ../include/global.h
mxv.o: ../include/constant.h
mxv.o: ../include/sysdep.h
mxv.o: ../include/impurity.h
mxv.o: ../include/matrix.h
blklu.o: blklu.c
blklu.o: ../include/global.h
blklu.o: ../include/constant.h
blklu.o: ../include/sysdep.h
blklu.o: ../include/diffuse.h
blklu.o: ../include/impurity.h
blklu.o: ../include/matrix.h
E 1
