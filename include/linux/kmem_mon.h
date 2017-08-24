#ifndef __KMEM_MON_H__
#define __KMEM_MON_H__

#include <linux/sched.h>

#ifdef CONFIG_MTPROF_KMEM

#ifndef TRUE
/*                                   */
#define TRUE 1
#endif

#ifndef FALSE
/*                                     */
#define FALSE 0
#endif

/*                                                  */
#define MAX_PROCESS_NUM (16*1024)	/*                                        */
#define MAX_KMEM_MON_NUM (20*1024)
#define MAX_ADDR_TABLE (64*1024)	/*                      */
#define MAX_CALLER_TABLE (80*1024)
#define MAX_FAIL_PARAMETER 256
#define MAX_MEM_CLASS_NUM 64
#define MAX_CMD_LINE 64
#define MAX_PID_LEN 6		/*                         */
#define MAX_ADDR_IDX (16*1024)	/*    */
#define ADDR_SHIFT 14

typedef enum {
	KMEM_MON_TYPE_KMALLOC = 0,	/*                       */
	KMEM_MON_TYPE_KMEM_CACHE,	/*                 */
	KMEM_MON_TYPE_PAGES,	/*                                 */
	KMEM_MON_TYPE_PMEM,
	KMEM_MON_TYPE_M4U,
	KMEM_MON_TYPE_VMALLOC,
	KMEM_MON_TYPE_ASHMEM,
	KMEM_MON_TYPE_KMALLOCWRAPPER,
} MEM_CLASS_T;

typedef struct mem_class_info_struct {
	MEM_CLASS_T mem_class;

	int index[MAX_KMEM_MON_NUM];
} mem_class_info_t;

/*                             */
typedef struct caller_info_struct {
	unsigned long caller_addr;
	int bytes_req, bytes_alloc;
	int bytes_free;
	int freq_alloc, freq_free;
	int pid;
	MEM_CLASS_T mem_class;
	int next_node;
} caller_info_t;

/*                                                   */
typedef struct addr_info_struct {
	unsigned long addr;
	int caller_hash;
	int next;
} addr_info_t;

typedef struct process_info_struct {
	pid_t pid;
	pid_t tgid;
	char cmdline[MAX_CMD_LINE];
	char comm[TASK_COMM_LEN];	/*                               
                                                  
                            
                                                 */
	int start_idx;		/*                   */
	/*                                            */
} process_info_t;

/*                                                                            */
/* 
                           
  
                                                        
  
 */
/* 
                           
                                                       
 */
typedef struct mem_info_struct {
	MEM_CLASS_T mem_class;

/*                                                  */
	size_t total_bytes_req;
	size_t total_bytes_alloc;

	size_t total_bytes_free;

	int alloc_freq;
	int free_freq;

	int peak_every_req;	/*                                  */
	unsigned long peak_caller;	/*                */
/*      */

	/*                           */
#if 0
	int fail_freq;
	unsigned long fail_caller;
	char last_fail_parameter[MAX_FAIL_PARAMETER];	/*                                           */
#endif
	int caller_start_idx;
	int next_mem_node;
	/*  */
	/*                                                  */
	/*  */
} mem_info_t;

#endif				/*                           */

extern void kmem_mon_kmalloc(unsigned long caller, const void *addr, int bytes_req,
			     int bytes_alloc);
extern void kmem_mon_kfree(unsigned long caller, const void *addr);
extern void kmem_mon_pmem_alloc(int req, int alloc);
extern void kmem_mon_pmem_free(int size);
extern void kmem_mon_m4u_alloc(int req, int alloc);
extern void kmem_mon_m4u_dealloc(const unsigned int addr, const unsigned int req_size);
extern void kmem_mon_vmalloc(unsigned long caller, const void *addr, int bytes_req,
			     int bytes_alloc);
extern void kmem_mon_vfree(int size);
extern void kmem_mon_ashmem_mmap(int size);
extern void kmem_mon_ashmem_release(int size);
extern void kmem_mon_kmallocwrapper(unsigned long caller, int size);
/*                                                                                                              */
extern void kmem_mon_kfreewrapper(const void *addr);
extern void kmem_mon_kmem_cache_alloc(unsigned long caller, const void *addr, size_t bytes_req,
				      size_t bytes_alloc);
extern void kmem_mon_kmem_cache_free(unsigned long caller, const void *addr);

#endif				/*                */
