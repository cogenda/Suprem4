      subroutine reduce(ndstk, nr, iold, renum, ndeg, lvl, lvls1,
     * lvls2, ccstor, ibw2, ipf2)
c  subroutine reduce determines a row and column permutation which,
c  when applied to a given sparse matrix, produces a permuted
c  matrix with a smaller bandwidth and profile.
c  the input array is a connection table which represents the
c  indices of the nonzero elements of the matrix, a.  the algo-
c  rithm is described in terms of the adjacency graph which
c  has the characteristic that there is an edge (connection)
c  between nodes i and j if a(i,j) .ne. 0 and i .ne. j.
c  dimensioning information--the following integer arrays must be
c  dimensioned in the calling routine.
c    ndstk(nr,d1)        d1 is .ge. maximum degree of all nodes.
c    iold(d2)            d2 and nr are .ge. the total number of
c    renum(d2+1)         nodes in the graph.
c    ndeg(d2)            storage requirements can be significantly
c    lvl(d2)             decreased for ibm 360 and 370 computers
c    lvls1(d2)           by replacing integer ndstk by
c    lvls2(d2)           integer*2 ndstk in subroutines reduce,
c    ccstor(d2)          dgree, fndiam, tree and number.
c  common information--the following common block must be in the
c  calling routine.
c    common/gra/n,idpth,ideg
c  explanation of input variables--
c    ndstk-     connection table representing graph.
c               ndstk(i,j)=node number of jth connection to node
c               number i.  a connection of a node to itself is not
c               listed.  extra positions must have zero fill.
c    nr-        row dimension assigned ndstk in calling program.
c    iold(i)-   numbering of ith node upon input.
c               if no numbering exists then iold(i)=i.
c    n-         number of nodes in graph (equal to order of matrix).
c    ideg-      maximum degree of any node in the graph.
c  explanation of output variables--
c    renum(i)-  the new number for the ith node.
c    ndeg(i)-   the degree of the ith node.
c    ibw2-      the bandwidth after renumbering.
c    ipf2-      the profile after renumbering.
c    idpth-     number of levels in reduce level structure.
c  the following only have meaning if the graph was connected--
c    lvl(i)-    index into lvls1 to the first node in level i.
c               lvl(i+1)-lvl(i)= number of nodes in ith level
c    lvls1-     node numbers listed by level.
c    lvls2(i)-  the level assigned to node i by reduce.
c  working storage variable--
c    ccstor
c  local storage--
c    common/cc/-subroutines reduce, sort2 and piklvl assume that
c               the graph has at most 50 connected components.
c               subroutine fndiam assumes that there are at most
c               100 nodes in the last level.
c    common/lvlw/-subroutines setup and piklvl assume that there
c               are at most 100 levels.
c use integer*2 ndstk  with an ibm 360 or 370.
      integer ndstk
      integer stnode, rvnode, renum, xc, sort2, stnum, ccstor,
     * size, stpt, sbnum
      common /gra/ n, idpth, ideg
c it is assumed that the graph has at most 50 connected components.
      common /cc/ xc, size(500), stpt(500)
      common /lvlw/ nhigh(1000), nlow(1000), nacum(1000)
      dimension ccstor(1), iold(1)
      dimension ndstk(nr,1), lvl(1), lvls1(1), lvls2(1), renum(1),
     * ndeg(1)
      ibw2 = 0
      ipf2 = 0
c set renum(i)=0 for all i to indicate node i is unnumbered
      do 10 i=1,n
        renum(i) = 0
   10 continue
c compute degree of each node and original bandwidth and profile
      call dgree(ndstk, nr, ndeg, iold, ibw1, ipf1)
c sbnum= low end of available numbers for renumbering
c stnum= high end of available numbers for renumbering
      sbnum = 1
      stnum = n
c number the nodes of degree zero
      do 20 i=1,n
        if (ndeg(i).gt.0) go to 20
        renum(i) = stnum
        stnum = stnum - 1
   20 continue
c find an unnumbered node of min degree to start on
   30 lowdg = ideg + 1
      nflg = 1
      isdir = 1
      do 40 i=1,n
        if (ndeg(i).ge.lowdg) go to 40
        if (renum(i).gt.0) go to 40
        lowdg = ndeg(i)
        stnode = i
   40 continue
c find pseudo-diameter and associated level structures.
c stnode and rvnode are the ends of the diam and lvls1 and lvls2
c are the respective level structures.
      call fndiam(stnode, rvnode, ndstk, nr, ndeg, lvl, lvls1,
     * lvls2, ccstor, idflt)
      if (ndeg(stnode).le.ndeg(rvnode)) go to 50
c nflg indicates the end to begin numbering on
      nflg = -1
      stnode = rvnode
   50 call setup(lvl, lvls1, lvls2)
c find all the connected components  (xc counts them)
      xc = 0
      lroot = 1
      lvln = 1
      do 60 i=1,n
        if (lvl(i).ne.0) go to 60
        xc = xc + 1
        stpt(xc) = lroot
        call tree(i, ndstk, nr, lvl, ccstor, ndeg, lvlwth, lvlbot,
     *   lvln, maxlw, n)
        size(xc) = lvlbot + lvlwth - lroot
        lroot = lvlbot + lvlwth
        lvln = lroot
   60 continue
      if (sort2(dmy).eq.0) go to 70
      call piklvl(lvls1, lvls2, ccstor, idflt, isdir)
c on return from piklvl, isdir indicates the direction the largest
c component fell.  isdir is modified now to indicate the numbering
c direction.  num is set to the proper value for this direction.
   70 isdir = isdir*nflg
      num = sbnum
      if (isdir.lt.0) num = stnum
      call number(stnode, num, ndstk, lvls2, ndeg, renum, lvls1,
     * lvl, nr, nflg, ibw2, ipf2, ccstor, isdir)
c update stnum or sbnum after numbering
      if (isdir.lt.0) stnum = num
      if (isdir.gt.0) sbnum = num
      if (sbnum.le.stnum) go to 30
      if (ibw2.le.ibw1) return
c if original numbering is better than new one, set up to return it
      do 80 i=1,n
        renum(i) = iold(i)
   80 continue
      ibw2 = ibw1
      ipf2 = ipf1
      return
      end
      subroutine dgree(ndstk, nr, ndeg, iold, ibw1, ipf1)
c  dgree computes the degree of each node in ndstk and stores
c  it in the array ndeg.  the bandwidth and profile for the original
c  or input renumbering of the graph is computed also.
c use integer*2 ndstk  with an ibm 360 or 370.
      integer ndstk
      common /gra/ n, idpth, ideg
      dimension ndstk(nr,1), ndeg(1), iold(1)
      ibw1 = 0
      ipf1 = 0
      do 40 i=1,n
        ndeg(i) = 0
        irw = 0
        do 20 j=1,ideg
          itst = ndstk(i,j)
          if (itst) 30, 30, 10
   10     ndeg(i) = ndeg(i) + 1
          idif = iold(i) - iold(itst)
          if (irw.lt.idif) irw = idif
   20   continue
   30   ipf1 = ipf1 + irw
        if (irw.gt.ibw1) ibw1 = irw
   40 continue
      return
      end
      subroutine fndiam(snd1, snd2, ndstk, nr, ndeg, lvl, lvls1,
     * lvls2, iwk, idflt)
c  fndiam is the control procedure for finding the pseudo-diameter of
c  ndstk as well as the level structure from each end
c  snd1-        on input this is the node number of the first
c               attempt at finding a diameter.  on output it
c               contains the actual number used.
c  snd2-        on output contains other end of diameter
c  lvls1-       array containing level structure with snd1 as root
c  lvls2-       array containing level structure with snd2 as root
c  idflt-       flag used in picking final level structure, set
c               =1 if width of lvls1 .le. width of lvls2, otherwise =2
c  lvl,iwk-     working storage
c use integer*2 ndstk  with an ibm 360 or 370.
      integer ndstk
      integer flag, snd, snd1, snd2
      common /gra/ n, idpth, ideg
c it is assumed that the last level has at most 100 nodes.
      common /cc/ ndlst(1000)
      dimension ndstk(nr,1), ndeg(1), lvl(1), lvls1(1), lvls2(1),
     * iwk(1)
      flag = 0
      mtw2 = n
      snd = snd1
c zero lvl to indicate all nodes are available to tree
   10 do 20 i=1,n
        lvl(i) = 0
   20 continue
      lvln = 1
c drop a tree from snd
      call tree(snd, ndstk, nr, lvl, iwk, ndeg, lvlwth, lvlbot,
     * lvln, maxlw, mtw2)
      if (flag.ge.1) go to 50
      flag = 1
   30 idpth = lvln - 1
      mtw1 = maxlw
c copy level structure into lvls1
      do 40 i=1,n
        lvls1(i) = lvl(i)
   40 continue
      ndxn = 1
      ndxl = 0
      mtw2 = n
c sort last level by degree  and store in ndlst
      call sortdg(ndlst, iwk(lvlbot), ndxl, lvlwth, ndeg)
      snd = ndlst(1)
      go to 10
   50 if (idpth.ge.lvln-1) go to 60
c start again with new starting node
      snd1 = snd
      go to 30
   60 if (maxlw.ge.mtw2) go to 80
      mtw2 = maxlw
      snd2 = snd
c store narrowest reverse level structure in lvls2
      do 70 i=1,n
        lvls2(i) = lvl(i)
   70 continue
   80 if (ndxn.eq.ndxl) go to 90
c try next node in ndlst
      ndxn = ndxn + 1
      snd = ndlst(ndxn)
      go to 10
   90 idflt = 1
      if (mtw2.le.mtw1) idflt = 2
      return
      end
      subroutine tree(iroot, ndstk, nr, lvl, iwk, ndeg, lvlwth,
     * lvlbot, lvln, maxlw, ibort)
c  tree drops a tree in ndstk from iroot
c  lvl-         array indicating available nodes in ndstk with zero
c               entries. tree enters level numbers assigned
c               during execution of this procedure
c  iwk-         on output contains node numbers used in tree
c               arranged by levels (iwk(lvln) contains iroot
c               and iwk(lvlbot+lvlwth-1) contains last node entered)
c  lvlwth-      on output contains width of last level
c  lvlbot-      on output contains index into iwk of first
c               node in last level
c  maxlw-       on output contains the maximum level width
c  lvln-        on input the first available location in iwk
c               usually one but if iwk is used to store previous
c               connected components, lvln is next available location.
c               on output the total number of levels + 1
c  ibort-       input param which triggers early return if
c               maxlw becomes .ge. ibort
c use integer*2 ndstk  with an ibm 360 or 370.
      integer ndstk
      dimension ndstk(nr,1), lvl(1), iwk(1), ndeg(1)
      maxlw = 0
      itop = lvln
      inow = lvln
      lvlbot = lvln
      lvltop = lvln + 1
      lvln = 1
      lvl(iroot) = 1
      iwk(itop) = iroot
   10 lvln = lvln + 1
   20 iwknow = iwk(inow)
      ndrow = ndeg(iwknow)
      do 30 j=1,ndrow
        itest = ndstk(iwknow,j)
        if (lvl(itest).ne.0) go to 30
        lvl(itest) = lvln
        itop = itop + 1
        iwk(itop) = itest
   30 continue
      inow = inow + 1
      if (inow.lt.lvltop) go to 20
      lvlwth = lvltop - lvlbot
      if (maxlw.lt.lvlwth) maxlw = lvlwth
      if (maxlw.ge.ibort) return
      if (itop.lt.lvltop) return
      lvlbot = inow
      lvltop = itop + 1
      go to 10
      end
      subroutine sortdg(stk1, stk2, x1, x2, ndeg)
c sortdg sorts stk2 by degree of the node and adds it to the end
c of stk1 in order of lowest to highest degree.  x1 and x2 are the
c number of nodes in stk1 and stk2 respectively.
      integer x1, x2, stk1, stk2, temp
      common /gra/ n, idpth, ideg
      dimension ndeg(1), stk1(1), stk2(1)
      ind = x2
   10 itest = 0
      ind = ind - 1
      if (ind.lt.1) go to 30
      do 20 i=1,ind
        j = i + 1
        istk2 = stk2(i)
        jstk2 = stk2(j)
        if (ndeg(istk2).le.ndeg(jstk2)) go to 20
        itest = 1
        temp = stk2(i)
        stk2(i) = stk2(j)
        stk2(j) = temp
   20 continue
      if (itest.eq.1) go to 10
   30 do 40 i=1,x2
        x1 = x1 + 1
        stk1(x1) = stk2(i)
   40 continue
      return
      end
      subroutine setup(lvl, lvls1, lvls2)
c setup computes the reverse leveling info from lvls2 and stores
c it into lvls2.  nacum(i) is initialized to nodes/ith level for nodes
c on the pseudo-diameter of the graph.  lvl is initialized to non-
c zero for nodes on the pseudo-diam and nodes in a different
c component of the graph.
      common /gra/ n, idpth, ideg
c it is assumed that there are at most 100 levels.
      common /lvlw/ nhigh(1000), nlow(1000), nacum(1000)
      dimension lvl(1), lvls1(1), lvls2(1)
      do 10 i=1,idpth
        nacum(i) = 0
   10 continue
      do 30 i=1,n
        lvl(i) = 1
        lvls2(i) = idpth + 1 - lvls2(i)
        itemp = lvls2(i)
        if (itemp.gt.idpth) go to 30
        if (itemp.ne.lvls1(i)) go to 20
        nacum(itemp) = nacum(itemp) + 1
        go to 30
   20   lvl(i) = 0
   30 continue
      return
      end
      integer function sort2(dmy)
c sort2 sorts size and stpt into descending order according to
c values of size. xc=number of entries in each array
      integer temp, ccstor, size, stpt, xc
c it is assumed that the graph has at most 50 connected components.
      common /cc/ xc, size(500), stpt(500)
      sort2 = 0
      if (xc.eq.0) return
      sort2 = 1
      ind = xc
   10 itest = 0
      ind = ind - 1
      if (ind.lt.1) return
      do 20 i=1,ind
        j = i + 1
        if (size(i).ge.size(j)) go to 20
        itest = 1
        temp = size(i)
        size(i) = size(j)
        size(j) = temp
        temp = stpt(i)
        stpt(i) = stpt(j)
        stpt(j) = temp
   20 continue
      if (itest.eq.1) go to 10
      return
      end
      subroutine piklvl(lvls1, lvls2, ccstor, idflt, isdir)
c piklvl chooses the level structure  used in numbering graph
c lvls1-       on input contains forward leveling info
c lvls2-       on input contains reverse leveling info
c              on output the final level structure chosen
c ccstor-      on input contains connected component info
c idflt-       on input =1 if wdth lvls1.le.wdth lvls2, =2 otherwise
c nhigh        keeps track of level widths for high numbering
c nlow-        keeps track of level widths for low numbering
c nacum-       keeps track of level widths for chosen level structure
c xc-          number of connected components
c size(i)-     size of ith connected component
c stpt(i)-     index into ccstore of 1st node in ith con compt
c isdir-       flag which indicates which way the largest connected
c              component fell.  =+1 if low and -1 if high
      integer ccstor, size, stpt, xc, end
      common /gra/ n, idpth, ideg
c it is assumed that the graph has at most 50 components and
c that there are at most 100 levels.
      common /lvlw/ nhigh(1000), nlow(1000), nacum(1000)
      common /cc/ xc, size(500), stpt(500)
      dimension lvls1(1), lvls2(1), ccstor(1)
c for each connected component do
      do 80 i=1,xc
        j = stpt(i)
        end = size(i) + j - 1
c set nhigh and nlow equal to nacum
        do 10 k=1,idpth
          nhigh(k) = nacum(k)
          nlow(k) = nacum(k)
   10   continue
c update nhigh and nlow for each node in connected component
        do 20 k=j,end
          inode = ccstor(k)
          lvlnh = lvls1(inode)
          nhigh(lvlnh) = nhigh(lvlnh) + 1
          lvlnl = lvls2(inode)
          nlow(lvlnl) = nlow(lvlnl) + 1
   20   continue
        max1 = 0
        max2 = 0
c set max1=largest new number in nhigh
c set max2=largest new number in nlow
        do 30 k=1,idpth
          if (2*nacum(k).eq.nlow(k)+nhigh(k)) go to 30
          if (nhigh(k).gt.max1) max1 = nhigh(k)
          if (nlow(k).gt.max2) max2 = nlow(k)
   30   continue
c set it= number of level structure to be used
        it = 1
        if (max1.gt.max2) it = 2
        if (max1.eq.max2) it = idflt
        if (it.eq.2) go to 60
        if (i.eq.1) isdir = -1
c copy lvls1 into lvls2 for each node in connected component
        do 40 k=j,end
          inode = ccstor(k)
          lvls2(inode) = lvls1(inode)
   40   continue
c update nacum to be the same as nhigh
        do 50 k=1,idpth
          nacum(k) = nhigh(k)
   50   continue
        go to 80
c update nacum to be the same as nlow
   60   do 70 k=1,idpth
          nacum(k) = nlow(k)
   70   continue
   80 continue
      return
      end
      subroutine number(snd, num, ndstk, lvls2, ndeg, renum, lvlst,
     * lstpt, nr, nflg, ibw2, ipf2, ipfa, isdir)
c  number produces the numbering of the graph for min bandwidth
c  snd-         on input the node to begin numbering on
c  num-         on input and output, the next available number
c  lvls2-       the level structure to be used in numbering
c  renum-       the array used to store the new numbering
c  lvlst-       on output contains level structure
c  lstpt(i)-    on output, index into lvlst to first node in ith lvl
c               lstpt(i+1) - lstpt(i) = number of nodes in ith lvl
c  nflg-        =+1 if snd is forward end of pseudo-diam
c               =-1 if snd is reverse end of pseudo-diam
c  ibw2-        bandwidth of new numbering computed by number
c  ipf2-        profile of new numbering computed by number
c  ipfa-        working storage used to compute profile and bandwidth
c  isdir-       indicates step direction used in numbering(+1 or -1)
c use integer*2 ndstk  with an ibm 360 or 370.
      integer ndstk
      integer snd, stka, stkb, stkc, stkd, xa, xb, xc, xd, cx, end,
     * renum, test
      common /gra/ n, idpth, ideg
c the storage in common blocks cc and lvlw is now free and can
c be used for stacks.
      common /lvlw/ stka(1000), stkb(1000), stkc(1000)
      common /cc/ stkd(1000)
      dimension ipfa(1)
      dimension ndstk(nr,1), lvls2(1), ndeg(1), renum(1), lvlst(1),
     * lstpt(1)
c set up lvlst and lstpt from lvls2
      do 10 i=1,n
        ipfa(i) = 0
   10 continue
      nstpt = 1
      do 30 i=1,idpth
        lstpt(i) = nstpt
        do 20 j=1,n
          if (lvls2(j).ne.i) go to 20
          lvlst(nstpt) = j
          nstpt = nstpt + 1
   20   continue
   30 continue
      lstpt(idpth+1) = nstpt
c stka, stkb, stkc and stkd are stacks with pointers
c xa,xb,xc, and xd.  cx is a special pointer into stkc which
c indicates the particular node being processed.
c lvln keeps track of the level we are working at.
c initially stkc contains only the initial node, snd.
      lvln = 0
      if (nflg.lt.0) lvln = idpth + 1
      xc = 1
      stkc(xc) = snd
   40 cx = 1
      xd = 0
      lvln = lvln + nflg
      lst = lstpt(lvln)
      lnd = lstpt(lvln+1) - 1
c begin processing node stkc(cx)
   50 ipro = stkc(cx)
      renum(ipro) = num
      num = num + isdir
      end = ndeg(ipro)
      xa = 0
      xb = 0
c check all adjacent nodes
      do 80 i=1,end
        test = ndstk(ipro,i)
        inx = renum(test)
c only nodes not numbered or already on a stack are added
        if (inx.eq.0) go to 60
        if (inx.lt.0) go to 80
c do preliminary bandwidth and profile calculations
        nbw = (renum(ipro)-inx)*isdir
        if (isdir.gt.0) inx = renum(ipro)
        if (ipfa(inx).lt.nbw) ipfa(inx) = nbw
        go to 80
   60   renum(test) = -1
c put nodes on same level on stka, all others on stkb
        if (lvls2(test).eq.lvls2(ipro)) go to 70
        xb = xb + 1
        stkb(xb) = test
        go to 80
   70   xa = xa + 1
        stka(xa) = test
   80 continue
c sort stka and stkb into increasing degree and add stka to stkc
c and stkb to stkd
      if (xa.eq.0) go to 100
      if (xa.eq.1) go to 90
      call sortdg(stkc, stka, xc, xa, ndeg)
      go to 100
   90 xc = xc + 1
      stkc(xc) = stka(xa)
  100 if (xb.eq.0) go to 120
      if (xb.eq.1) go to 110
      call sortdg(stkd, stkb, xd, xb, ndeg)
      go to 120
  110 xd = xd + 1
      stkd(xd) = stkb(xb)
c be sure to process all nodes in stkc
  120 cx = cx + 1
      if (xc.ge.cx) go to 50
c when stkc is exhausted look for min degree node in same level
c which has not been processed
      max = ideg + 1
      snd = n + 1
      do 130 i=lst,lnd
        test = lvlst(i)
        if (renum(test).ne.0) go to 130
        if (ndeg(test).ge.max) go to 130
        renum(snd) = 0
        renum(test) = -1
        max = ndeg(test)
        snd = test
  130 continue
      if (snd.eq.n+1) go to 140
      xc = xc + 1
      stkc(xc) = snd
      go to 50
c if stkd is empty we are done, otherwise copy stkd onto stkc
c and begin processing new stkc
  140 if (xd.eq.0) go to 160
      do 150 i=1,xd
        stkc(i) = stkd(i)
  150 continue
      xc = xd
      go to 40
c do final bandwidth and profile calculations
  160 do 170 i=1,n
        if (ipfa(i).gt.ibw2) ibw2 = ipfa(i)
        ipf2 = ipf2 + ipfa(i)
  170 continue
      return
      end
