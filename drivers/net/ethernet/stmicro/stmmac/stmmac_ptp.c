/*******************************************************************************
  PTP 1588 clock using the STMMAC.

  Copyright (C) 2013  Vayavya Labs Pvt Ltd

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Author: Rayagond Kokatanur <rayagond@vayavyalabs.com>
*******************************************************************************/
#include "stmmac.h"
#include "stmmac_ptp.h"

/* 
                     
  
                                            
                                                   
  
                                                                          
 */
static int stmmac_adjust_freq(struct ptp_clock_info *ptp, s32 ppb)
{
	struct stmmac_priv *priv =
	    container_of(ptp, struct stmmac_priv, ptp_clock_ops);
	unsigned long flags;
	u32 diff, addend;
	int neg_adj = 0;
	u64 adj;

	if (ppb < 0) {
		neg_adj = 1;
		ppb = -ppb;
	}

	addend = priv->default_addend;
	adj = addend;
	adj *= ppb;
	diff = div_u64(adj, 1000000000ULL);
	addend = neg_adj ? (addend - diff) : (addend + diff);

	spin_lock_irqsave(&priv->ptp_lock, flags);

	priv->hw->ptp->config_addend(priv->ioaddr, addend);

	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

/* 
                     
  
                                            
                                        
  
                                                                        
 */
static int stmmac_adjust_time(struct ptp_clock_info *ptp, s64 delta)
{
	struct stmmac_priv *priv =
	    container_of(ptp, struct stmmac_priv, ptp_clock_ops);
	unsigned long flags;
	u32 sec, nsec;
	u32 quotient, reminder;
	int neg_adj = 0;

	if (delta < 0) {
		neg_adj = 1;
		delta = -delta;
	}

	quotient = div_u64_rem(delta, 1000000000ULL, &reminder);
	sec = quotient;
	nsec = reminder;

	spin_lock_irqsave(&priv->ptp_lock, flags);

	priv->hw->ptp->adjust_systime(priv->ioaddr, sec, nsec, neg_adj);

	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

/* 
                  
  
                                            
                                   
  
                                                                 
                                      
 */
static int stmmac_get_time(struct ptp_clock_info *ptp, struct timespec *ts)
{
	struct stmmac_priv *priv =
	    container_of(ptp, struct stmmac_priv, ptp_clock_ops);
	unsigned long flags;
	u64 ns;
	u32 reminder;

	spin_lock_irqsave(&priv->ptp_lock, flags);

	ns = priv->hw->ptp->get_systime(priv->ioaddr);

	spin_unlock_irqrestore(&priv->ptp_lock, flags);

	ts->tv_sec = div_u64_rem(ns, 1000000000ULL, &reminder);
	ts->tv_nsec = reminder;

	return 0;
}

/* 
                  
  
                                            
                         
  
                                                              
                  
 */
static int stmmac_set_time(struct ptp_clock_info *ptp,
			   const struct timespec *ts)
{
	struct stmmac_priv *priv =
	    container_of(ptp, struct stmmac_priv, ptp_clock_ops);
	unsigned long flags;

	spin_lock_irqsave(&priv->ptp_lock, flags);

	priv->hw->ptp->init_systime(priv->ioaddr, ts->tv_sec, ts->tv_nsec);

	spin_unlock_irqrestore(&priv->ptp_lock, flags);

	return 0;
}

static int stmmac_enable(struct ptp_clock_info *ptp,
			 struct ptp_clock_request *rq, int on)
{
	return -EOPNOTSUPP;
}

/*                                           */
static struct ptp_clock_info stmmac_ptp_clock_ops = {
	.owner = THIS_MODULE,
	.name = "stmmac_ptp_clock",
	.max_adj = 62500000,
	.n_alarm = 0,
	.n_ext_ts = 0,
	.n_per_out = 0,
	.pps = 0,
	.adjfreq = stmmac_adjust_freq,
	.adjtime = stmmac_adjust_time,
	.gettime = stmmac_get_time,
	.settime = stmmac_set_time,
	.enable = stmmac_enable,
};

/* 
                      
                                  
                                                                
                                                   
 */
int stmmac_ptp_register(struct stmmac_priv *priv)
{
	spin_lock_init(&priv->ptp_lock);
	priv->ptp_clock_ops = stmmac_ptp_clock_ops;

	priv->ptp_clock = ptp_clock_register(&priv->ptp_clock_ops,
					     priv->device);
	if (IS_ERR(priv->ptp_clock)) {
		priv->ptp_clock = NULL;
		pr_err("ptp_clock_register() failed on %s\n", priv->dev->name);
	} else
		pr_debug("Added PTP HW clock successfully on %s\n",
			 priv->dev->name);

	return 0;
}

/* 
                        
                                  
                                                                         
                   
 */
void stmmac_ptp_unregister(struct stmmac_priv *priv)
{
	if (priv->ptp_clock) {
		ptp_clock_unregister(priv->ptp_clock);
		pr_debug("Removed PTP HW clock successfully on %s\n",
			 priv->dev->name);
	}
}
