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

#ifndef DRG_BASE64_H
#define DRG_BASE64_H

/*
 * Base64 encodes data, the returned string should be freed after use.
 *
 * data      data to encode
 * data_len  size of the data
 * returns   newly allocated null terminated base64 string
 */
char *base64_encode(const unsigned char *data, const size_t data_len);

/*
 * Base64 decodes a string, the returned string should be freed after
 * use.
 * 
 * data        null terminated base64 string to decode
 * output_len  size of the decoded string
 * returns     newly allocated decoded string
 */
unsigned char *base64_decode(const char *data, const size_t data_len, 
                             size_t *output_len);

void encodeblock(unsigned char in[3], unsigned char out[4], int len);

#endif
