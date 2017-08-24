#ifndef MTD_UTILS_H
#define MTD_UTILS_H

/*                                                                         
                   
                                                                         */
#if defined(CONFIG_MTK_GPT_SCHEME_SUPPORT)    

typedef struct _MtdPart 
{
    char name[16];
    unsigned long long sz;    
    unsigned long long off;
    unsigned long long e_size;

} MtdPart;

#else

typedef struct _MtdPart 
{
    char name[16];
    unsigned int sz;    
    unsigned int off;
    unsigned int e_size;

} MtdPart;

#endif
/*                                                                         
                    
                                                                         */
/*                                     */
#define MAX_MTD_PARTITIONS              (25)

/*                       */
//                   
/*                        */
#define ROM_INFO_SEARCH_START           (0x0)
//     
//                                               
//      

/*                                                                         
                    
                                                                          */
extern MtdPart                          mtd_part_map[];

/*                                                                         
           
                                                                          */
char* mtd2pl (char* part_name);
char* pl2mtd (char* part_name);

#endif  //            
