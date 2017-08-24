/*
 *   fs/cifs/transport.c
 *
 *   Copyright (C) International Business Machines  Corp., 2002,2008
 *   Author(s): Steve French (sfrench@us.ibm.com)
 *   Jeremy Allison (jra@samba.org) 2006.
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published
 *   by the Free Software Foundation; either version 2.1 of the License, or
 *   (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <linux/fs.h>
#include <linux/list.h>
#include <linux/gfp.h>
#include <linux/wait.h>
#include <linux/net.h>
#include <linux/delay.h>
#include <linux/freezer.h>
#include <linux/tcp.h>
#include <linux/highmem.h>
#include <asm/uaccess.h>
#include <asm/processor.h>
#include <linux/mempool.h>
#include "cifspdu.h"
#include "cifsglob.h"
#include "cifsproto.h"
#include "cifs_debug.h"

void
cifs_wake_up_task(struct mid_q_entry *mid)
{
	wake_up_process(mid->callback_data);
}

struct mid_q_entry *
AllocMidQEntry(const struct smb_hdr *smb_buffer, struct TCP_Server_Info *server)
{
	struct mid_q_entry *temp;

	if (server == NULL) {
		cifs_dbg(VFS, "Null TCP session in AllocMidQEntry\n");
		return NULL;
	}

	temp = mempool_alloc(cifs_mid_poolp, GFP_NOFS);
	if (temp == NULL)
		return temp;
	else {
		memset(temp, 0, sizeof(struct mid_q_entry));
		temp->mid = smb_buffer->Mid;	/*           */
		temp->pid = current->pid;
		temp->command = cpu_to_le16(smb_buffer->Command);
		cifs_dbg(FYI, "For smb_command %d\n", smb_buffer->Command);
	/*                                   */ /*                       */
		/*                                            */
		temp->when_alloc = jiffies;
		temp->server = server;

		/*
                                                         
                                                     
   */
		temp->callback = cifs_wake_up_task;
		temp->callback_data = current;
	}

	atomic_inc(&midCount);
	temp->mid_state = MID_REQUEST_ALLOCATED;
	return temp;
}

void
DeleteMidQEntry(struct mid_q_entry *midEntry)
{
#ifdef CONFIG_CIFS_STATS2
	__le16 command = midEntry->server->vals->lock_cmd;
	unsigned long now;
#endif
	midEntry->mid_state = MID_FREE;
	atomic_dec(&midCount);
	if (midEntry->large_buf)
		cifs_buf_release(midEntry->resp_buf);
	else
		cifs_small_buf_release(midEntry->resp_buf);
#ifdef CONFIG_CIFS_STATS2
	now = jiffies;
	/*                                                            
                                                                 */
	if ((now - midEntry->when_alloc) > HZ) {
		if ((cifsFYI & CIFS_TIMER) && (midEntry->command != command)) {
			printk(KERN_DEBUG " CIFS slow rsp: cmd %d mid %llu",
			       midEntry->command, midEntry->mid);
			printk(" A: 0x%lx S: 0x%lx R: 0x%lx\n",
			       now - midEntry->when_alloc,
			       now - midEntry->when_sent,
			       now - midEntry->when_received);
		}
	}
#endif
	mempool_free(midEntry, cifs_mid_poolp);
}

void
cifs_delete_mid(struct mid_q_entry *mid)
{
	spin_lock(&GlobalMid_Lock);
	list_del(&mid->qhead);
	spin_unlock(&GlobalMid_Lock);

	DeleteMidQEntry(mid);
}

/*
                                                       
                                      
                                  
                               
                                                      
  
                                                                            
                                                            
 */
static int
smb_send_kvec(struct TCP_Server_Info *server, struct kvec *iov, size_t n_vec,
		size_t *sent)
{
	int rc = 0;
	int i = 0;
	struct msghdr smb_msg;
	unsigned int remaining;
	size_t first_vec = 0;
	struct socket *ssocket = server->ssocket;

	*sent = 0;

	smb_msg.msg_name = (struct sockaddr *) &server->dstaddr;
	smb_msg.msg_namelen = sizeof(struct sockaddr);
	smb_msg.msg_control = NULL;
	smb_msg.msg_controllen = 0;
	if (server->noblocksnd)
		smb_msg.msg_flags = MSG_DONTWAIT + MSG_NOSIGNAL;
	else
		smb_msg.msg_flags = MSG_NOSIGNAL;

	remaining = 0;
	for (i = 0; i < n_vec; i++)
		remaining += iov[i].iov_len;

	i = 0;
	while (remaining) {
		/*
                                                           
                                                        
                                                          
                                                         
                                                    
                                                         
                                                        
                                                       
                                                      
                                                         
                                                    
                                                        
                                                    
                                                     
                                                  
                                                   
   */
		rc = kernel_sendmsg(ssocket, &smb_msg, &iov[first_vec],
				    n_vec - first_vec, remaining);
		if (rc == -EAGAIN) {
			i++;
			if (i >= 14 || (!server->noblocksnd && (i > 2))) {
				cifs_dbg(VFS, "sends on sock %p stuck for 15 seconds\n",
					 ssocket);
				rc = -EAGAIN;
				break;
			}
			msleep(1 << i);
			continue;
		}

		if (rc < 0)
			break;

		/*                                        */
		*sent += rc;

		if (rc == remaining) {
			remaining = 0;
			break;
		}

		if (rc > remaining) {
			cifs_dbg(VFS, "sent %d requested %d\n", rc, remaining);
			break;
		}

		if (rc == 0) {
			/*                                                 
                                               */
			cifs_dbg(VFS, "tcp sent no data\n");
			msleep(500);
			continue;
		}

		remaining -= rc;

		/*                         */
		for (i = first_vec; i < n_vec; i++) {
			if (iov[i].iov_len) {
				if (rc > iov[i].iov_len) {
					rc -= iov[i].iov_len;
					iov[i].iov_len = 0;
				} else {
					iov[i].iov_base += rc;
					iov[i].iov_len -= rc;
					first_vec = i;
					break;
				}
			}
		}

		i = 0; /*                                        */
		rc = 0;
	}
	return rc;
}

/* 
                                                                         
                             
                                         
                                                         
  
                                                                             
                                                                            
                                                   
 */
void
cifs_rqst_page_to_kvec(struct smb_rqst *rqst, unsigned int idx,
			struct kvec *iov)
{
	/*
                                                         
                                                             
                                                              
                                                              
                                                               
                                                               
                                                    
  */
	iov->iov_base = kmap(rqst->rq_pages[idx]);

	/*                                                       */
	if (idx == (rqst->rq_npages - 1))
		iov->iov_len = rqst->rq_tailsz;
	else
		iov->iov_len = rqst->rq_pagesz;
}

static int
smb_send_rqst(struct TCP_Server_Info *server, struct smb_rqst *rqst)
{
	int rc;
	struct kvec *iov = rqst->rq_iov;
	int n_vec = rqst->rq_nvec;
	unsigned int smb_buf_length = get_rfc1002_length(iov[0].iov_base);
	unsigned int i;
	size_t total_len = 0, sent;
	struct socket *ssocket = server->ssocket;
	int val = 1;

	if (ssocket == NULL)
		return -ENOTSOCK;

	cifs_dbg(FYI, "Sending smb: smb_len=%u\n", smb_buf_length);
	dump_smb(iov[0].iov_base, iov[0].iov_len);

	/*                 */
	kernel_setsockopt(ssocket, SOL_TCP, TCP_CORK,
				(char *)&val, sizeof(val));

	rc = smb_send_kvec(server, iov, n_vec, &sent);
	if (rc < 0)
		goto uncork;

	total_len += sent;

	/*                                                  */
	for (i = 0; i < rqst->rq_npages; i++) {
		struct kvec p_iov;

		cifs_rqst_page_to_kvec(rqst, i, &p_iov);
		rc = smb_send_kvec(server, &p_iov, 1, &sent);
		kunmap(rqst->rq_pages[i]);
		if (rc < 0)
			break;

		total_len += sent;
	}

uncork:
	/*           */
	val = 0;
	kernel_setsockopt(ssocket, SOL_TCP, TCP_CORK,
				(char *)&val, sizeof(val));

	if ((total_len > 0) && (total_len != smb_buf_length + 4)) {
		cifs_dbg(FYI, "partial send (wanted=%u sent=%zu): terminating session\n",
			 smb_buf_length + 4, total_len);
		/*
                                                                
                                                               
                                                     
   */
		server->tcpStatus = CifsNeedReconnect;
	}

	if (rc < 0 && rc != -EINTR)
		cifs_dbg(VFS, "Error %d sending data on socket to server\n",
			 rc);
	else
		rc = 0;

	return rc;
}

static int
smb_sendv(struct TCP_Server_Info *server, struct kvec *iov, int n_vec)
{
	struct smb_rqst rqst = { .rq_iov = iov,
				 .rq_nvec = n_vec };

	return smb_send_rqst(server, &rqst);
}

int
smb_send(struct TCP_Server_Info *server, struct smb_hdr *smb_buffer,
	 unsigned int smb_buf_length)
{
	struct kvec iov;

	iov.iov_base = smb_buffer;
	iov.iov_len = smb_buf_length + 4;

	return smb_sendv(server, &iov, 1);
}

static int
wait_for_free_credits(struct TCP_Server_Info *server, const int timeout,
		      int *credits)
{
	int rc;

	spin_lock(&server->req_lock);
	if (timeout == CIFS_ASYNC_OP) {
		/*                                   */
		server->in_flight++;
		*credits -= 1;
		spin_unlock(&server->req_lock);
		return 0;
	}

	while (1) {
		if (*credits <= 0) {
			spin_unlock(&server->req_lock);
			cifs_num_waiters_inc(server);
			rc = wait_event_killable(server->request_q,
						 has_credits(server, credits));
			cifs_num_waiters_dec(server);
			if (rc)
				return rc;
			spin_lock(&server->req_lock);
		} else {
			if (server->tcpStatus == CifsExiting) {
				spin_unlock(&server->req_lock);
				return -ENOENT;
			}

			/*
                                                  
                                             
    */

			/*                                            */
			if (timeout != CIFS_BLOCKING_OP) {
				*credits -= 1;
				server->in_flight++;
			}
			spin_unlock(&server->req_lock);
			break;
		}
	}
	return 0;
}

static int
wait_for_free_request(struct TCP_Server_Info *server, const int timeout,
		      const int optype)
{
	return wait_for_free_credits(server, timeout,
				server->ops->get_credits_field(server, optype));
}

static int allocate_mid(struct cifs_ses *ses, struct smb_hdr *in_buf,
			struct mid_q_entry **ppmidQ)
{
	if (ses->server->tcpStatus == CifsExiting) {
		return -ENOENT;
	}

	if (ses->server->tcpStatus == CifsNeedReconnect) {
		cifs_dbg(FYI, "tcp session dead - return to caller to retry\n");
		return -EAGAIN;
	}

	if (ses->status != CifsGood) {
		/*                                                          */
		if ((in_buf->Command != SMB_COM_SESSION_SETUP_ANDX) &&
			(in_buf->Command != SMB_COM_NEGOTIATE))
			return -EAGAIN;
		/*                                     */
	}
	*ppmidQ = AllocMidQEntry(in_buf, ses->server);
	if (*ppmidQ == NULL)
		return -ENOMEM;
	spin_lock(&GlobalMid_Lock);
	list_add_tail(&(*ppmidQ)->qhead, &ses->server->pending_mid_q);
	spin_unlock(&GlobalMid_Lock);
	return 0;
}

static int
wait_for_response(struct TCP_Server_Info *server, struct mid_q_entry *midQ)
{
	int error;

	error = wait_event_freezekillable_unsafe(server->response_q,
				    midQ->mid_state != MID_REQUEST_SUBMITTED);
	if (error < 0)
		return -ERESTARTSYS;

	return 0;
}

struct mid_q_entry *
cifs_setup_async_request(struct TCP_Server_Info *server, struct smb_rqst *rqst)
{
	int rc;
	struct smb_hdr *hdr = (struct smb_hdr *)rqst->rq_iov[0].iov_base;
	struct mid_q_entry *mid;

	/*                                      */
	if (server->sec_mode & (SECMODE_SIGN_REQUIRED | SECMODE_SIGN_ENABLED))
		hdr->Flags2 |= SMBFLG2_SECURITY_SIGNATURE;

	mid = AllocMidQEntry(hdr, server);
	if (mid == NULL)
		return ERR_PTR(-ENOMEM);

	rc = cifs_sign_rqst(rqst, server, &mid->sequence_number);
	if (rc) {
		DeleteMidQEntry(mid);
		return ERR_PTR(rc);
	}

	return mid;
}

/*
                                                                        
                                                               
 */
int
cifs_call_async(struct TCP_Server_Info *server, struct smb_rqst *rqst,
		mid_receive_t *receive, mid_callback_t *callback,
		void *cbdata, const int flags)
{
	int rc, timeout, optype;
	struct mid_q_entry *mid;

	timeout = flags & CIFS_TIMEOUT_MASK;
	optype = flags & CIFS_OP_MASK;

	rc = wait_for_free_request(server, timeout, optype);
	if (rc)
		return rc;

	mutex_lock(&server->srv_mutex);
	mid = server->ops->setup_async_request(server, rqst);
	if (IS_ERR(mid)) {
		mutex_unlock(&server->srv_mutex);
		add_credits(server, 1, optype);
		wake_up(&server->request_q);
		return PTR_ERR(mid);
	}

	mid->receive = receive;
	mid->callback = callback;
	mid->callback_data = cbdata;
	mid->mid_state = MID_REQUEST_SUBMITTED;

	/*                             */
	spin_lock(&GlobalMid_Lock);
	list_add_tail(&mid->qhead, &server->pending_mid_q);
	spin_unlock(&GlobalMid_Lock);


	cifs_in_send_inc(server);
	rc = smb_send_rqst(server, rqst);
	cifs_in_send_dec(server);
	cifs_save_when_sent(mid);

	if (rc < 0)
		server->sequence_number -= 2;
	mutex_unlock(&server->srv_mutex);

	if (rc == 0)
		return 0;

	cifs_delete_mid(mid);
	add_credits(server, 1, optype);
	wake_up(&server->request_q);
	return rc;
}

/*
  
                                                                  
                      
  
                                                                 
                                                                             
  
 */
int
SendReceiveNoRsp(const unsigned int xid, struct cifs_ses *ses,
		 char *in_buf, int flags)
{
	int rc;
	struct kvec iov[1];
	int resp_buf_type;

	iov[0].iov_base = in_buf;
	iov[0].iov_len = get_rfc1002_length(in_buf) + 4;
	flags |= CIFS_NO_RESP;
	rc = SendReceive2(xid, ses, iov, 1, &resp_buf_type, flags);
	cifs_dbg(NOISY, "SendRcvNoRsp flags %d rc %d\n", flags, rc);

	return rc;
}

static int
cifs_sync_mid_result(struct mid_q_entry *mid, struct TCP_Server_Info *server)
{
	int rc = 0;

	cifs_dbg(FYI, "%s: cmd=%d mid=%llu state=%d\n",
		 __func__, le16_to_cpu(mid->command), mid->mid, mid->mid_state);

	spin_lock(&GlobalMid_Lock);
	switch (mid->mid_state) {
	case MID_RESPONSE_RECEIVED:
		spin_unlock(&GlobalMid_Lock);
		return rc;
	case MID_RETRY_NEEDED:
		rc = -EAGAIN;
		break;
	case MID_RESPONSE_MALFORMED:
		rc = -EIO;
		break;
	case MID_SHUTDOWN:
		rc = -EHOSTDOWN;
		break;
	default:
		list_del_init(&mid->qhead);
		cifs_dbg(VFS, "%s: invalid mid state mid=%llu state=%d\n",
			 __func__, mid->mid, mid->mid_state);
		rc = -EIO;
	}
	spin_unlock(&GlobalMid_Lock);

	DeleteMidQEntry(mid);
	return rc;
}

static inline int
send_cancel(struct TCP_Server_Info *server, void *buf, struct mid_q_entry *mid)
{
	return server->ops->send_cancel ?
				server->ops->send_cancel(server, buf, mid) : 0;
}

int
cifs_check_receive(struct mid_q_entry *mid, struct TCP_Server_Info *server,
		   bool log_error)
{
	unsigned int len = get_rfc1002_length(mid->resp_buf) + 4;

	dump_smb(mid->resp_buf, min_t(u32, 92, len));

	/*                                            */
	if (server->sec_mode & (SECMODE_SIGN_REQUIRED | SECMODE_SIGN_ENABLED)) {
		struct kvec iov;
		int rc = 0;
		struct smb_rqst rqst = { .rq_iov = &iov,
					 .rq_nvec = 1 };

		iov.iov_base = mid->resp_buf;
		iov.iov_len = len;
		/*                                 */
		rc = cifs_verify_signature(&rqst, server,
					   mid->sequence_number);
		if (rc)
			cifs_dbg(VFS, "SMB signature verification returned error = %d\n",
				 rc);
	}

	/*                                             */
	return map_smb_to_linux_error(mid->resp_buf, log_error);
}

struct mid_q_entry *
cifs_setup_request(struct cifs_ses *ses, struct smb_rqst *rqst)
{
	int rc;
	struct smb_hdr *hdr = (struct smb_hdr *)rqst->rq_iov[0].iov_base;
	struct mid_q_entry *mid;

	rc = allocate_mid(ses, hdr, &mid);
	if (rc)
		return ERR_PTR(rc);
	rc = cifs_sign_rqst(rqst, ses->server, &mid->sequence_number);
	if (rc) {
		cifs_delete_mid(mid);
		return ERR_PTR(rc);
	}
	return mid;
}

int
SendReceive2(const unsigned int xid, struct cifs_ses *ses,
	     struct kvec *iov, int n_vec, int *resp_buf_type /*     */,
	     const int flags)
{
	int rc = 0;
	int timeout, optype;
	struct mid_q_entry *midQ;
	char *buf = iov[0].iov_base;
	unsigned int credits = 1;
	struct smb_rqst rqst = { .rq_iov = iov,
				 .rq_nvec = n_vec };

	timeout = flags & CIFS_TIMEOUT_MASK;
	optype = flags & CIFS_OP_MASK;

	*resp_buf_type = CIFS_NO_BUFFER;  /*                     */

	if ((ses == NULL) || (ses->server == NULL)) {
		cifs_small_buf_release(buf);
		cifs_dbg(VFS, "Null session\n");
		return -EIO;
	}

	if (ses->server->tcpStatus == CifsExiting) {
		cifs_small_buf_release(buf);
		return -ENOENT;
	}

	/*
                                                                
                                                              
                    
  */

	rc = wait_for_free_request(ses->server, timeout, optype);
	if (rc) {
		cifs_small_buf_release(buf);
		return rc;
	}

	/*
                                                                        
                                                                       
                
  */

	mutex_lock(&ses->server->srv_mutex);

	midQ = ses->server->ops->setup_request(ses, &rqst);
	if (IS_ERR(midQ)) {
		mutex_unlock(&ses->server->srv_mutex);
		cifs_small_buf_release(buf);
		/*                                        */
		add_credits(ses->server, 1, optype);
		return PTR_ERR(midQ);
	}

	midQ->mid_state = MID_REQUEST_SUBMITTED;
	cifs_in_send_inc(ses->server);
	rc = smb_sendv(ses->server, iov, n_vec);
	cifs_in_send_dec(ses->server);
	cifs_save_when_sent(midQ);

	if (rc < 0)
		ses->server->sequence_number -= 2;
	mutex_unlock(&ses->server->srv_mutex);

	if (rc < 0) {
		cifs_small_buf_release(buf);
		goto out;
	}

	if (timeout == CIFS_ASYNC_OP) {
		cifs_small_buf_release(buf);
		goto out;
	}

	rc = wait_for_response(ses->server, midQ);
	if (rc != 0) {
		send_cancel(ses->server, buf, midQ);
		spin_lock(&GlobalMid_Lock);
		if (midQ->mid_state == MID_REQUEST_SUBMITTED) {
			midQ->callback = DeleteMidQEntry;
			spin_unlock(&GlobalMid_Lock);
			cifs_small_buf_release(buf);
			add_credits(ses->server, 1, optype);
			return rc;
		}
		spin_unlock(&GlobalMid_Lock);
	}

	cifs_small_buf_release(buf);

	rc = cifs_sync_mid_result(midQ, ses->server);
	if (rc != 0) {
		add_credits(ses->server, 1, optype);
		return rc;
	}

	if (!midQ->resp_buf || midQ->mid_state != MID_RESPONSE_RECEIVED) {
		rc = -EIO;
		cifs_dbg(FYI, "Bad MID state?\n");
		goto out;
	}

	buf = (char *)midQ->resp_buf;
	iov[0].iov_base = buf;
	iov[0].iov_len = get_rfc1002_length(buf) + 4;
	if (midQ->large_buf)
		*resp_buf_type = CIFS_LARGE_BUFFER;
	else
		*resp_buf_type = CIFS_SMALL_BUFFER;

	credits = ses->server->ops->get_credits(midQ);

	rc = ses->server->ops->check_receive(midQ, ses->server,
					     flags & CIFS_LOG_ERROR);

	/*                                                     */
	if ((flags & CIFS_NO_RESP) == 0)
		midQ->resp_buf = NULL;
out:
	cifs_delete_mid(midQ);
	add_credits(ses->server, credits, optype);

	return rc;
}

int
SendReceive(const unsigned int xid, struct cifs_ses *ses,
	    struct smb_hdr *in_buf, struct smb_hdr *out_buf,
	    int *pbytes_returned, const int timeout)
{
	int rc = 0;
	struct mid_q_entry *midQ;

	if (ses == NULL) {
		cifs_dbg(VFS, "Null smb session\n");
		return -EIO;
	}
	if (ses->server == NULL) {
		cifs_dbg(VFS, "Null tcp session\n");
		return -EIO;
	}

	if (ses->server->tcpStatus == CifsExiting)
		return -ENOENT;

	/*                                                             
                                                              
                    */

	if (be32_to_cpu(in_buf->smb_buf_length) > CIFSMaxBufSize +
			MAX_CIFS_HDR_SIZE - 4) {
		cifs_dbg(VFS, "Illegal length, greater than maximum frame, %d\n",
			 be32_to_cpu(in_buf->smb_buf_length));
		return -EIO;
	}

	rc = wait_for_free_request(ses->server, timeout, 0);
	if (rc)
		return rc;

	/*                                                                     
                                                                       
                */

	mutex_lock(&ses->server->srv_mutex);

	rc = allocate_mid(ses, in_buf, &midQ);
	if (rc) {
		mutex_unlock(&ses->server->srv_mutex);
		/*                                        */
		add_credits(ses->server, 1, 0);
		return rc;
	}

	rc = cifs_sign_smb(in_buf, ses->server, &midQ->sequence_number);
	if (rc) {
		mutex_unlock(&ses->server->srv_mutex);
		goto out;
	}

	midQ->mid_state = MID_REQUEST_SUBMITTED;

	cifs_in_send_inc(ses->server);
	rc = smb_send(ses->server, in_buf, be32_to_cpu(in_buf->smb_buf_length));
	cifs_in_send_dec(ses->server);
	cifs_save_when_sent(midQ);

	if (rc < 0)
		ses->server->sequence_number -= 2;

	mutex_unlock(&ses->server->srv_mutex);

	if (rc < 0)
		goto out;

	if (timeout == CIFS_ASYNC_OP)
		goto out;

	rc = wait_for_response(ses->server, midQ);
	if (rc != 0) {
		send_cancel(ses->server, in_buf, midQ);
		spin_lock(&GlobalMid_Lock);
		if (midQ->mid_state == MID_REQUEST_SUBMITTED) {
			/*                                        */
			midQ->callback = DeleteMidQEntry;
			spin_unlock(&GlobalMid_Lock);
			add_credits(ses->server, 1, 0);
			return rc;
		}
		spin_unlock(&GlobalMid_Lock);
	}

	rc = cifs_sync_mid_result(midQ, ses->server);
	if (rc != 0) {
		add_credits(ses->server, 1, 0);
		return rc;
	}

	if (!midQ->resp_buf || !out_buf ||
	    midQ->mid_state != MID_RESPONSE_RECEIVED) {
		rc = -EIO;
		cifs_dbg(VFS, "Bad MID state?\n");
		goto out;
	}

	*pbytes_returned = get_rfc1002_length(midQ->resp_buf);
	memcpy(out_buf, midQ->resp_buf, *pbytes_returned + 4);
	rc = cifs_check_receive(midQ, ses->server, 0);
out:
	cifs_delete_mid(midQ);
	add_credits(ses->server, 1, 0);

	return rc;
}

/*                                                    
                            */

static int
send_lock_cancel(const unsigned int xid, struct cifs_tcon *tcon,
			struct smb_hdr *in_buf,
			struct smb_hdr *out_buf)
{
	int bytes_returned;
	struct cifs_ses *ses = tcon->ses;
	LOCK_REQ *pSMB = (LOCK_REQ *)in_buf;

	/*                                            
                                                  
                                     
                              */

	pSMB->LockType = LOCKING_ANDX_CANCEL_LOCK|LOCKING_ANDX_LARGE_FILES;
	pSMB->Timeout = 0;
	pSMB->hdr.Mid = get_next_mid(ses->server);

	return SendReceive(xid, ses, in_buf, out_buf,
			&bytes_returned, 0);
}

int
SendReceiveBlockingLock(const unsigned int xid, struct cifs_tcon *tcon,
	    struct smb_hdr *in_buf, struct smb_hdr *out_buf,
	    int *pbytes_returned)
{
	int rc = 0;
	int rstart = 0;
	struct mid_q_entry *midQ;
	struct cifs_ses *ses;

	if (tcon == NULL || tcon->ses == NULL) {
		cifs_dbg(VFS, "Null smb session\n");
		return -EIO;
	}
	ses = tcon->ses;

	if (ses->server == NULL) {
		cifs_dbg(VFS, "Null tcp session\n");
		return -EIO;
	}

	if (ses->server->tcpStatus == CifsExiting)
		return -ENOENT;

	/*                                                             
                                                              
                    */

	if (be32_to_cpu(in_buf->smb_buf_length) > CIFSMaxBufSize +
			MAX_CIFS_HDR_SIZE - 4) {
		cifs_dbg(VFS, "Illegal length, greater than maximum frame, %d\n",
			 be32_to_cpu(in_buf->smb_buf_length));
		return -EIO;
	}

	rc = wait_for_free_request(ses->server, CIFS_BLOCKING_OP, 0);
	if (rc)
		return rc;

	/*                                                                     
                                                                       
                */

	mutex_lock(&ses->server->srv_mutex);

	rc = allocate_mid(ses, in_buf, &midQ);
	if (rc) {
		mutex_unlock(&ses->server->srv_mutex);
		return rc;
	}

	rc = cifs_sign_smb(in_buf, ses->server, &midQ->sequence_number);
	if (rc) {
		cifs_delete_mid(midQ);
		mutex_unlock(&ses->server->srv_mutex);
		return rc;
	}

	midQ->mid_state = MID_REQUEST_SUBMITTED;
	cifs_in_send_inc(ses->server);
	rc = smb_send(ses->server, in_buf, be32_to_cpu(in_buf->smb_buf_length));
	cifs_in_send_dec(ses->server);
	cifs_save_when_sent(midQ);

	if (rc < 0)
		ses->server->sequence_number -= 2;

	mutex_unlock(&ses->server->srv_mutex);

	if (rc < 0) {
		cifs_delete_mid(midQ);
		return rc;
	}

	/*                                                */
	rc = wait_event_interruptible(ses->server->response_q,
		(!(midQ->mid_state == MID_REQUEST_SUBMITTED)) ||
		((ses->server->tcpStatus != CifsGood) &&
		 (ses->server->tcpStatus != CifsNew)));

	/*                                   */
	if ((rc == -ERESTARTSYS) &&
		(midQ->mid_state == MID_REQUEST_SUBMITTED) &&
		((ses->server->tcpStatus == CifsGood) ||
		 (ses->server->tcpStatus == CifsNew))) {

		if (in_buf->Command == SMB_COM_TRANSACTION2) {
			/*                                                 
                               */
			rc = send_cancel(ses->server, in_buf, midQ);
			if (rc) {
				cifs_delete_mid(midQ);
				return rc;
			}
		} else {
			/*                                             
                                            */

			rc = send_lock_cancel(xid, tcon, in_buf, out_buf);

			/*                                         
                                                     */
			if (rc && rc != -ENOLCK) {
				cifs_delete_mid(midQ);
				return rc;
			}
		}

		rc = wait_for_response(ses->server, midQ);
		if (rc) {
			send_cancel(ses->server, in_buf, midQ);
			spin_lock(&GlobalMid_Lock);
			if (midQ->mid_state == MID_REQUEST_SUBMITTED) {
				/*                                        */
				midQ->callback = DeleteMidQEntry;
				spin_unlock(&GlobalMid_Lock);
				return rc;
			}
			spin_unlock(&GlobalMid_Lock);
		}

		/*                                            */
		rstart = 1;
	}

	rc = cifs_sync_mid_result(midQ, ses->server);
	if (rc != 0)
		return rc;

	/*                  */
	if (out_buf == NULL || midQ->mid_state != MID_RESPONSE_RECEIVED) {
		rc = -EIO;
		cifs_dbg(VFS, "Bad MID state?\n");
		goto out;
	}

	*pbytes_returned = get_rfc1002_length(midQ->resp_buf);
	memcpy(out_buf, midQ->resp_buf, *pbytes_returned + 4);
	rc = cifs_check_receive(midQ, ses->server, 0);
out:
	cifs_delete_mid(midQ);
	if (rstart && rc == -EACCES)
		return -ERESTARTSYS;
	return rc;
}
