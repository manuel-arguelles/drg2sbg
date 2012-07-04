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
#include "drgdata.h"
#include "config.h"

static void print_usage(char *prog_name)
{
    fprintf(stderr, "please use: %s [options] drgfile\n", prog_name);
    fprintf(stderr, "where options are:\n");
    fprintf(stderr, "   -v      Print program version and exit\n");
    fprintf(stderr, "   -o file Write to file (default to stdout)\n");
    fprintf(stderr, "\n");
}

static int output_file_idx(int argc, char *argv[])
{
    int i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "-o")==0 || strcmp(argv[i], "--output")==0)
            return ++i;
        i++;
    }
    return -1;
}

static int input_file_idx(int argc, char *argv[])
{
    int i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "-o")==0 || strcmp(argv[i], "--output")==0)
            i++;
        else if (strcmp(argv[i], "-v")==0 || strcmp(argv[i], "--version")==0)
            i = i; 
        else 
            return i;
        i++;
    }
    return -1;
}

static int print_version(int argc, char *argv[])
{
    int i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "-v")==0 || strcmp(argv[i], "--version")==0)
            return 1;
        i++;
    }
    return 0;   
}

static void print_formated(FILE *out, const char *string, int line_len)
{
    size_t size;
    size_t i;
    size_t cline = 1;

    if (string == NULL) 
        return;

    size = strlen(string);
    if (size == 0)
        return;
    
    fprintf(out, "## ");
    for (i = 0; i < size; i++) {
        if (string[i] == '\n') { 
            cline = 1;
            fprintf(out, "\n## ");
            continue;
        } else if (string[i] == ' ' && cline >= line_len) {
            cline = 1;
            fprintf(out, "\n## ");
            continue;
        }
        fputc(string[i], out);
        cline++;
    }
    fputc('\n', out);

}


int main(int argc, char *argv[])
{
    FILE *drg_fp;
    FILE *sbg_fp = NULL;
    DrgData *drg;
    int c, i = 0;
    char *output;
    
    i = print_version(argc, argv);
    if (i) {
        fprintf(stdout, "\n%s VERSION: %s\n\n", PACKAGE, VERSION);
        return 0;
    }
        
    i = input_file_idx(argc, argv);
    if (i == -1) {
        print_usage(argv[0]);
        return 1;
    }
    
    drg_fp = fopen(argv[i], "r");
    if (drg_fp == NULL) {
        fprintf(stderr, "could not open file %s: %s\n", argv[i], strerror(errno));
        return(1);
    }

    i = output_file_idx(argc, argv);
    if (i != -1) {
        sbg_fp = fopen(argv[i], "w");
        if (sbg_fp == NULL) {
            fprintf(stderr, "could not open output file %s: %s\n", argv[i], strerror(errno));
            return(1);
        }
    }   
    i = 0;

    drg = drg_data_new();
    while ((c = fgetc(drg_fp)) != EOF) {
        if (c == '@') {
            i++;
        } else if (c != '\n' && c != '\r') {
            drg_add_byte(drg, i, c);
        }
    }
    fclose(drg_fp);

    
    if (sbg_fp == NULL)
        sbg_fp = stdout;

    output = drg_get_uncoded_data(drg, INFO);
    print_formated(sbg_fp, output, 50);
    free(output);

    output = drg_get_uncoded_data(drg, SBG_DATA);
    if (output == NULL) {
        fprintf(stderr, "Error decoding drg file\n");   
        drg_data_free(drg);
        return 1;
    }

    
    fprintf(sbg_fp, "\n-SE\n%s\n", output);
    free(output);

    if (sbg_fp != stdout) 
        fclose(sbg_fp);
    
    drg_data_free(drg);
    
    return 0;
}


