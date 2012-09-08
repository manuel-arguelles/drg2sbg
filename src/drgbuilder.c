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
 * Drg2sbg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with drg2sbg.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "drgdata.h"
#include "base64.h"
#include "config.h"


/* Prototypes */
static void print_usage(char *prog_name);
static void print_version(void);
static void make_header(char *header);
static void drg_data_add_string(DrgData *drg, int element, const char *string);
static void drg_data_add_file(DrgData *drg, int element, FILE *fd);
static void drg_data_add_file_b64(DrgData *drg, int element, FILE *fd);


int main(int argc, char **argv)
{
    DrgData *drg;
    char *title = "Made with drgbuilder from drg2sbg";
    char header[6];

    FILE *dsc_fd = NULL;
    FILE *img_fd = NULL;
    FILE *sbg_fd = NULL;
    FILE *out_fd = NULL;

    int opt;
    int option_index;

    struct option long_option[] = {
        {"title", 1, 0, 't'},
        {"description", 1, 0, 'd'},
        {"image", 1, 0, 'i'},
        {"sbagen", 1, 0, 's'},
        {"output", 1, 0, 'o'},
        {"version", 0, 0, 'v'},
        {"help", 0, 0, 'h'},
        {0,0,0,0}
    };
    
    while ((opt = getopt_long(argc, argv, "t:d:i:s:o:vh", 
                              long_option, &option_index)) != -1) {
        switch (opt) {
        case 't':
            title = optarg;
            break;
        case 'd':
            if (!(dsc_fd = fopen(optarg, "r"))) {
                fprintf(stderr, "could not open file %s for reading: %s\n",
                        optarg, strerror(errno));
                return EXIT_FAILURE;
            }
            break;
        case 'i':
            if (!(img_fd = fopen(optarg, "r"))) {
                fprintf(stderr, "could not open file %s for reading: %s\n",
                        optarg, strerror(errno));
                return EXIT_FAILURE;
            }
            break;
        case 's':
            if (!(sbg_fd = fopen(optarg, "r"))) {
                fprintf(stderr, "could not open file %s for reading: %s\n",
                        optarg, strerror(errno));
                return EXIT_FAILURE;
            }
            break;
        case 'o':
            if (!(out_fd = fopen(optarg, "w"))) {
                fprintf(stderr, "could not open file %s for writing: %s\n",
                        optarg, strerror(errno));
                return EXIT_FAILURE;
            }
            break;
        case 'v':
            print_version();
            return EXIT_SUCCESS;
        case 'h':
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        default:
            fprintf(stderr, "Invalid option\n");
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (!out_fd) {
        out_fd = stdout;
    }

    if (!dsc_fd || !img_fd || !sbg_fd) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    drg = drg_data_new();
    if (drg == NULL) {
        fprintf(stderr, "Out of memory\n");
        return EXIT_FAILURE;
    }

    make_header(header);
    drg_data_add_string(drg, HEADER, header);
    drg_data_add_string(drg, TITLE, title);
    drg_data_add_file_b64(drg, IMAGE, img_fd);
    drg_data_add_file(drg, INFO, dsc_fd);
    drg_data_add_file(drg, SBG_DATA, sbg_fd);

  
    
    drg_dump_to_file(drg, HEADER, out_fd, -1);
    fprintf(out_fd, "\r\n");
    drg_dump_to_file(drg, TITLE, out_fd, 76);
    fprintf(out_fd, "@");
    drg_dump_to_file(drg, IMAGE, out_fd, 76);
    fprintf(out_fd, "@");
    drg_dump_to_file(drg, INFO, out_fd, 76);
    fprintf(out_fd, "@");
    drg_dump_to_file(drg, SBG_DATA, out_fd, 76);
    fprintf(out_fd, "@@");
    fprintf(out_fd, "\r\n");

    if (out_fd != stdout) {
        fclose(out_fd);
    }

    return EXIT_SUCCESS;

}

static void make_header(char *header)
{
    int rnd = 0;
    srand((unsigned int) time(NULL));

    rnd = (rand() + 99999) % 99999; 
    sprintf(header, "%05d", rnd);
    
}

static void drg_data_add_string(DrgData *drg, int element, const char *string)
{
    size_t i = 0;
    for (i = 0; i < strlen(string); i++) {
        drg_add_byte(drg, element, (int) string[i]);
    }
}

static void drg_data_add_file(DrgData *drg, int element, FILE *fd)
{
    int c;
    while ((c = fgetc(fd)) != EOF) {
        drg_add_byte(drg, element, c);
    }
}

static void drg_data_add_file_b64(DrgData *drg, int element, FILE *fd)
{
    unsigned char in[3], out[4];
    int linesize = 76;
    int len, i, blocksout = 0;
    while (!feof(fd)) {
        len = 0;
        for (i = 0; i < 3; i++) {
            in[i] = (unsigned char) fgetc(fd);
            if (!feof(fd)) {
                len++;
            } else {
                in[i] = 0;
            }
        }
        if (len) {
            encodeblock(in, out, len);
            for (i = 0; i < 4; i++) {
                drg_add_byte(drg, element, (int) out[i]);
            }
            blocksout++;
        }
        if (blocksout >= (linesize / 4) || feof(fd)) {
            if (blocksout) {
                drg_add_byte(drg, element, '\r');
                drg_add_byte(drg, element, '\n');
            }
            blocksout = 0;
        }
    }
}


static void print_usage(char *prog_name)
{
    fprintf(stderr, "please use: %s options\n", prog_name);
    fprintf(stderr, "where mandatory options are:\n");
    fprintf(stderr, "   -d file    Use description in file\n");
    fprintf(stderr, "   -i file    Use image in file\n");
    fprintf(stderr, "   -s file    Use sbagen data in file\n");
    fprintf(stderr, "optional options are:\n");
    fprintf(stderr, "   -t title   Set title\n");
    fprintf(stderr, "   -v         Print program version and exit\n");
    fprintf(stderr, "   -o file    Write to file (default to stdout)\n");
    fprintf(stderr, "\n");
}

static void print_version(void)
{
    fprintf(stdout, "%s, Version %s, Build %s\n\n", PACKAGE, VERSION, __DATE__);
    fprintf(stdout, "    Copyright (C) 2012  Manuel Argüelles <manuel.arguelles@gmail.com>\n\n");
    fprintf(stdout, "    This program is free software: you can redistribute it and/or modify\n");
    fprintf(stdout, "    it under the terms of the GNU General Public License as published by\n");
    fprintf(stdout, "    the Free Software Foundation, either version 2 of the License, or\n");
    fprintf(stdout, "    (at your option) any later version.\n\n");
    fprintf(stdout, "    This program is distributed in the hope that it will be useful,\n");
    fprintf(stdout, "    but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
    fprintf(stdout, "    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
    fprintf(stdout, "    GNU General Public License for more details.\n\n");
}
