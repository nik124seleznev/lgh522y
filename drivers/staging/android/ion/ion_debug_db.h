#ifndef ION_DEBUG_DB_H
#define ION_DEBUG_DB_H

/*                                      */

/*
                            
 */

/*                           */
enum dbcl_types
{
	DBCL_CLIENT,
	DBCL_BUFFER,
	DBCL_MMAP,
	DBCL_FD,
	_TOTAL_DBCL,
};

const char *dbcl_child_name[_TOTAL_DBCL]
	= { "client",
	    "buffer",
	    "mmap",
	    "fd",
	};

/*                               */
struct dbcl_child {
	void *raw_key;
	struct dentry *root;
	struct dentry *type[_TOTAL_DBCL];
	atomic_t refcount;
	struct list_head entry;
};

/*                     */
struct debug_dbcl {
	struct list_head child;		/*      */
};

/*                         */
enum dbis_types
{
	/*        */
	DBIS_CLIENTS = 0,
	DBIS_BUFFERS,
	DBIS_MMAPS,
	DBIS_FDS,
	DBIS_PIDS,
	_TOTAL_DBIS,

	/*      */
	DBIS_FILE,
	DBIS_DIR,
};

/*                             */
struct dbis_child {
	enum dbis_types attr;
	char *name;
};

struct dbis_child dbis_child_attr[] 
		= { 
		    {DBIS_FILE, "clients"},
		    {DBIS_FILE, "buffers"},
		    {DBIS_FILE, "mmaps"},
		    {DBIS_FILE, "fds"},
		    {DBIS_FILE, "pids"},
		    {DBIS_DIR,  "history_record"},
		    {DBIS_FILE, "history_clients"},
		    {DBIS_FILE, "history_buffers"},
		    {DBIS_FILE, "history_mmaps"},
		    {DBIS_FILE, "history_fds"},
		    {DBIS_FILE, "history_pids"},
		};

/*                   */
struct debug_dbis {
	struct dentry *child[_TOTAL_DBIS+1];
	/*                     */
	struct dentry *history_record[_TOTAL_DBIS];	/*                     */
};

/*                   */
struct ion_debug_db {
	struct dentry *checking_leakage;
	struct debug_dbcl dbcl;
	struct dentry *ion_statistics;
	struct debug_dbis dbis;
};

/*
                         
 */
static struct ion_debug_db debug_db_root;

/*                                                         */
struct dbis_client_entry {
	void *client;
	struct dbis_client_entry *next;
};
struct dbis_process_entry {
	pid_t pid;
	struct dbis_client_entry *clients;
	struct dbis_process_entry *next;
};
/*
                                                                                   
                                                                        
                                                                                     
                                                                        
  
                               
  
                                                                     
                                                               
                                                                       
 */
static inline void dbis_insert_proc_clients(struct dbis_process_entry *plist, void *client, pid_t pid)
{
	struct dbis_client_entry *client_entry = NULL;
	struct dbis_process_entry *process_entry = NULL, *pe_pos = NULL;
	struct dbis_process_entry **next_pe;
	struct dbis_client_entry **next_ce;

	/*                                       */
	client_entry = kmalloc(sizeof(struct dbis_client_entry), GFP_KERNEL);
	if (!client_entry)
		return;
	client_entry->client = client;
	client_entry->next = NULL;

	/*                                             */
	//                       
	next_pe = &plist;
	while (*next_pe) {
		/*                  */
		if ((*next_pe)->pid == pid) {
			next_ce = &(*next_pe)->clients;
			while (*next_ce) {
				next_ce = &(*next_ce)->next;
			}
			*next_ce = client_entry;
//                                   
			return;
		}
		/*                */
		if ((*next_pe)->pid < pid) {
			pe_pos = *next_pe;
		} else {
			break;
		}
		next_pe = &(*next_pe)->next;
	}
	
	/*                  */
	process_entry = kmalloc(sizeof(struct dbis_process_entry), GFP_KERNEL);
	if (!process_entry) {
		kfree(client_entry);
		return;
	}
	process_entry->pid = pid;
	process_entry->clients = client_entry;
	process_entry->next = NULL;

	/*                              */
#if 0
	if (!pe_pos) {
		*next_pe = process_entry;
	} else {
#endif
		process_entry->next = pe_pos->next;
		pe_pos->next = process_entry;
//  
}
static inline void destroy_proclist(struct dbis_process_entry *plist)
{
	struct dbis_process_entry *next_pe, *pe_free;
	struct dbis_client_entry *next_ce, *ce_free;

	/*                                */
	next_pe = plist->next;
	while(next_pe) {
		pe_free = next_pe;
		next_pe = next_pe->next;
		/*                                */
		next_ce = pe_free->clients;
		while (next_ce) {
			ce_free = next_ce;
			next_ce = next_ce->next;
			/*         */
			kfree(ce_free);
		}
		/*         */
		kfree(pe_free);
	}
}

#endif //               
