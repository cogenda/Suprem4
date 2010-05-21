h17315
s 00000/00000/00040
d D 2.1 93/02/03 15:13:12 suprem45 2 1
c Initial IV.GS release
e
s 00040/00000/00000
d D 1.1 93/02/03 15:12:01 suprem45 1 0
c date and time created 93/02/03 15:12:01 by suprem45
e
u
U
f e 0
t
T
I 1
# Makefile 4.1 8/18/89 16:34:18
#
# Makefile for the SupremIV include files. (handles sccs updates)
#
# change this for port to other machines
WRKDIR  = ..
#
# set up destination directories and sources
DESTLIB = 
INCL 	= -I$(WRKDIR)/include

CFLAGS	= -O $(INCL) -D$(SYS) $(LOCAL)
HFILES	= check.h constant.h dbaccess.h diffuse.h geom.h global.h impurity.h \
	  key.h material.h matrix.h plot.h regrid.h shell.h sysdep.h expr.h
SCCS	= s.check.h s.constant.h s.dbaccess.h s.diffuse.h s.geom.h s.global.h \
	  s.impurity.h s.key.h s.material.h s.matrix.h s.plot.h s.regrid.h \
	  s.shell.h s.sysdep.h s.expr.h
SRCS	= 
CFILES	= 
OBJS	= 

HFILES : $(HFILES)

tags    : $(SRCS)
	ctags $(SRCS)

clean   : 

version : 
	prs -d":Dt: :F:\n:C:\n\n" $(SCCS)

# change the default get line
.h~.h	:
	$(GET) $(GFLAGS) $<

depend  : 
	echo include/Makefile does not need to make depend


#LAST HUMAN LINE
E 1
