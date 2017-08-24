#ifndef SECIMGHEADER_H
#define SECIMGHEADER_H

/*                                                                         
                    
                                                                          */
#define CUSTOM_NAME                  "CUSTOM_NAME"
#define IMAGE_VERSION                "IMAGE_VERSION"

/*                                                                    
                                                   */
#define VERIFY_OFFSET                "VERIFY_OFFSET"
#define VERIFY_LENGTH                "VERIFY_LENGTH"

/*                                                                         
                           
                                                                          */
#define SEC_IMG_MAGIC                (0x53535353)
#define SEC_IMG_MAGIC_LEN            (4)

#define SEC_IMG_HEADER_SIZE          (64)

#define HASH_SIZE                    (20)
#define RSA_KEY_SIZE                 (128)
#define SIG_LEN                      (RSA_KEY_SIZE)
#define HASH_SIG_LEN                 (HASH_SIZE+SIG_LEN) 
                                     /*                  */
/*                       */
typedef struct _SEC_IMG_HEADER_V1
{
    unsigned int magic_number;
    
    unsigned char cust_name [16];
    unsigned int image_version;
    unsigned int image_length;  
    unsigned int image_offset;
    
    unsigned int sign_offset;
    unsigned int sign_length;
    
    unsigned int signature_offset;
    unsigned int signature_length;

    unsigned char dummy[16];

} SEC_IMG_HEADER_V1;

/*                   */ 
typedef struct _SEC_IMG_HEADER_V2
{
    unsigned int magic_number;

    /*                                                                 
                                                                               
                                                                              
                         
    
                                                       
                                                                              
                                                                          */
              
    unsigned char cust_name [32];
    unsigned int image_version;
    unsigned int image_length;  
    unsigned int image_offset;
    
    unsigned int sign_offset;
    unsigned int sign_length;
    
    unsigned int signature_offset;
    unsigned int signature_length;
    
} SEC_IMG_HEADER_V2, SEC_IMG_HEADER_V3, SEC_IMG_HEADER;

typedef struct _SEC_IMG_HEADER_V4
{
    unsigned int magic_number;

    unsigned char cust_name [32];
    unsigned int image_verion;
    unsigned int signature_length;  
    unsigned int image_offset;
    
    unsigned int ext_magic;
    unsigned int ext_hdr_length;

    unsigned int image_length_high;
    unsigned int image_length_low;
} SEC_IMG_HEADER_V4;


typedef union
{
    SEC_IMG_HEADER_V1 v1;
    SEC_IMG_HEADER_V2 v2;
    SEC_IMG_HEADER_V3 v3;
    SEC_IMG_HEADER_V4 v4;
    /*                */
    SEC_IMG_HEADER va;
} SEC_IMG_HEADER_U;

typedef enum
{
    SEC_HDR_V1 = 1,
    SEC_HDR_V2 = 2,
    SEC_HDR_V3 = 3,
    SEC_HDR_V4 = 4,
    UNSET
    
} SEC_IMG_HEADER_VER;

/*                                                                         
                                
                                                                          */

#define SEC_USBDL_WRITE_IMAGE_HEADER 0x11
#define SEC_USBDL_WRITE_IMAGE_SIGNATURE_HASH 0x22
#define SEC_USBDL_IMAGE_INFO_CHECK_RESULT 0x33
#define SEC_USBDL_WRITE_IMAGE_NAME 0x44
#define SEC_USBDL_WRITE_IMAGE_OFFSET 0x55
#define SEC_USBDL_WRITE_TYPE 0x66
#define SEC_USBDL_WRITE_IMAGE_EXTENSION 0x77


#define SEC_USBDL_IMAGE_NAME_LEN 16    
#define SEC_USBDL_IMAGE_INFO_RESULT_PASS 0x5555
#define SEC_USBDL_IMAGE_INFO_RESULT_FAIL 0x4444  


#endif   /*                */
