#ifndef __MD32_IRQ_H__
#define __MD32_IRQ_H__

#define MD32_MAX_USER  20
//                                

/*                    */
#define MD32_IPC_INT 0x100
#define WDT_INT 0x200
#define PMEM_DISP_INT 0x400
#define DMEM_DISP_INT 0x800
/*                        */
//                          
//                           

typedef struct
{
    void (*wdt_func[MD32_MAX_USER]) (void *);
    void (*reset_func[MD32_MAX_USER]) (void *);
    char MODULE_NAME[MD32_MAX_USER][100];
    void *private_data[MD32_MAX_USER];
    int in_use[MD32_MAX_USER];
} md32_wdt_func;

typedef struct
{
    void (*assert_func[MD32_MAX_USER]) (void *);
    void (*reset_func[MD32_MAX_USER]) (void *);
    char MODULE_NAME[MD32_MAX_USER][100];
    void *private_data[MD32_MAX_USER];
    int in_use[MD32_MAX_USER];
} md32_assert_func;

extern  irqreturn_t md32_irq_handler(int irq, void *dev_id);

#endif /*                */
