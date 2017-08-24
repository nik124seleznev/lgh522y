/*
 *
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */


#include <linux/dma-mapping.h>
#include <mali_kbase.h>
#include <mali_kbase_10969_workaround.h>

/*                                                                      
                                                                                         
                                                                                          
                                                                          
 */
/*                                                                         */
#define X_COORDINATE_MASK 0x00000FFF
#define Y_COORDINATE_MASK 0x0FFF0000
/*                                                                    */
#define JOB_HEADER_SIZE_IN_WORDS 10
#define JOB_HEADER_SIZE (JOB_HEADER_SIZE_IN_WORDS*sizeof(u32))

/*                     */
#define JOB_DESC_STATUS_WORD 0
/*                       */
#define JOB_DESC_RESTART_INDEX_WORD 1
/*                                */
#define JOB_DESC_FAULT_ADDR_LOW_WORD 2
/*                                  */
#define FRAG_JOB_DESC_MIN_TILE_COORD_WORD 8
/*                                  */
#define FRAG_JOB_DESC_MAX_TILE_COORD_WORD 9

int kbasep_10969_workaround_clamp_coordinates(struct kbase_jd_atom *katom)
{
	struct device *dev = katom->kctx->kbdev->dev;
	u32   clamped = 0;
	struct kbase_va_region *region;
	phys_addr_t *page_array;
	u64 page_index;
	u32 offset = katom->jc & (~PAGE_MASK);
	u32 *page_1 = NULL;
	u32 *page_2 = NULL;
	u32   job_header[JOB_HEADER_SIZE_IN_WORDS];
	void *dst = job_header;
	u32 minX, minY, maxX, maxY;
	u32 restartX, restartY;
	struct page *p;
	u32 copy_size;

	dev_warn(dev, "Called TILE_RANGE_FAULT workaround clamping function.\n");
	if (!(katom->core_req & BASE_JD_REQ_FS))
		return 0;

	kbase_gpu_vm_lock(katom->kctx);
	region = kbase_region_tracker_find_region_enclosing_address(katom->kctx,
			katom->jc);
	if (!region || (region->flags & KBASE_REG_FREE))
		goto out_unlock;

	page_array = kbase_get_phy_pages(region);
	if (!page_array)
		goto out_unlock;

	page_index = (katom->jc >> PAGE_SHIFT) - region->start_pfn;

	p = pfn_to_page(PFN_DOWN(page_array[page_index]));

	/*                                                                  
                                                                     
                                                 
                                                                      
                                     
  */
	copy_size = MIN(PAGE_SIZE - offset, JOB_HEADER_SIZE);

	page_1 = kmap_atomic(p);

	/*                                                       */
	page_1 += offset>>2;
	dma_sync_single_for_cpu(katom->kctx->kbdev->dev,
			page_private(p) + offset,
			copy_size, DMA_BIDIRECTIONAL);
	memcpy(dst, page_1, copy_size);

	/*                                              
                                    */
	if (copy_size < JOB_HEADER_SIZE) {
		p = pfn_to_page(PFN_DOWN(page_array[page_index + 1]));
		page_2 = kmap_atomic(p);

		dma_sync_single_for_cpu(katom->kctx->kbdev->dev,
				page_private(p),
				JOB_HEADER_SIZE - copy_size, DMA_BIDIRECTIONAL);
		memcpy(dst + copy_size, page_2, JOB_HEADER_SIZE - copy_size);
	}

	/*                                                                    */
	/*                                                                     */
	minX = job_header[FRAG_JOB_DESC_MIN_TILE_COORD_WORD] & X_COORDINATE_MASK;
	minY = job_header[FRAG_JOB_DESC_MIN_TILE_COORD_WORD] & Y_COORDINATE_MASK;
	maxX = job_header[FRAG_JOB_DESC_MAX_TILE_COORD_WORD] & X_COORDINATE_MASK;
	maxY = job_header[FRAG_JOB_DESC_MAX_TILE_COORD_WORD] & Y_COORDINATE_MASK;
	restartX = job_header[JOB_DESC_FAULT_ADDR_LOW_WORD] & X_COORDINATE_MASK;
	restartY = job_header[JOB_DESC_FAULT_ADDR_LOW_WORD] & Y_COORDINATE_MASK;

	dev_warn(dev, "Before Clamping:\n"
			"Jobstatus: %08x\n"
			"restartIdx: %08x\n"
			"Fault_addr_low: %08x\n"
			"minCoordsX: %08x minCoordsY: %08x\n"
			"maxCoordsX: %08x maxCoordsY: %08x\n",
			job_header[JOB_DESC_STATUS_WORD],
			job_header[JOB_DESC_RESTART_INDEX_WORD],
			job_header[JOB_DESC_FAULT_ADDR_LOW_WORD],
			minX, minY,
			maxX, maxY);

	/*                                                           */
	job_header[JOB_DESC_RESTART_INDEX_WORD] =
			job_header[JOB_DESC_FAULT_ADDR_LOW_WORD];

	if (restartX < minX) {
		job_header[JOB_DESC_RESTART_INDEX_WORD] = (minX) | restartY;
		dev_warn(dev,
			"Clamping restart X index to minimum. %08x clamped to %08x\n",
			restartX, minX);
		clamped =  1;
	}
	if (restartY < minY) {
		job_header[JOB_DESC_RESTART_INDEX_WORD] = (minY) | restartX;
		dev_warn(dev,
			"Clamping restart Y index to minimum. %08x clamped to %08x\n",
			restartY, minY);
		clamped =  1;
	}
	if (restartX > maxX) {
		job_header[JOB_DESC_RESTART_INDEX_WORD] = (maxX) | restartY;
		dev_warn(dev,
			"Clamping restart X index to maximum. %08x clamped to %08x\n",
			restartX, maxX);
		clamped =  1;
	}
	if (restartY > maxY) {
		job_header[JOB_DESC_RESTART_INDEX_WORD] = (maxY) | restartX;
		dev_warn(dev,
			"Clamping restart Y index to maximum. %08x clamped to %08x\n",
			restartY, maxY);
		clamped =  1;
	}

	if (clamped) {
		/*                                 
                                       */
		job_header[JOB_DESC_FAULT_ADDR_LOW_WORD] = 0x0;
		job_header[JOB_DESC_STATUS_WORD] = BASE_JD_EVENT_STOPPED;
		dev_warn(dev, "After Clamping:\n"
				"Jobstatus: %08x\n"
				"restartIdx: %08x\n"
				"Fault_addr_low: %08x\n"
				"minCoordsX: %08x minCoordsY: %08x\n"
				"maxCoordsX: %08x maxCoordsY: %08x\n",
				job_header[JOB_DESC_STATUS_WORD],
				job_header[JOB_DESC_RESTART_INDEX_WORD],
				job_header[JOB_DESC_FAULT_ADDR_LOW_WORD],
				minX, minY,
				maxX, maxY);

		/*                                                      */
		memcpy(page_1, dst, copy_size);
		p = pfn_to_page(PFN_DOWN(page_array[page_index]));
		dma_sync_single_for_device(katom->kctx->kbdev->dev,
				page_private(p) + offset,
				copy_size, DMA_TO_DEVICE);

		if (copy_size < JOB_HEADER_SIZE) {
			memcpy(page_2, dst + copy_size,
					JOB_HEADER_SIZE - copy_size);
			p = pfn_to_page(PFN_DOWN(page_array[page_index + 1]));
			dma_sync_single_for_device(katom->kctx->kbdev->dev,
					page_private(p),
					JOB_HEADER_SIZE - copy_size,
					DMA_TO_DEVICE);
		}
	}
	if (copy_size < JOB_HEADER_SIZE)
		kunmap_atomic(page_2);

	kunmap_atomic(page_1);

out_unlock:
	kbase_gpu_vm_unlock(katom->kctx);
	return clamped;
}
