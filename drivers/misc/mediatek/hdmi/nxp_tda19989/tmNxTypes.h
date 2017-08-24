/*                                                                          */
/*     (Copyright (C) 2003 Koninklijke Philips Electronics N.V.             */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*     under an Open Source License of any type without the expressed       */
/*                                                                          */
/*                                                                          */
/*
 * Copyright (C) 2000,2001
 *               Koninklijke Philips Electronics N.V.
 *               All Rights Reserved.
 *
 * Copyright (C) 2000,2001 TriMedia Technologies, Inc.
 *               All Rights Reserved.
 *
 *############################################################
 *
 * Module name  : tmNxTypes.h  %version: 7 %
 *
 * Last Update  : %date_modified: Tue Jul  8 18:08:00 2003 %
 *
 * Description: TriMedia/MIPS global type definitions.
 *
 * Document Ref: DVP Software Coding Guidelines Specification
 * DVP/MoReUse Naming Conventions specification
 * DVP Software Versioning Specification
 * DVP Device Library Architecture Specification
 * DVP Board Support Library Architecture Specification
 * DVP Hardware API Architecture Specification
 *
 *
 *############################################################
 */

#ifndef TMNXTYPES_H
#define TMNXTYPES_H

/*                                                                               */
/*                         */
/*                                                                               */
/*  */

/*                                                                               */
/*                        */
/*                                                                               */
/*  */
#include "tmFlags.h"		/*                                */

#ifdef __cplusplus
extern "C" {
#endif

/*                                                                               */
/*                    */
/*                                                                               */
/*  */

/*                                                                         
                                                                               
                                                                              
                 */

#ifndef _TMtypes_h
#define _TMtypes_h

#define False           0
#define True            1

#ifdef __cplusplus
#define Null            0
#else
#define Null            ((Void *) 0)
#endif

/*  */
/*                */
/*  */
	typedef signed char Int8;	/*                        */
	typedef signed short Int16;	/*                         */
	typedef signed long Int32;	/*                         */
	typedef unsigned char UInt8;	/*                        */
	typedef unsigned short UInt16;	/*                         */
	typedef unsigned long UInt32;	/*                         */
	typedef float Float;	/*                       */
	typedef unsigned int Bool;	/*                      */
	typedef char Char;	/*                                */
	typedef int Int;	/*                         */
	typedef unsigned int UInt;	/*                                  */
	typedef char *String;	/*                                */

/*                                                                               */
/*                                                                             */
/*                                                                       */
/*  */
	typedef char *Address;	/*                              */
	typedef char const *ConstAddress;
	typedef unsigned char Byte;	/*          */
	typedef float Float32;	/*                        */
	typedef double Float64;	/*                        */
	typedef void *Pointer;	/*                             */
	typedef void const *ConstPointer;
	typedef char const *ConstString;

	typedef Int Endian;
#define BigEndian       0
#define LittleEndian    1

	typedef UInt32 tmErrorCode_t;
	typedef UInt32 tmProgressCode_t;


	typedef struct tmVersion {
		UInt8 majorVersion;
		UInt8 minorVersion;
		UInt16 buildVersion;
	} tmVersion_t, *ptmVersion_t;
#endif				/*                */

/*                                        */
/*
                                                                      
  
                                                                         
                                                                            
                                                                          
                                                                            
                                                                            
                                                                             
*/
	typedef signed int IBits32;	/*                                   */
	typedef unsigned int UBits32;	/*                                   */
	typedef IBits32 * pIBits32;	/*                                      */
	typedef UBits32 * pUBits32;	/*                                      */

	typedef Int8 * pInt8;
	typedef Int16 * pInt16;
	typedef Int32 * pInt32;
	typedef UInt8 *pUInt8;	/*                        */
	typedef UInt16 *pUInt16;	/*                         */
	typedef UInt32 * pUInt32;
	typedef void Void, *pVoid;	/*                 */
	typedef Float * pFloat;
	typedef double Double, *pDouble;	/*                          */
	typedef Bool * pBool;
	typedef Char * pChar;
	typedef Int * pInt;
	typedef UInt * pUInt;
	typedef String * pString;

/*                                                                              
                                                                            
                      */
#if defined __STDC_VERSION__ && __STDC_VERSION__ > 199409L
/*                                                                      
                                                      */
#define TMFL_NATIVE_INT64 1
	typedef signed long long int Int64, *pInt64;	/*                */
	typedef unsigned long long int UInt64, *pUInt64;	/*                */
/*                                            */
/*                                                                          */
/*                                                   */ */
/*                             */
/*                                                              */
/*                                                              */
#else				/*                                                          */
#define TMFL_NATIVE_INT64 0
	typedef
	    struct {
		/*                                                                   
                                                                  */
#if TMFL_ENDIAN == TMFL_ENDIAN_BIG
		Int32 hi;
		UInt32 lo;
#else
		UInt32 lo;
		Int32 hi;
#endif
	} Int64, *pInt64;	/*                */
	typedef
	    struct {
#if TMFL_ENDIAN == TMFL_ENDIAN_BIG
		UInt32 hi;
		UInt32 lo;
#else
		UInt32 lo;
		UInt32 hi;
#endif
	} UInt64, *pUInt64;	/*                */
#endif				/*                                                       */

/*                                                                    */
#define HAL_DEVICE_NAME_LENGTH 16
/*                      */
	typedef UInt64 tmTimeStamp_t, *ptmTimeStamp_t;

/*                                                                     */
#define ticks   lo
#define hiTicks hi

	typedef union tmColor3	/*                        */
	{
		UBits32 u32;
#if (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
		struct {
			UBits32:8;
			UBits32 red:8;
			UBits32 green:8;
			UBits32 blue:8;
		} rgb;
		struct {
			UBits32:8;
			UBits32 y:8;
			UBits32 u:8;
			UBits32 v:8;
		} yuv;
		struct {
			UBits32:8;
			UBits32 u:8;
			UBits32 m:8;
			UBits32 l:8;
		} uml;
#else
		struct {
			UBits32 blue:8;
			UBits32 green:8;
			UBits32 red:8;
			 UBits32:8;
		} rgb;
		struct {
			UBits32 v:8;
			UBits32 u:8;
			UBits32 y:8;
			 UBits32:8;
		} yuv;
		struct {
			UBits32 l:8;
			UBits32 m:8;
			UBits32 u:8;
			 UBits32:8;
		} uml;
#endif
	} tmColor3_t, *ptmColor3_t;

	typedef union tmColor4	/*                        */
	{
		UBits32 u32;
#if (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
		struct {
			UBits32 alpha:8;
			UBits32 red:8;
			UBits32 green:8;
			UBits32 blue:8;
		} argb;
		struct {
			UBits32 alpha:8;
			UBits32 y:8;
			UBits32 u:8;
			UBits32 v:8;
		} ayuv;
		struct {
			UBits32 alpha:8;
			UBits32 u:8;
			UBits32 m:8;
			UBits32 l:8;
		} auml;
#else
		struct {
			UBits32 blue:8;
			UBits32 green:8;
			UBits32 red:8;
			UBits32 alpha:8;
		} argb;
		struct {
			UBits32 v:8;
			UBits32 u:8;
			UBits32 y:8;
			UBits32 alpha:8;
		} ayuv;
		struct {
			UBits32 l:8;
			UBits32 m:8;
			UBits32 u:8;
			UBits32 alpha:8;
		} auml;
#endif
	} tmColor4_t, *ptmColor4_t;

/*                                                                               */
/*                              */
/*  */
	typedef enum tmPowerState {
		tmPowerOn,	/*                                   */
		tmPowerStandby,	/*                                   */
		tmPowerSuspend,	/*                                   */
		tmPowerOff	/*                                   */
	} tmPowerState_t, *ptmPowerState_t;

/*                                                                               */
/*                            */
/*  */
	typedef struct tmSWVersion {
		UInt32 compatibilityNr;	/*                                */
		UInt32 majorVersionNr;	/*                                */
		UInt32 minorVersionNr;	/*                                */

	} tmSWVersion_t, *ptmSWVersion_t;

/*                                                                          
                                                                               
                                                                              
                 */
#ifndef _TMBOARDDEF_H_
#define _TMBOARDDEF_H_

/*                                                                               */
/*                   */
/*  */
	typedef Int tmUnitSelect_t, *ptmUnitSelect_t;

#define tmUnitNone (-1)
#define tmUnit0    0
#define tmUnit1    1
#define tmUnit2    2
#define tmUnit3    3
#define tmUnit4    4

/*              */
#define unitSelect_t       tmUnitSelect_t
#define unit0              tmUnit0
#define unit1              tmUnit1
#define unit2              tmUnit2
#define unit3              tmUnit3
#define unit4              tmUnit4
#define DEVICE_NAME_LENGTH HAL_DEVICE_NAME_LENGTH
/*              */

#endif				/*                    */

/*                                                                               */
/*                 */
/*  */
	typedef Int tmInstance_t, *ptmInstance_t;

/*                               */
	typedef Void(*ptmCallback_t) (UInt32 events, Void *pData, UInt32 userData);
#define tmCallback_t ptmCallback_t	/*              */

/*                                       */
#ifdef TMFL_CFG_INTELCE4100
#define KERN_INFO void
#define printk(fmt, args...) printf(fmt, ## args)
#endif

#ifdef __cplusplus
}
#endif
#endif				/*                  */
