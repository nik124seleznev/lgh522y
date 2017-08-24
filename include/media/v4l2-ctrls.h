/*
    V4L2 controls support header.

    Copyright (C) 2010  Hans Verkuil <hverkuil@xs4all.nl>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _V4L2_CTRLS_H
#define _V4L2_CTRLS_H

#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/videodev2.h>

/*                    */
struct file;
struct v4l2_ctrl_handler;
struct v4l2_ctrl_helper;
struct v4l2_ctrl;
struct video_device;
struct v4l2_subdev;
struct v4l2_subscribed_event;
struct v4l2_fh;
struct poll_table_struct;

/*                                                                               
                                                                               
                                                                    
                                                           
                  
                                                                  
                                                                            
                                                      
                                                                          
                                                                 
                                                        
  */
struct v4l2_ctrl_ops {
	int (*g_volatile_ctrl)(struct v4l2_ctrl *ctrl);
	int (*try_ctrl)(struct v4l2_ctrl *ctrl);
	int (*s_ctrl)(struct v4l2_ctrl *ctrl);
};

typedef void (*v4l2_ctrl_notify_fnc)(struct v4l2_ctrl *ctrl, void *priv);

/*                                           
                         
                                                      
                                                
                                              
                                                    
                                                         
                                                                         
                                                                  
                                
                                                                           
                                                             
               
                                                                           
                                                              
                                                               
                            
                                                                                 
                                          
                                                                              
                             
                                                                          
                                                              
                                                                 
                                                                    
                                  
                          
                        
                            
                            
                                          
                                          
                                                
                                                          
                                                                             
                                                                 
                                                                
                                                              
                                                                   
                                                                
                                                                           
                                                                   
                                                                
                  
                                
                                      
                                      
                                        
                                            
                                                                      
                                                                  
                                                                 
                                                                    
                          
  */
struct v4l2_ctrl {
	/*                       */
	struct list_head node;
	struct list_head ev_subs;
	struct v4l2_ctrl_handler *handler;
	struct v4l2_ctrl **cluster;
	unsigned ncontrols;
	unsigned int done:1;

	unsigned int is_new:1;
	unsigned int is_private:1;
	unsigned int is_auto:1;
	unsigned int has_volatiles:1;
	unsigned int call_notify:1;
	unsigned int manual_mode_value:8;

	const struct v4l2_ctrl_ops *ops;
	u32 id;
	const char *name;
	enum v4l2_ctrl_type type;
	s32 minimum, maximum, default_value;
	union {
		u32 step;
		u32 menu_skip_mask;
	};
	union {
		const char * const *qmenu;
		const s64 *qmenu_int;
	};
	unsigned long flags;
	union {
		s32 val;
		s64 val64;
		char *string;
	} cur;
	union {
		s32 val;
		s64 val64;
		char *string;
	};
	void *priv;
};

/*                                               
                                         
                                              
                                          
                                                                              
   
                                                                           
                                                                            
                                                     
  */
struct v4l2_ctrl_ref {
	struct list_head node;
	struct v4l2_ctrl_ref *next;
	struct v4l2_ctrl *ctrl;
	struct v4l2_ctrl_helper *helper;
};

/*                                                                       
                                                                        
                        
                               
                                                                   
                                                  
                                                       
                                               
                                                                         
                                                          
                  
                                                                       
                                                                                 
                                                                  
               
                                                                      
                                                         
                                                                
  */
struct v4l2_ctrl_handler {
	struct mutex _lock;
	struct mutex *lock;
	struct list_head ctrls;
	struct list_head ctrl_refs;
	struct v4l2_ctrl_ref *cached;
	struct v4l2_ctrl_ref **buckets;
	v4l2_ctrl_notify_fnc notify;
	void *notify_priv;
	u16 nr_of_buckets;
	int error;
};

/*                                                            
                          
                        
                            
                            
                                      
                                      
                                                          
                                       
                                
                                                                             
                                                                 
                                                                
                                                              
                                                                   
                                                                
                                                                           
                                                                   
                                                                
                  
                                                                           
                                             
  */
struct v4l2_ctrl_config {
	const struct v4l2_ctrl_ops *ops;
	u32 id;
	const char *name;
	enum v4l2_ctrl_type type;
	s32 min;
	s32 max;
	u32 step;
	s32 def;
	u32 flags;
	u32 menu_skip_mask;
	const char * const *qmenu;
	const s64 *qmenu_int;
	unsigned int is_private:1;
};

/*                                                                        
   
                                              
                                                                      
                           
   
                                                                         
                                                                           
             
   
                                                                            
                                                                        
                                                               
  */
void v4l2_ctrl_fill(u32 id, const char **name, enum v4l2_ctrl_type *type,
		    s32 *min, s32 *max, s32 *step, s32 *def, u32 *flags);


/*                                                                  
                              
                                                                     
                                                                 
                                                                  
                                                                  
                                                                    
                                                                 
                                 
                                                              
                                                               
   
                                                                           
                                  
   
                                                                       
                                                  
  */
int v4l2_ctrl_handler_init_class(struct v4l2_ctrl_handler *hdl,
				 unsigned nr_of_controls_hint,
				 struct lock_class_key *key, const char *name);

#ifdef CONFIG_LOCKDEP
#define v4l2_ctrl_handler_init(hdl, nr_of_controls_hint)		\
(									\
	({								\
		static struct lock_class_key _key;			\
		v4l2_ctrl_handler_init_class(hdl, nr_of_controls_hint,	\
					&_key,				\
					KBUILD_BASENAME ":"		\
					__stringify(__LINE__) ":"	\
					"(" #hdl ")->_lock");		\
	})								\
)
#else
#define v4l2_ctrl_handler_init(hdl, nr_of_controls_hint)		\
	v4l2_ctrl_handler_init_class(hdl, nr_of_controls_hint, NULL, NULL)
#endif

/*                                                                            
                     
                              
   
                                 
  */
void v4l2_ctrl_handler_free(struct v4l2_ctrl_handler *hdl);

/*                                                                           
                                                                            
                              
   
                                                               
   
                                              
  */
int v4l2_ctrl_handler_setup(struct v4l2_ctrl_handler *hdl);

/*                                                                         
                              
                                                                      
                                                              
                                                                 
          
   
                                   
   
                                 
  */
void v4l2_ctrl_handler_log_status(struct v4l2_ctrl_handler *hdl,
				  const char *prefix);

/*                                                                    
            
                              
                                           
                                                      
   
                                                                         
                                                                        
  */
struct v4l2_ctrl *v4l2_ctrl_new_custom(struct v4l2_ctrl_handler *hdl,
			const struct v4l2_ctrl_config *cfg, void *priv);

/*                                                                                     
                              
                          
                        
                                      
                                      
                                   
                                       
   
                                                                   
                                                                        
                                                      
   
                                                                         
   
                                                           
  */
struct v4l2_ctrl *v4l2_ctrl_new_std(struct v4l2_ctrl_handler *hdl,
			const struct v4l2_ctrl_ops *ops,
			u32 id, s32 min, s32 max, u32 step, s32 def);

/*                                                                                      
                              
                          
                        
                                      
                                                                    
                                                                 
                                                                
                                                              
                                                                   
                                                                
                                       
   
                                                                         
                                                  
   
                                                                             
  */
struct v4l2_ctrl *v4l2_ctrl_new_std_menu(struct v4l2_ctrl_handler *hdl,
			const struct v4l2_ctrl_ops *ops,
			u32 id, s32 max, s32 mask, s32 def);

/*                                                                          
                              
                              
                          
                        
                                      
                                                                   
                                                                 
                                                                
                                                              
                                                                   
                                                                
                                      
                         
   
                                                                            
                         
   
  */
struct v4l2_ctrl *v4l2_ctrl_new_std_menu_items(struct v4l2_ctrl_handler *hdl,
			const struct v4l2_ctrl_ops *ops, u32 id, s32 max,
			s32 mask, s32 def, const char * const *qmenu);

/*                                                                             
                              
                          
                        
                                      
                                      
                                           
   
                                                                              
                                                                         
   
                                                                                     
  */
struct v4l2_ctrl *v4l2_ctrl_new_int_menu(struct v4l2_ctrl_handler *hdl,
			const struct v4l2_ctrl_ops *ops,
			u32 id, s32 max, s32 def, const s64 *qmenu_int);

/*                                                                            
                              
                              
   
                                                                      
                                                                   
                                  
  */
struct v4l2_ctrl *v4l2_ctrl_add_ctrl(struct v4l2_ctrl_handler *hdl,
					  struct v4l2_ctrl *ctrl);

/*                                                                 
                 
                              
                                                               
                              
                                                                      
   
                                                                 
                                                                         
                                                          
                                                                        
                        
  */
int v4l2_ctrl_add_handler(struct v4l2_ctrl_handler *hdl,
			  struct v4l2_ctrl_handler *add,
			  bool (*filter)(const struct v4l2_ctrl *ctrl));

/*                                                                
                                        
   
                                                                          
                                                                         
                               
   
                                                             
  */
bool v4l2_ctrl_radio_filter(const struct v4l2_ctrl *ctrl);

/*                                                                                      
                                                       
                                                             
  */
void v4l2_ctrl_cluster(unsigned ncontrols, struct v4l2_ctrl **controls);


/*                                                                             
                                                             
                                                       
                                                                              
                                                                   
                                                                               
                    
                                                                                
                 
   
                                                                               
                                                                               
                                                                               
                              
   
                                                
   
                                                                          
                                                                               
                         
   
                                                                            
                                                                              
                                  
   
                                                                      
                                                                            
                               
  */
void v4l2_ctrl_auto_cluster(unsigned ncontrols, struct v4l2_ctrl **controls,
			u8 manual_val, bool set_volatile);


/*                                                      
                              
                                
   
                                                                           
                                          
  */
struct v4l2_ctrl *v4l2_ctrl_find(struct v4l2_ctrl_handler *hdl, u32 id);

/*                                                             
                                       
                                                      
   
                                                                    
                                  
                                                          
                                                           
   
                                                             
  */
void v4l2_ctrl_activate(struct v4l2_ctrl *ctrl, bool active);

/*                                                                
                                       
                                                        
   
                                                                   
                                  
                                                           
                                                                          
           
   
                                                                         
                         
  */
void v4l2_ctrl_grab(struct v4l2_ctrl *ctrl, bool grabbed);

/*                                                           
                                 
                                      
                                      
                                   
                                      
   
                                                                          
                                                                           
                                                   
   
                                                                          
                 
   
                                                                         
                         
  */
int v4l2_ctrl_modify_range(struct v4l2_ctrl *ctrl,
			s32 min, s32 max, u32 step, s32 def);

/*                                                        
                                
                               
  */
static inline void v4l2_ctrl_lock(struct v4l2_ctrl *ctrl)
{
	mutex_lock(ctrl->handler->lock);
}

/*                                                          
                                
                                 
  */
static inline void v4l2_ctrl_unlock(struct v4l2_ctrl *ctrl)
{
	mutex_unlock(ctrl->handler->lock);
}

/*                                                                       
                       
                                   
                                                                           
   
                                                                             
                                                                              
               
   
                                                                           
                                                    
  */
void v4l2_ctrl_notify(struct v4l2_ctrl *ctrl, v4l2_ctrl_notify_fnc notify, void *priv);

/*                                                                                       
                       
   
                                                                        
                                                                             
                                                  
   
                                                    
  */
s32 v4l2_ctrl_g_ctrl(struct v4l2_ctrl *ctrl);

/*                                                                                       
                       
                        
   
                                                                        
                                                                             
                                                  
   
                                                    
  */
int v4l2_ctrl_s_ctrl(struct v4l2_ctrl *ctrl, s32 val);

/*                                                                                                  
                       
   
                                                                        
                                                                             
                                                  
   
                                                           
  */
s64 v4l2_ctrl_g_ctrl_int64(struct v4l2_ctrl *ctrl);

/*                                                                                                  
                       
                        
   
                                                                        
                                                                             
                                                  
   
                                                           
  */
int v4l2_ctrl_s_ctrl_int64(struct v4l2_ctrl *ctrl, s64 val);

/*                                                          */
extern const struct v4l2_subscribed_event_ops v4l2_ctrl_sub_ev_ops;
void v4l2_ctrl_replace(struct v4l2_event *old, const struct v4l2_event *new);
void v4l2_ctrl_merge(const struct v4l2_event *old, struct v4l2_event *new);

/*                                                                         
                                   */
int v4l2_ctrl_log_status(struct file *file, void *fh);

/*                                                                      
                   */
int v4l2_ctrl_subscribe_event(struct v4l2_fh *fh,
				const struct v4l2_event_subscription *sub);

/*                                                                    */
unsigned int v4l2_ctrl_poll(struct file *file, struct poll_table_struct *wait);

/*                                                                          */
int v4l2_queryctrl(struct v4l2_ctrl_handler *hdl, struct v4l2_queryctrl *qc);
int v4l2_querymenu(struct v4l2_ctrl_handler *hdl, struct v4l2_querymenu *qm);
int v4l2_g_ctrl(struct v4l2_ctrl_handler *hdl, struct v4l2_control *ctrl);
int v4l2_s_ctrl(struct v4l2_fh *fh, struct v4l2_ctrl_handler *hdl,
						struct v4l2_control *ctrl);
int v4l2_g_ext_ctrls(struct v4l2_ctrl_handler *hdl, struct v4l2_ext_controls *c);
int v4l2_try_ext_ctrls(struct v4l2_ctrl_handler *hdl, struct v4l2_ext_controls *c);
int v4l2_s_ext_ctrls(struct v4l2_fh *fh, struct v4l2_ctrl_handler *hdl,
						struct v4l2_ext_controls *c);

/*                                                                         
                            */
int v4l2_subdev_queryctrl(struct v4l2_subdev *sd, struct v4l2_queryctrl *qc);
int v4l2_subdev_querymenu(struct v4l2_subdev *sd, struct v4l2_querymenu *qm);
int v4l2_subdev_g_ext_ctrls(struct v4l2_subdev *sd, struct v4l2_ext_controls *cs);
int v4l2_subdev_try_ext_ctrls(struct v4l2_subdev *sd, struct v4l2_ext_controls *cs);
int v4l2_subdev_s_ext_ctrls(struct v4l2_subdev *sd, struct v4l2_ext_controls *cs);
int v4l2_subdev_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl);
int v4l2_subdev_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl);

/*                                                                       
           */
int v4l2_ctrl_subdev_subscribe_event(struct v4l2_subdev *sd, struct v4l2_fh *fh,
				     struct v4l2_event_subscription *sub);

/*                                                     */
int v4l2_ctrl_subdev_log_status(struct v4l2_subdev *sd);

#endif