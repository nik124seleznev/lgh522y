#ifndef _METAG_CHECKSUM_H
#define _METAG_CHECKSUM_H

/*
                                                               
                             
  
                                                           
                       
  
                                                         
                                          
  
                                                      
 */
extern __wsum csum_partial(const void *buff, int len, __wsum sum);

/*
                                                         
            
  
                                                                     
                          
 */
extern __wsum csum_partial_copy(const void *src, void *dst, int len,
				__wsum sum);

/*
                                                             
  
                                                                     
                          
 */
extern __wsum csum_partial_copy_from_user(const void __user *src, void *dst,
					int len, __wsum sum, int *csum_err);

#define csum_partial_copy_nocheck(src, dst, len, sum)	\
	csum_partial_copy((src), (dst), (len), (sum))

/*
                          
 */
static inline __sum16 csum_fold(__wsum csum)
{
	u32 sum = (__force u32)csum;
	sum = (sum & 0xffff) + (sum >> 16);
	sum = (sum & 0xffff) + (sum >> 16);
	return (__force __sum16)~sum;
}

/*
                                                                   
                                               
 */
extern __sum16 ip_fast_csum(const void *iph, unsigned int ihl);

/*
                                                     
                                                  
 */
static inline __wsum csum_tcpudp_nofold(__be32 saddr, __be32 daddr,
					unsigned short len,
					unsigned short proto,
					__wsum sum)
{
	unsigned long len_proto = (proto + len) << 8;
	asm ("ADD    %0, %0, %1\n"
	     "ADDS   %0, %0, %2\n"
	     "ADDCS  %0, %0, #1\n"
	     "ADDS   %0, %0, %3\n"
	     "ADDCS  %0, %0, #1\n"
	     : "=d" (sum)
	     : "d" (daddr), "d" (saddr), "d" (len_proto),
	       "0" (sum)
	     : "cc");
	return sum;
}

static inline __sum16
csum_tcpudp_magic(__be32 saddr, __be32 daddr, unsigned short len,
		  unsigned short proto, __wsum sum)
{
	return csum_fold(csum_tcpudp_nofold(saddr, daddr, len, proto, sum));
}

/*
                                                                   
            
 */
extern __sum16 ip_compute_csum(const void *buff, int len);

#endif /*                   */
