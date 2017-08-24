#ifndef __CCCI_PMIC_H__
#define __CCCI_PMIC_H__

typedef enum
{
    PMIC6326_VSIM_ENABLE = 0,
    PMIC6326_VSIM_SET_AND_ENABLE = 1,
    PMIC6236_LOCK = 2,
    PMIC6326_UNLOCK = 3,
    PMIC6326_VSIM2_ENABLE = 4,
    PMIC6326_VSIM2_SET_AND_ENABLE = 5,
    PMIC6326_MAX
}pmic6326_ccci_op;

typedef enum
{
    PMIC6326_REQ = 0,        //                                       
    PMIC6326_RES = 1        //                                        
}pmic6326_ccci_type;

/*
                                               
                                                                             
                                                                    
                                                                         
*/

/*
                   
                                                                    
                                                                    
                                                         
*/

/*
                        
                                                              
                                                              
                                                
*/



typedef struct
{
    unsigned short    pmic6326_op;        //          
    unsigned short    pmic6326_type;        //                                  
    unsigned short    pmic6326_param1;
    unsigned short    pmic6326_param2;
}pmic6326_ccci_msg;

typedef struct
{
    unsigned int     pmic6326_exec_time;        //                                 
    unsigned short    pmic6326_param1;
    unsigned short    pmic6326_param2;
}pmic6326_ccci_msg_info;

/*
                     
                                                                 
                                                                 
                                                         
                                                                 
                                                
*/

typedef struct
{
    pmic6326_ccci_msg ccci_msg;
    pmic6326_ccci_msg_info ccci_msg_info;
}pmic6326_share_mem_info;

typedef struct
{
    pmic6326_ccci_msg      ccci_msg;
    pmic6326_ccci_msg_info ccci_msg_info;
} shared_mem_pmic_t;

int __init ccci_pmic_init(void);
void __exit ccci_pmic_exit(void);

#define CCCI_PMIC_SMEM_SIZE sizeof(shared_mem_pmic_t)
#endif //                
