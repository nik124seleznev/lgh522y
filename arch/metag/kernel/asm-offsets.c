/*
                                                         
                             
  
 */

#include <linux/kbuild.h>
#include <linux/thread_info.h>

int main(void)
{
	DEFINE(THREAD_INFO_SIZE, sizeof(struct thread_info));
	return 0;
}
