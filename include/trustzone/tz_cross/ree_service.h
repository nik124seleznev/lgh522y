
/*                                          */
/*                                                                  */

#ifndef __REE_SERVICE__
#define __REE_SERVICE__

#define REE_SERVICE_BUFFER_SIZE    128

enum ReeServiceCommand {
    REE_SERV_NONE = 0,
    REE_SERV_PUTS,                      //              
    REE_SERV_USLEEP,                    //         
    REE_SERV_MUTEX_CREATE, 
    REE_SERV_MUTEX_DESTROY, 
    REE_SERV_MUTEX_LOCK, 
    REE_SERV_MUTEX_UNLOCK,
    REE_SERV_MUTEX_TRYLOCK,
    REE_SERV_MUTEX_ISLOCK,
    REE_SERV_SEMAPHORE_CREATE, 
    REE_SERV_SEMAPHORE_DESTROY, 
    REE_SERV_SEMAPHORE_DOWN,
    REE_SERV_SEMAPHORE_DWNTO,           //                   
    REE_SERV_SEMAPHORE_TRYDWN,
    REE_SERV_SEMAPHORE_UP,
#if 0    
    REE_SERV_WAITQ_CREATE,
    REE_SERV_WAITQ_DESTROY,
    REE_SERV_WAITQ_WAIT,
    REE_SERV_WAITQ_WAITTO,    //                   
    REE_SERV_WAITQ_WAKEUP,
#endif    
    REE_SERV_REQUEST_IRQ,
    REE_SERV_ENABLE_IRQ,
    REE_SERV_ENABLE_CLOCK,
    REE_SERV_DISABLE_CLOCK,
    REE_SERV_THREAD_CREATE,
};

//                                      
struct ree_service_usleep
{
    unsigned int ustime;
};




#define MTEE_THREAD_NAME_NUM 32

struct REE_THREAD_INFO {
    uint32_t handle; //               
    char name[MTEE_THREAD_NAME_NUM]; //                        
};


#ifndef MTIRQF_NORMAL
//                                           
#define MTIRQF_SHARED          (1<<0)   //                           
#define MTIRQF_TRIGGER_LOW     (1<<1)   //                                
#define MTIRQF_TRIGGER_HIGH    (1<<2)   //                                 
#define MTIRQF_TRIGGER_RISING  (1<<3)   //                                 
#define MTIRQF_TRIGGER_FALLING (1<<4)   //                                  
#endif

struct ree_service_irq
{
    unsigned int irq;
    int enable;
    unsigned int flags;
    unsigned int token;
};

struct ree_service_clock
{
    unsigned int clk_id;
    char clk_name[112];
};

#endif /*                 */

