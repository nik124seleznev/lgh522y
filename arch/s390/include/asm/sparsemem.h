#ifndef _ASM_S390_SPARSEMEM_H
#define _ASM_S390_SPARSEMEM_H

#ifdef CONFIG_64BIT

#define SECTION_SIZE_BITS	28
#define MAX_PHYSMEM_BITS	46

#else

#define SECTION_SIZE_BITS	25
#define MAX_PHYSMEM_BITS	31

#endif /*              */

#endif /*                       */