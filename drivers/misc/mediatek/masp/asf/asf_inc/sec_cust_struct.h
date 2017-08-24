#ifndef _CUST_SEC_H
#define _CUST_SEC_H

/*                                                                         
                     
                                                                          */
#define RSA_KEY_LEN_2048                    (0)
#define RSA_KEY_LEN_1024                    (1)

/*                                                                         
                      
                                                                          */
#if RSA_KEY_LEN_2048
#if RSA_KEY_LEN_1024
#error "RSA_KEY_LEN_1024 should be disabled"
#endif
#endif

#if RSA_KEY_LEN_1024
#if RSA_KEY_LEN_2048
#error "RSA_KEY_LEN_2048 should be disabled"
#endif
#endif

/*                                                                         
                           
                                                                          */
#define RSA_KEY_MAX_LEN                     (128) //      
#define RSA_E_KEY_LEN                       (5) //      

#if RSA_KEY_LEN_2048
#define RSA_KEY_LEN                         (256) //      
#define HASH_LEN                            (32)  //      

#elif RSA_KEY_LEN_1024
#define RSA_KEY_LEN                         (128) //      
#define HASH_LEN                            (20)  //      

#else
#error "RSA_KEY_LEN is not defined"

#endif

/*                                                                         
                      
                                                                          */
typedef struct _CUST_SEC_INTER
{
    /*                         */
    unsigned char                   key_rsa_n[RSA_KEY_MAX_LEN*2]; //                         
    unsigned char                   key_rsa_d[RSA_KEY_MAX_LEN*2]; //                         
    unsigned char                   key_rsa_e[RSA_E_KEY_LEN]; //                         
} CUST_SEC_INTER;

#endif   /*           */
