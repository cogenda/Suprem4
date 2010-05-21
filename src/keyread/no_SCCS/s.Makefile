h41357
s 00039/00030/00034
d D 2.1 93/02/03 15:16:44 suprem45 2 1
c Initial IV.GS release
e
s 00064/00000/00000
d D 1.1 93/02/03 15:14:47 suprem45 1 0
c date and time created 93/02/03 15:14:47 by suprem45
e
u
U
f e 0
t
T
I 1
D 2
# Makefile 3.1 9/14/87 10:43:51
#
E 2
# Makefile for the SupremIV formatted key file reader.
#
D 2
# change this for port to other machines
WRKDIR  = ..
E 2
I 2
# Makefile 3.1 9/14/87 10:43:51
# modified: Mon Feb  1 11:36:39 PST 1993, Stephen E. Hansen 
E 2
#
D 2
# set up destination directories and sources
DEST    = ../bin/keyread
INCL 	= -I$(WRKDIR)/include
E 2

D 2
CFLAGS	= $(OPT) $(INCL) -D$(SYS) $(LOCAL)
LDFLAGS = $(LOCAL)
E 2
I 2
SYS     = BSD
LOCDEFS = -D$(SYS)
INCLUDE = -I../include
CFLAGS  = -c $(COPT) $(LOCDEFS) $(INCLUDE)

E 2
HFILES  = 
SCCS	= s.expand.c s.lexical.l s.main.c s.parser.y s.write.c
SRCS	= expand.c lexical.l main.c parser.y write.c
D 2
CFILES	= expand.c lexical.c main.c parser.c write.c
E 2
I 2
CSRCS	= expand.c lexical.c main.c parser.c write.c
E 2
OBJS	= expand.o main.o parser.o write.o
D 2
LIBS	= -ll -lm
E 2
I 2
LIBS	= -ly -ll -lm
E 2
SLIB	= ../bsd.o

D 2
$(DEST) : $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) $(SLIB) -o $(DEST)
E 2
I 2
keyread : $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) $(SLIB) -o $@
E 2

tags    : $(SRCS)
	ctags $(SRCS)

I 2
lint    : $(CSRCS)
	lint $(CFLAGS) $(CSRCS)

E 2
clean   : 
	rm -f $(OBJS) 

version : 
	prs -d":Dt: :F:\n:C:\n\n" $(SCCS)

# change the default actions
I 2
.c~.o   :
	$(GET) $(GFLAGS) $<
	$(CC) $(CFLAGS) $*.c -o $*.o

.c.o   :
	$(CC) $(CFLAGS) $*.c -o $*.o

E 2
.c~.c	:
	$(GET) $(GFLAGS) $<

I 2
.y.o    :
	$(YACC) $(YFLAGS) -d $<
	mv y.tab.c $*.c
	mv y.tab.h $*.h
	$(CC) $(CFLAGS) $*.c -o $*.o

.y.c    :
	$(YACC) $(YFLAGS) -d $<
	mv y.tab.c $*.c
	mv y.tab.h $*.h

E 2
.y~.y	:
	$(GET) $(GFLAGS) $<

.l~.l	:
	$(GET) $(GFLAGS) $<

.h~.h	:
	$(GET) $(GFLAGS) $<

D 2
depend  : 
	sed '/^#LAST-HUMAN-LINE/q' Makefile > Makefile.new
	mkdep $(INCL) $(SRCS) 
	cat .depend >> Makefile.new
	mv Makefile Makefile.old; mv Makefile.new Makefile
	rm .depend
E 2
I 2
depend  : $(YSRCS) always
	cat Makefile.proto > Makefile
	$(CC) -M $(CFLAGS) $(SRCS) >>Makefile
E 2

D 2
#LAST-HUMAN-LINE
expand.o: expand.c
expand.o: ../include/check.h
expand.o: ../include/key.h
main.o: main.c
main.o: ../include/sysdep.h
main.o: ../include/check.h
main.o: ../include/key.h
write.o: write.c
write.o: ../include/check.h
write.o: ../include/key.h
E 2
I 2
always:

#----------------------------------------
# DO NOT DELETE THIS LINE -- make depend may depend on it.
E 2
E 1
