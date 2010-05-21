h63464
s 00000/00000/00079
d D 2.1 93/02/03 15:39:43 suprem45 2 1
c Iniital IV.GS release
e
s 00079/00000/00000
d D 1.1 93/02/03 15:38:28 suprem45 1 0
c date and time created 93/02/03 15:38:28 by suprem45
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
DESTLIB = $(WRKDIR)/lib/xsup4.a
INCL 	= -I$(WRKDIR)/include

CFLAGS	= $(OPT) $(INCL) -D$(SYS) $(LOCAL)
HFILES  = 
SRCS    = Graph.c app1.c draw.c help.c hpgl.c idraw.c interface.c ps.c xgX.c
OBJS    = Graph.o app1.o draw.o help.o hpgl.o idraw.o interface.o ps.o xgX.o

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
Graph.o: Graph.c
Graph.o: GraphP.h
Graph.o: Graph.h
Graph.o: DataRec.h
Graph.o: HardInfo.h
app1.o: app1.c
app1.o: Graph.h
app1.o: DataRec.h
app1.o: xgraph.h
app1.o: HardInfo.h
draw.o: draw.c
draw.o: GraphP.h
draw.o: Graph.h
draw.o: DataRec.h
help.o: help.c
hpgl.o: hpgl.c
hpgl.o: DataRec.h
hpgl.o: plotter.h
idraw.o: idraw.c
idraw.o: DataRec.h
interface.o: interface.c
interface.o: GraphP.h
interface.o: Graph.h
interface.o: DataRec.h
interface.o: xgraph.h
ps.o: ps.c
ps.o: DataRec.h
xgX.o: xgX.c
xgX.o: GraphP.h
xgX.o: Graph.h
xgX.o: DataRec.h
E 1
