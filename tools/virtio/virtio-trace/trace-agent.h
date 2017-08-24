#ifndef __TRACE_AGENT_H__
#define __TRACE_AGENT_H__
#include <pthread.h>
#include <stdbool.h>

#define MAX_CPUS	256
#define PIPE_INIT       (1024*1024)

/*
                                                                   
                                         
                                                                  
                              
                                                                
                                                               
 */
struct agent_info {
	unsigned long pipe_size;
	bool use_stdout;
	int cpus;
	int ctl_fd;
	struct rw_thread_info *rw_ti[MAX_CPUS];
};

/*
                                                                
                                                        
                                                   
                                                    
                              
                                
                                         
 */
struct rw_thread_info {
	int cpu_num;
	int in_fd;
	int out_fd;
	int read_pipe;
	int write_pipe;
	unsigned long pipe_size;
};

/*                             */
extern bool global_sig_receive;

/*                      */
extern bool global_run_operation;
extern pthread_mutex_t mutex_notify;
extern pthread_cond_t cond_wakeup;

/*                                      */
extern int rw_ctl_init(const char *ctl_path);
extern void *rw_ctl_loop(int ctl_fd);

/*                             */
extern void *rw_thread_info_new(void);
extern void *rw_thread_init(int cpu, const char *in_path, const char *out_path,
			bool stdout_flag, unsigned long pipe_size,
			struct rw_thread_info *rw_ti);
extern pthread_t rw_thread_run(struct rw_thread_info *rw_ti);

static inline void *zalloc(size_t size)
{
	return calloc(1, size);
}

#define pr_err(format, ...) fprintf(stderr, format, ## __VA_ARGS__)
#define pr_info(format, ...) fprintf(stdout, format, ## __VA_ARGS__)
#ifdef DEBUG
#define pr_debug(format, ...) fprintf(stderr, format, ## __VA_ARGS__)
#else
#define pr_debug(format, ...) do {} while (0)
#endif

#endif /*                 */
