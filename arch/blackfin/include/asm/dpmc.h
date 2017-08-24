/*
 * Miscellaneous IOCTL commands for Dynamic Power Management Controller Driver
 *
 * Copyright (C) 2004-2009 Analog Device Inc.
 *
 * Licensed under the GPL-2
 */

#ifndef _BLACKFIN_DPMC_H_
#define _BLACKFIN_DPMC_H_

#ifdef __ASSEMBLY__
#define PM_REG0  R7
#define PM_REG1  R6
#define PM_REG2  R5
#define PM_REG3  R4
#define PM_REG4  R3
#define PM_REG5  R2
#define PM_REG6  R1
#define PM_REG7  R0
#define PM_REG8  P5
#define PM_REG9  P4
#define PM_REG10 P3
#define PM_REG11 P2
#define PM_REG12 P1
#define PM_REG13 P0

#define PM_REGSET0  R7:7
#define PM_REGSET1  R7:6
#define PM_REGSET2  R7:5
#define PM_REGSET3  R7:4
#define PM_REGSET4  R7:3
#define PM_REGSET5  R7:2
#define PM_REGSET6  R7:1
#define PM_REGSET7  R7:0
#define PM_REGSET8  R7:0, P5:5
#define PM_REGSET9  R7:0, P5:4
#define PM_REGSET10 R7:0, P5:3
#define PM_REGSET11 R7:0, P5:2
#define PM_REGSET12 R7:0, P5:1
#define PM_REGSET13 R7:0, P5:0

#define _PM_PUSH(n, x, w, base) PM_REG##n = w[FP + ((x) - (base))];
#define _PM_POP(n, x, w, base)  w[FP + ((x) - (base))] = PM_REG##n;
#define PM_PUSH_SYNC(n)         [--sp] = (PM_REGSET##n);
#define PM_POP_SYNC(n)          (PM_REGSET##n) = [sp++];
#define PM_PUSH(n, x)		PM_REG##n = [FP++];
#define PM_POP(n, x)            [FP--] = PM_REG##n;
#define PM_CORE_PUSH(n, x)      _PM_PUSH(n, x, , COREMMR_BASE)
#define PM_CORE_POP(n, x)       _PM_POP(n, x, , COREMMR_BASE)
#define PM_SYS_PUSH(n, x)       _PM_PUSH(n, x, , SYSMMR_BASE)
#define PM_SYS_POP(n, x)        _PM_POP(n, x, , SYSMMR_BASE)
#define PM_SYS_PUSH16(n, x)     _PM_PUSH(n, x, w, SYSMMR_BASE)
#define PM_SYS_POP16(n, x)      _PM_POP(n, x, w, SYSMMR_BASE)

	.macro bfin_init_pm_bench_cycles
#ifdef CONFIG_BFIN_PM_WAKEUP_TIME_BENCH
	R4 = 0;
	CYCLES = R4;
	CYCLES2 = R4;
	R4 = SYSCFG;
	BITSET(R4, 1);
	SYSCFG = R4;
#endif
	.endm

	.macro bfin_cpu_reg_save
	/*
                                                          
                               
  */
	[--sp] = (R7:0, P5:0);
	[--sp] = fp;
	[--sp] = usp;

	[--sp] = i0;
	[--sp] = i1;
	[--sp] = i2;
	[--sp] = i3;

	[--sp] = m0;
	[--sp] = m1;
	[--sp] = m2;
	[--sp] = m3;

	[--sp] = l0;
	[--sp] = l1;
	[--sp] = l2;
	[--sp] = l3;

	[--sp] = b0;
	[--sp] = b1;
	[--sp] = b2;
	[--sp] = b3;
	[--sp] = a0.x;
	[--sp] = a0.w;
	[--sp] = a1.x;
	[--sp] = a1.w;

	[--sp] = LC0;
	[--sp] = LC1;
	[--sp] = LT0;
	[--sp] = LT1;
	[--sp] = LB0;
	[--sp] = LB1;

	/*                                                        */
	r7 = RETI;
	[--sp] = RETS;
	[--sp] = ASTAT;
#ifndef CONFIG_BFIN_PM_WAKEUP_TIME_BENCH
	[--sp] = CYCLES;
	[--sp] = CYCLES2;
#endif
	[--sp] = SYSCFG;
	[--sp] = RETX;
	[--sp] = SEQSTAT;
	[--sp] = r7;

	/*                           */
	M3 = R0;
	.endm

	.macro bfin_cpu_reg_restore
	/*                        */
	RETI = [sp++];
	SEQSTAT = [sp++];
	RETX = [sp++];
	SYSCFG = [sp++];
#ifndef CONFIG_BFIN_PM_WAKEUP_TIME_BENCH
	CYCLES2 = [sp++];
	CYCLES = [sp++];
#endif
	ASTAT = [sp++];
	RETS = [sp++];

	LB1 = [sp++];
	LB0 = [sp++];
	LT1 = [sp++];
	LT0 = [sp++];
	LC1 = [sp++];
	LC0 = [sp++];

	a1.w = [sp++];
	a1.x = [sp++];
	a0.w = [sp++];
	a0.x = [sp++];
	b3 = [sp++];
	b2 = [sp++];
	b1 = [sp++];
	b0 = [sp++];

	l3 = [sp++];
	l2 = [sp++];
	l1 = [sp++];
	l0 = [sp++];

	m3 = [sp++];
	m2 = [sp++];
	m1 = [sp++];
	m0 = [sp++];

	i3 = [sp++];
	i2 = [sp++];
	i1 = [sp++];
	i0 = [sp++];

	usp = [sp++];
	fp = [sp++];
	(R7:0, P5:0) = [sp++];

	.endm

	.macro bfin_sys_mmr_save
	/*                  */
	FP.H = hi(SYSMMR_BASE);
	FP.L = lo(SYSMMR_BASE);
#ifdef SIC_IMASK0
	PM_SYS_PUSH(0, SIC_IMASK0)
	PM_SYS_PUSH(1, SIC_IMASK1)
# ifdef SIC_IMASK2
	PM_SYS_PUSH(2, SIC_IMASK2)
# endif
#else
# ifdef SIC_IMASK
	PM_SYS_PUSH(0, SIC_IMASK)
# endif
#endif

#ifdef SIC_IAR0
	PM_SYS_PUSH(3, SIC_IAR0)
	PM_SYS_PUSH(4, SIC_IAR1)
	PM_SYS_PUSH(5, SIC_IAR2)
#endif
#ifdef SIC_IAR3
	PM_SYS_PUSH(6, SIC_IAR3)
#endif
#ifdef SIC_IAR4
	PM_SYS_PUSH(7, SIC_IAR4)
	PM_SYS_PUSH(8, SIC_IAR5)
	PM_SYS_PUSH(9, SIC_IAR6)
#endif
#ifdef SIC_IAR7
	PM_SYS_PUSH(10, SIC_IAR7)
#endif
#ifdef SIC_IAR8
	PM_SYS_PUSH(11, SIC_IAR8)
	PM_SYS_PUSH(12, SIC_IAR9)
	PM_SYS_PUSH(13, SIC_IAR10)
#endif
	PM_PUSH_SYNC(13)
#ifdef SIC_IAR11
	PM_SYS_PUSH(0, SIC_IAR11)
#endif

#ifdef SIC_IWR
	PM_SYS_PUSH(1, SIC_IWR)
#endif
#ifdef SIC_IWR0
	PM_SYS_PUSH(1, SIC_IWR0)
#endif
#ifdef SIC_IWR1
	PM_SYS_PUSH(2, SIC_IWR1)
#endif
#ifdef SIC_IWR2
	PM_SYS_PUSH(3, SIC_IWR2)
#endif

#ifdef PINT0_ASSIGN
	PM_SYS_PUSH(4, PINT0_MASK_SET)
	PM_SYS_PUSH(5, PINT1_MASK_SET)
	PM_SYS_PUSH(6, PINT2_MASK_SET)
	PM_SYS_PUSH(7, PINT3_MASK_SET)
	PM_SYS_PUSH(8, PINT0_ASSIGN)
	PM_SYS_PUSH(9, PINT1_ASSIGN)
	PM_SYS_PUSH(10, PINT2_ASSIGN)
	PM_SYS_PUSH(11, PINT3_ASSIGN)
	PM_SYS_PUSH(12, PINT0_INVERT_SET)
	PM_SYS_PUSH(13, PINT1_INVERT_SET)
	PM_PUSH_SYNC(13)
	PM_SYS_PUSH(0, PINT2_INVERT_SET)
	PM_SYS_PUSH(1, PINT3_INVERT_SET)
	PM_SYS_PUSH(2, PINT0_EDGE_SET)
	PM_SYS_PUSH(3, PINT1_EDGE_SET)
	PM_SYS_PUSH(4, PINT2_EDGE_SET)
	PM_SYS_PUSH(5, PINT3_EDGE_SET)
#endif

#ifdef SYSCR
	PM_SYS_PUSH16(6, SYSCR)
#endif

#ifdef EBIU_AMGCTL
	PM_SYS_PUSH16(7, EBIU_AMGCTL)
	PM_SYS_PUSH(8, EBIU_AMBCTL0)
	PM_SYS_PUSH(9, EBIU_AMBCTL1)
#endif
#ifdef EBIU_FCTL
	PM_SYS_PUSH(10, EBIU_MBSCTL)
	PM_SYS_PUSH(11, EBIU_MODE)
	PM_SYS_PUSH(12, EBIU_FCTL)
	PM_PUSH_SYNC(12)
#else
	PM_PUSH_SYNC(9)
#endif
	.endm


	.macro bfin_sys_mmr_restore
/*                     */
	FP.H = hi(SYSMMR_BASE);
	FP.L = lo(SYSMMR_BASE);

#ifdef EBIU_FCTL
	PM_POP_SYNC(12)
	PM_SYS_POP(12, EBIU_FCTL)
	PM_SYS_POP(11, EBIU_MODE)
	PM_SYS_POP(10, EBIU_MBSCTL)
#else
	PM_POP_SYNC(9)
#endif

#ifdef EBIU_AMGCTL
	PM_SYS_POP(9, EBIU_AMBCTL1)
	PM_SYS_POP(8, EBIU_AMBCTL0)
	PM_SYS_POP16(7, EBIU_AMGCTL)
#endif

#ifdef SYSCR
	PM_SYS_POP16(6, SYSCR)
#endif

#ifdef PINT0_ASSIGN
	PM_SYS_POP(5, PINT3_EDGE_SET)
	PM_SYS_POP(4, PINT2_EDGE_SET)
	PM_SYS_POP(3, PINT1_EDGE_SET)
	PM_SYS_POP(2, PINT0_EDGE_SET)
	PM_SYS_POP(1, PINT3_INVERT_SET)
	PM_SYS_POP(0, PINT2_INVERT_SET)
	PM_POP_SYNC(13)
	PM_SYS_POP(13, PINT1_INVERT_SET)
	PM_SYS_POP(12, PINT0_INVERT_SET)
	PM_SYS_POP(11, PINT3_ASSIGN)
	PM_SYS_POP(10, PINT2_ASSIGN)
	PM_SYS_POP(9, PINT1_ASSIGN)
	PM_SYS_POP(8, PINT0_ASSIGN)
	PM_SYS_POP(7, PINT3_MASK_SET)
	PM_SYS_POP(6, PINT2_MASK_SET)
	PM_SYS_POP(5, PINT1_MASK_SET)
	PM_SYS_POP(4, PINT0_MASK_SET)
#endif

#ifdef SIC_IWR2
	PM_SYS_POP(3, SIC_IWR2)
#endif
#ifdef SIC_IWR1
	PM_SYS_POP(2, SIC_IWR1)
#endif
#ifdef SIC_IWR0
	PM_SYS_POP(1, SIC_IWR0)
#endif
#ifdef SIC_IWR
	PM_SYS_POP(1, SIC_IWR)
#endif

#ifdef SIC_IAR11
	PM_SYS_POP(0, SIC_IAR11)
#endif
	PM_POP_SYNC(13)
#ifdef SIC_IAR8
	PM_SYS_POP(13, SIC_IAR10)
	PM_SYS_POP(12, SIC_IAR9)
	PM_SYS_POP(11, SIC_IAR8)
#endif
#ifdef SIC_IAR7
	PM_SYS_POP(10, SIC_IAR7)
#endif
#ifdef SIC_IAR6
	PM_SYS_POP(9, SIC_IAR6)
	PM_SYS_POP(8, SIC_IAR5)
	PM_SYS_POP(7, SIC_IAR4)
#endif
#ifdef SIC_IAR3
	PM_SYS_POP(6, SIC_IAR3)
#endif
#ifdef SIC_IAR0
	PM_SYS_POP(5, SIC_IAR2)
	PM_SYS_POP(4, SIC_IAR1)
	PM_SYS_POP(3, SIC_IAR0)
#endif
#ifdef SIC_IMASK0
# ifdef SIC_IMASK2
	PM_SYS_POP(2, SIC_IMASK2)
# endif
	PM_SYS_POP(1, SIC_IMASK1)
	PM_SYS_POP(0, SIC_IMASK0)
#else
# ifdef SIC_IMASK
	PM_SYS_POP(0, SIC_IMASK)
# endif
#endif
	.endm

	.macro bfin_core_mmr_save
	/*                */
	I0.H = hi(COREMMR_BASE);
	I0.L = lo(COREMMR_BASE);
	I1 = I0;
	I2 = I0;
	I3 = I0;
	B0 = I0;
	B1 = I0;
	B2 = I0;
	B3 = I0;
	I1.L = lo(DCPLB_ADDR0);
	I2.L = lo(DCPLB_DATA0);
	I3.L = lo(ICPLB_ADDR0);
	B0.L = lo(ICPLB_DATA0);
	B1.L = lo(EVT2);
	B2.L = lo(IMASK);
	B3.L = lo(TCNTL);

	/*               */
	FP = B1;
	PM_PUSH(0, EVT2)
	PM_PUSH(1, EVT3)
	FP += 4;	/*      */
	PM_PUSH(2, EVT5)
	PM_PUSH(3, EVT6)
	PM_PUSH(4, EVT7)
	PM_PUSH(5, EVT8)
	PM_PUSH_SYNC(5)

	PM_PUSH(0, EVT9)
	PM_PUSH(1, EVT10)
	PM_PUSH(2, EVT11)
	PM_PUSH(3, EVT12)
	PM_PUSH(4, EVT13)
	PM_PUSH(5, EVT14)
	PM_PUSH(6, EVT15)

	/*     */
	FP = B2;
	PM_PUSH(7, IMASK)
	FP += 4;	/*       */
	PM_PUSH(8, ILAT)
	PM_PUSH(9, IPRIO)

	/*            */
	FP = B3;
	PM_PUSH(10, TCNTL)
	PM_PUSH(11, TPERIOD)
	PM_PUSH(12, TSCALE)
	PM_PUSH(13, TCOUNT)
	PM_PUSH_SYNC(13)

	/*                               */
	FP = I0;
	PM_CORE_PUSH(0, DMEM_CONTROL);
	PM_CORE_PUSH(1, IMEM_CONTROL);
	PM_CORE_PUSH(2, TBUFCTL);
	PM_PUSH_SYNC(2)

	/*            */
	FP = I1;
	PM_PUSH(0, DCPLB_ADDR0)
	PM_PUSH(1, DCPLB_ADDR1)
	PM_PUSH(2, DCPLB_ADDR2)
	PM_PUSH(3, DCPLB_ADDR3)
	PM_PUSH(4, DCPLB_ADDR4)
	PM_PUSH(5, DCPLB_ADDR5)
	PM_PUSH(6, DCPLB_ADDR6)
	PM_PUSH(7, DCPLB_ADDR7)
	PM_PUSH(8, DCPLB_ADDR8)
	PM_PUSH(9, DCPLB_ADDR9)
	PM_PUSH(10, DCPLB_ADDR10)
	PM_PUSH(11, DCPLB_ADDR11)
	PM_PUSH(12, DCPLB_ADDR12)
	PM_PUSH(13, DCPLB_ADDR13)
	PM_PUSH_SYNC(13)
	PM_PUSH(0, DCPLB_ADDR14)
	PM_PUSH(1, DCPLB_ADDR15)

	/*            */
	FP = I2;
	PM_PUSH(2, DCPLB_DATA0)
	PM_PUSH(3, DCPLB_DATA1)
	PM_PUSH(4, DCPLB_DATA2)
	PM_PUSH(5, DCPLB_DATA3)
	PM_PUSH(6, DCPLB_DATA4)
	PM_PUSH(7, DCPLB_DATA5)
	PM_PUSH(8, DCPLB_DATA6)
	PM_PUSH(9, DCPLB_DATA7)
	PM_PUSH(10, DCPLB_DATA8)
	PM_PUSH(11, DCPLB_DATA9)
	PM_PUSH(12, DCPLB_DATA10)
	PM_PUSH(13, DCPLB_DATA11)
	PM_PUSH_SYNC(13)
	PM_PUSH(0, DCPLB_DATA12)
	PM_PUSH(1, DCPLB_DATA13)
	PM_PUSH(2, DCPLB_DATA14)
	PM_PUSH(3, DCPLB_DATA15)

	/*            */
	FP = I3;
	PM_PUSH(4, ICPLB_ADDR0)
	PM_PUSH(5, ICPLB_ADDR1)
	PM_PUSH(6, ICPLB_ADDR2)
	PM_PUSH(7, ICPLB_ADDR3)
	PM_PUSH(8, ICPLB_ADDR4)
	PM_PUSH(9, ICPLB_ADDR5)
	PM_PUSH(10, ICPLB_ADDR6)
	PM_PUSH(11, ICPLB_ADDR7)
	PM_PUSH(12, ICPLB_ADDR8)
	PM_PUSH(13, ICPLB_ADDR9)
	PM_PUSH_SYNC(13)
	PM_PUSH(0, ICPLB_ADDR10)
	PM_PUSH(1, ICPLB_ADDR11)
	PM_PUSH(2, ICPLB_ADDR12)
	PM_PUSH(3, ICPLB_ADDR13)
	PM_PUSH(4, ICPLB_ADDR14)
	PM_PUSH(5, ICPLB_ADDR15)

	/*            */
	FP = B0;
	PM_PUSH(6, ICPLB_DATA0)
	PM_PUSH(7, ICPLB_DATA1)
	PM_PUSH(8, ICPLB_DATA2)
	PM_PUSH(9, ICPLB_DATA3)
	PM_PUSH(10, ICPLB_DATA4)
	PM_PUSH(11, ICPLB_DATA5)
	PM_PUSH(12, ICPLB_DATA6)
	PM_PUSH(13, ICPLB_DATA7)
	PM_PUSH_SYNC(13)
	PM_PUSH(0, ICPLB_DATA8)
	PM_PUSH(1, ICPLB_DATA9)
	PM_PUSH(2, ICPLB_DATA10)
	PM_PUSH(3, ICPLB_DATA11)
	PM_PUSH(4, ICPLB_DATA12)
	PM_PUSH(5, ICPLB_DATA13)
	PM_PUSH(6, ICPLB_DATA14)
	PM_PUSH(7, ICPLB_DATA15)
	PM_PUSH_SYNC(7)
	.endm

	.macro bfin_core_mmr_restore
	/*                   */
	I0.H = hi(COREMMR_BASE);
	I0.L = lo(COREMMR_BASE);
	I1 = I0;
	I2 = I0;
	I3 = I0;
	B0 = I0;
	B1 = I0;
	B2 = I0;
	B3 = I0;
	I1.L = lo(DCPLB_ADDR15);
	I2.L = lo(DCPLB_DATA15);
	I3.L = lo(ICPLB_ADDR15);
	B0.L = lo(ICPLB_DATA15);
	B1.L = lo(EVT15);
	B2.L = lo(IPRIO);
	B3.L = lo(TCOUNT);

	/*            */
	FP = B0;
	PM_POP_SYNC(7)
	PM_POP(7, ICPLB_DATA15)
	PM_POP(6, ICPLB_DATA14)
	PM_POP(5, ICPLB_DATA13)
	PM_POP(4, ICPLB_DATA12)
	PM_POP(3, ICPLB_DATA11)
	PM_POP(2, ICPLB_DATA10)
	PM_POP(1, ICPLB_DATA9)
	PM_POP(0, ICPLB_DATA8)
	PM_POP_SYNC(13)
	PM_POP(13, ICPLB_DATA7)
	PM_POP(12, ICPLB_DATA6)
	PM_POP(11, ICPLB_DATA5)
	PM_POP(10, ICPLB_DATA4)
	PM_POP(9, ICPLB_DATA3)
	PM_POP(8, ICPLB_DATA2)
	PM_POP(7, ICPLB_DATA1)
	PM_POP(6, ICPLB_DATA0)

	/*            */
	FP = I3;
	PM_POP(5, ICPLB_ADDR15)
	PM_POP(4, ICPLB_ADDR14)
	PM_POP(3, ICPLB_ADDR13)
	PM_POP(2, ICPLB_ADDR12)
	PM_POP(1, ICPLB_ADDR11)
	PM_POP(0, ICPLB_ADDR10)
	PM_POP_SYNC(13)
	PM_POP(13, ICPLB_ADDR9)
	PM_POP(12, ICPLB_ADDR8)
	PM_POP(11, ICPLB_ADDR7)
	PM_POP(10, ICPLB_ADDR6)
	PM_POP(9, ICPLB_ADDR5)
	PM_POP(8, ICPLB_ADDR4)
	PM_POP(7, ICPLB_ADDR3)
	PM_POP(6, ICPLB_ADDR2)
	PM_POP(5, ICPLB_ADDR1)
	PM_POP(4, ICPLB_ADDR0)

	/*            */
	FP = I2;
	PM_POP(3, DCPLB_DATA15)
	PM_POP(2, DCPLB_DATA14)
	PM_POP(1, DCPLB_DATA13)
	PM_POP(0, DCPLB_DATA12)
	PM_POP_SYNC(13)
	PM_POP(13, DCPLB_DATA11)
	PM_POP(12, DCPLB_DATA10)
	PM_POP(11, DCPLB_DATA9)
	PM_POP(10, DCPLB_DATA8)
	PM_POP(9, DCPLB_DATA7)
	PM_POP(8, DCPLB_DATA6)
	PM_POP(7, DCPLB_DATA5)
	PM_POP(6, DCPLB_DATA4)
	PM_POP(5, DCPLB_DATA3)
	PM_POP(4, DCPLB_DATA2)
	PM_POP(3, DCPLB_DATA1)
	PM_POP(2, DCPLB_DATA0)

	/*            */
	FP = I1;
	PM_POP(1, DCPLB_ADDR15)
	PM_POP(0, DCPLB_ADDR14)
	PM_POP_SYNC(13)
	PM_POP(13, DCPLB_ADDR13)
	PM_POP(12, DCPLB_ADDR12)
	PM_POP(11, DCPLB_ADDR11)
	PM_POP(10, DCPLB_ADDR10)
	PM_POP(9, DCPLB_ADDR9)
	PM_POP(8, DCPLB_ADDR8)
	PM_POP(7, DCPLB_ADDR7)
	PM_POP(6, DCPLB_ADDR6)
	PM_POP(5, DCPLB_ADDR5)
	PM_POP(4, DCPLB_ADDR4)
	PM_POP(3, DCPLB_ADDR3)
	PM_POP(2, DCPLB_ADDR2)
	PM_POP(1, DCPLB_ADDR1)
	PM_POP(0, DCPLB_ADDR0)


	/*                               */

	/*                                   
                                       
 */
	FP = I0;
	PM_POP_SYNC(2)
	PM_CORE_POP(2, TBUFCTL)
	PM_CORE_POP(1, IMEM_CONTROL)
	PM_CORE_POP(0, DMEM_CONTROL)

	/*            */
	FP = B3;
	R0 = 0x1;
	[FP - 0xC] = R0;

	PM_POP_SYNC(13)
	FP = B3;
	PM_POP(13, TCOUNT)
	PM_POP(12, TSCALE)
	PM_POP(11, TPERIOD)
	PM_POP(10, TCNTL)

	/*     */
	FP = B2;
	PM_POP(9, IPRIO)
	PM_POP(8, ILAT)
	FP += -4;	/*       */
	PM_POP(7, IMASK)

	/*               */
	FP = B1;
	PM_POP(6, EVT15)
	PM_POP(5, EVT14)
	PM_POP(4, EVT13)
	PM_POP(3, EVT12)
	PM_POP(2, EVT11)
	PM_POP(1, EVT10)
	PM_POP(0, EVT9)
	PM_POP_SYNC(5)
	PM_POP(5, EVT8)
	PM_POP(4, EVT7)
	PM_POP(3, EVT6)
	PM_POP(2, EVT5)
	FP += -4;	/*      */
	PM_POP(1, EVT3)
	PM_POP(0, EVT2)
	.endm
#endif

#include <mach/pll.h>

/*               */
#define DF			0x0001	/*                                  */
#define PLL_OFF			0x0002	/*                 */
#define STOPCK			0x0008	/*                */
#define PDWN			0x0020	/*                       */
#ifdef __ADSPBF539__
# define IN_DELAY		0x0014	/*                                       */
# define OUT_DELAY		0x00C0	/*                                        */
#else
# define IN_DELAY		0x0040	/*                                       */
# define OUT_DELAY		0x0080	/*                                        */
#endif
#define BYPASS			0x0100	/*                */
#define MSEL			0x7E00	/*                                        */
#define SPORT_HYST		0x8000	/*                                                  */
#define SET_MSEL(x)		(((x)&0x3F) << 0x9)	/*                                      */

/*               */
#define SSEL			0x000F	/*               */
#define CSEL			0x0030	/*             */
#define CSEL_DIV1		0x0000	/*                */
#define CSEL_DIV2		0x0010	/*                */
#define CSEL_DIV4		0x0020	/*                */
#define CSEL_DIV8		0x0030	/*                */

#define CCLK_DIV1 CSEL_DIV1
#define CCLK_DIV2 CSEL_DIV2
#define CCLK_DIV4 CSEL_DIV4
#define CCLK_DIV8 CSEL_DIV8

#define SET_SSEL(x)	((x) & 0xF)	/*                                     */
#define SCLK_DIV(x)	(x)		/*                */

/*                */
#define ACTIVE_PLLENABLED	0x0001	/*                                           */
#define FULL_ON			0x0002	/*                           */
#define ACTIVE_PLLDISABLED	0x0004	/*                                            */
#define PLL_LOCKED		0x0020	/*                              */

#define RTCWS			0x0400	/*                          */
#define CANWS			0x0800	/*                    */
#define USBWS			0x2000	/*                    */
#define KPADWS			0x4000	/*                       */
#define ROTWS			0x8000	/*                       */
#define GPWS			0x1000	/*                                */

/*              */
#if defined(__ADSPBF52x__) || defined(__ADSPBF51x__)
#define FREQ			0x3000	/*                                              */
#define FREQ_1000		0x3000	/*                              */
#else
#define FREQ			0x0003	/*                                              */
#define FREQ_333		0x0001	/*                                */
#define FREQ_667		0x0002	/*                                */
#define FREQ_1000		0x0003	/*                              */
#endif
#define HIBERNATE		0x0000	/*                                      */

#define GAIN			0x000C	/*                    */
#define GAIN_5			0x0000	/*          */
#define GAIN_10			0x0004	/*          */
#define GAIN_20			0x0008	/*          */
#define GAIN_50			0x000C	/*          */

#define VLEV			0x00F0	/*                        */
#ifdef __ADSPBF52x__
#define VLEV_085		0x0040	/*                                     */
#define VLEV_090		0x0050	/*                                     */
#define VLEV_095		0x0060	/*                                     */
#define VLEV_100		0x0070	/*                                     */
#define VLEV_105		0x0080	/*                                     */
#define VLEV_110		0x0090	/*                                     */
#define VLEV_115		0x00A0	/*                                     */
#define VLEV_120		0x00B0	/*                                     */
#else
#define VLEV_085		0x0060	/*                                     */
#define VLEV_090		0x0070	/*                                     */
#define VLEV_095		0x0080	/*                                     */
#define VLEV_100		0x0090	/*                                     */
#define VLEV_105		0x00A0	/*                                     */
#define VLEV_110		0x00B0	/*                                     */
#define VLEV_115		0x00C0	/*                                     */
#define VLEV_120		0x00D0	/*                                     */
#define VLEV_125		0x00E0	/*                                     */
#define VLEV_130		0x00F0	/*                                     */
#endif

#ifdef CONFIG_BF60x
#define PA15WE			0x00000001 /*                         */
#define PB15WE			0x00000002 /*                         */
#define PC15WE			0x00000004 /*                         */
#define PD06WE			0x00000008 /*                                      */
#define PE12WE			0x00000010 /*                                                   */
#define PG04WE			0x00000020 /*                                  */
#define PG13WE			0x00000040 /*                         */
#define USBWE			0x00000080 /*                          */
#else
#define WAKE			0x0100	/*                                        */
#define CANWE			0x0200	/*                                  */
#define PHYWE			0x0400	/*                                  */
#define GPWE			0x0400	/*                                */
#define MXVRWE			0x0400	/*                                   */
#define KPADWE			0x1000	/*                       */
#define ROTWE			0x2000	/*                       */
#define CLKBUFOE		0x4000	/*                            */
#define SCKELOW			0x8000	/*                                                     */

#if defined(__ADSPBF52x__) || defined(__ADSPBF51x__)
#define USBWE			0x0200	/*                                  */
#else
#define USBWE			0x0800	/*                                  */
#endif
#endif

#ifndef __ASSEMBLY__

void sleep_mode(u32 sic_iwr0, u32 sic_iwr1, u32 sic_iwr2);
void sleep_deeper(u32 sic_iwr0, u32 sic_iwr1, u32 sic_iwr2);
void do_hibernate(int wakeup);
void set_dram_srfs(void);
void unset_dram_srfs(void);

#define VRPAIR(vlev, freq) (((vlev) << 16) | ((freq) >> 16))

#ifdef CONFIG_CPU_FREQ
#define CPUFREQ_CPU 0
#endif
struct bfin_dpmc_platform_data {
	const unsigned int *tuple_tab;
	unsigned short tabsize;
	unsigned short vr_settling_time; /*       */
};

#endif

#endif	/*                 */
