c/*************************************************************************
c *									 *
c *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
c *                      Junior University. All rights reserved.	 *
c *     This subroutine may not be used outside of the SUPREM4 computer	 *
c *     program without the prior written consent of Stanford University.*
c *									 *
c *     Copyright c 1991 The board of regents of the University of 	 *
c *                      Florida.  All rights reserved.			 *
c *     This subroutine may not be used outside of the SUPREM4 computer	 *
c *     program without the prior written consent of the University of 	 *
c *     Florida.							 *
c *									 *
c *									 *
c *************************************************************************/
c/*   vector.f                Version 5.1     */
c/*   Last Modification : 7/3/91 10:44:25 */
c/************************************************************************
c *									*
c *	Snumbac - This routine performs a for/back solve loop on the	*
c *  factored matric contained in l and with the rhs as passed.  The 	*
c *  answer is returned in the rhs vector.				*
c *									*
c *  Reference:  Crout's Algorithm					*
c *									*
c *  Original:	MEL	8/85						*
c *									*
c ************************************************************************/
      subroutine NUMBAC(n, il, off, l, rhs )
      integer n
      integer il(0:3000)
      integer off
      double precision l(0:3000)
      double precision rhs(0:3000)
c
c     local variables
      integer i, j
      integer endloop
      double precision a
c
c     do the for solve
      do 100 i = 0, n-1
	  endloop = il(i+1)-1
c$doit ivdep
	  do 200 j = il(i), endloop
	      rhs(il(j)) = rhs(il(j)) - l(j) * rhs(i)
200       continue
100   continue
c
c     the diagonal solve
      do 300 i = 0, n-1
300	  rhs(i) = rhs(i) * l(i)
c
c     /*do the for back solves as required to get answer*/
      do 400 i = n-1, 0, -1
	  endloop = il(i+1)-1
c$doit ivdep
	  do 500 j = il(i), endloop
500           rhs(i) = rhs(i) - rhs(il(j)) * l(j + off)
400   continue
      return
      end

      subroutine UPDATE_BODY(nv, alpha, ans, p, ap, qinvr, r, nm)
      integer nv
      double precision alpha, ans(0:*), p(0:*)
      double precision ap(0:*), qinvr(0:*), r(0:*), nm

      integer j
      double precision t3

      t3 = 0.0

c$doit ivdep
      do 100 j = 0, nv-1
	  ans(j) = ans(j) + alpha * p(j)
	  r(j) = r(j) - alpha * ap(j)
	  qinvr(j) = r(j)
	  t3 = t3 + r(j) * r(j)
100   continue

      nm = t3
      return
      end
