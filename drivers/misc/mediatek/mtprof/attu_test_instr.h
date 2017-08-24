
static void test_instr_NEON(int printlog)
{
	unsigned long copy_size;
	unsigned long flags;
	unsigned long i, j, avg, pld_dst;
	unsigned long temp;
	unsigned long result[10];
	copy_size = 256;
	/*                     */
	/*                      */
	/*                        */

	if (printlog == 1)
		pr_err("\n\n\r == Start test Pattern 1 ===\n\r");
	/*                                                     */
	i = 0;
	while (i < 256 + 16 + 4) {
		if (i < 256) {
			copy_size = 256 + i * 256;	/*                               */
		} else if (i < 256 + 16) {
			copy_size = 1024 * 64 + (i - 256) * 1024 * 64;	/*                           */
		} else if (i < 256 + 16 + 4) {
			copy_size = 1024 * 1024 + (i - 256 - 16) * 1024 * 1024;	/*                      */
		}
		i++;
		mdelay(5);
		preempt_disable();
		local_irq_save(flags);
		/*                                             */
		/*                                               */
		/*                                                */
		inner_dcache_flush_all();
		if (printlog == 1)
			pr_err(" %lu :", copy_size);
		avg = 0;
		/*        */
		for (j = 0; j < 8; j++) {
			mdelay(3);
			/*                      */
			inner_dcache_flush_all();
#if 1
			temp = 0;

			/*                    */
			asm volatile ("mov %0, #0\n" "MRC p15, 0, %0, c9, c12, 0\n" "BIC %0, %0, #1 << 0\n"	/*         */
				      "ORR %0, %0, #1 << 2\n"	/*                   */
				      "BIC %0, %0, #1 << 3\n"	/*                         */
				      "MCR p15, 0, %0, c9, c12, 0\n":"+r" (temp)
 :  : "cc");
			asm volatile ("MRC p15, 0, %0, c9, c12, 0\n" "ORR %0, %0, #1 << 0\n"	/*        */
				      "MCR p15, 0, %0, c9, c12, 0\n"
				      "MRC p15, 0, %0, c9, c12, 1\n"
				      "ORR %0, %0, #1 << 31\n"
				      "MCR p15, 0, %0, c9, c12, 1\n":"+r" (temp)
 :  : "cc");
#endif

			asm volatile ("push {r0,r1,r2,r9}\n"
				      "stmfd        sp!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
				      "mov r0, %0\n"
				      "mov r1, %1\n"
				      "mov r2, %2\n"
				      "mov r3, %3\n" "subs        r2, r2, #64\n" "1:\n"
				      /*                                          */
				      ".word 0xf421020d\n"
				      /*                                     */
				      ".word 0xf421420d\n"
				      /*                                     */
				      /*                      */
				      /*                               */
				      /*                      */
				      "subs        r2, r2, #64\n" ".word 0xf400022d\n"
				      /*                                           */
				      ".word 0xf400422d\n"
				      /*                                           */
				      "bge         1b\n"
				      "ldmfd       sp!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
				      "pop {r0,r1,r2, r9}\n"
				      /*            */
 :  : "r" (&buffer_dst), "r"(&buffer_src), "r"(copy_size),
				      "r"(pld_dst)
 : );

			/*                                */
#if 1
			/*                                          */
			asm volatile ("MRC p15, 0, %0, c9, c12, 0\n" "BIC %0, %0, #1 << 0\n"	/*         */
				      "MCR p15, 0, %0, c9, c12, 0\n"
				      "MRC p15, 0, %0, c9, c13, 0\n":"+r" (temp)
 :  : "cc");
			result[j] = temp;
#endif
		}		/*               */
		avg = 0;
		for (j = 0; j < 8; j++) {
			avg += result[j];
		}
		avg = avg >> 3;
		if (printlog == 1)
			pr_debug(" %lu ", avg);
		for (pld_dst = 0; pld_dst < 64 * 16; pld_dst += 64) {
			avg = 0;
			for (j = 0; j < 8; j++) {
				mdelay(3);
				if (flush_cache == 1)
					inner_dcache_flush_all();
#if 1
				temp = 0;

				/*                    */
				asm volatile ("mov %0, #0\n" "MRC p15, 0, %0, c9, c12, 0\n" "BIC %0, %0, #1 << 0\n"	/*         */
					      "ORR %0, %0, #1 << 2\n"	/*                   */
					      "BIC %0, %0, #1 << 3\n"	/*                         */
					      "MCR p15, 0, %0, c9, c12, 0\n":"+r" (temp)
 :  : "cc");
				asm volatile ("MRC p15, 0, %0, c9, c12, 0\n" "ORR %0, %0, #1 << 0\n"	/*        */
					      "MCR p15, 0, %0, c9, c12, 0\n"
					      "MRC p15, 0, %0, c9, c12, 1\n"
					      "ORR %0, %0, #1 << 31\n"
					      "MCR p15, 0, %0, c9, c12, 1\n":"+r" (temp)
 :  : "cc");
#endif

				asm volatile ("push {r0,r1,r2,r9}\n"
					      "stmfd        sp!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
					      "mov r0, %0\n"
					      "mov r1, %1\n"
					      "mov r2, %2\n"
					      "mov r3, %3\n" "subs        r2, r2, #64\n" "1:\n"
					      /*                                          */
					      ".word 0xf421020d\n"
					      /*                                     */
					      ".word 0xf421420d\n"
					      /*                                     */
					      /*                      */
					      /*                          */
					      "subs        r2, r2, #64\n" ".word 0xf400022d\n"
					      /*                                           */
					      ".word 0xf400422d\n"
					      /*                                           */
					      /*                             */
					      "bge         1b\n"
					      "ldmfd       sp!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
					      "pop {r0,r1,r2, r9}\n"
					      /*            */
 :  : "r" (&buffer_dst), "r"(&buffer_src), "r"(copy_size),
					      "r"(pld_dst) : );

				/*                                */
#if 1
				/*                                          */
				asm volatile ("MRC p15, 0, %0, c9, c12, 0\n" "BIC %0, %0, #1 << 0\n"	/*         */
					      "MCR p15, 0, %0, c9, c12, 0\n"
					      "MRC p15, 0, %0, c9, c13, 0\n":"+r" (temp)
 :  : "cc");
				/*                                                              */
				/*                       */
				/*                         */
				result[j] = temp;
				/*                                             */
#endif
			}	/*               */
			avg = 0;
			for (j = 0; j < 8; j++) {
				avg += result[j];
			}
			avg = avg >> 3;
			if (printlog == 1)
				pr_debug(" %d ", avg);
		}		/*               */
		local_irq_restore(flags);
		preempt_enable();
	}
	if (printlog == 1)
		pr_err("\n\r ====NEON instruction test done ==== flush_cache:%d\n", flush_cache);
}

static void test_instr_only_pld(int printlog)
{
	unsigned long copy_size;
	unsigned long flags;
	int i, j, avg, pld_dst;
	int k;
	int temp;
	int result[10];
	copy_size = 256;
	/*                     */
	/*                      */
	/*                        */
	if (printlog == 1)
		pr_err("\n\n\r == Start test Pattern PLD only ===\n\r");
	/*                                            */
	i = 0;
	while (i < 256 + 16 + 4) {
		if (i < 256) {
			copy_size = 256 + i * 256;	/*                               */
		} else if (i < 256 + 16) {
			copy_size = 1024 * 64 + (i - 256) * 1024 * 64;	/*                           */
		} else if (i < 256 + 16 + 4) {
			copy_size = 1024 * 1024 + (i - 256 - 16) * 1024 * 1024;	/*                      */
		}
		i++;
		mdelay(5);
		preempt_disable();
		local_irq_save(flags);
		/*                                            */
		/*                                             */
		/*                                               */
		/*                                                */
		/*                                */
		inner_dcache_flush_all();
		if (printlog == 1)
			pr_err(" %lu :", copy_size);
		avg = 0;
		for (j = 0; j < 8; j++) {
			mdelay(3);
			if (flush_cache == 1)
				inner_dcache_flush_all();
#if 1
			/*               */
#if 0
			asm volatile ("MRC p15, 0, %0, c1, c0, 1\n" "ORR %0, %0, #1 << 11\n"	/*         */
				      "ORR %0, %0, #1 << 12\n"	/*         */
				      "MCR p15, 0, %0, c1, c0, 1\n" "MRC p15, 0, %0, c1, c0, 1\n" "BIC %0, %0, #1 << 11\n"	/*        */
				      "BIC %0, %0, #1 << 12\n"	/*        */
				      "MCR p15, 0, %0, c1, c0, 1\n":"+r" (temp)
 :  : "cc");
#endif
			/*                    */
			asm volatile ("MRC p15, 0, %0, c9, c12, 0\n" "BIC %0, %0, #1 << 0\n"	/*         */
				      "ORR %0, %0, #1 << 2\n"	/*                   */
				      "BIC %0, %0, #1 << 3\n"	/*                         */
				      "MCR p15, 0, %0, c9, c12, 0\n":"+r" (temp)
 :  : "cc");
			asm volatile ("MRC p15, 0, %0, c9, c12, 0\n" "ORR %0, %0, #1 << 0\n"	/*        */
				      "MCR p15, 0, %0, c9, c12, 0\n"
				      "MRC p15, 0, %0, c9, c12, 1\n"
				      "ORR %0, %0, #1 << 31\n"
				      "MCR p15, 0, %0, c9, c12, 1\n":"+r" (temp)
 :  : "cc");
#endif
			asm volatile ("push {r0,r1,r2,r9}\n"
				      "mov r0, %0\n"
				      "mov r1, %1\n"
				      "mov r2, %2\n"
				      "mov r9, %3\n"
				      "stmfd        sp!, {r3, r4, r5, r6, r7, r8, r12, lr}\n" "1:\n"
				      /*                          */
				      /*                          */
				      /*                      */
				      "ldmia       r1!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
				      "subs        r2, r2, #32 \n"
				      "stmia       r0!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
				      "blt 2f\n"
				      "ldmia       r1!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
				      "subs        r2, r2, #32 \n"
				      "stmia       r0!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
				      "bge 1b\n"
				      "2:\n"
				      "ldmfd       sp!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
				      "pop {r0,r1,r2,r9}\n" :  : "r" (&buffer_dst), "r"(&buffer_src),
				      "r"(copy_size), "r"(pld_dst)
 : );


			/*                                */
#if 1
			/*                                          */
			asm volatile ("MRC p15, 0, %0, c9, c12, 0\n" "BIC %0, %0, #1 << 0\n"	/*         */
				      "MCR p15, 0, %0, c9, c12, 0\n"
				      "MRC p15, 0, %0, c9, c13, 0\n":"+r" (temp)
 :  : "cc");
#endif
			result[j] = temp;
			/*                         */
		}
		avg = 0;
		for (j = 0; j < 8; j++) {
			avg += result[j];
		}
		avg = avg >> 3;
		if (printlog == 1)
			pr_debug(" %d ", avg);
		for (pld_dst = 0; pld_dst < 64 * 16; pld_dst += 64) {
			for (j = 0; j < 8; j++) {
				mdelay(3);
				if (flush_cache == 1)
					inner_dcache_flush_all();
#if 1
#if 0
				/*               */
				asm volatile ("MRC p15, 0, %0, c1, c0, 1\n" "ORR %0, %0, #1 << 11\n"	/*         */
					      "ORR %0, %0, #1 << 12\n"	/*         */
					      "MCR p15, 0, %0, c1, c0, 1\n" "MRC p15, 0, %0, c1, c0, 1\n" "BIC %0, %0, #1 << 11\n"	/*        */
					      "BIC %0, %0, #1 << 12\n"	/*        */
					      "MCR p15, 0, %0, c1, c0, 1\n":"+r" (temp)
 :  : "cc");
#endif
				/*                    */
				asm volatile ("MRC p15, 0, %0, c9, c12, 0\n" "BIC %0, %0, #1 << 0\n"	/*         */
					      "ORR %0, %0, #1 << 2\n"	/*                   */
					      "BIC %0, %0, #1 << 3\n"	/*                         */
					      "MCR p15, 0, %0, c9, c12, 0\n":"+r" (temp)
 :  : "cc");
				asm volatile ("MRC p15, 0, %0, c9, c12, 0\n" "ORR %0, %0, #1 << 0\n"	/*        */
					      "MCR p15, 0, %0, c9, c12, 0\n"
					      "MRC p15, 0, %0, c9, c12, 1\n"
					      "ORR %0, %0, #1 << 31\n"
					      "MCR p15, 0, %0, c9, c12, 1\n":"+r" (temp)
 :  : "cc");
#endif
				asm volatile ("push {r0,r1,r2,r9}\n"
					      "mov r0, %0\n"
					      "mov r1, %1\n"
					      "mov r2, %2\n"
					      "mov r9, %3\n"
					      "stmfd        sp!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
					      "1:\n" "pld         [r1, r9]\n"
					      /*                          */
					      /*                      */
					      "ldmia       r1!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
					      "subs        r2, r2, #32 \n"
					      "stmia       r0!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
					      "blt 2f\n"
					      "ldmia       r1!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
					      "subs        r2, r2, #32 \n"
					      "stmia       r0!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
					      "bge 1b\n"
					      "2:\n"
					      "ldmfd       sp!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
					      "pop {r0,r1,r2,r9}\n" :  : "r" (&buffer_dst),
					      "r"(&buffer_src), "r"(copy_size), "r"(pld_dst)
 : );


				/*                                */
#if 1
				/*                                          */
				asm volatile ("MRC p15, 0, %0, c9, c12, 0\n" "BIC %0, %0, #1 << 0\n"	/*         */
					      "MCR p15, 0, %0, c9, c12, 0\n"
					      "MRC p15, 0, %0, c9, c13, 0\n":"+r" (temp)
 :  : "cc");
#endif
				result[j] = temp;
				/*                         */
			}
			avg = 0;
			for (j = 0; j < 8; j++) {
				avg += result[j];
			}
			avg = avg >> 3;
			if (printlog == 1)
				pr_debug(" %d ", avg);
		}
		local_irq_restore(flags);
		preempt_enable();
	}
	if (printlog == 1)
		pr_err("\n\r ==== test done only pld ==== flush_cache:%d\n", flush_cache);
}

static void test_instr_pld_pldw(int printlog)
{
	unsigned long copy_size;
	unsigned long flags;
	int i, j, avg, pld_dst;
	int k;
	int temp;
	int result[10];
	copy_size = 1024 * 64;
	if (printlog == 1)
		pr_err("\n\n\r == Start test pld+pldw ===\n\r");
	/*                                            */
	/*                                                */
	/*                                             */
	/*                                               */
	i = 0;
	while (i < 256 + 16 + 4) {
		if (i < 256) {
			copy_size = 256 + i * 256;	/*                               */
		} else if (i < 256 + 16) {
			copy_size = 1024 * 64 + (i - 256) * 1024 * 64;	/*                           */
		} else if (i < 256 + 16 + 4) {
			copy_size = 1024 * 1024 + (i - 256 - 16) * 1024 * 1024;	/*                      */
		}
		i++;
		mdelay(5);
		preempt_disable();
		local_irq_save(flags);
		/*                                */
		inner_dcache_flush_all();
		if (printlog == 1)
			pr_err(" %lu :", copy_size);
		avg = 0;
		for (j = 0; j < 8; j++) {
			mdelay(3);
			if (flush_cache == 1)
				inner_dcache_flush_all();
#if 1
			/*               */
#if 0
			asm volatile ("MRC p15, 0, %0, c1, c0, 1\n" "ORR %0, %0, #1 << 11\n"	/*         */
				      "ORR %0, %0, #1 << 12\n"	/*         */
				      "MCR p15, 0, %0, c1, c0, 1\n" "MRC p15, 0, %0, c1, c0, 1\n" "BIC %0, %0, #1 << 11\n"	/*        */
				      "BIC %0, %0, #1 << 12\n"	/*        */
				      "MCR p15, 0, %0, c1, c0, 1\n":"+r" (temp)
 :  : "cc");
#endif
			/*                    */
			asm volatile ("MRC p15, 0, %0, c9, c12, 0\n" "BIC %0, %0, #1 << 0\n"	/*         */
				      "ORR %0, %0, #1 << 2\n"	/*                   */
				      "BIC %0, %0, #1 << 3\n"	/*                         */
				      "MCR p15, 0, %0, c9, c12, 0\n":"+r" (temp)
 :  : "cc");
			asm volatile ("MRC p15, 0, %0, c9, c12, 0\n" "ORR %0, %0, #1 << 0\n"	/*        */
				      "MCR p15, 0, %0, c9, c12, 0\n"
				      "MRC p15, 0, %0, c9, c12, 1\n"
				      "ORR %0, %0, #1 << 31\n"
				      "MCR p15, 0, %0, c9, c12, 1\n":"+r" (temp)
 :  : "cc");
#endif
			asm volatile ("push {r0,r1,r2,r9}\n"
				      "mov r0, %0\n"
				      "mov r1, %1\n"
				      "mov r2, %2\n"
				      "mov r9, %3\n"
				      "stmfd        sp!, {r3, r4, r5, r6, r7, r8, r12, lr}\n" "1:\n"
				      /*                          */
				      /*                          */
				      /*                      */
				      "ldmia       r1!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
				      "subs        r2, r2, #32 \n"
				      "stmia       r0!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
				      "blt 2f\n"
				      "ldmia       r1!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
				      "subs        r2, r2, #32 \n"
				      "stmia       r0!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
				      "bge 1b\n"
				      "2:\n"
				      "ldmfd       sp!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
				      "pop {r0,r1,r2,r9}\n" :  : "r" (&buffer_dst), "r"(&buffer_src),
				      "r"(copy_size), "r"(pld_dst)
 : );


			/*                                */
#if 1
			/*                                          */
			asm volatile ("MRC p15, 0, %0, c9, c12, 0\n" "BIC %0, %0, #1 << 0\n"	/*         */
				      "MCR p15, 0, %0, c9, c12, 0\n"
				      "MRC p15, 0, %0, c9, c13, 0\n":"+r" (temp)
 :  : "cc");
#endif
			result[j] = temp;
			/*                         */
		}
		avg = 0;
		for (j = 0; j < 8; j++) {
			avg += result[j];
		}
		avg = avg >> 3;
		if (printlog == 1)
			pr_debug(" %d ", avg);
		for (pld_dst = 0; pld_dst < 64 * 16; pld_dst += 64) {
			/*                                                   */
			for (j = 0; j < 8; j++) {
				mdelay(3);
				if (flush_cache == 1)
					inner_dcache_flush_all();
#if 1
#if 0
				/*               */
				asm volatile ("MRC p15, 0, %0, c1, c0, 1\n" "ORR %0, %0, #1 << 11\n"	/*         */
					      "ORR %0, %0, #1 << 12\n"	/*         */
					      "MCR p15, 0, %0, c1, c0, 1\n" "MRC p15, 0, %0, c1, c0, 1\n" "BIC %0, %0, #1 << 11\n"	/*        */
					      "BIC %0, %0, #1 << 12\n"	/*        */
					      "MCR p15, 0, %0, c1, c0, 1\n":"+r" (temp)
 :  : "cc");
#endif
				/*                    */
				asm volatile ("MRC p15, 0, %0, c9, c12, 0\n" "BIC %0, %0, #1 << 0\n"	/*         */
					      "ORR %0, %0, #1 << 2\n"	/*                   */
					      "BIC %0, %0, #1 << 3\n"	/*                         */
					      "MCR p15, 0, %0, c9, c12, 0\n":"+r" (temp)
 :  : "cc");
				asm volatile ("MRC p15, 0, %0, c9, c12, 0\n" "ORR %0, %0, #1 << 0\n"	/*        */
					      "MCR p15, 0, %0, c9, c12, 0\n"
					      "MRC p15, 0, %0, c9, c12, 1\n"
					      "ORR %0, %0, #1 << 31\n"
					      "MCR p15, 0, %0, c9, c12, 1\n":"+r" (temp)
 :  : "cc");
#endif
				asm volatile ("push {r0,r1,r2,r9}\n"
					      "mov r0, %0\n"
					      "mov r1, %1\n"
					      "mov r2, %2\n"
					      "mov r9, %3\n"
					      "stmfd        sp!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
					      "1:\n" "pld         [r1, r9]\n"
					      /*                          */
					      ".word 0xf790f009\n"
					      "ldmia       r1!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
					      "subs        r2, r2, #32 \n"
					      "stmia       r0!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
					      "blt 2f\n"
					      "ldmia       r1!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
					      "subs        r2, r2, #32 \n"
					      "stmia       r0!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
					      "bge 1b\n" "2:\n"
					      "ldmfd       sp!, {r3, r4, r5, r6, r7, r8, r12, lr}\n"
					      "pop {r0,r1,r2,r9}\n" :  : "r" (&buffer_dst),
					      "r"(&buffer_src), "r"(copy_size), "r"(pld_dst)
 : );


				/*                                */
#if 1
				/*                                          */
				asm volatile ("MRC p15, 0, %0, c9, c12, 0\n" "BIC %0, %0, #1 << 0\n"	/*         */
					      "MCR p15, 0, %0, c9, c12, 0\n"
					      "MRC p15, 0, %0, c9, c13, 0\n":"+r" (temp)
 :  : "cc");
#endif
				result[j] = temp;
				/*                         */
			}
			avg = 0;
			for (j = 0; j < 8; j++) {
				avg += result[j];
			}
			avg = avg >> 3;
			if (printlog == 1)
				pr_debug(" %d ", avg);
		}
		local_irq_restore(flags);
		preempt_enable();
	}
	if (printlog == 1)
		pr_err("\n\r ==== test done pld+pldw==== flush_cache:%d\n", flush_cache);
}
