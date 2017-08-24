/*
 * em_canid.c  Ematch rule to match CAN frames according to their CAN IDs
 *
 *              This program is free software; you can distribute it and/or
 *              modify it under the terms of the GNU General Public License
 *              as published by the Free Software Foundation; either version
 *              2 of the License, or (at your option) any later version.
 *
 * Idea:       Oliver Hartkopp <oliver.hartkopp@volkswagen.de>
 * Copyright:  (c) 2011 Czech Technical University in Prague
 *             (c) 2011 Volkswagen Group Research
 * Authors:    Michal Sojka <sojkam1@fel.cvut.cz>
 *             Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *             Rostislav Lisovy <lisovy@gmail.cz>
 * Funded by:  Volkswagen Group Research
 */

#include <linux/slab.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/skbuff.h>
#include <net/pkt_cls.h>
#include <linux/can.h>

#define EM_CAN_RULES_MAX 500

struct canid_match {
	/*                                                                   */
	DECLARE_BITMAP(match_sff, (1 << CAN_SFF_ID_BITS));

	int rules_count;
	int sff_rules_count;
	int eff_rules_count;

	/*
                                                           
                                                               
                                
  */
	struct can_filter rules_raw[];
};

/* 
                                                                    
 */
static canid_t em_canid_get_id(struct sk_buff *skb)
{
	/*                                        */
	struct can_frame *cf = (struct can_frame *)skb->data;

	return cf->can_id;
}

static void em_canid_sff_match_add(struct canid_match *cm, u32 can_id,
					u32 can_mask)
{
	int i;

	/*
                                             
                                            
  */
	can_mask &= CAN_SFF_MASK;
	can_id &= can_mask;

	/*              */
	if (can_mask == CAN_SFF_MASK) {
		set_bit(can_id, cm->match_sff);
		return;
	}

	/*            */
	if (can_mask == 0) {
		bitmap_fill(cm->match_sff, (1 << CAN_SFF_ID_BITS));
		return;
	}

	/*
                            
                                              
                            
  */
	for (i = 0; i < (1 << CAN_SFF_ID_BITS); i++) {
		if ((i & can_mask) == can_id)
			set_bit(i, cm->match_sff);
	}
}

static inline struct canid_match *em_canid_priv(struct tcf_ematch *m)
{
	return (struct canid_match *)m->data;
}

static int em_canid_match(struct sk_buff *skb, struct tcf_ematch *m,
			 struct tcf_pkt_info *info)
{
	struct canid_match *cm = em_canid_priv(m);
	canid_t can_id;
	int match = 0;
	int i;
	const struct can_filter *lp;

	can_id = em_canid_get_id(skb);

	if (can_id & CAN_EFF_FLAG) {
		for (i = 0, lp = cm->rules_raw;
		     i < cm->eff_rules_count; i++, lp++) {
			if (!(((lp->can_id ^ can_id) & lp->can_mask))) {
				match = 1;
				break;
			}
		}
	} else { /*     */
		can_id &= CAN_SFF_MASK;
		match = (test_bit(can_id, cm->match_sff) ? 1 : 0);
	}

	return match;
}

static int em_canid_change(struct tcf_proto *tp, void *data, int len,
			  struct tcf_ematch *m)
{
	struct can_filter *conf = data; /*                  */
	struct canid_match *cm;
	struct canid_match *cm_old = (struct canid_match *)m->data;
	int i;

	if (!len)
		return -EINVAL;

	if (len % sizeof(struct can_filter))
		return -EINVAL;

	if (len > sizeof(struct can_filter) * EM_CAN_RULES_MAX)
		return -EINVAL;

	cm = kzalloc(sizeof(struct canid_match) + len, GFP_KERNEL);
	if (!cm)
		return -ENOMEM;

	cm->rules_count = len / sizeof(struct can_filter);

	/*
                                                                 
                                                                   
                                                               
                                                                 
                                                              
                                                                 
                                                                   
  */

	/*                                     */
	for (i = 0; i < cm->rules_count; i++) {
		if (conf[i].can_id & CAN_EFF_FLAG) {
			memcpy(cm->rules_raw + cm->eff_rules_count,
				&conf[i],
				sizeof(struct can_filter));

			cm->eff_rules_count++;
		}
	}

	/*                        */
	for (i = 0; i < cm->rules_count; i++) {
		if (!(conf[i].can_id & CAN_EFF_FLAG)) {
			memcpy(cm->rules_raw
				+ cm->eff_rules_count
				+ cm->sff_rules_count,
				&conf[i], sizeof(struct can_filter));

			cm->sff_rules_count++;

			em_canid_sff_match_add(cm,
				conf[i].can_id, conf[i].can_mask);
		}
	}

	m->datalen = sizeof(struct canid_match) + len;
	m->data = (unsigned long)cm;

	if (cm_old != NULL) {
		pr_err("canid: Configuring an existing ematch!\n");
		kfree(cm_old);
	}

	return 0;
}

static void em_canid_destroy(struct tcf_proto *tp, struct tcf_ematch *m)
{
	struct canid_match *cm = em_canid_priv(m);

	kfree(cm);
}

static int em_canid_dump(struct sk_buff *skb, struct tcf_ematch *m)
{
	struct canid_match *cm = em_canid_priv(m);

	/*
                                                                   
                          
  */
	if (nla_put_nohdr(skb, sizeof(struct can_filter) * cm->rules_count,
	    &cm->rules_raw) < 0)
		return -EMSGSIZE;

	return 0;
}

static struct tcf_ematch_ops em_canid_ops = {
	.kind	  = TCF_EM_CANID,
	.change	  = em_canid_change,
	.match	  = em_canid_match,
	.destroy  = em_canid_destroy,
	.dump	  = em_canid_dump,
	.owner	  = THIS_MODULE,
	.link	  = LIST_HEAD_INIT(em_canid_ops.link)
};

static int __init init_em_canid(void)
{
	return tcf_em_register(&em_canid_ops);
}

static void __exit exit_em_canid(void)
{
	tcf_em_unregister(&em_canid_ops);
}

MODULE_LICENSE("GPL");

module_init(init_em_canid);
module_exit(exit_em_canid);

MODULE_ALIAS_TCF_EMATCH(TCF_EM_CANID);
