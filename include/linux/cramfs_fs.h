#ifndef __CRAMFS_H
#define __CRAMFS_H

#include <uapi/linux/cramfs_fs.h>

/*                                                 */
int cramfs_uncompress_block(void *dst, int dstlen, void *src, int srclen);
int cramfs_uncompress_init(void);
void cramfs_uncompress_exit(void);
#endif
