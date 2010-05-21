h40860
s 00000/00000/00087
d D 2.1 93/02/03 15:25:06 suprem45 2 1
c Initial IV.GS release
e
s 00087/00000/00000
d D 1.1 93/02/03 15:24:13 suprem45 1 0
c date and time created 93/02/03 15:24:13 by suprem45
e
u
U
f e 0
t
T
I 1
# Makefile 4.1 8/18/89 16:39:59
#
# Makefile for SupremIV miscellaneous utilities.
#
# change this for port to other machines
WRKDIR  = ..
#
# set up destination directories and sources
DESTLIB = $(WRKDIR)/lib/misc.a
INCL 	= -I$(WRKDIR)/include

CFLAGS	= $(OPT) $(INCL) -D$(SYS) $(LOCAL)
HFILES  = 
SCCS    = s.cpu.c s.echo.c s.get.c s.man.c s.read.c s.reader.c \
	  s.print_time.c s.s.panic.c
SRCS    = cpu.c echo.c get.c man.c read.c reader.c print_time.c \
	  panic.c sint.c
CFILES  = cpu.c echo.c get.c man.c read.c reader.c print_time.c \
	  panic.c sint.c
OBJS	= cpu.o echo.o get.o man.o read.o reader.o print_time.o\
	  panic.o sint.o

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
cpu.o: cpu.c
cpu.o: ../include/global.h
cpu.o: ../include/shell.h
echo.o: echo.c
echo.o: ../include/global.h
echo.o: ../include/expr.h
get.o: get.c
get.o: ../include/global.h
get.o: ../include/check.h
man.o: man.c
man.o: ../include/sysdep.h
read.o: read.c
read.o: ../include/global.h
read.o: ../include/sysdep.h
read.o: ../include/check.h
reader.o: reader.c
reader.o: ../include/global.h
reader.o: ../include/check.h
reader.o: ../include/key.h
print_time.o: print_time.c
print_time.o: ../include/global.h
print_time.o: ../include/shell.h
panic.o: panic.c
panic.o: ../include/global.h
panic.o: ../include/constant.h
panic.o: ../include/geom.h
panic.o: ../include/dbaccess.h
panic.o: ../include/skel.h
panic.o: ../include/material.h
panic.o: ../include/impurity.h
panic.o: ../include/diffuse.h
panic.o: ../include/shell.h
panic.o: ../include/sysdep.h
E 1
