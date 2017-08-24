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
#include "acinterp.h"
#include "acpredef.h"
#include "amlresrc.h"

#define _COMPONENT          ACPI_NAMESPACE
ACPI_MODULE_NAME("nsconvert")

/*                                                                              
  
                                          
  
                                                            
                                                                                
  
                                                           
  
                                                                        
  
                                                                              */
acpi_status
acpi_ns_convert_to_integer(union acpi_operand_object *original_object,
			   union acpi_operand_object **return_object)
{
	union acpi_operand_object *new_object;
	acpi_status status;
	u64 value = 0;
	u32 i;

	switch (original_object->common.type) {
	case ACPI_TYPE_STRING:

		/*                              */

		status = acpi_ut_strtoul64(original_object->string.pointer,
					   ACPI_ANY_BASE, &value);
		if (ACPI_FAILURE(status)) {
			return (status);
		}
		break;

	case ACPI_TYPE_BUFFER:

		/*                                                           */

		if (original_object->buffer.length > 8) {
			return (AE_AML_OPERAND_TYPE);
		}

		/*                                                */

		for (i = 0; i < original_object->buffer.length; i++) {
			value |=
			    ((u64)original_object->buffer.
			     pointer[i] << (i * 8));
		}
		break;

	default:
		return (AE_AML_OPERAND_TYPE);
	}

	new_object = acpi_ut_create_integer_object(value);
	if (!new_object) {
		return (AE_NO_MEMORY);
	}

	*return_object = new_object;
	return (AE_OK);
}

/*                                                                              
  
                                         
  
                                                            
                                                                                
  
                                                           
  
                                                                       
  
                                                                              */

acpi_status
acpi_ns_convert_to_string(union acpi_operand_object *original_object,
			  union acpi_operand_object **return_object)
{
	union acpi_operand_object *new_object;
	acpi_size length;
	acpi_status status;

	switch (original_object->common.type) {
	case ACPI_TYPE_INTEGER:
		/*
                                                    
                                                                
                                                       
   */
		if (original_object->integer.value == 0) {

			/*                                   */

			new_object = acpi_ut_create_string_object(0);
			if (!new_object) {
				return (AE_NO_MEMORY);
			}
		} else {
			status =
			    acpi_ex_convert_to_string(original_object,
						      &new_object,
						      ACPI_IMPLICIT_CONVERT_HEX);
			if (ACPI_FAILURE(status)) {
				return (status);
			}
		}
		break;

	case ACPI_TYPE_BUFFER:
		/*
                                                 
                                                                    
                                                                   
                                                                     
   */
		length = 0;
		while ((length < original_object->buffer.length) &&
		       (original_object->buffer.pointer[length])) {
			length++;
		}

		/*                              */

		new_object = acpi_ut_create_string_object(length);
		if (!new_object) {
			return (AE_NO_MEMORY);
		}

		/*
                                                                       
                            
   */
		ACPI_MEMCPY(new_object->string.pointer,
			    original_object->buffer.pointer, length);
		break;

	default:
		return (AE_AML_OPERAND_TYPE);
	}

	*return_object = new_object;
	return (AE_OK);
}

/*                                                                              
  
                                         
  
                                                            
                                                                                
  
                                                           
  
                                                                               
  
                                                                              */

acpi_status
acpi_ns_convert_to_buffer(union acpi_operand_object *original_object,
			  union acpi_operand_object **return_object)
{
	union acpi_operand_object *new_object;
	acpi_status status;
	union acpi_operand_object **elements;
	u32 *dword_buffer;
	u32 count;
	u32 i;

	switch (original_object->common.type) {
	case ACPI_TYPE_INTEGER:
		/*
                                  
                                                                
                                                                   
                                                                  
                          
   */
		status =
		    acpi_ex_convert_to_buffer(original_object, &new_object);
		if (ACPI_FAILURE(status)) {
			return (status);
		}
		break;

	case ACPI_TYPE_STRING:

		/*                                               */

		new_object =
		    acpi_ut_create_buffer_object(original_object->string.
						 length);
		if (!new_object) {
			return (AE_NO_MEMORY);
		}

		ACPI_MEMCPY(new_object->buffer.pointer,
			    original_object->string.pointer,
			    original_object->string.length);
		break;

	case ACPI_TYPE_PACKAGE:
		/*
                                                                    
                                                                    
                                                             
   */

		/*                                              */

		elements = original_object->package.elements;
		count = original_object->package.count;

		for (i = 0; i < count; i++) {
			if ((!*elements) ||
			    ((*elements)->common.type != ACPI_TYPE_INTEGER)) {
				return (AE_AML_OPERAND_TYPE);
			}
			elements++;
		}

		/*                                                     */

		new_object = acpi_ut_create_buffer_object(ACPI_MUL_4(count));
		if (!new_object) {
			return (AE_NO_MEMORY);
		}

		/*                                                              */

		elements = original_object->package.elements;
		dword_buffer = ACPI_CAST_PTR(u32, new_object->buffer.pointer);

		for (i = 0; i < count; i++) {
			*dword_buffer = (u32)(*elements)->integer.value;
			dword_buffer++;
			elements++;
		}
		break;

	default:
		return (AE_AML_OPERAND_TYPE);
	}

	*return_object = new_object;
	return (AE_OK);
}

/*                                                                              
  
                                          
  
                                                                         
                                                                                
  
                                                           
  
                                                                              
  
                                                                              */

acpi_status
acpi_ns_convert_to_unicode(union acpi_operand_object *original_object,
			   union acpi_operand_object **return_object)
{
	union acpi_operand_object *new_object;
	char *ascii_string;
	u16 *unicode_buffer;
	u32 unicode_length;
	u32 i;

	if (!original_object) {
		return (AE_OK);
	}

	/*                                                              */

	if (original_object->common.type == ACPI_TYPE_BUFFER) {
		if (original_object->buffer.length < 2) {
			return (AE_AML_OPERAND_VALUE);
		}

		*return_object = NULL;
		return (AE_OK);
	}

	/*
                                                                  
                     
  */
	ascii_string = original_object->string.pointer;
	unicode_length = (original_object->string.length * 2) + 2;

	/*                                                 */

	new_object = acpi_ut_create_buffer_object(unicode_length);
	if (!new_object) {
		return (AE_NO_MEMORY);
	}

	unicode_buffer = ACPI_CAST_PTR(u16, new_object->buffer.pointer);

	/*                          */

	for (i = 0; i < original_object->string.length; i++) {
		unicode_buffer[i] = (u16)ascii_string[i];
	}

	*return_object = new_object;
	return (AE_OK);
}

/*                                                                              
  
                                           
  
                                                            
                                                                                
  
                                                          
  
                                                                          
                       
  
                                                                              */

acpi_status
acpi_ns_convert_to_resource(union acpi_operand_object *original_object,
			    union acpi_operand_object **return_object)
{
	union acpi_operand_object *new_object;
	u8 *buffer;

	/*
                                                                     
                                                           
                                  
                                        
   
                                                       
                        
  */
	if (original_object) {
		switch (original_object->common.type) {
		case ACPI_TYPE_INTEGER:

			/*                                  */

			if (original_object->integer.value) {
				return (AE_AML_OPERAND_TYPE);
			}
			break;

		case ACPI_TYPE_BUFFER:

			if (original_object->buffer.length) {

				/*                                              */

				*return_object = NULL;
				return (AE_OK);
			}
			break;

		case ACPI_TYPE_STRING:
		default:

			return (AE_AML_OPERAND_TYPE);
		}
	}

	/*                                                          */

	new_object = acpi_ut_create_buffer_object(2);
	if (!new_object) {
		return (AE_NO_MEMORY);
	}

	buffer = ACPI_CAST_PTR(u8, new_object->buffer.pointer);

	/*                                                        */

	buffer[0] = (ACPI_RESOURCE_NAME_END_TAG | ASL_RDESC_END_TAG_SIZE);
	buffer[1] = 0x00;

	*return_object = new_object;
	return (AE_OK);
}
