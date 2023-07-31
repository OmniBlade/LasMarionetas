/* Copyright (C) 2002 Jean-Marc Valin & David Rowe
   File: cb_search.c
   Overlapped codebook search

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

#ifndef CB_SEARCH_H
#define CB_SEARCH_H

#include "speex_bits.h"

typedef struct split_cb_params {
   int     subvect_size;
   int     nb_subvect;
   float  *shape_cb;
   int     shape_bits;
} split_cb_params;


void split_cb_search_nogain(
float target[],			/* target vector */
float ak[],			/* LPCs for this subframe */
float awk1[],			/* Weighted LPCs for this subframe */
float awk2[],			/* Weighted LPCs for this subframe */
void *par,                      /* Codebook/search parameters*/
int   p,                        /* number of LPC coeffs */
int   nsf,                      /* number of samples in subframe */
float *exc,
SpeexBits *bits,
float *stack,
int   complexity
);

void split_cb_search_shape_sign(
float target[],			/* target vector */
float ak[],			/* LPCs for this subframe */
float awk1[],			/* Weighted LPCs for this subframe */
float awk2[],			/* Weighted LPCs for this subframe */
void *par,                      /* Codebook/search parameters*/
int   p,                        /* number of LPC coeffs */
int   nsf,                      /* number of samples in subframe */
float *exc,
SpeexBits *bits,
float *stack,
int   complexity
);

void split_cb_shape_sign_unquant(
float *exc,
void *par,                      /* non-overlapping codebook */
int   nsf,                      /* number of samples in subframe */
SpeexBits *bits,
float *stack
);

void split_cb_nogain_unquant(
float *exc,
void *par,                      /* non-overlapping codebook */
int   nsf,                      /* number of samples in subframe */
SpeexBits *bits,
float *stack
);


void noise_codebook_quant(
float target[],			/* target vector */
float ak[],			/* LPCs for this subframe */
float awk1[],			/* Weighted LPCs for this subframe */
float awk2[],			/* Weighted LPCs for this subframe */
void *par,                      /* Codebook/search parameters*/
int   p,                        /* number of LPC coeffs */
int   nsf,                      /* number of samples in subframe */
float *exc,
SpeexBits *bits,
float *stack,
int   complexity
);


void noise_codebook_unquant(
float *exc,
void *par,                      /* non-overlapping codebook */
int   nsf,                      /* number of samples in subframe */
SpeexBits *bits,
float *stack
);

#endif
