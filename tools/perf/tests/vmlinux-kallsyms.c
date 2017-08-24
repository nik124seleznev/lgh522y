#include <linux/compiler.h>
#include <linux/rbtree.h>
#include <string.h>
#include "map.h"
#include "symbol.h"
#include "util.h"
#include "tests.h"
#include "debug.h"
#include "machine.h"

static int vmlinux_matches_kallsyms_filter(struct map *map __maybe_unused,
					   struct symbol *sym)
{
	bool *visited = symbol__priv(sym);
	*visited = true;
	return 0;
}

int test__vmlinux_matches_kallsyms(void)
{
	int err = -1;
	struct rb_node *nd;
	struct symbol *sym;
	struct map *kallsyms_map, *vmlinux_map;
	struct machine kallsyms, vmlinux;
	enum map_type type = MAP__FUNCTION;
	struct ref_reloc_sym ref_reloc_sym = { .name = "_stext", };

	/*
           
   
                                                                  
                                                                      
  */
	machine__init(&kallsyms, "", HOST_KERNEL_ID);
	machine__init(&vmlinux, "", HOST_KERNEL_ID);

	/*
           
   
                                                                      
                                                                        
                                                                       
  */
	if (machine__create_kernel_maps(&kallsyms) < 0) {
		pr_debug("machine__create_kernel_maps ");
		goto out;
	}

	/*
           
   
                                                                     
  */
	if (machine__load_kallsyms(&kallsyms, "/proc/kallsyms", type, NULL) <= 0) {
		pr_debug("dso__load_kallsyms ");
		goto out;
	}

	/*
           
   
                                                                       
                                                                       
                                                                        
                                           
  */
	kallsyms_map = machine__kernel_map(&kallsyms, type);

	sym = map__find_symbol_by_name(kallsyms_map, ref_reloc_sym.name, NULL);
	if (sym == NULL) {
		pr_debug("dso__find_symbol_by_name ");
		goto out;
	}

	ref_reloc_sym.addr = sym->start;

	/*
           
   
                                                                        
  */
	if (machine__create_kernel_maps(&vmlinux) < 0) {
		pr_debug("machine__create_kernel_maps ");
		goto out;
	}

	vmlinux_map = machine__kernel_map(&vmlinux, type);
	map__kmap(vmlinux_map)->ref_reloc_sym = &ref_reloc_sym;

	/*
           
   
                                                                     
                                          
   
                                                                        
                                                             
                                                                        
                         
  */
	if (machine__load_vmlinux_path(&vmlinux, type,
				       vmlinux_matches_kallsyms_filter) <= 0) {
		pr_debug("Couldn't find a vmlinux that matches the kernel running on this machine, skipping test\n");
		err = TEST_SKIP;
		goto out;
	}

	err = 0;
	/*
           
   
                                                                               
                                                                           
                      
  */
	for (nd = rb_first(&vmlinux_map->dso->symbols[type]); nd; nd = rb_next(nd)) {
		struct symbol *pair, *first_pair;
		bool backwards = true;

		sym  = rb_entry(nd, struct symbol, rb_node);

		if (sym->start == sym->end)
			continue;

		first_pair = machine__find_kernel_symbol(&kallsyms, type, sym->start, NULL, NULL);
		pair = first_pair;

		if (pair && pair->start == sym->start) {
next_pair:
			if (strcmp(sym->name, pair->name) == 0) {
				/*
                                                
                                                   
                                             
                                                 
                                                 
                                               
                               
     */
				s64 skew = sym->end - pair->end;
				if (llabs(skew) < page_size)
					continue;

				pr_debug("%#" PRIx64 ": diff end addr for %s v: %#" PRIx64 " k: %#" PRIx64 "\n",
					 sym->start, sym->name, sym->end, pair->end);
			} else {
				struct rb_node *nnd;
detour:
				nnd = backwards ? rb_prev(&pair->rb_node) :
						  rb_next(&pair->rb_node);
				if (nnd) {
					struct symbol *next = rb_entry(nnd, struct symbol, rb_node);

					if (next->start == sym->start) {
						pair = next;
						goto next_pair;
					}
				}

				if (backwards) {
					backwards = false;
					pair = first_pair;
					goto detour;
				}

				pr_debug("%#" PRIx64 ": diff name v: %s k: %s\n",
					 sym->start, sym->name, pair->name);
			}
		} else
			pr_debug("%#" PRIx64 ": %s not on kallsyms\n", sym->start, sym->name);

		err = -1;
	}

	if (!verbose)
		goto out;

	pr_info("Maps only in vmlinux:\n");

	for (nd = rb_first(&vmlinux.kmaps.maps[type]); nd; nd = rb_next(nd)) {
		struct map *pos = rb_entry(nd, struct map, rb_node), *pair;
		/*
                                                                       
                                                                   
                                                                        
                
   */
		pair = map_groups__find_by_name(&kallsyms.kmaps, type,
						(pos->dso->kernel ?
							pos->dso->short_name :
							pos->dso->name));
		if (pair)
			pair->priv = 1;
		else
			map__fprintf(pos, stderr);
	}

	pr_info("Maps in vmlinux with a different name in kallsyms:\n");

	for (nd = rb_first(&vmlinux.kmaps.maps[type]); nd; nd = rb_next(nd)) {
		struct map *pos = rb_entry(nd, struct map, rb_node), *pair;

		pair = map_groups__find(&kallsyms.kmaps, type, pos->start);
		if (pair == NULL || pair->priv)
			continue;

		if (pair->start == pos->start) {
			pair->priv = 1;
			pr_info(" %" PRIx64 "-%" PRIx64 " %" PRIx64 " %s in kallsyms as",
				pos->start, pos->end, pos->pgoff, pos->dso->name);
			if (pos->pgoff != pair->pgoff || pos->end != pair->end)
				pr_info(": \n*%" PRIx64 "-%" PRIx64 " %" PRIx64 "",
					pair->start, pair->end, pair->pgoff);
			pr_info(" %s\n", pair->dso->name);
			pair->priv = 1;
		}
	}

	pr_info("Maps only in kallsyms:\n");

	for (nd = rb_first(&kallsyms.kmaps.maps[type]);
	     nd; nd = rb_next(nd)) {
		struct map *pos = rb_entry(nd, struct map, rb_node);

		if (!pos->priv)
			map__fprintf(pos, stderr);
	}
out:
	machine__exit(&kallsyms);
	machine__exit(&vmlinux);
	return err;
}
