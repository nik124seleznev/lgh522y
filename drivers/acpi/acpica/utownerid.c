/*                                                                              
  
                                                              
  
                                                                              */

/*
 * Copyright (C) 2000 - 2013, Intel Corp.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce at minimum a disclaimer
 *    substantially similar to the "NO WARRANTY" disclaimer below
 *    ("Disclaimer") and any redistribution must be conditioned upon
 *    including a substantially similar Disclaimer requirement for further
 *    binary redistribution.
 * 3. Neither the names of the above-listed copyright holders nor the names
 *    of any contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * NO WARRANTY
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 */

#include <acpi/acpi.h>
#include "accommon.h"
#include "acnamesp.h"

#define _COMPONENT          ACPI_UTILITIES
ACPI_MODULE_NAME("utownerid")

/*                                                                              
  
                                         
  
                                                                    
  
                      
  
                                                                            
                                                                           
                                                               
  
                                                                              */
acpi_status acpi_ut_allocate_owner_id(acpi_owner_id * owner_id)
{
	u32 i;
	u32 j;
	u32 k;
	acpi_status status;

	ACPI_FUNCTION_TRACE(ut_allocate_owner_id);

	/*                                                               */

	if (*owner_id) {
		ACPI_ERROR((AE_INFO, "Owner ID [0x%2.2X] already exists",
			    *owner_id));
		return_ACPI_STATUS(AE_ALREADY_EXISTS);
	}

	/*                              */

	status = acpi_ut_acquire_mutex(ACPI_MTX_CACHES);
	if (ACPI_FAILURE(status)) {
		return_ACPI_STATUS(status);
	}

	/*
                                                                    
                                                                          
                        
  */
	for (i = 0, j = acpi_gbl_last_owner_id_index;
	     i < (ACPI_NUM_OWNERID_MASKS + 1); i++, j++) {
		if (j >= ACPI_NUM_OWNERID_MASKS) {
			j = 0;	/*                                   */
		}

		for (k = acpi_gbl_next_owner_id_offset; k < 32; k++) {
			if (acpi_gbl_owner_id_mask[j] == ACPI_UINT32_MAX) {

				/*                                    */

				break;
			}

			if (!(acpi_gbl_owner_id_mask[j] & (1 << k))) {
				/*
                                                                
                                                                
                                               
     */
				acpi_gbl_owner_id_mask[j] |= (1 << k);

				acpi_gbl_last_owner_id_index = (u8)j;
				acpi_gbl_next_owner_id_offset = (u8)(k + 1);

				/*
                                                           
      
                                                             
                                                   
     */
				*owner_id =
				    (acpi_owner_id) ((k + 1) + ACPI_MUL_32(j));

				ACPI_DEBUG_PRINT((ACPI_DB_VALUES,
						  "Allocated OwnerId: %2.2X\n",
						  (unsigned int)*owner_id));
				goto exit;
			}
		}

		acpi_gbl_next_owner_id_offset = 0;
	}

	/*
                                                            
                                                                       
                                                                       
                                                                    
              
   
                                                                            
                                                                  
  */
	status = AE_OWNER_ID_LIMIT;
	ACPI_ERROR((AE_INFO,
		    "Could not allocate new OwnerId (255 max), AE_OWNER_ID_LIMIT"));

      exit:
	(void)acpi_ut_release_mutex(ACPI_MTX_CACHES);
	return_ACPI_STATUS(status);
}

/*                                                                              
  
                                        
  
                                                                                
  
                                                                          
                                                                         
                                        
  
                                                                         
  
                                                                              */

void acpi_ut_release_owner_id(acpi_owner_id * owner_id_ptr)
{
	acpi_owner_id owner_id = *owner_id_ptr;
	acpi_status status;
	u32 index;
	u32 bit;

	ACPI_FUNCTION_TRACE_U32(ut_release_owner_id, owner_id);

	/*                                                         */

	*owner_id_ptr = 0;

	/*                              */

	if (owner_id == 0) {
		ACPI_ERROR((AE_INFO, "Invalid OwnerId: 0x%2.2X", owner_id));
		return_VOID;
	}

	/*                              */

	status = acpi_ut_acquire_mutex(ACPI_MTX_CACHES);
	if (ACPI_FAILURE(status)) {
		return_VOID;
	}

	/*                          */

	owner_id--;

	/*                                */

	index = ACPI_DIV_32(owner_id);
	bit = 1 << ACPI_MOD_32(owner_id);

	/*                                       */

	if (acpi_gbl_owner_id_mask[index] & bit) {
		acpi_gbl_owner_id_mask[index] ^= bit;
	} else {
		ACPI_ERROR((AE_INFO,
			    "Release of non-allocated OwnerId: 0x%2.2X",
			    owner_id + 1));
	}

	(void)acpi_ut_release_mutex(ACPI_MTX_CACHES);
	return_VOID;
}
