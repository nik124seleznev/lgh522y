/* 
                                                                              
  
                                                                 
 */

/* This header file contains assembly-language definitions (assembly
   macros, etc.) for this specific Xtensa processor's TIE extensions
   and options.  It is customized to this Xtensa processor configuration.

   Copyright (c) 1999-2010 Tensilica Inc.

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  */

#ifndef _XTENSA_CORE_TIE_ASM_H
#define _XTENSA_CORE_TIE_ASM_H

/*                                                                 */
/*                   */
#define XTHAL_SAS_TIE	0x0001	/*                                 */
#define XTHAL_SAS_OPT	0x0002	/*                                  */
#define XTHAL_SAS_ANYOT	0x0003	/*                   */
/*                                           */
#define XTHAL_SAS_NOCC	0x0004	/*                                            */
#define XTHAL_SAS_CC	0x0008	/*                                            */
#define XTHAL_SAS_ANYCC	0x000C	/*                   */
/*                                       */
#define XTHAL_SAS_CALR	0x0010	/*              */
#define XTHAL_SAS_CALE	0x0020	/*              */
#define XTHAL_SAS_GLOB	0x0040	/*                                          */
#define XTHAL_SAS_ANYABI 0x0070	/*                        */
/*        */
#define XTHAL_SAS_ALL	0xFFFF	/*                                  */
#define XTHAL_SAS3(optie,ccuse,abi)	( ((optie) & XTHAL_SAS_ANYOT)  \
					| ((ccuse) & XTHAL_SAS_ANYCC)  \
					| ((abi)   & XTHAL_SAS_ANYABI) )



    /*
                                                                               
                                                     
                            
                                                               
                                                          
                                                                                
                                                            
                            
                                                                                  
                                                                  
                                                                              
                                                                          
                                   
                                                                                
                                                                   
                                                                            
                                                                     
                                                                        
     */
    .macro xchal_ncp_store  ptr at1 at2 at3 at4  continue=0 ofs=-1 select=XTHAL_SAS_ALL alloc=0
	xchal_sa_start	\continue, \ofs
	//                                                          
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_GLOB) & ~(\select)
	xchal_sa_align	\ptr, 0, 1020, 4, 4
	rur.THREADPTR	\at1		//                 
	s32i	\at1, \ptr, .Lxchal_ofs_+0
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.elseif ((XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_GLOB) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 1020, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.endif
	//                                                                 
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 1016, 4, 4
	rsr	\at1, ACCLO			//             
	s32i	\at1, \ptr, .Lxchal_ofs_+0
	rsr	\at1, ACCHI			//             
	s32i	\at1, \ptr, .Lxchal_ofs_+4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 8
	.elseif ((XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 1016, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 8
	.endif
	//                                                                     
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 1004, 4, 4
	rsr	\at1, M0			//             
	s32i	\at1, \ptr, .Lxchal_ofs_+0
	rsr	\at1, M1			//             
	s32i	\at1, \ptr, .Lxchal_ofs_+4
	rsr	\at1, M2			//             
	s32i	\at1, \ptr, .Lxchal_ofs_+8
	rsr	\at1, M3			//             
	s32i	\at1, \ptr, .Lxchal_ofs_+12
	rsr	\at1, SCOMPARE1			//                         
	s32i	\at1, \ptr, .Lxchal_ofs_+16
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 20
	.elseif ((XTHAL_SAS_OPT | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 1004, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 20
	.endif
    .endm	//                

    /*
                                                                                  
                                                     
                            
                                                               
                                                          
                                                                                
                                                            
                            
                                                                                 
                                                                  
                                                                              
                                                                         
                                   
                                                                               
                                                                   
                                                                            
                                                                     
                                                                       
     */
    .macro xchal_ncp_load  ptr at1 at2 at3 at4  continue=0 ofs=-1 select=XTHAL_SAS_ALL alloc=0
	xchal_sa_start	\continue, \ofs
	//                                                          
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_GLOB) & ~(\select)
	xchal_sa_align	\ptr, 0, 1020, 4, 4
	l32i	\at1, \ptr, .Lxchal_ofs_+0
	wur.THREADPTR	\at1		//                 
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.elseif ((XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_GLOB) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 1020, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.endif
	//                                                                 
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 1016, 4, 4
	l32i	\at1, \ptr, .Lxchal_ofs_+0
	wsr	\at1, ACCLO			//             
	l32i	\at1, \ptr, .Lxchal_ofs_+4
	wsr	\at1, ACCHI			//             
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 8
	.elseif ((XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 1016, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 8
	.endif
	//                                                                     
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 1004, 4, 4
	l32i	\at1, \ptr, .Lxchal_ofs_+0
	wsr	\at1, M0			//             
	l32i	\at1, \ptr, .Lxchal_ofs_+4
	wsr	\at1, M1			//             
	l32i	\at1, \ptr, .Lxchal_ofs_+8
	wsr	\at1, M2			//             
	l32i	\at1, \ptr, .Lxchal_ofs_+12
	wsr	\at1, M3			//             
	l32i	\at1, \ptr, .Lxchal_ofs_+16
	wsr	\at1, SCOMPARE1			//                         
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 20
	.elseif ((XTHAL_SAS_OPT | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 1004, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 20
	.endif
    .endm	//               


#define XCHAL_NCP_NUM_ATMPS	1



#define XCHAL_SA_NUM_ATMPS	1

#endif /*                      */

