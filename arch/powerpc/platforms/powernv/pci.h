#ifndef __POWERNV_PCI_H
#define __POWERNV_PCI_H

struct pci_dn;

enum pnv_phb_type {
	PNV_PHB_P5IOC2	= 0,
	PNV_PHB_IODA1	= 1,
	PNV_PHB_IODA2	= 2,
};

/*                                        */
enum pnv_phb_model {
	PNV_PHB_MODEL_UNKNOWN,
	PNV_PHB_MODEL_P5IOC2,
	PNV_PHB_MODEL_P7IOC,
	PNV_PHB_MODEL_PHB3,
};

#define PNV_PCI_DIAG_BUF_SIZE	4096
#define PNV_IODA_PE_DEV		(1 << 0)	/*                          */
#define PNV_IODA_PE_BUS		(1 << 1)	/*                        */
#define PNV_IODA_PE_BUS_ALL	(1 << 2)	/*                          */

/*                                                           */
struct pnv_phb;
struct pnv_ioda_pe {
	unsigned long		flags;
	struct pnv_phb		*phb;

	/*                                                  
                                                     
                                             
  */
	struct pci_dev		*pdev;
	struct pci_bus		*pbus;

	/*                                                       
                                  
  */
	unsigned int		rid;

	/*           */
	unsigned int		pe_number;

	/*                                                         
               
  */
	unsigned int		dma_weight;

	/*                                             */
	int			tce32_seg;
	int			tce32_segcount;
	struct iommu_table	tce32_table;

	/*                                                    */

	/*                                                       
                                                            
              
  */
	int			mve_number;

	/*                      */
	struct list_head	dma_link;
	struct list_head	list;
};

struct pnv_phb {
	struct pci_controller	*hose;
	enum pnv_phb_type	type;
	enum pnv_phb_model	model;
	u64			opal_id;
	void __iomem		*regs;
	int			initialized;
	spinlock_t		lock;

#ifdef CONFIG_PCI_MSI
	unsigned int		msi_base;
	unsigned int		msi32_support;
	struct msi_bitmap	msi_bmp;
#endif
	int (*msi_setup)(struct pnv_phb *phb, struct pci_dev *dev,
			 unsigned int hwirq, unsigned int virq,
			 unsigned int is_64, struct msi_msg *msg);
	void (*dma_dev_setup)(struct pnv_phb *phb, struct pci_dev *pdev);
	void (*fixup_phb)(struct pci_controller *hose);
	u32 (*bdfn_to_pe)(struct pnv_phb *phb, struct pci_bus *bus, u32 devfn);
	void (*shutdown)(struct pnv_phb *phb);

	union {
		struct {
			struct iommu_table iommu_table;
		} p5ioc2;

		struct {
			/*                    */
			unsigned int		total_pe;
			unsigned int		m32_size;
			unsigned int		m32_segsize;
			unsigned int		m32_pci_base;
			unsigned int		io_size;
			unsigned int		io_segsize;
			unsigned int		io_pci_base;

			/*                      */
			unsigned long		*pe_alloc;

			/*                       */
			unsigned int		*m32_segmap;
			unsigned int		*io_segmap;
			struct pnv_ioda_pe	*pe_array;

			/*          */
			int			irq_chip_init;
			struct irq_chip		irq_chip;

			/*                               
                                 
    */
			struct list_head	pe_list;

			/*                                           
                                                  
                        
    */
			unsigned char		pe_rmap[0x10000];

			/*                              */
			unsigned long		tce32_count;

			/*                                             
                
    */
			unsigned int		dma_weight;
			unsigned int		dma_pe_count;

			/*                                    
                                           
    */
			struct list_head	pe_dma_list;
		} ioda;
	};

	/*                      */
	union {
		unsigned char			blob[PNV_PCI_DIAG_BUF_SIZE];
		struct OpalIoP7IOCPhbErrorData	p7ioc;
	} diag;
};

extern struct pci_ops pnv_pci_ops;

extern void pnv_pci_setup_iommu_table(struct iommu_table *tbl,
				      void *tce_mem, u64 tce_size,
				      u64 dma_offset);
extern void pnv_pci_init_p5ioc2_hub(struct device_node *np);
extern void pnv_pci_init_ioda_hub(struct device_node *np);
extern void pnv_pci_init_ioda2_phb(struct device_node *np);
extern void pnv_pci_ioda_tce_invalidate(struct iommu_table *tbl,
					u64 *startp, u64 *endp);

#endif /*                 */
