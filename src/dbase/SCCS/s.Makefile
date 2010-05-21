h44431
s 00000/00000/00237
d D 2.1 93/02/03 14:33:19 suprem45 2 1
c Initial IV.GS release
e
s 00237/00000/00000
d D 1.1 93/02/03 14:30:25 suprem45 1 0
c date and time created 93/02/03 14:30:25 by suprem45
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
DESTLIB = $(WRKDIR)/lib/dbase.a
INCL = -I$(WRKDIR)/include

CFLAGS	= $(OPT) -c $(INCL) -D$(SYS) $(LOCAL)
LFLAGS	= $(INCL) -D$(SYS) $(LOCAL)
HFILES  = 
SRCS	= alloc.c check.c clktri.c couple.c dimen.c dispose.c edge.c \
	element.c flip.c geometry.c list.c locate.c make_db.c neigh.c \
	node.c point.c region.c \
	grid_upd.c grid_loop.c kill_node.c new_layer.c remove.c geom.c
OBJS	= alloc.o check.o clktri.o couple.o dimen.o dispose.o edge.o \
	element.o flip.o geometry.o list.o locate.o make_db.o neigh.o \
	node.o point.o region.o \
	grid_upd.o grid_loop.o kill_node.o new_layer.o remove.o geom.o

$(DESTLIB) : $(OBJS)
	$(AR) rv $(DESTLIB) $(OBJS)
	$(RANLIB) $(DESTLIB)

tags    : $(SRCS)
	ctags $(SRCS)

lint    : 
	lint $(LFLAGS) -DSTATIC_ALLOCATION_TIME $(SRCS) > lint.list

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
alloc.o: alloc.c
alloc.o: ../include/global.h
alloc.o: ../include/constant.h
alloc.o: ../include/dbaccess.h
alloc.o: ../include/geom.h
alloc.o: ../include/skel.h
alloc.o: ../include/impurity.h
alloc.o: ../include/diffuse.h
check.o: check.c
check.o: ../include/global.h
check.o: ../include/constant.h
check.o: ../include/dbaccess.h
check.o: ../include/geom.h
check.o: ../include/skel.h
check.o: ../include/material.h
clktri.o: clktri.c
clktri.o: ../include/global.h
clktri.o: ../include/constant.h
clktri.o: ../include/geom.h
clktri.o: ../include/dbaccess.h
clktri.o: ../include/skel.h
couple.o: couple.c
couple.o: ../include/constant.h
couple.o: ../include/global.h
couple.o: ../include/geom.h
couple.o: ../include/dbaccess.h
couple.o: ../include/skel.h
couple.o: ../include/material.h
couple.o: ../include/matrix.h
couple.o: ../include/impurity.h
dimen.o: dimen.c
dimen.o: ../include/global.h
dimen.o: ../include/constant.h
dimen.o: ../include/impurity.h
dimen.o: ../include/geom.h
dimen.o: ../include/dbaccess.h
dimen.o: ../include/skel.h
dispose.o: dispose.c
dispose.o: ../include/global.h
dispose.o: ../include/constant.h
dispose.o: ../include/geom.h
dispose.o: ../include/dbaccess.h
dispose.o: ../include/skel.h
dispose.o: ../include/material.h
dispose.o: ../include/impurity.h
dispose.o: ../include/diffuse.h
dispose.o: ../include/regrid.h
edge.o: edge.c
edge.o: ../include/global.h
edge.o: ../include/constant.h
edge.o: ../include/dbaccess.h
edge.o: ../include/geom.h
edge.o: ../include/skel.h
element.o: element.c
element.o: ../include/global.h
element.o: ../include/constant.h
element.o: ../include/dbaccess.h
element.o: ../include/geom.h
element.o: ../include/skel.h
element.o: ../include/material.h
flip.o: flip.c
flip.o: ../include/global.h
flip.o: ../include/constant.h
flip.o: ../include/geom.h
flip.o: ../include/dbaccess.h
flip.o: ../include/skel.h
geometry.o: geometry.c
geometry.o: ../include/global.h
geometry.o: ../include/constant.h
geometry.o: ../include/geom.h
geometry.o: ../include/dbaccess.h
geometry.o: ../include/skel.h
geometry.o: ../include/material.h
geometry.o: ../include/matrix.h
list.o: list.c
list.o: ../include/global.h
list.o: ../include/constant.h
list.o: ../include/dbaccess.h
list.o: ../include/geom.h
list.o: ../include/skel.h
locate.o: locate.c
locate.o: ../include/global.h
locate.o: ../include/constant.h
locate.o: ../include/geom.h
locate.o: ../include/dbaccess.h
locate.o: ../include/skel.h
make_db.o: make_db.c
make_db.o: ../include/global.h
make_db.o: ../include/constant.h
make_db.o: ../include/geom.h
make_db.o: ../include/dbaccess.h
make_db.o: ../include/skel.h
make_db.o: ../include/material.h
neigh.o: neigh.c
neigh.o: ../include/global.h
neigh.o: ../include/constant.h
neigh.o: ../include/dbaccess.h
neigh.o: ../include/geom.h
neigh.o: ../include/skel.h
node.o: node.c
node.o: ../include/global.h
node.o: ../include/constant.h
node.o: ../include/geom.h
node.o: ../include/dbaccess.h
node.o: ../include/skel.h
node.o: ../include/material.h
node.o: ../include/impurity.h
node.o: ../include/diffuse.h
point.o: point.c
point.o: ../include/global.h
point.o: ../include/constant.h
point.o: ../include/geom.h
point.o: ../include/dbaccess.h
point.o: ../include/skel.h
point.o: ../include/material.h
point.o: ../include/impurity.h
point.o: ../include/diffuse.h
region.o: region.c
region.o: ../include/global.h
region.o: ../include/constant.h
region.o: ../include/geom.h
region.o: ../include/dbaccess.h
region.o: ../include/skel.h
region.o: ../include/material.h
grid_upd.o: grid_upd.c
grid_upd.o: ../include/global.h
grid_upd.o: ../include/constant.h
grid_upd.o: ../include/geom.h
grid_upd.o: ../include/dbaccess.h
grid_upd.o: ../include/skel.h
grid_upd.o: ../include/material.h
grid_upd.o: ../include/impurity.h
grid_upd.o: ../include/regrid.h
grid_upd.o: ../include/matrix.h
grid_loop.o: grid_loop.c
grid_loop.o: ../include/global.h
grid_loop.o: ../include/constant.h
grid_loop.o: ../include/geom.h
grid_loop.o: ../include/dbaccess.h
grid_loop.o: ../include/skel.h
grid_loop.o: ../include/material.h
grid_loop.o: ../include/impurity.h
grid_loop.o: ../include/regrid.h
grid_loop.o: ../include/matrix.h
kill_node.o: kill_node.c
kill_node.o: ../include/constant.h
kill_node.o: ../include/global.h
kill_node.o: ../include/dbaccess.h
kill_node.o: ../include/geom.h
kill_node.o: ../include/skel.h
kill_node.o: ../include/material.h
kill_node.o: ../include/skel.h
new_layer.o: new_layer.c
new_layer.o: ../include/global.h
new_layer.o: ../include/constant.h
new_layer.o: ../include/geom.h
new_layer.o: ../include/dbaccess.h
new_layer.o: ../include/skel.h
new_layer.o: ../include/material.h
new_layer.o: ../include/impurity.h
remove.o: remove.c
remove.o: ../include/global.h
remove.o: ../include/constant.h
remove.o: ../include/dbaccess.h
remove.o: ../include/geom.h
remove.o: ../include/skel.h
remove.o: ../include/material.h
remove.o: ../include/impurity.h
geom.o: geom.c
geom.o: ../include/global.h
geom.o: ../include/constant.h
geom.o: ../include/skel.h
geom.o: ../include/dbaccess.h
geom.o: ../include/geom.h
E 1
