h19059
s 00094/00000/00295
d D 2.1 93/02/03 14:50:51 suprem45 2 1
c Initial IV.GS release
e
s 00295/00000/00000
d D 1.1 93/02/03 14:41:15 suprem45 1 0
c date and time created 93/02/03 14:41:15 by suprem45
e
u
U
f e 0
t
T
I 1
#
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
CFLAGS  = $(OPT) $(INCL) -D$(SYS) -DDEBUG $(LOCAL)
LFLAGS  = $(INCL) -D$(SYS) -DDEBUG $(LOCAL)
CFILES  = time.c solve.c solve_time.c setup.c block_set.c norm.c diffuse.c \
	  species.c movie.c active.c prepare.c \
	  Boron.c Phosphorus.c Arsenic.c Antimony.c Interst.c Vacancy.c \
I 2
	  Generic.c Silicon.c Selenium.c Tin.c Germanium.c \
	  Beryllium.c Magnesium.c Zinc.c Carbon.c \
E 2
	  diff_init.c predep.c defect.c moving.c Trap.c Gold.c Cesium.c
OBJS    = time.o solve.o solve_time.o setup.o block_set.o norm.o diffuse.o \
	  species.o movie.o active.o prepare.o \
	  Boron.o Phosphorus.o Arsenic.o Antimony.o Interst.o Vacancy.o \
I 2
	  Generic.o Silicon.o Selenium.o Tin.o Germanium.o \
	  Beryllium.o Magnesium.o Zinc.o Carbon.o \
E 2
	  diff_init.o predep.o defect.o moving.o Trap.o Gold.o Cesium.o
SRCS    = $(CFILES) 
DEST	= $(WRKDIR)/lib/diffuse.a

$(DEST) : $(OBJS) 
	-rm $(DEST)
	$(AR) rv $(DEST) $(OBJS)
	$(RANLIB) $(DEST)

lint    : 
	lint $(LFLAGS) -DSTATIC_ALLOCATION_TIME $(SRCS) > lint.list

tags    : $(SRCS) 
	ctags $(SRCS) $(SUBSRC)

clean   : 
	rm -f $(OBJS) 

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
time.o: time.c
time.o: ../include/global.h
time.o: ../include/constant.h
time.o: ../include/geom.h
time.o: ../include/dbaccess.h
time.o: ../include/skel.h
time.o: ../include/diffuse.h
time.o: ../include/impurity.h
time.o: ../include/matrix.h
solve.o: solve.c
solve.o: ../include/global.h
solve.o: ../include/constant.h
solve.o: ../include/geom.h
solve.o: ../include/dbaccess.h
solve.o: ../include/skel.h
solve.o: ../include/diffuse.h
solve.o: ../include/impurity.h
solve.o: ../include/material.h
solve.o: ../include/matrix.h
solve.o: ../include/defect.h
solve_time.o: solve_time.c
solve_time.o: ../include/global.h
solve_time.o: ../include/constant.h
solve_time.o: ../include/geom.h
solve_time.o: ../include/dbaccess.h
solve_time.o: ../include/skel.h
solve_time.o: ../include/diffuse.h
solve_time.o: ../include/impurity.h
solve_time.o: ../include/material.h
solve_time.o: ../include/matrix.h
solve_time.o: ../include/sysdep.h
solve_time.o: ../include/device.h
solve_time.o: ../include/poly.h
solve_time.o: ../include/defect.h
solve_time.o: ../include/expr.h
setup.o: setup.c
setup.o: ../include/global.h
setup.o: ../include/constant.h
setup.o: ../include/geom.h
setup.o: ../include/dbaccess.h
setup.o: ../include/skel.h
setup.o: ../include/diffuse.h
setup.o: ../include/material.h
setup.o: ../include/impurity.h
setup.o: ../include/matrix.h
block_set.o: block_set.c
block_set.o: ../include/global.h
block_set.o: ../include/constant.h
block_set.o: ../include/geom.h
block_set.o: ../include/dbaccess.h
block_set.o: ../include/skel.h
block_set.o: ../include/impurity.h
block_set.o: ../include/defect.h
block_set.o: ../include/material.h
block_set.o: ../include/diffuse.h
block_set.o: ../include/matrix.h
norm.o: norm.c
diffuse.o: diffuse.c
diffuse.o: ../include/global.h
diffuse.o: ../include/constant.h
diffuse.o: ../include/geom.h
diffuse.o: ../include/dbaccess.h
diffuse.o: ../include/skel.h
diffuse.o: ../include/diffuse.h
diffuse.o: ../include/defect.h
diffuse.o: ../include/matrix.h
diffuse.o: ../include/impurity.h
diffuse.o: ../include/material.h
diffuse.o: ../include/expr.h
species.o: species.c
species.o: ../include/global.h
species.o: ../include/constant.h
species.o: ../include/geom.h
species.o: ../include/dbaccess.h
species.o: ../include/skel.h
species.o: ../include/diffuse.h
species.o: ../include/impurity.h
species.o: ../include/matrix.h
movie.o: movie.c
movie.o: ../include/global.h
movie.o: ../include/shell.h
active.o: active.c
active.o: ../include/global.h
active.o: ../include/constant.h
active.o: ../include/geom.h
active.o: ../include/dbaccess.h
active.o: ../include/skel.h
active.o: ../include/impurity.h
active.o: ../include/defect.h
active.o: ../include/diffuse.h
active.o: ../include/material.h
active.o: ../include/plot.h
active.o: ../include/device.h
prepare.o: prepare.c
prepare.o: ../include/global.h
prepare.o: ../include/constant.h
prepare.o: ../include/geom.h
prepare.o: ../include/dbaccess.h
prepare.o: ../include/skel.h
prepare.o: ../include/diffuse.h
prepare.o: ../include/impurity.h
prepare.o: ../include/defect.h
prepare.o: ../include/material.h
prepare.o: ../include/expr.h
prepare.o: ../include/matrix.h
I 2
Generic.o: Generic.c
Generic.o: ../include/global.h
Generic.o: ../include/constant.h
Generic.o: ../include/geom.h
Generic.o: ../include/dbaccess.h
Generic.o: ../include/skel.h
Generic.o: ../include/impurity.h
Generic.o: ../include/matrix.h
Generic.o: ../include/material.h
Generic.o: ../include/diffuse.h
Silicon.o: Silicon.c
Silicon.o: ../include/global.h
Silicon.o: ../include/constant.h
Silicon.o: ../include/geom.h
Silicon.o: ../include/dbaccess.h
Silicon.o: ../include/skel.h
Silicon.o: ../include/impurity.h
Silicon.o: ../include/matrix.h
Silicon.o: ../include/material.h
Silicon.o: ../include/diffuse.h
Selenium.o: Selenium.c
Selenium.o: ../include/global.h
Selenium.o: ../include/constant.h
Selenium.o: ../include/geom.h
Selenium.o: ../include/dbaccess.h
Selenium.o: ../include/skel.h
Selenium.o: ../include/impurity.h
Selenium.o: ../include/matrix.h
Selenium.o: ../include/material.h
Selenium.o: ../include/diffuse.h
Tin.o: Tin.c
Tin.o: ../include/global.h
Tin.o: ../include/constant.h
Tin.o: ../include/geom.h
Tin.o: ../include/dbaccess.h
Tin.o: ../include/skel.h
Tin.o: ../include/impurity.h
Tin.o: ../include/matrix.h
Tin.o: ../include/material.h
Tin.o: ../include/diffuse.h
Germanium.o: Germanium.c
Germanium.o: ../include/global.h
Germanium.o: ../include/constant.h
Germanium.o: ../include/geom.h
Germanium.o: ../include/dbaccess.h
Germanium.o: ../include/skel.h
Germanium.o: ../include/impurity.h
Germanium.o: ../include/matrix.h
Germanium.o: ../include/material.h
Germanium.o: ../include/diffuse.h
Beryllium.o: Beryllium.c
Beryllium.o: ../include/global.h
Beryllium.o: ../include/constant.h
Beryllium.o: ../include/geom.h
Beryllium.o: ../include/dbaccess.h
Beryllium.o: ../include/skel.h
Beryllium.o: ../include/impurity.h
Beryllium.o: ../include/matrix.h
Beryllium.o: ../include/material.h
Beryllium.o: ../include/diffuse.h
Magnesium.o: Magnesium.c
Magnesium.o: ../include/global.h
Magnesium.o: ../include/constant.h
Magnesium.o: ../include/geom.h
Magnesium.o: ../include/dbaccess.h
Magnesium.o: ../include/skel.h
Magnesium.o: ../include/impurity.h
Magnesium.o: ../include/matrix.h
Magnesium.o: ../include/material.h
Magnesium.o: ../include/diffuse.h
Zinc.o: Zinc.c
Zinc.o: ../include/global.h
Zinc.o: ../include/constant.h
Zinc.o: ../include/geom.h
Zinc.o: ../include/dbaccess.h
Zinc.o: ../include/skel.h
Zinc.o: ../include/impurity.h
Zinc.o: ../include/matrix.h
Zinc.o: ../include/material.h
Zinc.o: ../include/diffuse.h
Carbon.o: Carbon.c
Carbon.o: ../include/global.h
Carbon.o: ../include/constant.h
Carbon.o: ../include/geom.h
Carbon.o: ../include/dbaccess.h
Carbon.o: ../include/skel.h
Carbon.o: ../include/impurity.h
Carbon.o: ../include/matrix.h
Carbon.o: ../include/material.h
Carbon.o: ../include/diffuse.h
E 2
Boron.o: Boron.c
Boron.o: ../include/global.h
Boron.o: ../include/constant.h
Boron.o: ../include/geom.h
Boron.o: ../include/dbaccess.h
Boron.o: ../include/skel.h
Boron.o: ../include/impurity.h
Boron.o: ../include/matrix.h
Boron.o: ../include/material.h
Boron.o: ../include/diffuse.h
Phosphorus.o: Phosphorus.c
Phosphorus.o: ../include/global.h
Phosphorus.o: ../include/constant.h
Phosphorus.o: ../include/geom.h
Phosphorus.o: ../include/dbaccess.h
Phosphorus.o: ../include/skel.h
Phosphorus.o: ../include/impurity.h
Phosphorus.o: ../include/matrix.h
Phosphorus.o: ../include/material.h
Phosphorus.o: ../include/diffuse.h
Arsenic.o: Arsenic.c
Arsenic.o: ../include/global.h
Arsenic.o: ../include/constant.h
Arsenic.o: ../include/geom.h
Arsenic.o: ../include/dbaccess.h
Arsenic.o: ../include/skel.h
Arsenic.o: ../include/impurity.h
Arsenic.o: ../include/matrix.h
Arsenic.o: ../include/material.h
Arsenic.o: ../include/diffuse.h
Antimony.o: Antimony.c
Antimony.o: ../include/global.h
Antimony.o: ../include/constant.h
Antimony.o: ../include/geom.h
Antimony.o: ../include/dbaccess.h
Antimony.o: ../include/skel.h
Antimony.o: ../include/impurity.h
Antimony.o: ../include/matrix.h
Antimony.o: ../include/material.h
Antimony.o: ../include/diffuse.h
Interst.o: Interst.c
Interst.o: ../include/global.h
Interst.o: ../include/constant.h
Interst.o: ../include/geom.h
Interst.o: ../include/dbaccess.h
Interst.o: ../include/skel.h
Interst.o: ../include/impurity.h
Interst.o: ../include/defect.h
Interst.o: ../include/material.h
Interst.o: ../include/diffuse.h
Interst.o: trap.h
Interst.o: ../include/matrix.h
Interst.o: ../include/plot.h
Vacancy.o: Vacancy.c
Vacancy.o: ../include/global.h
Vacancy.o: ../include/constant.h
Vacancy.o: ../include/geom.h
Vacancy.o: ../include/dbaccess.h
Vacancy.o: ../include/skel.h
Vacancy.o: ../include/impurity.h
Vacancy.o: ../include/defect.h
Vacancy.o: ../include/material.h
Vacancy.o: ../include/diffuse.h
Vacancy.o: ../include/matrix.h
diff_init.o: diff_init.c
diff_init.o: ../include/global.h
diff_init.o: ../include/constant.h
diff_init.o: ../include/impurity.h
diff_init.o: ../include/defect.h
diff_init.o: ../include/material.h
diff_init.o: ../include/diffuse.h
diff_init.o: ../include/check.h
predep.o: predep.c
predep.o: ../include/global.h
predep.o: ../include/constant.h
predep.o: ../include/geom.h
predep.o: ../include/dbaccess.h
predep.o: ../include/skel.h
predep.o: ../include/impurity.h
predep.o: ../include/material.h
predep.o: ../include/diffuse.h
defect.o: defect.c
defect.o: ../include/global.h
defect.o: ../include/constant.h
defect.o: ../include/geom.h
defect.o: ../include/dbaccess.h
defect.o: ../include/skel.h
defect.o: ../include/impurity.h
defect.o: ../include/defect.h
defect.o: ../include/material.h
defect.o: ../include/matrix.h
defect.o: ../include/diffuse.h
defect.o: trap.h
moving.o: moving.c
moving.o: ../include/global.h
moving.o: ../include/constant.h
moving.o: ../include/geom.h
moving.o: ../include/dbaccess.h
moving.o: ../include/skel.h
moving.o: ../include/diffuse.h
moving.o: ../include/material.h
moving.o: ../include/impurity.h
moving.o: ../include/matrix.h
Trap.o: Trap.c
Trap.o: ../include/global.h
Trap.o: ../include/constant.h
Trap.o: ../include/geom.h
Trap.o: ../include/dbaccess.h
Trap.o: ../include/skel.h
Trap.o: ../include/impurity.h
Trap.o: ../include/matrix.h
Trap.o: ../include/material.h
Trap.o: ../include/diffuse.h
Trap.o: ../include/defect.h
Gold.o: Gold.c
Gold.o: ../include/global.h
Gold.o: ../include/constant.h
Gold.o: ../include/geom.h
Gold.o: ../include/dbaccess.h
Gold.o: ../include/skel.h
Gold.o: ../include/impurity.h
Gold.o: ../include/matrix.h
Gold.o: ../include/material.h
Cesium.o: Cesium.c
Cesium.o: ../include/global.h
Cesium.o: ../include/constant.h
Cesium.o: ../include/geom.h
Cesium.o: ../include/dbaccess.h
Cesium.o: ../include/skel.h
Cesium.o: ../include/impurity.h
Cesium.o: ../include/matrix.h
Cesium.o: ../include/material.h
E 1
