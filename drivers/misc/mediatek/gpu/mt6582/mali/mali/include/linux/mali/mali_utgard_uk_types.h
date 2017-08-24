/*
 * Copyright (C) 2010-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/* 
                        
                                                                    
 */

#ifndef __MALI_UTGARD_UK_TYPES_H__
#define __MALI_UTGARD_UK_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

/*                                                               */
#define MALI_UK_TIMELINE_GP   0
#define MALI_UK_TIMELINE_PP   1
#define MALI_UK_TIMELINE_SOFT 2
#define MALI_UK_TIMELINE_MAX  3

typedef struct {
	u32 points[MALI_UK_TIMELINE_MAX];
	s32 sync_fd;
} _mali_uk_fence_t;

/* 
                                                      
  
     
 */

/* 
                                                           
  
     
 */

/*                                  
      */

/*                                                                            
                     
  
                           */
typedef enum {
	_MALI_UK_CORE_SUBSYSTEM,      /*                           */
	_MALI_UK_MEMORY_SUBSYSTEM,    /*                             */
	_MALI_UK_PP_SUBSYSTEM,        /*                                         */
	_MALI_UK_GP_SUBSYSTEM,        /*                                       */
	_MALI_UK_PROFILING_SUBSYSTEM, /*                                */
	_MALI_UK_PMM_SUBSYSTEM,       /*                                              */
	_MALI_UK_VSYNC_SUBSYSTEM,     /*                            */
} _mali_uk_subsystem_t;

/*                                                                      
                                                               
  
                                           
                                                                                   
                                                               */
typedef enum {
	/*                 */

	_MALI_UK_OPEN                    = 0, /*                    */
	_MALI_UK_CLOSE,                       /*                     */
	_MALI_UK_WAIT_FOR_NOTIFICATION,       /*                                     */
	_MALI_UK_GET_API_VERSION,             /*                               */
	_MALI_UK_POST_NOTIFICATION,           /*                                 */
	_MALI_UK_GET_USER_SETTING,            /*                                *//*         */
	_MALI_UK_GET_USER_SETTINGS,           /*                                 *//*         */
	_MALI_UK_REQUEST_HIGH_PRIORITY,       /*                                     */
	_MALI_UK_TIMELINE_GET_LATEST_POINT,   /*                                         */
	_MALI_UK_TIMELINE_WAIT,               /*                             */
	_MALI_UK_TIMELINE_CREATE_SYNC_FENCE,  /*                                          */
	_MALI_UK_SOFT_JOB_START,              /*                              */
	_MALI_UK_SOFT_JOB_SIGNAL,             /*                               */

	/*                   */

	_MALI_UK_INIT_MEM                = 0,    /*                        */
	_MALI_UK_TERM_MEM,                       /*                        */
	_MALI_UK_GET_BIG_BLOCK,                  /*                             */
	_MALI_UK_FREE_BIG_BLOCK,                 /*                              */
	_MALI_UK_MAP_MEM,                        /*                        */
	_MALI_UK_UNMAP_MEM,                      /*                          */
	_MALI_UK_QUERY_MMU_PAGE_TABLE_DUMP_SIZE, /*                                                */
	_MALI_UK_DUMP_MMU_PAGE_TABLE,            /*                                       */
	_MALI_UK_ATTACH_DMA_BUF,                 /*                              */
	_MALI_UK_RELEASE_DMA_BUF,                /*                               */
	_MALI_UK_DMA_BUF_GET_SIZE,               /*                                */
	_MALI_UK_ATTACH_UMP_MEM,                 /*                              */
	_MALI_UK_RELEASE_UMP_MEM,                /*                               */
	_MALI_UK_MAP_EXT_MEM,                    /*                                */
	_MALI_UK_UNMAP_EXT_MEM,                  /*                                  */
	_MALI_UK_VA_TO_MALI_PA,                  /*                             */
	_MALI_UK_MEM_WRITE_SAFE,                 /*                              */

	/*                                 */

	_MALI_UK_START_JOB           = 0,     /*                                                   */
	_MALI_UK_GET_NUMBER_OF_CORES,         /*                                                     */
	_MALI_UK_GET_CORE_VERSION,            /*                                                                       */

	/*                                */

	_MALI_UK_PP_START_JOB            = _MALI_UK_START_JOB,            /*                            */
	_MALI_UK_GET_PP_NUMBER_OF_CORES  = _MALI_UK_GET_NUMBER_OF_CORES,  /*                                      */
	_MALI_UK_GET_PP_CORE_VERSION     = _MALI_UK_GET_CORE_VERSION,     /*                                   */
	_MALI_UK_PP_DISABLE_WB,                                           /*                                 */
	_MALI_UK_PP_AND_GP_START_JOB,                                     /*                                   */

	/*                              */

	_MALI_UK_GP_START_JOB            = _MALI_UK_START_JOB,            /*                            */
	_MALI_UK_GET_GP_NUMBER_OF_CORES  = _MALI_UK_GET_NUMBER_OF_CORES,  /*                                      */
	_MALI_UK_GET_GP_CORE_VERSION     = _MALI_UK_GET_CORE_VERSION,     /*                                   */
	_MALI_UK_GP_SUSPEND_RESPONSE,                                     /*                                   */

	/*                      */

	_MALI_UK_PROFILING_START         = 0, /*                                */
	_MALI_UK_PROFILING_ADD_EVENT,         /*                                    */
	_MALI_UK_PROFILING_STOP,              /*                               */
	_MALI_UK_PROFILING_GET_EVENT,         /*                                    */
	_MALI_UK_PROFILING_CLEAR,             /*                                */
	_MALI_UK_PROFILING_GET_CONFIG,        /*                                     */
	_MALI_UK_PROFILING_REPORT_SW_COUNTERS,/*                                             */

	/*                           */
	_MALI_UK_VSYNC_EVENT_REPORT      = 0, /*                                  */

} _mali_uk_functions;

/*                                               
  
                                        
 */
typedef struct {
	void *ctx;                      /*                                                    */
	u32 size;                       /*                                                                            */
} _mali_uk_get_system_info_size_s;


/*                                                      
      */

/* 
                                               
                                                          
  
                                                                           
                                                                              
                                                
 */
typedef u32 _mali_core_version;

/* 
                                                                
                                                                                                       
                                                                             
                                                                                             
  
                                                   
 */
typedef enum _mali_driver_mode {
	_MALI_DRIVER_MODE_RAW = 1,    /*                                 */
	_MALI_DRIVER_MODE_NORMAL = 2  /*                            */
} _mali_driver_mode;

/*                               
  
                                           */
typedef enum _mali_core_type {
	_MALI_GP2 = 2,                /*                                         */
	_MALI_200 = 5,                /*                                           */
	_MALI_400_GP = 6,             /*                                         */
	_MALI_400_PP = 7,             /*                                           */
	/*                                                        */
} _mali_core_type;


/*                                     
  
                                                                           
                                                                            
                                                                    
  
                                                                           
                                                                           
               
  
                      
 */
typedef enum _mali_bus_usage {

	_MALI_PP_READABLE   = (1<<0),  /*                                     */
	_MALI_PP_WRITEABLE  = (1<<1),  /*                                      */
	_MALI_GP_READABLE   = (1<<2),  /*                                   */
	_MALI_GP_WRITEABLE  = (1<<3),  /*                                    */
	_MALI_CPU_READABLE  = (1<<4),  /*                      */
	_MALI_CPU_WRITEABLE = (1<<5),  /*                       */
	_MALI_GP_L2_ALLOC   = (1<<6),  /*                                 */
	_MALI_MMU_READABLE  = _MALI_PP_READABLE | _MALI_GP_READABLE,   /*                                                      */
	_MALI_MMU_WRITEABLE = _MALI_PP_WRITEABLE | _MALI_GP_WRITEABLE, /*                                                       */
} _mali_bus_usage;

typedef enum mali_memory_cache_settings {
	MALI_CACHE_STANDARD 			= 0,
	MALI_CACHE_GP_READ_ALLOCATE     = 1,
} mali_memory_cache_settings ;


/*                                                 
  
                                                                          
                                                     
                                                                               
  
                                                                       
                                                                           
         
  
                                                                           
                                                                              
                                 
  
                                                                            
                                                                             
                                                                           
                                                      
  
                                                                               
                                                         
                                                                               
                                                                               
                                     
 */
typedef struct _mali_mem_info {
	u32 size;                     /*                                    */
	_mali_bus_usage flags;        /*                                   */
	u32 maximum_order_supported;  /*                       */
	u32 identifier;               /*                                            */
	struct _mali_mem_info * next; /*                  */
} _mali_mem_info;

/*     */ /*                         */


/*                                            
      */

/*                                                                            
      */

/*                                                      
  
                                                                  
                                                                              
                                                                              
  
                                                                                  
                                                                              
                                                                         
                                                                                   
                                              
                                                                             
                                                                                  
                                                            
                                                                                   
  
 */
typedef enum _maligp_job_suspended_response_code {
	_MALIGP_JOB_ABORT,                  /*                                  */
	_MALIGP_JOB_RESUME_WITH_NEW_HEAP    /*                                                   */
} _maligp_job_suspended_response_code;

typedef struct {
	void *ctx;                      /*                                                    */
	u32 cookie;                     /*                                                                 */
	_maligp_job_suspended_response_code code; /*                                                                                    */
	u32 arguments[2];               /*                                                                                                                         */
} _mali_uk_gp_suspend_response_s;

/*     */ /*                                          */

/*                                                            
      */

/*                                                                                     */
typedef enum {
	_MALI_UK_START_JOB_STARTED,                         /*               */
	_MALI_UK_START_JOB_NOT_STARTED_DO_REQUEUE           /*                                                                     */
} _mali_uk_start_job_status;

/*                                                                                              */

typedef enum {
	_MALI_UK_JOB_STATUS_END_SUCCESS         = 1<<(16+0),
	_MALI_UK_JOB_STATUS_END_OOM             = 1<<(16+1),
	_MALI_UK_JOB_STATUS_END_ABORT           = 1<<(16+2),
	_MALI_UK_JOB_STATUS_END_TIMEOUT_SW      = 1<<(16+3),
	_MALI_UK_JOB_STATUS_END_HANG            = 1<<(16+4),
	_MALI_UK_JOB_STATUS_END_SEG_FAULT       = 1<<(16+5),
	_MALI_UK_JOB_STATUS_END_ILLEGAL_JOB     = 1<<(16+6),
	_MALI_UK_JOB_STATUS_END_UNKNOWN_ERR     = 1<<(16+7),
	_MALI_UK_JOB_STATUS_END_SHUTDOWN        = 1<<(16+8),
	_MALI_UK_JOB_STATUS_END_SYSTEM_UNUSABLE = 1<<(16+9)
} _mali_uk_job_status;

#define MALIGP2_NUM_REGS_FRAME (6)

/*                                               
  
                                  
                                                                               
                                                                     
                                                               
                                                                                
                                                                              
                                               
                                                                                   
                                                                                        
                                                                         
                                                                                 
                                                                        
                                                                        
                                                                   
                                                                                   
  
                                                                                    
                                                                               
                                                                              
               
  
                                                                               
                                                                      
                                                                            
                                                                           
                                                                                  
          
  
                                                                                            
                                                                                  
                                                                         
                                                              
                                                                                      
                                                                              
                                                                  
                                                                                  
                                                              
                                                                               
                                                                           
                   
  
                                                                         
                                                                           
                                                                              
                                                                                
                                                                                  
                                                                            
                                                                             
                                                                                 
  
 */
typedef struct {
	void *ctx;                          /*                                                    */
	u32 user_job_ptr;                   /*                                                                     */
	u32 priority;                       /*                                                           */
	u32 frame_registers[MALIGP2_NUM_REGS_FRAME]; /*                                                         */
	u32 perf_counter_flag;              /*                                                                                                                                                   */
	u32 perf_counter_src0;              /*                                                                           */
	u32 perf_counter_src1;              /*                                                                           */
	u32 frame_builder_id;               /*                                            */
	u32 flush_id;                       /*                                                      */
	_mali_uk_fence_t fence;             /*                                    */
	u32 *timeline_point_ptr;            /*                                                                                        */
} _mali_uk_gp_start_job_s;

#define _MALI_PERFORMANCE_COUNTER_FLAG_SRC0_ENABLE (1<<0) /*                                             */
#define _MALI_PERFORMANCE_COUNTER_FLAG_SRC1_ENABLE (1<<1) /*                                             */
#define _MALI_PERFORMANCE_COUNTER_FLAG_HEATMAP_ENABLE (1<<2) /*                                                                                               */

/*     */ /*                                 */

typedef struct {
	u32 user_job_ptr;               /*                                              */
	_mali_uk_job_status status;     /*                                */
	u32 heap_current_addr;          /*                                                            */
	u32 perf_counter0;              /*                                                           */
	u32 perf_counter1;              /*                                                           */
} _mali_uk_gp_job_finished_s;

typedef struct {
	u32 user_job_ptr;                    /*                                              */
	u32 cookie;                          /*                                                                          */
} _mali_uk_gp_job_suspended_s;

/*     */ /*                       */


/*                                              
      */

#define _MALI_PP_MAX_SUB_JOBS 8

#define _MALI_PP_MAX_FRAME_REGISTERS ((0x058/4)+1)

#define _MALI_PP_MAX_WB_REGISTERS ((0x02C/4)+1)

#define _MALI_DLBU_MAX_REGISTERS 4

/*                                   */
#define _MALI_PP_JOB_FLAG_NO_NOTIFICATION (1<<0)
#define _MALI_PP_JOB_FLAG_IS_WINDOW_SURFACE (1<<1)

/*                                                              
      */

/*                                               
  
                                    
                                                                       
                                                                   
                                                       
                                                                                
                                                                              
                                               
                                                                              
                                                                        
                                                                                        
                                                                         
                                                                                 
                                                                        
                                                                        
                                                                   
                                                                                      
  
                                                                                    
                                                                               
                                                                              
               
  
                                                                            
                                                         
                                                                        
                                                                                           
                                                                                  
                                                                         
                                                              
                                                                                      
                                                                                 
                                                                  
                                                                                  
                                                              
                                                                               
                                                                           
                   
  
 */
typedef struct {
	void *ctx;                      /*                                                    */
	u32 user_job_ptr;               /*                                             */
	u32 priority;                   /*                                                           */
	u32 frame_registers[_MALI_PP_MAX_FRAME_REGISTERS];         /*                                                                                */
	u32 frame_registers_addr_frame[_MALI_PP_MAX_SUB_JOBS - 1]; /*                                             */
	u32 frame_registers_addr_stack[_MALI_PP_MAX_SUB_JOBS - 1]; /*                                             */
	u32 wb0_registers[_MALI_PP_MAX_WB_REGISTERS];
	u32 wb1_registers[_MALI_PP_MAX_WB_REGISTERS];
	u32 wb2_registers[_MALI_PP_MAX_WB_REGISTERS];
	u32 dlbu_registers[_MALI_DLBU_MAX_REGISTERS]; /*                                              */
	u32 num_cores;                      /*                                                     */
	u32 perf_counter_flag;              /*                                                                                                                                                   */
	u32 perf_counter_src0;              /*                                                                           */
	u32 perf_counter_src1;              /*                                                                           */
	u32 frame_builder_id;               /*                                            */
	u32 flush_id;                       /*                                                      */
	u32 flags;                          /*                                                             */
	u32 tilesx;                         /*                                                                          */
	u32 tilesy;                         /*                                                                               */
	u32 heatmap_mem;                    /*                                                                      */
	u32 num_memory_cookies;             /*                                                 */
	u32 *memory_cookies;                /*                                        */
	_mali_uk_fence_t fence;             /*                                    */
	u32 *timeline_point_ptr;            /*                                                                                        */
} _mali_uk_pp_start_job_s;

typedef struct {
	void *ctx;                          /*                                                    */
	_mali_uk_gp_start_job_s *gp_args;   /*                                                          */
	_mali_uk_pp_start_job_s *pp_args;   /*                                                          */
} _mali_uk_pp_and_gp_start_job_s;

/*     */ /*                                 */

typedef struct {
	u32 user_job_ptr;                          /*                                              */
	_mali_uk_job_status status;                /*                                */
	u32 perf_counter0[_MALI_PP_MAX_SUB_JOBS];  /*                                                                                */
	u32 perf_counter1[_MALI_PP_MAX_SUB_JOBS];  /*                                                                                */
	u32 perf_counter_src0;
	u32 perf_counter_src1;
} _mali_uk_pp_job_finished_s;

typedef struct {
	u32 number_of_enabled_cores;               /*                                         */
} _mali_uk_pp_num_cores_changed_s;



/* 
                                     
 */
typedef enum {
	_MALI_UK_PP_JOB_WB0 = 1,
	_MALI_UK_PP_JOB_WB1 = 2,
	_MALI_UK_PP_JOB_WB2 = 4,
} _mali_uk_pp_job_wbx_flag;

typedef struct {
	void *ctx;                      /*                                                    */
	u32 fb_id;                      /*                                                        */
	u32 wb0_memory;
	u32 wb1_memory;
	u32 wb2_memory;
} _mali_uk_pp_disable_wb_s;


/*     */ /*                       */

/*                                          
      */

typedef struct {
	void *ctx;                          /*                                                    */
	u32 type;                           /*                         */
	u32 user_job;                       /*                                             */
	u32 *job_id_ptr;                    /*                                                             */
	_mali_uk_fence_t fence;             /*                                    */
	u32 point;                          /*                                             */
} _mali_uk_soft_job_start_s;

typedef struct {
	u32 user_job;                       /*                                              */
} _mali_uk_soft_job_activated_s;

typedef struct {
	void *ctx;                          /*                                                    */
	u32 job_id;                         /*                        */
} _mali_uk_soft_job_signal_s;

/*     */ /*                             */

/*                                    
      */

/*                                                                
      */

/*                                    
  
                                                                              
  
                                                      
                                                                                                       
                                                                                     
  
                                        
 */
typedef enum {
	/*                     */

	_MALI_NOTIFICATION_CORE_SHUTDOWN_IN_PROGRESS =  (_MALI_UK_CORE_SUBSYSTEM << 16) | 0x20,
	_MALI_NOTIFICATION_APPLICATION_QUIT =           (_MALI_UK_CORE_SUBSYSTEM << 16) | 0x40,
	_MALI_NOTIFICATION_SETTINGS_CHANGED =           (_MALI_UK_CORE_SUBSYSTEM << 16) | 0x80,
	_MALI_NOTIFICATION_SOFT_ACTIVATED =             (_MALI_UK_CORE_SUBSYSTEM << 16) | 0x100,

	/*                                   */

	_MALI_NOTIFICATION_PP_FINISHED =                (_MALI_UK_PP_SUBSYSTEM << 16) | 0x10,
	_MALI_NOTIFICATION_PP_NUM_CORE_CHANGE =         (_MALI_UK_PP_SUBSYSTEM << 16) | 0x20,

	/*                                 */

	_MALI_NOTIFICATION_GP_FINISHED =                (_MALI_UK_GP_SUBSYSTEM << 16) | 0x10,
	_MALI_NOTIFICATION_GP_STALLED =                 (_MALI_UK_GP_SUBSYSTEM << 16) | 0x20,

} _mali_uk_notification_type;

/*                                                                                */
#define _MALI_NOTIFICATION_SUBSYSTEM_MASK 0xFFFF0000
#define _MALI_NOTIFICATION_SUBSYSTEM_SHIFT 16
#define _MALI_NOTIFICATION_ID_MASK 0x0000FFFF
#define _MALI_NOTIFICATION_ID_SHIFT 0


/*                                                                                  
  
  
 */
typedef enum {
	_MALI_UK_USER_SETTING_SW_EVENTS_ENABLE = 0,
	_MALI_UK_USER_SETTING_COLORBUFFER_CAPTURE_ENABLED,
	_MALI_UK_USER_SETTING_DEPTHBUFFER_CAPTURE_ENABLED,
	_MALI_UK_USER_SETTING_STENCILBUFFER_CAPTURE_ENABLED,
	_MALI_UK_USER_SETTING_PER_TILE_COUNTERS_CAPTURE_ENABLED,
	_MALI_UK_USER_SETTING_BUFFER_CAPTURE_COMPOSITOR,
	_MALI_UK_USER_SETTING_BUFFER_CAPTURE_WINDOW,
	_MALI_UK_USER_SETTING_BUFFER_CAPTURE_OTHER,
	_MALI_UK_USER_SETTING_BUFFER_CAPTURE_N_FRAMES,
	_MALI_UK_USER_SETTING_BUFFER_CAPTURE_RESIZE_FACTOR,
	_MALI_UK_USER_SETTING_SW_COUNTER_ENABLED,
	_MALI_UK_USER_SETTING_MAX,
} _mali_uk_user_setting_t;

/*                             */
extern const char *_mali_uk_user_setting_descriptions[];
#define _MALI_UK_USER_SETTING_DESCRIPTIONS \
{                                           \
	"sw_events_enable",                 \
	"colorbuffer_capture_enable",       \
	"depthbuffer_capture_enable",       \
	"stencilbuffer_capture_enable",     \
	"per_tile_counters_enable",         \
	"buffer_capture_compositor",        \
	"buffer_capture_window",            \
	"buffer_capture_other",             \
	"buffer_capture_n_frames",          \
	"buffer_capture_resize_factor",     \
	"sw_counters_enable",               \
};

/*                                                                                     
 */
typedef struct {
	_mali_uk_user_setting_t setting;
	u32 value;
} _mali_uk_settings_changed_s;

/*                                                        
  
                                                                              
                                                            
  
                                                                        
                                                                            
                                                                            
                                                     
                                                   
  
                                                                       
  
                                                         
                                                     
                                                         
                                                                       
                                                                               
                                            
                                           
                                                                                                 
                                                                                                            
                                                                                                      
            
                                                                                              
                                                                                                   
                                                                                                     
                                     
                                                                                                        
                                          
                                                                                                         
                                                              
                                           
                                                                                                            
                                                                                                          
                                                                                      
                                          
                                                                                                 
                                                                                                             
                         
                                                                                            
                                                            
 */
typedef struct {
	void *ctx;                       /*                                                    */
	_mali_uk_notification_type type; /*                                        */
	union {
		_mali_uk_gp_job_suspended_s gp_job_suspended;/*                                                                               */
		_mali_uk_gp_job_finished_s  gp_job_finished; /*                                                                                */
		_mali_uk_pp_job_finished_s  pp_job_finished; /*                                                                                */
		_mali_uk_settings_changed_s setting_changed;/*                                                                                      */
		_mali_uk_soft_job_activated_s soft_job_activated; /*                                                                                   */
	} data;
} _mali_uk_wait_for_notification_s;

/*                                                    
  
                                                                                   
                                                              
 */
typedef struct {
	void *ctx;                       /*                                                    */
	_mali_uk_notification_type type; /*                                     */
} _mali_uk_post_notification_s;

/*     */ /*                                          */

/*                                                    
      */

/*                                                 */

/*                                                
  
                                                                         */
#define _MAKE_VERSION_ID(x) (((x) << 16UL) | (x))

/*                                                                       
               */
#define _IS_VERSION_ID(x) (((x) & 0xFFFF) == (((x) >> 16UL) & 0xFFFF))

/*                                                                               
      */
#define _GET_VERSION(x) (((x) >> 16UL) & 0xFFFF)

/*                                                                */
#define _IS_API_MATCH(x, y) (IS_VERSION_ID((x)) && IS_VERSION_ID((y)) && (GET_VERSION((x)) == GET_VERSION((y))))

/* 
                      
                                          
                                                                 
                                                       
                                                           
 */
#define _MALI_API_VERSION 401
#define _MALI_UK_API_VERSION _MAKE_VERSION_ID(_MALI_API_VERSION)

/* 
                                                                                 
                                                                             
                                                                             
 */
typedef u32 _mali_uk_api_version;

/*                                                 
  
                                                                           
                                                                              
                             
  
                                                                          
                                                                                
              
  
                                                                               
                                                                                
                                                
 */
typedef struct {
	void *ctx;                      /*                                                    */
	_mali_uk_api_version version;   /*                                                */
	int compatible;                 /*                                                          */
} _mali_uk_get_api_version_s;
/*     */ /*                                    */

/*                                                                 */

/*                                                                                             
  
                                                                                                                   
                              
  
                                                                                 
  
 */
typedef struct {
	void *ctx;                       /*                                                    */
	u32 settings[_MALI_UK_USER_SETTING_MAX]; /*                                     */
} _mali_uk_get_user_settings_s;

/*                                                                                                    
 */
typedef struct {
	void *ctx;                       /*                                                    */
	_mali_uk_user_setting_t setting; /*                       */
	u32 value;                       /*                          */
} _mali_uk_get_user_setting_s;

/*                                                         */
typedef struct {
	void *ctx;                       /*                                                    */
} _mali_uk_request_high_priority_s;

/*     */ /*                         */


/*                                      
      */

/*                                                                                                */
#define _MALI_MAP_EXTERNAL_MAP_GUARD_PAGE (1<<0)

typedef struct {
	void *ctx;                      /*                                                    */
	u32 phys_addr;                  /*                         */
	u32 size;                       /*             */
	u32 mali_address;               /*                                                   */
	u32 rights;                     /*                                              */
	u32 flags;                      /*                                                          */
	u32 cookie;                     /*                                                              */
} _mali_uk_map_external_mem_s;

typedef struct {
	void *ctx;                      /*                                                    */
	u32 cookie;                     /*                                                              */
} _mali_uk_unmap_external_mem_s;

/*                                                                                                                   */
typedef struct {
	void *ctx;                      /*                                                    */
	u32 mem_fd;                     /*                          */
	u32 size;                       /*             */
	u32 mali_address;               /*                                                   */
	u32 rights;                     /*                                              */
	u32 flags;                      /*                                                          */
	u32 cookie;                     /*                                                              */
} _mali_uk_attach_dma_buf_s;

typedef struct {
	void *ctx;                      /*                                                    */
	u32 mem_fd;                     /*                          */
	u32 size;                       /*              */
} _mali_uk_dma_buf_get_size_s;

typedef struct {
	void *ctx;                      /*                                                    */
	u32 cookie;                     /*                                                             */
} _mali_uk_release_dma_buf_s;

/*                                                                                                           */
typedef struct {
	void *ctx;                      /*                                                    */
	u32 secure_id;                  /*                  */
	u32 size;                       /*             */
	u32 mali_address;               /*                                                   */
	u32 rights;                     /*                                              */
	u32 flags;                      /*                                                          */
	u32 cookie;                     /*                                                              */
} _mali_uk_attach_ump_mem_s;

typedef struct {
	void *ctx;                      /*                                                    */
	u32 cookie;                     /*                                                             */
} _mali_uk_release_ump_mem_s;

/*                                                
  
                                                                          
                                                                           
                                       
  
                                                                           
            
  
                                                                              
              
  
                                                                                    
                   
  
                                                               
  
                                                                            
                                                                         
                                     
 */
typedef struct {
	void *ctx;                      /*                                                    */
	void *va;                       /*                                                      */
	u32 pa;                         /*                                            */
	u32 size;                       /*                                         */
} _mali_uk_va_to_mali_pa_s;

/* 
                                                     
 */
typedef struct {
	void *ctx;        /*                                                    */
	const void *src;  /*                                   */
	void *dest;       /*                                    */
	u32 size;         /*                                                                                                      */
} _mali_uk_mem_write_safe_s;

typedef struct {
	void *ctx;                      /*                                                    */
	u32 size;                       /*                                                                      */
} _mali_uk_query_mmu_page_table_dump_size_s;

typedef struct {
	void *ctx;                      /*                                                    */
	u32 size;                       /*                                                             */
	void *buffer;                   /*                                                         */
	u32 register_writes_size;       /*                                   */
	u32 *register_writes;           /*                                                                 */
	u32 page_table_dump_size;       /*                                     */
	u32 *page_table_dump;           /*                                                                   */
} _mali_uk_dump_mmu_page_table_s;

/*     */ /*                           */


/*                                                
      */

/*                                                         
  
                                                                                   
                                                                                       
                                                                     
 */
typedef struct {
	void *ctx;                      /*                                                    */
	u32 number_of_total_cores;      /*                                                                */
	u32 number_of_enabled_cores;    /*                                                    */
} _mali_uk_get_pp_number_of_cores_s;

/*                                                      
  
                                                                                   
                                                                                     
                                                                     
 */
typedef struct {
	void *ctx;                      /*                                                    */
	_mali_core_version version;     /*                                                                  */
} _mali_uk_get_pp_core_version_s;

/*     */ /*                       */


/*                                              
      */

/*                                                         
  
                                                                                   
                                                                                       
                                                                   
 */
typedef struct {
	void *ctx;                      /*                                                    */
	u32 number_of_cores;            /*                                                        */
} _mali_uk_get_gp_number_of_cores_s;

/*                                                      
  
                                                                                   
                                                                                     
                                                                   
 */
typedef struct {
	void *ctx;                      /*                                                    */
	_mali_core_version version;     /*                                                                 */
} _mali_uk_get_gp_core_version_s;

typedef struct {
	void *ctx;                      /*                                                    */
	u32 limit;                      /*                                                                                              */
} _mali_uk_profiling_start_s;

typedef struct {
	void *ctx;                      /*                                                    */
	u32 event_id;                   /*                                                                          */
	u32 data[5];                    /*                            */
} _mali_uk_profiling_add_event_s;

typedef struct {
	void *ctx;                      /*                                                    */
	u32 count;                      /*                                      */
} _mali_uk_profiling_stop_s;

typedef struct {
	void *ctx;                      /*                                                    */
	u32 index;                      /*                                              */
	u64 timestamp;                  /*                            */
	u32 event_id;                   /*                                                                        */
	u32 data[5];                    /*                             */
} _mali_uk_profiling_get_event_s;

typedef struct {
	void *ctx;                      /*                                                    */
} _mali_uk_profiling_clear_s;

/*     */ /*                       */


/*                                        
      */

/*                                          
  
                                                                            
                      
                                                                              
                                                                  
                                                                            
                                                                          
                                                                                 
                                                       
  
                                                                            
                                                                              
  
                                                                            
                                                                         
                                                                             
                                                                            
                                                                           
                                                                          
                                                                        
                                              
  
                                                                        
                                                               
 */
typedef struct {
	void *ctx;                      /*                                                    */
	void *mapping;                  /*                                                            */
	u32 size;                       /*                                      */
	u32 phys_addr;                  /*                                                                                  */
	u32 cookie;                     /*                                                  */
	void *uku_private;              /*                                                     */
	void *ukk_private;              /*                                                       */
	mali_memory_cache_settings cache_settings; /*                                                               */
} _mali_uk_mem_mmap_s;

/*                                            
  
                                                                              
                                                                          
                                                                               
                                                                     
  
                                                                            
                                                                            
 */
typedef struct {
	void *ctx;                      /*                                                    */
	void *mapping;                  /*                                            */
	u32 size;                       /*                                     */
	u32 cookie;                     /*                              */
} _mali_uk_mem_munmap_s;
/*     */ /*                           */

/*                                                          
      */

/*                     
  
                                                                           
                                                                
 */
typedef enum _mali_uk_vsync_event {
	_MALI_UK_VSYNC_EVENT_BEGIN_WAIT = 0,
	_MALI_UK_VSYNC_EVENT_END_WAIT
} _mali_uk_vsync_event;

/*                                             
  
 */
typedef struct {
	void *ctx;                      /*                                                    */
	_mali_uk_vsync_event event;     /*                          */
} _mali_uk_vsync_event_report_s;

/*     */ /*                          */

/*                                                                      
      */

/*                                
  
                                                                                
 */
typedef struct {
	void *ctx;                      /*                                                    */
	u32* counters;                  /*                                    */
	u32  num_counters;              /*                                                 */
} _mali_uk_sw_counters_report_s;

/*     */ /*                                       */

/*                                               
      */

typedef struct {
	void *ctx;                      /*                                                    */
	u32 timeline;                   /*                    */
	u32 point;                      /*                                  */
} _mali_uk_timeline_get_latest_point_s;

typedef struct {
	void *ctx;                      /*                                                    */
	_mali_uk_fence_t fence;         /*              */
	u32 timeout;                    /*                                                 */
	u32 status;                     /*                                                       */
} _mali_uk_timeline_wait_s;

typedef struct {
	void *ctx;                      /*                                                    */
	_mali_uk_fence_t fence;         /*                                                   */
	s32 sync_fd;                    /*                                                  */
} _mali_uk_timeline_create_sync_fence_s;

/*     */ /*                             */

/*     */ /*                   */

/*     */ /*                  */

#ifdef __cplusplus
}
#endif

#endif /*                            */
