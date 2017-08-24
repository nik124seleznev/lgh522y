/*
                                                                  
                                            
 */

#ifndef _UAPICRYPTOCOP_H
#define _UAPICRYPTOCOP_H

#include <linux/uio.h>


#define CRYPTOCOP_SESSION_ID_NONE (0)

typedef unsigned long long int cryptocop_session_id;

/*                  */
#define ETRAXCRYPTOCOP_IOCTYPE         (250)

#define CRYPTOCOP_IO_CREATE_SESSION    _IOWR(ETRAXCRYPTOCOP_IOCTYPE, 1, struct strcop_session_op)
#define CRYPTOCOP_IO_CLOSE_SESSION     _IOW(ETRAXCRYPTOCOP_IOCTYPE, 2, struct strcop_session_op)
#define CRYPTOCOP_IO_PROCESS_OP        _IOWR(ETRAXCRYPTOCOP_IOCTYPE, 3, struct strcop_crypto_op)
#define CRYPTOCOP_IO_MAXNR             (3)

typedef enum {
	cryptocop_cipher_des = 0,
	cryptocop_cipher_3des = 1,
	cryptocop_cipher_aes = 2,
	cryptocop_cipher_m2m = 3, /*                                                         */
	cryptocop_cipher_none
} cryptocop_cipher_type;

typedef enum {
	cryptocop_digest_sha1 = 0,
	cryptocop_digest_md5 = 1,
	cryptocop_digest_none
} cryptocop_digest_type;

typedef enum {
	cryptocop_csum_le = 0,
	cryptocop_csum_be = 1,
	cryptocop_csum_none
} cryptocop_csum_type;

typedef enum {
	cryptocop_cipher_mode_ecb = 0,
	cryptocop_cipher_mode_cbc,
	cryptocop_cipher_mode_none
} cryptocop_cipher_mode;

typedef enum {
	cryptocop_3des_eee = 0,
	cryptocop_3des_eed = 1,
	cryptocop_3des_ede = 2,
	cryptocop_3des_edd = 3,
	cryptocop_3des_dee = 4,
	cryptocop_3des_ded = 5,
	cryptocop_3des_dde = 6,
	cryptocop_3des_ddd = 7
} cryptocop_3des_mode;

/*                                         */
struct strcop_session_op{
	cryptocop_session_id    ses_id;

	cryptocop_cipher_type   cipher; /*                           */

	cryptocop_cipher_mode   cmode; /*                */
	cryptocop_3des_mode     des3_mode;

	cryptocop_digest_type   digest; /*                 */

	cryptocop_csum_type     csum;   /*              */

	unsigned char           *key;
	size_t                  keylen;
};

#define CRYPTOCOP_CSUM_LENGTH         (2)
#define CRYPTOCOP_MAX_DIGEST_LENGTH   (20)  /*                  */
#define CRYPTOCOP_MAX_IV_LENGTH       (16)  /*                      */
#define CRYPTOCOP_MAX_KEY_LENGTH      (32)

struct strcop_crypto_op{
	cryptocop_session_id ses_id;

	/*         */
	unsigned char            *indata;
	size_t                   inlen; /*                      */

	/*                       */
	unsigned char            do_cipher:1;
	unsigned char            decrypt:1; /*                            */
	unsigned char            cipher_explicit:1;
	size_t                   cipher_start;
	size_t                   cipher_len;
	/*                                                                  
                                                                 
                                              */
	unsigned char            cipher_iv[CRYPTOCOP_MAX_IV_LENGTH];
	/*          */
	unsigned char            *cipher_outdata;
	size_t                   cipher_outlen;

	/*                       */
	unsigned char            do_digest:1;
	size_t                   digest_start;
	size_t                   digest_len;
	/*                                                                      */
	unsigned char            digest[CRYPTOCOP_MAX_DIGEST_LENGTH];

	/*                         */
	unsigned char            do_csum:1;
	size_t                   csum_start;
	size_t                   csum_len;
	/*          */
	unsigned char            csum[CRYPTOCOP_CSUM_LENGTH];
};




#endif /*                  */
