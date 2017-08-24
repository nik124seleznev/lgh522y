/* 
        
              
  
                
          
  
                    
                                                  
  
               
                           
  
                       
                  
              
  
 */

#ifndef _SP5_OAL_H_
#define _SP5_OAL_H_

//              
#include "vcodec_OAL_v2.h"

//              
#include "val_types_private.h"

/* 
                   
                   
                   
                                          
 */
typedef enum ___VAL_OAL_TYPE
{
    _BYTE_  = 0x5000,     //       
    _WORD_,               //       
    _LONG_                //       
} __VAL_OAL_TYPE;


#define  SP5_VCodecQueryMemType                       VCodecDrvQueryMemType                     //                                                       
#define  SP5_VCodecQueryPhysicalAddr                  VCodecDrvQueryPhysicalAddr                //                                                            
#define  SP5_VCodecSwitchMemType                      VCodecDrvSwitchMemType                    //                                                        
#define  SP5_VCodecFlushCachedBuffer                  VCodecDrvFlushCachedBuffer                //                                                            
#define  SP5_VCodecInvalidateCachedBuffer             VCodecDrvInvalidateCachedBuffer           //                                                                 
#define  SP5_VCodecFlushCachedBufferAll               VCodecDrvFlushCachedBufferAll             //                                                               
#define  SP5_VCodecInvalidateCachedBufferAll          VCodecDrvInvalidateCachedBufferAll        //                                                                    
#define  SP5_VCodecFlushInvalidateCacheBufferAll      VCodecDrvFlushInvalidateCacheBufferAll    //                                                                        
#define  SP5_VCodecMemSet                             VCodecDrvMemSet                           //                                                 
#define  SP5_VCodecMemCopy                            VCodecDrvMemCopy                          //                                                  
#define  SP5_VCodecAssertFail                         VCodecDrvAssertFail                       //                                                     
#define  SP5_VCodecMMAP                               VCodecDrvMMAP                             //                                               
#define  SP5_VCodecUnMMAP                             VCodecDrvUnMMAP                           //                                                 
#define  SP5_VCodecWaitISR                            VCodecDrvWaitISR                          //                                                  
#define  SP5_VCodecLockHW                             VCodecDrvLockHW                           //                                                 
#define  SP5_VCodecUnLockHW                           VCodecDrvUnLockHW                         //                                                   
#define  SP5_VCodecInitHWLock                         VCodecDrvInitHWLock                       //                                                     
#define  SP5_VCodecDeInitHWLock                       VCodecDrvDeInitHWLock                     //                                                       
#if 0
#define  SP5_VcodecTraceLog0                          VCodecDrvTraceLog0                        //                                                    
#define  SP5_VcodecTraceLog1                          VCodecDrvTraceLog1                        //                                                    
#define  SP5_VcodecTraceLog2                          VCodecDrvTraceLog2                        //                                                    
#define  SP5_VcodecTraceLog4                          VCodecDrvTraceLog4                        //                                                    
#define  SP5_VcodecTraceLog8                          VCodecDrvTraceLog8                        //                                                    
#else
#define  SP5_VCodecPrintf                             VCodecPrintf                              //                                              
#endif
#define  SP5_VdoMemAllocAligned                       VCodecDrvMemAllocAligned                  //                                                          
#define  SP5_VdoMemFree                               VCodecDrvMemFree                          //                                                  
#define  SP5_VdoIntMalloc                             VCodecDrvIntMalloc                        //                                                    
#define  SP5_VdoIntFree                               VCodecDrvIntFree                          //                                                  
#define  SP5_RegSync                                  VCodecDrvRegSync                          //                                                  
#define  SP5_RegSyncWriteB                            VCodecDrvRegSyncWriteB                    //                                                        
#define  SP5_RegSyncWriteW                            VCodecDrvRegSyncWriteW                    //                                                        
#define  SP5_RegSyncWriteL                            VCodecDrvRegSyncWriteL                    //                                                        
#define  SP5_VMPEG4EncCodecWaitISR                    VMPEG4EncCodecDrvWaitISR                  //                                                          
#define  SP5_VMPEG4EncCodecLockHW                     VMPEG4EncCodecDrvLockHW                   //                                                         
#define  SP5_VMPEG4EncCodecUnLockHW                   VMPEG4EncCodecDrvUnLockHW                 //                                                           


/* 
                
                           
                   
                                                               
         
                                                          
         
                                               
         
                                             
               
         
 */
void SP5_VCodecQueryMemType(IN void            *pBuffer_VA,
                            IN unsigned int    u4Size,
                            OUT VCODEC_MEMORY_TYPE_T *peMemType);


/* 
                
                                
                   
                                                
         
                                                          
         
                                                  
               
         
 */
void SP5_VCodecQueryPhysicalAddr(IN void       *pBuffer_VA,
                                 OUT void     **pBufferOut_PA);


/* 
                
                            
                   
                                                                
         
                                                          
         
                                               
         
                                            
         
                                                           
               
                                                  
 */
int SP5_VCodecSwitchMemType(IN void            *pBuffer_VA,
                            IN unsigned int    u4Size,
                            IN VCODEC_MEMORY_TYPE_T eMemType,
                            OUT void           **pBufferOut_VA);


/* 
                
                                
                   
                                             
         
                                                          
         
                                               
               
         
 */
void SP5_VCodecFlushCachedBuffer(IN void         *pBuffer_VA,
                                 IN unsigned int u4Size);


/* 
                
                                     
                   
                                                  
         
                                                          
         
                                               
               
         
 */
void SP5_VCodecInvalidateCachedBuffer(IN void         *pBuffer_VA,
                                      IN unsigned int   u4Size);


/* 
                
                                   
                   
                                         
               
         
 */
void SP5_VCodecFlushCachedBufferAll();


/* 
                
                                        
                   
                                              
               
         
 */
void SP5_VCodecInvalidateCachedBufferAll();


/* 
                
                                            
                   
                                                      
               
         
 */
void SP5_VCodecFlushInvalidateCacheBufferAll();


/* 
                
                     
                   
                                    
         
                                                          
         
                                                            
         
                                                             
               
         
 */
void  SP5_VCodecMemSet(IN void                *pBuffer_VA,
                       IN char                cValue,
                       IN unsigned int        u4Length);


/* 
                
                      
                   
                                     
         
                                                              
         
                                                         
         
                                                                
               
         
 */
void  SP5_VCodecMemCopy(IN void             *pvDest ,
                        IN const void      *pvSrc ,
                        IN unsigned int      u4Length);


/* 
                
                         
                   
                                                 
         
                                                          
         
                                                          
         
                                                              
               
         
 */
void SP5_VCodecAssertFail(IN char *ptr,
                          IN int i4Line,
                          IN int i4Arg);


/* 
                
                   
                   
                                    
         
                                                                                  
               
         
 */
void SP5_VCodecMMAP(VCODEC_OAL_MMAP_T *prParam);


/* 
                
                     
                   
                                      
         
                                                                                    
               
         
 */
void SP5_VCodecUnMMAP(VCODEC_OAL_MMAP_T *prParam);


/* 
                
                      
                   
                                                                                      
         
                                                                               
               
                                                 
 */
int SP5_VCodecWaitISR(VCODEC_OAL_ISR_T *prParam);


/* 
                
                     
                   
                                                                   
         
                                                                             
               
                                                 
 */
int SP5_VCodecLockHW(VCODEC_OAL_HW_LOCK_T *prParam);


/* 
                
                       
                   
                                                                                    
         
                                                                               
               
                                                 
 */
int SP5_VCodecUnLockHW(VCODEC_OAL_HW_LOCK_T *prParam);


/* 
                
                         
                   
                                      
         
                                                                                  
               
         
 */
void SP5_VCodecInitHWLock(VCODEC_OAL_HW_REGISTER_T *prParam);


/* 
                
                           
                   
                                        
         
                                                                                    
               
         
 */
void SP5_VCodecDeInitHWLock(VCODEC_OAL_HW_REGISTER_T *prParam);


#if 0
/* 
                
                        
                   
                                             
         
                                                                        
         
                                                         
               
         
 */
void SP5_VcodecTraceLog0(IN VCODEC_LOG_GROUP_T eGroup,
                         IN VCODEC_LOG_INDEX_T eIndex
                        );


/* 
                
                        
                   
                                             
         
                                                                        
         
                                                         
         
                                               
               
         
 */
void SP5_VcodecTraceLog1(IN VCODEC_LOG_GROUP_T eGroup,
                         IN VCODEC_LOG_INDEX_T eIndex,
                         IN UINT64 arg
                        );


/* 
                
                        
                   
                                             
         
                                                                        
         
                                                         
         
                                                
         
                                                
               
         
 */
void SP5_VcodecTraceLog2(IN VCODEC_LOG_GROUP_T eGroup,
                         IN  VCODEC_LOG_INDEX_T eIndex,
                         IN  UINT64 arg1,
                         IN  UINT64 arg2
                        );


/* 
                
                        
                   
                                             
         
                                                                        
         
                                                         
         
                                                
         
                                                
         
                                                
         
                                                
               
         
 */
void SP5_VcodecTraceLog4(IN VCODEC_LOG_GROUP_T eGroup,
                         IN  VCODEC_LOG_INDEX_T eIndex,
                         IN  UINT64 arg1,
                         IN  UINT64 arg2, IN  UINT64 arg3,
                         IN  UINT64 arg4
                        );


/* 
                
                        
                   
                                             
         
                                                                        
         
                                                         
         
                                                
         
                                                
         
                                                
         
                                                
         
                                                
         
                                                
         
                                                
         
                                                
               
         
 */
void SP5_VcodecTraceLog8(IN VCODEC_LOG_GROUP_T eGroup,
                         IN  VCODEC_LOG_INDEX_T eIndex,
                         IN  UINT64 arg1,
                         IN  UINT64 arg2,
                         IN  UINT64 arg3,
                         IN UINT64 arg4,
                         IN  UINT64 arg5,
                         IN  UINT64 arg6,
                         IN  UINT64 arg7,
                         IN  UINT64 arg8
                        );
#else
/* 
                
                     
                   
                                             
         
                                       
         
                                            
 */
VCODEC_OAL_ERROR_T SP5_VCodecPrintf(IN const char *_Format, ...);
#endif


/* 
                
                           
                   
                                                      
         
                                                
         
                                                  
         
                                                            
         
                                        
         
                                                          
         
                                                                
               
               
 */
VAL_VOID_T SP5_VdoMemAllocAligned(VAL_VOID_T *handle, VAL_UINT32_T size, unsigned int u4AlignSize, VCODEC_MEMORY_TYPE_T cachable, VCODEC_BUFFER_T *pBuf, VAL_MEM_CODEC_T eMemCodec);


/* 
                
                   
                   
                                                     
         
                                                
         
                                                         
               
               
 */
VAL_VOID_T SP5_VdoMemFree(VAL_VOID_T *handle, VCODEC_BUFFER_T *pBuf);


/* 
                
                     
                   
                                                      
         
                                                
         
                                                  
         
                                                            
         
                                                          
               
               
 */
VAL_VOID_T SP5_VdoIntMalloc(HANDLE  handle, unsigned int size, unsigned int alignedsize, VCODEC_BUFFER_T *prBuffer_adr);


/* 
                
                   
                   
                                                     
         
                                                
         
                                                         
               
               
 */
VAL_VOID_T SP5_VdoIntFree(HANDLE handle, VCODEC_BUFFER_T *prBuffer_adr);


/* 
                
                
                   
                                           
         
                                           
         
                                           
         
                                             
               
               
 */
VAL_VOID_T SP5_RegSync(int type, unsigned int v, unsigned int a);


/* 
                
                              
                   
                                                                                      
         
                                                                               
               
                                                 
 */
int SP5_VMPEG4EncCodecWaitISR(VCODEC_OAL_ISR_T *prParam);


/* 
                
                             
                   
                                                                   
         
                                                                             
               
                                                 
 */
int SP5_VMPEG4EncCodecLockHW(VCODEC_OAL_HW_LOCK_T *prParam);


/* 
                
                               
                   
                                                                                    
         
                                                                               
               
                                                 
 */
int SP5_VMPEG4EncCodecUnLockHW(VCODEC_OAL_HW_LOCK_T *prParam);


/* 
                
             
                   
                             
         
                                                        
               
                                                                                                   
 */
VAL_RESULT_T eValInit(VAL_HANDLE_T *a_phHalHandle);


/* 
                
               
                   
                               
         
                                                        
               
                                                                                                   
 */
VAL_RESULT_T eValDeInit(VAL_HANDLE_T *a_phHalHandle);


/* 
                
                           
                   
                                                     
         
                                                          
               
                                                  
 */
int VCodecDrvCheck_Version(int version);

/*                                             */

/*                                      */


/* 
                
                               
                   
                                           
         
                                  
               
                                                    
 */
int VCodecDrvPthread_attr_init(OUT VCODEC_PTHREAD_ATTR_T *attr);


/* 
                
                                  
                   
                                              
         
                                 
               
                                                       
 */
int VCodecDrvPthread_attr_destroy(IN VCODEC_PTHREAD_ATTR_T *attr);


/* 
                
                                         
                   
                                                     
         
                                 
         
                                         
               
                                                                                 
 */
int VCodecDrvPthread_attr_getdetachstate(IN const VCODEC_PTHREAD_ATTR_T *attr, OUT int *detachstate);


/* 
                
                                         
                   
                                                     
         
                                 
         
                                         
               
                                                                                 
 */
int VCodecDrvPthread_attr_setdetachstate(IN VCODEC_PTHREAD_ATTR_T *attr, IN  int detachstate);


/* 
                
                            
                   
                                        
         
                                    
         
                                 
         
                                          
         
                                
               
                                                                                                
 */
int VCodecDrvPthread_create(OUT VCODEC_PTHREAD_T *thread, IN  const VCODEC_PTHREAD_ATTR_T *attr, IN  void * (*start_routine)(void *), IN  void *arg);


/* 
                
                          
                   
                                      
         
                                
         
                                
               
                                                
 */
int VCodecDrvPthread_kill(IN VCODEC_PTHREAD_T tid, IN  int sig);


/* 
                
                          
                   
                                      
         
                                    
               
         
 */
void VCodecDrvPthread_exit(OUT void *retval);


/* 
                
                          
                   
                                      
         
                                 
         
                                     
               
                                                 
 */
int VCodecDrvPthread_join(IN  VCODEC_PTHREAD_T thid, OUT void **ret_val);

//                                                       


/* 
                
                          
                   
                                      
         
                                         
         
                                         
               
                                                                     
 */
int VCodecDrvPthread_once(IN VCODEC_PTHREAD_ONCE_T  *once_control, IN void (*init_routine)(void));


/* 
                
                          
                   
                                      
               
                                                       
 */
VCODEC_PTHREAD_T VCodecDrvPthread_self(void);

//                                                                                            

/*                          */


/* 
                
                                    
                   
                                                
         
                                  
               
                                                              
 */
int VCodecDrvPthread_mutexattr_init(OUT VCODEC_PTHREAD_MUTEXATTR_T *attr);


/* 
                
                                       
                   
                                                   
         
                                 
               
                                                                 
 */
int VCodecDrvPthread_mutexattr_destroy(IN VCODEC_PTHREAD_MUTEXATTR_T *attr);


/* 
                
                                
                   
                                            
         
                                   
         
                                 
               
                                                                                          
 */
int VCodecDrvPthread_mutex_init(OUT VCODEC_PTHREAD_MUTEX_T *mutex, IN  const VCODEC_PTHREAD_MUTEXATTR_T *attr);


/* 
                
                                   
                   
                                               
         
                                  
               
                                                          
 */
int VCodecDrvPthread_mutex_destroy(IN VCODEC_PTHREAD_MUTEX_T *mutex);


/* 
                
                                
                   
                                            
         
                                  
               
                                                       
 */
int VCodecDrvPthread_mutex_lock(IN VCODEC_PTHREAD_MUTEX_T *mutex);


/* 
                
                                  
                   
                                              
         
                                  
               
                                                         
 */
int VCodecDrvPthread_mutex_unlock(IN VCODEC_PTHREAD_MUTEX_T *mutex);


/* 
                
                                   
                   
                                               
         
                                  
               
                                                          
 */
int VCodecDrvPthread_mutex_trylock(IN VCODEC_PTHREAD_MUTEX_T *mutex);

/*                         */


/* 
                
                               
                   
                                           
         
                                  
         
                                    
               
                           
 */
int VCodecDrvPthread_spin_init(OUT VCODEC_PTHREAD_SPINLOCK_T *lock, IN  int pshared);


/* 
                
                                  
                   
                                              
         
                                 
               
                           
 */
int VCodecDrvPthread_spin_destroy(IN VCODEC_PTHREAD_SPINLOCK_T *lock);


/* 
                
                               
                   
                                           
         
                                 
               
                           
 */
int VCodecDrvPthread_spin_lock(IN VCODEC_PTHREAD_SPINLOCK_T *lock);


/* 
                
                                  
                   
                                              
         
                                 
               
                           
 */
int VCodecDrvPthread_spin_trylock(IN VCODEC_PTHREAD_SPINLOCK_T *lock);


/* 
                
                                 
                   
                                             
         
                                 
               
                           
 */
int VCodecDrvPthread_spin_unlock(IN VCODEC_PTHREAD_SPINLOCK_T *lock);

/*                                       */


/* 
                
                                   
                   
                                               
         
                                  
               
                                                            
 */
int VCodecDrvPthread_condattr_init(OUT VCODEC_PTHREAD_CONDATTR_T *attr);


/* 
                
                                      
                   
                                                  
         
                                 
               
                                                               
 */
int VCodecDrvPthread_condattr_destroy(IN VCODEC_PTHREAD_CONDATTR_T *attr);


/* 
                
                               
                   
                                           
         
                                  
         
                                 
               
                                                                                      
 */
int VCodecDrvPthread_cond_init(OUT VCODEC_PTHREAD_COND_T *cond, IN  const VCODEC_PTHREAD_CONDATTR_T *attr);


/* 
                
                                  
                   
                                              
         
                                 
               
                                                       
 */
int VCodecDrvPthread_cond_destroy(IN VCODEC_PTHREAD_COND_T *cond);


/* 
                
                                    
                   
                                                
         
                                 
               
                                                         
 */
int VCodecDrvPthread_cond_broadcast(IN VCODEC_PTHREAD_COND_T *cond);


/* 
                
                                 
                   
                                             
         
                                 
               
                                                      
 */
int VCodecDrvPthread_cond_signal(IN VCODEC_PTHREAD_COND_T *cond);


/* 
                
                                 
                   
                                           
         
                                 
         
                                  
               
                                                                              
 */
int VCodecDrvPthread_cond_wait(IN VCODEC_PTHREAD_COND_T *cond, IN  VCODEC_PTHREAD_MUTEX_T *mutex);

/*                                                    */

/*                              */


/* 
                
                       
                   
                                  
         
                                
         
                                    
         
                                  
               
                                                
 */
int VCodecDrv_sem_init(IN VCODEC_OAL_SEM_T *sem, IN int pshared, IN unsigned int value);


/* 
                
                          
                   
                                     
         
                                
               
                                   
 */
int VCodecDrv_sem_destroy(IN VCODEC_OAL_SEM_T *sem);


/* 
                
                       
                   
                                  
         
                                
               
                                
 */
int VCodecDrv_sem_post(IN VCODEC_OAL_SEM_T *sem);


/* 
                
                       
                   
                                  
         
                                
               
                                
 */
int VCodecDrv_sem_wait(IN VCODEC_OAL_SEM_T *sem);

/*                            */


/* 
                
                         
                   
                                                                       
         
                                         
         
                                              
               
                                                                                                         
 */
VCODEC_OAL_ERROR_T VCodecDrvBindingCore(IN  VCODEC_PTHREAD_T ThreadHandle, IN  unsigned int u4Mask);


/* 
                
                           
                   
                                                                         
         
                                         
               
                                                                                                         
 */
VCODEC_OAL_ERROR_T VCodecDrvDeBindingCore(IN  VCODEC_PTHREAD_T ThreadHandle);


/* 
                
                         
                   
                                                                            
         
                                         
         
                                      
         
                                          
               
                                                                                                         
 */
VCODEC_OAL_ERROR_T VCodecDrvGetAffinity(IN  VCODEC_PTHREAD_T ThreadHandle, OUT  unsigned int *pu4Mask, OUT  unsigned int *pu4SetMask);


/* 
                
                         
                   
                                                       
         
                                   
         
                                         
               
                                                                                                         
 */
VCODEC_OAL_ERROR_T VCodecDrvCoreLoading(IN  int s4CPUid, OUT int *ps4Loading);


/* 
                
                         
                   
                                                   
         
                                        
               
                                                                                                         
 */
VCODEC_OAL_ERROR_T VCodecDrvCoreNumber(OUT int *ps4CPUNums);


/* 
                
                   
                   
                                       
         
                                     
               
         
 */
void VCodecDrvSleep(IN unsigned int u4Tick);


/* 
                
                        
                   
                                                                                       
         
                                            
         
                                              
               
                                                  
 */
int OAL_SMP_BindingCore(int aCurrentTid, int aCPUid);   //                            

/*                        */


/* 
                
                        
                   
                                         
         
                                        
         
                                         
         
                                          
               
                                                                                                                                     
 */
VCODEC_OAL_ERROR_T VCodecConfigMCIPort(IN unsigned int u4PortConfig, OUT unsigned int *pu4PortResult, IN VCODEC_CODEC_TYPE_T eCodecType);


/*                                      */


/* 
                
                                
                   
                                                                        
         
                                                
         
                                                  
         
                                                            
         
                                        
         
                                                          
               
         
 */
void VCodecDrvMemAllocAligned_NC(IN HANDLE hDrv, IN unsigned int u4Size, unsigned int u4AlignSize, IN VCODEC_MEMORY_TYPE_T fgCacheable, OUT VCODEC_BUFFER_T *prBuf);


/* 
                
                        
                   
                                                     
         
                                                
         
                                                         
               
         
 */
void VCodecDrvMemFree_NC(IN HANDLE hDrv, IN VCODEC_BUFFER_T *prBuf);


/* 
                
                       
                   
                                    
         
                                                
         
                                            
         
                                        
               
         
 */
VCODEC_DEC_ERROR_T VDecCodecQueryInfo(IN HANDLE hDrv, IN VCODEC_DEC_QUERY_INFO_TYPE_T ID, OUT void *pvQueryData);
#if 0
//      

#include "mach/sync_write.h"

#define SP5_REGSYNC_WriteB(v, a) \
    mt65xx_reg_sync_writeb(v, a);

#define SP5_REGSYNC_WriteW(v, a) \
    mt65xx_reg_sync_writew(v, a);

#define SP5_REGSYNC_WriteL(v, a) \
    mt65xx_reg_sync_writel(v, a);


VAL_VOID_T SP5_RegSyncWriteB(VAL_UINT32_T v, VAL_UINT32_T a);
VAL_VOID_T SP5_RegSyncWriteW(VAL_UINT32_T v, VAL_UINT32_T a);
VAL_VOID_T SP5_RegSyncWriteL(VAL_UINT32_T v, VAL_UINT32_T a);
#endif
#endif
