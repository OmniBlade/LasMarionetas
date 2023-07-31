/* Copyright (C) 2002 Jean-Marc Valin 
   File: modes.h

   Describes the different modes of the codec

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

#ifndef SPEEX_H
#define SPEEX_H

#include "speex_bits.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Values allowed for *ctl() requests */
#define SPEEX_SET_ENH 0
#define SPEEX_GET_ENH 1
/*Would be SPEEX_SET_FRAME_SIZE, but it's (currently) invalid*/
#define SPEEX_GET_FRAME_SIZE 3
#define SPEEX_SET_QUALITY 4
#define SPEEX_GET_QUALITY 5
#define SPEEX_SET_MODE 6
#define SPEEX_GET_MODE 7
#define SPEEX_SET_LOW_MODE 8
#define SPEEX_GET_LOW_MODE 9
#define SPEEX_SET_HIGH_MODE 10
#define SPEEX_GET_HIGH_MODE 11
#define SPEEX_SET_VBR 12
#define SPEEX_GET_VBR 13
#define SPEEX_SET_VBR_QUALITY 14
#define SPEEX_GET_VBR_QUALITY 15
#define SPEEX_SET_COMPLEXITY 16
#define SPEEX_GET_COMPLEXITY 17
/*Would be SPEEX_SET_FRAME_SIZE, but it's (currently) invalid*/
#define SPEEX_GET_BITRATE 19


/* Preserving compatibility:*/
#define SPEEX_SET_PF 0
#define SPEEX_GET_PF 1


/* Values allowed for mode queries */
#define SPEEX_MODE_FRAME_SIZE 0
#define SPEEX_SUBMODE_BITS_PER_FRAME 1


#define SPEEX_NB_MODES 2


struct SpeexMode;

/* Prototypes for mode function pointers */
typedef void *(*encoder_init_func)(struct SpeexMode *mode);
typedef void (*encoder_destroy_func)(void *st);
typedef void (*encode_func)(void *state, float *in, SpeexBits *bits);
typedef void *(*decoder_init_func)(struct SpeexMode *mode);
typedef void (*decoder_destroy_func)(void *st);
typedef void (*decode_func)(void *state, SpeexBits *bits, float *out);
typedef void (*encoder_ctl_func)(void *state, int request, void *ptr);
typedef void (*decoder_ctl_func)(void *state, int request, void *ptr);

typedef void (*mode_query_func)(void *mode, int request, void *ptr);

/** Struct defining a Speex mode */ 
typedef struct SpeexMode {
   /** Pointer to the low-level mode data */
   void *mode;

   /** Pointer to the mode query function */
   mode_query_func query;
   
   /** The name of the mode (you should not rely on this to identify the mode)*/
   char *modeName;

   /**ID of the mode*/
   int modeID;

   /**Version number of the bitstream (incremented every time we break
    bitstream compatibility*/
   int bitstream_version;

   /** Pointer to encoder initialization function */
   encoder_init_func enc_init;

   /** Pointer to encoder destruction function */
   encoder_destroy_func enc_destroy;

   /** Pointer to frame encoding function */
   encode_func enc;

   /** Pointer to decoder initialization function */
   decoder_init_func dec_init;

   /** Pointer to decoder destruction function */
   decoder_destroy_func dec_destroy;

   /** Pointer to frame decoding function */
   decode_func dec;

   /** ioctl-like requests for encoder */
   encoder_ctl_func enc_ctl;

   /** ioctl-like requests for decoder */
   decoder_ctl_func dec_ctl;

} SpeexMode;

/**Returns a handle to a newly created Speex encoder state structure. For now, the 
   "mode" arguent can be &nb_mode or &wb_mode . In the future, more modes may be 
   added. Note that for now if you have more than one channels to encode, you need 
   one state per channel.*/
void *speex_encoder_init(SpeexMode *mode);

/** Frees all resources associated to an existing Speex encoder state. */
void speex_encoder_destroy(void *state);

/** Uses an existing encoder state to encode one frame of speech pointed to by
    "in". The encoded bit-stream is saved in "bits".*/
void speex_encode(void *state, float *in, SpeexBits *bits);

/** Used like the ioctl function to control the encoder parameters */
void speex_encoder_ctl(void *state, int request, void *ptr);


/** Returns a handle to a newly created decoder state structure. For now, the mode
    arguent can be &nb_mode or &wb_mode . In the future, more modes may be added. 
    Note that for now if you have more than one channels to decode, you need one 
    state per channel. */ 
void *speex_decoder_init(SpeexMode *mode);

/** Frees all resources associated to an existing decoder state. */
void speex_decoder_destroy(void *state);

/** Uses an existing decoder state to decode one frame of speech from bit-stream 
    bits. The output speech is saved written to out. */
int speex_decode(void *state, SpeexBits *bits, float *out);

/** Used like the ioctl function to control the encoder parameters */
void speex_decoder_ctl(void *state, int request, void *ptr);


/** Query function for mode information */
void speex_mode_query(SpeexMode *mode, int request, void *ptr);


/** Default narrowband mode */
extern SpeexMode speex_nb_mode;

/** Default wideband mode */
extern SpeexMode speex_wb_mode;

/** List of all modes availavle */
extern SpeexMode *speex_mode_list[SPEEX_NB_MODES];

#ifdef __cplusplus
}
#endif


#endif
