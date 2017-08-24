/*
                                           
                           
*/
#ifdef CONFIG_SCHEDSTATS
extern void log_boot(char *str);
#else
static inline void log_boot(char *str)
{
}
#endif
