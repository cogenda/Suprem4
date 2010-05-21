/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *     Copyright c 1989 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *									 *
 *************************************************************************/
/*   diff_init.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:50:00 */


#include <stdio.h>
#include "global.h"
#include "constant.h"
#include "impurity.h"
#include "defect.h"
#include "material.h"
#include "diffuse.h"
#include "check.h"

extern time_val();
extern double Bdiff_coeff();
extern Bboundary(), Bactive(), Gaactive(), Gaboundary();
extern double Asdiff_coeff();
extern Asboundary(), Asactive();
extern double Sbdiff_coeff();
extern Sbboundary(), Sbactive();
extern double Pdiff_coeff();
extern Pboundary(), Pactive();
extern double Idiff_coeff();
extern Icoupling(), Iboundary(), Iactive(), Itime_val();
extern double Vdiff_coeff();
extern Vcoupling(), Vboundary(), Vactive(), Vtime_val();
extern double O2diff_coeff(), H2Odiff_coeff();
extern O2boundary(), H2Oboundary();
extern double Audiff_coeff(); 
extern Auboundary(), Aucoupling();
extern double Csdiff_coeff(); 
extern Csboundary(); 
extern IVblock_set(), PSbblock_set(), BAsblock_set(), poisson_block();
extern neut_block_set();
extern double Bediff_coeff();
extern Beboundary(), Beactive();
extern double Mgdiff_coeff();
extern Mgboundary(), Mgactive();
extern double Sediff_coeff();
extern Seboundary(), Seactive();
extern double Sidiff_coeff();
extern Siboundary(), Siactive();
extern double Sndiff_coeff();
extern Snboundary(), Snactive();
extern double Gediff_coeff();
extern Geboundary(), Geactive();
extern double Zndiff_coeff();
extern Znboundary(), Znactive();
extern double Cdiff_coeff();
extern Cboundary(), Cactive();
extern double Gdiff_coeff();
extern Gboundary(), Gactive();


/************************************************************************
 *									*
 *	diffuse_init() - This routine sets up the diffusion constants	*
 *  and routine pointers for each impurity.				*
 *									*
 *  Original:	MEL	1/85						*
 *									*
 ************************************************************************/
diffuse_init()
{
    int i, j, k, l;
    int mat, mat2;

    damage_read = FALSE;

    /* When we have a materials clearinghouse, this will go there*/
    for (mat = 0; mat < MAXMAT; mat++)
	for (mat2 = 0; mat2 < MAXMAT; mat2++)
	    alpha[ mat][ mat2] = 1.0;

    /*set up each impurity in turn*/
    for(i = 0; i < MAXIMP; i++) {
	imptosol[i] = -1;
	soltoimp[i] = -1;

	/* clear all flags except acceptor and active */
	CLEAR_FLAGS(i, (ALL_FLAGS^(ACTIVE_ACCEPTOR|ACTIVE)));

	switch (i) {

	/*clear the flags for the velocities and stresses, active conc*/
	case XVEL: case YVEL: case DELA:
	case Asa: case Sba: case Pa: case Ba:
	case Sxx: case Syy: case Sxy: case GRN:
	case iBea: case iMga: case iSea: case iSia: case iSna:
	case iGea: case iZna: case iCa: case iGa:
	case V: case I: case O2: case H2O: case T: case Psi:
		break;

	/*the rest are mobile diffusers*/
	default:
		SET_FLAGS(i, (DIFFUSING | MOBILE) );
		break;
	}
    }


    /*vacancies*/
    impur[V].diff_coeff = Vdiff_coeff;
    impur[V].coupling = Vcoupling;
    impur[V].boundary = Vboundary;
    impur[V].block_set = IVblock_set;
    impur[V].active = NULL;
    impur[V].algebc = NULL;
    impur[V].time_val = Vtime_val;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[V].constant[mat][i][0] = 0.0;
	    impur[V].constant[mat][i][1] = 0.0;
	}
	for(j = 0; j < MAXCHG; j++) {
	    Dfrac0[V][mat][j] = 0.0;
	    DfracE[V][mat][j] = 0.0;
	    for(i = 0; i < MAXIMP; Kcouple[V][mat][i++][j] = 0.0);
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[V].seg[SEG0][mat][mat2] = 1.0;
	    impur[V].seg[SEGE][mat][mat2] = 0.0;
	    impur[V].seg[TRN0][mat][mat2] = 0.0;
	    impur[V].seg[TRNE][mat][mat2] = 0.0;
	}
    }
    SET_FLAGS(V, PSEUDO);


    /*interstitials*/
    impur[I].diff_coeff = Idiff_coeff;
    impur[I].coupling = Icoupling;
    impur[I].boundary = Iboundary;
    impur[I].block_set = IVblock_set;
    impur[I].active = NULL;
    impur[I].algebc = NULL;
    impur[I].time_val = Itime_val;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[I].constant[mat][i][0] = 0.9;
	    impur[I].constant[mat][i][1] = 0.0;
	}
	for(j = 0; j < MAXCHG; j++) {
		Dfrac0[I][mat][j] = 0.0;
		DfracE[I][mat][j] = 0.0;
		for(i = 0; i < MAXIMP; Kcouple[I][mat][i++][j] = 0.0);
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[I].seg[SEG0][mat][mat2] = 1.0;
	    impur[I].seg[SEGE][mat][mat2] = 0.0;
	    impur[I].seg[TRN0][mat][mat2] = 0.0;
	    impur[I].seg[TRNE][mat][mat2] = 0.0;
	}
    }
    SET_FLAGS(I, PSEUDO);

    /*Arsenic*/
    impur[As].diff_coeff = Asdiff_coeff;
    impur[As].boundary = Asboundary;
    impur[As].coupling = NULL;
    impur[As].active = Asactive;
    impur[As].algebc = NULL;
    impur[As].time_val = time_val;
    impur[As].block_set = BAsblock_set;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[As].constant[mat][i][0] = 0.0;
	    impur[As].constant[mat][i][1] = 0.0;
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[As].seg[SEG0][mat][mat2] = 1.0;
	    impur[As].seg[SEGE][mat][mat2] = 0.0;
	    impur[As].seg[TRN0][mat][mat2] = 0.0;
	    impur[As].seg[TRNE][mat][mat2] = 0.0;
	}
    }
    for(i = 0; i < 2; i++)
	for(j = 0; j < MAXMAT; j++)
	    for(k = 0; k < MAXIMP; k++)
		for(l = 0; l < MAXCHG; l++) Kcouple[i][j][k][l] = 0.0;

    /*Phosphorus*/
    impur[P].diff_coeff = Pdiff_coeff;
    impur[P].boundary = Pboundary;
    impur[P].coupling = NULL;
    impur[P].active = Pactive;
    impur[P].algebc = NULL;
    impur[P].time_val = time_val;
    impur[P].block_set = PSbblock_set;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[P].constant[mat][i][0] = 0.0;
	    impur[P].constant[mat][i][1] = 0.0;
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[P].seg[SEG0][mat][mat2] = 1.0;
	    impur[P].seg[SEGE][mat][mat2] = 0.0;
	    impur[P].seg[TRN0][mat][mat2] = 0.0;
	    impur[P].seg[TRNE][mat][mat2] = 0.0;
	}
    }

    /*Antimony*/
    impur[Sb].diff_coeff = Sbdiff_coeff;
    impur[Sb].boundary = Sbboundary;
    impur[Sb].coupling = NULL;
    impur[Sb].active = Sbactive;
    impur[Sb].algebc = NULL;
    impur[Sb].time_val = time_val;
    impur[Sb].block_set = PSbblock_set;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[Sb].constant[mat][i][0] = 0.0;
	    impur[Sb].constant[mat][i][1] = 0.0;
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[Sb].seg[SEG0][mat][mat2] = 1.0;
	    impur[Sb].seg[SEGE][mat][mat2] = 0.0;
	    impur[Sb].seg[TRN0][mat][mat2] = 0.0;
	    impur[Sb].seg[TRNE][mat][mat2] = 0.0;
	}
    }

    /*Boron*/
    impur[Ga].diff_coeff = impur[B].diff_coeff = Bdiff_coeff;
    impur[B].boundary = Bboundary;
    impur[Ga].boundary = Gaboundary;
    impur[Ga].coupling = impur[B].coupling = NULL;
    impur[Ga].active = Gaactive;
    impur[B].active = Bactive;
    impur[Ga].algebc = impur[B].algebc = NULL;
    impur[Ga].time_val = impur[B].time_val = time_val;
    impur[Ga].block_set = neut_block_set;
    impur[B].block_set = BAsblock_set;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[Ga].constant[mat][i][0] = 0.0;
	    impur[Ga].constant[mat][i][1] = 0.0;
	    impur[B ].constant[mat][i][0] = 0.0;
	    impur[B ].constant[mat][i][1] = 0.0;
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[Ga].seg[SEG0][mat][mat2]=impur[B].seg[SEG0][mat][mat2] = 1.0;
	    impur[Ga].seg[SEGE][mat][mat2]=impur[B].seg[SEGE][mat][mat2] = 0.0;
	    impur[Ga].seg[TRN0][mat][mat2]=impur[B].seg[TRN0][mat][mat2] = 0.0;
	    impur[Ga].seg[TRNE][mat][mat2]=impur[B].seg[TRNE][mat][mat2] = 0.0;
	}
    }

    /*Oxidants*/
    impur[O2].diff_coeff = O2diff_coeff;
    impur[O2].boundary = O2boundary;
    impur[O2].coupling = NULL;
    impur[O2].active = NULL;
    impur[O2].algebc = NULL;
    impur[O2].time_val = time_val;
    impur[O2].block_set = neut_block_set;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[O2].constant[mat][i][0] = 0.0;
	    impur[O2].constant[mat][i][1] = 0.0;
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[O2].seg[SEG0][mat][mat2] = 1.0;
	    impur[O2].seg[SEGE][mat][mat2] = 0.0;
	    impur[O2].seg[TRN0][mat][mat2] = 0.0;
	    impur[O2].seg[TRNE][mat][mat2] = 0.0;
	}
    }
    SET_FLAGS(O2, (DIFFUSING | MOBILE | LOCKSTEP | STEADY) );

    impur[H2O].diff_coeff = H2Odiff_coeff;
    impur[H2O].boundary = H2Oboundary;
    impur[H2O].coupling = NULL;
    impur[H2O].active = NULL;
    impur[H2O].algebc = NULL;
    impur[H2O].time_val = time_val;
    impur[H2O].block_set = neut_block_set;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[H2O].constant[mat][i][0] = 0.0;
	    impur[H2O].constant[mat][i][1] = 0.0;
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[H2O].seg[0][mat][mat2] = 1.0;
	    impur[H2O].seg[1][mat][mat2] = 0.0;
	    impur[H2O].seg[SEG0][mat][mat2] = 1.0;
	    impur[H2O].seg[SEGE][mat][mat2] = 0.0;
	    impur[H2O].seg[TRN0][mat][mat2] = 0.0;
	    impur[H2O].seg[TRNE][mat][mat2] = 0.0;
	}
    }
    SET_FLAGS(H2O, (DIFFUSING | MOBILE | LOCKSTEP | STEADY) );

    impur[T].diff_coeff = NULL;
    impur[T].boundary = NULL;
    impur[T].coupling = NULL;
    impur[T].active = NULL;
    SET_FLAGS(T, PSEUDO);
    impur[T].algebc = NULL;
    impur[T].time_val = NULL;
    impur[T].block_set = IVblock_set;

    /*Gold*/
    impur[Au].diff_coeff = Audiff_coeff;
    impur[Au].boundary = Auboundary;
    impur[Au].coupling = Aucoupling;
    impur[Au].active = NULL;
    impur[Au].algebc = NULL;
    impur[Au].time_val = time_val;
    impur[Au].block_set = neut_block_set;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[Au].constant[mat][i][0] = 0.0;
	    impur[Au].constant[mat][i][1] = 0.0;
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[Au].seg[SEG0][mat][mat2] = 1.0;
	    impur[Au].seg[SEGE][mat][mat2] = 0.0;
	    impur[Au].seg[TRN0][mat][mat2] = 0.0;
	    impur[Au].seg[TRNE][mat][mat2] = 0.0;
	}
    }

    /*potential*/
#ifndef DEVICE
    SET_FLAGS(Psi, (PSEUDO | MOBILE | DIFFUSING | STEADY | LOCKSTEP));
    impur[Psi].diff_coeff = NULL;
    impur[Psi].boundary = NULL;
    impur[Psi].coupling = NULL;
    impur[Psi].active = NULL;
    impur[Psi].algebc = NULL;
    impur[Psi].time_val = NULL;
    impur[Psi].block_set = poisson_block;
#endif

    /*Cesium*/
    impur[Cs].diff_coeff = Csdiff_coeff;
    impur[Cs].boundary = Csboundary;
    impur[Cs].coupling = NULL;
    impur[Cs].active = NULL;
    impur[Cs].algebc = NULL;
    impur[Cs].time_val = time_val;
    impur[Cs].block_set = neut_block_set;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[Cs].constant[mat][i][0] = 0.0;
	    impur[Cs].constant[mat][i][1] = 0.0;
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[Cs].seg[SEG0][mat][mat2] = 1.0;
	    impur[Cs].seg[SEGE][mat][mat2] = 0.0;
	    impur[Cs].seg[TRN0][mat][mat2] = 0.0;
	    impur[Cs].seg[TRNE][mat][mat2] = 0.0;
	}
    }

    /*Beryllium*/
    impur[iBe].diff_coeff = Bediff_coeff;
    impur[iBe].boundary = Beboundary;
    impur[iBe].coupling = NULL;
    impur[iBe].active = Beactive;
    impur[iBe].algebc = NULL;
    impur[iBe].time_val = time_val;
    /*
    impur[iBe].block_set = neut_block_set;
     */
    impur[iBe].block_set = BAsblock_set;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[iBe].constant[mat][i][0] = 0.0;
	    impur[iBe].constant[mat][i][1] = 0.0;
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[iBe].seg[SEG0][mat][mat2] = 1.0;
	    impur[iBe].seg[SEGE][mat][mat2] = 0.0;
	    impur[iBe].seg[TRN0][mat][mat2] = 0.0;
	    impur[iBe].seg[TRNE][mat][mat2] = 0.0;
	}
    }

    /*Magnesium*/
    impur[iMg].diff_coeff = Mgdiff_coeff;
    impur[iMg].boundary = Mgboundary;
    impur[iMg].coupling = NULL;
    impur[iMg].active = Mgactive;
    impur[iMg].algebc = NULL;
    impur[iMg].time_val = time_val;
    /*
    impur[iMg].block_set = neut_block_set;
     */
    impur[iMg].block_set = BAsblock_set;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[iMg].constant[mat][i][0] = 0.0;
	    impur[iMg].constant[mat][i][1] = 0.0;
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[iMg].seg[SEG0][mat][mat2] = 1.0;
	    impur[iMg].seg[SEGE][mat][mat2] = 0.0;
	    impur[iMg].seg[TRN0][mat][mat2] = 0.0;
	    impur[iMg].seg[TRNE][mat][mat2] = 0.0;
	}
    }

    /*Selenium*/
    impur[iSe].diff_coeff = Sediff_coeff;
    impur[iSe].boundary = Seboundary;
    impur[iSe].coupling = NULL;
    impur[iSe].active = Seactive;
    impur[iSe].algebc = NULL;
    impur[iSe].time_val = time_val;
    /*
    impur[iSe].block_set = neut_block_set;
     */
    impur[iSe].block_set = BAsblock_set;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[iSe].constant[mat][i][0] = 0.0;
	    impur[iSe].constant[mat][i][1] = 0.0;
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[iSe].seg[SEG0][mat][mat2] = 1.0;
	    impur[iSe].seg[SEGE][mat][mat2] = 0.0;
	    impur[iSe].seg[TRN0][mat][mat2] = 0.0;
	    impur[iSe].seg[TRNE][mat][mat2] = 0.0;
	}
    }

    /*Silicon*/
    impur[iSi].diff_coeff = Sidiff_coeff;
    impur[iSi].boundary = Siboundary;
    impur[iSi].coupling = NULL;
    impur[iSi].active = Siactive;
    impur[iSi].algebc = NULL;
    impur[iSi].time_val = time_val;
    /*
    impur[iSi].block_set = neut_block_set;
     */
    impur[iSi].block_set = BAsblock_set;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[iSi].constant[mat][i][0] = 0.0;
	    impur[iSi].constant[mat][i][1] = 0.0;
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[iSi].seg[SEG0][mat][mat2] = 1.0;
	    impur[iSi].seg[SEGE][mat][mat2] = 0.0;
	    impur[iSi].seg[TRN0][mat][mat2] = 0.0;
	    impur[iSi].seg[TRNE][mat][mat2] = 0.0;
	}
    }

    /*Tin*/
    impur[iSn].diff_coeff = Sndiff_coeff;
    impur[iSn].boundary = Snboundary;
    impur[iSn].coupling = NULL;
    impur[iSn].active = Snactive;
    impur[iSn].algebc = NULL;
    impur[iSn].time_val = time_val;
    /*
    impur[iSn].block_set = neut_block_set;
     */
    impur[iSn].block_set = BAsblock_set;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[iSn].constant[mat][i][0] = 0.0;
	    impur[iSn].constant[mat][i][1] = 0.0;
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[iSn].seg[SEG0][mat][mat2] = 1.0;
	    impur[iSn].seg[SEGE][mat][mat2] = 0.0;
	    impur[iSn].seg[TRN0][mat][mat2] = 0.0;
	    impur[iSn].seg[TRNE][mat][mat2] = 0.0;
	}
    }

    /*Germanium*/
    impur[iGe].diff_coeff = Gediff_coeff;
    impur[iGe].boundary = Geboundary;
    impur[iGe].coupling = NULL;
    impur[iGe].active = Geactive;
    impur[iGe].algebc = NULL;
    impur[iGe].time_val = time_val;
    /*
    impur[iGe].block_set = neut_block_set;
     */
    impur[iGe].block_set = BAsblock_set;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[iGe].constant[mat][i][0] = 0.0;
	    impur[iGe].constant[mat][i][1] = 0.0;
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[iGe].seg[SEG0][mat][mat2] = 1.0;
	    impur[iGe].seg[SEGE][mat][mat2] = 0.0;
	    impur[iGe].seg[TRN0][mat][mat2] = 0.0;
	    impur[iGe].seg[TRNE][mat][mat2] = 0.0;
	}
    }

    /*Zinc*/
    impur[iZn].diff_coeff = Zndiff_coeff;
    impur[iZn].boundary = Znboundary;
    impur[iZn].coupling = NULL;
    impur[iZn].active = Znactive;
    impur[iZn].algebc = NULL;
    impur[iZn].time_val = time_val;
    /*
    impur[iZn].block_set = neut_block_set;
     */
    impur[iZn].block_set = BAsblock_set;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[iZn].constant[mat][i][0] = 0.0;
	    impur[iZn].constant[mat][i][1] = 0.0;
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[iZn].seg[SEG0][mat][mat2] = 1.0;
	    impur[iZn].seg[SEGE][mat][mat2] = 0.0;
	    impur[iZn].seg[TRN0][mat][mat2] = 0.0;
	    impur[iZn].seg[TRNE][mat][mat2] = 0.0;
	}
    }

    /*Carbon*/
    impur[iC].diff_coeff = Cdiff_coeff;
    impur[iC].boundary = Cboundary;
    impur[iC].coupling = NULL;
    impur[iC].active = Cactive;
    impur[iC].algebc = NULL;
    impur[iC].time_val = time_val;
    /*
    impur[iC].block_set = neut_block_set;
     */
    impur[iC].block_set = BAsblock_set;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[iC].constant[mat][i][0] = 0.0;
	    impur[iC].constant[mat][i][1] = 0.0;
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[iC].seg[SEG0][mat][mat2] = 1.0;
	    impur[iC].seg[SEGE][mat][mat2] = 0.0;
	    impur[iC].seg[TRN0][mat][mat2] = 0.0;
	    impur[iC].seg[TRNE][mat][mat2] = 0.0;
	}
    }

    /*Generic*/
    impur[iG].diff_coeff = Gdiff_coeff;
    impur[iG].boundary = Gboundary;
    impur[iG].coupling = NULL;
    impur[iG].active = Gactive;
    impur[iG].algebc = NULL;
    impur[iG].time_val = time_val;
    /*
    impur[iG].block_set = neut_block_set;
     */
    impur[iG].block_set = BAsblock_set;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
	    impur[iG].constant[mat][i][0] = 0.0;
	    impur[iG].constant[mat][i][1] = 0.0;
	}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[iG].seg[SEG0][mat][mat2] = 1.0;
	    impur[iG].seg[SEGE][mat][mat2] = 0.0;
	    impur[iG].seg[TRN0][mat][mat2] = 0.0;
	    impur[iG].seg[TRNE][mat][mat2] = 0.0;
	}
    }
}
