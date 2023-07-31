/**
 * @file
 *
 * @brief "RefPack" compression implementation.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "codex.h"
#include <string.h>

bool CODEX_API REF_is(const void *data)
{
    unsigned packtype = ((const uint8_t *)data)[1] | (((const uint8_t *)data)[0] << 8);

    return packtype == 0x10FB || packtype == 0x11FB || packtype == 0x90FB || packtype == 0x91FB;
}

int CODEX_API REF_decode(void *dest, const void *src, int *size)
{
    const uint8_t *getp;
    uint8_t *ref;
    uint8_t *putp;
    uint8_t first;
    uint8_t second;
    uint8_t third;
    uint8_t forth;
    unsigned run;
    unsigned flags;
    int uncompressed_length;

    getp = (const uint8_t *)src;
    putp = (uint8_t *)dest;
    uncompressed_length = 0;

    if (getp) {
        flags = *getp++;
        flags = (flags << 8) + *getp++;

        if (flags & 0x8000) {
            if (flags & 0x100) {
                getp += 4;
            }

            uncompressed_length = *getp++;
            uncompressed_length = (uncompressed_length << 8) + *getp++;
            uncompressed_length = (uncompressed_length << 8) + *getp++;
            uncompressed_length = (uncompressed_length << 8) + *getp++;
        } else {
            if (flags & 0x100) {
                getp += 3;
            }

            uncompressed_length = *getp++;
            uncompressed_length = (uncompressed_length << 8) + *getp++;
            uncompressed_length = (uncompressed_length << 8) + *getp++;
        }

        while (true) {
            first = *getp;
            /* short command */
            if (!(first & 0x80)) {
                do {
                    second = getp[1];
                    getp += 2;
                    run = first & 3;

                    while (run--) {
                        *putp++ = *getp++;
                    }

                    ref = putp - 1 - (((first & 0x60) << 3) + second);
                    run = ((first & 0x1c) >> 2) + 3;
                    memcpy(putp, ref, run);
                    putp += run;
                    first = *getp;
                } while (!(first & 0x80));
            }

            /* medium command */
            if (!(first & 0x40)) {
                second = getp[1];
                third = getp[2];
                getp += 3;
                run = second >> 6;

                while (run--) {
                    *putp++ = *getp++;
                }

                ref = putp - 1 - (((second & 0x3f) << 8) + third);
                run = (first & 0x3f) + 4;
                memcpy(putp, ref, run);
                putp += run;

                continue;
            }

            /* long command */
            if (!(first & 0x20)) {
                second = getp[1];
                third = getp[2];
                forth = getp[3];
                getp += 4;
                run = first & 3;

                while (run--) {
                    *putp++ = *getp++;
                }

                ref = putp - 1 - (((first & 0x10) >> 4 << 16) + (second << 8) + third);
                run = ((first & 0x0c) >> 2 << 8) + forth + 5;
                memcpy(putp, ref, run);
                putp += run;

                continue;
            }

            ++getp;
            /* byte command */
            run = ((first & 0x1f) << 2) + 4;

            if (run <= 112) {
                memcpy(putp, getp, run);
                putp += run;
                getp += run;

                continue;
            }

            /* end marker and maybe a few extra bytes */
            run = first & 3;

            while (run--) {
                *putp++ = *getp++;
            }

            break;
        }
    }

    if (size != NULL) {
        *size = (int)((char *)getp - (char *)src);
    }

    return uncompressed_length;
}

#define HASHTABLE_SIZE 65536
#define MAXBACK 131072
#define MAXBACK_MASK (MAXBACK - 1)

static uint32_t REF_matchlen(const uint8_t *s, const uint8_t *d, uint32_t maxmatch)
{
    uint32_t current;

    for (current = 0; current < maxmatch && *s++ == *d++; ++current)
        ;

    return current;
}

static int REF_compress(const uint8_t *src, int sourcesize, uint8_t *compresseddata, int quick)
{
    int32_t len;
    uint32_t tlen;
    uint32_t tcost;
    uint32_t run;
    uint32_t toffset;
    uint32_t boffset;
    uint32_t blen;
    uint32_t bcost;
    uint32_t mlen;
    const uint8_t *tptr;
    const uint8_t *getp;
    const uint8_t *runp;
    uint8_t *putp;

    int countliterals = 0;
    int countshort = 0;
    int countlong = 0;
    int countvlong = 0;
    int32_t hash;
    int32_t hoffset;
    int32_t minhoffset;
    int i;
    int32_t *link;
    int32_t *hashtbl;
    int32_t *hashptr;

    len = sourcesize;
    putp = compresseddata;
    run = 0;
    getp = runp = src;

    hashtbl = malloc(sizeof(int32_t) * HASHTABLE_SIZE);
    link = malloc(sizeof(int32_t) * MAXBACK);
    hashptr = hashtbl;
    memset(hashtbl, 0xFF, sizeof(int32_t) * HASHTABLE_SIZE);

    while (len > 0) {
        boffset = 0;
        blen = bcost = 2;
        mlen = len < 1028 ? len : 1028;
        tptr = getp - 1;
        hash = 0x10 * getp[1] ^ (uint16_t)(getp[2] | ((uint16_t)getp[0] << 8));
        hoffset = hashtbl[hash];
        minhoffset = (getp - src - MAXBACK_MASK) < 0 ? 0 : getp - src - MAXBACK_MASK;

        if (hoffset >= minhoffset) {
            do {
                tptr = src + hoffset;

                if (getp[blen] == tptr[blen]) {
                    tlen = REF_matchlen(getp, tptr, mlen);

                    if (tlen > blen) {
                        toffset = (getp - 1) - tptr;

                        if (toffset < 1024 && tlen <= 10) { /* two byte short command */
                            tcost = 2;
                        } else if (toffset < 16384 && tlen <= 67) { /* three byte medium command */
                            tcost = 3;
                        } else { /* four byte long command */
                            tcost = 4;
                        }

                        if (tlen - tcost + 4 > blen - bcost + 4) {
                            blen = tlen;
                            bcost = tcost;
                            boffset = toffset;

                            if (blen >= 1028) {
                                break;
                            }
                        }
                    }
                }
            } while ((hoffset = link[hoffset & MAXBACK_MASK]) >= minhoffset);
        }

        if (bcost >= blen) {
            hoffset = (getp - src);
            link[hoffset & MAXBACK_MASK] = hashtbl[hash];
            hashtbl[hash] = hoffset;

            ++run;
            ++getp;
            --len;
        } else {
            while (run > 3) { /* literal block of data */
                tlen = (run & ~3) < 112 ? (run & ~3) : 112;
                run -= tlen;
                *putp++ = (uint8_t)(0xE0 + (tlen >> 2) - 1);
                memcpy(putp, runp, tlen);
                runp += tlen;
                putp += tlen;
                ++countliterals;
            }

            if (bcost == 2) /* two byte short command */
            {
                *putp++ = (uint8_t)(((boffset >> 8) << 5) + ((blen - 3) << 2) + run);
                *putp++ = (uint8_t)boffset;
                ++countshort;
            } else if (bcost == 3) { /* three byte medium command */
                *putp++ = (uint8_t)(0x80 + (blen - 4));
                *putp++ = (uint8_t)((run << 6) + (boffset >> 8));
                *putp++ = (uint8_t)boffset;
                ++countlong;
            } else { /* four byte long command */
                *putp++ = (uint8_t)(0xC0 + ((boffset >> 16) << 4) + (((blen - 5) >> 8) << 2) + run);
                *putp++ = (uint8_t)(boffset >> 8);
                *putp++ = (uint8_t)(boffset);
                *putp++ = (uint8_t)(blen - 5);
                ++countvlong;
            }

            if (run) {
                memcpy(putp, runp, run);
                putp += run;
                run = 0;
            }

            if (quick) {
                hoffset = (getp - src);
                link[hoffset & MAXBACK_MASK] = hashtbl[hash];
                hashtbl[hash] = hoffset;
                getp += blen;
            } else {
                for (i = 0; i < (int)blen; ++i) {
                    hash = 0x10 * getp[1] ^ (uint16_t)(getp[2] | ((uint16_t)getp[0] << 8));
                    hoffset = (getp - src);
                    link[hoffset & MAXBACK_MASK] = hashtbl[hash];
                    hashtbl[hash] = hoffset;
                    ++getp;
                }
            }

            runp = getp;
            len -= blen;
        }
    }

    while (run > 3) /* no match at end, use literal */
    {
        tlen = (run & ~3) < 112 ? (run & ~3) : 112;
        run -= tlen;
        *putp++ = (uint8_t)(0xE0 + (tlen >> 2) - 1);
        memcpy(putp, runp, tlen);
        runp += tlen;
        putp += tlen;
    }

    *putp++ = (uint8_t)(0xFC + run); /* end of stream command + 0..3 literal */

    if (run) {
        memcpy(putp, runp, run);
        putp += run;
    }

    free(link);
    free(hashtbl);

    return putp - compresseddata;
}

int CODEX_API REF_encode(void *compresseddata, const void *source, int sourcesize)
{
    uint8_t *putp = compresseddata;
    int header_len = 0;

    if (sourcesize < 0xFFFFFF) {
        putp[0] = 0x10;
        putp[1] = 0xFB;
        putp[2] = ((unsigned)sourcesize & 0xFF0000) >> 16;
        putp[3] = ((unsigned)sourcesize & 0xFF00) >> 8;
        putp[4] = ((unsigned)sourcesize & 0xFF);
        header_len = 5;
    } else {
        putp[0] = 0x90;
        putp[1] = 0xFB;
        putp[2] = ((unsigned)sourcesize & 0xFF000000) >> 24;
        putp[3] = ((unsigned)sourcesize & 0xFF0000) >> 16;
        putp[4] = ((unsigned)sourcesize & 0xFF00) >> 8;
        putp[5] = ((unsigned)sourcesize & 0xFF);
        header_len = 6;
    }

    return header_len + REF_compress(source, sourcesize, &putp[header_len], 0);
}
