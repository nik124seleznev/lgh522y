/*
 *	X86 CPU microcode early update for Linux
 *
 *	Copyright (C) 2012 Fenghua Yu <fenghua.yu@intel.com>
 *			   H Peter Anvin" <hpa@zytor.com>
 *
 *	This driver allows to early upgrade microcode on Intel processors
 *	belonging to IA-32 family - PentiumPro, Pentium II,
 *	Pentium III, Xeon, Pentium 4, etc.
 *
 *	Reference: Section 9.11 of Volume 3, IA-32 Intel Architecture
 *	Software Developer's Manual.
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */
#include <linux/module.h>
#include <asm/microcode_intel.h>
#include <asm/processor.h>

#define QCHAR(a, b, c, d) ((a) + ((b) << 8) + ((c) << 16) + ((d) << 24))
#define CPUID_INTEL1 QCHAR('G', 'e', 'n', 'u')
#define CPUID_INTEL2 QCHAR('i', 'n', 'e', 'I')
#define CPUID_INTEL3 QCHAR('n', 't', 'e', 'l')
#define CPUID_AMD1 QCHAR('A', 'u', 't', 'h')
#define CPUID_AMD2 QCHAR('e', 'n', 't', 'i')
#define CPUID_AMD3 QCHAR('c', 'A', 'M', 'D')

#define CPUID_IS(a, b, c, ebx, ecx, edx)	\
		(!((ebx ^ (a))|(edx ^ (b))|(ecx ^ (c))))

/*
                                                                            
                                       
  
                                                                               
                                                             
  
                                                               
 */
static int __cpuinit x86_vendor(void)
{
	u32 eax = 0x00000000;
	u32 ebx, ecx = 0, edx;

	native_cpuid(&eax, &ebx, &ecx, &edx);

	if (CPUID_IS(CPUID_INTEL1, CPUID_INTEL2, CPUID_INTEL3, ebx, ecx, edx))
		return X86_VENDOR_INTEL;

	if (CPUID_IS(CPUID_AMD1, CPUID_AMD2, CPUID_AMD3, ebx, ecx, edx))
		return X86_VENDOR_AMD;

	return X86_VENDOR_UNKNOWN;
}

static int __cpuinit x86_family(void)
{
	u32 eax = 0x00000001;
	u32 ebx, ecx = 0, edx;
	int x86;

	native_cpuid(&eax, &ebx, &ecx, &edx);

	x86 = (eax >> 8) & 0xf;
	if (x86 == 15)
		x86 += (eax >> 20) & 0xff;

	return x86;
}

void __init load_ucode_bsp(void)
{
	int vendor, x86;

	if (!have_cpuid_p())
		return;

	vendor = x86_vendor();
	x86 = x86_family();

	if (vendor == X86_VENDOR_INTEL && x86 >= 6)
		load_ucode_intel_bsp();
}

void __cpuinit load_ucode_ap(void)
{
	int vendor, x86;

	if (!have_cpuid_p())
		return;

	vendor = x86_vendor();
	x86 = x86_family();

	if (vendor == X86_VENDOR_INTEL && x86 >= 6)
		load_ucode_intel_ap();
}