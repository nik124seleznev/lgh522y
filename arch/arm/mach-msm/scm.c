/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <linux/slab.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/errno.h>
#include <linux/err.h>

#include <asm/cacheflush.h>

#include "scm.h"

/*                             */
#define CACHELINESIZE 32

#define SCM_ENOMEM		-5
#define SCM_EOPNOTSUPP		-4
#define SCM_EINVAL_ADDR		-3
#define SCM_EINVAL_ARG		-2
#define SCM_ERROR		-1
#define SCM_INTERRUPTED		1

static DEFINE_MUTEX(scm_lock);

/* 
                                              
                                                        
                                       
                                             
                              
                                                      
  
                                                   
  
                                              
                      
                                                    
                      
                                                   
                                                     
                                                     
                      
                      
  
                                                                    
                                                                    
                                          
 */
struct scm_command {
	u32	len;
	u32	buf_offset;
	u32	resp_hdr_offset;
	u32	id;
	u32	buf[0];
};

/* 
                                                
                                            
                                                                        
                                                                 
 */
struct scm_response {
	u32	len;
	u32	buf_offset;
	u32	is_complete;
};

/* 
                                                
                                        
                                          
  
                                                                 
                                                                    
  
                                                                            
 */
static struct scm_command *alloc_scm_command(size_t cmd_size, size_t resp_size)
{
	struct scm_command *cmd;
	size_t len = sizeof(*cmd) + sizeof(struct scm_response) + cmd_size +
		resp_size;

	cmd = kzalloc(PAGE_ALIGN(len), GFP_KERNEL);
	if (cmd) {
		cmd->len = len;
		cmd->buf_offset = offsetof(struct scm_command, buf);
		cmd->resp_hdr_offset = cmd->buf_offset + cmd_size;
	}
	return cmd;
}

/* 
                                           
                        
  
                       
 */
static inline void free_scm_command(struct scm_command *cmd)
{
	kfree(cmd);
}

/* 
                                                              
                
  
                                                 
 */
static inline struct scm_response *scm_command_to_response(
		const struct scm_command *cmd)
{
	return (void *)cmd + cmd->resp_hdr_offset;
}

/* 
                                                               
                
  
                                                        
 */
static inline void *scm_get_command_buffer(const struct scm_command *cmd)
{
	return (void *)cmd->buf;
}

/* 
                                                                 
                 
  
                                                        
 */
static inline void *scm_get_response_buffer(const struct scm_response *rsp)
{
	return (void *)rsp + rsp->buf_offset;
}

static int scm_remap_error(int err)
{
	switch (err) {
	case SCM_ERROR:
		return -EIO;
	case SCM_EINVAL_ADDR:
	case SCM_EINVAL_ARG:
		return -EINVAL;
	case SCM_EOPNOTSUPP:
		return -EOPNOTSUPP;
	case SCM_ENOMEM:
		return -ENOMEM;
	}
	return -EINVAL;
}

static u32 smc(u32 cmd_addr)
{
	int context_id;
	register u32 r0 asm("r0") = 1;
	register u32 r1 asm("r1") = (u32)&context_id;
	register u32 r2 asm("r2") = cmd_addr;
	do {
		asm volatile(
			__asmeq("%0", "r0")
			__asmeq("%1", "r0")
			__asmeq("%2", "r1")
			__asmeq("%3", "r2")
#ifdef REQUIRES_SEC
			".arch_extension sec\n"
#endif
			"smc	#0	@ switch to secure world\n"
			: "=r" (r0)
			: "r" (r0), "r" (r1), "r" (r2)
			: "r3");
	} while (r0 == SCM_INTERRUPTED);

	return r0;
}

static int __scm_call(const struct scm_command *cmd)
{
	int ret;
	u32 cmd_addr = virt_to_phys(cmd);

	/*
                                                                 
                                                                    
                       
  */
	flush_cache_all();
	ret = smc(cmd_addr);
	if (ret < 0)
		ret = scm_remap_error(ret);

	return ret;
}

/* 
                                   
                              
                              
                           
                                         
                             
                                           
  
                                                                             
 */
int scm_call(u32 svc_id, u32 cmd_id, const void *cmd_buf, size_t cmd_len,
		void *resp_buf, size_t resp_len)
{
	int ret;
	struct scm_command *cmd;
	struct scm_response *rsp;

	cmd = alloc_scm_command(cmd_len, resp_len);
	if (!cmd)
		return -ENOMEM;

	cmd->id = (svc_id << 10) | cmd_id;
	if (cmd_buf)
		memcpy(scm_get_command_buffer(cmd), cmd_buf, cmd_len);

	mutex_lock(&scm_lock);
	ret = __scm_call(cmd);
	mutex_unlock(&scm_lock);
	if (ret)
		goto out;

	rsp = scm_command_to_response(cmd);
	do {
		u32 start = (u32)rsp;
		u32 end = (u32)scm_get_response_buffer(rsp) + resp_len;
		start &= ~(CACHELINESIZE - 1);
		while (start < end) {
			asm ("mcr p15, 0, %0, c7, c6, 1" : : "r" (start)
			     : "memory");
			start += CACHELINESIZE;
		}
	} while (!rsp->is_complete);

	if (resp_buf)
		memcpy(resp_buf, scm_get_response_buffer(rsp), resp_len);
out:
	free_scm_command(cmd);
	return ret;
}
EXPORT_SYMBOL(scm_call);

u32 scm_get_version(void)
{
	int context_id;
	static u32 version = -1;
	register u32 r0 asm("r0");
	register u32 r1 asm("r1");

	if (version != -1)
		return version;

	mutex_lock(&scm_lock);

	r0 = 0x1 << 8;
	r1 = (u32)&context_id;
	do {
		asm volatile(
			__asmeq("%0", "r0")
			__asmeq("%1", "r1")
			__asmeq("%2", "r0")
			__asmeq("%3", "r1")
#ifdef REQUIRES_SEC
			".arch_extension sec\n"
#endif
			"smc	#0	@ switch to secure world\n"
			: "=r" (r0), "=r" (r1)
			: "r" (r0), "r" (r1)
			: "r2", "r3");
	} while (r0 == SCM_INTERRUPTED);

	version = r1;
	mutex_unlock(&scm_lock);

	return version;
}
EXPORT_SYMBOL(scm_get_version);