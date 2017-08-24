/*
 * Copyright (C) 2012 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __ASM_PGTABLE_3LEVEL_HWDEF_H
#define __ASM_PGTABLE_3LEVEL_HWDEF_H

/*
                                                                             
                                                                          
                                                                            
                                            
 */
#define PTRS_PER_PTE		512
#define PTRS_PER_PMD		512
#define PTRS_PER_PGD		512

/*
                                                                        
 */
#define PGDIR_SHIFT		30
#define PGDIR_SIZE		(_AC(1, UL) << PGDIR_SHIFT)
#define PGDIR_MASK		(~(PGDIR_SIZE-1))

/*
                                                                         
 */
#define PMD_SHIFT		21
#define PMD_SIZE		(_AC(1, UL) << PMD_SHIFT)
#define PMD_MASK		(~(PMD_SIZE-1))

/*
                                             
 */
#define SECTION_SHIFT		21
#define SECTION_SIZE		(_AC(1, UL) << SECTION_SHIFT)
#define SECTION_MASK		(~(SECTION_SIZE-1))

#endif
