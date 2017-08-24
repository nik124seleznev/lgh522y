#ifndef _CIPHERHEADER_H
#define _CIPHERHEADER_H

/*                                                                         
                        
                                                                          */

#define CUSTOM_NAME                  "CUSTOM_NAME"
#define IMAGE_VERSION                "IMAGE_VERSION"
#define CIPHER_IMG_MAGIC             (0x63636363)
#define CIPHER_IMG_HEADER_SIZE       (128)
 
typedef struct _SEC_CIPHER_IMG_HEADER
{
    unsigned int magic_number;
    
    unsigned char cust_name [32];
    unsigned int image_version;
    unsigned int image_length;  
    unsigned int image_offset;
    
    unsigned int cipher_offset;
    unsigned int cipher_length;    

    /*
                  
                               
     */
    unsigned int aes_version;
    unsigned char dummy[68];

}CIPHER_HEADER;

#endif   /*               */


