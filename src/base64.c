/*
 * AUTHOR:         Bob Trower 08/04/01
 * PROJECT:        Crypt Data Packaging
 * COPYRIGHT:      Copyright (c) Trantor Standard Systems Inc., 2001
 * NOTE:           This source code may be used as you wish, subject to
 *                 the MIT license.
 *
 * LICENCE:        Copyright (c) 2001 Bob Trower, Trantor Standard Systems Inc.
 *
 *                 Permission is hereby granted, free of charge, to any person
 *                 obtaining a copy of this software and associated
 *                 documentation files (the "Software"), to deal in the
 *                 Software without restriction, including without limitation
 *                 the rights to use, copy, modify, merge, publish, distribute,
 *                 sublicense, and/or sell copies of the Software, and to
 *                 permit persons to whom the Software is furnished to do so,
 *                 subject to the following conditions:
 *
 *                 The above copyright notice and this permission notice shall
 *                 be included in all copies or substantial portions of the
 *                 Software.
 *
 *                 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 *                 KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *                 WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 *                 PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
 *                 OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *                 OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 *                 OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *                 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * URL:            http://base64.sourceforge.net/
 */

/*
 * Modified and adapted to drg2sbg by Manuel Argüelles
 * <manuel.arguelles@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base64.h"

/*
 * Translation Table as described in RFC1113
 */
static const char cb64[] = \
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
 * Translation Table to decode (created by author: Bob Trower)
 */
static const char cd64[] = \
	"|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/*
 * encodeblock
 *
 * encode 3 8-bit binary bytes as 4 '6-bit' characters
 */
void encodeblock(unsigned char in[3], unsigned char out[4], int len)
{
	out[0] = cb64[in[0] >> 2];
	out[1] = cb64[((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)];
	out[2] = (unsigned char) (len > 1 ? cb64[((in[1] & 0x0f) << 2) |
	                                         ((in[2] & 0xc0) >> 6)] : '=');
	out[3] = (unsigned char) (len > 2 ? cb64[in[2] & 0x3f] : '=');
}

/*
 * decodeblock
 *
 * decode 4 '6-bit' characters into 3 8-bit binary bytes
 */
static void decodeblock(unsigned char in[4], unsigned char out[3])
{
	out[0] = (unsigned char) (in[0] << 2 | in[1] >> 4);
	out[1] = (unsigned char) (in[1] << 4 | in[2] >> 2);
	out[2] = (unsigned char) (((in[2] << 6) & 0xc0) | in[3]);
}

char *base64_encode(const unsigned char *data, size_t data_len)
{
	unsigned char in[3], out[4];
	char *output = NULL;
	int i, len, outlen = 0;
	size_t j = 0;

	if (data_len < 1)
		return NULL;

	output = calloc((data_len * 2), sizeof(*output));
	if (output == NULL)
		return NULL;

	while(j < data_len) {
		len = 0;
		for (i = 0; i < 3; i++) {
			if (j < data_len) {
				in[i] = data[j++];
				len++;
			} else {
				in[i] = 0;
			}
		}
		if (len) {
			encodeblock(in, out, len);
			for (i = 0; i < 4; i++) {
				output[outlen++] = out[i];
			}
		}
	}
	output[outlen] = '\0';

	return output;
}

unsigned char *base64_decode(const char *data, size_t data_len,
                             size_t *output_len)
{
	unsigned char in[4], out[3], v;
	unsigned char *output = NULL;
	int i, len;
	size_t j = 0;

	if (output_len == NULL)
		return NULL;

	*output_len = 0;
	if (data_len < 1)
		return NULL;

	output = calloc(data_len, sizeof(*output));
	if (output == NULL)
		return NULL;

	while (j < data_len) {
		for (len = 0, i = 0; i < 4 && j < data_len; i++) {
			v = 0;
			while (j < data_len && v == 0) {
				v = (unsigned char) data[j++];
				v = (unsigned char) ((v < 43 || v > 122) ?
				                     0 : cd64[v - 43]);
				if (v) {
					v = (unsigned char) ((v == '$') ?
					                     0 : v - 61);
				}
			}
			if (j < data_len) {
				len++;
				if (v) {
					in[i] = (unsigned char) (v - 1);
				}
			} else {
				in[i] = 0;
			}
		}
		if (len) {
			decodeblock(in, out);
			for (i = 0; i < len - 1; i++) {
				output[(*output_len)++] = out[i];
			}
		}
	}

	return output;
}

