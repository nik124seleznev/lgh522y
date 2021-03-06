#ifndef _LINUX_NTP_INTERNAL_H
#define _LINUX_NTP_INTERNAL_H

extern void ntp_init(void);
extern void ntp_clear(void);
/*                                                                   */
extern u64 ntp_tick_length(void);
extern int second_overflow(unsigned long secs);
extern int ntp_validate_timex(struct timex *);
extern int __do_adjtimex(struct timex *, struct timespec *, s32 *);
extern void __hardpps(const struct timespec *, const struct timespec *);
#endif /*                       */
