/************************************************************************
 * HP-UX Realport Daemon interface file.
 *
 * Copyright (C) 1998, by Digi International.  All Rights Reserved.
 ************************************************************************/

#ifndef _DIGIDRP_H
#define _DIGIDRP_H

/*                                                                       
                                                          
                                                               
                                                              
                                                                        */

struct link_struct {
	int lk_fast_rate;  /*                          
                                     
                          */

	int lk_fast_delay; /*                        
                      */

	int lk_slow_rate;  /*                               
                                   
                          */

	int lk_slow_delay; /*                        
                      */

	int lk_header_size; /*                             
                                      
                 */
};

#define DIGI_GETLINK	_IOW('e', 103, struct link_struct)	/*                     */
#define DIGI_SETLINK	_IOW('e', 104, struct link_struct)	/*                     */


/*                                                                       
                                                          
                                                                     
                                                                        */

struct	digiflow_struct {
	unsigned char	startc;				/*                      */
	unsigned char	stopc;				/*                     */
};

/*                                                                       
                        
                                                                        */
#define DIGI_IXON	0x0001		/*                        */
#define DIGI_FAST	0x0002		/*                  */
#define RTSPACE		0x0004		/*                        */
#define CTSPACE		0x0008		/*                         */
#define DSRPACE		0x0010		/*                         */
#define DCDPACE		0x0020		/*                         */
#define DTRPACE		0x0040		/*                        */
#define DIGI_COOK	0x0080		/*                               */
#define DIGI_FORCEDCD	0x0100		/*                */
#define	DIGI_ALTPIN	0x0200		/*                            */
#define	DIGI_AIXON	0x0400		/*                         */
#define	DIGI_PRINTER	0x0800		/*                               */
#define DIGI_PP_INPUT	0x1000		/*                               */
#define DIGI_422	0x4000		/*                               */
#define DIGI_RTS_TOGGLE	0x8000		/*                      */


/*                                                                       
                                           
                                                                        */
#define DIGI_PLEN	8		/*               */
#define	DIGI_TSIZ	10		/*                     */


/*                                                                       
                                                          
                                                                        */
struct digi_struct {
	unsigned short	digi_flags;		/*                   */
	unsigned short	digi_maxcps;		/*                 */
	unsigned short	digi_maxchar;		/*                          */
	unsigned short	digi_bufsize;		/*              */
	unsigned char	digi_onlen;		/*                     */
	unsigned char	digi_offlen;		/*                      */
	char		digi_onstr[DIGI_PLEN];	/*                   */
	char		digi_offstr[DIGI_PLEN];	/*                    */
	char		digi_term[DIGI_TSIZ];	/*                 */
};

/*                                                                       
                                               
                                                                        */
/*             */
#define DIGI_GETA	_IOR('e', 94, struct digi_struct)

/*            */
#define DIGI_SETA	_IOW('e', 95, struct digi_struct)

/*                    */
#define DIGI_SETAW	_IOW('e', 96, struct digi_struct)

/*                           */
#define DIGI_SETAF	_IOW('e', 97, struct digi_struct)

/*                                          */
#define	DIGI_GETFLOW	_IOR('e', 99, struct digiflow_struct)

/*                                          */
#define	DIGI_SETFLOW	_IOW('e', 100, struct digiflow_struct)

/*                                          */
#define	DIGI_GETAFLOW	_IOR('e', 101, struct digiflow_struct)

/*                                          */
#define	DIGI_SETAFLOW	_IOW('e', 102, struct digiflow_struct)

/*                       */
#define	DIGI_SETCUSTOMBAUD	_IOW('e', 106, int)

/*                       */
#define	DIGI_GETCUSTOMBAUD	_IOR('e', 107, int)

#define	DIGI_GEDELAY	_IOR('d', 246, int)	/*            */
#define	DIGI_SEDELAY	_IOW('d', 247, int)	/*            */


#endif /*            */
