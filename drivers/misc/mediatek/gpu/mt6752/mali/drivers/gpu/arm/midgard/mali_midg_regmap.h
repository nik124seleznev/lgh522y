/*
 *
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */





#ifndef _MIDGARD_REGMAP_H_
#define _MIDGARD_REGMAP_H_

/*
                         
 */

#define GPU_CONTROL_BASE        0x0000
#define GPU_CONTROL_REG(r)      (GPU_CONTROL_BASE + (r))
#define GPU_ID                  0x000	/*                                  */
#define L2_FEATURES             0x004	/*                             */
#define L3_FEATURES             0x008	/*                             */
#define TILER_FEATURES          0x00C	/*                     */
#define MEM_FEATURES            0x010	/*                             */
#define MMU_FEATURES            0x014	/*                   */
#define AS_PRESENT              0x018	/*                                  */
#define JS_PRESENT              0x01C	/*                        */
#define GPU_IRQ_RAWSTAT         0x020	/*      */
#define GPU_IRQ_CLEAR           0x024	/*      */
#define GPU_IRQ_MASK            0x028	/*      */
#define GPU_IRQ_STATUS          0x02C	/*      */

/*           */
#define GPU_FAULT               (1 << 0)	/*                          */
#define MULTIPLE_GPU_FAULTS     (1 << 7)	/*                                   */
#define RESET_COMPLETED         (1 << 8)	/*                                                                
                                       */
#define POWER_CHANGED_SINGLE    (1 << 9)	/*                                                          */
#define POWER_CHANGED_ALL       (1 << 10)	/*                                                     
                                        */

#define PRFCNT_SAMPLE_COMPLETED (1 << 16)	/*                                                    */
#define CLEAN_CACHES_COMPLETED  (1 << 17)	/*                                                 */

#define GPU_IRQ_REG_ALL (GPU_FAULT | MULTIPLE_GPU_FAULTS | RESET_COMPLETED \
			| POWER_CHANGED_ALL | PRFCNT_SAMPLE_COMPLETED)

#define GPU_COMMAND             0x030	/*      */
#define GPU_STATUS              0x034	/*      */

#define GROUPS_L2_COHERENT      (1 << 0)	/*                              */
#define GROUPS_L3_COHERENT      (1 << 1)	/*                              */

#define GPU_FAULTSTATUS         0x03C	/*                                          */
#define GPU_FAULTADDRESS_LO     0x040	/*                                            */
#define GPU_FAULTADDRESS_HI     0x044	/*                                             */

#define PWR_KEY                 0x050	/*                                 */
#define PWR_OVERRIDE0           0x054	/*                                      */
#define PWR_OVERRIDE1           0x058	/*                                      */

#define PRFCNT_BASE_LO          0x060	/*                                                               */
#define PRFCNT_BASE_HI          0x064	/*                                                                */
#define PRFCNT_CONFIG           0x068	/*                                        */
#define PRFCNT_JM_EN            0x06C	/*                                                       */
#define PRFCNT_SHADER_EN        0x070	/*                                                        */
#define PRFCNT_TILER_EN         0x074	/*                                                 */
#define PRFCNT_L3_CACHE_EN      0x078	/*                                                    */
#define PRFCNT_MMU_L2_EN        0x07C	/*                                                        */

#define CYCLE_COUNT_LO          0x090	/*                              */
#define CYCLE_COUNT_HI          0x094	/*                               */
#define TIMESTAMP_LO            0x098	/*                                          */
#define TIMESTAMP_HI            0x09C	/*                                           */

#define THREAD_MAX_THREADS		0x0A0	/*                                         */
#define THREAD_MAX_WORKGROUP_SIZE 0x0A4	/*                             */
#define THREAD_MAX_BARRIER_SIZE 0x0A8	/*                                           */
#define THREAD_FEATURES         0x0AC	/*                      */

#define TEXTURE_FEATURES_0      0x0B0	/*                                                      */
#define TEXTURE_FEATURES_1      0x0B4	/*                                                       */
#define TEXTURE_FEATURES_2      0x0B8	/*                                                       */

#define TEXTURE_FEATURES_REG(n) GPU_CONTROL_REG(TEXTURE_FEATURES_0 + ((n) << 2))

#define JS0_FEATURES            0x0C0	/*                             */
#define JS1_FEATURES            0x0C4	/*                             */
#define JS2_FEATURES            0x0C8	/*                             */
#define JS3_FEATURES            0x0CC	/*                             */
#define JS4_FEATURES            0x0D0	/*                             */
#define JS5_FEATURES            0x0D4	/*                             */
#define JS6_FEATURES            0x0D8	/*                             */
#define JS7_FEATURES            0x0DC	/*                             */
#define JS8_FEATURES            0x0E0	/*                             */
#define JS9_FEATURES            0x0E4	/*                             */
#define JS10_FEATURES           0x0E8	/*                              */
#define JS11_FEATURES           0x0EC	/*                              */
#define JS12_FEATURES           0x0F0	/*                              */
#define JS13_FEATURES           0x0F4	/*                              */
#define JS14_FEATURES           0x0F8	/*                              */
#define JS15_FEATURES           0x0FC	/*                              */

#define JS_FEATURES_REG(n)      GPU_CONTROL_REG(JS0_FEATURES + ((n) << 2))

#define SHADER_PRESENT_LO       0x100	/*                                           */
#define SHADER_PRESENT_HI       0x104	/*                                            */

#define TILER_PRESENT_LO        0x110	/*                                          */
#define TILER_PRESENT_HI        0x114	/*                                           */

#define L2_PRESENT_LO           0x120	/*                                             */
#define L2_PRESENT_HI           0x124	/*                                              */

#define L3_PRESENT_LO           0x130	/*                                             */
#define L3_PRESENT_HI           0x134	/*                                              */

#define SHADER_READY_LO         0x140	/*                                         */
#define SHADER_READY_HI         0x144	/*                                          */

#define TILER_READY_LO          0x150	/*                                        */
#define TILER_READY_HI          0x154	/*                                         */

#define L2_READY_LO             0x160	/*                                           */
#define L2_READY_HI             0x164	/*                                            */

#define L3_READY_LO             0x170	/*                                           */
#define L3_READY_HI             0x174	/*                                            */

#define SHADER_PWRON_LO         0x180	/*                                            */
#define SHADER_PWRON_HI         0x184	/*                                             */

#define TILER_PWRON_LO          0x190	/*                                           */
#define TILER_PWRON_HI          0x194	/*                                            */

#define L2_PWRON_LO             0x1A0	/*                                              */
#define L2_PWRON_HI             0x1A4	/*                                               */

#define L3_PWRON_LO             0x1B0	/*                                              */
#define L3_PWRON_HI             0x1B4	/*                                               */

#define SHADER_PWROFF_LO        0x1C0	/*                                             */
#define SHADER_PWROFF_HI        0x1C4	/*                                              */

#define TILER_PWROFF_LO         0x1D0	/*                                            */
#define TILER_PWROFF_HI         0x1D4	/*                                             */

#define L2_PWROFF_LO            0x1E0	/*                                               */
#define L2_PWROFF_HI            0x1E4	/*                                                */

#define L3_PWROFF_LO            0x1F0	/*                                               */
#define L3_PWROFF_HI            0x1F4	/*                                                */

#define SHADER_PWRTRANS_LO      0x200	/*                                                    */
#define SHADER_PWRTRANS_HI      0x204	/*                                                     */

#define TILER_PWRTRANS_LO       0x210	/*                                                   */
#define TILER_PWRTRANS_HI       0x214	/*                                                    */

#define L2_PWRTRANS_LO          0x220	/*                                                      */
#define L2_PWRTRANS_HI          0x224	/*                                                       */

#define L3_PWRTRANS_LO          0x230	/*                                                      */
#define L3_PWRTRANS_HI          0x234	/*                                                       */

#define SHADER_PWRACTIVE_LO     0x240	/*                                          */
#define SHADER_PWRACTIVE_HI     0x244	/*                                           */

#define TILER_PWRACTIVE_LO      0x250	/*                                         */
#define TILER_PWRACTIVE_HI      0x254	/*                                          */

#define L2_PWRACTIVE_LO         0x260	/*                                            */
#define L2_PWRACTIVE_HI         0x264	/*                                             */

#define L3_PWRACTIVE_LO         0x270	/*                                            */
#define L3_PWRACTIVE_HI         0x274	/*                                             */

#define SHADER_CONFIG           0xF04	/*                                                                            */
#define TILER_CONFIG            0xF08   /*                                                                           */
#define L2_MMU_CONFIG           0xF0C	/*                                                                               */

#define JOB_CONTROL_BASE        0x1000

#define JOB_CONTROL_REG(r)      (JOB_CONTROL_BASE + (r))

#define JOB_IRQ_RAWSTAT         0x000	/*                               */
#define JOB_IRQ_CLEAR           0x004	/*                          */
#define JOB_IRQ_MASK            0x008	/*                         */
#define JOB_IRQ_STATUS          0x00C	/*                           */
#define JOB_IRQ_JS_STATE        0x010	/*                                                                   */
#define JOB_IRQ_THROTTLE        0x014	/*                                                                                                                                      */

#define JOB_SLOT0               0x800	/*                                        */
#define JOB_SLOT1               0x880	/*                                        */
#define JOB_SLOT2               0x900	/*                                        */
#define JOB_SLOT3               0x980	/*                                        */
#define JOB_SLOT4               0xA00	/*                                        */
#define JOB_SLOT5               0xA80	/*                                        */
#define JOB_SLOT6               0xB00	/*                                        */
#define JOB_SLOT7               0xB80	/*                                        */
#define JOB_SLOT8               0xC00	/*                                        */
#define JOB_SLOT9               0xC80	/*                                        */
#define JOB_SLOT10              0xD00	/*                                         */
#define JOB_SLOT11              0xD80	/*                                         */
#define JOB_SLOT12              0xE00	/*                                         */
#define JOB_SLOT13              0xE80	/*                                         */
#define JOB_SLOT14              0xF00	/*                                         */
#define JOB_SLOT15              0xF80	/*                                         */

#define JOB_SLOT_REG(n, r)      (JOB_CONTROL_REG(JOB_SLOT0 + ((n) << 7)) + (r))

#define JS_HEAD_LO             0x00	/*                                                      */
#define JS_HEAD_HI             0x04	/*                                                       */
#define JS_TAIL_LO             0x08	/*                                                      */
#define JS_TAIL_HI             0x0C	/*                                                       */
#define JS_AFFINITY_LO         0x10	/*                                                  */
#define JS_AFFINITY_HI         0x14	/*                                                   */
#define JS_CONFIG              0x18	/*                                            */

#define JS_COMMAND             0x20	/*                                      */
#define JS_STATUS              0x24	/*                                     */

#define JS_HEAD_NEXT_LO        0x40	/*                                                           */
#define JS_HEAD_NEXT_HI        0x44	/*                                                            */

#define JS_AFFINITY_NEXT_LO    0x50	/*                                                       */
#define JS_AFFINITY_NEXT_HI    0x54	/*                                                        */
#define JS_CONFIG_NEXT         0x58	/*                                                 */

#define JS_COMMAND_NEXT        0x60	/*                                           */

#define MEMORY_MANAGEMENT_BASE  0x2000
#define MMU_REG(r)              (MEMORY_MANAGEMENT_BASE + (r))

#define MMU_IRQ_RAWSTAT         0x000	/*                                    */
#define MMU_IRQ_CLEAR           0x004	/*                               */
#define MMU_IRQ_MASK            0x008	/*                              */
#define MMU_IRQ_STATUS          0x00C	/*                                */

#define MMU_AS0                 0x400	/*                                             */
#define MMU_AS1                 0x440	/*                                             */
#define MMU_AS2                 0x480	/*                                             */
#define MMU_AS3                 0x4C0	/*                                             */
#define MMU_AS4                 0x500	/*                                             */
#define MMU_AS5                 0x540	/*                                             */
#define MMU_AS6                 0x580	/*                                             */
#define MMU_AS7                 0x5C0	/*                                             */
#define MMU_AS8                 0x600	/*                                             */
#define MMU_AS9                 0x640	/*                                             */
#define MMU_AS10                0x680	/*                                              */
#define MMU_AS11                0x6C0	/*                                              */
#define MMU_AS12                0x700	/*                                              */
#define MMU_AS13                0x740	/*                                              */
#define MMU_AS14                0x780	/*                                              */
#define MMU_AS15                0x7C0	/*                                              */

#define MMU_AS_REG(n, r)        (MMU_REG(MMU_AS0 + ((n) << 6)) + (r))

#define AS_TRANSTAB_LO         0x00	/*                                                                   */
#define AS_TRANSTAB_HI         0x04	/*                                                                    */
#define AS_MEMATTR_LO          0x08	/*                                                       */
#define AS_MEMATTR_HI          0x0C	/*                                                        */
#define AS_LOCKADDR_LO         0x10	/*                                                        */
#define AS_LOCKADDR_HI         0x14	/*                                                         */
#define AS_COMMAND             0x18	/*                                               */
#define AS_FAULTSTATUS         0x1C	/*                                                    */
#define AS_FAULTADDRESS_LO     0x20	/*                                                  */
#define AS_FAULTADDRESS_HI     0x24	/*                                                   */
#define AS_STATUS              0x28	/*                                       */

/*                      */

/*
                                                             
                                                         
 */

#define MMU_REGS_PAGE_FAULT_FLAGS   16

/*                                                                                    */
#define MMU_REGS_PAGE_FAULT_FLAG(n) (n)
#define MMU_REGS_BUS_ERROR_FLAG(n)  (n + MMU_REGS_PAGE_FAULT_FLAGS)

/*
                                     
 */
#define AS_TRANSTAB_ADDR_SPACE_MASK   0xfffff000
#define AS_TRANSTAB_ADRMODE_UNMAPPED  (0u << 0)
#define AS_TRANSTAB_ADRMODE_IDENTITY  (1u << 1)
#define AS_TRANSTAB_ADRMODE_TABLE     (3u << 0)
#define AS_TRANSTAB_READ_INNER        (1u << 2)
#define AS_TRANSTAB_SHARE_OUTER       (1u << 4)

#define MMU_TRANSTAB_ADRMODE_MASK      0x00000003

/*
                                   
 */
#define AS_STATUS_AS_ACTIVE 0x01

#define AS_FAULTSTATUS_ACCESS_TYPE_MASK    (0x3<<8)
#define AS_FAULTSTATUS_ACCESS_TYPE_EX      (0x1<<8)
#define AS_FAULTSTATUS_ACCESS_TYPE_READ    (0x2<<8)
#define AS_FAULTSTATUS_ACCESS_TYPE_WRITE   (0x3<<8)

/*
                       
 */

/*                              */
#define JS_COMMAND_NOP         0x00	/*                                              */
#define JS_COMMAND_START       0x01	/*                                                             */
#define JS_COMMAND_SOFT_STOP   0x02	/*                                    */
#define JS_COMMAND_HARD_STOP   0x03	/*                                    */
#define JS_COMMAND_SOFT_STOP_0 0x04	/*                                          */
#define JS_COMMAND_HARD_STOP_0 0x05	/*                                          */
#define JS_COMMAND_SOFT_STOP_1 0x06	/*                                          */
#define JS_COMMAND_HARD_STOP_1 0x07	/*                                          */

#define JS_COMMAND_MASK        0x07    /*                                         */

/*                              */
#define AS_COMMAND_NOP         0x00	/*               */
#define AS_COMMAND_UPDATE      0x01	/*                                                                  */
#define AS_COMMAND_LOCK        0x02	/*                                         */
#define AS_COMMAND_UNLOCK      0x03	/*                                          */
#define AS_COMMAND_FLUSH       0x04	/*                                                                  
                                              */
#define AS_COMMAND_FLUSH_PT    0x04	/*                                                                   */
#define AS_COMMAND_FLUSH_MEM   0x05	/*                                                                   
                                                                          */

/*                                                           */
#define JS_CONFIG_START_FLUSH_NO_ACTION        (0u << 0)
#define JS_CONFIG_START_FLUSH_CLEAN            (1u << 8)
#define JS_CONFIG_START_FLUSH_CLEAN_INVALIDATE (3u << 8)
#define JS_CONFIG_START_MMU                    (1u << 10)
#define JS_CONFIG_JOB_CHAIN_FLAG               (1u << 11)
#define JS_CONFIG_END_FLUSH_NO_ACTION          JS_CONFIG_START_FLUSH_NO_ACTION
#define JS_CONFIG_END_FLUSH_CLEAN              (1u << 12)
#define JS_CONFIG_END_FLUSH_CLEAN_INVALIDATE   (3u << 12)
#define JS_CONFIG_THREAD_PRI(n)                ((n) << 16)

/*                           */

/*                                                                                          
                                                                             
 */

/*                                                                       */
#define JS_STATUS_NO_EXCEPTION_BASE   0x00
#define JS_STATUS_INTERRUPTED         (JS_STATUS_NO_EXCEPTION_BASE + 0x02)	/*                        */
#define JS_STATUS_STOPPED             (JS_STATUS_NO_EXCEPTION_BASE + 0x03)	/*                    */
#define JS_STATUS_TERMINATED          (JS_STATUS_NO_EXCEPTION_BASE + 0x04)	/*                       */

/*                      */
#define JS_STATUS_FAULT_BASE          0x40
#define JS_STATUS_CONFIG_FAULT        (JS_STATUS_FAULT_BASE)	/*                         */
#define JS_STATUS_POWER_FAULT         (JS_STATUS_FAULT_BASE + 0x01)	/*                        */
#define JS_STATUS_READ_FAULT          (JS_STATUS_FAULT_BASE + 0x02)	/*                       */
#define JS_STATUS_WRITE_FAULT         (JS_STATUS_FAULT_BASE + 0x03)	/*                        */
#define JS_STATUS_AFFINITY_FAULT      (JS_STATUS_FAULT_BASE + 0x04)	/*                           */
#define JS_STATUS_BUS_FAULT           (JS_STATUS_FAULT_BASE + 0x08)	/*                      */

/*                            */
#define JS_STATUS_INSTRUCTION_FAULT_BASE  0x50
#define JS_STATUS_INSTR_INVALID_PC        (JS_STATUS_INSTRUCTION_FAULT_BASE)	/*                             */
#define JS_STATUS_INSTR_INVALID_ENC       (JS_STATUS_INSTRUCTION_FAULT_BASE + 0x01)	/*                              */
#define JS_STATUS_INSTR_TYPE_MISMATCH     (JS_STATUS_INSTRUCTION_FAULT_BASE + 0x02)	/*                                */
#define JS_STATUS_INSTR_OPERAND_FAULT     (JS_STATUS_INSTRUCTION_FAULT_BASE + 0x03)	/*                                */
#define JS_STATUS_INSTR_TLS_FAULT         (JS_STATUS_INSTRUCTION_FAULT_BASE + 0x04)	/*                            */
#define JS_STATUS_INSTR_BARRIER_FAULT     (JS_STATUS_INSTRUCTION_FAULT_BASE + 0x05)	/*                                */
#define JS_STATUS_INSTR_ALIGN_FAULT       (JS_STATUS_INSTRUCTION_FAULT_BASE + 0x06)	/*                              */
/*                                                */
#define JS_STATUS_DATA_INVALID_FAULT      (JS_STATUS_INSTRUCTION_FAULT_BASE + 0x08)	/*                               */
#define JS_STATUS_TILE_RANGE_FAULT        (JS_STATUS_INSTRUCTION_FAULT_BASE + 0x09)	/*                             */
#define JS_STATUS_ADDRESS_RANGE_FAULT     (JS_STATUS_INSTRUCTION_FAULT_BASE + 0x0A)	/*                                */

/*              */
#define JS_STATUS_MEMORY_FAULT_BASE   0x60
#define JS_STATUS_OUT_OF_MEMORY       (JS_STATUS_MEMORY_FAULT_BASE)	/*                          */
#define JS_STATUS_UNKNOWN             0x7F	/*                    */

/*                    */
#define GPU_COMMAND_NOP                0x00	/*                               */
#define GPU_COMMAND_SOFT_RESET         0x01	/*                                                                  */
#define GPU_COMMAND_HARD_RESET         0x02	/*                                   */
#define GPU_COMMAND_PRFCNT_CLEAR       0x03	/*                                                           */
#define GPU_COMMAND_PRFCNT_SAMPLE      0x04	/*                                                             */
#define GPU_COMMAND_CYCLE_COUNT_START  0x05	/*                                                            */
#define GPU_COMMAND_CYCLE_COUNT_STOP   0x06	/*                                                           */
#define GPU_COMMAND_CLEAN_CACHES       0x07	/*                  */
#define GPU_COMMAND_CLEAN_INV_CACHES   0x08	/*                                 */

/*                    */

/*                   */
#define GPU_STATUS_PRFCNT_ACTIVE           (1 << 2)	/*                                             */

/*                               */
#define PRFCNT_CONFIG_AS_SHIFT    4	/*                                                        */
#define PRFCNT_CONFIG_MODE_OFF    0	/*                                        */
#define PRFCNT_CONFIG_MODE_MANUAL 1	/*                                                                                                                                       */
#define PRFCNT_CONFIG_MODE_TILE   2	/*                                                                                                */

/*                       */
/*                                                 */
#define AS_MEMATTR_IMPL_DEF_CACHE_POLICY 0x48
/*                                                        */
#define AS_MEMATTR_FORCE_TO_CACHE_ALL    0x4F
/*                                                 */
#define AS_MEMATTR_WRITE_ALLOC           0x4D
/*                                   */
#define AS_MEMATTR_INDEX_DEFAULT               0
/*                                   */
#define AS_MEMATTR_INDEX_IMPL_DEF_CACHE_POLICY 0
/*                */
#define AS_MEMATTR_INDEX_FORCE_TO_CACHE_ALL    1
/*                   */
#define AS_MEMATTR_INDEX_WRITE_ALLOC           2

/*                 */
#define GPU_ID_VERSION_STATUS_SHIFT       0
#define GPU_ID_VERSION_MINOR_SHIFT        4
#define GPU_ID_VERSION_MAJOR_SHIFT        12
#define GPU_ID_VERSION_PRODUCT_ID_SHIFT   16
#define GPU_ID_VERSION_STATUS             (0xF  << GPU_ID_VERSION_STATUS_SHIFT)
#define GPU_ID_VERSION_MINOR              (0xFF << GPU_ID_VERSION_MINOR_SHIFT)
#define GPU_ID_VERSION_MAJOR              (0xF  << GPU_ID_VERSION_MAJOR_SHIFT)
#define GPU_ID_VERSION_PRODUCT_ID         (0xFFFF << GPU_ID_VERSION_PRODUCT_ID_SHIFT)

/*                                               */
#define GPU_ID_PI_T60X                    0x6956
#define GPU_ID_PI_T62X                    0x0620
#define GPU_ID_PI_T76X                    0x0750
#define GPU_ID_PI_T72X                    0x0720
#ifdef MALI_INCLUDE_TFRX
#define GPU_ID_PI_TFRX                    0x0880
#endif /*                   */
#ifdef MALI_INCLUDE_TF2X
#define GPU_ID_PI_TF2X                    0x0860
#endif /*                   */

/*                                                                      */
#define GPU_ID_S_15DEV0                   0x1
#define GPU_ID_S_EAC                      0x2

/*                                                                                    */
#define GPU_ID_MAKE(id, major, minor, status) \
		(((id) << GPU_ID_VERSION_PRODUCT_ID_SHIFT) | \
		((major) << GPU_ID_VERSION_MAJOR_SHIFT) |   \
		((minor) << GPU_ID_VERSION_MINOR_SHIFT) |   \
		((status) << GPU_ID_VERSION_STATUS_SHIFT))

/*                     */

/*                         */

#define JS_FEATURE_NULL_JOB              (1u << 1)
#define JS_FEATURE_SET_VALUE_JOB         (1u << 2)
#define JS_FEATURE_CACHE_FLUSH_JOB       (1u << 3)
#define JS_FEATURE_COMPUTE_JOB           (1u << 4)
#define JS_FEATURE_VERTEX_JOB            (1u << 5)
#define JS_FEATURE_GEOMETRY_JOB          (1u << 6)
#define JS_FEATURE_TILER_JOB             (1u << 7)
#define JS_FEATURE_FUSED_JOB             (1u << 8)
#define JS_FEATURE_FRAGMENT_JOB          (1u << 9)

/*                             */

/*                        */
#define L2_MMU_CONFIG_LIMIT_EXTERNAL_READS_SHIFT        (24)
#define L2_MMU_CONFIG_LIMIT_EXTERNAL_READS              (0x3 << L2_MMU_CONFIG_LIMIT_EXTERNAL_READS_SHIFT)
#define L2_MMU_CONFIG_LIMIT_EXTERNAL_READS_OCTANT       (0x1 << L2_MMU_CONFIG_LIMIT_EXTERNAL_READS_SHIFT)
#define L2_MMU_CONFIG_LIMIT_EXTERNAL_READS_QUARTER      (0x2 << L2_MMU_CONFIG_LIMIT_EXTERNAL_READS_SHIFT)
#define L2_MMU_CONFIG_LIMIT_EXTERNAL_READS_HALF         (0x3 << L2_MMU_CONFIG_LIMIT_EXTERNAL_READS_SHIFT)

#define L2_MMU_CONFIG_LIMIT_EXTERNAL_WRITES_SHIFT       (26)
#define L2_MMU_CONFIG_LIMIT_EXTERNAL_WRITES             (0x3 << L2_MMU_CONFIG_LIMIT_EXTERNAL_WRITES_SHIFT)
#define L2_MMU_CONFIG_LIMIT_EXTERNAL_WRITES_OCTANT      (0x1 << L2_MMU_CONFIG_LIMIT_EXTERNAL_WRITES_SHIFT)
#define L2_MMU_CONFIG_LIMIT_EXTERNAL_WRITES_QUARTER     (0x2 << L2_MMU_CONFIG_LIMIT_EXTERNAL_WRITES_SHIFT)
#define L2_MMU_CONFIG_LIMIT_EXTERNAL_WRITES_HALF        (0x3 << L2_MMU_CONFIG_LIMIT_EXTERNAL_WRITES_SHIFT)
/*                            */

/*                    */

/*                                                  */
#define IMPLEMENTATION_UNSPECIFIED  0
#define IMPLEMENTATION_SILICON      1
#define IMPLEMENTATION_FPGA         2
#define IMPLEMENTATION_MODEL        3

/*                                                                             */
#define THREAD_MT_DEFAULT     256
#define THREAD_MWS_DEFAULT    256
#define THREAD_MBS_DEFAULT    256
#define THREAD_MR_DEFAULT     1024
#define THREAD_MTQ_DEFAULT    4
#define THREAD_MTGS_DEFAULT   10

/*                        */

/*                        */

#define SC_ALT_COUNTERS             (1ul << 3)
#define SC_OVERRIDE_FWD_PIXEL_KILL  (1ul << 4)
#define SC_SDC_DISABLE_OQ_DISCARD   (1ul << 6)
#define SC_LS_PAUSEBUFFER_DISABLE   (1ul << 16)
#define SC_ENABLE_TEXGRD_FLAGS      (1ul << 25)
/*                            */

/*                       */

#define TC_CLOCK_GATE_OVERRIDE      (1ul << 0)

/*                           */


#endif				/*                    */
