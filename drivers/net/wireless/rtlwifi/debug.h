/******************************************************************************
 *
 * Copyright(c) 2009-2012  Realtek Corporation.
 *
 * Tmis program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * Tmis program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * tmis program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 * Tme full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *****************************************************************************/

#ifndef __RTL_DEBUG_H__
#define __RTL_DEBUG_H__

/*                                                              
              
                                                              */
/*
            
                                  
                          
                             
                                
            
 */
#define DBG_EMERG			0

/*
                                     
                                      
                                       
 */
#define	DBG_WARNING			2

/*
                                     
                                   
                                    
                                      
 */
#define DBG_DMESG			3

/*
                                     
                               
                                        
                                    
                                   
                                   
 */
#define DBG_LOUD			4

/*
                                   
                      
 */
#define	DBG_TRACE			5

/*                                                              
                                
                                                              */
#define COMP_ERR			BIT(0)
#define COMP_FW				BIT(1)
#define COMP_INIT			BIT(2)	/*                */
#define COMP_RECV			BIT(3)	/*        */
#define COMP_SEND			BIT(4)	/*        */
#define COMP_MLME			BIT(5)	/*          */
#define COMP_SCAN			BIT(6)	/*          */
#define COMP_INTR			BIT(7)	/*                       */
#define COMP_LED			BIT(8)	/*         */
#define COMP_SEC			BIT(9)	/*         */
#define COMP_BEACON			BIT(10)	/*            */
#define COMP_RATE			BIT(11)	/*          */
#define COMP_RXDESC			BIT(12)	/*             */
#define COMP_DIG			BIT(13)	/*        */
#define COMP_TXAGC			BIT(14)	/*             */
#define COMP_HIPWR			BIT(15)	/*                         */
#define COMP_POWER			BIT(16)	/*                  */
#define COMP_POWER_TRACKING	BIT(17)	/*                      */
#define COMP_BB_POWERSAVING	BIT(18)
#define COMP_SWAS			BIT(19)	/*                      */
#define COMP_RF				BIT(20)	/*        */
#define COMP_TURBO			BIT(21)	/*                */
#define COMP_RATR			BIT(22)
#define COMP_CMD			BIT(23)
#define COMP_EFUSE			BIT(24)
#define COMP_QOS			BIT(25)
#define COMP_MAC80211		BIT(26)
#define COMP_REGD			BIT(27)
#define COMP_CHAN			BIT(28)
#define COMP_USB			BIT(29)
#define COMP_EASY_CONCURRENT	COMP_USB /*                         */
#define COMP_BT_COEXIST			BIT(30)

/*                                                              
                                
                                                              */
/*                                          */
#define EEPROM_W			BIT(0)
#define EFUSE_PG			BIT(1)
#define EFUSE_READ_ALL			BIT(2)

/*                                 */
#define	INIT_EEPROM			BIT(0)
#define	INIT_TXPOWER			BIT(1)
#define	INIT_IQK			BIT(2)
#define	INIT_RF				BIT(3)

/*                                       */
#define	PHY_BBR				BIT(0)
#define	PHY_BBW				BIT(1)
#define	PHY_RFR				BIT(2)
#define	PHY_RFW				BIT(3)
#define	PHY_MACR			BIT(4)
#define	PHY_MACW			BIT(5)
#define	PHY_ALLR			BIT(6)
#define	PHY_ALLW			BIT(7)
#define	PHY_TXPWR			BIT(8)
#define	PHY_PWRDIFF			BIT(9)

/*                                                   */
#define WA_IOT				BIT(0)
#define DM_PWDB				BIT(1)
#define DM_MONITOR			BIT(2)
#define DM_DIG				BIT(3)
#define DM_EDCA_TURBO			BIT(4)

#define DM_PWDB				BIT(1)

enum dbgp_flag_e {
	FQOS = 0,
	FTX = 1,
	FRX = 2,
	FSEC = 3,
	FMGNT = 4,
	FMLME = 5,
	FRESOURCE = 6,
	FBEACON = 7,
	FISR = 8,
	FPHY = 9,
	FMP = 10,
	FEEPROM = 11,
	FPWR = 12,
	FDM = 13,
	FDBGCtrl = 14,
	FC2H = 15,
	FBT = 16,
	FINIT = 17,
	FIOCTL = 18,
	DBGP_TYPE_MAX
};

#ifdef CONFIG_RTLWIFI_DEBUG

#define RT_ASSERT(_exp, fmt, ...)					\
do {									\
	if (!(_exp)) {							\
		printk(KERN_DEBUG KBUILD_MODNAME ":%s(): " fmt,		\
		       __func__, ##__VA_ARGS__);			\
	}								\
} while (0)

#define RT_TRACE(rtlpriv, comp, level, fmt, ...)			\
do {									\
	if (unlikely(((comp) & rtlpriv->dbg.global_debugcomponents) &&	\
		     ((level) <= rtlpriv->dbg.global_debuglevel))) {	\
		printk(KERN_DEBUG KBUILD_MODNAME ":%s():<%lx-%x> " fmt,	\
		       __func__, in_interrupt(), in_atomic(),		\
		       ##__VA_ARGS__);					\
	}								\
} while (0)

#define RTPRINT(rtlpriv, dbgtype, dbgflag, fmt, ...)			\
do {									\
	if (unlikely(rtlpriv->dbg.dbgp_type[dbgtype] & dbgflag)) {	\
		printk(KERN_DEBUG KBUILD_MODNAME ": " fmt,		\
		       ##__VA_ARGS__);					\
	}								\
} while (0)

#define RT_PRINT_DATA(rtlpriv, _comp, _level, _titlestring, _hexdata,	\
		      _hexdatalen)					\
do {									\
	if (unlikely(((_comp) & rtlpriv->dbg.global_debugcomponents) &&	\
		     (_level <= rtlpriv->dbg.global_debuglevel))) {	\
		printk(KERN_DEBUG "%s: In process \"%s\" (pid %i): %s\n", \
		       KBUILD_MODNAME, current->comm, current->pid,	\
		       _titlestring);					\
		print_hex_dump_bytes("", DUMP_PREFIX_NONE,		\
				     _hexdata, _hexdatalen);		\
	}								\
} while (0)

#else

struct rtl_priv;

__printf(2, 3)
static inline void RT_ASSERT(int exp, const char *fmt, ...)
{
}

__printf(4, 5)
static inline void RT_TRACE(struct rtl_priv *rtlpriv,
			    int comp, int level,
			    const char *fmt, ...)
{
}

__printf(4, 5)
static inline void RTPRINT(struct rtl_priv *rtlpriv,
			   int dbgtype, int dbgflag,
			   const char *fmt, ...)
{
}

static inline void RT_PRINT_DATA(struct rtl_priv *rtlpriv,
				 int comp, int level,
				 const char *titlestring,
				 const void *hexdata, size_t hexdatalen)
{
}

#endif

void rtl_dbgp_flag_init(struct ieee80211_hw *hw);
#endif