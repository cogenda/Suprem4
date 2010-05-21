define NN 3000

	integer function tomcal (neq, maxdeg, ia, reord, adjstk, lvlsc)
	integer ia(0:*)
	integer reord(0:*)
	integer adjstk(neq, maxdeg)
	integer neq, maxdeg
	integer lvlsc(0:*)

	common /gra/ n, idpth, ideg
	integer n, idpth, ideg

#...Locals
	integer iold(NN)
	integer renum(NN)
	integer ndeg(NN)
	integer lvl(NN)
	integer lvls1(NN)
	integer lvls2(NN)
	integer ccstor(NN)

	integer in, j, cnt, nr, ibw2, ipf2

	n = ia(0)-1
#	First order of the day is to copy ia into a different format...
	do in = 1, n
		do j = 1, maxdeg
			adjstk(in,j) = 0;
	ideg = 0;
	do in = 0, n-1 {
		cnt = 0;
		do j = ia(in), ia(in+1)-1
			if(ia(j) != in) {
				cnt = cnt+1
				adjstk(in+1, cnt) = ia(j) + 1;
			}
		if (cnt > ideg) ideg = cnt;
	}
	if (ideg > maxdeg) {tomcal = -1; return}

	do j = 1, n
		iold(j) = j;
	nr = neq;
	call reduce( adjstk, nr, iold, renum, ndeg, lvl, lvls1, lvls2,_
               ccstor, ibw2, ipf2)

	do j=1,n
		reord(j-1) = renum(j)-1;
	do j=1,n
		lvlsc(j-1) = lvls2(j);

	tomcal = 0	
	return
	end
