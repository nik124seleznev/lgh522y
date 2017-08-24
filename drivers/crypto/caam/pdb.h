/*
 * CAAM Protocol Data Block (PDB) definition header file
 *
 * Copyright 2008-2012 Freescale Semiconductor, Inc.
 *
 */

#ifndef CAAM_PDB_H
#define CAAM_PDB_H

/*
                                             
 */
#define PDBHMO_ESP_DECAP_SHIFT	12
#define PDBHMO_ESP_ENCAP_SHIFT	4
/*
                                                                          
                                         
                                                             
                                                                  
*/
#define PDBHMO_ESP_DECAP_DEC_TTL	(0x02 << PDBHMO_ESP_DECAP_SHIFT)
#define PDBHMO_ESP_ENCAP_DEC_TTL	(0x02 << PDBHMO_ESP_ENCAP_SHIFT)
/*
                                                                       
                                                   
 */
#define PDBHMO_ESP_DIFFSERV		(0x01 << PDBHMO_ESP_DECAP_SHIFT)
/*
                                                                            
                                                                            
 */
#define PDBHMO_ESP_DFBIT		(0x04 << PDBHMO_ESP_ENCAP_SHIFT)

/*
                                      
 */
#define PDBOPTS_ESP_ARSNONE	0x00 /*                      */
#define PDBOPTS_ESP_ARS32	0x40 /*                            */
#define PDBOPTS_ESP_ARS64	0xc0 /*                            */
#define PDBOPTS_ESP_IVSRC	0x20 /*                                   */
#define PDBOPTS_ESP_ESN		0x10 /*                            */
#define PDBOPTS_ESP_OUTFMT	0x08 /*                                   */
#define PDBOPTS_ESP_IPHDRSRC	0x08 /*                                  */
#define PDBOPTS_ESP_INCIPHDR	0x04 /*                                   */
#define PDBOPTS_ESP_IPVSN	0x02 /*                     */
#define PDBOPTS_ESP_TUNNEL	0x01 /*                              */
#define PDBOPTS_ESP_IPV6	0x02 /*                         */
#define PDBOPTS_ESP_DIFFSERV	0x40 /*                              */
#define PDBOPTS_ESP_UPDATE_CSUM 0x80 /*                                 */
#define PDBOPTS_ESP_VERIFY_CSUM 0x20 /*                                   */

/*
                                            
 */
struct ipsec_encap_cbc {
	u32 iv[4];
};

struct ipsec_encap_ctr {
	u32 ctr_nonce;
	u32 ctr_initial;
	u32 iv[2];
};

struct ipsec_encap_ccm {
	u32 salt; /*               */
	u8 b0_flags;
	u8 ctr_flags;
	u16 ctr_initial;
	u32 iv[2];
};

struct ipsec_encap_gcm {
	u32 salt; /*               */
	u32 rsvd1;
	u32 iv[2];
};

struct ipsec_encap_pdb {
	u8 hmo_rsvd;
	u8 ip_nh;
	u8 ip_nh_offset;
	u8 options;
	u32 seq_num_ext_hi;
	u32 seq_num;
	union {
		struct ipsec_encap_cbc cbc;
		struct ipsec_encap_ctr ctr;
		struct ipsec_encap_ccm ccm;
		struct ipsec_encap_gcm gcm;
	};
	u32 spi;
	u16 rsvd1;
	u16 ip_hdr_len;
	u32 ip_hdr[0]; /*                            */
};

struct ipsec_decap_cbc {
	u32 rsvd[2];
};

struct ipsec_decap_ctr {
	u32 salt;
	u32 ctr_initial;
};

struct ipsec_decap_ccm {
	u32 salt;
	u8 iv_flags;
	u8 ctr_flags;
	u16 ctr_initial;
};

struct ipsec_decap_gcm {
	u32 salt;
	u32 resvd;
};

struct ipsec_decap_pdb {
	u16 hmo_ip_hdr_len;
	u8 ip_nh_offset;
	u8 options;
	union {
		struct ipsec_decap_cbc cbc;
		struct ipsec_decap_ctr ctr;
		struct ipsec_decap_ccm ccm;
		struct ipsec_decap_gcm gcm;
	};
	u32 seq_num_ext_hi;
	u32 seq_num;
	u32 anti_replay[2];
	u32 end_index[0];
};

/*
                                                         
 */
struct ipsec_deco_dpovrd {
#define IPSEC_ENCAP_DECO_DPOVRD_USE 0x80
	u8 ovrd_ecn;
	u8 ip_hdr_len;
	u8 nh_offset;
	u8 next_header; /*                   */
};

/*
                                        
 */
#define WIFI_PDBOPTS_FCS	0x01
#define WIFI_PDBOPTS_AR		0x40

struct wifi_encap_pdb {
	u16 mac_hdr_len;
	u8 rsvd;
	u8 options;
	u8 iv_flags;
	u8 pri;
	u16 pn1;
	u32 pn2;
	u16 frm_ctrl_mask;
	u16 seq_ctrl_mask;
	u8 rsvd1[2];
	u8 cnst;
	u8 key_id;
	u8 ctr_flags;
	u8 rsvd2;
	u16 ctr_init;
};

struct wifi_decap_pdb {
	u16 mac_hdr_len;
	u8 rsvd;
	u8 options;
	u8 iv_flags;
	u8 pri;
	u16 pn1;
	u32 pn2;
	u16 frm_ctrl_mask;
	u16 seq_ctrl_mask;
	u8 rsvd1[4];
	u8 ctr_flags;
	u8 rsvd2;
	u16 ctr_init;
};

/*
                                        
 */
#define WIMAX_PDBOPTS_FCS	0x01
#define WIMAX_PDBOPTS_AR	0x40 /*            */

struct wimax_encap_pdb {
	u8 rsvd[3];
	u8 options;
	u32 nonce;
	u8 b0_flags;
	u8 ctr_flags;
	u16 ctr_init;
	/*                             */
	u32 pn;
	/*                           */
};

struct wimax_decap_pdb {
	u8 rsvd[3];
	u8 options;
	u32 nonce;
	u8 iv_flags;
	u8 ctr_flags;
	u16 ctr_init;
	/*                             */
	u32 pn;
	u8 rsvd1[2];
	u16 antireplay_len;
	u64 antireplay_scorecard;
	/*                           */
};

/*
                                         
 */
#define MACSEC_PDBOPTS_FCS	0x01
#define MACSEC_PDBOPTS_AR	0x40 /*                    */

struct macsec_encap_pdb {
	u16 aad_len;
	u8 rsvd;
	u8 options;
	u64 sci;
	u16 ethertype;
	u8 tci_an;
	u8 rsvd1;
	/*                             */
	u32 pn;
	/*                           */
};

struct macsec_decap_pdb {
	u16 aad_len;
	u8 rsvd;
	u8 options;
	u64 sci;
	u8 rsvd1[3];
	/*                             */
	u8 antireplay_len;
	u32 pn;
	u64 antireplay_scorecard;
	/*                           */
};

/*
                                    
 */

#define TLS_PDBOPTS_ARS32	0x40
#define TLS_PDBOPTS_ARS64	0xc0
#define TLS_PDBOPTS_OUTFMT	0x08
#define TLS_PDBOPTS_IV_WRTBK	0x02 /*                   */
#define TLS_PDBOPTS_EXP_RND_IV	0x01 /*                   */

struct tls_block_encap_pdb {
	u8 type;
	u8 version[2];
	u8 options;
	u64 seq_num;
	u32 iv[4];
};

struct tls_stream_encap_pdb {
	u8 type;
	u8 version[2];
	u8 options;
	u64 seq_num;
	u8 i;
	u8 j;
	u8 rsvd1[2];
};

struct dtls_block_encap_pdb {
	u8 type;
	u8 version[2];
	u8 options;
	u16 epoch;
	u16 seq_num[3];
	u32 iv[4];
};

struct tls_block_decap_pdb {
	u8 rsvd[3];
	u8 options;
	u64 seq_num;
	u32 iv[4];
};

struct tls_stream_decap_pdb {
	u8 rsvd[3];
	u8 options;
	u64 seq_num;
	u8 i;
	u8 j;
	u8 rsvd1[2];
};

struct dtls_block_decap_pdb {
	u8 rsvd[3];
	u8 options;
	u16 epoch;
	u16 seq_num[3];
	u32 iv[4];
	u64 antireplay_scorecard;
};

/*
                            
 */
#define SRTP_PDBOPTS_MKI	0x08
#define SRTP_PDBOPTS_AR		0x40

struct srtp_encap_pdb {
	u8 x_len;
	u8 mki_len;
	u8 n_tag;
	u8 options;
	u32 cnst0;
	u8 rsvd[2];
	u16 cnst1;
	u16 salt[7];
	u16 cnst2;
	u32 rsvd1;
	u32 roc;
	u32 opt_mki;
};

struct srtp_decap_pdb {
	u8 x_len;
	u8 mki_len;
	u8 n_tag;
	u8 options;
	u32 cnst0;
	u8 rsvd[2];
	u16 cnst1;
	u16 salt[7];
	u16 cnst2;
	u16 rsvd1;
	u16 seq_num;
	u32 roc;
	u64 antireplay_scorecard;
};

/*
                                 
                                                                 
                                                            
                              
 */
#define DSA_PDB_SGF_SHIFT	24
#define DSA_PDB_SGF_MASK	(0xff << DSA_PDB_SGF_SHIFT)
#define DSA_PDB_SGF_Q		(0x80 << DSA_PDB_SGF_SHIFT)
#define DSA_PDB_SGF_R		(0x40 << DSA_PDB_SGF_SHIFT)
#define DSA_PDB_SGF_G		(0x20 << DSA_PDB_SGF_SHIFT)
#define DSA_PDB_SGF_W		(0x10 << DSA_PDB_SGF_SHIFT)
#define DSA_PDB_SGF_S		(0x10 << DSA_PDB_SGF_SHIFT)
#define DSA_PDB_SGF_F		(0x08 << DSA_PDB_SGF_SHIFT)
#define DSA_PDB_SGF_C		(0x04 << DSA_PDB_SGF_SHIFT)
#define DSA_PDB_SGF_D		(0x02 << DSA_PDB_SGF_SHIFT)
#define DSA_PDB_SGF_AB_SIGN	(0x02 << DSA_PDB_SGF_SHIFT)
#define DSA_PDB_SGF_AB_VERIFY	(0x01 << DSA_PDB_SGF_SHIFT)

#define DSA_PDB_L_SHIFT		7
#define DSA_PDB_L_MASK		(0x3ff << DSA_PDB_L_SHIFT)

#define DSA_PDB_N_MASK		0x7f

struct dsa_sign_pdb {
	u32 sgf_ln; /*                                   */
	u8 *q;
	u8 *r;
	u8 *g;	/*         */
	u8 *s;
	u8 *f;
	u8 *c;
	u8 *d;
	u8 *ab; /*          */
	u8 *u;
};

struct dsa_verify_pdb {
	u32 sgf_ln;
	u8 *q;
	u8 *r;
	u8 *g;	/*         */
	u8 *w; /*         */
	u8 *f;
	u8 *c;
	u8 *d;
	u8 *tmp; /*                      */
	u8 *ab; /*                             */
};

#endif
