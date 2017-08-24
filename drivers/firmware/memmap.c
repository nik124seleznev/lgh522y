/*
 * linux/drivers/firmware/memmap.c
 *  Copyright (C) 2008 SUSE LINUX Products GmbH
 *  by Bernhard Walle <bernhard.walle@gmx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2.0 as published by
 * the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/string.h>
#include <linux/firmware-map.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/bootmem.h>
#include <linux/slab.h>
#include <linux/mm.h>

/*
                                                                                
 */

/*
                                                                    
                                                                     
                                                     
 */
struct firmware_map_entry {
	/*
                                                                       
                                            
  */
	u64			start;	/*                           */
	u64			end;	/*                                 */
	const char		*type;	/*                          */
	struct list_head	list;	/*                           */
	struct kobject		kobj;   /*                        */
};

/*
                                                                                
 */
static ssize_t memmap_attr_show(struct kobject *kobj,
				struct attribute *attr, char *buf);
static ssize_t start_show(struct firmware_map_entry *entry, char *buf);
static ssize_t end_show(struct firmware_map_entry *entry, char *buf);
static ssize_t type_show(struct firmware_map_entry *entry, char *buf);

static struct firmware_map_entry * __meminit
firmware_map_find_entry(u64 start, u64 end, const char *type);

/*
                                                                                
 */

struct memmap_attribute {
	struct attribute attr;
	ssize_t (*show)(struct firmware_map_entry *entry, char *buf);
};

static struct memmap_attribute memmap_start_attr = __ATTR_RO(start);
static struct memmap_attribute memmap_end_attr   = __ATTR_RO(end);
static struct memmap_attribute memmap_type_attr  = __ATTR_RO(type);

/*
                                                                      
 */
static struct attribute *def_attrs[] = {
	&memmap_start_attr.attr,
	&memmap_end_attr.attr,
	&memmap_type_attr.attr,
	NULL
};

static const struct sysfs_ops memmap_attr_ops = {
	.show = memmap_attr_show,
};

/*                              */
static LIST_HEAD(map_entries);
static DEFINE_SPINLOCK(map_entries_lock);

/*
                                                                           
                                                                         
                                                                         
                                               
 */
static LIST_HEAD(map_entries_bootmem);
static DEFINE_SPINLOCK(map_entries_bootmem_lock);


static inline struct firmware_map_entry *
to_memmap_entry(struct kobject *kobj)
{
	return container_of(kobj, struct firmware_map_entry, kobj);
}

static void __meminit release_firmware_map_entry(struct kobject *kobj)
{
	struct firmware_map_entry *entry = to_memmap_entry(kobj);

	if (PageReserved(virt_to_page(entry))) {
		/*
                                                                 
                                                              
                                                               
                                 
   */
		spin_lock(&map_entries_bootmem_lock);
		list_add(&entry->list, &map_entries_bootmem);
		spin_unlock(&map_entries_bootmem_lock);

		return;
	}

	kfree(entry);
}

static struct kobj_type __refdata memmap_ktype = {
	.release	= release_firmware_map_entry,
	.sysfs_ops	= &memmap_attr_ops,
	.default_attrs	= def_attrs,
};

/*
                                                                                
 */

/* 
                                                                                
                                     
                                               
                                    
                                                                               
                 
  
                                                                           
                                                           
  */
static int firmware_map_add_entry(u64 start, u64 end,
				  const char *type,
				  struct firmware_map_entry *entry)
{
	BUG_ON(start > end);

	entry->start = start;
	entry->end = end - 1;
	entry->type = type;
	INIT_LIST_HEAD(&entry->list);
	kobject_init(&entry->kobj, &memmap_ktype);

	spin_lock(&map_entries_lock);
	list_add_tail(&entry->list, &map_entries);
	spin_unlock(&map_entries_lock);

	return 0;
}

/* 
                                                                        
                
                         
  
                                                                  
  */
static inline void firmware_map_remove_entry(struct firmware_map_entry *entry)
{
	list_del(&entry->list);
}

/*
                            
 */
static int add_sysfs_fw_map_entry(struct firmware_map_entry *entry)
{
	static int map_entries_nr;
	static struct kset *mmap_kset;

	if (!mmap_kset) {
		mmap_kset = kset_create_and_add("memmap", NULL, firmware_kobj);
		if (!mmap_kset)
			return -ENOMEM;
	}

	entry->kobj.kset = mmap_kset;
	if (kobject_add(&entry->kobj, NULL, "%d", map_entries_nr++))
		kobject_put(&entry->kobj);

	return 0;
}

/*
                               
 */
static inline void remove_sysfs_fw_map_entry(struct firmware_map_entry *entry)
{
	kobject_put(&entry->kobj);
}

/*
                                                                           
                                     
                                               
                                    
                                      
  
                                                                         
                                                                          
                                                                     
  
                                                                           
 */
static struct firmware_map_entry * __meminit
firmware_map_find_entry_in_list(u64 start, u64 end, const char *type,
				struct list_head *list)
{
	struct firmware_map_entry *entry;

	list_for_each_entry(entry, list, list)
		if ((entry->start == start) && (entry->end == end) &&
		    (!strcmp(entry->type, type))) {
			return entry;
		}

	return NULL;
}

/*
                                                                  
                                     
                                               
                                    
  
                                                                     
                                                                       
                                                            
  
                                                                           
 */
static struct firmware_map_entry * __meminit
firmware_map_find_entry(u64 start, u64 end, const char *type)
{
	return firmware_map_find_entry_in_list(start, end, type, &map_entries);
}

/*
                                                                                  
                                     
                                               
                                    
  
                                                                              
                                      
  
                                                                           
 */
static struct firmware_map_entry * __meminit
firmware_map_find_entry_bootmem(u64 start, u64 end, const char *type)
{
	return firmware_map_find_entry_in_list(start, end, type,
					       &map_entries_bootmem);
}

/* 
                                                                        
                  
                                     
                                              
                                    
  
                                                                            
                                                                            
                                             
  
                                                                    
  */
int __meminit firmware_map_add_hotplug(u64 start, u64 end, const char *type)
{
	struct firmware_map_entry *entry;

	entry = firmware_map_find_entry_bootmem(start, end, type);
	if (!entry) {
		entry = kzalloc(sizeof(struct firmware_map_entry), GFP_ATOMIC);
		if (!entry)
			return -ENOMEM;
	} else {
		/*                                     */
		spin_lock(&map_entries_bootmem_lock);
		list_del(&entry->list);
		spin_unlock(&map_entries_bootmem_lock);

		memset(entry, 0, sizeof(*entry));
	}

	firmware_map_add_entry(start, end, type, entry);
	/*                         */
	add_sysfs_fw_map_entry(entry);

	return 0;
}

/* 
                                                            
                                     
                                   
                                    
  
                                                                          
                         
  
                                                     
  
                                                                    
  */
int __init firmware_map_add_early(u64 start, u64 end, const char *type)
{
	struct firmware_map_entry *entry;

	entry = alloc_bootmem(sizeof(struct firmware_map_entry));
	if (WARN_ON(!entry))
		return -ENOMEM;

	return firmware_map_add_entry(start, end, type, entry);
}

/* 
                                                          
                                     
                                   
                                    
  
                                    
  
                                                
  */
int __meminit firmware_map_remove(u64 start, u64 end, const char *type)
{
	struct firmware_map_entry *entry;

	spin_lock(&map_entries_lock);
	entry = firmware_map_find_entry(start, end - 1, type);
	if (!entry) {
		spin_unlock(&map_entries_lock);
		return -EINVAL;
	}

	firmware_map_remove_entry(entry);
	spin_unlock(&map_entries_lock);

	/*                         */
	remove_sysfs_fw_map_entry(entry);

	return 0;
}

/*
                                                                                
 */

static ssize_t start_show(struct firmware_map_entry *entry, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "0x%llx\n",
		(unsigned long long)entry->start);
}

static ssize_t end_show(struct firmware_map_entry *entry, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "0x%llx\n",
		(unsigned long long)entry->end);
}

static ssize_t type_show(struct firmware_map_entry *entry, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", entry->type);
}

static inline struct memmap_attribute *to_memmap_attr(struct attribute *attr)
{
	return container_of(attr, struct memmap_attribute, attr);
}

static ssize_t memmap_attr_show(struct kobject *kobj,
				struct attribute *attr, char *buf)
{
	struct firmware_map_entry *entry = to_memmap_entry(kobj);
	struct memmap_attribute *memmap_attr = to_memmap_attr(attr);

	return memmap_attr->show(entry, buf);
}

/*
                                                                    
                                                                           
                                                                         
                                                                      
                                                                         
                          
 */
static int __init firmware_memmap_init(void)
{
	struct firmware_map_entry *entry;

	list_for_each_entry(entry, &map_entries, list)
		add_sysfs_fw_map_entry(entry);

	return 0;
}
late_initcall(firmware_memmap_init);

