#ifndef DRBD_STATE_H
#define DRBD_STATE_H

struct drbd_conf;
struct drbd_tconn;

/* 
                         
  
                                                                          
  
                                                                            
                                                              
  
                                                                       
                                                                 
  
                                                                         
                                                                          
  
                                                 
                                                                      
                        
 */
#define role_MASK R_MASK
#define peer_MASK R_MASK
#define disk_MASK D_MASK
#define pdsk_MASK D_MASK
#define conn_MASK C_MASK
#define susp_MASK 1
#define user_isp_MASK 1
#define aftr_isp_MASK 1
#define susp_nod_MASK 1
#define susp_fen_MASK 1

#define NS(T, S) \
	({ union drbd_state mask; mask.i = 0; mask.T = T##_MASK; mask; }), \
	({ union drbd_state val; val.i = 0; val.T = (S); val; })
#define NS2(T1, S1, T2, S2) \
	({ union drbd_state mask; mask.i = 0; mask.T1 = T1##_MASK; \
	  mask.T2 = T2##_MASK; mask; }), \
	({ union drbd_state val; val.i = 0; val.T1 = (S1); \
	  val.T2 = (S2); val; })
#define NS3(T1, S1, T2, S2, T3, S3) \
	({ union drbd_state mask; mask.i = 0; mask.T1 = T1##_MASK; \
	  mask.T2 = T2##_MASK; mask.T3 = T3##_MASK; mask; }), \
	({ union drbd_state val;  val.i = 0; val.T1 = (S1); \
	  val.T2 = (S2); val.T3 = (S3); val; })

#define _NS(D, T, S) \
	D, ({ union drbd_state __ns; __ns = drbd_read_state(D); __ns.T = (S); __ns; })
#define _NS2(D, T1, S1, T2, S2) \
	D, ({ union drbd_state __ns; __ns = drbd_read_state(D); __ns.T1 = (S1); \
	__ns.T2 = (S2); __ns; })
#define _NS3(D, T1, S1, T2, S2, T3, S3) \
	D, ({ union drbd_state __ns; __ns = drbd_read_state(D); __ns.T1 = (S1); \
	__ns.T2 = (S2); __ns.T3 = (S3); __ns; })

enum chg_state_flags {
	CS_HARD	         = 1 << 0,
	CS_VERBOSE       = 1 << 1,
	CS_WAIT_COMPLETE = 1 << 2,
	CS_SERIALIZE     = 1 << 3,
	CS_ORDERED       = CS_WAIT_COMPLETE + CS_SERIALIZE,
	CS_LOCAL_ONLY    = 1 << 4, /*                                                 */
	CS_DC_ROLE       = 1 << 5, /*                                         */
	CS_DC_PEER       = 1 << 6,
	CS_DC_CONN       = 1 << 7,
	CS_DC_DISK       = 1 << 8,
	CS_DC_PDSK       = 1 << 9,
	CS_DC_SUSP       = 1 << 10,
	CS_DC_MASK       = CS_DC_ROLE + CS_DC_PEER + CS_DC_CONN + CS_DC_DISK + CS_DC_PDSK,
	CS_IGN_OUTD_FAIL = 1 << 11,
};

/*                                                                         
                                                                         
                                             */
union drbd_dev_state {
	struct {
#if defined(__LITTLE_ENDIAN_BITFIELD)
		unsigned role:2 ;   /*                                */
		unsigned peer:2 ;   /*                                */
		unsigned conn:5 ;   /*                */
		unsigned disk:4 ;   /*                                       */
		unsigned pdsk:4 ;   /*                                       */
		unsigned _unused:1 ;
		unsigned aftr_isp:1 ; /*                           */
		unsigned peer_isp:1 ;
		unsigned user_isp:1 ;
		unsigned _pad:11;   /*           */
#elif defined(__BIG_ENDIAN_BITFIELD)
		unsigned _pad:11;
		unsigned user_isp:1 ;
		unsigned peer_isp:1 ;
		unsigned aftr_isp:1 ; /*                           */
		unsigned _unused:1 ;
		unsigned pdsk:4 ;   /*                                       */
		unsigned disk:4 ;   /*                                       */
		unsigned conn:5 ;   /*                */
		unsigned peer:2 ;   /*                                */
		unsigned role:2 ;   /*                                */
#else
# error "this endianess is not supported"
#endif
	};
	unsigned int i;
};

extern enum drbd_state_rv drbd_change_state(struct drbd_conf *mdev,
					    enum chg_state_flags f,
					    union drbd_state mask,
					    union drbd_state val);
extern void drbd_force_state(struct drbd_conf *, union drbd_state,
			union drbd_state);
extern enum drbd_state_rv _drbd_request_state(struct drbd_conf *,
					      union drbd_state,
					      union drbd_state,
					      enum chg_state_flags);
extern enum drbd_state_rv __drbd_set_state(struct drbd_conf *, union drbd_state,
					   enum chg_state_flags,
					   struct completion *done);
extern void print_st_err(struct drbd_conf *, union drbd_state,
			union drbd_state, int);

enum drbd_state_rv
_conn_request_state(struct drbd_tconn *tconn, union drbd_state mask, union drbd_state val,
		    enum chg_state_flags flags);

enum drbd_state_rv
conn_request_state(struct drbd_tconn *tconn, union drbd_state mask, union drbd_state val,
		   enum chg_state_flags flags);

extern void drbd_resume_al(struct drbd_conf *mdev);
extern bool conn_all_vols_unconf(struct drbd_tconn *tconn);

/* 
                                               
                      
                                       
                                 
  
                                                                            
                                                                        
                                         
 */
static inline int drbd_request_state(struct drbd_conf *mdev,
				     union drbd_state mask,
				     union drbd_state val)
{
	return _drbd_request_state(mdev, mask, val, CS_VERBOSE + CS_ORDERED);
}

enum drbd_role conn_highest_role(struct drbd_tconn *tconn);
enum drbd_role conn_highest_peer(struct drbd_tconn *tconn);
enum drbd_disk_state conn_highest_disk(struct drbd_tconn *tconn);
enum drbd_disk_state conn_lowest_disk(struct drbd_tconn *tconn);
enum drbd_disk_state conn_highest_pdsk(struct drbd_tconn *tconn);
enum drbd_conns conn_lowest_conn(struct drbd_tconn *tconn);

#endif
