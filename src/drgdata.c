/* 
 * Copyright (C) 2012  Manuel Argüelles <manuel.arguelles@gmail.com>
 *
 * This file is part of drg2sbg.
 *
 * Drg2sbg is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include "drgdata.h"

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

struct drgdata_ {
    unsigned char *data[MAX_ELEMENTS];
    int len[MAX_ELEMENTS];
    int alloc[MAX_ELEMENTS];
};

/* Prototypes */
static const char *element_to_text(int element);


static const char *element_to_text(int element)
{
    char *str;

    switch (element) {
    case HEADERS:
        str = "headers section";
        break;
    case IMAGE:
        str = "image section";
        break;
    case INFO:
        str = "information section";
        break;
    case SBG_DATA:
        str = "sbagen section";
        break;
    default:
        str = "unknown section";
        break;
    }

    return str;
}

DrgData *drg_data_new(void) 
{
    int i;
    DrgData *drg;
    drg = malloc(sizeof(DrgData));
    if (drg == NULL) {
        return NULL;
    }

    for (i = 0; i < MAX_ELEMENTS; i++) {
        drg->data[i] = malloc(1024);
        drg->len[i] = 0;
        drg->alloc[i] = 1024;
    }
    
    return drg;
}

void drg_data_free(DrgData *drg)
{
    int i;
    for (i = 0; i < MAX_ELEMENTS; i++) {
        free(drg->data[i]);
    }
    free(drg);
}

void drg_add_byte(DrgData *drg, int element, int byte)
{
    assert(drg != NULL);
    if (element >= MAX_ELEMENTS)
        return;

    if (drg->len[element] < drg->alloc[element]) {
        drg->data[element][drg->len[element]] = (unsigned char)byte;
        drg->len[element]++;
    } else {
        unsigned char *new;
        drg->alloc[element]+=1024;
        new = realloc(drg->data[element], drg->alloc[element]);
        drg->data[element] = new;
        drg->data[element][drg->len[element]] = (unsigned char)byte;
        drg->len[element]++;
    }
}

char *drg_get_uncoded_data(DrgData *drg, int element)
{
    BIO *b64;
    BIO *mem;
    char *data;
    unsigned char temp;
    int a = 0, b = 0, i = 0, j = 0;

    unsigned char S[] = {
         22, 213, 140,  67, 234,  48, 108, 225,   6, 101, 194,  50,  44, 247,  58, 145,  20,  80, 241,
         60, 127, 154, 125,  33,  45, 166, 245,  84,  28, 110, 220,  56, 195, 181, 238, 109,  69, 216,
         31, 162,  61, 183,  74,  71, 129, 148, 170, 111, 137, 164, 179, 178,   9,  41, 160, 219,  77,
         93,  97, 143,  14, 158, 118, 152,   0, 221, 192, 116,  86,  65,  55, 173, 217,  32, 227, 119,
        102, 115, 254, 132,  95,  23,  49,  73, 211, 142,  66,  59,  85, 252, 138, 212, 243,  38, 134,
        165, 184,  13, 209, 124, 197, 141, 114,  43,  92, 133, 175, 205, 128,  68,  91, 104,  64, 126,
         39,  40,  46,  72, 139, 232, 182,   2, 131, 201, 188, 112, 200,  78, 159, 113, 237,  99, 249,
         90,   7,  47, 122,  36,  76, 117, 222, 149,  96,  82, 100, 208, 151, 198, 228,  94,  87, 190,
         42, 246,  10, 169, 171, 120,  51, 236, 255, 215, 191, 223,  54, 103,  89, 135,  57,  98, 176,
        161,  24, 235,  26,   3, 250, 233, 121,  79, 207, 242, 224,  11, 123, 193, 155, 157, 218, 186,
        244,  75, 167,  63, 206,  81,  29, 150, 229,   4,  15, 230,  37, 185,   1, 203,  35,  16, 136,
        204, 144, 253, 214, 168,  27, 189, 105, 231, 177,  18,  25,  52,  70,  88, 196, 210, 163, 239,
        156,  19,  34,  17, 202,  30,  21,  62, 147, 174, 240, 130,   8, 180, 106, 172,  83,  12, 146,
        251, 226,  53, 153, 107, 199, 248, 187, 5
    };

    if (element >= MAX_ELEMENTS) {
        fprintf(stderr, "ERROR: could not convert %s\n", 
                element_to_text(element));
        return NULL;
    }

    b64 = BIO_new(BIO_f_base64());

    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    mem = BIO_new_mem_buf(drg->data[element], drg->len[element]);
    
    mem = BIO_push(b64, mem);

    data = malloc(drg->len[element]);
    memset(data, 0, drg->len[element]);
    a = BIO_read(mem, data, drg->len[element]);

    BIO_set_close(mem, BIO_CLOSE);
    BIO_free(mem);

    if (a < 1) {
        fprintf(stderr, "ERROR: could not convert %s\n", 
                element_to_text(element));
        free(data);
        return NULL;
    }

    for (b = 0; b < a; b++) {
        i = (i +1) % 256;
        j = (j + S[i]) % 256;
        temp = S[i];
            S[i] = S[j];
            S[j] = temp;
        data[b] = data[b] ^ (S[(S[i] + S[j]) % 256]);
    }
    data[a] = '\0';

    return data;
}

