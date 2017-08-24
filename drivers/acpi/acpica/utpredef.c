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
#include "acpredef.h"

#define _COMPONENT          ACPI_UTILITIES
ACPI_MODULE_NAME("utpredef")

/*
                                                                      
                                                                          
 */
static const char *ut_rtype_names[] = {
	"/Integer",
	"/String",
	"/Buffer",
	"/Package",
	"/Reference",
};

/*                                                                              
  
                                                  
  
                                                                               
  
                                                          
  
                                                                             
                                                                           
                                  
  
                                                                              */

const union acpi_predefined_info *acpi_ut_get_next_predefined_method(const union
								     acpi_predefined_info
								     *this_name)
{

	/*
                                                               
                                          
  */
	if ((this_name->info.expected_btypes & ACPI_RTYPE_PACKAGE) &&
	    (this_name->info.expected_btypes != ACPI_RTYPE_ALL)) {
		this_name++;
	}

	this_name++;
	return (this_name);
}

/*                                                                              
  
                                               
  
                                                  
  
                                                                               
  
                                                                        
  
                                                                              */

const union acpi_predefined_info *acpi_ut_match_predefined_method(char *name)
{
	const union acpi_predefined_info *this_name;

	/*                                                                       */

	if (name[0] != '_') {
		return (NULL);
	}

	/*                                                       */

	this_name = acpi_gbl_predefined_methods;
	while (this_name->info.name[0]) {
		if (ACPI_COMPARE_NAME(name, this_name->info.name)) {
			return (this_name);
		}

		this_name = acpi_ut_get_next_predefined_method(this_name);
	}

	return (NULL);		/*           */
}

/*                                                                              
  
                                                 
  
                                                                            
                                                                     
  
                                           
  
                                                                         
  
                                                                              */

void acpi_ut_get_expected_return_types(char *buffer, u32 expected_btypes)
{
	u32 this_rtype;
	u32 i;
	u32 j;

	j = 1;
	buffer[0] = 0;
	this_rtype = ACPI_RTYPE_INTEGER;

	for (i = 0; i < ACPI_NUM_RTYPES; i++) {

		/*                                                                 */

		if (expected_btypes & this_rtype) {
			ACPI_STRCAT(buffer, &ut_rtype_names[i][j]);
			j = 0;	/*                                */
		}

		this_rtype <<= 1;	/*            */
	}
}

/*                                                                              
  
                                                              
  
                                                                              */

#if (defined ACPI_ASL_COMPILER || defined ACPI_HELP_APP)
#include <stdio.h>
#include <string.h>

/*                  */

static u32 acpi_ut_get_argument_types(char *buffer, u16 argument_types);

/*                                                            */

static const char *ut_external_type_names[] =	/*                        */
{
	", UNSUPPORTED-TYPE",
	", Integer",
	", String",
	", Buffer",
	", Package"
};

/*                                                     */

static const char *ut_resource_type_names[] = {
	"/1",
	"/2",
	"/3",
	"/8",
	"/16",
	"/32",
	"/64",
	"/variable",
};

/*                                                                              
  
                                           
  
                                                  
  
                                                                          
                      
  
                                                                    
                                       
  
                                                                              */

const union acpi_predefined_info *acpi_ut_match_resource_name(char *name)
{
	const union acpi_predefined_info *this_name;

	/*                                                                       */

	if (name[0] != '_') {
		return (NULL);
	}

	/*                                                       */

	this_name = acpi_gbl_resource_names;
	while (this_name->info.name[0]) {
		if (ACPI_COMPARE_NAME(name, this_name->info.name)) {
			return (this_name);
		}

		this_name++;
	}

	return (NULL);		/*           */
}

/*                                                                              
  
                                                 
  
                                                                      
                                                                               
                                                                         
  
                    
  
                                                                         
                                                                         
                                     
  
                                                                              */

void
acpi_ut_display_predefined_method(char *buffer,
				  const union acpi_predefined_info *this_name,
				  u8 multi_line)
{
	u32 arg_count;

	/*
                                                
                                 
  */
	arg_count = acpi_ut_get_argument_types(buffer,
					       this_name->info.argument_list);

	if (multi_line) {
		printf("      ");
	}

	printf("%4.4s    Requires %s%u argument%s",
	       this_name->info.name,
	       (this_name->info.argument_list & ARG_COUNT_IS_MINIMUM) ?
	       "(at least) " : "", arg_count, arg_count != 1 ? "s" : "");

	/*                                     */

	if (arg_count > 0) {
		printf(" (%s)", buffer);
	}

	if (multi_line) {
		printf("\n    ");
	}

	/*                                      */

	if (this_name->info.expected_btypes) {
		acpi_ut_get_expected_return_types(buffer,
						  this_name->info.
						  expected_btypes);
		printf("  Return value types: %s\n", buffer);
	} else {
		printf("  No return value\n");
	}
}

/*                                                                              
  
                                          
  
                                                                         
                                                                 
  
                                                                        
  
                                                                        
                                                                            
                      
  
                                                                              */

static u32 acpi_ut_get_argument_types(char *buffer, u16 argument_types)
{
	u16 this_argument_type;
	u16 sub_index;
	u16 arg_count;
	u32 i;

	*buffer = 0;
	sub_index = 2;

	/*                                                              */

	arg_count = (argument_types & METHOD_ARG_MASK);
	argument_types >>= METHOD_ARG_BIT_WIDTH;

	if (arg_count > METHOD_PREDEF_ARGS_MAX) {
		printf("**** Invalid argument count (%u) "
		       "in predefined info structure\n", arg_count);
		return (arg_count);
	}

	/*                                                                    */

	for (i = 0; i < arg_count; i++) {
		this_argument_type = (argument_types & METHOD_ARG_MASK);
		if (!this_argument_type
		    || (this_argument_type > METHOD_MAX_ARG_TYPE)) {
			printf("**** Invalid argument type (%u) "
			       "in predefined info structure\n",
			       this_argument_type);
			return (arg_count);
		}

		strcat(buffer,
		       ut_external_type_names[this_argument_type] + sub_index);

		/*                                   */

		argument_types >>= METHOD_ARG_BIT_WIDTH;
		sub_index = 0;
	}

	return (arg_count);
}

/*                                                                              
  
                                              
  
                                                                            
                                                                     
  
                                                                  
  
                                                                       
  
                                                                              */

u32 acpi_ut_get_resource_bit_width(char *buffer, u16 types)
{
	u32 i;
	u16 sub_index;
	u32 found;

	*buffer = 0;
	sub_index = 1;
	found = 0;

	for (i = 0; i < NUM_RESOURCE_WIDTHS; i++) {
		if (types & 1) {
			strcat(buffer, &(ut_resource_type_names[i][sub_index]));
			sub_index = 0;
			found++;
		}

		types >>= 1;
	}

	return (found);
}
#endif
