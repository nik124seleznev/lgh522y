/*
 *
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */





#include <ump/ump.h>
#include <memory.h>
#include <stdio.h>

/*
                                                      
                                                                               
                                                                                    
                                                                                        
                                                                                       
  
                                                                                      
                                         
 */

static int test_ump_user_api(void)
{
	/*                                    */
	const size_t alloc_size = 4096;

	ump_handle h = UMP_INVALID_MEMORY_HANDLE;
	ump_handle h_copy = UMP_INVALID_MEMORY_HANDLE;
	ump_handle h_clone = UMP_INVALID_MEMORY_HANDLE;

	void * mapping = NULL;

	ump_result ump_api_res;
	int result = -1;

	ump_secure_id id;

	size_t size_returned;

	ump_api_res = ump_open();
	if (UMP_OK != ump_api_res)
	{
		/*                               */
		/*           */
		return -1;
	}

	h = ump_allocate_64(alloc_size, UMP_PROT_CPU_RD | UMP_PROT_CPU_WR | UMP_PROT_X_RD | UMP_PROT_X_WR);
	/*                       */
	if (UMP_INVALID_MEMORY_HANDLE == h)
	{
		/*                    */
		goto cleanup;
	}

	/*                                                                 */

	/*               */
	id = ump_secure_id_get(h);
	/*                   */
	/*                          */

	/*               */
	/*                       */
	h_clone = ump_from_secure_id(id);
	/*                                                                       */
	/*                  */
	/*                   */
	ump_release(h_clone); /*                                                */
	h_clone = UMP_INVALID_MEMORY_HANDLE;


	/*                                                                                                               */
	/*
                                
     
   */
	h_copy = h;
	ump_retain(h_copy); /*                                                            */
	/*
     
   
                            
     
  */
	 ump_release(h_copy); /*                                                        */
	 h_copy = UMP_INVALID_MEMORY_HANDLE;
	 /*
     
  */
	
	/*                                                             */
	/*                                                                          */

	mapping = ump_map(h, 0, alloc_size);
	if (NULL == mapping)
	{
		/*                                                                  */
		goto cleanup;
	}

	memset(mapping, 0, alloc_size);

	/*                                                                                                 */
	ump_cpu_msync_now(h, UMP_MSYNC_CLEAN, mapping, alloc_size);
 	/*
                         

                

              

                

                 

                

                    

                
 */
	ump_cpu_msync_now(h, UMP_MSYNC_CLEAN_AND_INVALIDATE, mapping, alloc_size);

	/*                                                                                                    */

	/*                                          */
	ump_unmap(h, mapping, alloc_size);

	result = 0;

cleanup:
	ump_release(h);
	h = UMP_INVALID_MEMORY_HANDLE;

	ump_close();

	return result;
}

