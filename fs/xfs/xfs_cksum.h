#ifndef _XFS_CKSUM_H
#define _XFS_CKSUM_H 1

#define XFS_CRC_SEED	(~(__uint32_t)0)

/*
                                                                          
                                                                  
                          
 */
static inline __uint32_t
xfs_start_cksum(char *buffer, size_t length, unsigned long cksum_offset)
{
	__uint32_t zero = 0;
	__uint32_t crc;

	/*                                   */
	crc = crc32c(XFS_CRC_SEED, buffer, cksum_offset);

	/*                     */
	crc = crc32c(crc, &zero, sizeof(__u32));

	/*                                */
	return crc32c(crc, &buffer[cksum_offset + sizeof(__be32)],
		      length - (cksum_offset + sizeof(__be32)));
}

/*
                                                                
  
                                                                             
                                                                                
                                    
 */
static inline __le32
xfs_end_cksum(__uint32_t crc)
{
	return ~cpu_to_le32(crc);
}

/*
                                                
 */
static inline void
xfs_update_cksum(char *buffer, size_t length, unsigned long cksum_offset)
{
	__uint32_t crc = xfs_start_cksum(buffer, length, cksum_offset);

	*(__le32 *)(buffer + cksum_offset) = xfs_end_cksum(crc);
}

/*
                                              
 */
static inline int
xfs_verify_cksum(char *buffer, size_t length, unsigned long cksum_offset)
{
	__uint32_t crc = xfs_start_cksum(buffer, length, cksum_offset);

	return *(__le32 *)(buffer + cksum_offset) == xfs_end_cksum(crc);
}

#endif /*              */
