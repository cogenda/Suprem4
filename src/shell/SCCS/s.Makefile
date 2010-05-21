h18358
s 00000/00000/00107
d D 2.1 93/02/03 15:37:17 suprem45 2 1
c Initial IV.GS release
e
s 00107/00000/00000
d D 1.1 93/02/03 15:35:44 suprem45 1 0
c date and time created 93/02/03 15:35:44 by suprem45
e
u
U
f e 0
t
T
I 1
# Makefile 3.1 9/15/87 09:37:24
#
# Makefile for the SupremIV formatted key file reader.
#
# change this for port to other machines
WRKDIR  = ..
#
# set up destination directories and sources
DESTLIB	= $(WRKDIR)/lib/shell.a
INCL 	= -I$(WRKDIR)/include

CFLAGS	= $(OPT) $(INCL) -D$(SYS)  $(LOCAL)
LDFLAGS = 
HFILES  = 
SCCS	= s.do_action.c s.input.c s.lexical.l s.macro.c s.parser.y \
	  s.proc.c s.set.c s.parser_boot.c s.smisc.c
SRCS	= parser.y do_action.c input.c lex.c macro.c proc.c set.c list.c \
          parser_boot.c help.c smisc.c
CFILES	= parser.c do_action.c input.c lex.c macro.c proc.c set.c list.c \
	  parser_boot.c help.c smisc.c
OBJS	= parser.o do_action.o input.o lex.o macro.o proc.o set.o list.o \
	  parser_boot.o help.o smisc.o

$(DESTLIB) : $(OBJS)
	$(AR) rv $(DESTLIB) $(OBJS)
	$(RANLIB) $(DESTLIB)

tags    : $(SRCS)
	ctags $(SRCS)

clean   : 
	rm -f $(OBJS) 

version : 
	prs -d":Dt: :F:\n:C:\n\n" $(SCCS)

# change the default actions
.c~.c	:
	$(GET) $(GFLAGS) $<

.y~.y	:
	$(GET) $(GFLAGS) $<

.y.o	:
	$(YACC) $(YFLAGS) -d $<
	$(CC) $(CFLAGS) -c y.tab.c
	mv y.tab.c $*.c
	mv y.tab.o $@
	mv y.tab.h $*.h


.l~.l	:
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
do_action.o: do_action.c
do_action.o: ../include/shell.h
do_action.o: ../include/check.h
do_action.o: ../include/global.h
input.o: input.c
input.o: ../include/shell.h
lex.o: lex.c
lex.o: ../include/global.h
lex.o: ../include/constant.h
lex.o: ../include/sysdep.h
lex.o: parser.h
lex.o: ../include/shell.h
macro.o: macro.c
macro.o: ../include/global.h
macro.o: ../include/shell.h
macro.o: ../include/expr.h
proc.o: proc.c
proc.o: ../include/shell.h
set.o: set.c
set.o: ../include/global.h
set.o: ../include/shell.h
list.o: list.c
list.o: ../include/global.h
parser_boot.o: parser_boot.c
parser_boot.o: ../include/check.h
parser_boot.o: ../include/constant.h
parser_boot.o: ../include/dbaccess.h
parser_boot.o: ../include/geom.h
parser_boot.o: ../include/skel.h
parser_boot.o: ../include/global.h
parser_boot.o: ../include/key.h
parser_boot.o: ../include/shell.h
parser_boot.o: ../include/sysdep.h
parser_boot.o: ../include/expr.h
help.o: help.c
help.o: ../include/global.h
help.o: ../include/check.h
help.o: ../include/shell.h
smisc.o: smisc.c
smisc.o: ../include/global.h
smisc.o: ../include/expr.h
smisc.o: ../include/shell.h
E 1
