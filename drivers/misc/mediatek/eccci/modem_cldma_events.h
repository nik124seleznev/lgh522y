/*
                                                                 
                                                                          
  
                                                             
                                                             
                                           
  
                                                            
                                                                
                                                             
  
                                                                   
                                                                   
        
  
                                                 
  
                                       
  
                                                            
                                            
 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM modem_cd

/*
                                                               
                                                     
  
                                       
  
                       
 */
#if !defined(_TRACE_EVENT_SAMPLE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_EVENT_SAMPLE_H

/*
                                                                  
                                  
 */
#include <linux/tracepoint.h>

/*
                                                   
  
                                                                            
                                                       
  
                                                       
                                                  
  
                                                    
                                   
  
                                                                            
                                                                     
                                                                     
                        
                                                                   
                                                                       
                   
  
                                                                   
  
                                                                         
                           
  
                                                                       
                                                                         
                                                                        
  
                                                                        
                                                                  
                    
 */
TRACE_EVENT(md_tx_free,

	TP_PROTO(int *tx_free),

	TP_ARGS(tx_free),

	TP_STRUCT__entry(
		__field(	int *,	tx_free			)
	),

	TP_fast_assign(
		__entry->tx_free	= tx_free;
	),

	TP_printk("%d,%d,%d,%d,%d,%d,%d,%d", __entry->tx_free[0], __entry->tx_free[1], __entry->tx_free[2], __entry->tx_free[3],
										__entry->tx_free[4], __entry->tx_free[5], __entry->tx_free[6], __entry->tx_free[7])
);

TRACE_EVENT(md_rx_used,

	TP_PROTO(int *rx_used),

	TP_ARGS(rx_used),

	TP_STRUCT__entry(
		__field(	int *,	rx_used			)
	),

	TP_fast_assign(
		__entry->rx_used	= rx_used;
	),

	TP_printk("%d,%d,%d,%d,%d,%d,%d,%d", __entry->rx_used[0], __entry->rx_used[1], __entry->rx_used[2], __entry->rx_used[3],
										__entry->rx_used[4], __entry->rx_used[5], __entry->rx_used[6], __entry->rx_used[7])
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
#define TRACE_INCLUDE_FILE modem_cldma_events
#include <trace/define_trace.h>
