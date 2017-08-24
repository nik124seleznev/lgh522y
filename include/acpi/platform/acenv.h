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

#ifndef __ACENV_H__
#define __ACENV_H__

/*
                                                                             
                                                                          
                                      
 */

/*                           */

#define ACPI_BINARY_SEMAPHORE       0
#define ACPI_OSL_MUTEX              1

/*                              */

#define DEBUGGER_SINGLE_THREADED    0
#define DEBUGGER_MULTI_THREADED     1

/*                                                                             
  
                                             
  
                                                                             */

/*                    */

#ifdef ACPI_ASL_COMPILER
#define ACPI_APPLICATION
#define ACPI_DISASSEMBLER
#define ACPI_DEBUG_OUTPUT
#define ACPI_CONSTANT_EVAL_ONLY
#define ACPI_LARGE_NAMESPACE_NODE
#define ACPI_DATA_TABLE_DISASSEMBLY
#define ACPI_SINGLE_THREADED
#endif

/*                                                               */

#ifdef ACPI_EXEC_APP
#define ACPI_APPLICATION
#define ACPI_FULL_DEBUG
#define ACPI_MUTEX_DEBUG
#define ACPI_DBG_TRACK_ALLOCATIONS
#endif

/*                                                                         */

#ifdef ACPI_NAMES_APP
#define ACPI_DEBUGGER
#define ACPI_APPLICATION
#define ACPI_SINGLE_THREADED
#endif

/*
                                                                       
                   
 */
#if (defined ACPI_BIN_APP)   || \
	(defined ACPI_SRC_APP)   || \
	(defined ACPI_XTRACT_APP)
#define ACPI_APPLICATION
#define ACPI_SINGLE_THREADED
#endif

#ifdef ACPI_HELP_APP
#define ACPI_APPLICATION
#define ACPI_SINGLE_THREADED
#define ACPI_NO_ERROR_MESSAGES
#endif

/*                         */

#ifdef ACPI_LIBRARY
#define ACPI_USE_LOCAL_CACHE
#define ACPI_FUTURE_USAGE
#endif

/*                                    */

#ifdef ACPI_APPLICATION
#define ACPI_USE_SYSTEM_CLIBRARY
#define ACPI_USE_LOCAL_CACHE
#endif

/*                      */

#ifdef ACPI_FULL_DEBUG
#define ACPI_DEBUGGER
#define ACPI_DEBUG_OUTPUT
#define ACPI_DISASSEMBLER
#endif


/*                                     */

/*                                                                             
  
                                                                          
                     
  
                                                                             */

#if defined(_LINUX) || defined(__linux__)
#include <acpi/platform/aclinux.h>

#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#include "acfreebsd.h"

#elif defined(__NetBSD__)
#include "acnetbsd.h"

#elif defined(__sun)
#include "acsolaris.h"

#elif defined(MODESTO)
#include "acmodesto.h"

#elif defined(NETWARE)
#include "acnetware.h"

#elif defined(_CYGWIN)
#include "accygwin.h"

#elif defined(WIN32)
#include "acwin.h"

#elif defined(WIN64)
#include "acwin64.h"

#elif defined(_WRS_LIB_BUILD)
#include "acvxworks.h"

#elif defined(__OS2__)
#include "acos2.h"

#elif defined(_AED_EFI)
#include "acefi.h"

#elif defined(__HAIKU__)
#include "achaiku.h"

#else

/*                     */

#error Unknown target environment
#endif

/*                                    */

/*                                                                             
  
                                                                          
                                 
  
                                                                             */

/*                   */

#ifndef COMPILER_DEPENDENT_INT64
#define COMPILER_DEPENDENT_INT64   long long
#endif

#ifndef COMPILER_DEPENDENT_UINT64
#define COMPILER_DEPENDENT_UINT64  unsigned long long
#endif

/*                                                                */
#ifndef ACPI_MUTEX_TYPE
#define ACPI_MUTEX_TYPE             ACPI_BINARY_SEMAPHORE
#endif

/*                             */

#ifndef ACPI_ACQUIRE_GLOBAL_LOCK
#define ACPI_ACQUIRE_GLOBAL_LOCK(Glptr, acquired) acquired = 1
#endif

#ifndef ACPI_RELEASE_GLOBAL_LOCK
#define ACPI_RELEASE_GLOBAL_LOCK(Glptr, pending) pending = 0
#endif

/*                                                                */

#ifndef ACPI_FLUSH_CPU_CACHE
#define ACPI_FLUSH_CPU_CACHE()
#endif

/*                                                                   */

#ifndef ACPI_INLINE
#define ACPI_INLINE
#endif

/*
                                    
  
                                                                       
                                                      
                                                      
                                                                         
 */
#ifndef ACPI_SYSTEM_XFACE
#define ACPI_SYSTEM_XFACE
#endif

#ifndef ACPI_EXTERNAL_XFACE
#define ACPI_EXTERNAL_XFACE
#endif

#ifndef ACPI_INTERNAL_XFACE
#define ACPI_INTERNAL_XFACE
#endif

#ifndef ACPI_INTERNAL_VAR_XFACE
#define ACPI_INTERNAL_VAR_XFACE
#endif

/*
                           
                                                                             
                                                                     
  
                                                                      
                                                 
 */
#ifndef DEBUGGER_THREADING
#ifdef ACPI_APPLICATION
#define DEBUGGER_THREADING          DEBUGGER_SINGLE_THREADED

#else
#define DEBUGGER_THREADING          DEBUGGER_MULTI_THREADED
#endif
#endif				/*                     */

/*                                                                             
  
                          
  
                                                                             */

/*
                                                                            
                                                                          
                                                                        
                                              
  
                                                                                
                                                                      
  
                                                                  
                          
 */
#ifdef ACPI_USE_SYSTEM_CLIBRARY

/*                                                                         */

#ifdef ACPI_USE_STANDARD_HEADERS

/*                                                      */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#endif				/*                           */

/*                                                   */

#define ACPI_STRSTR(s1,s2)      strstr((s1), (s2))
#define ACPI_STRCHR(s1,c)       strchr((s1), (c))
#define ACPI_STRLEN(s)          (acpi_size) strlen((s))
#define ACPI_STRCPY(d,s)        (void) strcpy((d), (s))
#define ACPI_STRNCPY(d,s,n)     (void) strncpy((d), (s), (acpi_size)(n))
#define ACPI_STRNCMP(d,s,n)     strncmp((d), (s), (acpi_size)(n))
#define ACPI_STRCMP(d,s)        strcmp((d), (s))
#define ACPI_STRCAT(d,s)        (void) strcat((d), (s))
#define ACPI_STRNCAT(d,s,n)     strncat((d), (s), (acpi_size)(n))
#define ACPI_STRTOUL(d,s,n)     strtoul((d), (s), (acpi_size)(n))
#define ACPI_MEMCMP(s1,s2,n)    memcmp((const char *)(s1), (const char *)(s2), (acpi_size)(n))
#define ACPI_MEMCPY(d,s,n)      (void) memcpy((d), (s), (acpi_size)(n))
#define ACPI_MEMSET(d,s,n)      (void) memset((d), (s), (acpi_size)(n))

#define ACPI_TOUPPER(i)         toupper((int) (i))
#define ACPI_TOLOWER(i)         tolower((int) (i))
#define ACPI_IS_XDIGIT(i)       isxdigit((int) (i))
#define ACPI_IS_DIGIT(i)        isdigit((int) (i))
#define ACPI_IS_SPACE(i)        isspace((int) (i))
#define ACPI_IS_UPPER(i)        isupper((int) (i))
#define ACPI_IS_PRINT(i)        isprint((int) (i))
#define ACPI_IS_ALPHA(i)        isalpha((int) (i))

#else

/*                                                                             
  
                                                        
  
                                                                             */

/*
                                                                          
                                                                        
                                                                           
              
 */
#ifndef va_arg

#ifndef _VALIST
#define _VALIST
typedef char *va_list;
#endif				/*         */

/*                              */

#define  _AUPBND                (sizeof (acpi_native_int) - 1)
#define  _ADNBND                (sizeof (acpi_native_int) - 1)

/*                                          */

#define _bnd(X, bnd)            (((sizeof (X)) + (bnd)) & (~(bnd)))
#define va_arg(ap, T)           (*(T *)(((ap) += (_bnd (T, _AUPBND))) - (_bnd (T,_ADNBND))))
#define va_end(ap)              (ap = (va_list) NULL)
#define va_start(ap, A)         (void) ((ap) = (((char *) &(A)) + (_bnd (A,_AUPBND))))

#endif				/*        */

/*                                                          */

#define ACPI_STRSTR(s1,s2)      acpi_ut_strstr ((s1), (s2))
#define ACPI_STRCHR(s1,c)       acpi_ut_strchr ((s1), (c))
#define ACPI_STRLEN(s)          (acpi_size) acpi_ut_strlen ((s))
#define ACPI_STRCPY(d,s)        (void) acpi_ut_strcpy ((d), (s))
#define ACPI_STRNCPY(d,s,n)     (void) acpi_ut_strncpy ((d), (s), (acpi_size)(n))
#define ACPI_STRNCMP(d,s,n)     acpi_ut_strncmp ((d), (s), (acpi_size)(n))
#define ACPI_STRCMP(d,s)        acpi_ut_strcmp ((d), (s))
#define ACPI_STRCAT(d,s)        (void) acpi_ut_strcat ((d), (s))
#define ACPI_STRNCAT(d,s,n)     acpi_ut_strncat ((d), (s), (acpi_size)(n))
#define ACPI_STRTOUL(d,s,n)     acpi_ut_strtoul ((d), (s), (acpi_size)(n))
#define ACPI_MEMCMP(s1,s2,n)    acpi_ut_memcmp((const char *)(s1), (const char *)(s2), (acpi_size)(n))
#define ACPI_MEMCPY(d,s,n)      (void) acpi_ut_memcpy ((d), (s), (acpi_size)(n))
#define ACPI_MEMSET(d,v,n)      (void) acpi_ut_memset ((d), (v), (acpi_size)(n))
#define ACPI_TOUPPER(c)         acpi_ut_to_upper ((int) (c))
#define ACPI_TOLOWER(c)         acpi_ut_to_lower ((int) (c))

#endif				/*                          */

#endif				/*             */
