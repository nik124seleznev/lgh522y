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





#ifndef _ARM_CSTD_COMPILERS_H_
#define _ARM_CSTD_COMPILERS_H_

/*                                                                             
                                                                   
                                                                             */
/* 
                                       
     
 */

/* 
                   
                                                                     
             
 */
#define CSTD_TOOLCHAIN_MSVC         0

/* 
                   
                                                                         
             
 */
#define CSTD_TOOLCHAIN_GCC          0

/* 
                   
                                                                         
                                                                              
                                                                         
                
 */
#define CSTD_TOOLCHAIN_RVCT         0

/* 
                   
                                                                              
                            
 */
#define CSTD_TOOLCHAIN_RVCT_GCC_MODE 0

/* 
                   
                                                                              
 */
#define CSTD_CPU_X86_32             0

/* 
                   
                                                                       
             
 */
#define CSTD_CPU_X86_64             0

/* 
                   
                                                                       
 */
#define CSTD_CPU_ARM                0

/* 
                   
                                                                           
 */
#define CSTD_CPU_AARCH64            0


/* 
                   
                                                                       
 */
#define CSTD_CPU_MIPS               0

/* 
                   
                                                         
 */
#define CSTD_CPU_32BIT              0

/* 
                   
                                                         
 */
#define CSTD_CPU_64BIT              0

/* 
                   
                                                                         
                    
 */
#define CSTD_CPU_BIG_ENDIAN         0

/* 
                   
                                                                            
             
 */
#define CSTD_OS_WINDOWS             0

/* 
                   
                                                                              
                  
 */
#define CSTD_OS_WIN32               0

/* 
                   
                                                                              
                  
 */
#define CSTD_OS_WIN64               0

/* 
                   
                                                                        
 */
#define CSTD_OS_LINUX               0

/* 
                   
                                                                              
 */
#define CSTD_OS_LINUX_KERNEL        0

/* 
                   
                                                                            
                  
 */
#define CSTD_OS_LINUX32             0

/* 
                   
                                                                            
                  
 */
#define CSTD_OS_LINUX64             0

/* 
                   
                                                                          
 */
#define CSTD_OS_ANDROID             0

/* 
                   
                                                                                
 */
#define CSTD_OS_ANDROID_KERNEL      0

/* 
                   
                                                                              
                  
 */
#define CSTD_OS_ANDROID32           0

/* 
                   
                                                                              
                  
 */
#define CSTD_OS_ANDROID64           0

/* 
                   
                                                                        
                  
 */
#define CSTD_OS_APPLEOS             0

/* 
                   
                                                                               
                  
 */
#define CSTD_OS_APPLEOS32           0

/* 
                   
                                                                               
                  
 */
#define CSTD_OS_APPLEOS64           0

/* 
                       
                   
                                                                          
 */
#define CSTD_OS_SYMBIAN             0

/* 
                    
                   
                                                                          
            
 */
#define CSTD_OS_NONE                0

/*                                                                             
                              
                                                                             */

/*                                                                                                
                                                */
#define CSTD_PUSH_WARNING_GCC_WADDRESS
#define CSTD_POP_WARNING_GCC_WADDRESS

#if defined(_MSC_VER)
	#undef CSTD_TOOLCHAIN_MSVC
	#define CSTD_TOOLCHAIN_MSVC         1

#elif defined(__GNUC__)
	#undef CSTD_TOOLCHAIN_GCC
	#define CSTD_TOOLCHAIN_GCC          1

	/*                                   */
	#if defined(__ARMCC_VERSION)
		#undef CSTD_TOOLCHAIN_RVCT_GCC_MODE
		#define CSTD_TOOLCHAIN_RVCT_GCC_MODE    1
	#endif

	#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6 && MALI_GCC_WORKAROUND_MIDCOM_4598 == 0)
		/*                                                                                                         
                                                                                                         
                                                                                                        
                             */
		#undef CSTD_PUSH_WARNING_GCC_WADDRESS
		#define CSTD_PUSH_WARNING_GCC_WADDRESS \
			do\
			{\
				_Pragma("GCC diagnostic push")\
				_Pragma("GCC diagnostic ignored \"-Waddress\"")\
			}while(MALI_FALSE)

		#undef CSTD_POP_WARNING_GCC_WADDRESS
		#define CSTD_POP_WARNING_GCC_WADDRESS \
			do\
			{\
				_Pragma("GCC diagnostic pop")\
			}while(MALI_FALSE)
	#endif

#elif defined(__ARMCC_VERSION)
	#undef CSTD_TOOLCHAIN_RVCT
	#define CSTD_TOOLCHAIN_RVCT         1

#else
	#warning "Unsupported or unknown toolchain"

#endif

/*                                                                             
                        
                                                                             */
#if 1 == CSTD_TOOLCHAIN_MSVC
	#if defined(_M_IX86)
		#undef CSTD_CPU_X86_32
		#define CSTD_CPU_X86_32         1

	#elif defined(_M_X64) || defined(_M_AMD64)
		#undef CSTD_CPU_X86_64
		#define CSTD_CPU_X86_64         1

	#elif defined(_M_ARM)
		#undef CSTD_CPU_ARM
		#define CSTD_CPU_ARM            1

	#elif defined(_M_MIPS)
		#undef CSTD_CPU_MIPS
		#define CSTD_CPU_MIPS           1

	#else
		#warning "Unsupported or unknown host CPU for MSVC tools"

	#endif

#elif 1 == CSTD_TOOLCHAIN_GCC
	#if defined(__amd64__)
		#undef CSTD_CPU_X86_64
		#define CSTD_CPU_X86_64         1

	#elif defined(__i386__)
		#undef CSTD_CPU_X86_32
		#define CSTD_CPU_X86_32         1

	#elif defined(__arm__)
		#undef CSTD_CPU_ARM
		#define CSTD_CPU_ARM            1

	#elif defined(__aarch64__)
		#undef CSTD_CPU_AARCH64
		#define CSTD_CPU_AARCH64        1

	#elif defined(__mips__)
		#undef CSTD_CPU_MIPS
		#define CSTD_CPU_MIPS           1

	#else
		#warning "Unsupported or unknown host CPU for GCC tools"

	#endif

#elif 1 == CSTD_TOOLCHAIN_RVCT
	#undef CSTD_CPU_ARM
	#define CSTD_CPU_ARM                1

#else
	#warning "Unsupported or unknown toolchain"

#endif

/*                                                                             
                                   
                                                                             */

#if ((1 == CSTD_CPU_X86_32) || (1 == CSTD_CPU_X86_64))
	/*                                                                     */

#elif 1 == CSTD_CPU_AARCH64
	/*                        */

#elif 1 == CSTD_TOOLCHAIN_RVCT
	#if defined(__BIG_ENDIAN)
		#undef CSTD_ENDIAN_BIG
		#define CSTD_ENDIAN_BIG         1
	#endif

#elif ((1 == CSTD_TOOLCHAIN_GCC) && (1 == CSTD_CPU_ARM))
	#if defined(__ARMEB__)
		#undef CSTD_ENDIAN_BIG
		#define CSTD_ENDIAN_BIG         1
	#endif

#elif ((1 == CSTD_TOOLCHAIN_GCC) && (1 == CSTD_CPU_MIPS))
	#if defined(__MIPSEB__)
		#undef CSTD_ENDIAN_BIG
		#define CSTD_ENDIAN_BIG         1
	#endif

#elif 1 == CSTD_TOOLCHAIN_MSVC
	/*                                                                  */

#else
	#warning "Unsupported or unknown CPU"

#endif

/*                                                                             
                                                    
                                                                             */
#if 1 == CSTD_TOOLCHAIN_MSVC
	#if defined(_WIN32) && !defined(_WIN64)
		#undef CSTD_OS_WINDOWS
		#define CSTD_OS_WINDOWS         1
		#undef CSTD_OS_WIN32
		#define CSTD_OS_WIN32           1
		#undef CSTD_CPU_32BIT
		#define CSTD_CPU_32BIT          1

	#elif defined(_WIN32) && defined(_WIN64)
		#undef CSTD_OS_WINDOWS
		#define CSTD_OS_WINDOWS         1
		#undef CSTD_OS_WIN64
		#define CSTD_OS_WIN64           1
		#undef CSTD_CPU_64BIT
		#define CSTD_CPU_64BIT          1

	#else
		#warning "Unsupported or unknown host OS for MSVC tools"

	#endif

#elif 1 == CSTD_TOOLCHAIN_GCC
	#if defined(_WIN32) && defined(_WIN64)
		#undef CSTD_OS_WINDOWS
		#define CSTD_OS_WINDOWS         1
		#undef CSTD_OS_WIN64
		#define CSTD_OS_WIN64           1
		#undef CSTD_CPU_64BIT
		#define CSTD_CPU_64BIT          1

	#elif defined(_WIN32) && !defined(_WIN64)
		#undef CSTD_OS_WINDOWS
		#define CSTD_OS_WINDOWS         1
		#undef CSTD_OS_WIN32
		#define CSTD_OS_WIN32           1
		#undef CSTD_CPU_32BIT
		#define CSTD_CPU_32BIT          1

	#elif defined(ANDROID)
		#undef CSTD_OS_ANDROID
		#define CSTD_OS_ANDROID         1

		#if defined(__KERNEL__)
			#undef CSTD_OS_ANDROID_KERNEL
			#define CSTD_OS_ANDROID_KERNEL  1
		#endif

		#if defined(__LP64__) || defined(_LP64)
			#undef CSTD_OS_ANDROID64
			#define CSTD_OS_ANDROID64       1
			#undef CSTD_CPU_64BIT
			#define CSTD_CPU_64BIT          1
		#else
			#undef CSTD_OS_ANDROID32
			#define CSTD_OS_ANDROID32       1
			#undef CSTD_CPU_32BIT
			#define CSTD_CPU_32BIT          1
		#endif

	#elif defined(__KERNEL__) || defined(__linux)
		#undef CSTD_OS_LINUX
		#define CSTD_OS_LINUX           1
		
		#if defined(__KERNEL__)
			#undef CSTD_OS_LINUX_KERNEL
			#define CSTD_OS_LINUX_KERNEL    1
		#endif

		#if defined(__LP64__) || defined(_LP64)
			#undef CSTD_OS_LINUX64
			#define CSTD_OS_LINUX64         1
			#undef CSTD_CPU_64BIT
			#define CSTD_CPU_64BIT          1
		#else
			#undef CSTD_OS_LINUX32
			#define CSTD_OS_LINUX32         1
			#undef CSTD_CPU_32BIT
			#define CSTD_CPU_32BIT          1
		#endif

	#elif defined(__APPLE__)
		#undef CSTD_OS_APPLEOS
		#define CSTD_OS_APPLEOS         1

		#if defined(__LP64__) || defined(_LP64)
			#undef CSTD_OS_APPLEOS64
			#define CSTD_OS_APPLEOS64       1
			#undef CSTD_CPU_64BIT
			#define CSTD_CPU_64BIT          1
		#else
			#undef CSTD_OS_APPLEOS32
			#define CSTD_OS_APPLEOS32       1
			#undef CSTD_CPU_32BIT
			#define CSTD_CPU_32BIT          1
		#endif

	#elif defined(__SYMBIAN32__)
		#undef CSTD_OS_SYMBIAN
		#define CSTD_OS_SYMBIAN         1
		#undef CSTD_CPU_32BIT
		#define CSTD_CPU_32BIT          1

	#else
		#undef CSTD_OS_NONE
		#define CSTD_OS_NONE            1
		#undef CSTD_CPU_32BIT
		#define CSTD_CPU_32BIT          1

#endif

#elif 1 == CSTD_TOOLCHAIN_RVCT

	#if defined(ANDROID)
		#undef CSTD_OS_ANDROID
		#undef CSTD_OS_ANDROID32
		#define CSTD_OS_ANDROID         1
		#define CSTD_OS_ANDROID32       1

	#elif defined(__linux)
		#undef CSTD_OS_LINUX
		#undef CSTD_OS_LINUX32
		#define CSTD_OS_LINUX           1
		#define CSTD_OS_LINUX32         1

	#elif defined(__SYMBIAN32__)
		#undef CSTD_OS_SYMBIAN
		#define CSTD_OS_SYMBIAN         1

	#else
		#undef CSTD_OS_NONE
		#define CSTD_OS_NONE            1

#endif

#else
	#warning "Unsupported or unknown host OS"

#endif

/*                                                                             
                                                             
                                                                             */
/* 
                                                           
     
  
                                                                       
                                                                            
                                                                         
                                                                              
                               
  
                                                                             
                                                             
                               
  
                                                                        
                                                                  
                                                                         
                                                                          
                                                            
                                                                         
                                                                           
             
                                                                       
                                                                           
                                                                    
  
                                                                              
                                                                            
                         
 */

/* 
                        
                                                                            
            
 */

/* 
                        
                                                                    
                    
 */

/* 
                      
                                                                       
                                           
 */

/* 
                       
                                                                           
                          
 */

/* 
     
 */

#if 1 ==  CSTD_OS_LINUX
	#define CSTD_LINK_IMPORT __attribute__((visibility("default")))
	#define CSTD_LINK_EXPORT __attribute__((visibility("default")))
	#define CSTD_LINK_IMPL   __attribute__((visibility("default")))
	#define CSTD_LINK_LOCAL  __attribute__((visibility("hidden")))

#elif 1 ==  CSTD_OS_WINDOWS
	#define CSTD_LINK_IMPORT __declspec(dllimport)
	#define CSTD_LINK_EXPORT __declspec(dllexport)
	#define CSTD_LINK_IMPL   __declspec(dllexport)
	#define CSTD_LINK_LOCAL

#elif 1 ==  CSTD_OS_SYMBIAN
	#define CSTD_LINK_IMPORT IMPORT_C
	#define CSTD_LINK_EXPORT IMPORT_C
	#define CSTD_LINK_IMPL   EXPORT_C
	#define CSTD_LINK_LOCAL

#elif 1 ==  CSTD_OS_APPLEOS
	#define CSTD_LINK_IMPORT __attribute__((visibility("default")))
	#define CSTD_LINK_EXPORT __attribute__((visibility("default")))
	#define CSTD_LINK_IMPL   __attribute__((visibility("default")))
	#define CSTD_LINK_LOCAL  __attribute__((visibility("hidden")))

#elif 1 ==  CSTD_OS_ANDROID
	#define CSTD_LINK_IMPORT __attribute__((visibility("default")))
	#define CSTD_LINK_EXPORT __attribute__((visibility("default")))
	#define CSTD_LINK_IMPL   __attribute__((visibility("default")))
	#define CSTD_LINK_LOCAL  __attribute__((visibility("hidden")))

#else /*              */
	#define CSTD_LINK_IMPORT
	#define CSTD_LINK_EXPORT
	#define CSTD_LINK_IMPL
	#define CSTD_LINK_LOCAL

#endif

/* 
     
 */

#endif /*                              */
