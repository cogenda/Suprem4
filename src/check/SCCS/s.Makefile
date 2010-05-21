h19757
s 00000/00000/00102
d D 2.1 93/02/03 14:12:02 suprem45 2 1
c Initial IV.GS release
c *** CHANGED *** 93/02/03 14:15:31 suprem45
c 
e
s 00102/00000/00000
d D 1.1 93/02/03 14:08:35 suprem45 1 0
c date and time created 93/02/03 14:08:35 by suprem45
e
u
U
f e 0
t
T
I 1
# Makefile 3.1 87/09/14 10:33:42
#
# Makefile for the SupremIV parameter parser and checker.  
#
# change when porting to other systems
WRKDIR  = ..
#
# set up destination directories and sources
DESTLIB = $(WRKDIR)/lib/check.a
INCL 	= -I$(WRKDIR)/include

CFLAGS	= $(OPT) $(INCL) -D$(SYS) $(LOCAL)
HFILES  = check.h
SCCS    = s.verify.c s.split.c s.check.c \
	  s.parse.c s.reduce.c s.eval.c s.lexical.c s.vector.c
SRCS    = verify.c split.c check.c \
	  parse.c reduce.c eval.c lexical.c vector.c 
CFILES  = verify.c split.c check.c \
	  parse.c reduce.c eval.c lexical.c vector.c 
OBJS	= verify.o split.o check.o \
	  parse.o reduce.o eval.o lexical.o vector.o 

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
verify.o: verify.c
verify.o: ../include/global.h
verify.o: ../include/sysdep.h
verify.o: ../include/check.h
verify.o: ../include/expr.h
split.o: split.c
split.o: ../include/global.h
split.o: ../include/check.h
check.o: check.c
check.o: ../include/global.h
check.o: ../include/check.h
parse.o: parse.c
parse.o: ../include/global.h
parse.o: ../include/expr.h
reduce.o: reduce.c
reduce.o: ../include/global.h
reduce.o: ../include/expr.h
eval.o: eval.c
eval.o: ../include/global.h
eval.o: ../include/constant.h
eval.o: ../include/dbaccess.h
eval.o: ../include/geom.h
eval.o: ../include/skel.h
eval.o: ../include/expr.h
lexical.o: lexical.c
lexical.o: ../include/constant.h
lexical.o: ../include/global.h
lexical.o: ../include/sysdep.h
lexical.o: ../include/expr.h
lexical.o: ../include/material.h
vector.o: vector.c
vector.o: ../include/global.h
vector.o: ../include/constant.h
vector.o: ../include/dbaccess.h
vector.o: ../include/geom.h
vector.o: ../include/skel.h
vector.o: ../include/expr.h
vector.o: ../include/material.h
vector.o: ../include/impurity.h
vector.o: ../include/diffuse.h
sint.o: sint.c
sint.o: ../include/global.h
sint.o: ../include/constant.h
sint.o: ../include/dbaccess.h
sint.o: ../include/geom.h
sint.o: ../include/skel.h
sint.o: ../include/expr.h
sint.o: ../include/plot.h
E 1
