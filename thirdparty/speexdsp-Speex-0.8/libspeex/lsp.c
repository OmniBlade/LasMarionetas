/*---------------------------------------------------------------------------*\
Original copyright
	FILE........: AKSLSPD.C
	TYPE........: Turbo C
	COMPANY.....: Voicetronix
	AUTHOR......: David Rowe
	DATE CREATED: 24/2/93

Modified by Jean-Marc Valin

   This file contains functions for converting Linear Prediction
   Coefficients (LPC) to Line Spectral Pair (LSP) and back. Note that the
   LSP coefficients are not in radians format but in the x domain of the
   unit circle.

   Speex License:

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "lsp.h"
#include "stack_alloc.h"


#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif


/*---------------------------------------------------------------------------*\

	FUNCTION....: cheb_poly_eva()

	AUTHOR......: David Rowe
	DATE CREATED: 24/2/93

    This function evalutes a series of chebyshev polynomials

\*---------------------------------------------------------------------------*/



float cheb_poly_eva(float *coef,float x,int m,float *stack)
/*  float coef[]  	coefficients of the polynomial to be evaluated 	*/
/*  float x   		the point where polynomial is to be evaluated 	*/
/*  int m 		order of the polynomial 			*/


{
    int i;
    float *T,*t,*u,*v,sum;

    /* Allocate memory for chebyshev series formulation */

    T=PUSH(stack, m/2+1);

    /* Initialise pointers */

    t = T;                          	/* T[i-2] 			*/
    *t++ = 1.0;
    u = t--;                        	/* T[i-1] 			*/
    *u++ = x;
    v = u--;                        	/* T[i] 			*/

    /* Evaluate chebyshev series formulation using iterative approach 	*/

    for(i=2;i<=m/2;i++)
	*v++ = (2*x)*(*u++) - *t++;  	/* T[i] = 2*x*T[i-1] - T[i-2]	*/

    sum=0.0;                        	/* initialise sum to zero 	*/
    t = T;                          	/* reset pointer 		*/

    /* Evaluate polynomial and return value also free memory space */

    for(i=0;i<=m/2;i++)
	sum+=coef[(m/2)-i]**t++;

    POP(stack);
    return sum;
}


/*---------------------------------------------------------------------------*\

	FUNCTION....: lpc_to_lsp()

	AUTHOR......: David Rowe
	DATE CREATED: 24/2/93

    This function converts LPC coefficients to LSP
    coefficients.

\*---------------------------------------------------------------------------*/


int lpc_to_lsp (float *a,int lpcrdr,float *freq,int nb,float delta, float *stack)
/*  float *a 		     	lpc coefficients			*/
/*  int lpcrdr			order of LPC coefficients (10) 		*/
/*  float *freq 	      	LSP frequencies in the x domain       	*/
/*  int nb			number of sub-intervals (4) 		*/
/*  float delta			grid spacing interval (0.02) 		*/


{

    float psuml,psumr,psumm,temp_xr,xl,xr,xm=0;
    float temp_psumr/*,temp_qsumr*/;
    int i,j,m,flag,k;
    float *Q;                 	/* ptrs for memory allocation 		*/
    float *P;
    float *px;                	/* ptrs of respective P'(z) & Q'(z)	*/
    float *qx;
    float *p;
    float *q;
    float *pt;                	/* ptr used for cheb_poly_eval()
				whether P' or Q' 			*/
    int roots=0;              	/* DR 8/2/94: number of roots found 	*/
    flag = 1;                	/*  program is searching for a root when,
				1 else has found one 			*/
    m = lpcrdr/2;            	/* order of P'(z) & Q'(z) polynimials 	*/


    /* Allocate memory space for polynomials */
    Q = PUSH(stack, (m+1));
    P = PUSH(stack, (m+1));

    /* determine P'(z)'s and Q'(z)'s coefficients where
      P'(z) = P(z)/(1 + z^(-1)) and Q'(z) = Q(z)/(1-z^(-1)) */

    px = P;                      /* initilaise ptrs 			*/
    qx = Q;
    p = px;
    q = qx;
    *px++ = 1.0;
    *qx++ = 1.0;
    for(i=1;i<=m;i++){
	*px++ = a[i]+a[lpcrdr+1-i]-*p++;
	*qx++ = a[i]-a[lpcrdr+1-i]+*q++;
    }
    px = P;
    qx = Q;
    for(i=0;i<m;i++){
	*px = 2**px;
	*qx = 2**qx;
	 px++;
	 qx++;
    }
    px = P;             	/* re-initialise ptrs 			*/
    qx = Q;

    /* Search for a zero in P'(z) polynomial first and then alternate to Q'(z).
    Keep alternating between the two polynomials as each zero is found 	*/

    xr = 0;             	/* initialise xr to zero 		*/
    xl = 1.0;               	/* start at point xl = 1 		*/


    for(j=0;j<lpcrdr;j++){
	if(j%2)            	/* determines whether P' or Q' is eval. */
	    pt = qx;
	else
	    pt = px;

	psuml = cheb_poly_eva(pt,xl,lpcrdr,stack);	/* evals poly. at xl 	*/
	flag = 1;
	while(flag && (xr >= -1.0)){
	    xr = xl - delta ;                  	/* interval spacing 	*/
	    psumr = cheb_poly_eva(pt,xr,lpcrdr,stack);/* poly(xl-delta_x) 	*/
	    temp_psumr = psumr;
	    temp_xr = xr;

    /* if no sign change increment xr and re-evaluate poly(xr). Repeat til
    sign change.
    if a sign change has occurred the interval is bisected and then
    checked again for a sign change which determines in which
    interval the zero lies in.
    If there is no sign change between poly(xm) and poly(xl) set interval
    between xm and xr else set interval between xl and xr and repeat till
    root is located within the specified limits 			*/

	    if((psumr*psuml)<0.0){
		roots++;

		psumm=psuml;
		for(k=0;k<=nb;k++){
		    xm = (xl+xr)/2;        	/* bisect the interval 	*/
		    psumm=cheb_poly_eva(pt,xm,lpcrdr,stack);
		    if(psumm*psuml>0.){
			psuml=psumm;
			xl=xm;
		    }
		    else{
			psumr=psumm;
			xr=xm;
		    }
		}

	       /* once zero is found, reset initial interval to xr 	*/
	       freq[j] = (xm);
	       xl = xm;
	       flag = 0;       		/* reset flag for next search 	*/
	    }
	    else{
		psuml=temp_psumr;
		xl=temp_xr;
	    }
	}
    }
    POP(stack);
    POP(stack);
    return(roots);
}


/*---------------------------------------------------------------------------*\

	FUNCTION....: lsp_to_lpc()

	AUTHOR......: David Rowe
	DATE CREATED: 24/2/93

    lsp_to_lpc: This function converts LSP coefficients to LPC
    coefficients.

\*---------------------------------------------------------------------------*/


void lsp_to_lpc(float *freq,float *ak,int lpcrdr, float *stack)
/*  float *freq 	array of LSP frequencies in the x domain	*/
/*  float *ak 		array of LPC coefficients 			*/
/*  int lpcrdr  	order of LPC coefficients 			*/


{
    int i,j;
    float xout1,xout2,xin1,xin2;
    float *Wp;
    float *pw,*n1,*n2,*n3,*n4=NULL;
    int m = lpcrdr/2;

    Wp = PUSH(stack, 4*m+2);
    pw = Wp;

    /* initialise contents of array */

    for(i=0;i<=4*m+1;i++){       	/* set contents of buffer to 0 */
	*pw++ = 0.0;
    }

    /* Set pointers up */

    pw = Wp;
    xin1 = 1.0;
    xin2 = 1.0;

    /* reconstruct P(z) and Q(z) by  cascading second order
      polynomials in form 1 - 2xz(-1) +z(-2), where x is the
      LSP coefficient */

    for(j=0;j<=lpcrdr;j++){
	for(i=0;i<m;i++){
	    n1 = pw+(i*4);
	    n2 = n1 + 1;
	    n3 = n2 + 1;
	    n4 = n3 + 1;
	    xout1 = xin1 - 2*(freq[2*i]) * *n1 + *n2;
	    xout2 = xin2 - 2*(freq[2*i+1]) * *n3 + *n4;
	    *n2 = *n1;
	    *n4 = *n3;
	    *n1 = xin1;
	    *n3 = xin2;
	    xin1 = xout1;
	    xin2 = xout2;
	}
	xout1 = xin1 + *(n4+1);
	xout2 = xin2 - *(n4+2);
	ak[j] = (xout1 + xout2)*0.5;
	*(n4+1) = xin1;
	*(n4+2) = xin2;

	xin1 = 0.0;
	xin2 = 0.0;
    }
    POP(stack);
}

/*Added by JMV
  Makes sure the LSPs are stable*/
void lsp_enforce_margin(float *lsp, int len, float margin)
{
   int i;
   if (lsp[0]<margin)
      lsp[0]=margin;
   if (lsp[len-1]>M_PI-margin)
      lsp[len]=margin;
   for (i=1;i<len-1;i++)
   {
      if (lsp[i]<lsp[i-1]+margin)
         lsp[i]=lsp[i-1]+margin;

      if (lsp[i]>lsp[i+1]-margin)
         lsp[i]= .5* (lsp[i] + lsp[i+1]-margin);
   }
}
