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
ACPI_MODULE_NAME("utmisc")

/*                                                                              
  
                                          
  
                                                              
  
                                                                           
  
                                                                  
  
                                                                              */
u8 acpi_ut_is_pci_root_bridge(char *id)
{

	/*
                                       
                                                 
  */
	if (!(ACPI_STRCMP(id,
			  PCI_ROOT_HID_STRING)) ||
	    !(ACPI_STRCMP(id, PCI_EXPRESS_ROOT_HID_STRING))) {
		return (TRUE);
	}

	return (FALSE);
}

/*                                                                              
  
                                    
  
                                                   
  
                                                                      
  
                                                                        
                                                                              
                                                         
  
                                                                              */

u8 acpi_ut_is_aml_table(struct acpi_table_header *table)
{

	/*                                                       */

	if (ACPI_COMPARE_NAME(table->signature, ACPI_SIG_DSDT) ||
	    ACPI_COMPARE_NAME(table->signature, ACPI_SIG_PSDT) ||
	    ACPI_COMPARE_NAME(table->signature, ACPI_SIG_SSDT)) {
		return (TRUE);
	}

	return (FALSE);
}

/*                                                                              
  
                                       
  
                                                       
  
                                              
  
                                                                     
  
                                                                              */

u32 acpi_ut_dword_byte_swap(u32 value)
{
	union {
		u32 value;
		u8 bytes[4];
	} out;
	union {
		u32 value;
		u8 bytes[4];
	} in;

	ACPI_FUNCTION_ENTRY();

	in.value = value;

	out.bytes[0] = in.bytes[3];
	out.bytes[1] = in.bytes[2];
	out.bytes[2] = in.bytes[1];
	out.bytes[3] = in.bytes[0];

	return (out.value);
}

/*                                                                              
  
                                         
  
                                                    
  
                    
  
                                                                        
                                                                        
                                                                         
                                   
  
                                                                              */

void acpi_ut_set_integer_width(u8 revision)
{

	if (revision < 2) {

		/*             */

		acpi_gbl_integer_bit_width = 32;
		acpi_gbl_integer_nybble_width = 8;
		acpi_gbl_integer_byte_width = 4;
	} else {
		/*                         */

		acpi_gbl_integer_bit_width = 64;
		acpi_gbl_integer_nybble_width = 16;
		acpi_gbl_integer_byte_width = 8;
	}
}

/*                                                                              
  
                                                    
  
                                                                     
                                                     
                                                                 
  
                      
  
                                              
  
                                                                              */

acpi_status
acpi_ut_create_update_state_and_push(union acpi_operand_object *object,
				     u16 action,
				     union acpi_generic_state **state_list)
{
	union acpi_generic_state *state;

	ACPI_FUNCTION_ENTRY();

	/*                                         */

	if (!object) {
		return (AE_OK);
	}

	state = acpi_ut_create_update_state(object, action);
	if (!state) {
		return (AE_NO_MEMORY);
	}

	acpi_ut_push_generic_state(state_list, state);
	return (AE_OK);
}

/*                                                                              
  
                                         
  
                                                         
                                                                                
                                                                          
                                                                     
  
                      
  
                                      
  
                                                                              */

acpi_status
acpi_ut_walk_package_tree(union acpi_operand_object *source_object,
			  void *target_object,
			  acpi_pkg_callback walk_callback, void *context)
{
	acpi_status status = AE_OK;
	union acpi_generic_state *state_list = NULL;
	union acpi_generic_state *state;
	u32 this_index;
	union acpi_operand_object *this_source_obj;

	ACPI_FUNCTION_TRACE(ut_walk_package_tree);

	state = acpi_ut_create_pkg_state(source_object, target_object, 0);
	if (!state) {
		return_ACPI_STATUS(AE_NO_MEMORY);
	}

	while (state) {

		/*                                */

		this_index = state->pkg.index;
		this_source_obj = (union acpi_operand_object *)
		    state->pkg.source_object->package.elements[this_index];

		/*
               
                                                          
                                                             
                                                                
                                                                   
                   
   */
		if ((!this_source_obj) ||
		    (ACPI_GET_DESCRIPTOR_TYPE(this_source_obj) !=
		     ACPI_DESC_TYPE_OPERAND)
		    || (this_source_obj->common.type != ACPI_TYPE_PACKAGE)) {
			status =
			    walk_callback(ACPI_COPY_TYPE_SIMPLE,
					  this_source_obj, state, context);
			if (ACPI_FAILURE(status)) {
				return_ACPI_STATUS(status);
			}

			state->pkg.index++;
			while (state->pkg.index >=
			       state->pkg.source_object->package.count) {
				/*
                                                                  
                                                              
                                                  
      
                                                              
     */
				acpi_ut_delete_generic_state(state);
				state = acpi_ut_pop_generic_state(&state_list);

				/*                                        */

				if (!state) {
					/*
                                                           
                                                          
                
      */
					return_ACPI_STATUS(AE_OK);
				}

				/*
                                                          
                                
     */
				state->pkg.index++;
			}
		} else {
			/*                                     */

			status =
			    walk_callback(ACPI_COPY_TYPE_PACKAGE,
					  this_source_obj, state, context);
			if (ACPI_FAILURE(status)) {
				return_ACPI_STATUS(status);
			}

			/*
                                                 
                                                              
    */
			acpi_ut_push_generic_state(&state_list, state);
			state = acpi_ut_create_pkg_state(this_source_obj,
							 state->pkg.
							 this_target_obj, 0);
			if (!state) {

				/*                                       */

				while (state_list) {
					state =
					    acpi_ut_pop_generic_state
					    (&state_list);
					acpi_ut_delete_generic_state(state);
				}
				return_ACPI_STATUS(AE_NO_MEMORY);
			}
		}
	}

	/*                          */

	return_ACPI_STATUS(AE_AML_INTERNAL);
}

#ifdef ACPI_DEBUG_OUTPUT
/*                                                                              
  
                                             
  
                                                             
                                                                             
                                                                            
                                                               
  
                           
  
                                                              
  
                                                                              */

void
acpi_ut_display_init_pathname(u8 type,
			      struct acpi_namespace_node *obj_handle,
			      char *path)
{
	acpi_status status;
	struct acpi_buffer buffer;

	ACPI_FUNCTION_ENTRY();

	/*                                                               */

	if (!(acpi_dbg_level & ACPI_LV_INIT_NAMES)) {
		return;
	}

	/*                                   */

	buffer.length = ACPI_ALLOCATE_LOCAL_BUFFER;
	status = acpi_ns_handle_to_pathname(obj_handle, &buffer);
	if (ACPI_FAILURE(status)) {
		return;
	}

	/*                        */

	switch (type) {
	case ACPI_TYPE_METHOD:
		acpi_os_printf("Executing  ");
		break;

	default:
		acpi_os_printf("Initializing ");
		break;
	}

	/*                                    */

	acpi_os_printf("%-12s %s",
		       acpi_ut_get_type_name(type), (char *)buffer.pointer);

	/*                                                          */

	if (path) {
		acpi_os_printf(".%s", path);
	}
	acpi_os_printf("\n");

	ACPI_FREE(buffer.pointer);
}
#endif
