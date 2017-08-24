/*
* Copyright (C) 2011-2014 MediaTek Inc.
* 
* This program is free software: you can redistribute it and/or modify it under the terms of the 
* GNU General Public License version 2 as published by the Free Software Foundation.
* 
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include "met_drv.h"
#include "mt_gpufreq.h"

extern struct metdevice met_gpu;
static struct delayed_work dwork;

//                                
//                                                          
#define FMT7	"%d,%d,%d,%d,%d\n"
#define VAL7	value[0],value[1],value[2],value[3],value[4]

#define SAMPLE_FMT	"%5lu.%06lu"
#define SAMPLE_VAL	(unsigned long)(timestamp), nano_rem/1000

static const int metDelay = 20;

extern unsigned long gpu_get_current_utilization(void);

void ms_gpu(unsigned long long timestamp, unsigned char cnt, unsigned int *value)
{
    //                                                       

    switch (cnt) {
    //                                                               
    case 5: trace_printk(FMT7, VAL7); break;
    }
}

unsigned int mt_gpufreq_cur_load(void)
{
    unsigned int loading = (unsigned int)gpu_get_current_utilization();
    if(loading > 100)
    {
        return 100;
    }
    else
    {
        return loading;
    }
}

unsigned int mt_gpufreq_cur_freq(void)
{
    return 500000; //      
}

static void wq_get_sample(struct work_struct *work)
{
    if (met_gpu.mode) {
        int cpu;
        unsigned int value[7];
        unsigned long long stamp;

        cpu = smp_processor_id();
        stamp = cpu_clock(cpu);

        value[0] = (mt_gpufreq_cur_freq()/1000);  //         
        value[1] = mt_gpufreq_cur_load();      //               
        value[2] = 0;                             //             
        value[3] = 0;                             //             
        value[4] = 0;                             //                          

        ms_gpu(stamp, 5, value);
        msleep(metDelay);
        schedule_delayed_work(&dwork, 0);
    }
}

//                                                 
static void sample_start(void)
{
	INIT_DELAYED_WORK(&dwork, wq_get_sample);
	schedule_delayed_work(&dwork, 0);
	return;
}

//                                                
static void sample_stop(void)
{
	cancel_delayed_work_sync(&dwork);
	return;
}

//                                                                                                                     
static char header[] = "met-info [000] 0.0: ms_ud_sys_header: ms_gpu,clock (MHz),util (%),GPFR,PPFR,ICC,d,d,d,d,d\n";
static char help[] = "  --gpu                                 monitor gpu\n";


//                                            
static int sample_print_help(char *buf, int len)
{
	return snprintf(buf, PAGE_SIZE, help);
}

//                                                                 
static int sample_print_header(char *buf, int len)
{
	return snprintf(buf, PAGE_SIZE, header);
}

struct metdevice met_gpu = {
	.name = "gpu",
	.owner = THIS_MODULE,
	.type = MET_TYPE_BUS,
	.start = sample_start,
	.stop = sample_stop,
	.print_help = sample_print_help,
	.print_header = sample_print_header,
};

EXPORT_SYMBOL(met_gpu);
