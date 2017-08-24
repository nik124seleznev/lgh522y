/*
                                                                 
                                                                          
  
                                                             
                                                             
                                           
  
                                                            
                                                                
                                                             
  
                                                                   
                                                                   
        
  
                                                 
  
                                       
  
                                                            
                                            
 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM modem_ut

/*
                                                               
                                                     
  
                                       
  
                       
 */
#if !defined(_TRACE_EVENT_SAMPLE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_EVENT_SAMPLE_H

/*
                                                                  
                                  
 */
#include <linux/tracepoint.h>

/*
                                                   
  
                                                                            
                                                       
  
                                                       
                                                  
  
                                                    
                                   
  
                                                                            
                                                                     
                                                                     
                        
                                                                   
                                                                       
                   
  
                                                                   
  
                                                                         
                           
  
                                                                       
                                                                         
                                                                        
  
                                                                        
                                                                  
                    
 */
TRACE_EVENT(process_count,

	TP_PROTO(unsigned int rx_cnt, unsigned int tx_cnt, unsigned int rx_rate, unsigned int tx_rate),

	TP_ARGS(rx_cnt, tx_cnt, rx_rate, tx_rate),

	TP_STRUCT__entry(
		__field(	unsigned int,	rx_cnt			)
		__field(	unsigned int,	tx_cnt			)
		__field(	unsigned int,	rx_rate			)
		__field(	unsigned int,	tx_rate			)
	),

	TP_fast_assign(
		__entry->rx_cnt	= rx_cnt;
		__entry->tx_cnt	= tx_cnt;
		__entry->rx_rate	= rx_rate;
		__entry->tx_rate	= tx_rate;
	),

	TP_printk("%u,%u,%u,%u", __entry->rx_cnt, __entry->tx_cnt, __entry->rx_rate, __entry->tx_rate)
);
#endif

/*                                               */


/*
                                                                   
                                                                 
                                  
  
                                                                   
        
  
                                                        
  
                                                                     
                                                 
  
                                           
  
                                                              
                                                             
  
                                                              
  
                                           
  
                                                                     
  
                                                  
  
                                                                       
                                                                         
          
 */
#undef TRACE_INCLUDE_PATH
#undef TRACE_INCLUDE_FILE
#define TRACE_INCLUDE_PATH .
/*
                                                                              
 */
#define TRACE_INCLUDE_FILE modem_ut_events
#include <trace/define_trace.h>
