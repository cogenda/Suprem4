h05736
s 00000/00000/00139
d D 2.1 93/02/03 15:23:08 suprem45 2 1
c Initial IV.GS release
e
s 00139/00000/00000
d D 1.1 93/02/03 15:22:10 suprem45 1 0
c date and time created 93/02/03 15:22:10 by suprem45
e
u
U
f e 0
t
T
I 1
# Makefile 3.1 9/14/87 10:49:45
#
# Makefile for the SupremIV mesh code.
# Primarily, this code reads and writes mesh data to files in several
# different formats.
#
# change this for port to other machines
WRKDIR  = ..
#
# set up destination directories and sources
DESTLIB = $(WRKDIR)/lib/mesh.a
INCL = -I$(WRKDIR)/include

CFLAGS	= $(OPT) -c $(INCL) -D$(SYS) $(LOCAL)
HFILES  = 
SRCS	= check_active.c ig2_meshio.c initialize.c newbackside.c \
	pi_meshio.c profile.c rect.c save_simpl.c structure.c
OBJS	= check_active.o ig2_meshio.o initialize.o newbackside.o \
	pi_meshio.o profile.o rect.o save_simpl.o structure.o


$(DESTLIB) : $(OBJS)
	$(AR) rv $(DESTLIB) $(OBJS)
	$(RANLIB) $(DESTLIB)

tags    : $(SRCS)
	ctags $(SRCS)

lint    : $(SRCS)
	lint -I$(INCL) $(CFILES)

clean   : 
	rm -f $(OBJS) 

version : 
	prs -d":Dt: :F:\n:C:\n\n" $(SCCS)

# change the default get line
.c~.o	:
	$(GET) $(GFLAGS) $<
	$(CC) $(CFLAGS) $*.c

.c.o   :
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
check_active.o: check_active.c
check_active.o: ../include/global.h
check_active.o: ../include/constant.h
check_active.o: ../include/sysdep.h
check_active.o: ../include/impurity.h
ig2_meshio.o: ig2_meshio.c
ig2_meshio.o: ../include/global.h
ig2_meshio.o: ../include/constant.h
ig2_meshio.o: ../include/dbaccess.h
ig2_meshio.o: ../include/geom.h
ig2_meshio.o: ../include/skel.h
ig2_meshio.o: ../include/material.h
ig2_meshio.o: ../include/impurity.h
ig2_meshio.o: ../include/sysdep.h
ig2_meshio.o: ../include/expr.h
initialize.o: initialize.c
initialize.o: ../include/global.h
initialize.o: ../include/constant.h
initialize.o: ../include/dbaccess.h
initialize.o: ../include/geom.h
initialize.o: ../include/skel.h
initialize.o: ../include/impurity.h
initialize.o: ../include/material.h
newbackside.o: newbackside.c
newbackside.o: ../include/constant.h
newbackside.o: ../include/geom.h
newbackside.o: ../include/dbaccess.h
newbackside.o: ../include/skel.h
newbackside.o: ../include/shell.h
newbackside.o: ../include/implant.h
pi_meshio.o: pi_meshio.c
pi_meshio.o: ../include/global.h
pi_meshio.o: ../include/constant.h
pi_meshio.o: ../include/dbaccess.h
pi_meshio.o: ../include/geom.h
pi_meshio.o: ../include/skel.h
pi_meshio.o: ../include/material.h
pi_meshio.o: ../include/impurity.h
pi_meshio.o: ../include/sysdep.h
pi_meshio.o: ../include/diffuse.h
pi_meshio.o: ../include/regrid.h
pi_meshio.o: ../include/expr.h
pi_meshio.o: ../include/plot.h
profile.o: profile.c
profile.o: ../include/global.h
profile.o: ../include/constant.h
profile.o: ../include/dbaccess.h
profile.o: ../include/geom.h
profile.o: ../include/skel.h
profile.o: ../include/impurity.h
profile.o: ../include/material.h
profile.o: ../include/defect.h
profile.o: ../include/expr.h
rect.o: rect.c
rect.o: ../include/sysdep.h
rect.o: ../include/constant.h
rect.o: ../include/dbaccess.h
rect.o: ../include/geom.h
rect.o: ../include/skel.h
rect.o: ../include/material.h
rect.o: ../include/global.h
save_simpl.o: save_simpl.c
save_simpl.o: ../include/constant.h
save_simpl.o: simpl-dix.h
save_simpl.o: simpl.h
save_simpl.o: ../include/global.h
save_simpl.o: ../include/geom.h
save_simpl.o: ../include/dbaccess.h
save_simpl.o: ../include/skel.h
save_simpl.o: ../include/material.h
save_simpl.o: ../include/implant.h
save_simpl.o: ../include/plot.h
save_simpl.o: ../include/bound.h
structure.o: structure.c
structure.o: ../include/global.h
structure.o: ../include/constant.h
structure.o: ../include/impurity.h
structure.o: ../include/dbaccess.h
structure.o: ../include/geom.h
structure.o: ../include/skel.h
E 1
