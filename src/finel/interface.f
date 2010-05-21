      integer function tomcal (neq, maxdeg, ia, reord, adjstk, lvlsc)
      integer ia(0:*)
      integer reord(0:*)
      integer adjstk(neq, maxdeg)
      integer neq, maxdeg
      integer lvlsc(0:*)
      common /gra/ n, idpth, ideg
      integer n, idpth, ideg
c...Locals
      integer iold(3000)
      integer renum(3000)
      integer ndeg(3000)
      integer lvl(3000)
      integer lvls1(3000)
      integer lvls2(3000)
      integer ccstor(3000)
      integer in, j, cnt, nr, ibw2, ipf2
      n = ia(0)-1
c	First order of the day is to copy ia into a different format...
      do 23000 in = 1, n
         do 23002 j = 1, maxdeg
            adjstk(in,j) = 0
23002       continue
23000    continue
      ideg = 0
      do 23004 in = 0, n-1 
         cnt = 0
         do 23006 j = ia(in), ia(in+1)-1
            if(.not.(ia(j) .ne. in))goto 23008
               cnt = cnt+1
               adjstk(in+1, cnt) = ia(j) + 1
23008       continue
23006       continue
         if(.not.(cnt .gt. ideg))goto 23010
            ideg = cnt
23010    continue
23004    continue
      if(.not.(ideg .gt. maxdeg))goto 23012
         tomcal = -1
         return
23012 continue
      do 23014 j = 1, n
         iold(j) = j
23014    continue
      nr = neq
      call reduce( adjstk, nr, iold, renum, ndeg, lvl, lvls1, lvls2, 
     c ccstor, ibw2, ipf2)
      do 23016 j=1,n
         reord(j-1) = renum(j)-1
23016    continue
      do 23018 j=1,n
         lvlsc(j-1) = lvls2(j)
23018    continue
      tomcal = 0
      return
      end
