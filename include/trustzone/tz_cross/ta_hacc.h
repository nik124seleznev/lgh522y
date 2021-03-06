/*                                   
 */

#ifndef __TRUSTZONE_SEJ_TA__
#define __TRUSTZONE_SEJ_TA__

#define TZ_CRYPTO_TA_UUID   "0d5fe516-821d-11e2-bdb4-d485645c4311"

/*                            */
/*                                                           
                   */

/*                     */
#define TZCMD_HACC_INIT     0
#define TZCMD_HACC_INTERNAL 1
#define TZCMD_SECURE_ALGO   2

typedef struct _ta_crypto_data_ {
    unsigned int    size;
    unsigned char   bAC;
    unsigned int    user;     /*           */
    unsigned char   bDoLock;
    unsigned int    aes_type; /*         */
    unsigned char   bEn;    
} ta_crypto_data;

typedef struct _ta_secure_algo_data_ {
    unsigned char  direction;
    unsigned int   contentAddr;
    unsigned int   contentLen;
    unsigned char *customSeed;
    unsigned char *resText;
} ta_secure_algo_data;

#endif /*                           */
