#ifndef _UAPI_PARISC_PDC_H
#define _UAPI_PARISC_PDC_H

/*
                        
                                                  
 */

#define PDC_WARN		  3	/*                               */
#define PDC_REQ_ERR_1		  2	/*              */
#define PDC_REQ_ERR_0		  1	/*                                       */
#define PDC_OK			  0	/*                             */
#define PDC_BAD_PROC		 -1	/*                              */
#define PDC_BAD_OPTION		 -2	/*                                 */
#define PDC_ERROR		 -3	/*                                          */
#define PDC_NE_MOD		 -5	/*                   */
#define PDC_NE_CELL_MOD		 -7	/*                       */
#define PDC_INVALID_ARG		-10	/*                                 */
#define PDC_BUS_POW_WARN	-12	/*                                                 */
#define PDC_NOT_NARROW		-17	/*                           */

/*
                      
 */

#define PDC_POW_FAIL	1		/*                       */
#define PDC_POW_FAIL_PREPARE	0	/*                       */

#define PDC_CHASSIS	2		/*                       */
#define PDC_CHASSIS_DISP	0	/*                        */
#define PDC_CHASSIS_WARN	1	/*                         */
#define PDC_CHASSIS_DISPWARN	2	/*                              */
#define PDC_RETURN_CHASSIS_INFO 128	/*                                                  */

#define PDC_PIM         3               /*                              */
#define PDC_PIM_HPMC            0       /*                              */
#define PDC_PIM_RETURN_SIZE     1       /*                              */
#define PDC_PIM_LPMC            2       /*                              */
#define PDC_PIM_SOFT_BOOT       3       /*                              */
#define PDC_PIM_TOC             4       /*                              */

#define PDC_MODEL	4		/*                            */
#define PDC_MODEL_INFO		0	/*                       */
#define PDC_MODEL_BOOTID	1	/*                  */
#define PDC_MODEL_VERSIONS	2	/*                              */
#define PDC_MODEL_SYSMODEL	3	/*                          */
#define PDC_MODEL_ENSPEC	4	/*                        */
#define PDC_MODEL_DISPEC	5	/*                         */
#define PDC_MODEL_CPU_ID	6	/*                                       */
#define PDC_MODEL_CAPABILITIES	7	/*                         */
/*                                                                           */
#define  PDC_MODEL_OS64			(1 << 0)
#define  PDC_MODEL_OS32			(1 << 1)
#define  PDC_MODEL_IOPDIR_FDC		(1 << 2)
#define  PDC_MODEL_NVA_MASK		(3 << 4)
#define  PDC_MODEL_NVA_SUPPORTED	(0 << 4)
#define  PDC_MODEL_NVA_SLOW		(1 << 4)
#define  PDC_MODEL_NVA_UNSUPPORTED	(3 << 4)
#define PDC_MODEL_GET_BOOT__OP	8	/*                           */
#define PDC_MODEL_SET_BOOT__OP	9	/*                       */

#define PA89_INSTRUCTION_SET	0x4	/*                       */
#define PA90_INSTRUCTION_SET	0x8

#define PDC_CACHE	5		/*                              */
#define PDC_CACHE_INFO		0	/*                       */
#define PDC_CACHE_SET_COH	1	/*                      */
#define PDC_CACHE_RET_SPID	2	/*                       */

#define PDC_HPA		6		/*                         */
#define PDC_HPA_PROCESSOR	0
#define PDC_HPA_MODULES		1

#define PDC_COPROC	7		/*                                   */
#define PDC_COPROC_CFG		0	/*                                        */

#define PDC_IODC	8		/*                */
#define PDC_IODC_READ		0	/*                       */
/*                                                         */
#define PDC_IODC_RI_DATA_BYTES	0	/*                  */
/*                                         */
#define PDC_IODC_RI_INIT	3	/*                    */
#define PDC_IODC_RI_IO		4	/*                      */
#define PDC_IODC_RI_SPA		5	/*                      */
#define PDC_IODC_RI_CONFIG	6	/*                      */
/*                                      */
#define PDC_IODC_RI_TEST	8	/*                      */
#define PDC_IODC_RI_TLB		9	/*                      */
#define PDC_IODC_NINIT		2	/*                       */
#define PDC_IODC_DINIT		3	/*                   */
#define PDC_IODC_MEMERR		4	/*                         */
#define PDC_IODC_INDEX_DATA	0	/*                                  */
#define PDC_IODC_BUS_ERROR	-4	/*                        */
#define PDC_IODC_INVALID_INDEX	-5	/*                            */
#define PDC_IODC_COUNT		-6	/*                     */

#define PDC_TOD		9		/*                         */
#define PDC_TOD_READ		0	/*            */
#define PDC_TOD_WRITE		1	/*             */


#define PDC_STABLE	10		/*                            */
#define PDC_STABLE_READ		0
#define PDC_STABLE_WRITE	1
#define PDC_STABLE_RETURN_SIZE	2
#define PDC_STABLE_VERIFY_CONTENTS 3
#define PDC_STABLE_INITIALIZE	4

#define PDC_NVOLATILE	11		/*                       */

#define PDC_ADD_VALID	12		/*                            */
#define PDC_ADD_VALID_VERIFY	0	/*                                  */

#define PDC_INSTR	15		/*                                  */

#define PDC_PROC	16		/*               */

#define PDC_CONFIG	16		/*               */
#define PDC_CONFIG_DECONFIG	0
#define PDC_CONFIG_DRECONFIG	1
#define PDC_CONFIG_DRETURN_CONFIG 2

#define PDC_BLOCK_TLB	18		/*                           */
#define PDC_BTLB_INFO		0	/*                     */
#define PDC_BTLB_INSERT		1	/*                    */
#define PDC_BTLB_PURGE		2	/*                      */
#define PDC_BTLB_PURGE_ALL	3	/*                         */

#define PDC_TLB		19		/*                                   */
#define PDC_TLB_INFO		0	/*                     */
#define PDC_TLB_SETUP		1	/*                       */

#define PDC_MEM		20		/*                */
#define PDC_MEM_MEMINFO		0
#define PDC_MEM_ADD_PAGE	1
#define PDC_MEM_CLEAR_PDT	2
#define PDC_MEM_READ_PDT	3
#define PDC_MEM_RESET_CLEAR	4
#define PDC_MEM_GOODMEM		5
#define PDC_MEM_TABLE		128	/*                                */
#define PDC_MEM_RETURN_ADDRESS_TABLE	PDC_MEM_TABLE
#define PDC_MEM_GET_MEMORY_SYSTEM_TABLES_SIZE	131
#define PDC_MEM_GET_MEMORY_SYSTEM_TABLES	132
#define PDC_MEM_GET_PHYSICAL_LOCATION_FROM_MEMORY_ADDRESS 133

#define PDC_MEM_RET_SBE_REPLACED	5	/*                       */
#define PDC_MEM_RET_DUPLICATE_ENTRY	4
#define PDC_MEM_RET_BUF_SIZE_SMALL	1
#define PDC_MEM_RET_PDT_FULL		-11
#define PDC_MEM_RET_INVALID_PHYSICAL_LOCATION ~0ULL

#define PDC_PSW		21		/*                              */
#define PDC_PSW_MASK		0	/*                              */
#define PDC_PSW_GET_DEFAULTS	1	/*                              */
#define PDC_PSW_SET_DEFAULTS	2	/*                              */
#define PDC_PSW_ENDIAN_BIT	1	/*                              */
#define PDC_PSW_WIDE_BIT	2	/*                              */ 

#define PDC_SYSTEM_MAP	22		/*                      */
#define PDC_FIND_MODULE 	0
#define PDC_FIND_ADDRESS	1
#define PDC_TRANSLATE_PATH	2

#define PDC_SOFT_POWER	23		/*                    */
#define PDC_SOFT_POWER_INFO	0	/*                                         */
#define PDC_SOFT_POWER_ENABLE	1	/*                                  */


/*                    */

/*                       */
#define PDC_MEM_MAP	128		/*                           */
#define PDC_MEM_MAP_HPA		0	/*                         */

#define PDC_EEPROM	129		/*                */
#define PDC_EEPROM_READ_WORD	0
#define PDC_EEPROM_WRITE_WORD	1
#define PDC_EEPROM_READ_BYTE	2
#define PDC_EEPROM_WRITE_BYTE	3
#define PDC_EEPROM_EEPROM_PASSWORD -1000

#define PDC_NVM		130		/*                                  */
#define PDC_NVM_READ_WORD	0
#define PDC_NVM_WRITE_WORD	1
#define PDC_NVM_READ_BYTE	2
#define PDC_NVM_WRITE_BYTE	3

#define PDC_SEED_ERROR	132		/*               */

#define PDC_IO		135		/*                                 */
#define PDC_IO_READ_AND_CLEAR_ERRORS	0
#define PDC_IO_RESET			1
#define PDC_IO_RESET_DEVICES		2
/*                                                         */
#define PDC_IO_USB_SUSPEND	0xC000000000000000
#define PDC_IO_EEPROM_IO_ERR_TABLE_FULL	-5	/*              */
#define PDC_IO_NO_SUSPEND		-6	/*              */

#define PDC_BROADCAST_RESET 136		/*                       */
#define PDC_DO_RESET		0	/*                                   */
#define PDC_DO_FIRM_TEST_RESET	1	/*                                */
#define PDC_BR_RECONFIGURATION	2	/*                         */
#define PDC_FIRM_TEST_MAGIC	0xab9ec36fUL    /*                      */

#define PDC_LAN_STATION_ID 138		/*                                  */
#define PDC_LAN_STATION_ID_READ	0	/*                                  */

#define	PDC_LAN_STATION_ID_SIZE	6

#define PDC_CHECK_RANGES 139		/*               */

#define PDC_NV_SECTIONS	141		/*               */

#define PDC_PERFORMANCE	142		/*                        */

#define PDC_SYSTEM_INFO	143		/*                     */
#define PDC_SYSINFO_RETURN_INFO_SIZE	0
#define PDC_SYSINFO_RRETURN_SYS_INFO	1
#define PDC_SYSINFO_RRETURN_ERRORS	2
#define PDC_SYSINFO_RRETURN_WARNINGS	3
#define PDC_SYSINFO_RETURN_REVISIONS	4
#define PDC_SYSINFO_RRETURN_DIAGNOSE	5
#define PDC_SYSINFO_RRETURN_HV_DIAGNOSE	1005

#define PDC_RDR		144		/*               */
#define PDC_RDR_READ_BUFFER	0
#define PDC_RDR_READ_SINGLE	1
#define PDC_RDR_WRITE_SINGLE	2

#define PDC_INTRIGUE	145 		/*               */
#define PDC_INTRIGUE_WRITE_BUFFER 	 0
#define PDC_INTRIGUE_GET_SCRATCH_BUFSIZE 1
#define PDC_INTRIGUE_START_CPU_COUNTERS	 2
#define PDC_INTRIGUE_STOP_CPU_COUNTERS	 3

#define PDC_STI		146 		/*              */
/*                                        */

/*                                       */
#define PDC_PCI_INDEX	147
#define PDC_PCI_INTERFACE_INFO		0
#define PDC_PCI_SLOT_INFO		1
#define PDC_PCI_INFLIGHT_BYTES		2
#define PDC_PCI_READ_CONFIG		3
#define PDC_PCI_WRITE_CONFIG		4
#define PDC_PCI_READ_PCI_IO		5
#define PDC_PCI_WRITE_PCI_IO		6
#define PDC_PCI_READ_CONFIG_DELAY	7
#define PDC_PCI_UPDATE_CONFIG_DELAY	8
#define PDC_PCI_PCI_PATH_TO_PCI_HPA	9
#define PDC_PCI_PCI_HPA_TO_PCI_PATH	10
#define PDC_PCI_PCI_PATH_TO_PCI_BUS	11
#define PDC_PCI_PCI_RESERVED		12
#define PDC_PCI_PCI_INT_ROUTE_SIZE	13
#define PDC_PCI_GET_INT_TBL_SIZE	PDC_PCI_PCI_INT_ROUTE_SIZE
#define PDC_PCI_PCI_INT_ROUTE		14
#define PDC_PCI_GET_INT_TBL		PDC_PCI_PCI_INT_ROUTE 
#define PDC_PCI_READ_MON_TYPE		15
#define PDC_PCI_WRITE_MON_TYPE		16


/*                                                                */
#define PDC_INITIATOR	163
#define PDC_GET_INITIATOR	0
#define PDC_SET_INITIATOR	1
#define PDC_DELETE_INITIATOR	2
#define PDC_RETURN_TABLE_SIZE	3
#define PDC_RETURN_TABLE	4

#define PDC_LINK	165 		/*               */
#define PDC_LINK_PCI_ENTRY_POINTS	0  /*                 */
#define PDC_LINK_USB_ENTRY_POINTS	1  /*                 */

/*         
                               
                                       
 */
#define	CL_NULL		0	/*         */
#define	CL_RANDOM	1	/*                         */
#define	CL_SEQU		2	/*                             */
#define	CL_DUPLEX	7	/*                                          */
#define	CL_KEYBD	8	/*                                    */
#define	CL_DISPL	9	/*                               */
#define	CL_FC		10	/*                           */

/*                   */
#define ENTRY_INIT_SRCH_FRST	2
#define ENTRY_INIT_SRCH_NEXT	3
#define ENTRY_INIT_MOD_DEV	4
#define ENTRY_INIT_DEV		5
#define ENTRY_INIT_MOD		6
#define ENTRY_INIT_MSG		9

/*                 */
#define ENTRY_IO_BOOTIN		0
#define ENTRY_IO_BOOTOUT	1
#define ENTRY_IO_CIN		2
#define ENTRY_IO_COUT		3
#define ENTRY_IO_CLOSE		4
#define ENTRY_IO_GETMSG		9
#define ENTRY_IO_BBLOCK_IN	16
#define ENTRY_IO_BBLOCK_OUT	17

/*                  */

/*                     */

/*                   */

/*                  */

/*                           */
#define OS_ID_NONE		0	/*                 */
#define OS_ID_HPUX		1	/*           */
#define OS_ID_MPEXL		2	/*            */
#define OS_ID_OSF		3	/*         */
#define OS_ID_HPRT		4	/*           */
#define OS_ID_NOVEL		5	/*            */
#define OS_ID_LINUX		6	/*        */


/*                           */
#define OSTAT_OFF		0
#define OSTAT_FLT		1 
#define OSTAT_TEST		2
#define OSTAT_INIT		3
#define OSTAT_SHUT		4
#define OSTAT_WARN		5
#define OSTAT_RUN		6
#define OSTAT_ON		7

/*                                                     */
#define BOOT_CONSOLE_HPA_OFFSET  0x3c0
#define BOOT_CONSOLE_SPA_OFFSET  0x3c4
#define BOOT_CONSOLE_PATH_OFFSET 0x3a8

/*                                              */
#define NUM_PDC_RESULT	32

#if !defined(__ASSEMBLY__)

#include <linux/types.h>


/*                          */
#define	PF_AUTOBOOT	0x80
#define	PF_AUTOSEARCH	0x40
#define	PF_TIMER	0x0F

struct device_path {		/*           */
	unsigned char flags;	/*                  */
	unsigned char bc[6];	/*                            */
	unsigned char mod;
	unsigned int  layers[6];/*                            */
} __attribute__((aligned(8))) ;

struct pz_device {
	struct	device_path dp;	/*           */
	/*                    */
	unsigned int hpa;	/*                  */
	/*            */
	unsigned int spa;	/*                  */
	/*                                     */
	unsigned int iodc_io;	/*                    */
	short	pad;		/*          */
	unsigned short cl_class;/*           */
} __attribute__((aligned(8))) ;

struct zeropage {
	/*                                  */
	unsigned int	vec_special;		/*              */
	/*                           */
	unsigned int	vec_pow_fail; /*                       */
	/*                       */
	unsigned int	vec_toc;
	unsigned int	vec_toclen;
	/*                         */
	unsigned int vec_rendz;
	int	vec_pow_fail_flen;
	int	vec_pad[10];		
	
	/*                                      */
	int	pad0[112];

	/*                  */
	int	pad1[84];

	/*                                   */
	int	memc_cont;		/*                             */
	int	memc_phsize;		/*                      */
	int	memc_adsize;		/*                                                     */
	unsigned int mem_pdc_hi;	/*                 */

	/*                                             */
	/*                               */
	unsigned int mem_booterr[8];	/*                    */
	unsigned int mem_free;		/*                                        */
	/*                        */
	unsigned int mem_hpa;		/*                     */
	/*                           */
	unsigned int mem_pdc;		/*                 */
	unsigned int mem_10msec;	/*                                 */

	/*                                     */
	/*                        */
	unsigned int imm_hpa;		/*                */
	int	imm_soft_boot;		/*                                      */
	unsigned int	imm_spa_size;		/*                              */
	unsigned int	imm_max_mem;		/*                     */

	/*                                                   */
	struct pz_device mem_cons;	/*                               */
	struct pz_device mem_boot;	/*                            */
	struct pz_device mem_kbd;	/*                                */

	/*                  */
	int	pad430[116];

	/*                             */
	__u32	pad600[1];
	__u32	proc_sti;		/*                    */
	__u32	pad608[126];
};

#endif /*                        */

#endif /*                    */
