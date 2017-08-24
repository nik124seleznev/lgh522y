#ifndef _METAG_L2CACHE_H
#define _METAG_L2CACHE_H

#ifdef CONFIG_METAG_L2C

#include <asm/global_lock.h>
#include <asm/io.h>

/*
                                                                               
              
 */
extern int l2c_pfenable;

/*                                            */
extern struct sysdev_class cache_sysclass;

static inline void wr_fence(void);

/*
                                                   
 */

/*                                           */
static inline unsigned int meta_l2c_config(void)
{
	const unsigned int *corecfg3 = (const unsigned int *)METAC_CORE_CONFIG3;
	return *corecfg3;
}

/*                               */
static inline int meta_l2c_is_present(void)
{
	return meta_l2c_config() & METAC_CORECFG3_L2C_HAVE_L2C_BIT;
}

/*                                                                          */
static inline int meta_l2c_is_writeback(void)
{
	return meta_l2c_config() & METAC_CORECFG3_L2C_MODE_BIT;
}

/*                                                              */
static inline int meta_l2c_is_unified(void)
{
	return meta_l2c_config() & METAC_CORECFG3_L2C_UNIFIED_BIT;
}

/*                                */
static inline unsigned int meta_l2c_size(void)
{
	unsigned int size_s;
	if (!meta_l2c_is_present())
		return 0;
	size_s = (meta_l2c_config() & METAC_CORECFG3_L2C_SIZE_BITS)
			>> METAC_CORECFG3_L2C_SIZE_S;
	/*                   */
	return 1024 << size_s;
}

/*                                        */
static inline unsigned int meta_l2c_ways(void)
{
	unsigned int ways_s;
	if (!meta_l2c_is_present())
		return 0;
	ways_s = (meta_l2c_config() & METAC_CORECFG3_L2C_NUM_WAYS_BITS)
			>> METAC_CORECFG3_L2C_NUM_WAYS_S;
	return 0x1 << ways_s;
}

/*                                   */
static inline unsigned int meta_l2c_linesize(void)
{
	unsigned int line_size;
	if (!meta_l2c_is_present())
		return 0;
	line_size = (meta_l2c_config() & METAC_CORECFG3_L2C_LINE_SIZE_BITS)
			>> METAC_CORECFG3_L2C_LINE_SIZE_S;
	switch (line_size) {
	case METAC_CORECFG3_L2C_LINE_SIZE_64B:
		return 64;
	default:
		return 0;
	}
}

/*                                     */
static inline unsigned int meta_l2c_revision(void)
{
	return (meta_l2c_config() & METAC_CORECFG3_L2C_REV_ID_BITS)
			>> METAC_CORECFG3_L2C_REV_ID_S;
}


/*
                                                                        
                                                                             
               
 */
static inline void _meta_l2c_init(void)
{
	metag_out32(SYSC_L2C_INIT_INIT, SYSC_L2C_INIT);
	while (metag_in32(SYSC_L2C_INIT) == SYSC_L2C_INIT_IN_PROGRESS)
		/*            */;
}

/*
                                                                    
                                                                 
 */
static inline void _meta_l2c_purge(void)
{
	metag_out32(SYSC_L2C_PURGE_PURGE, SYSC_L2C_PURGE);
	while (metag_in32(SYSC_L2C_PURGE) == SYSC_L2C_PURGE_IN_PROGRESS)
		/*            */;
}

/*                                      */
static inline void _meta_l2c_enable(int enabled)
{
	unsigned int enable;

	enable = metag_in32(SYSC_L2C_ENABLE);
	if (enabled)
		enable |= SYSC_L2C_ENABLE_ENABLE_BIT;
	else
		enable &= ~SYSC_L2C_ENABLE_ENABLE_BIT;
	metag_out32(enable, SYSC_L2C_ENABLE);
}

/*                                               */
static inline void _meta_l2c_pf_enable(int pfenabled)
{
	unsigned int enable;

	enable = metag_in32(SYSC_L2C_ENABLE);
	if (pfenabled)
		enable |= SYSC_L2C_ENABLE_PFENABLE_BIT;
	else
		enable &= ~SYSC_L2C_ENABLE_PFENABLE_BIT;
	metag_out32(enable, SYSC_L2C_ENABLE);
}

/*                                        */
static inline int _meta_l2c_is_enabled(void)
{
	return metag_in32(SYSC_L2C_ENABLE) & SYSC_L2C_ENABLE_ENABLE_BIT;
}

/*                                                 */
static inline int _meta_l2c_pf_is_enabled(void)
{
	return metag_in32(SYSC_L2C_ENABLE) & SYSC_L2C_ENABLE_PFENABLE_BIT;
}


/*                                        */
static inline int meta_l2c_is_enabled(void)
{
	int en;

	/*
                                                                      
                                                                        
  */
	en = _meta_l2c_is_enabled();

	return en;
}

/*
                                   
                                                 
 */
int meta_l2c_disable(void);

/*
                                  
                                                
 */
int meta_l2c_enable(void);

/*                                                 */
static inline int meta_l2c_pf_is_enabled(void)
{
	return l2c_pfenable;
}

/*
                                                
                                                         
 */
int meta_l2c_pf_enable(int pfenable);

/*
                      
                                  
 */
int meta_l2c_flush(void);

/*
                                                    
                                                               
 */
static inline int meta_l2c_writeback(void)
{
	unsigned long flags;
	int en;

	/*                                                */
	if (!meta_l2c_is_writeback())
		return 1;

	/*
                                                                       
                                                                        
  */
	__global_lock1(flags);
	en = meta_l2c_is_enabled();
	if (likely(en)) {
		wr_fence();
		_meta_l2c_purge();
	}
	__global_unlock1(flags);

	return !en;
}

#else /*                  */

#define meta_l2c_config()		0
#define meta_l2c_is_present()		0
#define meta_l2c_is_writeback()		0
#define meta_l2c_is_unified()		0
#define meta_l2c_size()			0
#define meta_l2c_ways()			0
#define meta_l2c_linesize()		0
#define meta_l2c_revision()		0

#define meta_l2c_is_enabled()		0
#define _meta_l2c_pf_is_enabled()	0
#define meta_l2c_pf_is_enabled()	0
#define meta_l2c_disable()		1
#define meta_l2c_enable()		0
#define meta_l2c_pf_enable(X)		0
static inline int meta_l2c_flush(void)
{
	return 1;
}
static inline int meta_l2c_writeback(void)
{
	return 1;
}

#endif /*                  */

#endif /*                  */
