/*
 * dtutil.h
 *
 * Copyright (C) 2015
 *    Ruslan Bilovol (ruslan.bilovol@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __DTUTIL_H__
#define __DTUTIL_H__

typedef unsigned int u32;
typedef unsigned long long u64;

/*
 * FDT structures and constants as per
 * "Power.org™ Standard for Embedded Power Architecture™
 * Platform Requirements (ePAPR)" Version 1.1 – 08 April 2011
 */
#define FDT_MAGIC		0xd00dfeed
#define FDT_BEGIN_NODE		0x00000001
#define FDT_END_NODE		0x00000002
#define FDT_PROP		0x00000003
#define FDT_NOP			0x00000004
#define FDT_END			0x00000009

struct fdt_header {
	u32 magic;
	u32 totalsize;
	u32 off_dt_struct;
	u32 off_dt_strings;
	u32 off_mem_rsvmap;
	u32 version;
	u32 last_comp_version;
	u32 boot_cpuid_phys;
	u32 size_dt_strings;
	u32 size_dt_struct;
};

struct fdt_reserve_entry {
	u64 address;
	u64 size;
};

struct fdt_property {
	u32 len;
	u32 nameoff;
};

#endif /* __DTUTIL_H__ */
