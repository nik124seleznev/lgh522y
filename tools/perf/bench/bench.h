#ifndef BENCH_H
#define BENCH_H

/*
                                                                 
                                                                     
                                          
  
                                                                     
                             
 */
#ifdef __hppa__
# ifndef MADV_HUGEPAGE
#  define MADV_HUGEPAGE		67
# endif
# ifndef MADV_NOHUGEPAGE
#  define MADV_NOHUGEPAGE	68
# endif
#else
# ifndef MADV_HUGEPAGE
#  define MADV_HUGEPAGE		14
# endif
# ifndef MADV_NOHUGEPAGE
#  define MADV_NOHUGEPAGE	15
# endif
#endif

extern int bench_numa(int argc, const char **argv, const char *prefix);
extern int bench_sched_messaging(int argc, const char **argv, const char *prefix);
extern int bench_sched_pipe(int argc, const char **argv, const char *prefix);
extern int bench_mem_memcpy(int argc, const char **argv,
			    const char *prefix __maybe_unused);
extern int bench_mem_memset(int argc, const char **argv, const char *prefix);

#define BENCH_FORMAT_DEFAULT_STR	"default"
#define BENCH_FORMAT_DEFAULT		0
#define BENCH_FORMAT_SIMPLE_STR		"simple"
#define BENCH_FORMAT_SIMPLE		1

#define BENCH_FORMAT_UNKNOWN		-1

extern int bench_format;

#endif
