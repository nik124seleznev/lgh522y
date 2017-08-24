#ifndef __ASM_CPU_TYPE_H
#define __ASM_CPU_TYPE_H

/*
                            
 */
enum sparc_cpu {
  sun4m       = 0x00,
  sun4d       = 0x01,
  sun4e       = 0x02,
  sun4u       = 0x03, /*                */
  sun_unknown = 0x04,
  ap1000      = 0x05, /*                */
  sparc_leon  = 0x06, /*          */
};

#ifdef CONFIG_SPARC32
extern enum sparc_cpu sparc_cpu_model;

#define SUN4M_NCPUS            4              /*                               */

#else

#define sparc_cpu_model sun4u

#endif

#endif /*                  */
