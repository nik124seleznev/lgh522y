#ifndef __BUS_NUMA_H
#define __BUS_NUMA_H
/*
                                                                   
                                                        
 */
struct pci_root_res {
	struct list_head list;
	struct resource res;
};

struct pci_root_info {
	struct list_head list;
	char name[12];
	struct list_head resources;
	struct resource busn;
	int node;
	int link;
};

extern struct list_head pci_root_infos;
struct pci_root_info *alloc_pci_root_info(int bus_min, int bus_max,
						int node, int link);
extern void update_res(struct pci_root_info *info, resource_size_t start,
		      resource_size_t end, unsigned long flags, int merge);
#endif
