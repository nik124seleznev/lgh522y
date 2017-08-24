/*
 * Copyright 2012 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef RADEON_ACPI_H
#define RADEON_ACPI_H

struct radeon_device;
struct acpi_bus_event;

int radeon_atif_handler(struct radeon_device *rdev,
		struct acpi_bus_event *event);

/*                                       
          
                                     
                                                  
                                                 
                                                                              
                                                                          
                                                                            
                                                                               
                                                                                
                                                                          
                                                                                
                                                                       
                          
  
          
                                     
                                                  
                                                 
                                                                           
                              
  
          
                                                
                                                             
                                      
                                                                         
                                          
  
          
                                     
                                                  
                                                 
                                                                    
  
 */
/*      */
#define ATIF_FUNCTION_VERIFY_INTERFACE                             0x0
/*                                     
             
          
                                                        
                  
                                       
                                         
 */
/*                    */
#       define ATIF_DISPLAY_SWITCH_REQUEST_SUPPORTED               (1 << 0)
#       define ATIF_EXPANSION_MODE_CHANGE_REQUEST_SUPPORTED        (1 << 1)
#       define ATIF_THERMAL_STATE_CHANGE_REQUEST_SUPPORTED         (1 << 2)
#       define ATIF_FORCED_POWER_STATE_CHANGE_REQUEST_SUPPORTED    (1 << 3)
#       define ATIF_SYSTEM_POWER_SOURCE_CHANGE_REQUEST_SUPPORTED   (1 << 4)
#       define ATIF_DISPLAY_CONF_CHANGE_REQUEST_SUPPORTED          (1 << 5)
#       define ATIF_PX_GFX_SWITCH_REQUEST_SUPPORTED                (1 << 6)
#       define ATIF_PANEL_BRIGHTNESS_CHANGE_REQUEST_SUPPORTED      (1 << 7)
#       define ATIF_DGPU_DISPLAY_EVENT_SUPPORTED                   (1 << 8)
/*                            */
#       define ATIF_GET_SYSTEM_PARAMETERS_SUPPORTED               (1 << 0)
#       define ATIF_GET_SYSTEM_BIOS_REQUESTS_SUPPORTED            (1 << 1)
#       define ATIF_SELECT_ACTIVE_DISPLAYS_SUPPORTED              (1 << 2)
#       define ATIF_GET_LID_STATE_SUPPORTED                       (1 << 3)
#       define ATIF_GET_TV_STANDARD_FROM_CMOS_SUPPORTED           (1 << 4)
#       define ATIF_SET_TV_STANDARD_IN_CMOS_SUPPORTED             (1 << 5)
#       define ATIF_GET_PANEL_EXPANSION_MODE_FROM_CMOS_SUPPORTED  (1 << 6)
#       define ATIF_SET_PANEL_EXPANSION_MODE_IN_CMOS_SUPPORTED    (1 << 7)
#       define ATIF_TEMPERATURE_CHANGE_NOTIFICATION_SUPPORTED     (1 << 12)
#       define ATIF_GET_GRAPHICS_DEVICE_TYPES_SUPPORTED           (1 << 14)
#define ATIF_FUNCTION_GET_SYSTEM_PARAMETERS                        0x1
/*                                          
             
          
                                                        
                           
                
  
     
  
                                                        
                           
                
                              
  
        
            
                                                     
                                                 
                                                    
                                                     
         
                                            
 */
#define ATIF_FUNCTION_GET_SYSTEM_BIOS_REQUESTS                     0x2
/*                                             
             
          
                                                        
                                 
                               
                                               
                                                                
                                                    
                                       
                              
                                        
 */
/*                        */
#       define ATIF_DISPLAY_SWITCH_REQUEST                         (1 << 0)
#       define ATIF_EXPANSION_MODE_CHANGE_REQUEST                  (1 << 1)
#       define ATIF_THERMAL_STATE_CHANGE_REQUEST                   (1 << 2)
#       define ATIF_FORCED_POWER_STATE_CHANGE_REQUEST              (1 << 3)
#       define ATIF_SYSTEM_POWER_SOURCE_CHANGE_REQUEST             (1 << 4)
#       define ATIF_DISPLAY_CONF_CHANGE_REQUEST                    (1 << 5)
#       define ATIF_PX_GFX_SWITCH_REQUEST                          (1 << 6)
#       define ATIF_PANEL_BRIGHTNESS_CHANGE_REQUEST                (1 << 7)
#       define ATIF_DGPU_DISPLAY_EVENT                             (1 << 8)
/*                      */
#       define ATIF_PANEL_EXPANSION_DISABLE                        0
#       define ATIF_PANEL_EXPANSION_FULL                           1
#       define ATIF_PANEL_EXPANSION_ASPECT                         2
/*                       */
#       define ATIF_TARGET_GFX_SINGLE                              0
#       define ATIF_TARGET_GFX_PX_IGPU                             1
#       define ATIF_TARGET_GFX_PX_DGPU                             2
/*                     */
#       define ATIF_POWER_SOURCE_AC                                1
#       define ATIF_POWER_SOURCE_DC                                2
#       define ATIF_POWER_SOURCE_RESTRICTED_AC_1                   3
#       define ATIF_POWER_SOURCE_RESTRICTED_AC_2                   4
#define ATIF_FUNCTION_SELECT_ACTIVE_DISPLAYS                       0x3
/*                                           
        
                                                        
                            
                             
          
                                                        
                            
 */
#       define ATIF_LCD1                                           (1 << 0)
#       define ATIF_CRT1                                           (1 << 1)
#       define ATIF_TV                                             (1 << 2)
#       define ATIF_DFP1                                           (1 << 3)
#       define ATIF_CRT2                                           (1 << 4)
#       define ATIF_LCD2                                           (1 << 5)
#       define ATIF_DFP2                                           (1 << 7)
#       define ATIF_CV                                             (1 << 8)
#       define ATIF_DFP3                                           (1 << 9)
#       define ATIF_DFP4                                           (1 << 10)
#       define ATIF_DFP5                                           (1 << 11)
#       define ATIF_DFP6                                           (1 << 12)
#define ATIF_FUNCTION_GET_LID_STATE                                0x4
/*                                  
             
          
                                                        
                                         
  
                                                               
                                         
 */
#define ATIF_FUNCTION_GET_TV_STANDARD_FROM_CMOS                    0x5
/*                                              
             
          
                                                        
            
                      
 */
#       define ATIF_TV_STD_NTSC                                    0
#       define ATIF_TV_STD_PAL                                     1
#       define ATIF_TV_STD_PALM                                    2
#       define ATIF_TV_STD_PAL60                                   3
#       define ATIF_TV_STD_NTSCJ                                   4
#       define ATIF_TV_STD_PALCN                                   5
#       define ATIF_TV_STD_PALN                                    6
#       define ATIF_TV_STD_SCART_RGB                               9
#define ATIF_FUNCTION_SET_TV_STANDARD_IN_CMOS                      0x6
/*                                            
        
                                                        
            
                      
               
 */
#define ATIF_FUNCTION_GET_PANEL_EXPANSION_MODE_FROM_CMOS           0x7
/*                                                       
             
          
                                                        
                               
 */
#define ATIF_FUNCTION_SET_PANEL_EXPANSION_MODE_IN_CMOS             0x8
/*                                                     
        
                                                        
                               
               
 */
#define ATIF_FUNCTION_TEMPERATURE_CHANGE_NOTIFICATION              0xD
/*                                                    
        
                                                        
                            
                                                
               
 */
#define ATIF_FUNCTION_GET_GRAPHICS_DEVICE_TYPES                    0xF
/*                                              
             
          
                                
                                                                      
                          
                                              
                          
 */
/*       */
#       define ATIF_PX_REMOVABLE_GRAPHICS_DEVICE                   (1 << 0)
#       define ATIF_XGP_PORT                                       (1 << 1)
#       define ATIF_VGA_ENABLED_GRAPHICS_DEVICE                    (1 << 2)
#       define ATIF_XGP_PORT_IN_DOCK                               (1 << 3)

/*      */
#define ATPX_FUNCTION_VERIFY_INTERFACE                             0x0
/*                                     
             
          
                                                        
                  
                                         
 */
/*                            */
#       define ATPX_GET_PX_PARAMETERS_SUPPORTED                    (1 << 0)
#       define ATPX_POWER_CONTROL_SUPPORTED                        (1 << 1)
#       define ATPX_DISPLAY_MUX_CONTROL_SUPPORTED                  (1 << 2)
#       define ATPX_I2C_MUX_CONTROL_SUPPORTED                      (1 << 3)
#       define ATPX_GRAPHICS_DEVICE_SWITCH_START_NOTIFICATION_SUPPORTED (1 << 4)
#       define ATPX_GRAPHICS_DEVICE_SWITCH_END_NOTIFICATION_SUPPORTED   (1 << 5)
#       define ATPX_GET_DISPLAY_CONNECTORS_MAPPING_SUPPORTED       (1 << 7)
#       define ATPX_GET_DISPLAY_DETECTION_PORTS_SUPPORTED          (1 << 8)
#define ATPX_FUNCTION_GET_PX_PARAMETERS                            0x1
/*                                      
             
          
                                                        
                           
                
 */
/*       */
#       define ATPX_LVDS_I2C_AVAILABLE_TO_BOTH_GPUS                (1 << 0)
#       define ATPX_CRT1_I2C_AVAILABLE_TO_BOTH_GPUS                (1 << 1)
#       define ATPX_DVI1_I2C_AVAILABLE_TO_BOTH_GPUS                (1 << 2)
#       define ATPX_CRT1_RGB_SIGNAL_MUXED                          (1 << 3)
#       define ATPX_TV_SIGNAL_MUXED                                (1 << 4)
#       define ATPX_DFP_SIGNAL_MUXED                               (1 << 5)
#       define ATPX_SEPARATE_MUX_FOR_I2C                           (1 << 6)
#       define ATPX_DYNAMIC_PX_SUPPORTED                           (1 << 7)
#       define ATPX_ACF_NOT_SUPPORTED                              (1 << 8)
#       define ATPX_FIXED_NOT_SUPPORTED                            (1 << 9)
#       define ATPX_DYNAMIC_DGPU_POWER_OFF_SUPPORTED               (1 << 10)
#       define ATPX_DGPU_REQ_POWER_FOR_DISPLAYS                    (1 << 11)
#define ATPX_FUNCTION_POWER_CONTROL                                0x2
/*                                  
        
                                                        
                                                       
               
 */
#define ATPX_FUNCTION_DISPLAY_MUX_CONTROL                          0x3
/*                                        
        
                                                        
                                                 
               
 */
#       define ATPX_INTEGRATED_GPU                                 0
#       define ATPX_DISCRETE_GPU                                   1
#define ATPX_FUNCTION_I2C_MUX_CONTROL                              0x4
/*                                    
        
                                                        
                                                     
               
 */
#define ATPX_FUNCTION_GRAPHICS_DEVICE_SWITCH_START_NOTIFICATION    0x5
/*                                                              
        
                                                        
                                        
               
 */
#define ATPX_FUNCTION_GRAPHICS_DEVICE_SWITCH_END_NOTIFICATION      0x6
/*                                                            
        
                                                        
                                        
               
 */
#define ATPX_FUNCTION_GET_DISPLAY_CONNECTORS_MAPPING               0x8
/*                                                   
             
          
                                       
                                                                            
                                                                      
                                                                                
                                                                                 
                                                                      
 */
/*       */
#       define ATPX_DISPLAY_OUTPUT_SUPPORTED_BY_ADAPTER_ID_DEVICE  (1 << 0)
#       define ATPX_DISPLAY_HPD_SUPPORTED_BY_ADAPTER_ID_DEVICE     (1 << 1)
#       define ATPX_DISPLAY_I2C_SUPPORTED_BY_ADAPTER_ID_DEVICE     (1 << 2)
#define ATPX_FUNCTION_GET_DISPLAY_DETECTION_PORTS                  0x9
/*                                                
             
          
                                  
                                                                  
                                             
                                                                  
                                             
  
                                
 */
/*        */
#       define ATPX_HPD_NONE                                       0
#       define ATPX_HPD1                                           1
#       define ATPX_HPD2                                           2
#       define ATPX_HPD3                                           3
#       define ATPX_HPD4                                           4
#       define ATPX_HPD5                                           5
#       define ATPX_HPD6                                           6
/*        */
#       define ATPX_DDC_NONE                                       0
#       define ATPX_DDC1                                           1
#       define ATPX_DDC2                                           2
#       define ATPX_DDC3                                           3
#       define ATPX_DDC4                                           4
#       define ATPX_DDC5                                           5
#       define ATPX_DDC6                                           6
#       define ATPX_DDC7                                           7
#       define ATPX_DDC8                                           8

/*      */
#define ATCS_FUNCTION_VERIFY_INTERFACE                             0x0
/*                                     
             
          
                                                        
                  
                                         
 */
/*                            */
#       define ATCS_GET_EXTERNAL_STATE_SUPPORTED                   (1 << 0)
#       define ATCS_PCIE_PERFORMANCE_REQUEST_SUPPORTED             (1 << 1)
#       define ATCS_PCIE_DEVICE_READY_NOTIFICATION_SUPPORTED       (1 << 2)
#       define ATCS_SET_PCIE_BUS_WIDTH_SUPPORTED                   (1 << 3)
#define ATCS_FUNCTION_GET_EXTERNAL_STATE                           0x1
/*                                       
             
          
                                                        
                           
                                         
 */
/*       */
#       define ATCS_DOCKED                                         (1 << 0)
#define ATCS_FUNCTION_PCIE_PERFORMANCE_REQUEST                     0x2
/*                                             
        
                                                        
                                                                     
                           
                
                       
                              
          
                                                        
                       
 */
/*       */
#       define ATCS_ADVERTISE_CAPS                                 (1 << 0)
#       define ATCS_WAIT_FOR_COMPLETION                            (1 << 1)
/*              */
#       define ATCS_PCIE_LINK_SPEED                                1
/*                     */
#       define ATCS_REMOVE                                         0
#       define ATCS_FORCE_LOW_POWER                                1
#       define ATCS_PERF_LEVEL_1                                   2 /*            */
#       define ATCS_PERF_LEVEL_2                                   3 /*            */
#       define ATCS_PERF_LEVEL_3                                   4 /*            */
/*              */
#       define ATCS_REQUEST_REFUSED                                1
#       define ATCS_REQUEST_COMPLETE                               2
#       define ATCS_REQUEST_IN_PROGRESS                            3
#define ATCS_FUNCTION_PCIE_DEVICE_READY_NOTIFICATION               0x3
/*                                                   
             
               
 */
#define ATCS_FUNCTION_SET_PCIE_BUS_WIDTH                           0x4
/*                                       
        
                                                        
                                                                     
                                 
          
                                                        
                                 
 */

#endif
