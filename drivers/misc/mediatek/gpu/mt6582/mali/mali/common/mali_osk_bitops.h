/*
 * Copyright (C) 2010, 2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/* 
                          
                                                                          
 */

#ifndef __MALI_OSK_BITOPS_H__
#define __MALI_OSK_BITOPS_H__

#ifdef __cplusplus
extern "C" {
#endif

MALI_STATIC_INLINE void _mali_internal_clear_bit( u32 bit, u32 *addr )
{
	MALI_DEBUG_ASSERT( bit < 32 );
	MALI_DEBUG_ASSERT( NULL != addr );

	(*addr) &= ~(1 << bit);
}

MALI_STATIC_INLINE void _mali_internal_set_bit( u32 bit, u32 *addr )
{
	MALI_DEBUG_ASSERT( bit < 32 );
	MALI_DEBUG_ASSERT( NULL != addr );

	(*addr) |= (1 << bit);
}

MALI_STATIC_INLINE u32 _mali_internal_test_bit( u32 bit, u32 value )
{
	MALI_DEBUG_ASSERT( bit < 32 );
	return value & (1 << bit);
}

MALI_STATIC_INLINE int _mali_internal_find_first_zero_bit( u32 value )
{
	u32 inverted;
	u32 negated;
	u32 isolated;
	u32 leading_zeros;

	/*                                                                             */
	inverted = ~value; /*                 */
	/*                                               
                                                                        */

	/*                                                                  */
	negated = (u32)-inverted ; /*                                          */
	/*                           */

	isolated = negated & inverted ; /*                                               */
	/*                                                                                             
                                                     */

	leading_zeros = _mali_osk_clz( isolated );

	return 31 - leading_zeros;
}


/*                                                          
      */

/* 
                                                                            
                         
  
                                                                             
                                                                           
                                                                 
  
                                                                          
               
 */

/*                                                  
                                                                         
                  
                                           
 */
MALI_STATIC_INLINE void _mali_osk_clear_nonatomic_bit( u32 nr, u32 *addr )
{
	addr += nr >> 5; /*                       */
	nr = nr & ((1 << 5)-1); /*                                */

	_mali_internal_clear_bit( nr, addr );
}

/*                                                
                                                                       
                  
                                           
 */
MALI_STATIC_INLINE void _mali_osk_set_nonatomic_bit( u32 nr, u32 *addr )
{
	addr += nr >> 5; /*                       */
	nr = nr & ((1 << 5)-1); /*                                */

	_mali_internal_set_bit( nr, addr );
}

/*                                                 
                                                                        
                  
                                           
                                                                            
                                                     
 */
MALI_STATIC_INLINE u32 _mali_osk_test_bit( u32 nr, u32 *addr )
{
	addr += nr >> 5; /*                       */
	nr = nr & ((1 << 5)-1); /*                                */

	return _mali_internal_test_bit( nr, *addr );
}

/*                            */
/*                                                              
                                         
                                                     
                                                                               
                          
 */
MALI_STATIC_INLINE u32 _mali_osk_find_first_zero_bit( const u32 *addr, u32 maxbit )
{
	u32 total;

	for ( total = 0; total < maxbit; total += 32, ++addr ) {
		int result;
		result = _mali_internal_find_first_zero_bit( *addr );

		/*                                          */
		if ( result >= 0 ) {
			total += (u32)result;
			break;
		}
	}

	/*                                         */
	if ( total >= maxbit ) {
		total = maxbit;
	}

	return total; /*                                                 */
}
/*     */ /*                            */

#ifdef __cplusplus
}
#endif

#endif /*                       */
