/*                                                                            
  
            
            
                
  
           
           
         
  
               
               
                                                                        
  
          
          
                            
  
                                                                            */

#define pr_fmt(fmt) "[" KBUILD_MODNAME "][pfp]" fmt

#include <linux/module.h>
#include <linux/device.h>
#include <linux/vmalloc.h>
#include "ccmni_pfp.h"

/*                                     */
#undef __PFP_KERNEL_DEBUG__ 


#if 1

#define  PFP_LOG(instance_inx, flag, args...)

#else

    #define  PFP_LOG(instance_inx, flag, args...)         \
    do                                                    \
    {                                                     \
        printk(flag "CCMNI%d: ", instance_inx);           \
        printk(args);                                     \
    } while (0)

#endif

ccmni_record_t ccmni_dev          [MAX_PDP_CONT_NUM];
unsigned char  frame_cooked_data  [MAX_PFP_LEN_FIELD_VALUE];
unsigned char  frame_raw_data     [MAX_PFP_LEN_FIELD_VALUE + 4];
unsigned char  unframe_raw_data   [FREE_RAW_DATA_BUF_SIZE];
unsigned char  unframe_cooked_data[FREE_COOKED_DATA_BUF_SIZE];

#ifndef __SUPPORT_DYNAMIC_MULTIPLE_FRAME__
complete_ippkt_t complete_ippkt_pool[SUPPORT_PKT_NUM];
#endif

#ifndef __SUPPORT_DYNAMIC_MULTIPLE_FRAME__
complete_ippkt_t* get_one_available_complete_ippkt_entry()
{
    int i = 0;
    for (i=0; i < SUPPORT_PKT_NUM; i++)
    {
        if (complete_ippkt_pool[i].entry_used == 0)
        {
            complete_ippkt_pool[i].entry_used = 1;
            return &complete_ippkt_pool[i];
        }
    }
    BUG_ON(1); 
    return NULL;
}

void release_one_used_complete_ippkt_entry(complete_ippkt_t *entry)
{
    entry->pkt_size = 0;
    entry->pkt_data = NULL;
    entry->entry_used = 0;
    entry->next = NULL;
}
#endif

void pfp_reset(int ccmni_inx)
{
    ccmni_dev[ccmni_inx].unframe_state = PARSE_PFP_FRAME_START_FLAG_STATE;
    ccmni_dev[ccmni_inx].pkt_size = 0;
    ccmni_dev[ccmni_inx].last_pkt_node = NULL;
}





frame_info_t pfp_frame(unsigned char* raw_data, unsigned char* cooked_data, int cooked_size, int frame_flag, int ccmni_inx)
{
    frame_info_t entry;

    /*                                                */
    raw_data[0] = PFP_FRAME_START_FLAG;
    raw_data[1] = PFP_FRAME_MAGIC_NUM;
    /*                               */
    raw_data[2] = (cooked_size & 0x000000FF); 
    /*                                */
    raw_data[3] = ((cooked_size >> 8) & 0x000000FF);
 
    //                                                                                                                   
    /*                                                  */
    memcpy(raw_data + 4, cooked_data, cooked_size);

    entry.num_frames = 1;
    //                                                                                         
    entry.frame_list[0].frame_size = cooked_size + 4;
    entry.frame_list[0].frame_data = raw_data;
    entry.pending_data_flag = FRAME_END;
    entry.consumed_length   = cooked_size;
    
    /*                                                                       
                                                                        
              
                                                                               
                                                                   
     */
    
    return entry;
}

packet_info_t pfp_unframe(unsigned char* cooked_data, int cooked_data_buf_size,
                          unsigned char* raw_data, int raw_size, int ccmni_inx)
{
    /*                                                                       
                                                                   
     */
    
    //                                                                                
    //                                            
    
    int local_raw_size                  = raw_size;
    int local_cooked_data_buf_free_size = cooked_data_buf_size;
    unsigned char *local_raw_data       = raw_data;
    unsigned char *local_cooked_data    = cooked_data;
    int consumed_length                 = 0;
    int keep_parsing                    = 1;
    int retrieved_num_ip_pkts           = 0;
    packet_info_t entry                 = {0};

    /*                                                                                */
    ccmni_dev[ccmni_inx].last_pkt_node = NULL;
    
    do
    {
#ifdef __PFP_KERNEL_DEBUG__
        pr_notice("CCMNI%d: pfp_unframe_state=%d\n",ccmni_inx,ccmni_dev[ccmni_inx].unframe_state);
#endif
        switch(ccmni_dev[ccmni_inx].unframe_state)
        {
            case PARSE_PFP_FRAME_START_FLAG_STATE:
            {
                //                                                               
#ifdef __PFP_KERNEL_DEBUG__                
                pr_notice("CCMNI%d: check start flag-local_raw_data[0]=0x%02x\n",ccmni_inx,local_raw_data[0]);
#endif
                if (local_raw_data[0] == PFP_FRAME_START_FLAG)
                {
                    /* 
                                                                            
                                                                          
                    */
                    local_raw_data++;
                    local_raw_size--;
                    consumed_length++;                    
                    break;   
                }
                else
                {
                    /*                                            */
                    ccmni_dev[ccmni_inx].unframe_state = PARSE_PFP_FRAME_MAGIC_NUM_STATE;
                }
                
            }
            
            case PARSE_PFP_FRAME_MAGIC_NUM_STATE:
            {
                //                                                              
#ifdef __PFP_KERNEL_DEBUG__                
                pr_notice("CCMNI%d: check magic num-local_raw_data[0]=0x%02x\n",ccmni_inx,local_raw_data[0]);
#endif
                
                if (local_raw_data[0] != PFP_FRAME_MAGIC_NUM)
                {
                    /*                                                          */
                    ccmni_dev[ccmni_inx].unframe_state = PARSE_PFP_FRAME_START_FLAG_STATE;
                }
                else
                {
                    ccmni_dev[ccmni_inx].unframe_state = PARSE_PFP_FRAME_LENGTH_FIELD_STATE;
                }
                
                local_raw_data++;
                local_raw_size--;
                consumed_length++;
                
                break;
            }
            
            case PARSE_PFP_FRAME_LENGTH_FIELD_STATE:
            {
                /*                                                                     */
#ifdef __PFP_KERNEL_DEBUG__                
                pr_notice("CCMNI%d: local_raw_size=%d\n",ccmni_inx,local_raw_size);
#endif
                if (local_raw_size >= 2)
                {
                    ccmni_dev[ccmni_inx].pkt_size = ((local_raw_data[1] << 8) | local_raw_data[0]);

#ifdef __PFP_KERNEL_DEBUG__
                    pr_notice("CCMNI%d: pkt_size=%d,len[0]=0x%02x,len[1]=0x%02x\n",ccmni_inx,ccmni_dev[ccmni_inx].pkt_size,local_raw_data[0],local_raw_data[1]);
#endif                    
                    /*                        */
                    local_raw_data  += 2;
                    local_raw_size  -= 2;
                    consumed_length += 2;

                    /*                                                                              */
                    if (ccmni_dev[ccmni_inx].pkt_size <= MAX_PFP_LEN_FIELD_VALUE)
                    {
                        ccmni_dev[ccmni_inx].unframe_state = PARSE_PFP_FRAME_GET_DATA_STATE;
                    }
                    else
                    {
                        /*                                                         */
#ifdef __PFP_KERNEL_DEBUG__
                        pr_notice("CCMNI%d: Reset decode state then continue to parse\n",ccmni_inx);
#endif
                        ccmni_dev[ccmni_inx].unframe_state = PARSE_PFP_FRAME_START_FLAG_STATE;
                        ccmni_dev[ccmni_inx].pkt_size      = 0;
                    }
                }
                else
                {
                    /*
                                                                                
                                                                               
                     */
#ifdef __PFP_KERNEL_DEBUG__                    
                    pr_notice("CCMNI%d: not enough len bytes\n",ccmni_inx);
#endif
                    keep_parsing = 0;
                }
                
                break;
            }
            
            case PARSE_PFP_FRAME_GET_DATA_STATE:
            {
                if (local_raw_size < ccmni_dev[ccmni_inx].pkt_size)
                {
                    /*
                                                                            
                                                                                  
                     */
#ifdef __PFP_KERNEL_DEBUG__                    
                    pr_notice("CCMNI%d: not enough pkt bytes\n",ccmni_inx);
#endif
                    keep_parsing = 0;
                }
                else
                {
                    /*
                                                                                 
                                                         
                     */
                    
                    if (local_cooked_data_buf_free_size >= ccmni_dev[ccmni_inx].pkt_size)
                    {                       
                        complete_ippkt_t *current_node  = entry.pkt_list;
                        complete_ippkt_t *previous_node = ccmni_dev[ccmni_inx].last_pkt_node;

                        /*
                                                                                          
                                                      
                         */
                        
                        if (current_node == NULL)
                        {
#ifdef __PFP_KERNEL_DEBUG__
                            pr_notice("CCMNI%d: malloc for first pkt node\n",ccmni_inx);
#endif
#ifdef __SUPPORT_DYNAMIC_MULTIPLE_FRAME__                             
                            current_node   = (complete_ippkt_t *) vmalloc(sizeof(complete_ippkt_t));
#else
                            /*                          */
                            current_node   = (complete_ippkt_t *) get_one_available_complete_ippkt_entry();
#endif                      
                            if (current_node != NULL)
                            {
                                entry.pkt_list = current_node;
                            }
                            else
                            {
                                /*              */
#ifdef __PFP_KERNEL_DEBUG__
                                pr_notice("CCMNI%d: Can't find one available complete_ippkt entry case1\n",ccmni_inx);
#endif
                                entry.try_decode_again = 1;
                                goto error_handle_return;
                            }
                        }
                        else
                        { 
#if 0
                            do
                            {   
                                previous_node = current_node;
                                current_node  = current_node->next;   
                            }
                            while(current_node != NULL);
#endif

#ifdef __SUPPORT_DYNAMIC_MULTIPLE_FRAME__
                            /*                                                                    */
                            current_node = (complete_ippkt_t *) vmalloc(sizeof(complete_ippkt_t));
#else
                            /*                          */
                            current_node   = (complete_ippkt_t *) get_one_available_complete_ippkt_entry();
#endif
                            if (current_node != NULL)
                            {
                                previous_node->next = current_node;
                            }
                            else
                            {
                                /*              */
#ifdef __PFP_KERNEL_DEBUG__
                                pr_notice("CCMNI%d: Can't find one available complete_ippkt entry case2\n",ccmni_inx);
#endif                        
                                entry.try_decode_again = 1;
                                goto error_handle_return;
                            }
                        }
#ifdef __PFP_KERNEL_DEBUG__                        
                        pr_notice("CCMNI%d: prepare pkt node\n",ccmni_inx);
#endif
                        current_node->pkt_size = ccmni_dev[ccmni_inx].pkt_size;
                        
                        /*                                                  */
                        memcpy(local_cooked_data, local_raw_data, ccmni_dev[ccmni_inx].pkt_size);


                        //                                                                                                                                          

                        current_node->pkt_data = local_cooked_data;
                        local_cooked_data     += ccmni_dev[ccmni_inx].pkt_size;
                        current_node->next     = NULL;
                        
                        /*
                                                                                                 
                                                                                          
                         */
                        
                        ccmni_dev[ccmni_inx].last_pkt_node = current_node;
                        
                        local_raw_data += ccmni_dev[ccmni_inx].pkt_size;
                        local_raw_size -= ccmni_dev[ccmni_inx].pkt_size;
                        local_cooked_data_buf_free_size -= ccmni_dev[ccmni_inx].pkt_size;
                        
                        /*
                                                                                                  
                                           
                         */
                        
                        consumed_length += ccmni_dev[ccmni_inx].pkt_size;
                        retrieved_num_ip_pkts++;
                        
                        /*                                        */
                        ccmni_dev[ccmni_inx].unframe_state = PARSE_PFP_FRAME_START_FLAG_STATE;
                        ccmni_dev[ccmni_inx].pkt_size      = 0;
                    }
                    else
                    {
                        /*
                                                                                              
                                                                               
                         */
#ifdef __PFP_KERNEL_DEBUG__                        
                        pr_notice("CCMNI%d: not enough free space provided by cooked_data\n",ccmni_inx);
#endif
                        keep_parsing = 0;
                    }
                }
                break;
            }
            
            default:
            {
                break;
            }
        }    
    }
    while (local_raw_size > 0 && keep_parsing == 1);
error_handle_return:
    entry.consumed_length      = consumed_length;
    entry.num_complete_packets = retrieved_num_ip_pkts;
    entry.parse_data_state     = ccmni_dev[ccmni_inx].unframe_state;
   
    return entry;
}


void traverse_pkt_list(complete_ippkt_t *node)
{
    complete_ippkt_t *t_pkt_node    = node;
    complete_ippkt_t *prev_pkt_node = NULL;
    
    while (t_pkt_node != NULL)
    {
#ifdef __PFP_KERNEL_DEBUG__
        pr_notice("Packet Node: data=0x%08x, size=%d\n", t_pkt_node->pkt_data, t_pkt_node->pkt_size);
#endif
        prev_pkt_node = t_pkt_node;
        t_pkt_node    = t_pkt_node->next;
        
        vfree(prev_pkt_node);
    }
}

EXPORT_SYMBOL(pfp_frame);
EXPORT_SYMBOL(pfp_unframe);
EXPORT_SYMBOL(release_one_used_complete_ippkt_entry);

