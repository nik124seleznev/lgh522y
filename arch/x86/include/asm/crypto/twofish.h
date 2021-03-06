#ifndef ASM_X86_TWOFISH_H
#define ASM_X86_TWOFISH_H

#include <linux/crypto.h>
#include <crypto/twofish.h>
#include <crypto/lrw.h>
#include <crypto/b128ops.h>

struct twofish_lrw_ctx {
	struct lrw_table_ctx lrw_table;
	struct twofish_ctx twofish_ctx;
};

struct twofish_xts_ctx {
	struct twofish_ctx tweak_ctx;
	struct twofish_ctx crypt_ctx;
};

/*                                                           */
asmlinkage void twofish_enc_blk(struct twofish_ctx *ctx, u8 *dst,
				const u8 *src);
asmlinkage void twofish_dec_blk(struct twofish_ctx *ctx, u8 *dst,
				const u8 *src);

/*                                 */
asmlinkage void __twofish_enc_blk_3way(struct twofish_ctx *ctx, u8 *dst,
				       const u8 *src, bool xor);
asmlinkage void twofish_dec_blk_3way(struct twofish_ctx *ctx, u8 *dst,
				     const u8 *src);

/*                                 */
asmlinkage void twofish_ecb_enc_8way(struct twofish_ctx *ctx, u8 *dst,
				     const u8 *src);
asmlinkage void twofish_ecb_dec_8way(struct twofish_ctx *ctx, u8 *dst,
				     const u8 *src);
asmlinkage void twofish_cbc_dec_8way(struct twofish_ctx *ctx, u8 *dst,
				     const u8 *src);
asmlinkage void twofish_ctr_8way(struct twofish_ctx *ctx, u8 *dst,
				 const u8 *src, le128 *iv);
asmlinkage void twofish_xts_enc_8way(struct twofish_ctx *ctx, u8 *dst,
				     const u8 *src, le128 *iv);
asmlinkage void twofish_xts_dec_8way(struct twofish_ctx *ctx, u8 *dst,
				     const u8 *src, le128 *iv);

/*                                         */
extern void twofish_dec_blk_cbc_3way(void *ctx, u128 *dst, const u128 *src);
extern void twofish_enc_blk_ctr(void *ctx, u128 *dst, const u128 *src,
				le128 *iv);
extern void twofish_enc_blk_ctr_3way(void *ctx, u128 *dst, const u128 *src,
				     le128 *iv);

extern int lrw_twofish_setkey(struct crypto_tfm *tfm, const u8 *key,
			      unsigned int keylen);

extern void lrw_twofish_exit_tfm(struct crypto_tfm *tfm);

extern int xts_twofish_setkey(struct crypto_tfm *tfm, const u8 *key,
			      unsigned int keylen);

/*                                 */
extern void twofish_xts_enc(void *ctx, u128 *dst, const u128 *src, le128 *iv);
extern void twofish_xts_dec(void *ctx, u128 *dst, const u128 *src, le128 *iv);

#endif /*                   */
