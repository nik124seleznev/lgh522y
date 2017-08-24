/*
 * include/asm-xtensa/platform-iss/simcall.h
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2001 Tensilica Inc.
 */

#ifndef _XTENSA_PLATFORM_ISS_SIMCALL_H
#define _XTENSA_PLATFORM_ISS_SIMCALL_H


/*
                                                            
 */

#define SYS_nop		0	/*        */
#define SYS_exit	1	/* */
#define SYS_fork	2
#define SYS_read	3	/* */
#define SYS_write	4	/* */
#define SYS_open	5	/* */
#define SYS_close	6	/* */
#define SYS_rename	7	/*               */
#define SYS_creat	8	/* */
#define SYS_link	9	/*                             */
#define SYS_unlink	10	/* */
#define SYS_execv	11	/*              */
#define SYS_execve	12	/*            */
#define SYS_pipe	13	/*           */
#define SYS_stat	14	/*             */
#define SYS_chmod	15
#define SYS_chown	16	/*              */
#define SYS_utime	17	/*            */
#define SYS_wait	18	/*               */
#define SYS_lseek	19	/* */
#define SYS_getpid	20
#define SYS_isatty	21	/*             */
#define SYS_fstat	22	/*                 */
#define SYS_time	23	/*             */
#define SYS_gettimeofday 24	/*                                         */
#define SYS_times	25	/*                                              */
#define SYS_socket      26
#define SYS_sendto      27
#define SYS_recvfrom    28
#define SYS_select_one  29      /*                                                        */
#define SYS_bind        30
#define SYS_ioctl	31

/*
                            
 */

#define  XTISS_SELECT_ONE_READ    1
#define  XTISS_SELECT_ONE_WRITE   2
#define  XTISS_SELECT_ONE_EXCEPT  3

static int errno;

static inline int __simc(int a, int b, int c, int d)
{
	int ret;
	register int a1 asm("a2") = a;
	register int b1 asm("a3") = b;
	register int c1 asm("a4") = c;
	register int d1 asm("a5") = d;
	__asm__ __volatile__ (
			"simcall\n"
			"mov %0, a2\n"
			"mov %1, a3\n"
			: "=a" (ret), "=a" (errno), "+r"(a1), "+r"(b1)
			: "r"(c1), "r"(d1)
			: "memory");
	return ret;
}

static inline int simc_open(const char *file, int flags, int mode)
{
	return __simc(SYS_open, (int) file, flags, mode);
}

static inline int simc_close(int fd)
{
	return __simc(SYS_close, fd, 0, 0);
}

static inline int simc_ioctl(int fd, int request, void *arg)
{
	return __simc(SYS_ioctl, fd, request, (int) arg);
}

static inline int simc_read(int fd, void *buf, size_t count)
{
	return __simc(SYS_read, fd, (int) buf, count);
}

static inline int simc_write(int fd, const void *buf, size_t count)
{
	return __simc(SYS_write, fd, (int) buf, count);
}

static inline int simc_poll(int fd)
{
	struct timeval tv = { .tv_sec = 0, .tv_usec = 0 };

	return __simc(SYS_select_one, fd, XTISS_SELECT_ONE_READ, (int)&tv);
}

static inline int simc_lseek(int fd, uint32_t off, int whence)
{
	return __simc(SYS_lseek, fd, off, whence);
}

#endif /*                                */

