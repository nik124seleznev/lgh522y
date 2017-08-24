/*
 * BPF Jit compiler for s390.
 *
 * Copyright IBM Corp. 2012
 *
 * Author(s): Martin Schwidefsky <schwidefsky@de.ibm.com>
 */
#include <linux/moduleloader.h>
#include <linux/netdevice.h>
#include <linux/if_vlan.h>
#include <linux/filter.h>
#include <asm/cacheflush.h>
#include <asm/processor.h>
#include <asm/facility.h>

/*
               
                      
                           
                                              
                            
                         
                                        
                     
                                              
                             
                                
                                                              
 */
int bpf_jit_enable __read_mostly;

/*
                                          
 */
extern u8 sk_load_word[], sk_load_half[], sk_load_byte[], sk_load_byte_msh[];
extern u8 sk_load_word_ind[], sk_load_half_ind[], sk_load_byte_ind[];

struct bpf_jit {
	unsigned int seen;
	u8 *start;
	u8 *prg;
	u8 *mid;
	u8 *lit;
	u8 *end;
	u8 *base_ip;
	u8 *ret0_ip;
	u8 *exit_ip;
	unsigned int off_load_word;
	unsigned int off_load_half;
	unsigned int off_load_byte;
	unsigned int off_load_bmsh;
	unsigned int off_load_iword;
	unsigned int off_load_ihalf;
	unsigned int off_load_ibyte;
};

#define BPF_SIZE_MAX	4096	/*                      */

#define SEEN_DATAREF	1	/*                             */
#define SEEN_XREG	2	/*             */
#define SEEN_MEM	4	/*                                 */
#define SEEN_RET0	8	/*                                    */
#define SEEN_LITERAL	16	/*                    */
#define SEEN_LOAD_WORD	32	/*                        */
#define SEEN_LOAD_HALF	64	/*                        */
#define SEEN_LOAD_BYTE	128	/*                        */
#define SEEN_LOAD_BMSH	256	/*                            */
#define SEEN_LOAD_IWORD	512	/*                            */
#define SEEN_LOAD_IHALF	1024	/*                            */
#define SEEN_LOAD_IBYTE	2048	/*                            */

#define EMIT2(op)					\
({							\
	if (jit->prg + 2 <= jit->mid)			\
		*(u16 *) jit->prg = op;			\
	jit->prg += 2;					\
})

#define EMIT4(op)					\
({							\
	if (jit->prg + 4 <= jit->mid)			\
		*(u32 *) jit->prg = op;			\
	jit->prg += 4;					\
})

#define EMIT4_DISP(op, disp)				\
({							\
	unsigned int __disp = (disp) & 0xfff;		\
	EMIT4(op | __disp);				\
})

#define EMIT4_IMM(op, imm)				\
({							\
	unsigned int __imm = (imm) & 0xffff;		\
	EMIT4(op | __imm);				\
})

#define EMIT4_PCREL(op, pcrel)				\
({							\
	long __pcrel = ((pcrel) >> 1) & 0xffff;		\
	EMIT4(op | __pcrel);				\
})

#define EMIT6(op1, op2)					\
({							\
	if (jit->prg + 6 <= jit->mid) {			\
		*(u32 *) jit->prg = op1;		\
		*(u16 *) (jit->prg + 4) = op2;		\
	}						\
	jit->prg += 6;					\
})

#define EMIT6_DISP(op1, op2, disp)			\
({							\
	unsigned int __disp = (disp) & 0xfff;		\
	EMIT6(op1 | __disp, op2);			\
})

#define EMIT6_IMM(op, imm)				\
({							\
	unsigned int __imm = (imm);			\
	EMIT6(op | (__imm >> 16), __imm & 0xffff);	\
})

#define EMIT_CONST(val)					\
({							\
	unsigned int ret;				\
	ret = (unsigned int) (jit->lit - jit->base_ip);	\
	jit->seen |= SEEN_LITERAL;			\
	if (jit->lit + 4 <= jit->end)			\
		*(u32 *) jit->lit = val;		\
	jit->lit += 4;					\
	ret;						\
})

#define EMIT_FN_CONST(bit, fn)				\
({							\
	unsigned int ret;				\
	ret = (unsigned int) (jit->lit - jit->base_ip);	\
	if (jit->seen & bit) {				\
		jit->seen |= SEEN_LITERAL;		\
		if (jit->lit + 8 <= jit->end)		\
			*(void **) jit->lit = fn;	\
		jit->lit += 8;				\
	}						\
	ret;						\
})

static void bpf_jit_prologue(struct bpf_jit *jit)
{
	/*                                                    */
	if (jit->seen & SEEN_DATAREF) {
		/*                        */
		EMIT6(0xeb8ff058, 0x0024);
		/*               */
		EMIT4(0xb90400ef);
		/*                   */
		EMIT4_IMM(0xa7fa0000, (jit->seen & SEEN_MEM) ? -112 : -80);
		/*                    */
		EMIT6(0xe3e0f098, 0x0024);
	} else if ((jit->seen & SEEN_XREG) && (jit->seen & SEEN_LITERAL))
		/*                          */
		EMIT6(0xebcdf078, 0x0024);
	else if (jit->seen & SEEN_XREG)
		/*                    */
		EMIT6(0xe3c0f078, 0x0024);
	else if (jit->seen & SEEN_LITERAL)
		/*                    */
		EMIT6(0xe3d0f080, 0x0024);

	/*                    */
	if (jit->seen & SEEN_LITERAL) {
		/*             */
		EMIT2(0x0dd0);
		jit->base_ip = jit->prg;
	}
	jit->off_load_word = EMIT_FN_CONST(SEEN_LOAD_WORD, sk_load_word);
	jit->off_load_half = EMIT_FN_CONST(SEEN_LOAD_HALF, sk_load_half);
	jit->off_load_byte = EMIT_FN_CONST(SEEN_LOAD_BYTE, sk_load_byte);
	jit->off_load_bmsh = EMIT_FN_CONST(SEEN_LOAD_BMSH, sk_load_byte_msh);
	jit->off_load_iword = EMIT_FN_CONST(SEEN_LOAD_IWORD, sk_load_word_ind);
	jit->off_load_ihalf = EMIT_FN_CONST(SEEN_LOAD_IHALF, sk_load_half_ind);
	jit->off_load_ibyte = EMIT_FN_CONST(SEEN_LOAD_IBYTE, sk_load_byte_ind);

	/*                                 */
	if (jit->seen & SEEN_DATAREF) {
		/*                   */
		EMIT4_DISP(0x58b02000, offsetof(struct sk_buff, len));
		/*                        */
		EMIT4_DISP(0x5bb02000, offsetof(struct sk_buff, data_len));
		/*                     */
		EMIT6_DISP(0xe3a02000, 0x0004,
			   offsetof(struct sk_buff, data));
	}
}

static void bpf_jit_epilogue(struct bpf_jit *jit)
{
	/*          */
	if (jit->seen & SEEN_RET0) {
		jit->ret0_ip = jit->prg;
		/*            */
		EMIT4(0xa7290000);
	}
	jit->exit_ip = jit->prg;
	/*                   */
	if (jit->seen & SEEN_DATAREF)
		/*                             */
		EMIT6_DISP(0xeb8ff000, 0x0004,
			   (jit->seen & SEEN_MEM) ? 200 : 168);
	else if ((jit->seen & SEEN_XREG) && (jit->seen & SEEN_LITERAL))
		/*                         */
		EMIT6(0xebcdf078, 0x0004);
	else if (jit->seen & SEEN_XREG)
		/*                   */
		EMIT6(0xe3c0f078, 0x0004);
	else if (jit->seen & SEEN_LITERAL)
		/*                   */
		EMIT6(0xe3d0f080, 0x0004);
	/*         */
	EMIT2(0x07fe);
}

/*
                                                    
 */
static void bpf_jit_noleaks(struct bpf_jit *jit, struct sock_filter *filter)
{
	/*                                             */
	if (jit->seen & SEEN_MEM)
		/*                       */
		EMIT6(0xd73ff000, 0xf000);
	/*                               */
	if (jit->seen & SEEN_XREG)
		/*            */
		EMIT4(0xa7c80000);
	/*                                                */
	switch (filter[0].code) {
	case BPF_S_LD_W_ABS:
	case BPF_S_LD_H_ABS:
	case BPF_S_LD_B_ABS:
	case BPF_S_LD_W_LEN:
	case BPF_S_LD_W_IND:
	case BPF_S_LD_H_IND:
	case BPF_S_LD_B_IND:
	case BPF_S_LD_IMM:
	case BPF_S_LD_MEM:
	case BPF_S_MISC_TXA:
	case BPF_S_ANC_PROTOCOL:
	case BPF_S_ANC_PKTTYPE:
	case BPF_S_ANC_IFINDEX:
	case BPF_S_ANC_MARK:
	case BPF_S_ANC_QUEUE:
	case BPF_S_ANC_HATYPE:
	case BPF_S_ANC_RXHASH:
	case BPF_S_ANC_CPU:
	case BPF_S_ANC_VLAN_TAG:
	case BPF_S_ANC_VLAN_TAG_PRESENT:
	case BPF_S_RET_K:
		/*                                   */
		break;
	default: /*       */
		/*           */
		EMIT4(0xa7580000);
	}
}

static int bpf_jit_insn(struct bpf_jit *jit, struct sock_filter *filter,
			unsigned int *addrs, int i, int last)
{
	unsigned int K;
	int offset;
	unsigned int mask;

	K = filter->k;
	switch (filter->code) {
	case BPF_S_ALU_ADD_X: /*        */
		jit->seen |= SEEN_XREG;
		/*             */
		EMIT2(0x1a5c);
		break;
	case BPF_S_ALU_ADD_K: /*        */
		if (!K)
			break;
		if (K <= 16383)
			/*             */
			EMIT4_IMM(0xa75a0000, K);
		else if (test_facility(21))
			/*              */
			EMIT6_IMM(0xc25b0000, K);
		else
			/*                    */
			EMIT4_DISP(0x5a50d000, EMIT_CONST(K));
		break;
	case BPF_S_ALU_SUB_X: /*        */
		jit->seen |= SEEN_XREG;
		/*             */
		EMIT2(0x1b5c);
		break;
	case BPF_S_ALU_SUB_K: /*        */
		if (!K)
			break;
		if (K <= 16384)
			/*            */
			EMIT4_IMM(0xa75a0000, -K);
		else if (test_facility(21))
			/*             */
			EMIT6_IMM(0xc25b0000, -K);
		else
			/*                    */
			EMIT4_DISP(0x5b50d000, EMIT_CONST(K));
		break;
	case BPF_S_ALU_MUL_X: /*        */
		jit->seen |= SEEN_XREG;
		/*              */
		EMIT4(0xb252005c);
		break;
	case BPF_S_ALU_MUL_K: /*        */
		if (K <= 16383)
			/*           */
			EMIT4_IMM(0xa75c0000, K);
		else if (test_facility(34))
			/*              */
			EMIT6_IMM(0xc2510000, K);
		else
			/*                     */
			EMIT4_DISP(0x7150d000, EMIT_CONST(K));
		break;
	case BPF_S_ALU_DIV_X: /*        */
		jit->seen |= SEEN_XREG | SEEN_RET0;
		/*               */
		EMIT2(0x12cc);
		/*           */
		EMIT4_PCREL(0xa7840000, (jit->ret0_ip - jit->prg));
		/*           */
		EMIT4(0xa7480000);
		/*              */
		EMIT4(0xb997004c);
		break;
	case BPF_S_ALU_DIV_K: /*        */
		if (K == 1)
			break;
		/*           */
		EMIT4(0xa7480000);
		/*                     */
		EMIT6_DISP(0xe340d000, 0x0097, EMIT_CONST(K));
		break;
	case BPF_S_ALU_MOD_X: /*        */
		jit->seen |= SEEN_XREG | SEEN_RET0;
		/*               */
		EMIT2(0x12cc);
		/*           */
		EMIT4_PCREL(0xa7840000, (jit->ret0_ip - jit->prg));
		/*           */
		EMIT4(0xa7480000);
		/*              */
		EMIT4(0xb997004c);
		/*            */
		EMIT2(0x1854);
		break;
	case BPF_S_ALU_MOD_K: /*        */
		if (K == 1) {
			/*           */
			EMIT4(0xa7580000);
			break;
		}
		/*           */
		EMIT4(0xa7480000);
		/*                     */
		EMIT6_DISP(0xe340d000, 0x0097, EMIT_CONST(K));
		/*            */
		EMIT2(0x1854);
		break;
	case BPF_S_ALU_AND_X: /*        */
		jit->seen |= SEEN_XREG;
		/*             */
		EMIT2(0x145c);
		break;
	case BPF_S_ALU_AND_K: /*        */
		if (test_facility(21))
			/*              */
			EMIT6_IMM(0xc05b0000, K);
		else
			/*                    */
			EMIT4_DISP(0x5450d000, EMIT_CONST(K));
		break;
	case BPF_S_ALU_OR_X: /*        */
		jit->seen |= SEEN_XREG;
		/*             */
		EMIT2(0x165c);
		break;
	case BPF_S_ALU_OR_K: /*        */
		if (test_facility(21))
			/*              */
			EMIT6_IMM(0xc05d0000, K);
		else
			/*                    */
			EMIT4_DISP(0x5650d000, EMIT_CONST(K));
		break;
	case BPF_S_ANC_ALU_XOR_X: /*         */
	case BPF_S_ALU_XOR_X:
		jit->seen |= SEEN_XREG;
		/*             */
		EMIT2(0x175c);
		break;
	case BPF_S_ALU_XOR_K: /*        */
		if (!K)
			break;
		/*                    */
		EMIT4_DISP(0x5750d000, EMIT_CONST(K));
		break;
	case BPF_S_ALU_LSH_X: /*          */
		jit->seen |= SEEN_XREG;
		/*                 */
		EMIT4(0x8950c000);
		break;
	case BPF_S_ALU_LSH_K: /*         */
		if (K == 0)
			break;
		/*           */
		EMIT4_DISP(0x89500000, K);
		break;
	case BPF_S_ALU_RSH_X: /*          */
		jit->seen |= SEEN_XREG;
		/*                 */
		EMIT4(0x8850c000);
		break;
	case BPF_S_ALU_RSH_K: /*          */
		if (K == 0)
			break;
		/*           */
		EMIT4_DISP(0x88500000, K);
		break;
	case BPF_S_ALU_NEG: /*        */
		/*             */
		EMIT2(0x1155);
		break;
	case BPF_S_JMP_JA: /*         */
		offset = addrs[i + K] + jit->start - jit->prg;
		EMIT4_PCREL(0xa7f40000, offset);
		break;
	case BPF_S_JMP_JGT_K: /*                         */
		mask = 0x200000; /*    */
		goto kbranch;
	case BPF_S_JMP_JGE_K: /*                          */
		mask = 0xa00000; /*     */
		goto kbranch;
	case BPF_S_JMP_JEQ_K: /*                          */
		mask = 0x800000; /*    */
kbranch:	/*                                                  */
		if (filter->jt != filter->jf) {
			if (K <= 16383)
				/*             */
				EMIT4_IMM(0xa75e0000, K);
			else if (test_facility(21))
				/*              */
				EMIT6_IMM(0xc25f0000, K);
			else
				/*                    */
				EMIT4_DISP(0x5950d000, EMIT_CONST(K));
		}
branch:		if (filter->jt == filter->jf) {
			if (filter->jt == 0)
				break;
			/*        */
			offset = addrs[i + filter->jt] + jit->start - jit->prg;
			EMIT4_PCREL(0xa7f40000, offset);
			break;
		}
		if (filter->jt != 0) {
			/*                 */
			offset = addrs[i + filter->jt] + jit->start - jit->prg;
			EMIT4_PCREL(0xa7040000 | mask, offset);
		}
		if (filter->jf != 0) {
			/*                    */
			offset = addrs[i + filter->jf] + jit->start - jit->prg;
			EMIT4_PCREL(0xa7040000 | (mask ^ 0xf00000), offset);
		}
		break;
	case BPF_S_JMP_JSET_K: /*                         */
		mask = 0x700000; /*     */
		/*                                               */
		if (filter->jt != filter->jf) {
			if (K > 65535) {
				/*            */
				EMIT2(0x1845);
				/*                    */
				EMIT4_DISP(0x5440d000, EMIT_CONST(K));
			} else
				/*            */
				EMIT4_IMM(0xa7510000, K);
		}
		goto branch;
	case BPF_S_JMP_JGT_X: /*                         */
		mask = 0x200000; /*    */
		goto xbranch;
	case BPF_S_JMP_JGE_X: /*                          */
		mask = 0xa00000; /*     */
		goto xbranch;
	case BPF_S_JMP_JEQ_X: /*                          */
		mask = 0x800000; /*    */
xbranch:	/*                                                  */
		if (filter->jt != filter->jf) {
			jit->seen |= SEEN_XREG;
			/*             */
			EMIT2(0x195c);
		}
		goto branch;
	case BPF_S_JMP_JSET_X: /*                         */
		mask = 0x700000; /*     */
		/*                                               */
		if (filter->jt != filter->jf) {
			jit->seen |= SEEN_XREG;
			/*            */
			EMIT2(0x1845);
			/*             */
			EMIT2(0x144c);
		}
		goto branch;
	case BPF_S_LD_W_ABS: /*                            */
		jit->seen |= SEEN_DATAREF | SEEN_RET0 | SEEN_LOAD_WORD;
		offset = jit->off_load_word;
		goto load_abs;
	case BPF_S_LD_H_ABS: /*                            */
		jit->seen |= SEEN_DATAREF | SEEN_RET0 | SEEN_LOAD_HALF;
		offset = jit->off_load_half;
		goto load_abs;
	case BPF_S_LD_B_ABS: /*                           */
		jit->seen |= SEEN_DATAREF | SEEN_RET0 | SEEN_LOAD_BYTE;
		offset = jit->off_load_byte;
load_abs:	if ((int) K < 0)
			goto out;
call_fn:	/*                            */
		EMIT6_DISP(0xe310d000, 0x0004, offset);
		/*                    */
		EMIT4_DISP(0x5830d000, EMIT_CONST(K));
		/*              */
		EMIT2(0x0d81);
		/*            */
		EMIT4_PCREL(0xa7740000, (jit->ret0_ip - jit->prg));
		break;
	case BPF_S_LD_W_IND: /*                              */
		jit->seen |= SEEN_DATAREF | SEEN_RET0 | SEEN_LOAD_IWORD;
		offset = jit->off_load_iword;
		goto call_fn;
	case BPF_S_LD_H_IND: /*                              */
		jit->seen |= SEEN_DATAREF | SEEN_RET0 | SEEN_LOAD_IHALF;
		offset = jit->off_load_ihalf;
		goto call_fn;
	case BPF_S_LD_B_IND: /*                             */
		jit->seen |= SEEN_DATAREF | SEEN_RET0 | SEEN_LOAD_IBYTE;
		offset = jit->off_load_ibyte;
		goto call_fn;
	case BPF_S_LDX_B_MSH:
		/*                                       */
		jit->seen |= SEEN_RET0;
		if ((int) K < 0) {
			/*          */
			EMIT4_PCREL(0xa7f40000, (jit->ret0_ip - jit->prg));
			break;
		}
		jit->seen |= SEEN_DATAREF | SEEN_LOAD_BMSH;
		offset = jit->off_load_bmsh;
		goto call_fn;
	case BPF_S_LD_W_LEN: /*               */
		BUILD_BUG_ON(FIELD_SIZEOF(struct sk_buff, len) != 4);
		/*                     */
		EMIT4_DISP(0x58502000, offsetof(struct sk_buff, len));
		break;
	case BPF_S_LDX_W_LEN: /*               */
		jit->seen |= SEEN_XREG;
		/*                      */
		EMIT4_DISP(0x58c02000, offsetof(struct sk_buff, len));
		break;
	case BPF_S_LD_IMM: /*       */
		if (K <= 16383)
			/*           */
			EMIT4_IMM(0xa7580000, K);
		else if (test_facility(21))
			/*               */
			EMIT6_IMM(0xc05f0000, K);
		else
			/*                    */
			EMIT4_DISP(0x5850d000, EMIT_CONST(K));
		break;
	case BPF_S_LDX_IMM: /*       */
		jit->seen |= SEEN_XREG;
		if (K <= 16383)
			/*              */
			EMIT4_IMM(0xa7c80000, K);
		else if (test_facility(21))
			/*                */
			EMIT6_IMM(0xc0cf0000, K);
		else
			/*                     */
			EMIT4_DISP(0x58c0d000, EMIT_CONST(K));
		break;
	case BPF_S_LD_MEM: /*            */
		jit->seen |= SEEN_MEM;
		/*                 */
		EMIT4_DISP(0x5850f000,
			   (jit->seen & SEEN_DATAREF) ? 160 + K*4 : K*4);
		break;
	case BPF_S_LDX_MEM: /*            */
		jit->seen |= SEEN_XREG | SEEN_MEM;
		/*                  */
		EMIT4_DISP(0x58c0f000,
			   (jit->seen & SEEN_DATAREF) ? 160 + K*4 : K*4);
		break;
	case BPF_S_MISC_TAX: /*       */
		jit->seen |= SEEN_XREG;
		/*             */
		EMIT2(0x18c5);
		break;
	case BPF_S_MISC_TXA: /*       */
		jit->seen |= SEEN_XREG;
		/*             */
		EMIT2(0x185c);
		break;
	case BPF_S_RET_K:
		if (K == 0) {
			jit->seen |= SEEN_RET0;
			if (last)
				break;
			/*          */
			EMIT4_PCREL(0xa7f40000, jit->ret0_ip - jit->prg);
		} else {
			if (K <= 16383)
				/*            */
				EMIT4_IMM(0xa7290000, K);
			else
				/*                    */
				EMIT6_DISP(0xe320d000, 0x0016, EMIT_CONST(K));
			/*          */
			if (last && !(jit->seen & SEEN_RET0))
				break;
			EMIT4_PCREL(0xa7f40000, jit->exit_ip - jit->prg);
		}
		break;
	case BPF_S_RET_A:
		/*               */
		EMIT4(0xb9160025);
		/*          */
		EMIT4_PCREL(0xa7f40000, jit->exit_ip - jit->prg);
		break;
	case BPF_S_ST: /*            */
		jit->seen |= SEEN_MEM;
		/*                  */
		EMIT4_DISP(0x5050f000,
			   (jit->seen & SEEN_DATAREF) ? 160 + K*4 : K*4);
		break;
	case BPF_S_STX: /*                                  */
		jit->seen |= SEEN_XREG | SEEN_MEM;
		/*                   */
		EMIT4_DISP(0x50c0f000,
			   (jit->seen & SEEN_DATAREF) ? 160 + K*4 : K*4);
		break;
	case BPF_S_ANC_PROTOCOL: /*                           */
		BUILD_BUG_ON(FIELD_SIZEOF(struct sk_buff, protocol) != 2);
		/*           */
		EMIT4(0xa7580000);
		/*                              */
		EMIT4_DISP(0xbf532000, offsetof(struct sk_buff, protocol));
		break;
	case BPF_S_ANC_IFINDEX:	/*                         
                             */
		BUILD_BUG_ON(FIELD_SIZEOF(struct net_device, ifindex) != 4);
		jit->seen |= SEEN_RET0;
		/*                      */
		EMIT6_DISP(0xe3102000, 0x0004, offsetof(struct sk_buff, dev));
		/*              */
		EMIT4(0xb9020011);
		/*           */
		EMIT4_PCREL(0xa7840000, jit->ret0_ip - jit->prg);
		/*                         */
		EMIT4_DISP(0x58501000, offsetof(struct net_device, ifindex));
		break;
	case BPF_S_ANC_MARK: /*               */
		BUILD_BUG_ON(FIELD_SIZEOF(struct sk_buff, mark) != 4);
		/*                      */
		EMIT4_DISP(0x58502000, offsetof(struct sk_buff, mark));
		break;
	case BPF_S_ANC_QUEUE: /*                        */
		BUILD_BUG_ON(FIELD_SIZEOF(struct sk_buff, queue_mapping) != 2);
		/*           */
		EMIT4(0xa7580000);
		/*                                   */
		EMIT4_DISP(0xbf532000, offsetof(struct sk_buff, queue_mapping));
		break;
	case BPF_S_ANC_HATYPE:	/*                         
                          */
		BUILD_BUG_ON(FIELD_SIZEOF(struct net_device, type) != 2);
		jit->seen |= SEEN_RET0;
		/*                      */
		EMIT6_DISP(0xe3102000, 0x0004, offsetof(struct sk_buff, dev));
		/*              */
		EMIT4(0xb9020011);
		/*           */
		EMIT4_PCREL(0xa7840000, jit->ret0_ip - jit->prg);
		/*           */
		EMIT4(0xa7580000);
		/*                          */
		EMIT4_DISP(0xbf531000, offsetof(struct net_device, type));
		break;
	case BPF_S_ANC_RXHASH: /*                 */
		BUILD_BUG_ON(FIELD_SIZEOF(struct sk_buff, rxhash) != 4);
		/*                        */
		EMIT4_DISP(0x58502000, offsetof(struct sk_buff, rxhash));
		break;
	case BPF_S_ANC_VLAN_TAG:
	case BPF_S_ANC_VLAN_TAG_PRESENT:
		BUILD_BUG_ON(FIELD_SIZEOF(struct sk_buff, vlan_tci) != 2);
		BUILD_BUG_ON(VLAN_TAG_PRESENT != 0x1000);
		/*           */
		EMIT4(0xa7580000);
		/*                              */
		EMIT4_DISP(0xbf532000, offsetof(struct sk_buff, vlan_tci));
		if (filter->code == BPF_S_ANC_VLAN_TAG) {
			/*                 */
			EMIT4_IMM(0xa5570000, ~VLAN_TAG_PRESENT);
		} else {
			/*                 */
			EMIT4_IMM(0xa5570000, VLAN_TAG_PRESENT);
			/*            */
			EMIT4_DISP(0x88500000, 12);
		}
		break;
	case BPF_S_ANC_CPU: /*                        */
#ifdef CONFIG_SMP
		/*                   */
		EMIT4_DISP(0x58500000, offsetof(struct _lowcore, cpu_nr));
#else
		/*           */
		EMIT4(0xa7580000);
#endif
		break;
	default: /*                      */
		goto out;
	}
	addrs[i] = jit->prg - jit->start;
	return 0;
out:
	return -1;
}

void bpf_jit_compile(struct sk_filter *fp)
{
	unsigned long size, prg_len, lit_len;
	struct bpf_jit jit, cjit;
	unsigned int *addrs;
	int pass, i;

	if (!bpf_jit_enable)
		return;
	addrs = kcalloc(fp->len, sizeof(*addrs), GFP_KERNEL);
	if (addrs == NULL)
		return;
	memset(&jit, 0, sizeof(cjit));
	memset(&cjit, 0, sizeof(cjit));

	for (pass = 0; pass < 10; pass++) {
		jit.prg = jit.start;
		jit.lit = jit.mid;

		bpf_jit_prologue(&jit);
		bpf_jit_noleaks(&jit, fp->insns);
		for (i = 0; i < fp->len; i++) {
			if (bpf_jit_insn(&jit, fp->insns + i, addrs, i,
					 i == fp->len - 1))
				goto out;
		}
		bpf_jit_epilogue(&jit);
		if (jit.start) {
			WARN_ON(jit.prg > cjit.prg || jit.lit > cjit.lit);
			if (memcmp(&jit, &cjit, sizeof(jit)) == 0)
				break;
		} else if (jit.prg == cjit.prg && jit.lit == cjit.lit) {
			prg_len = jit.prg - jit.start;
			lit_len = jit.lit - jit.mid;
			size = max_t(unsigned long, prg_len + lit_len,
				     sizeof(struct work_struct));
			if (size >= BPF_SIZE_MAX)
				goto out;
			jit.start = module_alloc(size);
			if (!jit.start)
				goto out;
			jit.prg = jit.mid = jit.start + prg_len;
			jit.lit = jit.end = jit.start + prg_len + lit_len;
			jit.base_ip += (unsigned long) jit.start;
			jit.exit_ip += (unsigned long) jit.start;
			jit.ret0_ip += (unsigned long) jit.start;
		}
		cjit = jit;
	}
	if (bpf_jit_enable > 1) {
		pr_err("flen=%d proglen=%lu pass=%d image=%p\n",
		       fp->len, jit.end - jit.start, pass, jit.start);
		if (jit.start) {
			printk(KERN_ERR "JIT code:\n");
			print_fn_code(jit.start, jit.mid - jit.start);
			print_hex_dump(KERN_ERR, "JIT literals:\n",
				       DUMP_PREFIX_ADDRESS, 16, 1,
				       jit.mid, jit.end - jit.mid, false);
		}
	}
	if (jit.start)
		fp->bpf_func = (void *) jit.start;
out:
	kfree(addrs);
}

static void jit_free_defer(struct work_struct *arg)
{
	module_free(NULL, arg);
}

/*                                                    
                                     
 */
void bpf_jit_free(struct sk_filter *fp)
{
	struct work_struct *work;

	if (fp->bpf_func == sk_run_filter)
		return;
	work = (struct work_struct *)fp->bpf_func;
	INIT_WORK(work, jit_free_defer);
	schedule_work(work);
}
