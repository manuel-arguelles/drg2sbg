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
 * along with Drg2sbg.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <locale.h>

#include "drgdata.h"
#include "base64.h"
#include "config.h"


static void print_usage(char *prog_name)
{
	fprintf(stderr, "please use: %s [options] drgfile\n", prog_name);
	fprintf(stderr, "where options are:\n");
	fprintf(stderr, "   -v         Print program version and exit\n");
	fprintf(stderr, "   -o file    Write to file (default to stdout)\n");
	fprintf(stderr, "   -r element Output raw element\n");
	fprintf(stderr, "\n");
}

static void print_raw_usage(char *prog_name)
{
	fprintf(stderr, "when using %s with '-r element' option\n", prog_name);
	fprintf(stderr, "element must be one of the drgfile elements:\n");
	fprintf(stderr, "   1  Header\n");
	fprintf(stderr, "   2  Title\n");
	fprintf(stderr, "   3  Image\n");
	fprintf(stderr, "   4  Description\n");
	fprintf(stderr, "   5  Sbagen data\n");
	fprintf(stderr, "\n");
}

static void print_version(void)
{
	fprintf(stdout, "%s, Version %s, Build %s\n\n", PACKAGE, VERSION,
	        __DATE__);
	fprintf(stdout, "    Copyright (C) 2012  Manuel Argüelles "
	        "<manuel.arguelles@gmail.com>\n\n");
	fprintf(stdout, "    This program is free software: you can "
	        "redistribute it and/or modify\n");
	fprintf(stdout, "    it under the terms of the GNU General Public "
	        "License as published by\n");
	fprintf(stdout, "    the Free Software Foundation, either version 2 "
	        "of the License, or\n");
	fprintf(stdout, "    (at your option) any later version.\n\n");
	fprintf(stdout, "    This program is distributed in the hope that it "
	        "will be useful,\n");
	fprintf(stdout, "    but WITHOUT ANY WARRANTY; without even the "
	        "implied warranty of\n");
	fprintf(stdout, "    MERCHANTABILITY or FITNESS FOR A PARTICULAR "
	        "PURPOSE.  See the\n");
	fprintf(stdout, "    GNU General Public License for more details.\n\n");
}

static int output_file_idx(int argc, char *argv[])
{
	int i = 1;
	while (i < argc) {
		if (strcmp(argv[i], "-o") == 0 ||
		    strcmp(argv[i], "--output") == 0)
			return ++i;
		i++;
	}
	return -1;
}

static int input_file_idx(int argc, char *argv[])
{
	int i = 1;
	while (i < argc) {
		if (strcmp(argv[i], "-o") == 0 ||
		    strcmp(argv[i], "--output") == 0)
			i++;
		else if (strcmp(argv[i], "-r") == 0 ||
		         strcmp(argv[i], "--raw") == 0)
			i++;
		else if (strcmp(argv[i], "-v") == 0 ||
		         strcmp(argv[i], "--version") == 0)
			i = i;
		else
			return i;
		i++;
	}
	return -1;
}

static int raw_output_idx(int argc, char *argv[])
{
	int i = 1;
	while (i < argc) {
		if (strcmp(argv[i], "-r") == 0 ||
		    strcmp(argv[i], "--raw") == 0)
			return ++i;
		i++;
	}
	return -1;
}

static int check_print_version(int argc, char *argv[])
{
	int i = 1;
	while (i < argc) {
		if (strcmp(argv[i], "-v")==0 || strcmp(argv[i], "--version")==0)
			return 1;
		i++;
	}
	return 0;
}

static void print_formated(FILE *out, const char *string, size_t line_len)
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

static void print_raw(FILE *out, DrgData *drg, int element)
{
	unsigned char *output = NULL;
	size_t len = 0;

	output = drg_get_uncoded_data(drg, element, &len);

	if (output) {
		fwrite(output, len, sizeof(unsigned char), out);
		free(output);
	}

	if (element != IMAGE)
		fprintf(out, "\n");
}

int main(int argc, char *argv[])
{
	FILE *drg_fp;
	FILE *sbg_fp = NULL;
	DrgData *drg;
	int c, i = 0;
	int raw = 0;
	char *output;

	setlocale(LC_ALL, "");

	i = check_print_version(argc, argv);
	if (i) {
		print_version();
		return EXIT_SUCCESS;
	}

	i = input_file_idx(argc, argv);
	if (i > 0) {
		drg_fp = fopen(argv[i], "r");
		if (drg_fp == NULL) {
			fprintf(stderr, "could not open file %s: %s\n", argv[i],
			        strerror(errno));
			return EXIT_FAILURE;
		}
	} else {
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	i = output_file_idx(argc, argv);
	if (i > 0) {
		if (i < argc) {
			sbg_fp = fopen(argv[i], "w");
			if (sbg_fp == NULL) {
				fprintf(stderr,
				        "could not open output file %s: %s\n",
				        argv[i], strerror(errno));
				return EXIT_FAILURE;
			}
		} else {
			fprintf(stderr,
			        "could not open output file (not specified)\n");
			return EXIT_FAILURE;
		}
	}

	i = raw_output_idx(argc, argv);
	if (i > 0) {
		if (i < argc) {
			raw = atoi(argv[i]);
			if (raw < 1 || raw > 5) {
				print_raw_usage(argv[0]);
				return EXIT_FAILURE;
			}
		} else {
			print_raw_usage(argv[0]);
			return EXIT_FAILURE;
		}
	}
	i = 0;

	drg = drg_data_new();
	/* The header is a special element, not separated by @ */
	while (( c = fgetc(drg_fp)) != EOF) {
		if (c == '\n' || c == '\r' || c == '@')
			break;
		else
			drg_add_byte(drg, i, c);
	}

	i = 1;
	/* Rest of elements separated by @ */
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

	if (raw == 0) {
		output = (char *) drg_get_uncoded_data(drg, INFO, NULL);
		print_formated(sbg_fp, output, 50);
		free(output);

		output = (char *) drg_get_uncoded_data(drg, SBG_DATA, NULL);
		if (output == NULL) {
			fprintf(stderr, "Error decoding drg file\n");
			drg_data_free(drg);
			return EXIT_FAILURE;
		}

		fprintf(sbg_fp, "\n-SE\n%s\n", output);
		free(output);
	} else {
		print_raw(sbg_fp, drg, raw - 1);
	}

	if (sbg_fp != stdout)
		fclose(sbg_fp);

	drg_data_free(drg);

	return 0;
}
