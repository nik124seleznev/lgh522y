#ifndef __CMDQSECTL_API_H__
#define __CMDQSECTL_API_H__

/* 
                                                                
 */
#define CMD_CMDQ_TL_SUBMIT_TASK	  1
#define CMD_CMDQ_TL_RES_RELEASE	  2  //                                                      
#define CMD_CMDQ_TL_CANCEL_TASK	  3
#define CMD_CMDQ_TL_PATH_RES_ALLOCATE 4 //                                      
#define CMD_CMDQ_TL_PATH_RES_RELEASE  5 //                                         

#define CMD_CMDQ_TL_TEST_HELLO_TL	(4000) //                                
#define CMD_CMDQ_TL_TEST_DUMMY	  (4001) //                                              
#define CMD_CMDQ_TL_TEST_SMI_DUMP	(4002)
#define CMD_CMDQ_TL_TRAP_DR_INFINITELY (4004)
#define CMD_CMDQ_TL_DUMP (4005)

#define CMD_CMDQ_TL_SECTRACE_MAP      (3000)
#define CMD_CMDQ_TL_SECTRACE_UNMAP    (3001)
#define CMD_CMDQ_TL_SECTRACE_TRANSACT (3002)


/* 
                    
 */
#define EXIT_ERROR				  ((uint32_t)(-1))

/* 
                                              
 */

/* 
                 
                                            
 */
#define TL_CMDQ_UUID { { 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }

#endif //                      
