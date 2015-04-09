/*
 * dtutil.c - simple Flattened Device Tree (FDT) blob decoder
 *
 * Copyright (C) 2015
 *    Ruslan Bilovol (ruslan.bilovol@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdio.h>
#include <stdlib.h>
#include "dtutil.h"

#define swap32(n) \
    ( ((((u32) n) << 24) & 0xFF000000) |    \
      ((((u32) n) <<  8) & 0x00FF0000) |    \
      ((((u32) n) >>  8) & 0x0000FF00) |    \
      ((((u32) n) >> 24) & 0x000000FF) )

#define __be_32(n) swap32(n)

#define MAX_DT_BLOB_SIZE	10000
#define FDT_32_SIZE		4
#define FDT_64_SIZE		8

static void print_usage(char* exname)
{
	printf ("usage: %s blob.dtb\n", exname);
	exit(EXIT_FAILURE);
}

static void print_dt_header(struct fdt_header *h)
{
	printf("magic\t\t\t=0x%08x\n", h->magic);
	printf("totalsize\t\t=0x%08x\n", h->totalsize);
	printf("off_dt_struct\t\t=0x%08x\n", h->off_dt_struct);
	printf("off_dt_strings\t\t=0x%08x\n", h->off_dt_strings);
	printf("off_mem_rsvmap\t\t=0x%08x\n", h->off_mem_rsvmap);
	printf("version\t\t\t=0x%08x\n", h->version);
	printf("last_comp_version\t=0x%08x\n", h->last_comp_version);
	printf("boot_cpuid_phys\t\t=0x%08x\n", h->boot_cpuid_phys);
	printf("size_dt_strings\t\t=0x%08x\n", h->size_dt_strings);
	printf("size_dt_struct\t\t=0x%08x\n", h->size_dt_struct);
}

static void parse_dt_header(struct fdt_header *h, void *dt_blob)
{
	struct fdt_header *tmp;

	tmp = (struct fdt_header *)dt_blob;

	h->magic = __be_32(tmp->magic);
	h->totalsize = __be_32(tmp->totalsize);
	h->off_dt_struct = __be_32(tmp->off_dt_struct);
	h->off_dt_strings = __be_32(tmp->off_dt_strings);
	h->off_mem_rsvmap = __be_32(tmp->off_mem_rsvmap);
	h->version = __be_32(tmp->version);
	h->last_comp_version = __be_32(tmp->last_comp_version);
	h->boot_cpuid_phys = __be_32(tmp->boot_cpuid_phys);
	h->size_dt_strings = __be_32(tmp->size_dt_strings);
	h->size_dt_struct = __be_32(tmp->size_dt_struct);
}

/* FIXME: this function is ugly */
static void parse_and_print_dt_details(struct fdt_header *dt_header, void *dt_blob)
{
	void *dt_struct_start;
	int i, j;

	dt_struct_start = dt_blob + dt_header->off_dt_struct;

	for (i = 0; i < dt_header->size_dt_struct; ) {
		if ( __be_32(*((int *)(dt_struct_start + i))) == FDT_BEGIN_NODE) {
			i += FDT_32_SIZE;
			if (!(strcmp((dt_struct_start + i), "chosen"))) {
				printf ("found 'chosen' at 0x%x\n", (dt_header->off_dt_struct + i) );
				i += FDT_32_SIZE;
				for (; i < dt_header->size_dt_struct; ) {
					if ( __be_32(*((int *)(dt_struct_start + i))) == FDT_PROP) {
						struct fdt_property *fdt_prop;
						void *str;

						i += FDT_32_SIZE;
						fdt_prop = (struct fdt_property *)(dt_struct_start + i);
						str = dt_blob + __be_32(fdt_prop->nameoff) + dt_header->off_dt_strings;

						if (!strcmp(str, "linux,initrd-start")) {
							printf ("found 'linux,initrd-start' = 0x%x\n", __be_32(*((int *)(dt_struct_start + i + FDT_64_SIZE))));
							break;
						}
					}
					i += FDT_32_SIZE;
				}

				for (; i < dt_header->size_dt_struct; ) {
					if ( __be_32(*((int *)(dt_struct_start + i))) == FDT_PROP) {
						struct fdt_property *fdt_prop;
						void *str;

						i += FDT_32_SIZE;
						fdt_prop = (struct fdt_property *)(dt_struct_start + i);
						str = dt_blob + __be_32(fdt_prop->nameoff) + dt_header->off_dt_strings;
						if (!strcmp(str, "linux,initrd-end")) {
							printf ("found 'linux,initrd-end' = 0x%x\n", __be_32(*((int *)(dt_struct_start + i + FDT_64_SIZE))));
							break;
						}
					}
					i += FDT_32_SIZE;
				}
			}
		}
		i += FDT_32_SIZE;
	}
}

int main (int argc, char **argv)
{
	FILE *dt_blob_file;
	size_t dt_blob_file_size = 0;
	int dt_blob[MAX_DT_BLOB_SIZE];
	struct fdt_header dt_header;

	if (argc != 2)
		print_usage(argv[0]);

	dt_blob_file = fopen(argv[1], "r");

	if (!dt_blob_file) {
		printf("Error opening file %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	dt_blob_file_size = fread(dt_blob, 4, MAX_DT_BLOB_SIZE, dt_blob_file);
	if (dt_blob_file_size < sizeof(struct fdt_header)) {
		printf("Error: Size of file is less then size of FDT header. Invalid DT file?\n");
		fclose(dt_blob_file);
		exit(EXIT_FAILURE);
	}

	parse_dt_header(&dt_header, dt_blob);
	if (dt_header.magic != FDT_MAGIC) {
		printf("Error: FDT magic number is not found. Invalid DT file?\n");
		fclose(dt_blob_file);
		exit(EXIT_FAILURE);
	}

	print_dt_header(&dt_header);

	parse_and_print_dt_details(&dt_header, dt_blob);

	fclose(dt_blob_file);
	exit(EXIT_SUCCESS);
}

