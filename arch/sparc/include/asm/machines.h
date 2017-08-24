/*
 * machines.h:  Defines for taking apart the machine type value in the
 *              idprom and determining the kind of machine we are on.
 *
 * Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 */
#ifndef _SPARC_MACHINES_H
#define _SPARC_MACHINES_H

struct Sun_Machine_Models {
	char *name;
	unsigned char id_machtype;
};

/*                                                     
  
                  
                  
                  
                 
  
                                                                
                                                                       
 */

#define SM_ARCH_MASK  0xf0
#define  M_LEON       0x30
#define SM_SUN4M      0x70
#define SM_SUN4M_OBP  0x80

#define SM_TYP_MASK   0x0f

/*               */
#define M_LEON3_SOC   0x02    /*           */

/*                                                                    
                                                                 
                                                      
                                                                 
                                                                      
 */
#define SM_4M_SS60    0x01    /*                                        */
#define SM_4M_SS50    0x02    /*                                        */
#define SM_4M_SS40    0x03    /*                                        */

/*                       */
/*                       */
/*                       */

#endif /*                      */
