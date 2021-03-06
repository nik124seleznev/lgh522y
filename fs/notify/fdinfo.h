#ifndef __FSNOTIFY_FDINFO_H__
#define __FSNOTIFY_FDINFO_H__

#include <linux/errno.h>
#include <linux/proc_fs.h>

struct seq_file;
struct file;

#ifdef CONFIG_PROC_FS

#ifdef CONFIG_INOTIFY_USER
extern int inotify_show_fdinfo(struct seq_file *m, struct file *f);
#endif

#ifdef CONFIG_FANOTIFY
extern int fanotify_show_fdinfo(struct seq_file *m, struct file *f);
#endif

#else /*                */

#define inotify_show_fdinfo	NULL
#define fanotify_show_fdinfo	NULL

#endif /*                */

#endif /*                       */
