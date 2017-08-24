#define CONSOLE_BUF (128*1024) 
/*
                           
                      
                      
                      
                       
                      
                      
*/
extern char *usb_buf ; 
extern volatile u32 buf_len ;
extern u8 data_ep_num;
extern char *usb_buf_readp ;

extern struct usb_ep *acm_in_ep ; 
extern volatile bool cdc_set_contr ;
extern volatile bool usb_connected ;
extern volatile bool com_opend ;  //                              
extern bool gadget_is_ready;


extern u32 send_data(u32 ep_num,u8 *pbuffer,u32 data_len);
extern void format_and_send_string(const char *s,unsigned int count);
extern struct tty_driver *usb_console_device(struct console *co, int *index);
