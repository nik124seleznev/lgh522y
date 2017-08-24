
#include <mach/mt_typedefs.h>
#include <mach/mt_sec_hal.h>
#include <mach/sec_osal.h>

#include "hacc_mach.h"
#include "sec_error.h"

extern int open_sdriver_connection(void);
extern int tee_secure_request(unsigned int user, unsigned char *data, unsigned int data_size, 
    unsigned int direction, unsigned char *seed, unsigned int seed_size);
extern int close_sdriver_connection(void);

/*                                          */
unsigned char masp_hal_secure_algo_init(void)
{
    bool ret = TRUE;

    return ret;
}

/*                                           */
unsigned char masp_hal_secure_algo_deinit(void)
{
    bool ret = TRUE;

    return ret;
}

/*                                         */
unsigned int masp_hal_sp_hacc_init (unsigned char *sec_seed, unsigned int size)
{
    /*                                           */
    return 0;
}

unsigned int masp_hal_sp_hacc_blk_sz (void)
{
    return AES_BLK_SZ;
}

static char* hacc_secure_request(HACC_USER user, unsigned char *buf, unsigned int buf_size, 
    BOOL bEncrypt, BOOL bDoLock, unsigned char *sec_seed, unsigned int seed_size)
{
    unsigned int ret = SEC_OK;

    /*               */
    if(TRUE == bDoLock)
    {
        /*                                                                    */            
        ret = osal_hacc_lock();

        if(ret)
        {
            ret = ERR_SBOOT_HACC_LOCK_FAIL;
            goto _exit;        
        }
    }
    /*               */
    masp_hal_secure_algo_init();


    if(buf_size != 0)
    {
        /*                               */
        if(open_sdriver_connection() < 0)
        {
            ret = ERR_HACC_OPEN_SECURE_CONNECTION_FAIL;
            goto _exit;
        }

        /*                     */
        if( (ret = tee_secure_request((unsigned int)user, buf, buf_size, (unsigned int)bEncrypt, sec_seed, seed_size)) != SEC_OK)
        {
            ret = ERR_HACC_REQUEST_SECURE_SERVICE_FAIL;
            goto _exit;
        }

        if(close_sdriver_connection() < 0)
        {
            ret = ERR_HACC_CLOSE_SECURE_CONNECTION_FAIL;
            goto _exit;
        }
    }
    else
    {
        printk("[HACC] hacc_secure_request - buffer size is 0, no encryption or decyrption is performed\n");
    }


_exit:
    /*                */
    masp_hal_secure_algo_deinit();
    /*                   */
    if(TRUE == bDoLock)
    {    
        osal_hacc_unlock();
    }

    if(ret)
    {
        printk("[HACC] hacc_secure_request fail (0x%x) (don't ASSERT)\n", ret);   
            
        //          
    }

    return buf;
}

void masp_hal_secure_algo(unsigned char Direction, unsigned int ContentAddr, unsigned int ContentLen, unsigned char *CustomSeed, unsigned char *ResText)
{    
    unsigned int err = 0;
    unsigned char *src, *dst;
    unsigned int i = 0;

    /*                      */         
    do
    {
        /*                                                                    */       
        err = osal_hacc_lock();
    }while( 0 != err );

    /*                                           */
    src = (unsigned char *)ContentAddr;
    dst = (unsigned char *)ResText;  
    
    /*                                                    */
    switch (Direction)
    {
        case TRUE:     
            dst = hacc_secure_request(HACC_USER3, (unsigned char*)src, ContentLen, TRUE, FALSE, CustomSeed, _CRYPTO_SEED_LEN);//       
            break;

        case FALSE:
            dst = hacc_secure_request(HACC_USER3, (unsigned char*)src, ContentLen, FALSE, FALSE, CustomSeed, _CRYPTO_SEED_LEN);//       
            break;

        default:
            err = ERR_KER_CRYPTO_INVALID_MODE;
            goto _wrong_direction;
    }

    /*             */
    for (i=0; i < ContentLen; i++)
    {   
        *(ResText+i) = *(dst+i);
    }

_wrong_direction:
    /*                          */
    osal_hacc_unlock();

    if(err)
    {
        printk("[HACC] masp_hal_secure_algo error (0x%x) (don't ASSERT)\n", err);   
        //          
    }
}

/*
                                                              
                                                                                                 
                                                                           
                                                                                             
 */
unsigned char* masp_hal_sp_hacc_enc(unsigned char *buf, unsigned int size, unsigned char bAC, HACC_USER user, unsigned char bDoLock)
{
    return hacc_secure_request(user, buf, size, TRUE, bDoLock, NULL, 0);
}

unsigned char* masp_hal_sp_hacc_dec(unsigned char *buf, unsigned int size, unsigned char bAC, HACC_USER user, unsigned char bDoLock)
{
    return hacc_secure_request(user, buf, size, FALSE, bDoLock, NULL, 0);
}

