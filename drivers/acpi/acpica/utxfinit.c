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

#include <linux/export.h>
#include <acpi/acpi.h>
#include "accommon.h"
#include "acevents.h"
#include "acnamesp.h"
#include "acdebug.h"
#include "actables.h"

#define _COMPONENT          ACPI_UTILITIES
ACPI_MODULE_NAME("utxfinit")

/*                                                                              
  
                                         
  
                    
  
                      
  
                                                                            
                                                                 
  
                                                                              */
acpi_status acpi_initialize_subsystem(void)
{
	acpi_status status;

	ACPI_FUNCTION_TRACE(acpi_initialize_subsystem);

	acpi_gbl_startup_flags = ACPI_SUBSYSTEM_INITIALIZE;
	ACPI_DEBUG_EXEC(acpi_ut_init_stack_ptr_trace());

	/*                                   */

	status = acpi_os_initialize();
	if (ACPI_FAILURE(status)) {
		ACPI_EXCEPTION((AE_INFO, status, "During OSL initialization"));
		return_ACPI_STATUS(status);
	}

	/*                                              */

	status = acpi_ut_init_globals();
	if (ACPI_FAILURE(status)) {
		ACPI_EXCEPTION((AE_INFO, status,
				"During initialization of globals"));
		return_ACPI_STATUS(status);
	}

	/*                                  */

	status = acpi_ut_mutex_initialize();
	if (ACPI_FAILURE(status)) {
		ACPI_EXCEPTION((AE_INFO, status,
				"During Global Mutex creation"));
		return_ACPI_STATUS(status);
	}

	/*
                                        
                                  
  */
	status = acpi_ns_root_initialize();
	if (ACPI_FAILURE(status)) {
		ACPI_EXCEPTION((AE_INFO, status,
				"During Namespace initialization"));
		return_ACPI_STATUS(status);
	}

	/*                                                                 */

	status = acpi_ut_initialize_interfaces();
	if (ACPI_FAILURE(status)) {
		ACPI_EXCEPTION((AE_INFO, status,
				"During OSI interfaces initialization"));
		return_ACPI_STATUS(status);
	}

	/*                                            */

	ACPI_DEBUGGER_EXEC(status = acpi_db_initialize());
	return_ACPI_STATUS(status);
}
ACPI_EXPORT_SYMBOL(acpi_initialize_subsystem)

/*                                                                              
  
                                     
  
                                                         
  
                      
  
                                                                          
                                                               
  
                                                                              */
acpi_status acpi_enable_subsystem(u32 flags)
{
	acpi_status status = AE_OK;

	ACPI_FUNCTION_TRACE(acpi_enable_subsystem);

#if (!ACPI_REDUCED_HARDWARE)

	/*                  */

	if (!(flags & ACPI_NO_ACPI_ENABLE)) {
		ACPI_DEBUG_PRINT((ACPI_DB_EXEC,
				  "[Init] Going into ACPI mode\n"));

		acpi_gbl_original_mode = acpi_hw_get_mode();

		status = acpi_enable();
		if (ACPI_FAILURE(status)) {
			ACPI_WARNING((AE_INFO, "AcpiEnable failed"));
			return_ACPI_STATUS(status);
		}
	}

	/*
                                                                     
                                              
  */
	status = acpi_tb_initialize_facs();
	if (ACPI_FAILURE(status)) {
		ACPI_WARNING((AE_INFO, "Could not map the FACS table"));
		return_ACPI_STATUS(status);
	}
#endif				/*                        */

	/*
                                                                      
                                                      
                                            
  */
	if (!(flags & ACPI_NO_ADDRESS_SPACE_INIT)) {
		ACPI_DEBUG_PRINT((ACPI_DB_EXEC,
				  "[Init] Installing default address space handlers\n"));

		status = acpi_ev_install_region_handlers();
		if (ACPI_FAILURE(status)) {
			return_ACPI_STATUS(status);
		}
	}
#if (!ACPI_REDUCED_HARDWARE)
	/*
                                                              
   
                                                                         
                                                                      
                                                                           
                         
   
                                                                  
                                                                  
                                                                          
                                                                    
                     
  */
	if (!(flags & ACPI_NO_EVENT_INIT)) {
		ACPI_DEBUG_PRINT((ACPI_DB_EXEC,
				  "[Init] Initializing ACPI events\n"));

		status = acpi_ev_initialize_events();
		if (ACPI_FAILURE(status)) {
			return_ACPI_STATUS(status);
		}
	}

	/*
                                                                       
                            
  */
	if (!(flags & ACPI_NO_HANDLER_INIT)) {
		ACPI_DEBUG_PRINT((ACPI_DB_EXEC,
				  "[Init] Installing SCI/GL handlers\n"));

		status = acpi_ev_install_xrupt_handlers();
		if (ACPI_FAILURE(status)) {
			return_ACPI_STATUS(status);
		}
	}
#endif				/*                        */

	return_ACPI_STATUS(status);
}
ACPI_EXPORT_SYMBOL(acpi_enable_subsystem)

/*                                                                              
  
                                       
  
                                                         
  
                      
  
                                                                         
                                                                         
  
                                                                              */
acpi_status acpi_initialize_objects(u32 flags)
{
	acpi_status status = AE_OK;

	ACPI_FUNCTION_TRACE(acpi_initialize_objects);

	/*
                        
   
                                                                        
                                                                     
                                      
  */
	if (!(flags & ACPI_NO_ADDRESS_SPACE_INIT)) {
		ACPI_DEBUG_PRINT((ACPI_DB_EXEC,
				  "[Init] Executing _REG OpRegion methods\n"));

		status = acpi_ev_initialize_op_regions();
		if (ACPI_FAILURE(status)) {
			return_ACPI_STATUS(status);
		}
	}

	/*
                                                                         
                                                                        
                                                                         
                                                                     
                                                                       
                       
  */
	acpi_ns_exec_module_code_list();

	/*
                                                                   
                                                                        
                                                            
  */
	if (!(flags & ACPI_NO_OBJECT_INIT)) {
		ACPI_DEBUG_PRINT((ACPI_DB_EXEC,
				  "[Init] Completing Initialization of ACPI Objects\n"));

		status = acpi_ns_initialize_objects();
		if (ACPI_FAILURE(status)) {
			return_ACPI_STATUS(status);
		}
	}

	/*
                                                                        
                          
  */
	if (!(flags & ACPI_NO_DEVICE_INIT)) {
		ACPI_DEBUG_PRINT((ACPI_DB_EXEC,
				  "[Init] Initializing ACPI Devices\n"));

		status = acpi_ns_initialize_devices();
		if (ACPI_FAILURE(status)) {
			return_ACPI_STATUS(status);
		}
	}

	/*
                                                                       
                                                                      
                                  
  */
	status = acpi_purge_cached_objects();

	acpi_gbl_startup_flags |= ACPI_INITIALIZED_OK;
	return_ACPI_STATUS(status);
}
ACPI_EXPORT_SYMBOL(acpi_initialize_objects)
