/*
 *  Copyright (C) 2004,2005  ADDI-DATA GmbH for the source code of this module.
 *
 *	ADDI-DATA GmbH
 *	Dieselstrasse 3
 *	D-77833 Ottersweier
 *	Tel: +19(0)7223/9493-0
 *	Fax: +49(0)7223/9493-92
 *	http://www.addi-data.com
 *	info@addi-data.com
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <linux/sched.h>
#include <linux/interrupt.h>

#define LOBYTE(W)	(unsigned char)((W) & 0xFF)
#define HIBYTE(W)	(unsigned char)(((W) >> 8) & 0xFF)
#define MAKEWORD(H, L)	(unsigned short)((L) | ((H) << 8))
#define LOWORD(W)	(unsigned short)((W) & 0xFFFF)
#define HIWORD(W)	(unsigned short)(((W) >> 16) & 0xFFFF)
#define MAKEDWORD(H, L)	(unsigned int)((L) | ((H) << 16))

#define ADDI_ENABLE		1
#define ADDI_DISABLE		0
#define APCI1710_SAVE_INTERRUPT	1

#define ADDIDATA_EEPROM		1
#define ADDIDATA_NO_EEPROM	0
#define ADDIDATA_93C76		"93C76"
#define ADDIDATA_S5920		"S5920"
#define ADDIDATA_S5933		"S5933"
#define ADDIDATA_9054		"9054"

/*                         */
#define ADDIDATA_ENABLE		1
#define ADDIDATA_DISABLE	0

/*            */

/*                             */
struct addi_board {
	const char *pc_DriverName;	/*              */
	int i_IorangeBase1;
	int i_PCIEeprom;	/*                        */
	char *pc_EepromChip;	/*               */
	int i_NbrAiChannel;	/*                   */
	int i_NbrAiChannelDiff;	/*                                */
	int i_AiChannelList;	/*                  */
	int i_NbrAoChannel;	/*                   */
	int i_AiMaxdata;	/*                    */
	int i_AoMaxdata;	/*                    */
	const struct comedi_lrange *pr_AiRangelist;	/*                   */
	const struct comedi_lrange *pr_AoRangelist;	/*                   */

	int i_NbrDiChannel;	/*                        */
	int i_NbrDoChannel;	/*                        */
	int i_DoMaxdata;	/*                                */

	int i_NbrTTLChannel;	/*                         */

	int i_Dma;		/*                     */
	int i_Timer;		/*                                   */
	unsigned char b_AvailableConvertUnit;
	unsigned int ui_MinAcquisitiontimeNs;	/*                                   */
	unsigned int ui_MinDelaytimeNs;	/*                             */

	/*                     */
	void (*interrupt)(int irq, void *d);
	int (*reset)(struct comedi_device *);

	/*                     */

	/*              */
	int (*ai_config)(struct comedi_device *, struct comedi_subdevice *,
			 struct comedi_insn *, unsigned int *);
	int (*ai_read)(struct comedi_device *, struct comedi_subdevice *,
		       struct comedi_insn *, unsigned int *);
	int (*ai_write)(struct comedi_device *, struct comedi_subdevice *,
			struct comedi_insn *, unsigned int *);
	int (*ai_bits)(struct comedi_device *, struct comedi_subdevice *,
		       struct comedi_insn *, unsigned int *);
	int (*ai_cmdtest)(struct comedi_device *, struct comedi_subdevice *,
			  struct comedi_cmd *);
	int (*ai_cmd)(struct comedi_device *, struct comedi_subdevice *);
	int (*ai_cancel)(struct comedi_device *, struct comedi_subdevice *);

	/*               */
	int (*ao_config)(struct comedi_device *, struct comedi_subdevice *,
			 struct comedi_insn *, unsigned int *);
	int (*ao_write)(struct comedi_device *, struct comedi_subdevice *,
			struct comedi_insn *, unsigned int *);
	int (*ao_bits)(struct comedi_device *, struct comedi_subdevice *,
		       struct comedi_insn *, unsigned int *);

	/*               */
	int (*di_config)(struct comedi_device *, struct comedi_subdevice *,
			 struct comedi_insn *, unsigned int *);
	int (*di_read)(struct comedi_device *, struct comedi_subdevice *,
		       struct comedi_insn *, unsigned int *);
	int (*di_write)(struct comedi_device *, struct comedi_subdevice *,
			struct comedi_insn *, unsigned int *);
	int (*di_bits)(struct comedi_device *, struct comedi_subdevice *,
		       struct comedi_insn *, unsigned int *);

	/*                */
	int (*do_config)(struct comedi_device *, struct comedi_subdevice *,
			 struct comedi_insn *, unsigned int *);
	int (*do_write)(struct comedi_device *, struct comedi_subdevice *,
			struct comedi_insn *, unsigned int *);
	int (*do_bits)(struct comedi_device *, struct comedi_subdevice *,
		       struct comedi_insn *, unsigned int *);
	int (*do_read)(struct comedi_device *, struct comedi_subdevice *,
		       struct comedi_insn *, unsigned int *);

	/*       */
	int (*timer_config)(struct comedi_device *, struct comedi_subdevice *,
			    struct comedi_insn *, unsigned int *);
	int (*timer_write)(struct comedi_device *, struct comedi_subdevice *,
			   struct comedi_insn *, unsigned int *);
	int (*timer_read)(struct comedi_device *, struct comedi_subdevice *,
			  struct comedi_insn *, unsigned int *);
	int (*timer_bits)(struct comedi_device *, struct comedi_subdevice *,
			  struct comedi_insn *, unsigned int *);

	/*        */
	int (*ttl_config)(struct comedi_device *, struct comedi_subdevice *,
			  struct comedi_insn *, unsigned int *);
	int (*ttl_bits)(struct comedi_device *, struct comedi_subdevice *,
			struct comedi_insn *, unsigned int *);
	int (*ttl_read)(struct comedi_device *, struct comedi_subdevice *,
			struct comedi_insn *, unsigned int *);
	int (*ttl_write)(struct comedi_device *, struct comedi_subdevice *,
			 struct comedi_insn *, unsigned int *);
};

/*                       */

union str_ModuleInfo {
	/*                           */
	struct {
		union {
			struct {
				unsigned char b_ModeRegister1;
				unsigned char b_ModeRegister2;
				unsigned char b_ModeRegister3;
				unsigned char b_ModeRegister4;
			} s_ByteModeRegister;
			unsigned int dw_ModeRegister1_2_3_4;
		} s_ModeRegister;

		struct {
			unsigned int b_IndexInit:1;
			unsigned int b_CounterInit:1;
			unsigned int b_ReferenceInit:1;
			unsigned int b_IndexInterruptOccur:1;
			unsigned int b_CompareLogicInit:1;
			unsigned int b_FrequencyMeasurementInit:1;
			unsigned int b_FrequencyMeasurementEnable:1;
		} s_InitFlag;

	} s_SiemensCounterInfo;

	/*           */
	struct {
		unsigned char b_SSIProfile;
		unsigned char b_PositionTurnLength;
		unsigned char b_TurnCptLength;
		unsigned char b_SSIInit;
	} s_SSICounterInfo;

	/*               */
	struct {
		unsigned char b_TTLInit;
		unsigned char b_PortConfiguration[4];
	} s_TTLIOInfo;

	/*                   */
	struct {
		unsigned char b_DigitalInit;
		unsigned char b_ChannelAMode;
		unsigned char b_ChannelBMode;
		unsigned char b_OutputMemoryEnabled;
		unsigned int dw_OutputMemory;
	} s_DigitalIOInfo;

      /*                   */
	/*                   */
      /*                   */

	struct {
		struct {
			unsigned char b_82X54Init;
			unsigned char b_InputClockSelection;
			unsigned char b_InputClockLevel;
			unsigned char b_OutputLevel;
			unsigned char b_HardwareGateLevel;
			unsigned int dw_ConfigurationWord;
		} s_82X54TimerInfo[3];
		unsigned char b_InterruptMask;
	} s_82X54ModuleInfo;

      /*                   */
	/*                   */
      /*                   */

	struct {
		unsigned char b_ChronoInit;
		unsigned char b_InterruptMask;
		unsigned char b_PCIInputClock;
		unsigned char b_TimingUnit;
		unsigned char b_CycleMode;
		double d_TimingInterval;
		unsigned int dw_ConfigReg;
	} s_ChronoModuleInfo;

      /*                     */
	/*                     */
      /*                     */

	struct {
		struct {
			unsigned char b_PulseEncoderInit;
		} s_PulseEncoderInfo[4];
		unsigned int dw_SetRegister;
		unsigned int dw_ControlRegister;
		unsigned int dw_StatusRegister;
	} s_PulseEncoderModuleInfo;

	/*                  */
	struct {
		struct {
			unsigned char b_TorCounterInit;
			unsigned char b_TimingUnit;
			unsigned char b_InterruptEnable;
			double d_TimingInterval;
			unsigned int ul_RealTimingInterval;
		} s_TorCounterInfo[2];
		unsigned char b_PCIInputClock;
	} s_TorCounterModuleInfo;

	/*           */
	struct {
		struct {
			unsigned char b_PWMInit;
			unsigned char b_TimingUnit;
			unsigned char b_InterruptEnable;
			double d_LowTiming;
			double d_HighTiming;
			unsigned int ul_RealLowTiming;
			unsigned int ul_RealHighTiming;
		} s_PWMInfo[2];
		unsigned char b_ClockSelection;
	} s_PWMModuleInfo;

	/*           */
	struct {
		struct {
			unsigned char b_ETMEnable;
			unsigned char b_ETMInterrupt;
		} s_ETMInfo[2];
		unsigned char b_ETMInit;
		unsigned char b_TimingUnit;
		unsigned char b_ClockSelection;
		double d_TimingInterval;
		unsigned int ul_Timing;
	} s_ETMModuleInfo;

	/*           */
	struct {
		unsigned char b_CDAEnable;
		unsigned char b_CDAInterrupt;
		unsigned char b_CDAInit;
		unsigned char b_FctSelection;
		unsigned char b_CDAReadFIFOOverflow;
	} s_CDAModuleInfo;

};

/*                                                */
struct addi_private {

	int iobase;
	int i_IobaseAmcc;	/*                          */
	int i_IobaseAddon;	/*                    */
	int i_IobaseReserved;
	void __iomem *dw_AiBase;
	unsigned char b_AiContinuous;	/*                     */
	unsigned char b_AiInitialisation;
	unsigned int ui_AiActualScan;	/*                            */
	unsigned int ui_AiBufferPtr;	/*                             */
	unsigned int ui_AiNbrofChannels;	/*                                */
	unsigned int ui_AiScanLength;	/*                            */
	unsigned int ui_AiActualScanPosition;	/*                          */
	unsigned int *pui_AiChannelList;	/*                  */
	unsigned int ui_AiChannelList[32];	/*                  */
	unsigned char b_AiChannelConfiguration[32];	/*                  */
	unsigned int ui_AiReadData[32];
	unsigned int dw_AiInitialised;
	unsigned int ui_AiTimer0;	/*                           */
	unsigned int ui_AiTimer1;	/*                           */
	unsigned int ui_AiFlags;
	unsigned int ui_AiDataLength;
	short *AiData;	/*                         */
	unsigned int ui_AiNbrofScans;	/*                        */
	unsigned short us_UseDma;	/*                    */
	unsigned char b_DmaDoubleBuffer;	/*                              */
	unsigned int ui_DmaActualBuffer;	/*                           */
	/*                       */
	/*                                                                                    */
	short *ul_DmaBufferVirtual[2];	/*                                  */
	unsigned int ul_DmaBufferHw[2];	/*                         */
	unsigned int ui_DmaBufferSize[2];	/*                              */
	unsigned int ui_DmaBufferUsesize[2];	/*                                          */
	unsigned int ui_DmaBufferSamples[2];	/*                  */
	unsigned int ui_DmaBufferPages[2];	/*                            */
	unsigned char b_DigitalOutputRegister;	/*                          */
	unsigned char b_OutputMemoryStatus;
	unsigned char b_AnalogInputChannelNbr;	/*                           */
	unsigned char b_AnalogOutputChannelNbr;	/*                           */
	unsigned char b_TimerSelectMode;	/*                                      */
	unsigned char b_ModeSelectRegister;	/*                                      */
	unsigned short us_OutputRegister;	/*                                     */
	unsigned char b_InterruptState;
	unsigned char b_TimerInit;	/*                                        */
	unsigned char b_TimerStarted;	/*                                       */
	unsigned char b_Timer2Mode;	/*                           */
	unsigned char b_Timer2Interrupt;	/*                                     */
	unsigned char b_AiCyclicAcquisition;	/*                              */
	unsigned char b_InterruptMode;	/*                 */
	unsigned char b_EocEosInterrupt;	/*                                   */
	unsigned int ui_EocEosConversionTime;
	unsigned char b_EocEosConversionTimeBase;
	unsigned char b_SingelDiff;
	unsigned char b_ExttrigEnable;	/*                                       */

	/*                                */
	struct task_struct *tsk_Current;

	/*                               */
	struct {
		unsigned int ui_Address;	/*               */
		unsigned int ui_FlashAddress;
		unsigned char b_InterruptNbr;	/*                        */
		unsigned char b_SlotNumber;	/*                 */
		unsigned char b_BoardVersion;
		unsigned int dw_MolduleConfiguration[4];	/*               */
	} s_BoardInfos;

	/*                 */
	struct {
		unsigned int ul_InterruptOccur;	/*                          */
						/*                       */
		unsigned int ui_Read;	/*           */
		unsigned int ui_Write;	/*            */
		struct {
			unsigned char b_OldModuleMask;
			unsigned int ul_OldInterruptMask;	/*                */
			unsigned int ul_OldCounterLatchValue;	/*                         */
		} s_FIFOInterruptParameters[APCI1710_SAVE_INTERRUPT];
	} s_InterruptParameters;

	union str_ModuleInfo s_ModuleInfo[4];
	unsigned int ul_TTLPortConfiguration[10];

	/*                                                          */
	struct {
		int i_NbrAiChannel;	/*                   */
		int i_NbrAoChannel;	/*                   */
		int i_AiMaxdata;	/*                    */
		int i_AoMaxdata;	/*                    */
		int i_NbrDiChannel;	/*                        */
		int i_NbrDoChannel;	/*                        */
		int i_DoMaxdata;	/*                                */
		int i_Dma;		/*                     */
		int i_Timer;		/*                                 */
		unsigned int ui_MinAcquisitiontimeNs;
					/*                                   */
		unsigned int ui_MinDelaytimeNs;
					/*                             */
	} s_EeParameters;
};
