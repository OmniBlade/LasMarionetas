/* Copyright (C) 2002 Jean-Marc Valin 
   File: speex_header.c
   Describes the Speex header

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

#include "speex_header.h"
#include "misc.h"
#include <stdio.h>
#include "speex.h"
#include <stdlib.h>

#define ENDIAN_SWITCH(x) {x=le_int(x);}


/*
typedef struct SpeexHeader {
   char speex_string[8];
   char speex_version[SPEEX_HEADER_VERSION_LENGTH];
   int speex_header_version;
   int header_size;
   int rate;
   int mode;
   int mode_bitstream_version;
   int nb_channels;
   int bitrate;
   int frame_size;
   int vbr;
   int frames_per_packet;
   int reserved1;
   int reserved2;
   int reserved3;
} SpeexHeader;
*/

void speex_init_header(SpeexHeader *header, int rate, int nb_channels, SpeexMode *m)
{
   int i;
   char *h="Speex   ";
   /*
   strncpy(header->speex_string, "Speex   ", 8);
   strncpy(header->speex_version, VERSION, SPEEX_HEADER_VERSION_LENGTH-1);
   header->speex_version[SPEEX_HEADER_VERSION_LENGTH-1]=0;
   */
   for (i=0;i<8;i++)
      header->speex_string[i]=h[i];
   for (i=0;i<SPEEX_HEADER_VERSION_LENGTH-1 && VERSION[i];i++)
      header->speex_version[i]=VERSION[i];
   for (;i<SPEEX_HEADER_VERSION_LENGTH;i++)
      header->speex_version[i]=0;
   
   header->speex_header_version = SPEEX_HEADER_VERSION;
   header->header_size = sizeof(SpeexHeader);
   
   header->rate = rate;
   header->mode = m->modeID;
   header->mode_bitstream_version = m->bitstream_version;
   if (m->modeID<0)
      fprintf (stderr, "This mode is meant to be used alone\n");
   header->nb_channels = nb_channels;
   header->bitrate = -1;
   speex_mode_query(m, SPEEX_MODE_FRAME_SIZE, &header->frame_size);
   header->vbr = 0;
   
   header->frames_per_packet = 0;
   header->reserved1 = 0;
   header->reserved2 = 0;
   header->reserved3 = 0;
}

char *speex_header_to_packet(SpeexHeader *header, int *size)
{
   SpeexHeader *le_header;
   le_header = speex_alloc(sizeof(SpeexHeader));
   
   speex_move(le_header, header, sizeof(SpeexHeader));
   
   /*Make sure everything is now little-endian*/
   ENDIAN_SWITCH(le_header->speex_header_version);
   ENDIAN_SWITCH(le_header->header_size);
   ENDIAN_SWITCH(le_header->rate);
   ENDIAN_SWITCH(le_header->mode);
   ENDIAN_SWITCH(le_header->mode_bitstream_version);
   ENDIAN_SWITCH(le_header->nb_channels);
   ENDIAN_SWITCH(le_header->bitrate);
   ENDIAN_SWITCH(le_header->frame_size);
   ENDIAN_SWITCH(le_header->vbr);
   ENDIAN_SWITCH(le_header->frames_per_packet);

   *size = sizeof(SpeexHeader);
   return (char *)le_header;
}

SpeexHeader *speex_packet_to_header(char *packet, int size)
{
   int i;
   SpeexHeader *le_header;
   char *h = "Speex   ";
   for (i=0;i<8;i++)
      if (packet[i]!=h[i])
   {
      fprintf (stderr, "This doesn't look like a Speex file\n");
      return NULL;
   }

   if (sizeof(SpeexHeader) != size)
   {
      fprintf (stderr, "Speex header size mismarch\n");
      return NULL;
   }
   
   le_header = speex_alloc(sizeof(SpeexHeader));
   
   speex_move(le_header, packet, sizeof(SpeexHeader));
   
   /*Make sure everything is converted correctly from little-endian*/
   ENDIAN_SWITCH(le_header->speex_header_version);
   ENDIAN_SWITCH(le_header->header_size);
   ENDIAN_SWITCH(le_header->rate);
   ENDIAN_SWITCH(le_header->mode);
   ENDIAN_SWITCH(le_header->mode_bitstream_version);
   ENDIAN_SWITCH(le_header->nb_channels);
   ENDIAN_SWITCH(le_header->bitrate);
   ENDIAN_SWITCH(le_header->frame_size);
   ENDIAN_SWITCH(le_header->vbr);
   ENDIAN_SWITCH(le_header->frames_per_packet);

   return le_header;

}
