h37329
s 00032/00019/00103
d D 2.1 93/02/03 15:13:17 suprem45 2 1
c Initial IV.GS release
e
s 00122/00000/00000
d D 1.1 93/02/03 15:12:04 suprem45 1 0
c date and time created 93/02/03 15:12:04 by suprem45
e
u
U
f e 0
t
T
I 1
/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   defect.h                Version 5.1     */
/*   Last Modification : 7/3/91  08:55:00 */

/************************************************************************
 *									*
 *	This file contains the definitions and constants for the 	*
 *  impurity data.  This also includes the defect data.  It depends on	*
 *  constant.h being included first.					*
 *									*
 ************************************************************************/

#ifdef STATIC_ALLOCATION_TIME
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN int damage_read;

/*define the number of charge states and their name*/
D 2
#define MAXCHG 5
E 2
I 2
#define MAXCHG 7
E 2
#define CN 0		/*neutral*/
#define CM 1		/*negative*/
#define CDM 2		/*double negative*/
D 2
#define CP 3		/*positive*/
#define CDP 4		/*double positive*/
E 2
I 2
#define CTM 3		/*tripple negative*/
#define CP 4		/*positive*/
#define CDP 5		/*double positive*/
#define CTP 6		/*tripple positive*/
E 2

#define which(A) (((A)==V)?0:1)

/*Fractional portions for each charge state*/
EXTERN double Dfrac[2][MAXMAT][MAXCHG];
EXTERN double Dfrac0[2][MAXMAT][MAXCHG];
EXTERN double DfracE[2][MAXMAT][MAXCHG];

/*the temperature coefficients for thermequilibrium between defects and impurities*/
EXTERN double Kcouple[2][MAXMAT][MAXIMP][MAXCHG];
#define Kcpl(A,M,I,C) Kcouple[which(A)][(M)][(I)][(C)]

/*get some space for the constants required*/
EXTERN double Dconst[2][35][MAXMAT][MAXMAT];
EXTERN int Dint[2][MAXMAT][MAXMAT];
EXTERN char *Dstr[2][2][MAXMAT][MAXMAT];

#define GROWTH	0x01
#define TIME  	0x02
#define RECOM  	0x04
#define SEGRE  	0x08

/*bulk storage terms*/
D 2
#define D0(A,M)		impur[A].constant[M][0]		/*diffusion coefficient*/
#define DE(A,M)		impur[A].constant[M][1]		/*diffusion coefficient*/
#define Di(A,M)		impur[A].constant[M][2]		/*diffusion coefficient*/
#define Kr0(A,M)	impur[A].constant[M][3]		/*bulk recombination rate*/
#define KrE(A,M)	impur[A].constant[M][4]		/*bulk recombination rate*/
#define Kr(A,M)		impur[A].constant[M][5]		/*bulk recombination rate*/
#define Cstar0(A,M)	impur[A].constant[M][6]		/*equilibrium constant*/
#define CstarE(A,M)	impur[A].constant[M][7]		/*equilibrium constant*/
#define Cstar(A,M)	impur[A].constant[M][8]		/*equilibrium constant*/
#define ktrap0(A,M)	impur[A].constant[M][9]		/*trap lifetime*/
#define ktrapE(A,M)	impur[A].constant[M][10]	/*trap lifetime*/
#define ktrap(A,M)	impur[A].constant[M][11]	/*trap lifetime*/
E 2
I 2
/*diffusion coefficient*/
#define D0(A,M)		impur[A].constant[M][0][0]
#define DE(A,M)		impur[A].constant[M][1][0]
#define Di(A,M)		impur[A].constant[M][2][0]
E 2

I 2
/*bulk recombination rate*/
#define Kr0(A,M)	impur[A].constant[M][3][0]
#define KrE(A,M)	impur[A].constant[M][4][0]
#define Kr(A,M)		impur[A].constant[M][5][0]

/*equilibrium constant*/
#define Cstar0(A,M)	impur[A].constant[M][6][0]
#define CstarE(A,M)	impur[A].constant[M][7][0]
#define Cstar(A,M)	impur[A].constant[M][8][0]

/*trap lifetime*/
#define ktrap0(A,M)	impur[A].constant[M][9][0]
#define ktrapE(A,M)	impur[A].constant[M][10][0]
#define ktrap(A,M)	impur[A].constant[M][11][0]

E 2
#define model(A,M1,M2)	Dint[which(A)][M1][M2]

/*interface properties*/
#define Ksurf0(A,M1,M2)	Dconst[which(A)][0][M1][M2]	/*surface recombination*/
#define KsurfE(A,M1,M2)	Dconst[which(A)][1][M1][M2]	/*surface recombination*/
#define Ksurf(A,M1,M2)	Dconst[which(A)][2][M1][M2]	/*surface recombination*/
#define Krat0(A,M1,M2)	Dconst[which(A)][3][M1][M2]	/*growing/inert ratio*/
#define KratE(A,M1,M2)	Dconst[which(A)][4][M1][M2]	/*growing/inert ratio*/
#define Krat(A,M1,M2)	Dconst[which(A)][5][M1][M2]	/*growing/inert ratio*/
#define Kpow0(A,M1,M2)	Dconst[which(A)][6][M1][M2]	/*power dependence on growth*/
#define KpowE(A,M1,M2)	Dconst[which(A)][7][M1][M2]	/*power dependence on growth*/
#define Kpow(A,M1,M2)	Dconst[which(A)][8][M1][M2]	/*power dependence on growth*/

/*time dependence injection*/
#define A0(A,M1,M2)	Dconst[which(A)][9][M1][M2]	/*time injection constant*/
#define AE(A,M1,M2)	Dconst[which(A)][10][M1][M2]	/*time injection constant*/
#define A(A,M1,M2)	Dconst[which(A)][11][M1][M2]	/*time injection constant*/
#define t00(A,M1,M2)	Dconst[which(A)][12][M1][M2]	/*linear time constant*/
#define t0E(A,M1,M2)	Dconst[which(A)][13][M1][M2]	/*linear time constant*/
#define t0(A,M1,M2)	Dconst[which(A)][14][M1][M2]	/*linear time constant*/
#define Tpow0(A,M1,M2)	Dconst[which(A)][15][M1][M2]	/*time dependence power*/
#define TpowE(A,M1,M2)	Dconst[which(A)][16][M1][M2]	/*time dependence power*/
#define Tpow(A,M1,M2)	Dconst[which(A)][17][M1][M2]	/*time dependence power*/

/*growth dependence injection*/
#define theta0(A,M1,M2)	Dconst[which(A)][18][M1][M2]	/*growth dependence*/
#define thetaE(A,M1,M2)	Dconst[which(A)][19][M1][M2]	/*growth dependence*/
#define theta(A,M1,M2)	Dconst[which(A)][20][M1][M2]	/*growth dependence*/
#define Gpow0(A,M1,M2)	Dconst[which(A)][21][M1][M2]	/*growth dependence power*/
#define GpowE(A,M1,M2)	Dconst[which(A)][22][M1][M2]	/*growth dependence power*/
#define Gpow(A,M1,M2)	Dconst[which(A)][23][M1][M2]	/*growth dependence power*/
#define vmole(A,M1,M2)	Dconst[which(A)][24][M1][M2]	/*molecular concentration term*/

/*segregation terms*/
#define Trn0(A,M1,M2)	Dconst[which(A)][25][M1][M2]	/*transport rate*/
#define TrnE(A,M1,M2)	Dconst[which(A)][26][M1][M2]	/*transport rate*/
#define Trn(A,M1,M2)	Dconst[which(A)][27][M1][M2]	/*transport rate*/
#define Seg0(A,M1,M2)	Dconst[which(A)][28][M1][M2]	/*segregation const*/
#define SegE(A,M1,M2)	Dconst[which(A)][29][M1][M2]	/*segregation const*/
#define Seg(A,M1,M2)	Dconst[which(A)][30][M1][M2]	/*segregation const*/

/*formula values*/
#define recstr(A,M1,M2) Dstr[which(A)][0][M1][M2]
#define injstr(A,M1,M2) Dstr[which(A)][1][M1][M2]

/*defect trap constants*/
D 2
#define Tfrac0(M)	impur[T].constant[M][0]		/*fill fraction at equilibrium*/
#define TfracE(M)	impur[T].constant[M][1]		/*fill fraction at equilibrium*/
#define Tfrac(M)	impur[T].constant[M][2]		/*fill fraction at equilibrium*/
#define Ttot(M)		impur[T].constant[M][3]		/*total trap concentration*/
E 2
I 2
/*fill fraction at equilibrium*/
#define Tfrac0(M)	impur[T].constant[M][0][0]
/*fill fraction at equilibrium*/
#define TfracE(M)	impur[T].constant[M][1][0]
/*fill fraction at equilibrium*/
#define Tfrac(M)	impur[T].constant[M][2][0]
/*total trap concentration*/
#define Ttot(M)		impur[T].constant[M][3][0]
E 2

EXTERN int trap_on;
E 1
