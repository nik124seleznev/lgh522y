#ifndef __DISP_MGR_H
#define __DISP_MGR_H

#define MAX_INPUT_CONFIG 		4
#define MAX_QUEUE_BUFFER_COUNT 	3
#define UNKNOWN_GROUP_ID		0xff

typedef unsigned int UINT;
typedef unsigned char BOOL;

#define MAKE_DISP_SESSION(mode, type, dev) (UINT)((mode) << 24 | (type)<<16 | (dev))
#define DISP_SESSION_MODE(id) (((id)>>24)&0xff)
#define DISP_SESSION_TYPE(id) (((id)>>16)&0xff)
#define DISP_SESSION_DEV(id) ((id)&0xff)

//                                                                              
//                                            
//                                                              
//                            
struct disp_session_config_t;
struct disp_session_input_config_t;
struct disp_session_output_config_t;

//                                                                              
//                       
//                            
typedef enum {
   DCP_STATUS_OK = 0,

   DCP_STATUS_NOT_IMPLEMENTED,
   DCP_STATUS_ALREADY_EXIST,
   DCP_STATUS_DONT_EXIST,
   DCP_STATUS_INVALID_PARAM,
   DCP_STATUS_ERROR,
} DCP_STATUS;

typedef enum {
	FREE,
	ACTIVE,
	QUEUED,
	ACQUIRED,
	DONE,
}JOB_STATUS;

typedef struct disp_buffer_queue_t {
	spinlock_t lock;
	UINT write_slot;
	UINT read_slot;
	UINT reserved;
	UINT buffer_count;
	UINT buffer_queue[MAX_QUEUE_BUFFER_COUNT];
}disp_buffer_queue;

typedef struct input_config_t {
	UINT layer_id;
	UINT layer_enable;
	UINT index;
	UINT format;
	UINT src_x;
	UINT src_y;
	UINT dst_x;
	UINT dst_y;
	UINT width;
	UINT height;
	UINT pitch;
	UINT address;
	UINT alpha_enable;
	UINT alpha;
	UINT security;
	UINT color_key_enable;
	UINT color_key;
	UINT dirty;
}input_config;

typedef struct output_config_t {
	UINT layer_id;
	UINT index;
	UINT format;
	UINT x;
	UINT y;
	UINT width;
	UINT height;
	UINT pitch;
	UINT pitchUV;
	UINT address;
	UINT security;
	UINT dirty;
}output_config;

typedef struct disp_job_t {
	struct list_head list;
	struct mutex lock;
	JOB_STATUS status;
	//                         
	UINT group_id;
	input_config  input[MAX_INPUT_CONFIG];
	output_config output;
}disp_job;

//                                                                              
//                      
//                            

//                                                                              
//                                                                
/* 
                                                    
 */
DCP_STATUS disp_create_session (struct disp_session_config_t *config);
/* 
                                             
 */
DCP_STATUS disp_destroy_session (struct disp_session_config_t *config);
/* 
                                                       
 */
DCP_STATUS disp_set_session_input (struct disp_session_input_config_t *input);
/* 
                                                             
 */
DCP_STATUS disp_set_session_output (struct disp_session_output_config_t *input);
//   

/* 
                                                                               
                                       
                              
  
                                                           
 */
disp_job* disp_deque_job (UINT gid);

/* 
                                                      
                              
  
                                                                  
 */
DCP_STATUS disp_enque_job(UINT gid);


/* 
                                                  
  
                                                            
 */
disp_job* disp_acquire_job (void);

/* 
                                                         
  
                                                    
 */
UINT disp_release_job (void);

/* 
                                        
                                
                                                      
  
                                                          
 */
disp_job* disp_query_job (void);

/* 
                                            
  
                                      
 */
disp_job* disp_recycle_job(BOOL all);

/* 
                                           
                                       
                              
  
                                                                  
 */
DCP_STATUS disp_cancel_job(UINT gid);

/* 
                                                                 
                             
 
                                                                                  
*/
DCP_STATUS disp_check_todo_job(UINT gid);


/* 
                                                     
                       
                                               
                                          
                           
  
                                                           
                                                                     
                   
 */
DCP_STATUS disp_buffer_queue_init (disp_buffer_queue *que, UINT address[], UINT cnt);

/* 
                                                               
                        
                                                 
                                          
                           
  
                                                           
                                                                     
                   
 */
DCP_STATUS disp_buffer_queue_init_continous (disp_buffer_queue *que, UINT address, UINT size, UINT cnt);

/* 
                                                      
 */
UINT disp_deque_buffer (disp_buffer_queue *que);

/* 
                               
 */
void disp_enque_buffer (disp_buffer_queue *que);

/* 
                                              
 */
BOOL disp_acquire_buffer (disp_buffer_queue *que);

/* 
                                   
 */
void disp_release_buffer (disp_buffer_queue *que);

/* 
                                           
                                                 
 */
UINT disp_request_buffer (disp_buffer_queue *que);



#endif //            
