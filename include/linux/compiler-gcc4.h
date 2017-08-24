#ifndef __LINUX_COMPILER_H
#error "Please don't include <linux/compiler-gcc4.h> directly, include <linux/compiler.h> instead."
#endif

/*                                 */
#ifdef __KERNEL__
# if GCC_VERSION >= 40100 &&  GCC_VERSION <= 40101
#  error Your version of gcc miscompiles the __weak directive
# endif
#endif

#define __used			__attribute__((__used__))
#define __must_check 		__attribute__((warn_unused_result))
#define __compiler_offsetof(a,b) __builtin_offsetof(a,b)

#if GCC_VERSION >= 40100 && GCC_VERSION < 40600
# define __compiletime_object_size(obj) __builtin_object_size(obj, 0)
#endif

#if GCC_VERSION >= 40300
/*                                                                   
                                                                    
                                                                  
                                                                      
                   

                                                                         
                                                                             
                                           

                                                                     
                                                                    
                      */
#define __cold			__attribute__((__cold__))

#define __UNIQUE_ID(prefix) __PASTE(__PASTE(__UNIQUE_ID_, prefix), __COUNTER__)

#ifndef __CHECKER__
# define __compiletime_warning(message) __attribute__((warning(message)))
# define __compiletime_error(message) __attribute__((error(message)))
#endif /*             */
#endif /*                      */

#if GCC_VERSION >= 40500
/*
                                                               
                                                                
                     
  
                                                                    
                                                                      
                                                                
 */
#define unreachable() __builtin_unreachable()

/*                                                             */
#define __noclone	__attribute__((__noclone__))

#endif /*                      */

#if GCC_VERSION >= 40600
/*
                                                                         
 */
#define __visible __attribute__((externally_visible))
#endif

/*
                                                     
  
                                                      
  
                                                                          
                                         
  
                                                                   
 */
#define asm_volatile_goto(x...)	do { asm goto(x); asm (""); } while (0)

#ifdef CONFIG_ARCH_USE_BUILTIN_BSWAP
#if GCC_VERSION >= 40400
#define __HAVE_BUILTIN_BSWAP32__
#define __HAVE_BUILTIN_BSWAP64__
#endif
#if GCC_VERSION >= 40800 || (defined(__powerpc__) && GCC_VERSION >= 40600)
#define __HAVE_BUILTIN_BSWAP16__
#endif
#endif /*                               */
