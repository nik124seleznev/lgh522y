/*
 * asm/tbx.h
 *
 * Copyright (C) 2000-2012 Imagination Technologies.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation.
 *
 * Thread binary interface header
 */

#ifndef _ASM_METAG_TBX_H_
#define _ASM_METAG_TBX_H_

/*                     */
#include <asm/metag_isa.h>
/*                             */
#include <asm/metag_mem.h>

#ifdef  TBI_1_4
#ifndef TBI_MUTEXES_1_4
#define TBI_MUTEXES_1_4
#endif
#ifndef TBI_SEMAPHORES_1_4
#define TBI_SEMAPHORES_1_4
#endif
#ifndef TBI_ASYNC_SWITCH_1_4
#define TBI_ASYNC_SWITCH_1_4
#endif
#ifndef TBI_FASTINT_1_4
#define TBI_FASTINT_1_4
#endif
#endif


/*                                                                  
                                                                     
                                                                    
           */
#define TBID_SIGPOLL_BIT    0x02 /*                                         */
/*                                                              */
#define TBID_IS_SEGSTR( Id ) (((Id) & (TBID_SEGTYPE_BITS>>1)) == 0)

/*                                                              */
#define TBID_SEGTYPE_BITS   0x0F /*                                     */
#define TBID_SEGTYPE_S      0
#define TBID_SEGSCOPE_BITS  0x30 /*                                    */
#define TBID_SEGSCOPE_S     4
#define TBID_SEGGADDR_BITS  0xC0 /*                                      */
#define TBID_SEGGADDR_S     6

/*                                                                */
#define TBID_SEGTYPE_TEXT   0x02 /*              */
#define TBID_SEGTYPE_DATA   0x04 /*              */
#define TBID_SEGTYPE_STACK  0x06 /*               */
#define TBID_SEGTYPE_HEAP   0x0A /*              */
#define TBID_SEGTYPE_ROOT   0x0C /*                     */
#define TBID_SEGTYPE_STRING 0x0E /*                      */

/*                                            */
#define TBID_SEGSCOPE_INIT     0 /*                                         */
#define TBID_SEGSCOPE_LOCAL    1 /*                        */
#define TBID_SEGSCOPE_GLOBAL   2 /*                                       */
#define TBID_SEGSCOPE_SHARED   3 /*                                      */

/*                                                                   
                                                                         
               */
#define TBID_SEGGADDR_NULL     0 /*                                         */
#define TBID_SEGGADDR_READ     1 /*                         */
#define TBID_SEGGADDR_WRITE    2 /*                         */
#define TBID_SEGGADDR_EXEC     3 /*                         */

/*                                                                        
                                            */

/*                                                                
                                                                        
                                                             */
#define TBID_ISTAT_BIT    0x01000000
#define TBID_ISTAT_S      24

/*                                                                   
   
                                                                        
                                                                       
                                                                         
                                                                     */
#define TBID_PSTAT_BIT    0x02000000
#define TBID_PSTAT_S      25

/*                                                                          
                                                 */
#define TBID_THREAD_BITS  0xFC000000
#define TBID_THREAD_S     26

/*                          */
#define TBID_THREAD_NULL   (-32) /*                                          */
#define TBID_THREAD_GLOBAL (-31) /*                              */
#define TBID_THREAD_HOST   ( -1) /*                */
#define TBID_THREAD_EXTIO  (TBID_THREAD_HOST)   /*                      */

/*                                                                      
                      */
#define TBID_IS_VIRTTHREAD( Id ) ((Id) < 0)

/*                                                               */
#define TBID_IS_REALTHREAD( Id ) ((Id) >= 0)

/*                                                     */
#define TBID_SEG( Thread, Scope, Type )                           (\
    ((Thread)<<TBID_THREAD_S) + ((Scope)<<TBID_SEGSCOPE_S) + (Type))

/*                                              */
#define TBID_SIG( Thread, SigNum )                                        (\
    ((Thread)<<TBID_THREAD_S) + ((SigNum)<<TBID_SIGNUM_S) + TBID_SIGNAL_BIT)

/*                                                                       */
#define TBID_THD( Thread ) ((Thread)<<TBID_THREAD_S)
#define TBID_THD_NULL      ((TBID_THREAD_NULL)  <<TBID_THREAD_S)
#define TBID_THD_GLOBAL    ((TBID_THREAD_GLOBAL)<<TBID_THREAD_S)

/*                                                                       
                                                                    */
#define TBIXXF_SIGNUM_IIF   0x01 /*                           */
#define TBIXXF_SIGNUM_PGF   0x02 /*                         */
#define TBIXXF_SIGNUM_DHF   0x03 /*                            */
#define TBIXXF_SIGNUM_IGF   0x05 /*                                 */
#define TBIXXF_SIGNUM_DGF   0x07 /*                                      */
#define TBIXXF_SIGNUM_IPF   0x09 /*                       */
#define TBIXXF_SIGNUM_DPF   0x0B /*                        */
#define TBIXXF_SIGNUM_IHF   0x0D /*                            */
#define TBIXXF_SIGNUM_DWF   0x0F /*                             */

/*                                                                       
                                                                       
                                                                              
                                                                       */
#define TBID_SIGNUM_SW0     0x00 /*                                  */
#define TBID_SIGNUM_SW1     0x01 /*                                    */
#define TBID_SIGNUM_SW2     0x02 /*                                          */
#define TBID_SIGNUM_SW3     0x03 /*                                          */
#ifdef TBI_1_4
#define TBID_SIGNUM_FPE     0x04 /*                                             */
#define TBID_SIGNUM_FPD     0x05 /*                                         */
/*                                                */
#define TBID_SIGNUM_BUS     0x07 /*                                */
/*                    */
#else
/*                    */
#endif
#define TBID_SIGNUM_SWS     0x0A /*                                 */
#define TBID_SIGNUM_SWK     0x0B /*                                 */
/*                    */
#define TBID_SIGNUM_TRT     0x10 /*               */
#define TBID_SIGNUM_LWK     0x11 /*                                          */
#define TBID_SIGNUM_XXF     0x12 /*                                        */
#ifdef TBI_1_4
#define TBID_SIGNUM_DFR     0x13 /*                            */
#else
#define TBID_SIGNUM_FPE     0x13 /*                       */
#endif
/*                                                      */
#define TBID_SIGNUM_TR1(Thread) (0x14+(Thread))
#define TBID_SIGNUM_T10     0x14
#define TBID_SIGNUM_T11     0x15
#define TBID_SIGNUM_T12     0x16
#define TBID_SIGNUM_T13     0x17
/*                                                      */
#define TBID_SIGNUM_TR2(Thread) (0x18+(Thread))
#define TBID_SIGNUM_T20     0x18
#define TBID_SIGNUM_T21     0x19
#define TBID_SIGNUM_T22     0x1A
#define TBID_SIGNUM_T23     0x1B
#define TBID_SIGNUM_TR3     0x1C /*                               */
#define TBID_SIGNUM_TR4     0x1D /*                               */
#define TBID_SIGNUM_TR5     0x1E /*                               */
#define TBID_SIGNUM_TR6     0x1F /*                               */
#define TBID_SIGNUM_MAX     0x1F

/*                                                                 
                                                                       */
#define TBI_TRIG_BIT(SigNum)                                      (\
    ((SigNum) >= TBID_SIGNUM_TRT) ? 1<<((SigNum)-TBID_SIGNUM_TRT) :\
    ( ((SigNum) == TBID_SIGNUM_SWS) ||                             \
      ((SigNum) == TBID_SIGNUM_SWK)    ) ?                         \
                         TXSTAT_KICK_BIT : TXSTATI_BGNDHALT_BIT    )

/*                                                             
                                                                     */
#define TBI_TRIG_VEC(SigNum)                                      (\
    ((SigNum) >= TBID_SIGNUM_T10) ? ((SigNum)-TBID_SIGNUM_TRT) : -1)

/*                                                                     */
#define TBI_TRIGS_INIT( Thread )                           (\
    TXSTAT_KICK_BIT + TBI_TRIG_BIT(TBID_SIGNUM_TR1(Thread)) )
#define TBI_INTS_INIT( Thread )                            (\
    TXSTAT_KICK_BIT + TXSTATI_BGNDHALT_BIT                  \
                    + TBI_TRIG_BIT(TBID_SIGNUM_TR2(Thread)) )

#ifndef __ASSEMBLY__
/*                                                               */
typedef volatile int TBISPIN, *PTBISPIN;

/*                                                        
                           
 */
typedef volatile int *PTBIKICK;

#if defined(METAC_1_0) || defined(METAC_1_1)
/*                         */
#define TBI_KICK( pKick ) do { pKick[0] = 1; } while (0)
#else
/*                                                                 */
#ifdef LINSYSEVENT_WR_COMBINE_FLUSH
/*                                                           */
#define TBI_KICK( pKick )                                                do {\
    volatile int *pFlush = (volatile int *) LINSYSEVENT_WR_COMBINE_FLUSH;    \
    pFlush[0] = 0;                                                           \
    pKick[0]  = 1;                                                } while (0)
#endif
#endif /*                                             */
#endif /*                     */

#ifndef __ASSEMBLY__
/*                              */
typedef struct _tbidual_tag_ {
    /*                                                                */
    int U0, U1;
} TBIDUAL, *PTBIDUAL;
#endif /*                     */

/*                                       */
#define TBIDUAL_U0      (0)
#define TBIDUAL_U1      (4)

#define TBIDUAL_BYTES   (8)

#define TBICTX_CRIT_BIT 0x0001  /*                             */
#define TBICTX_SOFT_BIT 0x0002  /*                                           */
#ifdef TBI_FASTINT_1_4
#define TBICTX_FINT_BIT 0x0004  /*                       */
#endif
#define TBICTX_FPAC_BIT 0x0010  /*                                          */
#define TBICTX_XMCC_BIT 0x0020  /*                             */
#define TBICTX_CBUF_BIT 0x0040  /*                                          */
#define TBICTX_CBRP_BIT 0x0080  /*                                    */
#define TBICTX_XDX8_BIT 0x0100  /*                         */
#define TBICTX_XAXX_BIT 0x0200  /*                                     */
#define TBICTX_XHL2_BIT 0x0400  /*                                   */
#define TBICTX_XTDP_BIT 0x0800  /*                         */
#define TBICTX_XEXT_BIT 0x1000  /*                                        
                                                                            
                                                                          */
#define TBICTX_WAIT_BIT 0x2000  /*                                         */
#define TBICTX_XCBF_BIT 0x4000  /*                                          */
#define TBICTX_PRIV_BIT 0x8000  /*                                       */

#ifdef METAC_1_0
#define TBICTX_XAX3_BIT 0x0200  /*                             */
#define TBICTX_AX_REGS  5       /*                                         */
#else
#define TBICTX_XAX4_BIT 0x0200  /*                             */
#define TBICTX_AX_REGS  4       /*                         */
#endif

#ifdef TBI_1_4
#define TBICTX_CFGFPU_FX16_BIT  0x00010000               /*                        */

/*                                                                */
#define METAC_COREID_CFGXCTX_MASK( Value )                                 (\
	( (((Value & METAC_COREID_CFGDSP_BITS)>>                                \
	             METAC_COREID_CFGDSP_S      ) == METAC_COREID_CFGDSP_MIN) ? \
	         ~(TBICTX_XHL2_BIT+TBICTX_XTDP_BIT+                             \
	           TBICTX_XAXX_BIT+TBICTX_XDX8_BIT ) : ~0U )                    )
#endif

/*                                                                          
                                                                            
                                                                             
                                                                            
                         
   
                                                                           
                                                                  
                                                             
   
                                                                            
                                                                          
                                                                     
                                                                             
                                                  */
#ifndef __ASSEMBLY__
typedef union _tbiextctx_tag_ {
    long long Val;
    TBIDUAL AX2;
    struct _tbiextctxext_tag {
#ifdef TBI_1_4
        short DspramSizes;      /*                                      
                                                                   */
#else
        short Reserved0;
#endif
        short SaveMask;         /*                           */
        PTBIDUAL pExt;          /*                                         */
    
    } Ctx;
    
} TBIEXTCTX, *PTBIEXTCTX;

/*                                                                   */
extern TBIEXTCTX __TBIExtCtx;
#endif /*                     */

/*                                         */
#define TBIEXTCTX_AX2           (0)
#define TBIEXTCTX_Ctx           (0)
#define TBIEXTCTX_Ctx_SaveMask  (TBIEXTCTX_Ctx + 2)
#define TBIEXTCTX_Ctx_pExt      (TBIEXTCTX_Ctx + 2 + 2)

/*                                                  */
#define TBICTXEXT_BYTES          (8)
#define TBICTXEXTBB8_BYTES     (8*8)
#define TBICTXEXTAX3_BYTES     (3*8)
#define TBICTXEXTAX4_BYTES     (4*8)
#ifdef METAC_1_0
#define TBICTXEXTAXX_BYTES     TBICTXEXTAX3_BYTES
#else
#define TBICTXEXTAXX_BYTES     TBICTXEXTAX4_BYTES
#endif
#define TBICTXEXTHL2_BYTES     (3*8)
#define TBICTXEXTTDR_BYTES    (27*8)
#define TBICTXEXTTDP_BYTES TBICTXEXTTDR_BYTES

#ifdef TBI_1_4
#define TBICTXEXTFX8_BYTES	(4*8)
#define TBICTXEXTFPAC_BYTES	(1*4 + 2*2 + 4*8)
#define TBICTXEXTFACF_BYTES	(3*8)
#endif

/*                                                         */
#define TBICTXEXT_MAXBITS  (TBICTX_XEXT_BIT|                \
                            TBICTX_XDX8_BIT|TBICTX_XAXX_BIT|\
                            TBICTX_XHL2_BIT|TBICTX_XTDP_BIT )

/*                                                                    */
#define TBICTXEXT_MAXBYTES (TBICTXEXT_BYTES+TBICTXEXTBB8_BYTES+\
                         TBICTXEXTAXX_BYTES+TBICTXEXTHL2_BYTES+\
                                            TBICTXEXTTDP_BYTES )

#ifdef TBI_FASTINT_1_4
/*                                                         */
#define TBICTX2EXT_MAXBITS (TBICTX_XDX8_BIT|TBICTX_XAXX_BIT|\
                            TBICTX_XHL2_BIT|TBICTX_XTDP_BIT )

/*                                                                    */
#define TBICTX2EXT_MAXBYTES (TBICTXEXTBB8_BYTES+TBICTXEXTAXX_BYTES\
                             +TBICTXEXTHL2_BYTES+TBICTXEXTTDP_BYTES )
#endif

/*                                                                       
                                                     

                                                                         
                                                                          
                                                                     
                                                     
        
                                                                             
                                                                            
                              

                      
   
                                                                           
                        
                                                     
                                                                    
        
                                                                           
                                                 

                                                                          
                                                                       
                                                                            
                                                                            
                                                        
   
                                                         
         
                                                 
                                                           
        
                                                                        
                                  
         
        
                                                        
    
                                       
                                  
                                            

                                                   
         
                                                     
                                            
            
                                                                           
            
   
 */

/*                                 */
#ifndef __ASSEMBLY__
typedef struct _tbictx_tag_ {
    /*                                                             */
    short Flags;
    /*                                                                 */
    short SaveMask;
    /*                */
    int CurrPC;
    /*                                */
    TBIDUAL DX[8];
    /*                                                                    
                                                                           
                                               */
    int CurrRPT, CurrBPOBITS, CurrMODE, CurrDIVTIME;
    /*                          */
    TBIDUAL AX[2];
    TBIEXTCTX Ext;
    TBIDUAL AX3[TBICTX_AX_REGS-3];
    
    /*                                                                       
                                                                         
                        */
    
} TBICTX, *PTBICTX;

#ifdef TBI_FASTINT_1_4
typedef struct _tbictx2_tag_ {
    TBIDUAL AX[2];    /*            */
    TBIDUAL DX[2];    /*            */
    int     CurrMODE;
    int     CurrRPT;
    int     CurrSTATUS;
    void   *CurrPC;   /*                        */
} TBICTX2, *PTBICTX2;
/*                        
                                                     
                                                         
                                                                                                   
 */

typedef struct _tbictxgp_tag_ {
    short    DspramSizes;
    short    SaveMask;
    void    *pExt;
    TBIDUAL  DX[6]; /*                      */
    TBIDUAL  AX[2]; /*           */
} TBICTXGP, *PTBICTXGP;

#define TBICTXGP_DspramSizes (0)
#define TBICTXGP_SaveMask    (TBICTXGP_DspramSizes + 2)
#define TBICTXGP_MAX_BYTES   (2 + 2 + 4 + 8*(6+2))

#endif
#endif /*                     */

/*                                      */
#define TBICTX_Flags            (0)
#define TBICTX_SaveMask         (2)
#define TBICTX_CurrPC           (4)
#define TBICTX_DX               (2 + 2 + 4)
#define TBICTX_CurrRPT          (2 + 2 + 4 + 8 * 8)
#define TBICTX_CurrMODE         (2 + 2 + 4 + 8 * 8 + 4 + 4)
#define TBICTX_AX               (2 + 2 + 4 + 8 * 8 + 4 + 4 + 4 + 4)
#define TBICTX_Ext              (2 + 2 + 4 + 8 * 8 + 4 + 4 + 4 + 4 + 2 * 8)
#define TBICTX_Ext_AX2          (TBICTX_Ext + TBIEXTCTX_AX2)
#define TBICTX_Ext_AX2_U0       (TBICTX_Ext + TBIEXTCTX_AX2 + TBIDUAL_U0)
#define TBICTX_Ext_AX2_U1       (TBICTX_Ext + TBIEXTCTX_AX2 + TBIDUAL_U1)
#define TBICTX_Ext_Ctx_pExt     (TBICTX_Ext + TBIEXTCTX_Ctx_pExt)
#define TBICTX_Ext_Ctx_SaveMask (TBICTX_Ext + TBIEXTCTX_Ctx_SaveMask)

#ifdef TBI_FASTINT_1_4
#define TBICTX2_BYTES (8 * 2 + 8 * 2 + 4 + 4 + 4 + 4)
#define TBICTXEXTCB0_BYTES (4 + 4 + 8)

#define TBICTX2_CRIT_MAX_BYTES (TBICTX2_BYTES + TBICTXEXTCB0_BYTES + 6 * TBIDUAL_BYTES)
#define TBI_SWITCH_NEXT_PC(PC, EXTRA) ((PC) + (EXTRA & 1) ? 8 : 4)
#endif

#ifndef __ASSEMBLY__
/*                                                               */
typedef struct _tbictxextcb0_tag_ {
    /*                                                                    */
    unsigned long CBFlags, CBAddr;
    /*             */
    TBIDUAL CBData;
    
} TBICTXEXTCB0, *PTBICTXEXTCB0;

/*                                                                         */
typedef struct _tbictxextrp6_tag_ {
    /*                                                            */
    unsigned long RPMask, Reserved0;
    TBIDUAL CBData[6];
    
} TBICTXEXTRP6, *PTBICTXEXTRP6;

/*                                                        */
typedef struct _tbictxextbb8_tag_ {
    /*                                               */
    TBIDUAL UX[8];
    
} TBICTXEXTBB8, *PTBICTXEXTBB8;

/*                                                        */
typedef struct _tbictxextbb3_tag_ {
    /*                                                  */
    TBIDUAL UX[3];
    
} TBICTXEXTBB3, *PTBICTXEXTBB3;

/*                                                                      */
typedef struct _tbictxextbb4_tag_ {
    /*                                                         */
    TBIDUAL UX[4];
    
} TBICTXEXTBB4, *PTBICTXEXTBB4;

/*                                                                 */
typedef struct _tbictxexthl2_tag_ {
    /*                                  */
    TBIDUAL Start, End, Count;
    
} TBICTXEXTHL2, *PTBICTXEXTHL2;

/*                                                        */
typedef struct _tbictxexttdp_tag_ {
    /*                                                            */
    TBIDUAL Acc32[1];
    /*                                                              */
    TBIDUAL Acc64[1];
    /*                                                          */
    TBIDUAL PReg[4];
    /*                                       */
    int CurrMRSIZE, Reserved0;
    
} TBICTXEXTTDP, *PTBICTXEXTTDP;

/*                                                                          */
typedef struct _tbictxexttdpr_tag_ {
    /*                                                            */
    TBIDUAL Acc32[1];
    /*                                                            */
    TBIDUAL Acc40[1];
    /*                  */
    TBIDUAL RP0[2],  WP0[2],  RP1[2],  WP1[2];
    /*                    */
    TBIDUAL RPI0[2], WPI0[2], RPI1[2], WPI1[2];
    /*                    */
    unsigned long Tmplt[16];
    /*                                                            */
    int CurrMRSIZE, CurrDRSIZE;
    
} TBICTXEXTTDPR, *PTBICTXEXTTDPR;

#ifdef TBI_1_4
/*                                                         
                                                                */
#define TBICTXEXTFPU_CONFIG_MASK  ( (METAC_COREID_NOFPACC_BIT+     \
                                     METAC_COREID_CFGFPU_BITS ) << \
                                     METAC_COREID_CONFIG_BITS       )

/*                                                     */
#define TBICTXEXTFPU_DEFRFPU_MASK (TXDEFR_FPU_FE_BITS)

/*                                                        */
typedef struct _tbictxextfpu_tag_ {
    /*                             */
    int CfgFpu;
    /*                                           
      
                                                               
                           
                                      
                           
                                    
     */
    short DefrFpu;

    /*                            */
    short ModeFpu;
    
    /*                              */
    TBIDUAL FX[4];
   
    /*                                                                          */
    /*                                                                          */
} TBICTXEXTFPU, *PTBICTXEXTFPU;

/*                                                          */
typedef struct _tbictxextfpacc_tag_ {
    /*                                                     */
    TBIDUAL FAcc32[3];
    
} TBICTXEXTFPACC, *PTBICTXEXTFPACC;
#endif

/*                         */
struct _tbi_tag_ ;

/*                                                     */
typedef union _tbires_tag_ {
    /*                                                              */
    long long Val;

    /*                                         */
    struct _tbires_sig_tag_ { 
        /*                                                                */
        unsigned short TrigMask;
        /*                                                               */
        unsigned short SaveMask;
        /*                                                              */
        PTBICTX pCtx;
    } Sig;

    /*                              */
    struct _tbires_thrdprivid_tag_ {
        /*                                                */
        int Id;
        /*                                                        */
        int Priv;
    } Thrd;

    /*                                            */
    struct _tbires_switch_tag_ { 
        /*                                        */
        void *pPara;
        /*                                                       */
        PTBICTX pCtx;
    } Switch;
    
    /*                              */
    struct _tbires_ccb_tag_ {
        void *pCCB;
        int COff;
    } CCB;

    struct _tbires_tlb_tag_ {
        int Leaf;  /*               */
        int Flags; /*           */
    } Tlb;

#ifdef TBI_FASTINT_1_4
    struct _tbires_intr_tag_ {
      short    TrigMask;
      short    SaveMask;
      PTBICTX2 pCtx;
    } Intr;
#endif

} TBIRES, *PTBIRES;
#endif /*                     */

#ifndef __ASSEMBLY__
/*                                                                            
                      
   
                                                                  
                                                                            
                                                
   
                                                                            
                                                                       
                                                                   
          
                                                                         
                                            
   
                                                                        
                                                                         
                                                                
        
                                                                        
                                       
          
                                                                            
                                                                             
                                                                         
                                                                    
                                 
   
                                                                           
                         

                                                                           
                                                                             
                                       
          
                                                                         
                                                            
          
                                    
   
                          

 */
typedef TBIRES (*PTBIAPIFN)( TBIRES State, int SigNum,
                             int Triggers, int InstOrSWSId,
                             volatile struct _tbi_tag_ *pTBI );
#endif /*                     */

#ifndef __ASSEMBLY__
/*                                                                     */
typedef volatile struct _tbiseg_tag_ {
    volatile struct _tbiseg_tag_ *pLink;
    int Id;                           /*                   */
    TBISPIN Lock;                     /*                                   */
    unsigned int Bytes;               /*                         */
    void *pGAddr;                     /*                                     */
    void *pLAddr;                     /*                                    */
    int Data[2];                      /*                                         */

} TBISEG, *PTBISEG;
#endif /*                     */

/*                                       */
#define TBISEG_pLink    ( 0)
#define TBISEG_Id       ( 4)
#define TBISEG_Lock     ( 8)
#define TBISEG_Bytes    (12)
#define TBISEG_pGAddr   (16)
#define TBISEG_pLAddr   (20)
#define TBISEG_Data     (24)

#ifndef __ASSEMBLY__
typedef volatile struct _tbi_tag_ {
    int SigMask;                      /*                                  */
    PTBIKICK pKick;                   /*                          */
    void *pCCB;                       /*                     */
    PTBISEG pSeg;                     /*                           */
    PTBIAPIFN fnSigs[TBID_SIGNUM_MAX+1];/*                          */
} *PTBI, TBI;
#endif /*                     */

/*                                   */
#define TBI_SigMask     (0)
#define TBI_pKick       (4)
#define TBI_pCCB        (8)
#define TBI_pSeg       (12)
#define TBI_fnSigs     (16)

#ifdef TBI_1_4
#ifndef __ASSEMBLY__
/*                                                 */
extern TBIRES __TBIHandleDFR ( TBIRES State, int SigNum,
                               int Triggers, int InstOrSWSId,
                               volatile struct _tbi_tag_ *pTBI );
#endif
#endif

/*                                     */
#define METAG_TBI_STRS (0x5300) /*                              */
#define METAG_TBI_STRE (0x4500) /*                                     */
#define METAG_TBI_STRG (0x4700) /*                              */
#define METAG_TBI_STRX (0x5A00) /*                                      */

#ifndef __ASSEMBLY__
typedef volatile struct _tbistr_tag_ {
    short Bytes;                      /*                          */
    short Tag;                        /*                                 */
    short Len;                        /*                                        */
    short TransLen;                   /*                                 */
    char String[8];                   /*                                 */

} TBISTR, *PTBISTR;
#endif /*                     */

/*                                                                       
           */
#define METAG_TBI_ICACHE_SIZE_S    0             /*                    */
#define METAG_TBI_ICACHE_SIZE_BITS 0x0000000F
#define METAG_TBI_ICACHE_FILL_S    4
#define METAG_TBI_ICACHE_FILL_BITS 0x000000F0
#define METAG_TBI_DCACHE_SIZE_S    8
#define METAG_TBI_DCACHE_SIZE_BITS 0x00000F00
#define METAG_TBI_DCACHE_FILL_S    12
#define METAG_TBI_DCACHE_FILL_BITS 0x0000F000

/*                      
                                                                      
                                                                             
                                       
                                    
                      
                      
                      
                      
                      

                        
                                                                        
                                                                        
                                                                        
                                        
                        
                        
                        
        
                        
 */

#define METAG_TBI_CACHE_SIZE_BASE_LOG2 14

/*                                                              */
#ifndef __ASSEMBLY__
#define TBISTR_DECL( Name, Str )                                       \
    __attribute__ ((__section__ (".tbistr") )) const char Name[] = #Str
#endif

/*                                                          */
#define TBI_TIMERWAIT_MIN (-16)         /*                              */
#define TBI_TIMERWAIT_MAX (-0x7FFFFFFF) /*                              */

#ifndef __ASSEMBLY__
/*                                                                     
                                                                       
                                                                            
                                                                             
                                                         */
#define TBI_GETREG( Reg )                                  __extension__ ({\
   int __GRValue;                                                          \
   __asm__ volatile ("MOV\t%0," #Reg "\t/* (*TBI_GETREG OK) */" :          \
                     "=r" (__GRValue) );                                   \
    __GRValue;                                                            })

#define TBI_SETREG( Reg, Value )                                       do {\
   int __SRValue = Value;                                                  \
   __asm__ volatile ("MOV\t" #Reg ",%0\t/* (*TBI_SETREG OK) */" :          \
                     : "r" (__SRValue) );                       } while (0)

#define TBI_SWAPREG( Reg, Value )                                      do {\
   int __XRValue = (Value);                                                \
   __asm__ volatile ("SWAP\t" #Reg ",%0\t/* (*TBI_SWAPREG OK) */" :        \
                     "=r" (__XRValue) : "0" (__XRValue) );                 \
   Value = __XRValue;                                           } while (0)

/*                                                                         
                                                       */
#define TBI_NOINTSCRITON                                             do {\
   __asm__ volatile ("LOCK1\t\t/* (*TBI_NOINTSCRITON OK) */");} while (0)
#define TBI_NOINTSCRITOFF                                             do {\
   __asm__ volatile ("LOCK0\t\t/* (*TBI_NOINTSCRITOFF OK) */");} while (0)
/*                                                  */

#define TBI_LOCK( TrigState )                                          do {\
   int __TRValue;                                                          \
   int __ALOCKHI = LINSYSEVENT_WR_ATOMIC_LOCK & 0xFFFF0000;                \
   __asm__ volatile ("MOV %0,#0\t\t/* (*TBI_LOCK ... */\n\t"               \
                     "SWAP\t%0,TXMASKI\t/* ... */\n\t"                     \
                     "LOCK2\t\t/* ... */\n\t"                              \
                     "SETD\t[%1+#0x40],D1RtP /* ... OK) */" :              \
                     "=r&" (__TRValue) : "u" (__ALOCKHI) );                \
   TrigState = __TRValue;                                       } while (0)
#define TBI_CRITON( TrigState )                                        do {\
   int __TRValue;                                                          \
   __asm__ volatile ("MOV %0,#0\t\t/* (*TBI_CRITON ... */\n\t"             \
                     "SWAP\t%0,TXMASKI\t/* ... */\n\t"                     \
                     "LOCK1\t\t/* ... OK) */" :                            \
                     "=r" (__TRValue) );                                   \
   TrigState = __TRValue;                                       } while (0)

#define TBI_INTSX( TrigState )                                         do {\
   int __TRValue = TrigState;                                              \
   __asm__ volatile ("SWAP\t%0,TXMASKI\t/* (*TBI_INTSX OK) */" :           \
                     "=r" (__TRValue) : "0" (__TRValue) );                 \
   TrigState = __TRValue;                                       } while (0)

#define TBI_UNLOCK( TrigState )                                        do {\
   int __TRValue = TrigState;                                              \
   int __ALOCKHI = LINSYSEVENT_WR_ATOMIC_LOCK & 0xFFFF0000;                \
   __asm__ volatile ("SETD\t[%1+#0x00],D1RtP\t/* (*TBI_UNLOCK ... */\n\t"  \
                     "LOCK0\t\t/* ... */\n\t"                              \
                     "MOV\tTXMASKI,%0\t/* ... OK) */" :                    \
                     : "r" (__TRValue), "u" (__ALOCKHI) );      } while (0)

#define TBI_CRITOFF( TrigState )                                       do {\
   int __TRValue = TrigState;                                              \
   __asm__ volatile ("LOCK0\t\t/* (*TBI_CRITOFF ... */\n\t"                \
                     "MOV\tTXMASKI,%0\t/* ... OK) */" :                    \
                     : "r" (__TRValue) );                       } while (0)

#define TBI_TRIGSX( SrcDst ) do { TBI_SWAPREG( TXMASK, SrcDst );} while (0)

/*                                                              */
#define TBI_INTSOR( Bits )                                              do {\
    int __TT = 0; TBI_INTSX(__TT);                                          \
    __TT |= (Bits); TBI_INTSX(__TT);                             } while (0)
    
#define TBI_INTSAND( Bits )                                             do {\
    int __TT = 0; TBI_INTSX(__TT);                                          \
    __TT &= (Bits); TBI_INTSX(__TT);                             } while (0)

#ifdef TBI_1_4
#define TBI_DEFRICTRLSOR( Bits )                                        do {\
    int __TT = TBI_GETREG( CT.20 );                                         \
    __TT |= (Bits); TBI_SETREG( CT.20, __TT);                    } while (0)
    
#define TBI_DEFRICTRLSAND( Bits )                                       do {\
    int __TT = TBI_GETREG( TXDEFR );                                        \
    __TT &= (Bits); TBI_SETREG( CT.20, __TT);                    } while (0)
#endif

#define TBI_TRIGSOR( Bits )                                             do {\
    int __TT = TBI_GETREG( TXMASK );                                        \
    __TT |= (Bits); TBI_SETREG( TXMASK, __TT);                   } while (0)
    
#define TBI_TRIGSAND( Bits )                                            do {\
    int __TT = TBI_GETREG( TXMASK );                                        \
    __TT &= (Bits); TBI_SETREG( TXMASK, __TT);                   } while (0)

/*                                                                       
                                                                          */
#define TBI_STOPINTS( Value )                                           do {\
    int __TT = TBI_GETREG( TXMASKI );                                       \
    __TT &= TXSTATI_BGNDHALT_BIT; TBI_INTSX( __TT );                        \
    Value = __TT;                                                } while (0)
#define TBI_RESTINTS( Value )                                           do {\
    int __TT = Value; TBI_INTSX( __TT );                         } while (0)

/*                                                           */
PTBISEG __TBISegList( void );

/*                                                                  */
PTBISEG __TBIFindSeg( PTBISEG pStart, int Id );

/*                                                                 */
PTBISEG __TBINewSeg( PTBISEG pFromSeg, int Id, unsigned int Bytes );

/*                                                                         */
PTBISEG __TBIMakeNewSeg( int Id, unsigned int Bytes );

/*                                                                          */
void __TBIAddSeg( PTBISEG pSeg );
#define __TBIADDSEG_DEF     /*                                     */

/*                                                              */
int __TBIThreadId( void );

/*                                            */
TBIRES __TBIThrdPrivId( void );

/*                                                  
                                                               */
PTBI __TBI( int Id );

/*                                                                         
                         */
int __TBIPoll( PTBISPIN pLock, int Mask );

/*                                                                      */
int __TBISpin( PTBISPIN pLock, int Mask );

/*                                                                         */
TBIRES __TBIUnExpXXX( TBIRES State, int SigNum,
                   int Triggers, int Inst, PTBI pTBI );

/*                                                                          
                                                                            
                                                                              
                                                                              
                                                                      
                                                                             
                                                                        
                                             */
TBIRES __TBISyncTrigger( TBIRES State, int Id );

/*                                                                         
                                                                        
                                                                   
                                                                        
                                                                            
                                                                             
                                                                         
                                                                          
                                                                      
                                                                        
                        */
TBIRES __TBIASyncTrigger( TBIRES State );

/*                                                                             
                                                                           
                                                                          
                                                                           
                                                                           
                                                                             
                                                                           
                       */
TBIRES __TBISwitch( TBIRES NextThread, PTBICTX *rpSaveCtx );

/*                                                                        
                                                                          
                                                                   
                                                                           
                                                          */
typedef int (*PTBIMAINFN)( TBIRES Arg /*                              */ );
PTBICTX __TBISwitchInit( void *pStack, PTBIMAINFN fnMain, ... );

/*                                                                    
                                                                           
                                                                          
                                                                           
                                                                             
                                                                      
                                                                      
                                                                  */
void __TBISyncResume( TBIRES NextThread, int TrigsMask );

/*                                                               
                    */
void *__TBICtxSave( TBIRES State, void *pExt );
void *__TBICtxRestore( TBIRES State, void *pExt );

#ifdef TBI_1_4
#ifdef TBI_FASTINT_1_4
/*                                                              
                                        
 */
PTBICTXGP __TBICtx2SaveCrit( PTBICTX2 pCurrentCtx, PTBICTX2 pSaveCtx );
void *__TBICtx2SaveGP( PTBICTXGP pCurrentCtxGP, PTBICTXGP pSaveCtxGP );

/*                                                               
                    */
void *__TBICtx2Save( PTBICTXGP pCtxGP, short SaveMask, void *pExt );
void *__TBICtx2Restore( PTBICTX2 pCtx, short SaveMask, void *pExt );
#endif

/*                                                                            
                          */
void *__TBICtxFPUSave( TBIRES State, void *pExt );
void *__TBICtxFPURestore( TBIRES State, void *pExt );

#ifdef TBI_FASTINT_1_4
extern void *__TBICtx2FPUSave (PTBICTXGP, short, void*);
extern void *__TBICtx2FPURestore (PTBICTXGP, short, void*);
#endif
#endif

#ifdef TBI_1_4
/*                                                 */
void *__TBIDspramSaveA (short DspramSizes, void *pExt);
void *__TBIDspramSaveB (short DspramSizes, void *pExt);
void *__TBIDspramRestoreA (short DspramSizes, void *pExt);
void *__TBIDspramRestoreB (short DspramSizes, void *pExt);
#endif

/*                                                                       
                                                                             
                                                                             
                                                                            
                                                                            
                                                                            
                                                                            
                                                                             
                                                                             
   
                                                                          
                                                                           
                                                                      
                                                                         
                                                                        
                                                   
   
                                                                           
                                                                       
                                                                             
                     
   
                                                                       
                                                                            
                                                                               
                                                  */
TBIRES __TBINestInts( TBIRES State, void *pExt, int NoNestMask );

/*                                                                        
                                                                          
                                                                          
                                                                            
                                                                      */
void __TBIASyncResume( TBIRES State );

/*                                                                       
                                                                         
                                                           
                                                                          
                                                                        
                                                                             
                         */
void __TBIASyncThin( TBIRES State, TBIRES Thin );

/*                                                                           */
int __TBILock( void );

/*                                                                            */
void __TBIUnlock( int TrigState );

/*                                                                       
                                                                          
                                                 */
int __TBICritOn( void );
void __TBICritOff( int TrigState );

/*                                                          */
int __TBIIntsX( int NewMask );

/*                                                           */
int __TBITrigsX( int NewMask );

/*                                                                         
                                                                             
                                                                           
                                                                             
                                  
      
                                                                         
                                  */
void __TBITimerCtrl( int Id, int Wait );

/*                                                                           
                                             */
long long __TBITimeStamp( int Id );

/*                                                                        
                                                                           
                                                                             
                                                                             
                                                                     
                                                                       
                                                                             
                                                                     */
int __TBITimerAdd( int Id, int Wait );

/*                                                                      
                                                                            
                                                                          
          */
const TBISTR *__TBIFindStr( const TBISTR *pStart,
                            const char *pStr, int MatchLen );

/*                                                                      
                                                                 
                                                                              
                                                                          
                              */ 
const void *__TBITransStr( const char *pStr, int Len );



/*                                                                          
                                                     */
void *__TBIPhysAccess( int Channel, int PhysAddr, int Bytes );
void __TBIPhysRelease( int Channel, void *pLinAddr );

#ifdef METAC_1_0
/*                                                         */
#define TBIDCACHE_FLUSH( pAddr )
#define TBIDCACHE_PRELOAD( Type, pAddr ) ((Type) (pAddr))
#define TBIDCACHE_REFRESH( Type, pAddr ) ((Type) (pAddr))
#endif
#ifdef METAC_1_1
/*                                                                         */
#define TBIDCACHE_FLUSH( pAddr )          ((volatile char *) \
                 (((unsigned int) (pAddr))>>LINSYSLFLUSH_S))[0] = 0

extern void * __builtin_dcache_preload (void *);

/*                                                                      */
#define TBIDCACHE_PRELOAD( Type, Addr )                                    \
  ((Type) __builtin_dcache_preload ((void *)(Addr)))

extern void * __builtin_dcache_refresh (void *);

/*                                                                   */
#define TBIDCACHE_REFRESH( Type, Addr )                   __extension__ ({ \
  Type __addr = (Type)(Addr);                                              \
  (void)__builtin_dcache_refresh ((void *)(((unsigned int)(__addr))>>6));  \
  __addr; })

#endif
#ifndef METAC_1_0
#ifndef METAC_1_1
/*                            */
extern void __builtin_dcache_flush (void *);

/*                                                                         */
#define TBIDCACHE_FLUSH( Addr )                                            \
  __builtin_dcache_flush ((void *)(Addr))

extern void * __builtin_dcache_preload (void *);

/*                                                                      */
#define TBIDCACHE_PRELOAD( Type, Addr )                                    \
  ((Type) __builtin_dcache_preload ((void *)(Addr)))

extern void * __builtin_dcache_refresh (void *);

/*                                                                   */
#define TBIDCACHE_REFRESH( Type, Addr )                                    \
  ((Type) __builtin_dcache_refresh ((void *)(Addr)))

#endif
#endif

/*                      */
#define TBIMCACHE_FLUSH() { ((volatile int *) LINSYSCFLUSH_MMCU)[0] = 0; }

#ifdef METAC_2_1
/*                                                      */
#define TBIMTABLE_LEAFDATA(ADDR) TBIXCACHE_RD((int)(ADDR) & (-1<<6))

#ifndef __ASSEMBLY__
/*                                                           */
#define TBIMTABLE_DATA(ADDR) __extension__ ({ TBIRES __p;                     \
                                              __p.Val = TBIXCACHE_RL((int)(ADDR) & (-1<<6));   \
                                              __p; })
#endif
#endif

/*                                                      
                    
 */
#define _TBIMTABLE_LIN2PHYS(PHYS, LIN, LMASK) (void*)(((int)(PHYS)&0xFFFFF000)\
                                               +((int)(LIN)&(LMASK)))

/*                                        */
#define TBIMTABLE_LIN2PHYS(LEAFDATA, ADDR)                                    \
          (((LEAFDATA) & CRLINPHY0_VAL_BIT)                                   \
              ? _TBIMTABLE_LIN2PHYS(LEAFDATA, ADDR, 0x00000FFF)               \
              : 0)

/*                                                 */
void __TBIDumpSegListEntries( void );
void __TBILogF( const char *pFmt, ... );
void __TBIAssert( const char *pFile, int LineNum, const char *pExp );
void __TBICont( const char *pMsg, ... ); /*                                  */

/*                                              */
extern const char __TBISigNames[];
#endif /*                     */



/*                                                       */
#define TBI_SIGNAME_SCALE   4
#define TBI_SIGNAME_SCALE_S 2

#define TBI_1_3 

#ifdef TBI_1_3

#ifndef __ASSEMBLY__
#define TBIXCACHE_RD(ADDR)                                 __extension__ ({\
    void * __Addr = (void *)(ADDR);                                        \
    int __Data;                                                            \
    __asm__ volatile ( "CACHERD\t%0,[%1+#0]" :                             \
                       "=r" (__Data) : "r" (__Addr) );                     \
    __Data;                                                               })

#define TBIXCACHE_RL(ADDR)                                 __extension__ ({\
    void * __Addr = (void *)(ADDR);                                        \
    long long __Data;                                                      \
    __asm__ volatile ( "CACHERL\t%0,%t0,[%1+#0]" :                         \
                       "=d" (__Data) : "r" (__Addr) );                     \
    __Data;                                                               })

#define TBIXCACHE_WD(ADDR, DATA)                                      do {\
    void * __Addr = (void *)(ADDR);                                       \
    int __Data = DATA;                                                    \
    __asm__ volatile ( "CACHEWD\t[%0+#0],%1" :                            \
                       : "r" (__Addr), "r" (__Data) );          } while(0)

#define TBIXCACHE_WL(ADDR, DATA)                                      do {\
    void * __Addr = (void *)(ADDR);                                       \
    long long __Data = DATA;                                              \
    __asm__ volatile ( "CACHEWL\t[%0+#0],%1,%t1" :                        \
                       : "r" (__Addr), "r" (__Data) );          } while(0)

#ifdef TBI_4_0

#define TBICACHE_FLUSH_L1D_L2(ADDR)                                       \
  TBIXCACHE_WD(ADDR, CACHEW_FLUSH_L1D_L2)
#define TBICACHE_WRITEBACK_L1D_L2(ADDR)                                   \
  TBIXCACHE_WD(ADDR, CACHEW_WRITEBACK_L1D_L2)
#define TBICACHE_INVALIDATE_L1D(ADDR)                                     \
  TBIXCACHE_WD(ADDR, CACHEW_INVALIDATE_L1D)
#define TBICACHE_INVALIDATE_L1D_L2(ADDR)                                  \
  TBIXCACHE_WD(ADDR, CACHEW_INVALIDATE_L1D_L2)
#define TBICACHE_INVALIDATE_L1DTLB(ADDR)                                  \
  TBIXCACHE_WD(ADDR, CACHEW_INVALIDATE_L1DTLB)
#define TBICACHE_INVALIDATE_L1I(ADDR)                                     \
  TBIXCACHE_WD(ADDR, CACHEW_INVALIDATE_L1I)
#define TBICACHE_INVALIDATE_L1ITLB(ADDR)                                  \
  TBIXCACHE_WD(ADDR, CACHEW_INVALIDATE_L1ITLB)

#endif /*         */
#endif /*                     */

/* 
                                                                            
                                                                    
 */
#ifndef __ASSEMBLY__
#define METAG_LINPC( PCVal )                                              (\
    ( (TBI_GETREG(TXPRIVEXT) & TXPRIVEXT_MINIMON_BIT) != 0 ) ?           ( \
        ( ((PCVal) & 0x00900000) == 0x00900000 ) ?                         \
          (((PCVal) & 0xFFE00000) + (((PCVal) & 0x001FFFFC)>>1) + 1) :     \
        ( ((PCVal) & 0x00800000) == 0x00000000 ) ?                         \
          (((PCVal) & 0xFF800000) + (((PCVal) & 0x007FFFFC)>>1) + 1) :     \
                                                             (PCVal)   )   \
                                                                 : (PCVal) )
#define METAG_LINPC_X2BIT 0x00000001       /*                              */

/*                                                                       */
#define METAG_PCMINIM( LinVal )                                           (\
        (((LinVal) & 0x00980000) == 0x00880000) ?                          \
            (((LinVal) & 0xFFE00000) + (((LinVal) & 0x000FFFFE)<<1)) :     \
        (((LinVal) & 0x00C00000) == 0x00000000) ?                          \
            (((LinVal) & 0xFF800000) + (((LinVal) & 0x003FFFFE)<<1)) : 0   )

/*                                                                    */
#define METAG_PCLIN( LinVal )                              ( 0xFFFFFFFC & (\
        ( (LinVal & METAG_LINPC_X2BIT) != 0 ) ? METAG_PCMINIM( LinVal ) :  \
                                                               (LinVal)   ))

/*
                                                                             
                                                                      
 */
#define TBIMCACHE_TFLUSH(Thread)                                   do {\
    ((volatile int *)( LINSYSCFLUSH_TxMMCU_BASE            +           \
                      (LINSYSCFLUSH_TxMMCU_STRIDE*(Thread)) ))[0] = 0; \
                                                             } while(0)

/*
                                                                      
                                                                       
                                            
 */
#define TBIICACHE_FLUSH( pAddr ) TBIXCACHE_WD (pAddr, CACHEW_ICACHE_BIT)

/*                                                                         */
#define TBIMCACHE_AFLUSH( pAddr, SegType )                                \
    TBIXCACHE_WD(pAddr, CACHEW_TLBFLUSH_BIT + (                           \
                 ((SegType) == TBID_SEGTYPE_TEXT) ? CACHEW_ICACHE_BIT : 0 ))

/*
                                                                          
                                                                          
                                                                            
          
  
                                                                               
                                                                        
                                       
  
                                                                        
  
                                                                           
                                                                         
                                                                     
                                   
 */
int __TBIMMUCacheStride( const void *pStart, int Bytes );

/*
                                                                          
                                                                         
                                                                         
                                                                         
  
                                                                          
                                                                    
                                                                  
 */
void __TBIMMUCacheFlush( const void *pStart, int Bytes, int SegType );

/*
                                                                         
                                                                       
                                                                       
                                                                       
                                                                         
                                                    
   
                                                                             
                                                                     
  
                                                                           
                                                                              
                                                                             
                                                                           
                                                                          
                                                                           
                                                                  
 */
void *__TBICoreCacheOpt( const void *pStart, int Bytes, int SegType, int Mode );
void __TBICoreCacheEnd( const void *pOpt, int Bytes, int SegType );

/*
                                                                           
                                                                              
                                                                        
                                                                           
                                                                            
                                                                         
 */
void __TBIPhysOptim( int Channel, int IMode, int DMode );
void __TBIPhysFlush( int Channel, const void *pStart, int Bytes );
#endif
#endif /*               */

#endif /*                   */
