/*
 * Linux network driver for Brocade Converged Network Adapter.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License (GPL) Version 2 as
 * published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */
/*
 * Copyright (c) 2005-2010 Brocade Communications Systems, Inc.
 * All rights reserved
 * www.brocade.com
 */

#include "bfa_ioc.h"
#include "bfi_reg.h"
#include "bfa_defs.h"

/*                       */

/*                                                                     */

#define bfa_ioc_firmware_lock(__ioc)			\
			((__ioc)->ioc_hwif->ioc_firmware_lock(__ioc))
#define bfa_ioc_firmware_unlock(__ioc)			\
			((__ioc)->ioc_hwif->ioc_firmware_unlock(__ioc))
#define bfa_ioc_reg_init(__ioc) ((__ioc)->ioc_hwif->ioc_reg_init(__ioc))
#define bfa_ioc_map_port(__ioc) ((__ioc)->ioc_hwif->ioc_map_port(__ioc))
#define bfa_ioc_notify_fail(__ioc)			\
			((__ioc)->ioc_hwif->ioc_notify_fail(__ioc))
#define bfa_ioc_sync_start(__ioc)               \
			((__ioc)->ioc_hwif->ioc_sync_start(__ioc))
#define bfa_ioc_sync_join(__ioc)			\
			((__ioc)->ioc_hwif->ioc_sync_join(__ioc))
#define bfa_ioc_sync_leave(__ioc)			\
			((__ioc)->ioc_hwif->ioc_sync_leave(__ioc))
#define bfa_ioc_sync_ack(__ioc)				\
			((__ioc)->ioc_hwif->ioc_sync_ack(__ioc))
#define bfa_ioc_sync_complete(__ioc)			\
			((__ioc)->ioc_hwif->ioc_sync_complete(__ioc))

#define bfa_ioc_mbox_cmd_pending(__ioc)		\
			(!list_empty(&((__ioc)->mbox_mod.cmd_q)) || \
			readl((__ioc)->ioc_regs.hfn_mbox_cmd))

static bool bfa_nw_auto_recover = true;

/*
                       
 */
static void bfa_ioc_hw_sem_init(struct bfa_ioc *ioc);
static void bfa_ioc_hw_sem_get(struct bfa_ioc *ioc);
static void bfa_ioc_hw_sem_get_cancel(struct bfa_ioc *ioc);
static void bfa_ioc_hwinit(struct bfa_ioc *ioc, bool force);
static void bfa_ioc_poll_fwinit(struct bfa_ioc *ioc);
static void bfa_ioc_send_enable(struct bfa_ioc *ioc);
static void bfa_ioc_send_disable(struct bfa_ioc *ioc);
static void bfa_ioc_send_getattr(struct bfa_ioc *ioc);
static void bfa_ioc_hb_monitor(struct bfa_ioc *ioc);
static void bfa_ioc_hb_stop(struct bfa_ioc *ioc);
static void bfa_ioc_reset(struct bfa_ioc *ioc, bool force);
static void bfa_ioc_mbox_poll(struct bfa_ioc *ioc);
static void bfa_ioc_mbox_flush(struct bfa_ioc *ioc);
static void bfa_ioc_recover(struct bfa_ioc *ioc);
static void bfa_ioc_event_notify(struct bfa_ioc *, enum bfa_ioc_event);
static void bfa_ioc_disable_comp(struct bfa_ioc *ioc);
static void bfa_ioc_lpu_stop(struct bfa_ioc *ioc);
static void bfa_nw_ioc_debug_save_ftrc(struct bfa_ioc *ioc);
static void bfa_ioc_fail_notify(struct bfa_ioc *ioc);
static void bfa_ioc_pf_enabled(struct bfa_ioc *ioc);
static void bfa_ioc_pf_disabled(struct bfa_ioc *ioc);
static void bfa_ioc_pf_failed(struct bfa_ioc *ioc);
static void bfa_ioc_pf_hwfailed(struct bfa_ioc *ioc);
static void bfa_ioc_pf_fwmismatch(struct bfa_ioc *ioc);
static void bfa_ioc_boot(struct bfa_ioc *ioc, enum bfi_fwboot_type boot_type,
			 u32 boot_param);
static u32 bfa_ioc_smem_pgnum(struct bfa_ioc *ioc, u32 fmaddr);
static void bfa_ioc_get_adapter_serial_num(struct bfa_ioc *ioc,
						char *serial_num);
static void bfa_ioc_get_adapter_fw_ver(struct bfa_ioc *ioc,
						char *fw_ver);
static void bfa_ioc_get_pci_chip_rev(struct bfa_ioc *ioc,
						char *chip_rev);
static void bfa_ioc_get_adapter_optrom_ver(struct bfa_ioc *ioc,
						char *optrom_ver);
static void bfa_ioc_get_adapter_manufacturer(struct bfa_ioc *ioc,
						char *manufacturer);
static void bfa_ioc_get_adapter_model(struct bfa_ioc *ioc, char *model);
static u64 bfa_ioc_get_pwwn(struct bfa_ioc *ioc);

/*                                            */
enum ioc_event {
	IOC_E_RESET		= 1,	/*                      */
	IOC_E_ENABLE		= 2,	/*                       */
	IOC_E_DISABLE		= 3,	/*                       */
	IOC_E_DETACH		= 4,	/*                         */
	IOC_E_ENABLED		= 5,	/*                */
	IOC_E_FWRSP_GETATTR	= 6,	/*                              */
	IOC_E_DISABLED		= 7,	/*                 */
	IOC_E_PFFAILED		= 8,	/*                              */
	IOC_E_HBFAIL		= 9,	/*                      */
	IOC_E_HWERROR		= 10,	/*                            */
	IOC_E_TIMEOUT		= 11,	/*             */
	IOC_E_HWFAILED		= 12,	/*                              */
};

bfa_fsm_state_decl(bfa_ioc, uninit, struct bfa_ioc, enum ioc_event);
bfa_fsm_state_decl(bfa_ioc, reset, struct bfa_ioc, enum ioc_event);
bfa_fsm_state_decl(bfa_ioc, enabling, struct bfa_ioc, enum ioc_event);
bfa_fsm_state_decl(bfa_ioc, getattr, struct bfa_ioc, enum ioc_event);
bfa_fsm_state_decl(bfa_ioc, op, struct bfa_ioc, enum ioc_event);
bfa_fsm_state_decl(bfa_ioc, fail_retry, struct bfa_ioc, enum ioc_event);
bfa_fsm_state_decl(bfa_ioc, fail, struct bfa_ioc, enum ioc_event);
bfa_fsm_state_decl(bfa_ioc, disabling, struct bfa_ioc, enum ioc_event);
bfa_fsm_state_decl(bfa_ioc, disabled, struct bfa_ioc, enum ioc_event);
bfa_fsm_state_decl(bfa_ioc, hwfail, struct bfa_ioc, enum ioc_event);

static struct bfa_sm_table ioc_sm_table[] = {
	{BFA_SM(bfa_ioc_sm_uninit), BFA_IOC_UNINIT},
	{BFA_SM(bfa_ioc_sm_reset), BFA_IOC_RESET},
	{BFA_SM(bfa_ioc_sm_enabling), BFA_IOC_ENABLING},
	{BFA_SM(bfa_ioc_sm_getattr), BFA_IOC_GETATTR},
	{BFA_SM(bfa_ioc_sm_op), BFA_IOC_OPERATIONAL},
	{BFA_SM(bfa_ioc_sm_fail_retry), BFA_IOC_INITFAIL},
	{BFA_SM(bfa_ioc_sm_fail), BFA_IOC_FAIL},
	{BFA_SM(bfa_ioc_sm_disabling), BFA_IOC_DISABLING},
	{BFA_SM(bfa_ioc_sm_disabled), BFA_IOC_DISABLED},
	{BFA_SM(bfa_ioc_sm_hwfail), BFA_IOC_HWFAIL},
};

/*
                                                
 */
static void bfa_iocpf_enable(struct bfa_ioc *ioc);
static void bfa_iocpf_disable(struct bfa_ioc *ioc);
static void bfa_iocpf_fail(struct bfa_ioc *ioc);
static void bfa_iocpf_initfail(struct bfa_ioc *ioc);
static void bfa_iocpf_getattrfail(struct bfa_ioc *ioc);
static void bfa_iocpf_stop(struct bfa_ioc *ioc);

/*                            */
enum iocpf_event {
	IOCPF_E_ENABLE		= 1,	/*                        */
	IOCPF_E_DISABLE		= 2,	/*                         */
	IOCPF_E_STOP		= 3,	/*                         */
	IOCPF_E_FWREADY		= 4,	/*                           */
	IOCPF_E_FWRSP_ENABLE	= 5,	/*                       */
	IOCPF_E_FWRSP_DISABLE	= 6,	/*                        */
	IOCPF_E_FAIL		= 7,	/*                            */
	IOCPF_E_INITFAIL	= 8,	/*                              */
	IOCPF_E_GETATTRFAIL	= 9,	/*                              */
	IOCPF_E_SEMLOCKED	= 10,   /*                           */
	IOCPF_E_TIMEOUT		= 11,   /*                        */
	IOCPF_E_SEM_ERROR	= 12,   /*                         */
};

/*              */
enum bfa_iocpf_state {
	BFA_IOCPF_RESET		= 1,	/*                         */
	BFA_IOCPF_SEMWAIT	= 2,	/*                                 */
	BFA_IOCPF_HWINIT	= 3,	/*                                */
	BFA_IOCPF_READY		= 4,	/*                        */
	BFA_IOCPF_INITFAIL	= 5,	/*                */
	BFA_IOCPF_FAIL		= 6,	/*                */
	BFA_IOCPF_DISABLING	= 7,	/*                           */
	BFA_IOCPF_DISABLED	= 8,	/*                     */
	BFA_IOCPF_FWMISMATCH	= 9,	/*                                  */
};

bfa_fsm_state_decl(bfa_iocpf, reset, struct bfa_iocpf, enum iocpf_event);
bfa_fsm_state_decl(bfa_iocpf, fwcheck, struct bfa_iocpf, enum iocpf_event);
bfa_fsm_state_decl(bfa_iocpf, mismatch, struct bfa_iocpf, enum iocpf_event);
bfa_fsm_state_decl(bfa_iocpf, semwait, struct bfa_iocpf, enum iocpf_event);
bfa_fsm_state_decl(bfa_iocpf, hwinit, struct bfa_iocpf, enum iocpf_event);
bfa_fsm_state_decl(bfa_iocpf, enabling, struct bfa_iocpf, enum iocpf_event);
bfa_fsm_state_decl(bfa_iocpf, ready, struct bfa_iocpf, enum iocpf_event);
bfa_fsm_state_decl(bfa_iocpf, initfail_sync, struct bfa_iocpf,
						enum iocpf_event);
bfa_fsm_state_decl(bfa_iocpf, initfail, struct bfa_iocpf, enum iocpf_event);
bfa_fsm_state_decl(bfa_iocpf, fail_sync, struct bfa_iocpf, enum iocpf_event);
bfa_fsm_state_decl(bfa_iocpf, fail, struct bfa_iocpf, enum iocpf_event);
bfa_fsm_state_decl(bfa_iocpf, disabling, struct bfa_iocpf, enum iocpf_event);
bfa_fsm_state_decl(bfa_iocpf, disabling_sync, struct bfa_iocpf,
						enum iocpf_event);
bfa_fsm_state_decl(bfa_iocpf, disabled, struct bfa_iocpf, enum iocpf_event);

static struct bfa_sm_table iocpf_sm_table[] = {
	{BFA_SM(bfa_iocpf_sm_reset), BFA_IOCPF_RESET},
	{BFA_SM(bfa_iocpf_sm_fwcheck), BFA_IOCPF_FWMISMATCH},
	{BFA_SM(bfa_iocpf_sm_mismatch), BFA_IOCPF_FWMISMATCH},
	{BFA_SM(bfa_iocpf_sm_semwait), BFA_IOCPF_SEMWAIT},
	{BFA_SM(bfa_iocpf_sm_hwinit), BFA_IOCPF_HWINIT},
	{BFA_SM(bfa_iocpf_sm_enabling), BFA_IOCPF_HWINIT},
	{BFA_SM(bfa_iocpf_sm_ready), BFA_IOCPF_READY},
	{BFA_SM(bfa_iocpf_sm_initfail_sync), BFA_IOCPF_INITFAIL},
	{BFA_SM(bfa_iocpf_sm_initfail), BFA_IOCPF_INITFAIL},
	{BFA_SM(bfa_iocpf_sm_fail_sync), BFA_IOCPF_FAIL},
	{BFA_SM(bfa_iocpf_sm_fail), BFA_IOCPF_FAIL},
	{BFA_SM(bfa_iocpf_sm_disabling), BFA_IOCPF_DISABLING},
	{BFA_SM(bfa_iocpf_sm_disabling_sync), BFA_IOCPF_DISABLING},
	{BFA_SM(bfa_iocpf_sm_disabled), BFA_IOCPF_DISABLED},
};

/*                   */

/*                                    */
static void
bfa_ioc_sm_uninit_entry(struct bfa_ioc *ioc)
{
}

/*                         */
static void
bfa_ioc_sm_uninit(struct bfa_ioc *ioc, enum ioc_event event)
{
	switch (event) {
	case IOC_E_RESET:
		bfa_fsm_set_state(ioc, bfa_ioc_sm_reset);
		break;

	default:
		bfa_sm_fault(event);
	}
}

/*                                                 */
static void
bfa_ioc_sm_reset_entry(struct bfa_ioc *ioc)
{
	bfa_fsm_set_state(&ioc->iocpf, bfa_iocpf_sm_reset);
}

/*                        */
static void
bfa_ioc_sm_reset(struct bfa_ioc *ioc, enum ioc_event event)
{
	switch (event) {
	case IOC_E_ENABLE:
		bfa_fsm_set_state(ioc, bfa_ioc_sm_enabling);
		break;

	case IOC_E_DISABLE:
		bfa_ioc_disable_comp(ioc);
		break;

	case IOC_E_DETACH:
		bfa_fsm_set_state(ioc, bfa_ioc_sm_uninit);
		break;

	default:
		bfa_sm_fault(event);
	}
}

static void
bfa_ioc_sm_enabling_entry(struct bfa_ioc *ioc)
{
	bfa_iocpf_enable(ioc);
}

/*                                                                     
                         
 */
static void
bfa_ioc_sm_enabling(struct bfa_ioc *ioc, enum ioc_event event)
{
	switch (event) {
	case IOC_E_ENABLED:
		bfa_fsm_set_state(ioc, bfa_ioc_sm_getattr);
		break;

	case IOC_E_PFFAILED:
		/*                      */
	case IOC_E_HWERROR:
		ioc->cbfn->enable_cbfn(ioc->bfa, BFA_STATUS_IOC_FAILURE);
		bfa_fsm_set_state(ioc, bfa_ioc_sm_fail);
		if (event != IOC_E_PFFAILED)
			bfa_iocpf_initfail(ioc);
		break;

	case IOC_E_HWFAILED:
		ioc->cbfn->enable_cbfn(ioc->bfa, BFA_STATUS_IOC_FAILURE);
		bfa_fsm_set_state(ioc, bfa_ioc_sm_hwfail);
		break;

	case IOC_E_DISABLE:
		bfa_fsm_set_state(ioc, bfa_ioc_sm_disabling);
		break;

	case IOC_E_DETACH:
		bfa_fsm_set_state(ioc, bfa_ioc_sm_uninit);
		bfa_iocpf_stop(ioc);
		break;

	case IOC_E_ENABLE:
		break;

	default:
		bfa_sm_fault(event);
	}
}

/*                                                 */
static void
bfa_ioc_sm_getattr_entry(struct bfa_ioc *ioc)
{
	mod_timer(&ioc->ioc_timer, jiffies +
		msecs_to_jiffies(BFA_IOC_TOV));
	bfa_ioc_send_getattr(ioc);
}

/*                                                 */
static void
bfa_ioc_sm_getattr(struct bfa_ioc *ioc, enum ioc_event event)
{
	switch (event) {
	case IOC_E_FWRSP_GETATTR:
		del_timer(&ioc->ioc_timer);
		bfa_fsm_set_state(ioc, bfa_ioc_sm_op);
		break;

	case IOC_E_PFFAILED:
	case IOC_E_HWERROR:
		del_timer(&ioc->ioc_timer);
		/*              */
	case IOC_E_TIMEOUT:
		ioc->cbfn->enable_cbfn(ioc->bfa, BFA_STATUS_IOC_FAILURE);
		bfa_fsm_set_state(ioc, bfa_ioc_sm_fail);
		if (event != IOC_E_PFFAILED)
			bfa_iocpf_getattrfail(ioc);
		break;

	case IOC_E_DISABLE:
		del_timer(&ioc->ioc_timer);
		bfa_fsm_set_state(ioc, bfa_ioc_sm_disabling);
		break;

	case IOC_E_ENABLE:
		break;

	default:
		bfa_sm_fault(event);
	}
}

static void
bfa_ioc_sm_op_entry(struct bfa_ioc *ioc)
{
	ioc->cbfn->enable_cbfn(ioc->bfa, BFA_STATUS_OK);
	bfa_ioc_event_notify(ioc, BFA_IOC_E_ENABLED);
	bfa_ioc_hb_monitor(ioc);
}

static void
bfa_ioc_sm_op(struct bfa_ioc *ioc, enum ioc_event event)
{
	switch (event) {
	case IOC_E_ENABLE:
		break;

	case IOC_E_DISABLE:
		bfa_ioc_hb_stop(ioc);
		bfa_fsm_set_state(ioc, bfa_ioc_sm_disabling);
		break;

	case IOC_E_PFFAILED:
	case IOC_E_HWERROR:
		bfa_ioc_hb_stop(ioc);
		/*                      */
	case IOC_E_HBFAIL:
		if (ioc->iocpf.auto_recover)
			bfa_fsm_set_state(ioc, bfa_ioc_sm_fail_retry);
		else
			bfa_fsm_set_state(ioc, bfa_ioc_sm_fail);

		bfa_ioc_fail_notify(ioc);

		if (event != IOC_E_PFFAILED)
			bfa_iocpf_fail(ioc);
		break;

	default:
		bfa_sm_fault(event);
	}
}

static void
bfa_ioc_sm_disabling_entry(struct bfa_ioc *ioc)
{
	bfa_iocpf_disable(ioc);
}

/*                       */
static void
bfa_ioc_sm_disabling(struct bfa_ioc *ioc, enum ioc_event event)
{
	switch (event) {
	case IOC_E_DISABLED:
		bfa_fsm_set_state(ioc, bfa_ioc_sm_disabled);
		break;

	case IOC_E_HWERROR:
		/*
                                                  
                                                    
                             
   */
		bfa_iocpf_fail(ioc);
		break;

	case IOC_E_HWFAILED:
		bfa_fsm_set_state(ioc, bfa_ioc_sm_hwfail);
		bfa_ioc_disable_comp(ioc);
		break;

	default:
		bfa_sm_fault(event);
	}
}

/*                               */
static void
bfa_ioc_sm_disabled_entry(struct bfa_ioc *ioc)
{
	bfa_ioc_disable_comp(ioc);
}

static void
bfa_ioc_sm_disabled(struct bfa_ioc *ioc, enum ioc_event event)
{
	switch (event) {
	case IOC_E_ENABLE:
		bfa_fsm_set_state(ioc, bfa_ioc_sm_enabling);
		break;

	case IOC_E_DISABLE:
		ioc->cbfn->disable_cbfn(ioc->bfa);
		break;

	case IOC_E_DETACH:
		bfa_fsm_set_state(ioc, bfa_ioc_sm_uninit);
		bfa_iocpf_stop(ioc);
		break;

	default:
		bfa_sm_fault(event);
	}
}

static void
bfa_ioc_sm_fail_retry_entry(struct bfa_ioc *ioc)
{
}

/*                                */
static void
bfa_ioc_sm_fail_retry(struct bfa_ioc *ioc, enum ioc_event event)
{
	switch (event) {
	case IOC_E_ENABLED:
		bfa_fsm_set_state(ioc, bfa_ioc_sm_getattr);
		break;

	case IOC_E_PFFAILED:
	case IOC_E_HWERROR:
		/* 
                                 
   */
		ioc->cbfn->enable_cbfn(ioc->bfa, BFA_STATUS_IOC_FAILURE);
		bfa_fsm_set_state(ioc, bfa_ioc_sm_fail);
		if (event != IOC_E_PFFAILED)
			bfa_iocpf_initfail(ioc);
		break;

	case IOC_E_HWFAILED:
		ioc->cbfn->enable_cbfn(ioc->bfa, BFA_STATUS_IOC_FAILURE);
		bfa_fsm_set_state(ioc, bfa_ioc_sm_hwfail);
		break;

	case IOC_E_ENABLE:
		break;

	case IOC_E_DISABLE:
		bfa_fsm_set_state(ioc, bfa_ioc_sm_disabling);
		break;

	case IOC_E_DETACH:
		bfa_fsm_set_state(ioc, bfa_ioc_sm_uninit);
		bfa_iocpf_stop(ioc);
		break;

	default:
		bfa_sm_fault(event);
	}
}

static void
bfa_ioc_sm_fail_entry(struct bfa_ioc *ioc)
{
}

/*              */
static void
bfa_ioc_sm_fail(struct bfa_ioc *ioc, enum ioc_event event)
{
	switch (event) {
	case IOC_E_ENABLE:
		ioc->cbfn->enable_cbfn(ioc->bfa, BFA_STATUS_IOC_FAILURE);
		break;

	case IOC_E_DISABLE:
		bfa_fsm_set_state(ioc, bfa_ioc_sm_disabling);
		break;

	case IOC_E_DETACH:
		bfa_fsm_set_state(ioc, bfa_ioc_sm_uninit);
		bfa_iocpf_stop(ioc);
		break;

	case IOC_E_HWERROR:
		/*                                  */
		break;

	default:
		bfa_sm_fault(event);
	}
}

static void
bfa_ioc_sm_hwfail_entry(struct bfa_ioc *ioc)
{
}

/*              */
static void
bfa_ioc_sm_hwfail(struct bfa_ioc *ioc, enum ioc_event event)
{
	switch (event) {

	case IOC_E_ENABLE:
		ioc->cbfn->enable_cbfn(ioc->bfa, BFA_STATUS_IOC_FAILURE);
		break;

	case IOC_E_DISABLE:
		ioc->cbfn->disable_cbfn(ioc->bfa);
		break;

	case IOC_E_DETACH:
		bfa_fsm_set_state(ioc, bfa_ioc_sm_uninit);
		break;

	default:
		bfa_sm_fault(event);
	}
}

/*                     */

/*                                                 */
static void
bfa_iocpf_sm_reset_entry(struct bfa_iocpf *iocpf)
{
	iocpf->fw_mismatch_notified = false;
	iocpf->auto_recover = bfa_nw_auto_recover;
}

/*                                         */
static void
bfa_iocpf_sm_reset(struct bfa_iocpf *iocpf, enum iocpf_event event)
{
	switch (event) {
	case IOCPF_E_ENABLE:
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_fwcheck);
		break;

	case IOCPF_E_STOP:
		break;

	default:
		bfa_sm_fault(event);
	}
}

/*                                                 */
static void
bfa_iocpf_sm_fwcheck_entry(struct bfa_iocpf *iocpf)
{
	bfa_ioc_hw_sem_init(iocpf->ioc);
	bfa_ioc_hw_sem_get(iocpf->ioc);
}

/*                                                        */
static void
bfa_iocpf_sm_fwcheck(struct bfa_iocpf *iocpf, enum iocpf_event event)
{
	struct bfa_ioc *ioc = iocpf->ioc;

	switch (event) {
	case IOCPF_E_SEMLOCKED:
		if (bfa_ioc_firmware_lock(ioc)) {
			if (bfa_ioc_sync_start(ioc)) {
				bfa_ioc_sync_join(ioc);
				bfa_fsm_set_state(iocpf, bfa_iocpf_sm_hwinit);
			} else {
				bfa_ioc_firmware_unlock(ioc);
				bfa_nw_ioc_hw_sem_release(ioc);
				mod_timer(&ioc->sem_timer, jiffies +
					msecs_to_jiffies(BFA_IOC_HWSEM_TOV));
			}
		} else {
			bfa_nw_ioc_hw_sem_release(ioc);
			bfa_fsm_set_state(iocpf, bfa_iocpf_sm_mismatch);
		}
		break;

	case IOCPF_E_SEM_ERROR:
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_fail);
		bfa_ioc_pf_hwfailed(ioc);
		break;

	case IOCPF_E_DISABLE:
		bfa_ioc_hw_sem_get_cancel(ioc);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_reset);
		bfa_ioc_pf_disabled(ioc);
		break;

	case IOCPF_E_STOP:
		bfa_ioc_hw_sem_get_cancel(ioc);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_reset);
		break;

	default:
		bfa_sm_fault(event);
	}
}

/*                                   */
static void
bfa_iocpf_sm_mismatch_entry(struct bfa_iocpf *iocpf)
{
	/*                                                      */
	if (!iocpf->fw_mismatch_notified)
		bfa_ioc_pf_fwmismatch(iocpf->ioc);

	iocpf->fw_mismatch_notified = true;
	mod_timer(&(iocpf->ioc)->iocpf_timer, jiffies +
		msecs_to_jiffies(BFA_IOC_TOV));
}

/*                                  */
static void
bfa_iocpf_sm_mismatch(struct bfa_iocpf *iocpf, enum iocpf_event event)
{
	struct bfa_ioc *ioc = iocpf->ioc;

	switch (event) {
	case IOCPF_E_TIMEOUT:
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_fwcheck);
		break;

	case IOCPF_E_DISABLE:
		del_timer(&ioc->iocpf_timer);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_reset);
		bfa_ioc_pf_disabled(ioc);
		break;

	case IOCPF_E_STOP:
		del_timer(&ioc->iocpf_timer);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_reset);
		break;

	default:
		bfa_sm_fault(event);
	}
}

/*                        */
static void
bfa_iocpf_sm_semwait_entry(struct bfa_iocpf *iocpf)
{
	bfa_ioc_hw_sem_get(iocpf->ioc);
}

/*                                           */
static void
bfa_iocpf_sm_semwait(struct bfa_iocpf *iocpf, enum iocpf_event event)
{
	struct bfa_ioc *ioc = iocpf->ioc;

	switch (event) {
	case IOCPF_E_SEMLOCKED:
		if (bfa_ioc_sync_complete(ioc)) {
			bfa_ioc_sync_join(ioc);
			bfa_fsm_set_state(iocpf, bfa_iocpf_sm_hwinit);
		} else {
			bfa_nw_ioc_hw_sem_release(ioc);
			mod_timer(&ioc->sem_timer, jiffies +
				msecs_to_jiffies(BFA_IOC_HWSEM_TOV));
		}
		break;

	case IOCPF_E_SEM_ERROR:
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_fail);
		bfa_ioc_pf_hwfailed(ioc);
		break;

	case IOCPF_E_DISABLE:
		bfa_ioc_hw_sem_get_cancel(ioc);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_disabling_sync);
		break;

	default:
		bfa_sm_fault(event);
	}
}

static void
bfa_iocpf_sm_hwinit_entry(struct bfa_iocpf *iocpf)
{
	iocpf->poll_time = 0;
	bfa_ioc_reset(iocpf->ioc, false);
}

/*                                                       
                                   
 */
static void
bfa_iocpf_sm_hwinit(struct bfa_iocpf *iocpf, enum iocpf_event event)
{
	struct bfa_ioc *ioc = iocpf->ioc;

	switch (event) {
	case IOCPF_E_FWREADY:
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_enabling);
		break;

	case IOCPF_E_TIMEOUT:
		bfa_nw_ioc_hw_sem_release(ioc);
			bfa_ioc_pf_failed(ioc);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_initfail_sync);
		break;

	case IOCPF_E_DISABLE:
		del_timer(&ioc->iocpf_timer);
		bfa_ioc_sync_leave(ioc);
		bfa_nw_ioc_hw_sem_release(ioc);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_disabled);
		break;

	default:
		bfa_sm_fault(event);
	}
}

static void
bfa_iocpf_sm_enabling_entry(struct bfa_iocpf *iocpf)
{
	mod_timer(&(iocpf->ioc)->iocpf_timer, jiffies +
		msecs_to_jiffies(BFA_IOC_TOV));
	/* 
                                                       
  */
	iocpf->ioc->cbfn->reset_cbfn(iocpf->ioc->bfa);
	bfa_ioc_send_enable(iocpf->ioc);
}

/*                                                                     
                         
 */
static void
bfa_iocpf_sm_enabling(struct bfa_iocpf *iocpf, enum iocpf_event event)
{
	struct bfa_ioc *ioc = iocpf->ioc;

	switch (event) {
	case IOCPF_E_FWRSP_ENABLE:
		del_timer(&ioc->iocpf_timer);
		bfa_nw_ioc_hw_sem_release(ioc);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_ready);
		break;

	case IOCPF_E_INITFAIL:
		del_timer(&ioc->iocpf_timer);
		/*
                         
   */
	case IOCPF_E_TIMEOUT:
		bfa_nw_ioc_hw_sem_release(ioc);
		if (event == IOCPF_E_TIMEOUT)
			bfa_ioc_pf_failed(ioc);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_initfail_sync);
		break;

	case IOCPF_E_DISABLE:
		del_timer(&ioc->iocpf_timer);
		bfa_nw_ioc_hw_sem_release(ioc);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_disabling);
		break;

	default:
		bfa_sm_fault(event);
	}
}

static void
bfa_iocpf_sm_ready_entry(struct bfa_iocpf *iocpf)
{
	bfa_ioc_pf_enabled(iocpf->ioc);
}

static void
bfa_iocpf_sm_ready(struct bfa_iocpf *iocpf, enum iocpf_event event)
{
	switch (event) {
	case IOCPF_E_DISABLE:
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_disabling);
		break;

	case IOCPF_E_GETATTRFAIL:
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_initfail_sync);
		break;

	case IOCPF_E_FAIL:
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_fail_sync);
		break;

	default:
		bfa_sm_fault(event);
	}
}

static void
bfa_iocpf_sm_disabling_entry(struct bfa_iocpf *iocpf)
{
	mod_timer(&(iocpf->ioc)->iocpf_timer, jiffies +
		msecs_to_jiffies(BFA_IOC_TOV));
	bfa_ioc_send_disable(iocpf->ioc);
}

/*                       */
static void
bfa_iocpf_sm_disabling(struct bfa_iocpf *iocpf, enum iocpf_event event)
{
	struct bfa_ioc *ioc = iocpf->ioc;

	switch (event) {
	case IOCPF_E_FWRSP_DISABLE:
		del_timer(&ioc->iocpf_timer);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_disabling_sync);
		break;

	case IOCPF_E_FAIL:
		del_timer(&ioc->iocpf_timer);
		/*
                         
   */

	case IOCPF_E_TIMEOUT:
		writel(BFI_IOC_FAIL, ioc->ioc_regs.ioc_fwstate);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_disabling_sync);
		break;

	case IOCPF_E_FWRSP_ENABLE:
		break;

	default:
		bfa_sm_fault(event);
	}
}

static void
bfa_iocpf_sm_disabling_sync_entry(struct bfa_iocpf *iocpf)
{
	bfa_ioc_hw_sem_get(iocpf->ioc);
}

/*                                      */
static void
bfa_iocpf_sm_disabling_sync(struct bfa_iocpf *iocpf, enum iocpf_event event)
{
	struct bfa_ioc *ioc = iocpf->ioc;

	switch (event) {
	case IOCPF_E_SEMLOCKED:
		bfa_ioc_sync_leave(ioc);
		bfa_nw_ioc_hw_sem_release(ioc);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_disabled);
		break;

	case IOCPF_E_SEM_ERROR:
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_fail);
		bfa_ioc_pf_hwfailed(ioc);
		break;

	case IOCPF_E_FAIL:
		break;

	default:
		bfa_sm_fault(event);
	}
}

/*                               */
static void
bfa_iocpf_sm_disabled_entry(struct bfa_iocpf *iocpf)
{
	bfa_ioc_mbox_flush(iocpf->ioc);
	bfa_ioc_pf_disabled(iocpf->ioc);
}

static void
bfa_iocpf_sm_disabled(struct bfa_iocpf *iocpf, enum iocpf_event event)
{
	struct bfa_ioc *ioc = iocpf->ioc;

	switch (event) {
	case IOCPF_E_ENABLE:
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_semwait);
		break;

	case IOCPF_E_STOP:
		bfa_ioc_firmware_unlock(ioc);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_reset);
		break;

	default:
		bfa_sm_fault(event);
	}
}

static void
bfa_iocpf_sm_initfail_sync_entry(struct bfa_iocpf *iocpf)
{
	bfa_nw_ioc_debug_save_ftrc(iocpf->ioc);
	bfa_ioc_hw_sem_get(iocpf->ioc);
}

/*                                 */
static void
bfa_iocpf_sm_initfail_sync(struct bfa_iocpf *iocpf, enum iocpf_event event)
{
	struct bfa_ioc *ioc = iocpf->ioc;

	switch (event) {
	case IOCPF_E_SEMLOCKED:
		bfa_ioc_notify_fail(ioc);
		bfa_ioc_sync_leave(ioc);
		writel(BFI_IOC_FAIL, ioc->ioc_regs.ioc_fwstate);
		bfa_nw_ioc_hw_sem_release(ioc);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_initfail);
		break;

	case IOCPF_E_SEM_ERROR:
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_fail);
		bfa_ioc_pf_hwfailed(ioc);
		break;

	case IOCPF_E_DISABLE:
		bfa_ioc_hw_sem_get_cancel(ioc);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_disabling_sync);
		break;

	case IOCPF_E_STOP:
		bfa_ioc_hw_sem_get_cancel(ioc);
		bfa_ioc_firmware_unlock(ioc);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_reset);
		break;

	case IOCPF_E_FAIL:
		break;

	default:
		bfa_sm_fault(event);
	}
}

static void
bfa_iocpf_sm_initfail_entry(struct bfa_iocpf *iocpf)
{
}

/*                                 */
static void
bfa_iocpf_sm_initfail(struct bfa_iocpf *iocpf, enum iocpf_event event)
{
	struct bfa_ioc *ioc = iocpf->ioc;

	switch (event) {
	case IOCPF_E_DISABLE:
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_disabled);
		break;

	case IOCPF_E_STOP:
		bfa_ioc_firmware_unlock(ioc);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_reset);
		break;

	default:
		bfa_sm_fault(event);
	}
}

static void
bfa_iocpf_sm_fail_sync_entry(struct bfa_iocpf *iocpf)
{
	/* 
                                                     
  */
	bfa_ioc_lpu_stop(iocpf->ioc);

	/* 
                                         
  */
	bfa_ioc_mbox_flush(iocpf->ioc);
	bfa_ioc_hw_sem_get(iocpf->ioc);
}

/*                         */
static void
bfa_iocpf_sm_fail_sync(struct bfa_iocpf *iocpf, enum iocpf_event event)
{
	struct bfa_ioc *ioc = iocpf->ioc;

	switch (event) {
	case IOCPF_E_SEMLOCKED:
		bfa_ioc_sync_ack(ioc);
		bfa_ioc_notify_fail(ioc);
		if (!iocpf->auto_recover) {
			bfa_ioc_sync_leave(ioc);
			writel(BFI_IOC_FAIL, ioc->ioc_regs.ioc_fwstate);
			bfa_nw_ioc_hw_sem_release(ioc);
			bfa_fsm_set_state(iocpf, bfa_iocpf_sm_fail);
		} else {
			if (bfa_ioc_sync_complete(ioc))
				bfa_fsm_set_state(iocpf, bfa_iocpf_sm_hwinit);
			else {
				bfa_nw_ioc_hw_sem_release(ioc);
				bfa_fsm_set_state(iocpf, bfa_iocpf_sm_semwait);
			}
		}
		break;

	case IOCPF_E_SEM_ERROR:
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_fail);
		bfa_ioc_pf_hwfailed(ioc);
		break;

	case IOCPF_E_DISABLE:
		bfa_ioc_hw_sem_get_cancel(ioc);
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_disabling_sync);
		break;

	case IOCPF_E_FAIL:
		break;

	default:
		bfa_sm_fault(event);
	}
}

static void
bfa_iocpf_sm_fail_entry(struct bfa_iocpf *iocpf)
{
}

/*                         */
static void
bfa_iocpf_sm_fail(struct bfa_iocpf *iocpf, enum iocpf_event event)
{
	switch (event) {
	case IOCPF_E_DISABLE:
		bfa_fsm_set_state(iocpf, bfa_iocpf_sm_disabled);
		break;

	default:
		bfa_sm_fault(event);
	}
}

/*                           */

/*                                                    */
static void
bfa_ioc_event_notify(struct bfa_ioc *ioc, enum bfa_ioc_event event)
{
	struct bfa_ioc_notify *notify;
	struct list_head			*qe;

	list_for_each(qe, &ioc->notify_q) {
		notify = (struct bfa_ioc_notify *)qe;
		notify->cbfn(notify->cbarg, event);
	}
}

static void
bfa_ioc_disable_comp(struct bfa_ioc *ioc)
{
	ioc->cbfn->disable_cbfn(ioc->bfa);
	bfa_ioc_event_notify(ioc, BFA_IOC_E_DISABLED);
}

bool
bfa_nw_ioc_sem_get(void __iomem *sem_reg)
{
	u32 r32;
	int cnt = 0;
#define BFA_SEM_SPINCNT	3000

	r32 = readl(sem_reg);

	while ((r32 & 1) && (cnt < BFA_SEM_SPINCNT)) {
		cnt++;
		udelay(2);
		r32 = readl(sem_reg);
	}

	if (!(r32 & 1))
		return true;

	return false;
}

void
bfa_nw_ioc_sem_release(void __iomem *sem_reg)
{
	readl(sem_reg);
	writel(1, sem_reg);
}

/*                 */
static void
bfa_ioc_fwver_clear(struct bfa_ioc *ioc)
{
	u32 pgnum, pgoff, loff = 0;
	int i;

	pgnum = PSS_SMEM_PGNUM(ioc->ioc_regs.smem_pg0, loff);
	pgoff = PSS_SMEM_PGOFF(loff);
	writel(pgnum, ioc->ioc_regs.host_page_num_fn);

	for (i = 0; i < (sizeof(struct bfi_ioc_image_hdr) / sizeof(u32)); i++) {
		writel(0, ioc->ioc_regs.smem_page_start + loff);
		loff += sizeof(u32);
	}
}


static void
bfa_ioc_hw_sem_init(struct bfa_ioc *ioc)
{
	struct bfi_ioc_image_hdr fwhdr;
	u32 fwstate, r32;

	/*                                      */
	r32 = readl(ioc->ioc_regs.ioc_init_sem_reg);
	while (r32 & 0x1) {
		udelay(20);
		r32 = readl(ioc->ioc_regs.ioc_init_sem_reg);
	}

	fwstate = readl(ioc->ioc_regs.ioc_fwstate);
	if (fwstate == BFI_IOC_UNINIT) {
		writel(1, ioc->ioc_regs.ioc_init_sem_reg);
		return;
	}

	bfa_nw_ioc_fwver_get(ioc, &fwhdr);

	if (swab32(fwhdr.exec) == BFI_FWBOOT_TYPE_NORMAL) {
		writel(1, ioc->ioc_regs.ioc_init_sem_reg);
		return;
	}

	bfa_ioc_fwver_clear(ioc);
	writel(BFI_IOC_UNINIT, ioc->ioc_regs.ioc_fwstate);
	writel(BFI_IOC_UNINIT, ioc->ioc_regs.alt_ioc_fwstate);

	/*
                                              
  */
	readl(ioc->ioc_regs.ioc_sem_reg);
	writel(1, ioc->ioc_regs.ioc_sem_reg);

	/*                       */
	writel(1, ioc->ioc_regs.ioc_init_sem_reg);
}

static void
bfa_ioc_hw_sem_get(struct bfa_ioc *ioc)
{
	u32	r32;

	/* 
                                                                        
                                                                     
  */
	r32 = readl(ioc->ioc_regs.ioc_sem_reg);
	if (r32 == ~0) {
		bfa_fsm_send_event(&ioc->iocpf, IOCPF_E_SEM_ERROR);
		return;
	}
	if (!(r32 & 1)) {
		bfa_fsm_send_event(&ioc->iocpf, IOCPF_E_SEMLOCKED);
		return;
	}

	mod_timer(&ioc->sem_timer, jiffies +
		msecs_to_jiffies(BFA_IOC_HWSEM_TOV));
}

void
bfa_nw_ioc_hw_sem_release(struct bfa_ioc *ioc)
{
	writel(1, ioc->ioc_regs.ioc_sem_reg);
}

static void
bfa_ioc_hw_sem_get_cancel(struct bfa_ioc *ioc)
{
	del_timer(&ioc->sem_timer);
}

/*                                                           */
static void
bfa_ioc_lmem_init(struct bfa_ioc *ioc)
{
	u32	pss_ctl;
	int		i;
#define PSS_LMEM_INIT_TIME  10000

	pss_ctl = readl(ioc->ioc_regs.pss_ctl_reg);
	pss_ctl &= ~__PSS_LMEM_RESET;
	pss_ctl |= __PSS_LMEM_INIT_EN;

	/*
                                
  */
	pss_ctl |= __PSS_I2C_CLK_DIV(3UL);
	writel(pss_ctl, ioc->ioc_regs.pss_ctl_reg);

	/* 
                                                 
  */
	i = 0;
	do {
		pss_ctl = readl(ioc->ioc_regs.pss_ctl_reg);
		i++;
	} while (!(pss_ctl & __PSS_LMEM_INIT_DONE) && (i < PSS_LMEM_INIT_TIME));

	/* 
                                                                      
                  
  */
	BUG_ON(!(pss_ctl & __PSS_LMEM_INIT_DONE));

	pss_ctl &= ~(__PSS_LMEM_INIT_DONE | __PSS_LMEM_INIT_EN);
	writel(pss_ctl, ioc->ioc_regs.pss_ctl_reg);
}

static void
bfa_ioc_lpu_start(struct bfa_ioc *ioc)
{
	u32	pss_ctl;

	/* 
                                
  */
	pss_ctl = readl(ioc->ioc_regs.pss_ctl_reg);
	pss_ctl &= ~__PSS_LPU0_RESET;

	writel(pss_ctl, ioc->ioc_regs.pss_ctl_reg);
}

static void
bfa_ioc_lpu_stop(struct bfa_ioc *ioc)
{
	u32	pss_ctl;

	/* 
                            
  */
	pss_ctl = readl(ioc->ioc_regs.pss_ctl_reg);
	pss_ctl |= (__PSS_LPU0_RESET | __PSS_LPU1_RESET);

	writel(pss_ctl, ioc->ioc_regs.pss_ctl_reg);
}

/*                                   */
void
bfa_nw_ioc_fwver_get(struct bfa_ioc *ioc, struct bfi_ioc_image_hdr *fwhdr)
{
	u32	pgnum;
	u32	loff = 0;
	int		i;
	u32	*fwsig = (u32 *) fwhdr;

	pgnum = bfa_ioc_smem_pgnum(ioc, loff);
	writel(pgnum, ioc->ioc_regs.host_page_num_fn);

	for (i = 0; i < (sizeof(struct bfi_ioc_image_hdr) / sizeof(u32));
	     i++) {
		fwsig[i] =
			swab32(readl((loff) + (ioc->ioc_regs.smem_page_start)));
		loff += sizeof(u32);
	}
}

/*                       */
bool
bfa_nw_ioc_fwver_cmp(struct bfa_ioc *ioc, struct bfi_ioc_image_hdr *fwhdr)
{
	struct bfi_ioc_image_hdr *drv_fwhdr;
	int i;

	drv_fwhdr = (struct bfi_ioc_image_hdr *)
		bfa_cb_image_get_chunk(bfa_ioc_asic_gen(ioc), 0);

	for (i = 0; i < BFI_IOC_MD5SUM_SZ; i++) {
		if (fwhdr->md5sum[i] != drv_fwhdr->md5sum[i])
			return false;
	}

	return true;
}

/*                                                                        
                                              
 */
static bool
bfa_ioc_fwver_valid(struct bfa_ioc *ioc, u32 boot_env)
{
	struct bfi_ioc_image_hdr fwhdr, *drv_fwhdr;

	bfa_nw_ioc_fwver_get(ioc, &fwhdr);
	drv_fwhdr = (struct bfi_ioc_image_hdr *)
		bfa_cb_image_get_chunk(bfa_ioc_asic_gen(ioc), 0);

	if (fwhdr.signature != drv_fwhdr->signature)
		return false;

	if (swab32(fwhdr.bootenv) != boot_env)
		return false;

	return bfa_nw_ioc_fwver_cmp(ioc, &fwhdr);
}

/*                                                                 */
static void
bfa_ioc_msgflush(struct bfa_ioc *ioc)
{
	u32	r32;

	r32 = readl(ioc->ioc_regs.lpu_mbox_cmd);
	if (r32)
		writel(1, ioc->ioc_regs.lpu_mbox_cmd);
}

static void
bfa_ioc_hwinit(struct bfa_ioc *ioc, bool force)
{
	enum bfi_ioc_state ioc_fwstate;
	bool fwvalid;
	u32 boot_env;

	ioc_fwstate = readl(ioc->ioc_regs.ioc_fwstate);

	if (force)
		ioc_fwstate = BFI_IOC_UNINIT;

	boot_env = BFI_FWBOOT_ENV_OS;

	/* 
                              
  */
	fwvalid = (ioc_fwstate == BFI_IOC_UNINIT) ?
		false : bfa_ioc_fwver_valid(ioc, boot_env);

	if (!fwvalid) {
		bfa_ioc_boot(ioc, BFI_FWBOOT_TYPE_NORMAL, boot_env);
		bfa_ioc_poll_fwinit(ioc);
		return;
	}

	/* 
                                                                         
                                                         
  */
	if (ioc_fwstate == BFI_IOC_INITING) {
		bfa_ioc_poll_fwinit(ioc);
		return;
	}

	/* 
                                                             
                       
  */
	if (ioc_fwstate == BFI_IOC_DISABLED || ioc_fwstate == BFI_IOC_OP) {
		/* 
                                                             
                                                              
   */
		bfa_ioc_msgflush(ioc);
		bfa_fsm_send_event(&ioc->iocpf, IOCPF_E_FWREADY);
		return;
	}

	/* 
                                            
  */
	bfa_ioc_boot(ioc, BFI_FWBOOT_TYPE_NORMAL, boot_env);
	bfa_ioc_poll_fwinit(ioc);
}

void
bfa_nw_ioc_timeout(void *ioc_arg)
{
	struct bfa_ioc *ioc = (struct bfa_ioc *) ioc_arg;

	bfa_fsm_send_event(ioc, IOC_E_TIMEOUT);
}

static void
bfa_ioc_mbox_send(struct bfa_ioc *ioc, void *ioc_msg, int len)
{
	u32 *msgp = (u32 *) ioc_msg;
	u32 i;

	BUG_ON(!(len <= BFI_IOC_MSGLEN_MAX));

	/*
                                        
  */
	for (i = 0; i < len / sizeof(u32); i++)
		writel(cpu_to_le32(msgp[i]),
			      ioc->ioc_regs.hfn_mbox + i * sizeof(u32));

	for (; i < BFI_IOC_MSGLEN_MAX / sizeof(u32); i++)
		writel(0, ioc->ioc_regs.hfn_mbox + i * sizeof(u32));

	/*
                                               
  */
	writel(1, ioc->ioc_regs.hfn_mbox_cmd);
	(void) readl(ioc->ioc_regs.hfn_mbox_cmd);
}

static void
bfa_ioc_send_enable(struct bfa_ioc *ioc)
{
	struct bfi_ioc_ctrl_req enable_req;
	struct timeval tv;

	bfi_h2i_set(enable_req.mh, BFI_MC_IOC, BFI_IOC_H2I_ENABLE_REQ,
		    bfa_ioc_portid(ioc));
	enable_req.clscode = htons(ioc->clscode);
	do_gettimeofday(&tv);
	enable_req.tv_sec = ntohl(tv.tv_sec);
	bfa_ioc_mbox_send(ioc, &enable_req, sizeof(struct bfi_ioc_ctrl_req));
}

static void
bfa_ioc_send_disable(struct bfa_ioc *ioc)
{
	struct bfi_ioc_ctrl_req disable_req;

	bfi_h2i_set(disable_req.mh, BFI_MC_IOC, BFI_IOC_H2I_DISABLE_REQ,
		    bfa_ioc_portid(ioc));
	bfa_ioc_mbox_send(ioc, &disable_req, sizeof(struct bfi_ioc_ctrl_req));
}

static void
bfa_ioc_send_getattr(struct bfa_ioc *ioc)
{
	struct bfi_ioc_getattr_req attr_req;

	bfi_h2i_set(attr_req.mh, BFI_MC_IOC, BFI_IOC_H2I_GETATTR_REQ,
		    bfa_ioc_portid(ioc));
	bfa_dma_be_addr_set(attr_req.attr_addr, ioc->attr_dma.pa);
	bfa_ioc_mbox_send(ioc, &attr_req, sizeof(attr_req));
}

void
bfa_nw_ioc_hb_check(void *cbarg)
{
	struct bfa_ioc *ioc = cbarg;
	u32	hb_count;

	hb_count = readl(ioc->ioc_regs.heartbeat);
	if (ioc->hb_count == hb_count) {
		bfa_ioc_recover(ioc);
		return;
	} else {
		ioc->hb_count = hb_count;
	}

	bfa_ioc_mbox_poll(ioc);
	mod_timer(&ioc->hb_timer, jiffies +
		msecs_to_jiffies(BFA_IOC_HB_TOV));
}

static void
bfa_ioc_hb_monitor(struct bfa_ioc *ioc)
{
	ioc->hb_count = readl(ioc->ioc_regs.heartbeat);
	mod_timer(&ioc->hb_timer, jiffies +
		msecs_to_jiffies(BFA_IOC_HB_TOV));
}

static void
bfa_ioc_hb_stop(struct bfa_ioc *ioc)
{
	del_timer(&ioc->hb_timer);
}

/*                                    */
static void
bfa_ioc_download_fw(struct bfa_ioc *ioc, u32 boot_type,
		    u32 boot_env)
{
	u32 *fwimg;
	u32 pgnum;
	u32 loff = 0;
	u32 chunkno = 0;
	u32 i;
	u32 asicmode;

	fwimg = bfa_cb_image_get_chunk(bfa_ioc_asic_gen(ioc), chunkno);

	pgnum = bfa_ioc_smem_pgnum(ioc, loff);

	writel(pgnum, ioc->ioc_regs.host_page_num_fn);

	for (i = 0; i < bfa_cb_image_get_size(bfa_ioc_asic_gen(ioc)); i++) {
		if (BFA_IOC_FLASH_CHUNK_NO(i) != chunkno) {
			chunkno = BFA_IOC_FLASH_CHUNK_NO(i);
			fwimg = bfa_cb_image_get_chunk(bfa_ioc_asic_gen(ioc),
					BFA_IOC_FLASH_CHUNK_ADDR(chunkno));
		}

		/* 
               
   */
		writel((swab32(fwimg[BFA_IOC_FLASH_OFFSET_IN_CHUNK(i)])),
			      ((ioc->ioc_regs.smem_page_start) + (loff)));

		loff += sizeof(u32);

		/* 
                                   
   */
		loff = PSS_SMEM_PGOFF(loff);
		if (loff == 0) {
			pgnum++;
			writel(pgnum,
				      ioc->ioc_regs.host_page_num_fn);
		}
	}

	writel(bfa_ioc_smem_pgnum(ioc, 0),
		      ioc->ioc_regs.host_page_num_fn);

	/*
                                                  
 */
	asicmode = BFI_FWBOOT_DEVMODE(ioc->asic_gen, ioc->asic_mode,
					ioc->port0_mode, ioc->port1_mode);
	writel(asicmode, ((ioc->ioc_regs.smem_page_start)
			+ BFI_FWBOOT_DEVMODE_OFF));
	writel(boot_type, ((ioc->ioc_regs.smem_page_start)
			+ (BFI_FWBOOT_TYPE_OFF)));
	writel(boot_env, ((ioc->ioc_regs.smem_page_start)
			+ (BFI_FWBOOT_ENV_OFF)));
}

static void
bfa_ioc_reset(struct bfa_ioc *ioc, bool force)
{
	bfa_ioc_hwinit(ioc, force);
}

/*                                  */
static void
bfa_ioc_enable_reply(struct bfa_ioc *ioc, enum bfa_mode port_mode,
			u8 cap_bm)
{
	struct bfa_iocpf *iocpf = &ioc->iocpf;

	ioc->port_mode = ioc->port_mode_cfg = port_mode;
	ioc->ad_cap_bm = cap_bm;
	bfa_fsm_send_event(iocpf, IOCPF_E_FWRSP_ENABLE);
}

/*                                                       */
static void
bfa_ioc_getattr_reply(struct bfa_ioc *ioc)
{
	struct bfi_ioc_attr *attr = ioc->attr;

	attr->adapter_prop  = ntohl(attr->adapter_prop);
	attr->card_type     = ntohl(attr->card_type);
	attr->maxfrsize	    = ntohs(attr->maxfrsize);

	bfa_fsm_send_event(ioc, IOC_E_FWRSP_GETATTR);
}

/*                                           */
static void
bfa_ioc_mbox_attach(struct bfa_ioc *ioc)
{
	struct bfa_ioc_mbox_mod *mod = &ioc->mbox_mod;
	int	mc;

	INIT_LIST_HEAD(&mod->cmd_q);
	for (mc = 0; mc < BFI_MC_MAX; mc++) {
		mod->mbhdlr[mc].cbfn = NULL;
		mod->mbhdlr[mc].cbarg = ioc->bfa;
	}
}

/*                                                           */
static void
bfa_ioc_mbox_poll(struct bfa_ioc *ioc)
{
	struct bfa_ioc_mbox_mod *mod = &ioc->mbox_mod;
	struct bfa_mbox_cmd *cmd;
	bfa_mbox_cmd_cbfn_t cbfn;
	void *cbarg;
	u32 stat;

	/* 
                                     
  */
	if (list_empty(&mod->cmd_q))
		return;

	/* 
                                                                  
  */
	stat = readl(ioc->ioc_regs.hfn_mbox_cmd);
	if (stat)
		return;

	/* 
                                
  */
	bfa_q_deq(&mod->cmd_q, &cmd);
	bfa_ioc_mbox_send(ioc, cmd->msg, sizeof(cmd->msg));

	/* 
                                                                      
  */
	if (cmd->cbfn) {
		cbfn = cmd->cbfn;
		cbarg = cmd->cbarg;
		cmd->cbfn = NULL;
		cbfn(cbarg);
	}
}

/*                               */
static void
bfa_ioc_mbox_flush(struct bfa_ioc *ioc)
{
	struct bfa_ioc_mbox_mod *mod = &ioc->mbox_mod;
	struct bfa_mbox_cmd *cmd;

	while (!list_empty(&mod->cmd_q))
		bfa_q_deq(&mod->cmd_q, &cmd);
}

/* 
                                                                        
  
                           
                                               
                        
                                  
 */
static int
bfa_nw_ioc_smem_read(struct bfa_ioc *ioc, void *tbuf, u32 soff, u32 sz)
{
	u32 pgnum, loff, r32;
	int i, len;
	u32 *buf = tbuf;

	pgnum = PSS_SMEM_PGNUM(ioc->ioc_regs.smem_pg0, soff);
	loff = PSS_SMEM_PGOFF(soff);

	/*
                                                    
 */
	if (bfa_nw_ioc_sem_get(ioc->ioc_regs.ioc_init_sem_reg) == 0)
		return 1;

	writel(pgnum, ioc->ioc_regs.host_page_num_fn);

	len = sz/sizeof(u32);
	for (i = 0; i < len; i++) {
		r32 = swab32(readl((loff) + (ioc->ioc_regs.smem_page_start)));
		buf[i] = be32_to_cpu(r32);
		loff += sizeof(u32);

		/* 
                                   
   */
		loff = PSS_SMEM_PGOFF(loff);
		if (loff == 0) {
			pgnum++;
			writel(pgnum, ioc->ioc_regs.host_page_num_fn);
		}
	}

	writel(PSS_SMEM_PGNUM(ioc->ioc_regs.smem_pg0, 0),
	       ioc->ioc_regs.host_page_num_fn);

	/*
                     
  */
	readl(ioc->ioc_regs.ioc_init_sem_reg);
	writel(1, ioc->ioc_regs.ioc_init_sem_reg);
	return 0;
}

/*                                                         */
int
bfa_nw_ioc_debug_fwtrc(struct bfa_ioc *ioc, void *trcdata, int *trclen)
{
	u32 loff = BFI_IOC_TRC_OFF + BNA_DBG_FWTRC_LEN * ioc->port_id;
	int tlen, status = 0;

	tlen = *trclen;
	if (tlen > BNA_DBG_FWTRC_LEN)
		tlen = BNA_DBG_FWTRC_LEN;

	status = bfa_nw_ioc_smem_read(ioc, trcdata, loff, tlen);
	*trclen = tlen;
	return status;
}

/*                                    */
static void
bfa_nw_ioc_debug_save_ftrc(struct bfa_ioc *ioc)
{
	int tlen;

	if (ioc->dbg_fwsave_once) {
		ioc->dbg_fwsave_once = 0;
		if (ioc->dbg_fwsave_len) {
			tlen = ioc->dbg_fwsave_len;
			bfa_nw_ioc_debug_fwtrc(ioc, ioc->dbg_fwsave, &tlen);
		}
	}
}

/*                                                         */
int
bfa_nw_ioc_debug_fwsave(struct bfa_ioc *ioc, void *trcdata, int *trclen)
{
	int tlen;

	if (ioc->dbg_fwsave_len == 0)
		return BFA_STATUS_ENOFSAVE;

	tlen = *trclen;
	if (tlen > ioc->dbg_fwsave_len)
		tlen = ioc->dbg_fwsave_len;

	memcpy(trcdata, ioc->dbg_fwsave, tlen);
	*trclen = tlen;
	return BFA_STATUS_OK;
}

static void
bfa_ioc_fail_notify(struct bfa_ioc *ioc)
{
	/* 
                                                                 
  */
	ioc->cbfn->hbfail_cbfn(ioc->bfa);
	bfa_ioc_event_notify(ioc, BFA_IOC_E_FAILED);
	bfa_nw_ioc_debug_save_ftrc(ioc);
}

/*                        */
static void
bfa_ioc_pf_enabled(struct bfa_ioc *ioc)
{
	bfa_fsm_send_event(ioc, IOC_E_ENABLED);
}

static void
bfa_ioc_pf_disabled(struct bfa_ioc *ioc)
{
	bfa_fsm_send_event(ioc, IOC_E_DISABLED);
}

static void
bfa_ioc_pf_failed(struct bfa_ioc *ioc)
{
	bfa_fsm_send_event(ioc, IOC_E_PFFAILED);
}

static void
bfa_ioc_pf_hwfailed(struct bfa_ioc *ioc)
{
	bfa_fsm_send_event(ioc, IOC_E_HWFAILED);
}

static void
bfa_ioc_pf_fwmismatch(struct bfa_ioc *ioc)
{
	/* 
                                                            
  */
	ioc->cbfn->enable_cbfn(ioc->bfa, BFA_STATUS_IOC_FAILURE);
}

/*            */
static enum bfa_status
bfa_ioc_pll_init(struct bfa_ioc *ioc)
{
	/*
                                                                   
  */
	bfa_nw_ioc_sem_get(ioc->ioc_regs.ioc_init_sem_reg);

	bfa_ioc_pll_init_asic(ioc);

	ioc->pllinit = true;

	/*                 */
	bfa_ioc_lmem_init(ioc);

	/*
                       
  */
	bfa_nw_ioc_sem_release(ioc->ioc_regs.ioc_init_sem_reg);

	return BFA_STATUS_OK;
}

/*                                                                   
                       
 */
static void
bfa_ioc_boot(struct bfa_ioc *ioc, enum bfi_fwboot_type boot_type,
		u32 boot_env)
{
	bfa_ioc_stats(ioc, ioc_boots);

	if (bfa_ioc_pll_init(ioc) != BFA_STATUS_OK)
		return;

	/* 
                                                          
  */
	if (boot_type == BFI_FWBOOT_TYPE_MEMTEST) {
		writel(BFI_IOC_MEMTEST, ioc->ioc_regs.ioc_fwstate);
		writel(BFI_IOC_MEMTEST, ioc->ioc_regs.alt_ioc_fwstate);
	} else {
		writel(BFI_IOC_INITING, ioc->ioc_regs.ioc_fwstate);
		writel(BFI_IOC_INITING, ioc->ioc_regs.alt_ioc_fwstate);
	}

	bfa_ioc_msgflush(ioc);
	bfa_ioc_download_fw(ioc, boot_type, boot_env);
	bfa_ioc_lpu_start(ioc);
}

/*                                           */
void
bfa_nw_ioc_auto_recover(bool auto_recover)
{
	bfa_nw_auto_recover = auto_recover;
}

static bool
bfa_ioc_msgget(struct bfa_ioc *ioc, void *mbmsg)
{
	u32	*msgp = mbmsg;
	u32	r32;
	int		i;

	r32 = readl(ioc->ioc_regs.lpu_mbox_cmd);
	if ((r32 & 1) == 0)
		return false;

	/* 
                     
  */
	for (i = 0; i < (sizeof(union bfi_ioc_i2h_msg_u) / sizeof(u32));
	     i++) {
		r32 = readl(ioc->ioc_regs.lpu_mbox +
				   i * sizeof(u32));
		msgp[i] = htonl(r32);
	}

	/* 
                                                         
  */
	writel(1, ioc->ioc_regs.lpu_mbox_cmd);
	readl(ioc->ioc_regs.lpu_mbox_cmd);

	return true;
}

static void
bfa_ioc_isr(struct bfa_ioc *ioc, struct bfi_mbmsg *m)
{
	union bfi_ioc_i2h_msg_u	*msg;
	struct bfa_iocpf *iocpf = &ioc->iocpf;

	msg = (union bfi_ioc_i2h_msg_u *) m;

	bfa_ioc_stats(ioc, ioc_isrs);

	switch (msg->mh.msg_id) {
	case BFI_IOC_I2H_HBEAT:
		break;

	case BFI_IOC_I2H_ENABLE_REPLY:
		bfa_ioc_enable_reply(ioc,
			(enum bfa_mode)msg->fw_event.port_mode,
			msg->fw_event.cap_bm);
		break;

	case BFI_IOC_I2H_DISABLE_REPLY:
		bfa_fsm_send_event(iocpf, IOCPF_E_FWRSP_DISABLE);
		break;

	case BFI_IOC_I2H_GETATTR_REPLY:
		bfa_ioc_getattr_reply(ioc);
		break;

	default:
		BUG_ON(1);
	}
}

/* 
                                                                
  
                       
                                  
 */
void
bfa_nw_ioc_attach(struct bfa_ioc *ioc, void *bfa, struct bfa_ioc_cbfn *cbfn)
{
	ioc->bfa	= bfa;
	ioc->cbfn	= cbfn;
	ioc->fcmode	= false;
	ioc->pllinit	= false;
	ioc->dbg_fwsave_once = true;
	ioc->iocpf.ioc  = ioc;

	bfa_ioc_mbox_attach(ioc);
	INIT_LIST_HEAD(&ioc->notify_q);

	bfa_fsm_set_state(ioc, bfa_ioc_sm_uninit);
	bfa_fsm_send_event(ioc, IOC_E_RESET);
}

/*                                 */
void
bfa_nw_ioc_detach(struct bfa_ioc *ioc)
{
	bfa_fsm_send_event(ioc, IOC_E_DETACH);

	/*                                       */
	INIT_LIST_HEAD(&ioc->notify_q);
}

/* 
                                                  
  
                                               
 */
void
bfa_nw_ioc_pci_init(struct bfa_ioc *ioc, struct bfa_pcidev *pcidev,
		 enum bfi_pcifn_class clscode)
{
	ioc->clscode	= clscode;
	ioc->pcidev	= *pcidev;

	/* 
                                         
  */
	ioc->port0_mode = ioc->port1_mode = BFI_PORT_MODE_FC;
	ioc->asic_mode  = BFI_ASIC_MODE_FC;

	switch (pcidev->device_id) {
	case PCI_DEVICE_ID_BROCADE_CT:
		ioc->asic_gen = BFI_ASIC_GEN_CT;
		ioc->port0_mode = ioc->port1_mode = BFI_PORT_MODE_ETH;
		ioc->asic_mode  = BFI_ASIC_MODE_ETH;
		ioc->port_mode = ioc->port_mode_cfg = BFA_MODE_CNA;
		ioc->ad_cap_bm = BFA_CM_CNA;
		break;

	case BFA_PCI_DEVICE_ID_CT2:
		ioc->asic_gen = BFI_ASIC_GEN_CT2;
		if (clscode == BFI_PCIFN_CLASS_FC &&
			pcidev->ssid == BFA_PCI_CT2_SSID_FC) {
			ioc->asic_mode  = BFI_ASIC_MODE_FC16;
			ioc->fcmode = true;
			ioc->port_mode = ioc->port_mode_cfg = BFA_MODE_HBA;
			ioc->ad_cap_bm = BFA_CM_HBA;
		} else {
			ioc->port0_mode = ioc->port1_mode = BFI_PORT_MODE_ETH;
			ioc->asic_mode  = BFI_ASIC_MODE_ETH;
			if (pcidev->ssid == BFA_PCI_CT2_SSID_FCoE) {
				ioc->port_mode =
				ioc->port_mode_cfg = BFA_MODE_CNA;
				ioc->ad_cap_bm = BFA_CM_CNA;
			} else {
				ioc->port_mode =
				ioc->port_mode_cfg = BFA_MODE_NIC;
				ioc->ad_cap_bm = BFA_CM_NIC;
			}
		}
		break;

	default:
		BUG_ON(1);
	}

	/* 
                                 
  */
	if (ioc->asic_gen == BFI_ASIC_GEN_CT)
		bfa_nw_ioc_set_ct_hwif(ioc);
	else {
		WARN_ON(ioc->asic_gen != BFI_ASIC_GEN_CT2);
		bfa_nw_ioc_set_ct2_hwif(ioc);
		bfa_nw_ioc_ct2_poweron(ioc);
	}

	bfa_ioc_map_port(ioc);
	bfa_ioc_reg_init(ioc);
}

/* 
                                                   
  
                                                    
                                             
 */
void
bfa_nw_ioc_mem_claim(struct bfa_ioc *ioc,  u8 *dm_kva, u64 dm_pa)
{
	/* 
                                     
  */
	ioc->attr_dma.kva = dm_kva;
	ioc->attr_dma.pa = dm_pa;
	ioc->attr = (struct bfi_ioc_attr *) dm_kva;
}

/*                                     */
u32
bfa_nw_ioc_meminfo(void)
{
	return roundup(sizeof(struct bfi_ioc_attr), BFA_DMA_ALIGN_SZ);
}

void
bfa_nw_ioc_enable(struct bfa_ioc *ioc)
{
	bfa_ioc_stats(ioc, ioc_enables);
	ioc->dbg_fwsave_once = true;

	bfa_fsm_send_event(ioc, IOC_E_ENABLE);
}

void
bfa_nw_ioc_disable(struct bfa_ioc *ioc)
{
	bfa_ioc_stats(ioc, ioc_disables);
	bfa_fsm_send_event(ioc, IOC_E_DISABLE);
}

/*                                              */
void
bfa_nw_ioc_debug_memclaim(struct bfa_ioc *ioc, void *dbg_fwsave)
{
	ioc->dbg_fwsave = dbg_fwsave;
	ioc->dbg_fwsave_len = ioc->iocpf.auto_recover ? BNA_DBG_FWTRC_LEN : 0;
}

static u32
bfa_ioc_smem_pgnum(struct bfa_ioc *ioc, u32 fmaddr)
{
	return PSS_SMEM_PGNUM(ioc->ioc_regs.smem_pg0, fmaddr);
}

/*                                                                           */
void
bfa_nw_ioc_mbox_regisr(struct bfa_ioc *ioc, enum bfi_mclass mc,
		    bfa_ioc_mbox_mcfunc_t cbfn, void *cbarg)
{
	struct bfa_ioc_mbox_mod *mod = &ioc->mbox_mod;

	mod->mbhdlr[mc].cbfn	= cbfn;
	mod->mbhdlr[mc].cbarg = cbarg;
}

/* 
                                                                       
  
                     
                        
  
                                                                  
 */
bool
bfa_nw_ioc_mbox_queue(struct bfa_ioc *ioc, struct bfa_mbox_cmd *cmd,
			bfa_mbox_cmd_cbfn_t cbfn, void *cbarg)
{
	struct bfa_ioc_mbox_mod *mod = &ioc->mbox_mod;
	u32			stat;

	cmd->cbfn = cbfn;
	cmd->cbarg = cbarg;

	/* 
                                                       
  */
	if (!list_empty(&mod->cmd_q)) {
		list_add_tail(&cmd->qe, &mod->cmd_q);
		return true;
	}

	/* 
                                                    
  */
	stat = readl(ioc->ioc_regs.hfn_mbox_cmd);
	if (stat) {
		list_add_tail(&cmd->qe, &mod->cmd_q);
		return true;
	}

	/* 
                                                
  */
	bfa_ioc_mbox_send(ioc, cmd->msg, sizeof(cmd->msg));

	return false;
}

/*                           */
void
bfa_nw_ioc_mbox_isr(struct bfa_ioc *ioc)
{
	struct bfa_ioc_mbox_mod *mod = &ioc->mbox_mod;
	struct bfi_mbmsg m;
	int				mc;

	if (bfa_ioc_msgget(ioc, &m)) {
		/* 
                                        
   */
		mc = m.mh.msg_class;
		if (mc == BFI_MC_IOC) {
			bfa_ioc_isr(ioc, &m);
			return;
		}

		if ((mc >= BFI_MC_MAX) || (mod->mbhdlr[mc].cbfn == NULL))
			return;

		mod->mbhdlr[mc].cbfn(mod->mbhdlr[mc].cbarg, &m);
	}

	bfa_ioc_lpu_read_stat(ioc);

	/* 
                                        
  */
	bfa_ioc_mbox_poll(ioc);
}

void
bfa_nw_ioc_error_isr(struct bfa_ioc *ioc)
{
	bfa_ioc_stats(ioc, ioc_hbfails);
	bfa_ioc_stats_hb_count(ioc, ioc->hb_count);
	bfa_fsm_send_event(ioc, IOC_E_HWERROR);
}

/*                                */
bool
bfa_nw_ioc_is_disabled(struct bfa_ioc *ioc)
{
	return bfa_fsm_cmp_state(ioc, bfa_ioc_sm_disabling) ||
		bfa_fsm_cmp_state(ioc, bfa_ioc_sm_disabled);
}

/*                                   */
bool
bfa_nw_ioc_is_operational(struct bfa_ioc *ioc)
{
	return bfa_fsm_cmp_state(ioc, bfa_ioc_sm_op);
}

/*                                                                      
                                   
 */
void
bfa_nw_ioc_notify_register(struct bfa_ioc *ioc,
			struct bfa_ioc_notify *notify)
{
	list_add_tail(&notify->qe, &ioc->notify_q);
}

#define BFA_MFG_NAME "Brocade"
static void
bfa_ioc_get_adapter_attr(struct bfa_ioc *ioc,
			 struct bfa_adapter_attr *ad_attr)
{
	struct bfi_ioc_attr *ioc_attr;

	ioc_attr = ioc->attr;

	bfa_ioc_get_adapter_serial_num(ioc, ad_attr->serial_num);
	bfa_ioc_get_adapter_fw_ver(ioc, ad_attr->fw_ver);
	bfa_ioc_get_adapter_optrom_ver(ioc, ad_attr->optrom_ver);
	bfa_ioc_get_adapter_manufacturer(ioc, ad_attr->manufacturer);
	memcpy(&ad_attr->vpd, &ioc_attr->vpd,
		      sizeof(struct bfa_mfg_vpd));

	ad_attr->nports = bfa_ioc_get_nports(ioc);
	ad_attr->max_speed = bfa_ioc_speed_sup(ioc);

	bfa_ioc_get_adapter_model(ioc, ad_attr->model);
	/*                                             */
	bfa_ioc_get_adapter_model(ioc, ad_attr->model_descr);

	ad_attr->card_type = ioc_attr->card_type;
	ad_attr->is_mezz = bfa_mfg_is_mezz(ioc_attr->card_type);

	if (BFI_ADAPTER_IS_SPECIAL(ioc_attr->adapter_prop))
		ad_attr->prototype = 1;
	else
		ad_attr->prototype = 0;

	ad_attr->pwwn = bfa_ioc_get_pwwn(ioc);
	ad_attr->mac  = bfa_nw_ioc_get_mac(ioc);

	ad_attr->pcie_gen = ioc_attr->pcie_gen;
	ad_attr->pcie_lanes = ioc_attr->pcie_lanes;
	ad_attr->pcie_lanes_orig = ioc_attr->pcie_lanes_orig;
	ad_attr->asic_rev = ioc_attr->asic_rev;

	bfa_ioc_get_pci_chip_rev(ioc, ad_attr->hw_ver);
}

static enum bfa_ioc_type
bfa_ioc_get_type(struct bfa_ioc *ioc)
{
	if (ioc->clscode == BFI_PCIFN_CLASS_ETH)
		return BFA_IOC_TYPE_LL;

	BUG_ON(!(ioc->clscode == BFI_PCIFN_CLASS_FC));

	return (ioc->attr->port_mode == BFI_PORT_MODE_FC)
		? BFA_IOC_TYPE_FC : BFA_IOC_TYPE_FCoE;
}

static void
bfa_ioc_get_adapter_serial_num(struct bfa_ioc *ioc, char *serial_num)
{
	memcpy(serial_num,
			(void *)ioc->attr->brcd_serialnum,
			BFA_ADAPTER_SERIAL_NUM_LEN);
}

static void
bfa_ioc_get_adapter_fw_ver(struct bfa_ioc *ioc, char *fw_ver)
{
	memcpy(fw_ver, ioc->attr->fw_version, BFA_VERSION_LEN);
}

static void
bfa_ioc_get_pci_chip_rev(struct bfa_ioc *ioc, char *chip_rev)
{
	BUG_ON(!(chip_rev));

	memset(chip_rev, 0, BFA_IOC_CHIP_REV_LEN);

	chip_rev[0] = 'R';
	chip_rev[1] = 'e';
	chip_rev[2] = 'v';
	chip_rev[3] = '-';
	chip_rev[4] = ioc->attr->asic_rev;
	chip_rev[5] = '\0';
}

static void
bfa_ioc_get_adapter_optrom_ver(struct bfa_ioc *ioc, char *optrom_ver)
{
	memcpy(optrom_ver, ioc->attr->optrom_version,
		      BFA_VERSION_LEN);
}

static void
bfa_ioc_get_adapter_manufacturer(struct bfa_ioc *ioc, char *manufacturer)
{
	memcpy(manufacturer, BFA_MFG_NAME, BFA_ADAPTER_MFG_NAME_LEN);
}

static void
bfa_ioc_get_adapter_model(struct bfa_ioc *ioc, char *model)
{
	struct bfi_ioc_attr *ioc_attr;

	BUG_ON(!(model));
	memset(model, 0, BFA_ADAPTER_MODEL_NAME_LEN);

	ioc_attr = ioc->attr;

	snprintf(model, BFA_ADAPTER_MODEL_NAME_LEN, "%s-%u",
		BFA_MFG_NAME, ioc_attr->card_type);
}

static enum bfa_ioc_state
bfa_ioc_get_state(struct bfa_ioc *ioc)
{
	enum bfa_iocpf_state iocpf_st;
	enum bfa_ioc_state ioc_st = bfa_sm_to_state(ioc_sm_table, ioc->fsm);

	if (ioc_st == BFA_IOC_ENABLING ||
		ioc_st == BFA_IOC_FAIL || ioc_st == BFA_IOC_INITFAIL) {

		iocpf_st = bfa_sm_to_state(iocpf_sm_table, ioc->iocpf.fsm);

		switch (iocpf_st) {
		case BFA_IOCPF_SEMWAIT:
			ioc_st = BFA_IOC_SEMWAIT;
			break;

		case BFA_IOCPF_HWINIT:
			ioc_st = BFA_IOC_HWINIT;
			break;

		case BFA_IOCPF_FWMISMATCH:
			ioc_st = BFA_IOC_FWMISMATCH;
			break;

		case BFA_IOCPF_FAIL:
			ioc_st = BFA_IOC_FAIL;
			break;

		case BFA_IOCPF_INITFAIL:
			ioc_st = BFA_IOC_INITFAIL;
			break;

		default:
			break;
		}
	}
	return ioc_st;
}

void
bfa_nw_ioc_get_attr(struct bfa_ioc *ioc, struct bfa_ioc_attr *ioc_attr)
{
	memset((void *)ioc_attr, 0, sizeof(struct bfa_ioc_attr));

	ioc_attr->state = bfa_ioc_get_state(ioc);
	ioc_attr->port_id = ioc->port_id;
	ioc_attr->port_mode = ioc->port_mode;

	ioc_attr->port_mode_cfg = ioc->port_mode_cfg;
	ioc_attr->cap_bm = ioc->ad_cap_bm;

	ioc_attr->ioc_type = bfa_ioc_get_type(ioc);

	bfa_ioc_get_adapter_attr(ioc, &ioc_attr->adapter_attr);

	ioc_attr->pci_attr.device_id = ioc->pcidev.device_id;
	ioc_attr->pci_attr.pcifn = ioc->pcidev.pci_func;
	bfa_ioc_get_pci_chip_rev(ioc, ioc_attr->pci_attr.chip_rev);
}

/*            */
static u64
bfa_ioc_get_pwwn(struct bfa_ioc *ioc)
{
	return ioc->attr->pwwn;
}

mac_t
bfa_nw_ioc_get_mac(struct bfa_ioc *ioc)
{
	return ioc->attr->mac;
}

/*                                                    */
static void
bfa_ioc_recover(struct bfa_ioc *ioc)
{
	pr_crit("Heart Beat of IOC has failed\n");
	bfa_ioc_stats(ioc, ioc_hbfails);
	bfa_ioc_stats_hb_count(ioc, ioc->hb_count);
	bfa_fsm_send_event(ioc, IOC_E_HBFAIL);
}

/*                              */

static void
bfa_iocpf_enable(struct bfa_ioc *ioc)
{
	bfa_fsm_send_event(&ioc->iocpf, IOCPF_E_ENABLE);
}

static void
bfa_iocpf_disable(struct bfa_ioc *ioc)
{
	bfa_fsm_send_event(&ioc->iocpf, IOCPF_E_DISABLE);
}

static void
bfa_iocpf_fail(struct bfa_ioc *ioc)
{
	bfa_fsm_send_event(&ioc->iocpf, IOCPF_E_FAIL);
}

static void
bfa_iocpf_initfail(struct bfa_ioc *ioc)
{
	bfa_fsm_send_event(&ioc->iocpf, IOCPF_E_INITFAIL);
}

static void
bfa_iocpf_getattrfail(struct bfa_ioc *ioc)
{
	bfa_fsm_send_event(&ioc->iocpf, IOCPF_E_GETATTRFAIL);
}

static void
bfa_iocpf_stop(struct bfa_ioc *ioc)
{
	bfa_fsm_send_event(&ioc->iocpf, IOCPF_E_STOP);
}

void
bfa_nw_iocpf_timeout(void *ioc_arg)
{
	struct bfa_ioc  *ioc = (struct bfa_ioc *) ioc_arg;
	enum bfa_iocpf_state iocpf_st;

	iocpf_st = bfa_sm_to_state(iocpf_sm_table, ioc->iocpf.fsm);

	if (iocpf_st == BFA_IOCPF_HWINIT)
		bfa_ioc_poll_fwinit(ioc);
	else
		bfa_fsm_send_event(&ioc->iocpf, IOCPF_E_TIMEOUT);
}

void
bfa_nw_iocpf_sem_timeout(void *ioc_arg)
{
	struct bfa_ioc  *ioc = (struct bfa_ioc *) ioc_arg;

	bfa_ioc_hw_sem_get(ioc);
}

static void
bfa_ioc_poll_fwinit(struct bfa_ioc *ioc)
{
	u32 fwstate = readl(ioc->ioc_regs.ioc_fwstate);

	if (fwstate == BFI_IOC_DISABLED) {
		bfa_fsm_send_event(&ioc->iocpf, IOCPF_E_FWREADY);
		return;
	}

	if (ioc->iocpf.poll_time >= BFA_IOC_TOV) {
		bfa_nw_iocpf_timeout(ioc);
	} else {
		ioc->iocpf.poll_time += BFA_IOC_POLL_TOV;
		mod_timer(&ioc->iocpf_timer, jiffies +
			msecs_to_jiffies(BFA_IOC_POLL_TOV));
	}
}

/*
                        
 */

/*
                                                                   
                                                                    
                                                 
 */
#define BFA_FLASH_SEG_SZ	2048
#define BFA_FLASH_DMA_BUF_SZ	\
	roundup(0x010000 + sizeof(struct bfa_mfg_block), BFA_FLASH_SEG_SZ)

static void
bfa_flash_cb(struct bfa_flash *flash)
{
	flash->op_busy = 0;
	if (flash->cbfn)
		flash->cbfn(flash->cbarg, flash->status);
}

static void
bfa_flash_notify(void *cbarg, enum bfa_ioc_event event)
{
	struct bfa_flash *flash = cbarg;

	switch (event) {
	case BFA_IOC_E_DISABLED:
	case BFA_IOC_E_FAILED:
		if (flash->op_busy) {
			flash->status = BFA_STATUS_IOC_FAILURE;
			flash->cbfn(flash->cbarg, flash->status);
			flash->op_busy = 0;
		}
		break;
	default:
		break;
	}
}

/*
                            
 */
static void
bfa_flash_write_send(struct bfa_flash *flash)
{
	struct bfi_flash_write_req *msg =
			(struct bfi_flash_write_req *) flash->mb.msg;
	u32	len;

	msg->type = be32_to_cpu(flash->type);
	msg->instance = flash->instance;
	msg->offset = be32_to_cpu(flash->addr_off + flash->offset);
	len = (flash->residue < BFA_FLASH_DMA_BUF_SZ) ?
	       flash->residue : BFA_FLASH_DMA_BUF_SZ;
	msg->length = be32_to_cpu(len);

	/*                                                            */
	msg->last = (len == flash->residue) ? 1 : 0;

	bfi_h2i_set(msg->mh, BFI_MC_FLASH, BFI_FLASH_H2I_WRITE_REQ,
		    bfa_ioc_portid(flash->ioc));
	bfa_alen_set(&msg->alen, len, flash->dbuf_pa);
	memcpy(flash->dbuf_kva, flash->ubuf + flash->offset, len);
	bfa_nw_ioc_mbox_queue(flash->ioc, &flash->mb, NULL, NULL);

	flash->residue -= len;
	flash->offset += len;
}

/* 
                                                 
  
                            
 */
static void
bfa_flash_read_send(void *cbarg)
{
	struct bfa_flash *flash = cbarg;
	struct bfi_flash_read_req *msg =
			(struct bfi_flash_read_req *) flash->mb.msg;
	u32	len;

	msg->type = be32_to_cpu(flash->type);
	msg->instance = flash->instance;
	msg->offset = be32_to_cpu(flash->addr_off + flash->offset);
	len = (flash->residue < BFA_FLASH_DMA_BUF_SZ) ?
	       flash->residue : BFA_FLASH_DMA_BUF_SZ;
	msg->length = be32_to_cpu(len);
	bfi_h2i_set(msg->mh, BFI_MC_FLASH, BFI_FLASH_H2I_READ_REQ,
		    bfa_ioc_portid(flash->ioc));
	bfa_alen_set(&msg->alen, len, flash->dbuf_pa);
	bfa_nw_ioc_mbox_queue(flash->ioc, &flash->mb, NULL, NULL);
}

/* 
                                                                              
  
                             
                          
 */
static void
bfa_flash_intr(void *flasharg, struct bfi_mbmsg *msg)
{
	struct bfa_flash *flash = flasharg;
	u32	status;

	union {
		struct bfi_flash_query_rsp *query;
		struct bfi_flash_write_rsp *write;
		struct bfi_flash_read_rsp *read;
		struct bfi_mbmsg   *msg;
	} m;

	m.msg = msg;

	/*                                      */
	if (!flash->op_busy && msg->mh.msg_id != BFI_FLASH_I2H_EVENT)
		return;

	switch (msg->mh.msg_id) {
	case BFI_FLASH_I2H_QUERY_RSP:
		status = be32_to_cpu(m.query->status);
		if (status == BFA_STATUS_OK) {
			u32	i;
			struct bfa_flash_attr *attr, *f;

			attr = (struct bfa_flash_attr *) flash->ubuf;
			f = (struct bfa_flash_attr *) flash->dbuf_kva;
			attr->status = be32_to_cpu(f->status);
			attr->npart = be32_to_cpu(f->npart);
			for (i = 0; i < attr->npart; i++) {
				attr->part[i].part_type =
					be32_to_cpu(f->part[i].part_type);
				attr->part[i].part_instance =
					be32_to_cpu(f->part[i].part_instance);
				attr->part[i].part_off =
					be32_to_cpu(f->part[i].part_off);
				attr->part[i].part_size =
					be32_to_cpu(f->part[i].part_size);
				attr->part[i].part_len =
					be32_to_cpu(f->part[i].part_len);
				attr->part[i].part_status =
					be32_to_cpu(f->part[i].part_status);
			}
		}
		flash->status = status;
		bfa_flash_cb(flash);
		break;
	case BFI_FLASH_I2H_WRITE_RSP:
		status = be32_to_cpu(m.write->status);
		if (status != BFA_STATUS_OK || flash->residue == 0) {
			flash->status = status;
			bfa_flash_cb(flash);
		} else
			bfa_flash_write_send(flash);
		break;
	case BFI_FLASH_I2H_READ_RSP:
		status = be32_to_cpu(m.read->status);
		if (status != BFA_STATUS_OK) {
			flash->status = status;
			bfa_flash_cb(flash);
		} else {
			u32 len = be32_to_cpu(m.read->length);
			memcpy(flash->ubuf + flash->offset,
			       flash->dbuf_kva, len);
			flash->residue -= len;
			flash->offset += len;
			if (flash->residue == 0) {
				flash->status = status;
				bfa_flash_cb(flash);
			} else
				bfa_flash_read_send(flash);
		}
		break;
	case BFI_FLASH_I2H_BOOT_VER_RSP:
	case BFI_FLASH_I2H_EVENT:
		break;
	default:
		WARN_ON(1);
	}
}

/*
                         
 */
u32
bfa_nw_flash_meminfo(void)
{
	return roundup(BFA_FLASH_DMA_BUF_SZ, BFA_DMA_ALIGN_SZ);
}

/* 
                                          
  
                          
                      
                         
 */
void
bfa_nw_flash_attach(struct bfa_flash *flash, struct bfa_ioc *ioc, void *dev)
{
	flash->ioc = ioc;
	flash->cbfn = NULL;
	flash->cbarg = NULL;
	flash->op_busy = 0;

	bfa_nw_ioc_mbox_regisr(flash->ioc, BFI_MC_FLASH, bfa_flash_intr, flash);
	bfa_q_qe_init(&flash->ioc_notify);
	bfa_ioc_notify_init(&flash->ioc_notify, bfa_flash_notify, flash);
	list_add_tail(&flash->ioc_notify.qe, &flash->ioc->notify_q);
}

/* 
                                                 
  
                          
                                             
                                  
 */
void
bfa_nw_flash_memclaim(struct bfa_flash *flash, u8 *dm_kva, u64 dm_pa)
{
	flash->dbuf_kva = dm_kva;
	flash->dbuf_pa = dm_pa;
	memset(flash->dbuf_kva, 0, BFA_FLASH_DMA_BUF_SZ);
	dm_kva += roundup(BFA_FLASH_DMA_BUF_SZ, BFA_DMA_ALIGN_SZ);
	dm_pa += roundup(BFA_FLASH_DMA_BUF_SZ, BFA_DMA_ALIGN_SZ);
}

/* 
                                               
  
                          
                                   
                           
                            
  
                 
 */
enum bfa_status
bfa_nw_flash_get_attr(struct bfa_flash *flash, struct bfa_flash_attr *attr,
		      bfa_cb_flash cbfn, void *cbarg)
{
	struct bfi_flash_query_req *msg =
			(struct bfi_flash_query_req *) flash->mb.msg;

	if (!bfa_nw_ioc_is_operational(flash->ioc))
		return BFA_STATUS_IOC_NON_OP;

	if (flash->op_busy)
		return BFA_STATUS_DEVBUSY;

	flash->op_busy = 1;
	flash->cbfn = cbfn;
	flash->cbarg = cbarg;
	flash->ubuf = (u8 *) attr;

	bfi_h2i_set(msg->mh, BFI_MC_FLASH, BFI_FLASH_H2I_QUERY_REQ,
		    bfa_ioc_portid(flash->ioc));
	bfa_alen_set(&msg->alen, sizeof(struct bfa_flash_attr), flash->dbuf_pa);
	bfa_nw_ioc_mbox_queue(flash->ioc, &flash->mb, NULL, NULL);

	return BFA_STATUS_OK;
}

/* 
                                                     
  
                          
                              
                                      
                           
                           
                                                             
                           
                            
  
                 
 */
enum bfa_status
bfa_nw_flash_update_part(struct bfa_flash *flash, u32 type, u8 instance,
			 void *buf, u32 len, u32 offset,
			 bfa_cb_flash cbfn, void *cbarg)
{
	if (!bfa_nw_ioc_is_operational(flash->ioc))
		return BFA_STATUS_IOC_NON_OP;

	/*
                                           
  */
	if (!len || (len & 0x03))
		return BFA_STATUS_FLASH_BAD_LEN;

	if (type == BFA_FLASH_PART_MFG)
		return BFA_STATUS_EINVAL;

	if (flash->op_busy)
		return BFA_STATUS_DEVBUSY;

	flash->op_busy = 1;
	flash->cbfn = cbfn;
	flash->cbarg = cbarg;
	flash->type = type;
	flash->instance = instance;
	flash->residue = len;
	flash->offset = 0;
	flash->addr_off = offset;
	flash->ubuf = buf;

	bfa_flash_write_send(flash);

	return BFA_STATUS_OK;
}

/* 
                                                 
  
                          
                              
                                      
                         
                           
                                                             
                           
                            
  
                 
 */
enum bfa_status
bfa_nw_flash_read_part(struct bfa_flash *flash, u32 type, u8 instance,
		       void *buf, u32 len, u32 offset,
		       bfa_cb_flash cbfn, void *cbarg)
{
	if (!bfa_nw_ioc_is_operational(flash->ioc))
		return BFA_STATUS_IOC_NON_OP;

	/*
                                           
  */
	if (!len || (len & 0x03))
		return BFA_STATUS_FLASH_BAD_LEN;

	if (flash->op_busy)
		return BFA_STATUS_DEVBUSY;

	flash->op_busy = 1;
	flash->cbfn = cbfn;
	flash->cbarg = cbarg;
	flash->type = type;
	flash->instance = instance;
	flash->residue = len;
	flash->offset = 0;
	flash->addr_off = offset;
	flash->ubuf = buf;

	bfa_flash_read_send(flash);

	return BFA_STATUS_OK;
}
