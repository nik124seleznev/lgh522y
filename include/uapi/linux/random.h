/*
                         
  
                                                
 */

#ifndef _UAPI_LINUX_RANDOM_H
#define _UAPI_LINUX_RANDOM_H

#include <linux/types.h>
#include <linux/ioctl.h>
#include <linux/irqnr.h>

/*                                           */

/*                        */
#define RNDGETENTCNT	_IOR( 'R', 0x00, int )

/*                                                                 */
#define RNDADDTOENTCNT	_IOW( 'R', 0x01, int )

/*                                                          */
#define RNDGETPOOL	_IOR( 'R', 0x02, int [2] )

/* 
                                                                  
                    
 */
#define RNDADDENTROPY	_IOW( 'R', 0x03, int [2] )

/*                                              */
#define RNDZAPENTCNT	_IO( 'R', 0x04 )

/*                                                                    */
#define RNDCLEARPOOL	_IO( 'R', 0x06 )

struct rand_pool_info {
	int	entropy_count;
	int	buf_size;
	__u32	buf[0];
};

struct rnd_state {
	__u32 s1, s2, s3;
};

/*                    */


#endif /*                      */
