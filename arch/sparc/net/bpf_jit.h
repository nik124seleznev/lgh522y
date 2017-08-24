#ifndef _BPF_JIT_H
#define _BPF_JIT_H

/*             
                   
                                                            
                                                          
                                                               
                           
                           
                                                                         
                                        
                                  
                   
 */

#ifndef __ASSEMBLER__
#define G0		0x00
#define G1		0x01
#define G3		0x03
#define G6		0x06
#define O0		0x08
#define O1		0x09
#define O2		0x0a
#define O3		0x0b
#define O4		0x0c
#define O5		0x0d
#define SP		0x0e
#define O7		0x0f
#define FP		0x1e

#define r_SKB		O0
#define r_A		O1
#define r_X		O2
#define r_saved_O7	O3
#define r_HEADLEN	O4
#define r_SKB_DATA	O5
#define r_TMP		G1
#define r_TMP2		G2
#define r_OFF		G3

/*                                               */
extern u32 bpf_jit_load_word[];
extern u32 bpf_jit_load_half[];
extern u32 bpf_jit_load_byte[];
extern u32 bpf_jit_load_byte_msh[];
extern u32 bpf_jit_load_word_positive_offset[];
extern u32 bpf_jit_load_half_positive_offset[];
extern u32 bpf_jit_load_byte_positive_offset[];
extern u32 bpf_jit_load_byte_msh_positive_offset[];
extern u32 bpf_jit_load_word_negative_offset[];
extern u32 bpf_jit_load_half_negative_offset[];
extern u32 bpf_jit_load_byte_negative_offset[];
extern u32 bpf_jit_load_byte_msh_negative_offset[];

#else
#define r_SKB		%o0
#define r_A		%o1
#define r_X		%o2
#define r_saved_O7	%o3
#define r_HEADLEN	%o4
#define r_SKB_DATA	%o5
#define r_TMP		%g1
#define r_TMP2		%g2
#define r_OFF		%g3
#endif

#endif /*            */
