/*
 * Copyright (C) 2010-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <linux/fs.h>       /*                        */
#include <asm/uaccess.h>    /*                   */

#include "mali_ukk.h"
#include "mali_osk.h"
#include "mali_kernel_common.h"
#include "mali_session.h"
#include "mali_ukk_wrappers.h"

int mem_write_safe_wrapper(struct mali_session_data *session_data, _mali_uk_mem_write_safe_s __user * uargs)
{
	_mali_uk_mem_write_safe_s kargs;
	_mali_osk_errcode_t err;

	MALI_CHECK_NON_NULL(uargs, -EINVAL);
	MALI_CHECK_NON_NULL(session_data, -EINVAL);

	if (0 != copy_from_user(&kargs, uargs, sizeof(_mali_uk_mem_write_safe_s))) {
		return -EFAULT;
	}

	kargs.ctx = session_data;

	/*                                    */
	if (!access_ok(VERIFY_WRITE, kargs.dest, kargs.size)
	    || !access_ok(VERIFY_READ, kargs.src, kargs.size)) {
		return -EINVAL;
	}

	/*                     */
	if ((kargs.size + kargs.dest) <= kargs.dest
	    || (kargs.size + kargs.src) <= kargs.src) {
		return -EINVAL;
	}

	err = _mali_ukk_mem_write_safe(&kargs);
	if (_MALI_OSK_ERR_OK != err) {
		return map_errcode(err);
	}

	if (0 != put_user(kargs.size, &uargs->size)) {
		return -EFAULT;
	}

	return 0;
}

int mem_map_ext_wrapper(struct mali_session_data *session_data, _mali_uk_map_external_mem_s __user * argument)
{
	_mali_uk_map_external_mem_s uk_args;
	_mali_osk_errcode_t err_code;

	/*                */
	/*                                                  */
	MALI_CHECK_NON_NULL( argument, -EINVAL);

	/*                                                                                                  */
	if ( 0 != copy_from_user(&uk_args, (void __user *)argument, sizeof(_mali_uk_map_external_mem_s)) ) {
		return -EFAULT;
	}

	uk_args.ctx = session_data;
	err_code = _mali_ukk_map_external_mem( &uk_args );

	if (0 != put_user(uk_args.cookie, &argument->cookie)) {
		if (_MALI_OSK_ERR_OK == err_code) {
			/*          */
			_mali_uk_unmap_external_mem_s uk_args_unmap;

			uk_args_unmap.ctx = session_data;
			uk_args_unmap.cookie = uk_args.cookie;
			err_code = _mali_ukk_unmap_external_mem( &uk_args_unmap );
			if (_MALI_OSK_ERR_OK != err_code) {
				MALI_DEBUG_PRINT(4, ("reverting _mali_ukk_unmap_external_mem, as a result of failing put_user(), failed\n"));
			}
		}
		return -EFAULT;
	}

	/*                                                         */
	return map_errcode(err_code);
}

int mem_unmap_ext_wrapper(struct mali_session_data *session_data, _mali_uk_unmap_external_mem_s __user * argument)
{
	_mali_uk_unmap_external_mem_s uk_args;
	_mali_osk_errcode_t err_code;

	/*                */
	/*                                                  */
	MALI_CHECK_NON_NULL( argument, -EINVAL);

	/*                                                                                                  */
	if ( 0 != copy_from_user(&uk_args, (void __user *)argument, sizeof(_mali_uk_unmap_external_mem_s)) ) {
		return -EFAULT;
	}

	uk_args.ctx = session_data;
	err_code = _mali_ukk_unmap_external_mem( &uk_args );

	/*                                                         */
	return map_errcode(err_code);
}

#if defined(CONFIG_MALI400_UMP)
int mem_release_ump_wrapper(struct mali_session_data *session_data, _mali_uk_release_ump_mem_s __user * argument)
{
	_mali_uk_release_ump_mem_s uk_args;
	_mali_osk_errcode_t err_code;

	/*                */
	/*                                                  */
	MALI_CHECK_NON_NULL( argument, -EINVAL);

	/*                                                                                                  */
	if ( 0 != copy_from_user(&uk_args, (void __user *)argument, sizeof(_mali_uk_release_ump_mem_s)) ) {
		return -EFAULT;
	}

	uk_args.ctx = session_data;
	err_code = _mali_ukk_release_ump_mem( &uk_args );

	/*                                                         */
	return map_errcode(err_code);
}

int mem_attach_ump_wrapper(struct mali_session_data *session_data, _mali_uk_attach_ump_mem_s __user * argument)
{
	_mali_uk_attach_ump_mem_s uk_args;
	_mali_osk_errcode_t err_code;

	/*                */
	/*                                                  */
	MALI_CHECK_NON_NULL( argument, -EINVAL);

	/*                                                                                                  */
	if ( 0 != copy_from_user(&uk_args, (void __user *)argument, sizeof(_mali_uk_attach_ump_mem_s)) ) {
		return -EFAULT;
	}

	uk_args.ctx = session_data;
	err_code = _mali_ukk_attach_ump_mem( &uk_args );

	if (0 != put_user(uk_args.cookie, &argument->cookie)) {
		if (_MALI_OSK_ERR_OK == err_code) {
			/*          */
			_mali_uk_release_ump_mem_s uk_args_unmap;

			uk_args_unmap.ctx = session_data;
			uk_args_unmap.cookie = uk_args.cookie;
			err_code = _mali_ukk_release_ump_mem( &uk_args_unmap );
			if (_MALI_OSK_ERR_OK != err_code) {
				MALI_DEBUG_PRINT(4, ("reverting _mali_ukk_attach_mem, as a result of failing put_user(), failed\n"));
			}
		}
		return -EFAULT;
	}

	/*                                                               */
	return map_errcode(err_code);
}
#endif /*                    */

int mem_query_mmu_page_table_dump_size_wrapper(struct mali_session_data *session_data, _mali_uk_query_mmu_page_table_dump_size_s __user * uargs)
{
	_mali_uk_query_mmu_page_table_dump_size_s kargs;
	_mali_osk_errcode_t err;

	MALI_CHECK_NON_NULL(uargs, -EINVAL);
	MALI_CHECK_NON_NULL(session_data, -EINVAL);

	kargs.ctx = session_data;

	err = _mali_ukk_query_mmu_page_table_dump_size(&kargs);
	if (_MALI_OSK_ERR_OK != err) return map_errcode(err);

	if (0 != put_user(kargs.size, &uargs->size)) return -EFAULT;

	return 0;
}

int mem_dump_mmu_page_table_wrapper(struct mali_session_data *session_data, _mali_uk_dump_mmu_page_table_s __user * uargs)
{
	_mali_uk_dump_mmu_page_table_s kargs;
	_mali_osk_errcode_t err;
	void *buffer;
	int rc = -EFAULT;

	/*                */
	MALI_CHECK_NON_NULL(uargs, -EINVAL);
	/*                                                  */

	kargs.buffer = NULL;

	/*                             */
	if (0 != get_user(buffer, &uargs->buffer)) goto err_exit;
	/*                                                        */
	if ( 0 != get_user(kargs.size, &uargs->size) ) goto err_exit;
	/*                                                   */
	if (!access_ok(VERIFY_WRITE, buffer, kargs.size)) goto err_exit;

	/*                                                                      */
	MALI_CHECK(kargs.size > 0, -ENOMEM);
	kargs.buffer = _mali_osk_valloc(kargs.size);
	if (NULL == kargs.buffer) {
		rc = -ENOMEM;
		goto err_exit;
	}

	kargs.ctx = session_data;
	err = _mali_ukk_dump_mmu_page_table(&kargs);
	if (_MALI_OSK_ERR_OK != err) {
		rc = map_errcode(err);
		goto err_exit;
	}

	/*                                                                 */
	if (0 != copy_to_user(uargs->buffer, kargs.buffer, kargs.size) ) goto err_exit;
	if (0 != put_user((kargs.register_writes - (u32 *)kargs.buffer) + (u32 *)uargs->buffer, &uargs->register_writes)) goto err_exit;
	if (0 != put_user((kargs.page_table_dump - (u32 *)kargs.buffer) + (u32 *)uargs->buffer, &uargs->page_table_dump)) goto err_exit;
	if (0 != put_user(kargs.register_writes_size, &uargs->register_writes_size)) goto err_exit;
	if (0 != put_user(kargs.page_table_dump_size, &uargs->page_table_dump_size)) goto err_exit;
	rc = 0;

err_exit:
	if (kargs.buffer) _mali_osk_vfree(kargs.buffer);
	return rc;
}
