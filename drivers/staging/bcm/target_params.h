#ifndef TARGET_PARAMS_H
#define TARGET_PARAMS_H

struct bcm_target_params {
	u32 m_u32CfgVersion;
	u32 m_u32CenterFrequency;
	u32 m_u32BandAScan;
	u32 m_u32BandBScan;
	u32 m_u32BandCScan;
	u32 m_u32ErtpsOptions;
	u32 m_u32PHSEnable;
	u32 m_u32HoEnable;
	u32 m_u32HoReserved1;
	u32 m_u32HoReserved2;
	u32 m_u32MimoEnable;
	u32 m_u32SecurityEnable;
	u32 m_u32PowerSavingModesEnable; /*                                                    */
	/*                          
                                                  
                                             
                                              
  */
	u32 m_u32PowerSavingModeOptions;
	u32 m_u32ArqEnable;
	/*                                                      */
	u32 m_u32HarqEnable;
	u32 m_u32EEPROMFlag;
	/*                                                                                 
                   
  */
	u32 m_u32Customize;
	u32 m_u32ConfigBW;  /*       */
	u32 m_u32ShutDownInitThresholdTimer;
	u32 m_u32RadioParameter;
	u32 m_u32PhyParameter1;
	u32 m_u32PhyParameter2;
	u32 m_u32PhyParameter3;
	u32 m_u32TestOptions; /*                                                                                                                                                                       */
	u32 m_u32MaxMACDataperDLFrame;
	u32 m_u32MaxMACDataperULFrame;
	u32 m_u32Corr2MacFlags;
	u32 HostDrvrConfig1;
	u32 HostDrvrConfig2;
	u32 HostDrvrConfig3;
	u32 HostDrvrConfig4;
	u32 HostDrvrConfig5;
	u32 HostDrvrConfig6;
	u32 m_u32SegmentedPUSCenable;
	/*                                                                                    
                           
                                                                                      
                                                                              
  */
	u32 m_u32BandAMCEnable;
};

#endif
