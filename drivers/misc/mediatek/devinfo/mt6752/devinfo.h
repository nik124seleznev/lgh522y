#ifndef DEVFINO_H
#define DEVFINO_H

 /*                                                                            
                    
                                                                             */
#define MODULE_NAME     "[devinfo]"
#define DEV_NAME        "devmap"
#define MAJOR_DEV_NUM    196
 /*                                                                            
                   
                                                                             */
#define DEV_IOC_MAGIC       'd'
#define READ_DEV_DATA       _IOR(DEV_IOC_MAGIC,  1, unsigned int)
#define DEV_IOC_MAXNR       (10)

#ifdef CONFIG_OF
/*                       */
#define DEVINFO_MAX_SIZE 50
struct devinfo_tag {
    u32 size;
    u32 tag;
    u32 data[DEVINFO_MAX_SIZE];    /*                    */
    u32 data_size;  /*                         */
};
#endif

#endif /*                  */

