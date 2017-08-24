/*
 * Host communication command constants for ChromeOS EC
 *
 * Copyright (C) 2012 Google, Inc
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * The ChromeOS EC multi function device is used to mux all the requests
 * to the EC device for its multiple features: keyboard controller,
 * battery charging and regulator control, firmware update.
 *
 * NOTE: This file is copied verbatim from the ChromeOS EC Open Source
 * project in an attempt to make future updates easy to make.
 */

#ifndef __CROS_EC_COMMANDS_H
#define __CROS_EC_COMMANDS_H

/*
                    
  
                                      
                                      
  
                                          
                                                                 
                                                               
                                     
                                                          
                                                             
                                                                  
  
                                                                       
                                                                       
                                                           
 */

/*                                  */
#define EC_PROTO_VERSION          0x00000002

/*                      */
#define EC_VER_MASK(version) (1UL << (version))

/*                                 */
#define EC_LPC_ADDR_ACPI_DATA  0x62
#define EC_LPC_ADDR_ACPI_CMD   0x66

/*                                */
#define EC_LPC_ADDR_HOST_DATA  0x200
#define EC_LPC_ADDR_HOST_CMD   0x204

/*                                                */
#define EC_LPC_ADDR_HOST_ARGS  0x800
#define EC_LPC_ADDR_HOST_PARAM 0x804
#define EC_HOST_PARAM_SIZE     0x0fc  /*                             */

/*                                                      */
#define EC_LPC_ADDR_OLD_PARAM  0x880
#define EC_OLD_PARAM_SIZE      0x080  /*                             */

/*                                   */
#define EC_LPC_CMDR_DATA	(1 << 0)  /*                             */
#define EC_LPC_CMDR_PENDING	(1 << 1)  /*                     */
#define EC_LPC_CMDR_BUSY	(1 << 2)  /*                                 */
#define EC_LPC_CMDR_CMD		(1 << 3)  /*                               */
#define EC_LPC_CMDR_ACPI_BRST	(1 << 4)  /*                       */
#define EC_LPC_CMDR_SCI		(1 << 5)  /*                      */
#define EC_LPC_CMDR_SMI		(1 << 6)  /*                      */

#define EC_LPC_ADDR_MEMMAP       0x900
#define EC_MEMMAP_SIZE         255 /*                                 */
#define EC_MEMMAP_TEXT_MAX     8   /*                                    */

/*                                                           */
#define EC_MEMMAP_TEMP_SENSOR      0x00 /*              */
#define EC_MEMMAP_FAN              0x10 /*            */
#define EC_MEMMAP_TEMP_SENSOR_B    0x18 /*                           */
#define EC_MEMMAP_ID               0x20 /*         */
#define EC_MEMMAP_ID_VERSION       0x22 /*                                */
#define EC_MEMMAP_THERMAL_VERSION  0x23 /*                                */
#define EC_MEMMAP_BATTERY_VERSION  0x24 /*                                */
#define EC_MEMMAP_SWITCHES_VERSION 0x25 /*                                */
#define EC_MEMMAP_EVENTS_VERSION   0x26 /*                                */
#define EC_MEMMAP_HOST_CMD_FLAGS   0x27 /*                              */
#define EC_MEMMAP_SWITCHES         0x30
#define EC_MEMMAP_HOST_EVENTS      0x34
#define EC_MEMMAP_BATT_VOLT        0x40 /*                         */
#define EC_MEMMAP_BATT_RATE        0x44 /*                      */
#define EC_MEMMAP_BATT_CAP         0x48 /*                            */
#define EC_MEMMAP_BATT_FLAG        0x4c /*                              */
#define EC_MEMMAP_BATT_DCAP        0x50 /*                         */
#define EC_MEMMAP_BATT_DVLT        0x54 /*                        */
#define EC_MEMMAP_BATT_LFCC        0x58 /*                                   */
#define EC_MEMMAP_BATT_CCNT        0x5c /*                     */
#define EC_MEMMAP_BATT_MFGR        0x60 /*                             */
#define EC_MEMMAP_BATT_MODEL       0x68 /*                             */
#define EC_MEMMAP_BATT_SERIAL      0x70 /*                              */
#define EC_MEMMAP_BATT_TYPE        0x78 /*                     */

/*                                                 */
#define EC_TEMP_SENSOR_ENTRIES     16
/*
                                                     
  
                                                        
 */
#define EC_TEMP_SENSOR_B_ENTRIES      8
#define EC_TEMP_SENSOR_NOT_PRESENT    0xff
#define EC_TEMP_SENSOR_ERROR          0xfe
#define EC_TEMP_SENSOR_NOT_POWERED    0xfd
#define EC_TEMP_SENSOR_NOT_CALIBRATED 0xfc
/*
                                                                        
                                                                
 */
#define EC_TEMP_SENSOR_OFFSET      200

#define EC_FAN_SPEED_ENTRIES       4       /*                                 */
#define EC_FAN_SPEED_NOT_PRESENT   0xffff  /*                   */
#define EC_FAN_SPEED_STALLED       0xfffe  /*             */

/*                                           */
#define EC_BATT_FLAG_AC_PRESENT   0x01
#define EC_BATT_FLAG_BATT_PRESENT 0x02
#define EC_BATT_FLAG_DISCHARGING  0x04
#define EC_BATT_FLAG_CHARGING     0x08
#define EC_BATT_FLAG_LEVEL_CRITICAL 0x10

/*                                    */
#define EC_SWITCH_LID_OPEN               0x01
#define EC_SWITCH_POWER_BUTTON_PRESSED   0x02
#define EC_SWITCH_WRITE_PROTECT_DISABLED 0x04
/*                                 */
#define EC_SWITCH_KEYBOARD_RECOVERY      0x08
/*                                                            */
#define EC_SWITCH_DEDICATED_RECOVERY     0x10
/*                                                                       */
#define EC_SWITCH_IGNORE0                0x20

/*                              */
/*                                                               */
#define EC_HOST_CMD_FLAG_LPC_ARGS_SUPPORTED  0x01

/*                       */
#define EC_WIRELESS_SWITCH_WLAN      0x01
#define EC_WIRELESS_SWITCH_BLUETOOTH 0x02

/*
                                                                               
                                                                         
                                            
 */
#ifndef __ACPI__

/*                               */
/*                                                                        */
#define EC_LPC_STATUS_TO_HOST     0x01
/*                                                                    */
#define EC_LPC_STATUS_FROM_HOST   0x02
/*                            */
#define EC_LPC_STATUS_PROCESSING  0x04
/*                                          */
#define EC_LPC_STATUS_LAST_CMD    0x08
/*                                                                          */
#define EC_LPC_STATUS_BURST_MODE  0x10
/*                                             */
#define EC_LPC_STATUS_SCI_PENDING 0x20
/*                                             */
#define EC_LPC_STATUS_SMI_PENDING 0x40
/*            */
#define EC_LPC_STATUS_RESERVED    0x80

/*
                                                                              
                                                            
 */
#define EC_LPC_STATUS_BUSY_MASK \
	(EC_LPC_STATUS_FROM_HOST | EC_LPC_STATUS_PROCESSING)

/*                             */
enum ec_status {
	EC_RES_SUCCESS = 0,
	EC_RES_INVALID_COMMAND = 1,
	EC_RES_ERROR = 2,
	EC_RES_INVALID_PARAM = 3,
	EC_RES_ACCESS_DENIED = 4,
	EC_RES_INVALID_RESPONSE = 5,
	EC_RES_INVALID_VERSION = 6,
	EC_RES_INVALID_CHECKSUM = 7,
	EC_RES_IN_PROGRESS = 8,		/*                               */
	EC_RES_UNAVAILABLE = 9,		/*                       */
	EC_RES_TIMEOUT = 10,		/*                  */
	EC_RES_OVERFLOW = 11,		/*                       */
};

/*
                                                                             
                                                                            
                                                                             
                                                                          
                                           
 */
enum host_event_code {
	EC_HOST_EVENT_LID_CLOSED = 1,
	EC_HOST_EVENT_LID_OPEN = 2,
	EC_HOST_EVENT_POWER_BUTTON = 3,
	EC_HOST_EVENT_AC_CONNECTED = 4,
	EC_HOST_EVENT_AC_DISCONNECTED = 5,
	EC_HOST_EVENT_BATTERY_LOW = 6,
	EC_HOST_EVENT_BATTERY_CRITICAL = 7,
	EC_HOST_EVENT_BATTERY = 8,
	EC_HOST_EVENT_THERMAL_THRESHOLD = 9,
	EC_HOST_EVENT_THERMAL_OVERLOAD = 10,
	EC_HOST_EVENT_THERMAL = 11,
	EC_HOST_EVENT_USB_CHARGER = 12,
	EC_HOST_EVENT_KEY_PRESSED = 13,
	/*
                                                                        
                                                                  
                                                                 
  */
	EC_HOST_EVENT_INTERFACE_READY = 14,
	/*                                          */
	EC_HOST_EVENT_KEYBOARD_RECOVERY = 15,

	/*                                  */
	EC_HOST_EVENT_THERMAL_SHUTDOWN = 16,
	/*                                       */
	EC_HOST_EVENT_BATTERY_SHUTDOWN = 17,

	/*
                                                                        
                                                              
                                                                     
                                                                       
                                                                    
  */
	EC_HOST_EVENT_INVALID = 32
};
/*                 */
#define EC_HOST_EVENT_MASK(event_code) (1UL << ((event_code) - 1))

/*                                    */
struct ec_lpc_host_args {
	uint8_t flags;
	uint8_t command_version;
	uint8_t data_size;
	/*
                                                                    
                                   
  */
	uint8_t checksum;
} __packed;

/*                                  */
/*
                                                                            
          
  
                                                                               
                                                                              
                                                                        
                                               
 */
#define EC_HOST_ARGS_FLAG_FROM_HOST 0x01
/*
                                                                            
  
                                                                             
                                                                  
                                             
 */
#define EC_HOST_ARGS_FLAG_TO_HOST   0x02

/*
                     
  
                                                                          
                                                                       
                                                                      
                                                                         
                                                                             
                                                           
 */

/*                                                                           */
/*                         */

/*
                                                                           
           
 */
#define EC_CMD_PROTO_VERSION 0x00

struct ec_response_proto_version {
	uint32_t version;
} __packed;

/*
                                                                   
            
 */
#define EC_CMD_HELLO 0x01

struct ec_params_hello {
	uint32_t in_data;  /*                    */
} __packed;

struct ec_response_hello {
	uint32_t out_data;  /*                                     */
} __packed;

/*                    */
#define EC_CMD_GET_VERSION 0x02

enum ec_current_image {
	EC_IMAGE_UNKNOWN = 0,
	EC_IMAGE_RO,
	EC_IMAGE_RW
};

struct ec_response_get_version {
	/*                                            */
	char version_string_ro[32];
	char version_string_rw[32];
	char reserved[32];       /*                            */
	uint32_t current_image;  /*                         */
} __packed;

/*           */
#define EC_CMD_READ_TEST 0x03

struct ec_params_read_test {
	uint32_t offset;   /*                                */
	uint32_t size;     /*                       */
} __packed;

struct ec_response_read_test {
	uint32_t data[32];
} __packed;

/*
                        
  
                                      
 */
#define EC_CMD_GET_BUILD_INFO 0x04

/*               */
#define EC_CMD_GET_CHIP_INFO 0x05

struct ec_response_get_chip_info {
	/*                         */
	char vendor[32];
	char name[32];
	char revision[32];  /*              */
} __packed;

/*                      */
#define EC_CMD_GET_BOARD_VERSION 0x06

struct ec_response_board_version {
	uint16_t board_version;  /*                                     */
} __packed;

/*
                           
  
                                                                         
                                                            
  
                                         
 */
#define EC_CMD_READ_MEMMAP 0x07

struct ec_params_read_memmap {
	uint8_t offset;   /*                                */
	uint8_t size;     /*                       */
} __packed;

/*                                       */
#define EC_CMD_GET_CMD_VERSIONS 0x08

struct ec_params_get_cmd_versions {
	uint8_t cmd;      /*                  */
} __packed;

struct ec_response_get_cmd_versions {
	/*
                                                                   
                    
  */
	uint32_t version_mask;
} __packed;

/*
                                                                          
                                                          
  
                                                                         
                                                                    
 */
#define EC_CMD_GET_COMMS_STATUS		0x09

/*                                                  */
enum ec_comms_status {
	EC_COMMS_STATUS_PROCESSING	= 1 << 0,	/*                */
};

struct ec_response_get_comms_status {
	uint32_t flags;		/*                              */
} __packed;


/*                                                                           */
/*                */

/*                */
#define EC_CMD_FLASH_INFO 0x10

struct ec_response_flash_info {
	/*                             */
	uint32_t flash_size;
	/*
                                                               
            
  */
	uint32_t write_block_size;
	/*
                                                               
            
  */
	uint32_t erase_block_size;
	/*
                                                                
                     
  */
	uint32_t protect_block_size;
} __packed;

/*
             
  
                                         
 */
#define EC_CMD_FLASH_READ 0x11

struct ec_params_flash_read {
	uint32_t offset;   /*                     */
	uint32_t size;     /*                       */
} __packed;

/*             */
#define EC_CMD_FLASH_WRITE 0x12

struct ec_params_flash_write {
	uint32_t offset;   /*                      */
	uint32_t size;     /*                        */
	/*
                                                                      
                                            
  */
	uint8_t data[64];
} __packed;

/*             */
#define EC_CMD_FLASH_ERASE 0x13

struct ec_params_flash_erase {
	uint32_t offset;   /*                      */
	uint32_t size;     /*                        */
} __packed;

/*
                            
  
                                                                        
                                                                           
                                                                        
                                                     
  
                                                     
 */
#define EC_CMD_FLASH_PROTECT 0x15
#define EC_VER_FLASH_PROTECT 1  /*                   */

/*                            */
/*                                           */
#define EC_FLASH_PROTECT_RO_AT_BOOT         (1 << 0)
/*
                                                                          
              
 */
#define EC_FLASH_PROTECT_RO_NOW             (1 << 1)
/*                                                */
#define EC_FLASH_PROTECT_ALL_NOW            (1 << 2)
/*                                          */
#define EC_FLASH_PROTECT_GPIO_ASSERTED      (1 << 3)
/*                                                                            */
#define EC_FLASH_PROTECT_ERROR_STUCK        (1 << 4)
/*
                                                                           
                                                                      
                                                                     
 */
#define EC_FLASH_PROTECT_ERROR_INCONSISTENT (1 << 5)
/*                                               */
#define EC_FLASH_PROTECT_ALL_AT_BOOT        (1 << 6)

struct ec_params_flash_protect {
	uint32_t mask;   /*                        */
	uint32_t flags;  /*                    */
} __packed;

struct ec_response_flash_protect {
	/*                                      */
	uint32_t flags;
	/*
                                                                   
                                                                       
                            
  */
	uint32_t valid_flags;
	/*                                                               */
	uint32_t writable_flags;
} __packed;

/*
                                                                          
                                                                 
 */

/*                            */
#define EC_CMD_FLASH_REGION_INFO 0x16
#define EC_VER_FLASH_REGION_INFO 1

enum ec_flash_region {
	/*                                       */
	EC_FLASH_REGION_RO,
	/*                                        */
	EC_FLASH_REGION_RW,
	/*
                                                                        
                       
  */
	EC_FLASH_REGION_WP_RO,
};

struct ec_params_flash_region_info {
	uint32_t region;  /*                      */
} __packed;

struct ec_response_flash_region_info {
	uint32_t offset;
	uint32_t size;
} __packed;

/*                        */
#define EC_CMD_VBNV_CONTEXT 0x17
#define EC_VER_VBNV_CONTEXT 1
#define EC_VBNV_BLOCK_SIZE 16

enum ec_vbnvcontext_op {
	EC_VBNV_CONTEXT_OP_READ,
	EC_VBNV_CONTEXT_OP_WRITE,
};

struct ec_params_vbnvcontext {
	uint32_t op;
	uint8_t block[EC_VBNV_BLOCK_SIZE];
} __packed;

struct ec_response_vbnvcontext {
	uint8_t block[EC_VBNV_BLOCK_SIZE];
} __packed;

/*                                                                           */
/*              */

/*                    */
#define EC_CMD_PWM_GET_FAN_TARGET_RPM 0x20

struct ec_response_pwm_get_fan_rpm {
	uint32_t rpm;
} __packed;

/*                    */
#define EC_CMD_PWM_SET_FAN_TARGET_RPM 0x21

struct ec_params_pwm_set_fan_target_rpm {
	uint32_t rpm;
} __packed;

/*                        */
#define EC_CMD_PWM_GET_KEYBOARD_BACKLIGHT 0x22

struct ec_response_pwm_get_keyboard_backlight {
	uint8_t percent;
	uint8_t enabled;
} __packed;

/*                        */
#define EC_CMD_PWM_SET_KEYBOARD_BACKLIGHT 0x23

struct ec_params_pwm_set_keyboard_backlight {
	uint8_t percent;
} __packed;

/*                               */
#define EC_CMD_PWM_SET_FAN_DUTY 0x24

struct ec_params_pwm_set_fan_duty {
	uint32_t percent;
} __packed;

/*                                                                           */
/*
                                                                             
                                                                               
                                                                      
                                                            
 */
#define EC_CMD_LIGHTBAR_CMD 0x28

struct rgb_s {
	uint8_t r, g, b;
};

#define LB_BATTERY_LEVELS 4
/*                                                                         
                                                                            
 */
struct lightbar_params {
	/*        */
	int google_ramp_up;
	int google_ramp_down;
	int s3s0_ramp_up;
	int s0_tick_delay[2];			/*        */
	int s0a_tick_delay[2];			/*        */
	int s0s3_ramp_down;
	int s3_sleep_for;
	int s3_ramp_up;
	int s3_ramp_down;

	/*             */
	uint8_t new_s0;
	uint8_t osc_min[2];			/*        */
	uint8_t osc_max[2];			/*        */
	uint8_t w_ofs[2];			/*        */

	/*                                                  */
	uint8_t bright_bl_off_fixed[2];		/*        */
	uint8_t bright_bl_on_min[2];		/*        */
	uint8_t bright_bl_on_max[2];		/*        */

	/*                          */
	uint8_t battery_threshold[LB_BATTERY_LEVELS - 1];

	/*                                        */
	uint8_t s0_idx[2][LB_BATTERY_LEVELS];	/*               */
	uint8_t s3_idx[2][LB_BATTERY_LEVELS];	/*                */

	/*               */
	struct rgb_s color[8];			/*                       */
} __packed;

struct ec_params_lightbar {
	uint8_t cmd;		      /*                                     */
	union {
		struct {
			/*         */
		} dump, off, on, init, get_seq, get_params;

		struct num {
			uint8_t num;
		} brightness, seq, demo;

		struct reg {
			uint8_t ctrl, reg, value;
		} reg;

		struct rgb {
			uint8_t led, red, green, blue;
		} rgb;

		struct lightbar_params set_params;
	};
} __packed;

struct ec_response_lightbar {
	union {
		struct dump {
			struct {
				uint8_t reg;
				uint8_t ic0;
				uint8_t ic1;
			} vals[23];
		} dump;

		struct get_seq {
			uint8_t num;
		} get_seq;

		struct lightbar_params get_params;

		struct {
			/*                  */
		} off, on, init, brightness, seq, reg, rgb, demo, set_params;
	};
} __packed;

/*                   */
enum lightbar_command {
	LIGHTBAR_CMD_DUMP = 0,
	LIGHTBAR_CMD_OFF = 1,
	LIGHTBAR_CMD_ON = 2,
	LIGHTBAR_CMD_INIT = 3,
	LIGHTBAR_CMD_BRIGHTNESS = 4,
	LIGHTBAR_CMD_SEQ = 5,
	LIGHTBAR_CMD_REG = 6,
	LIGHTBAR_CMD_RGB = 7,
	LIGHTBAR_CMD_GET_SEQ = 8,
	LIGHTBAR_CMD_DEMO = 9,
	LIGHTBAR_CMD_GET_PARAMS = 10,
	LIGHTBAR_CMD_SET_PARAMS = 11,
	LIGHTBAR_NUM_CMDS
};

/*                                                                           */
/*                        */

/*
                                                                         
                                              
 */

/*                            */
#define EC_CMD_VBOOT_HASH 0x2A

struct ec_params_vboot_hash {
	uint8_t cmd;             /*                        */
	uint8_t hash_type;       /*                         */
	uint8_t nonce_size;      /*                      */
	uint8_t reserved0;       /*                 */
	uint32_t offset;         /*                         */
	uint32_t size;           /*                         */
	uint8_t nonce_data[64];  /*                                     */
} __packed;

struct ec_response_vboot_hash {
	uint8_t status;          /*                           */
	uint8_t hash_type;       /*                         */
	uint8_t digest_size;     /*                              */
	uint8_t reserved0;       /*                   */
	uint32_t offset;         /*                                  */
	uint32_t size;           /*                        */
	uint8_t hash_digest[64]; /*                  */
} __packed;

enum ec_vboot_hash_cmd {
	EC_VBOOT_HASH_GET = 0,       /*                         */
	EC_VBOOT_HASH_ABORT = 1,     /*                                */
	EC_VBOOT_HASH_START = 2,     /*                            */
	EC_VBOOT_HASH_RECALC = 3,    /*                                  */
};

enum ec_vboot_hash_type {
	EC_VBOOT_HASH_TYPE_SHA256 = 0, /*         */
};

enum ec_vboot_hash_status {
	EC_VBOOT_HASH_STATUS_NONE = 0, /*                                   */
	EC_VBOOT_HASH_STATUS_DONE = 1, /*                           */
	EC_VBOOT_HASH_STATUS_BUSY = 2, /*                       */
};

/*
                                                                              
                                                                            
                                                                 
 */
#define EC_VBOOT_HASH_OFFSET_RO 0xfffffffe
#define EC_VBOOT_HASH_OFFSET_RW 0xfffffffd

/*                                                                           */
/*                               */

/*                            */
#define EC_CMD_USB_CHARGE_SET_MODE 0x30

struct ec_params_usb_charge_set_mode {
	uint8_t usb_port_id;
	uint8_t mode;
} __packed;

/*                                                                           */
/*                             */

/*                                                            */
#define EC_PSTORE_SIZE_MAX 64

/*                             */
#define EC_CMD_PSTORE_INFO 0x40

struct ec_response_pstore_info {
	/*                                   */
	uint32_t pstore_size;
	/*                                                                    */
	uint32_t access_size;
} __packed;

/*
                          
  
                                         
 */
#define EC_CMD_PSTORE_READ 0x41

struct ec_params_pstore_read {
	uint32_t offset;   /*                     */
	uint32_t size;     /*                       */
} __packed;

/*                          */
#define EC_CMD_PSTORE_WRITE 0x42

struct ec_params_pstore_write {
	uint32_t offset;   /*                      */
	uint32_t size;     /*                        */
	uint8_t data[EC_PSTORE_SIZE_MAX];
} __packed;

/*                                                                           */
/*                 */

/*                                    */
struct ec_params_rtc {
	uint32_t time;
} __packed;

struct ec_response_rtc {
	uint32_t time;
} __packed;

/*                           */
#define EC_CMD_RTC_GET_VALUE 0x44
#define EC_CMD_RTC_GET_ALARM 0x45

/*                             */
#define EC_CMD_RTC_SET_VALUE 0x46
#define EC_CMD_RTC_SET_ALARM 0x47

/*                                                                           */
/*                   */

/*                                         */
#define EC_CMD_PORT80_LAST_BOOT 0x48

struct ec_response_port80_last_boot {
	uint16_t code;
} __packed;

/*                                                                           */
/*                         */

/*                     */
#define EC_CMD_THERMAL_SET_THRESHOLD 0x50

struct ec_params_thermal_set_threshold {
	uint8_t sensor_type;
	uint8_t threshold_id;
	uint16_t value;
} __packed;

/*                     */
#define EC_CMD_THERMAL_GET_THRESHOLD 0x51

struct ec_params_thermal_get_threshold {
	uint8_t sensor_type;
	uint8_t threshold_id;
} __packed;

struct ec_response_thermal_get_threshold {
	uint16_t value;
} __packed;

/*                              */
#define EC_CMD_THERMAL_AUTO_FAN_CTRL 0x52

/*                             */
#define EC_CMD_TMP006_GET_CALIBRATION 0x53

struct ec_params_tmp006_get_calibration {
	uint8_t index;
} __packed;

struct ec_response_tmp006_get_calibration {
	float s0;
	float b0;
	float b1;
	float b2;
} __packed;

/*                             */
#define EC_CMD_TMP006_SET_CALIBRATION 0x54

struct ec_params_tmp006_set_calibration {
	uint8_t index;
	uint8_t reserved[3];  /*                 */
	float s0;
	float b0;
	float b1;
	float b2;
} __packed;

/*                                                                           */
/*                                 */

/*
                 
  
                                                                         
                          
 */
#define EC_CMD_MKBP_STATE 0x60

/*                                                                   */
#define EC_CMD_MKBP_INFO 0x61

struct ec_response_mkbp_info {
	uint32_t rows;
	uint32_t cols;
	uint8_t switches;
} __packed;

/*                    */
#define EC_CMD_MKBP_SIMULATE_KEY 0x62

struct ec_params_mkbp_simulate_key {
	uint8_t col;
	uint8_t row;
	uint8_t pressed;
} __packed;

/*                             */
#define EC_CMD_MKBP_SET_CONFIG 0x64
#define EC_CMD_MKBP_GET_CONFIG 0x65

/*       */
enum mkbp_config_flags {
	EC_MKBP_FLAGS_ENABLE = 1,	/*                          */
};

enum mkbp_config_valid {
	EC_MKBP_VALID_SCAN_PERIOD		= 1 << 0,
	EC_MKBP_VALID_POLL_TIMEOUT		= 1 << 1,
	EC_MKBP_VALID_MIN_POST_SCAN_DELAY	= 1 << 3,
	EC_MKBP_VALID_OUTPUT_SETTLE		= 1 << 4,
	EC_MKBP_VALID_DEBOUNCE_DOWN		= 1 << 5,
	EC_MKBP_VALID_DEBOUNCE_UP		= 1 << 6,
	EC_MKBP_VALID_FIFO_MAX_DEPTH		= 1 << 7,
};

/*                                              */
struct ec_mkbp_config {
	uint32_t valid_mask;		/*              */
	uint8_t flags;		/*                                     */
	uint8_t valid_flags;		/*                       */
	uint16_t scan_period_us;	/*                               */
	/*                                                          */
	uint32_t poll_timeout_us;
	/*
                                                                
                                                                    
                                            
  */
	uint16_t min_post_scan_delay_us;
	/*                                                              */
	uint16_t output_settle_us;
	uint16_t debounce_down_us;	/*                               */
	uint16_t debounce_up_us;	/*                             */
	/*                                                         */
	uint8_t fifo_max_depth;
} __packed;

struct ec_params_mkbp_set_config {
	struct ec_mkbp_config config;
} __packed;

struct ec_response_mkbp_get_config {
	struct ec_mkbp_config config;
} __packed;

/*                            */
#define EC_CMD_KEYSCAN_SEQ_CTRL 0x66

enum ec_keyscan_seq_cmd {
	EC_KEYSCAN_SEQ_STATUS = 0,	/*                        */
	EC_KEYSCAN_SEQ_CLEAR = 1,	/*                */
	EC_KEYSCAN_SEQ_ADD = 2,		/*                      */
	EC_KEYSCAN_SEQ_START = 3,	/*                        */
	EC_KEYSCAN_SEQ_COLLECT = 4,	/*                               */
};

enum ec_collect_flags {
	/*
                                                                    
                         
  */
	EC_KEYSCAN_SEQ_FLAG_DONE	= 1 << 0,
};

struct ec_collect_item {
	uint8_t flags;		/*                                    */
};

struct ec_params_keyscan_seq_ctrl {
	uint8_t cmd;	/*                                           */
	union {
		struct {
			uint8_t active;		/*              */
			uint8_t num_items;	/*                 */
			/*                              */
			uint8_t cur_item;
		} status;
		struct {
			/*
                                                    
                            
    */
			uint32_t time_us;
			uint8_t scan[0];	/*              */
		} add;
		struct {
			uint8_t start_item;	/*                      */
			uint8_t num_items;	/*                           */
		} collect;
	};
} __packed;

struct ec_result_keyscan_seq_ctrl {
	union {
		struct {
			uint8_t num_items;	/*                 */
			/*                    */
			struct ec_collect_item item[0];
		} collect;
	};
} __packed;

/*                                                                           */
/*                             */

/*                              */
#define EC_CMD_TEMP_SENSOR_GET_INFO 0x70

struct ec_params_temp_sensor_get_info {
	uint8_t id;
} __packed;

struct ec_response_temp_sensor_get_info {
	char sensor_name[32];
	uint8_t sensor_type;
} __packed;

/*                                                                           */

/*
                                                                           
                                                                           
         
 */

/*                                                                           */
/*                     */

/*
                                                                            
                        
 */
struct ec_params_host_event_mask {
	uint32_t mask;
} __packed;

struct ec_response_host_event_mask {
	uint32_t mask;
} __packed;

/*                                           */
#define EC_CMD_HOST_EVENT_GET_B         0x87
#define EC_CMD_HOST_EVENT_GET_SMI_MASK  0x88
#define EC_CMD_HOST_EVENT_GET_SCI_MASK  0x89
#define EC_CMD_HOST_EVENT_GET_WAKE_MASK 0x8d

/*                                         */
#define EC_CMD_HOST_EVENT_SET_SMI_MASK  0x8a
#define EC_CMD_HOST_EVENT_SET_SCI_MASK  0x8b
#define EC_CMD_HOST_EVENT_CLEAR         0x8c
#define EC_CMD_HOST_EVENT_SET_WAKE_MASK 0x8e
#define EC_CMD_HOST_EVENT_CLEAR_B       0x8f

/*                                                                           */
/*                 */

/*                              */
#define EC_CMD_SWITCH_ENABLE_BKLIGHT 0x90

struct ec_params_switch_enable_backlight {
	uint8_t enabled;
} __packed;

/*                               */
#define EC_CMD_SWITCH_ENABLE_WIRELESS 0x91

struct ec_params_switch_enable_wireless {
	uint8_t enabled;
} __packed;

/*                                                                           */
/*                                                                         */

/*                       */
#define EC_CMD_GPIO_SET 0x92

struct ec_params_gpio_set {
	char name[32];
	uint8_t val;
} __packed;

/*                */
#define EC_CMD_GPIO_GET 0x93

struct ec_params_gpio_get {
	char name[32];
} __packed;
struct ec_response_gpio_get {
	uint8_t val;
} __packed;

/*                                                                           */
/*                                                                    */

/*              */
#define EC_CMD_I2C_READ 0x94

struct ec_params_i2c_read {
	uint16_t addr;
	uint8_t read_size; /*                 */
	uint8_t port;
	uint8_t offset;
} __packed;
struct ec_response_i2c_read {
	uint16_t data;
} __packed;

/*               */
#define EC_CMD_I2C_WRITE 0x95

struct ec_params_i2c_write {
	uint16_t data;
	uint16_t addr;
	uint8_t write_size; /*                 */
	uint8_t port;
	uint8_t offset;
} __packed;

/*                                                                           */
/*                                                                          */

/*                                                 */
#define EC_CMD_CHARGE_FORCE_IDLE 0x96

struct ec_params_force_idle {
	uint8_t enabled;
} __packed;

/*                                                                           */
/*                                                                        */

/*                                                                */
#define EC_CMD_CONSOLE_SNAPSHOT 0x97

/*
                                               
  
                                                                         
                    
 */
#define EC_CMD_CONSOLE_READ 0x98

/*                                                                           */

/*
                                                        
  
                                                                         
                                 
 */
#define EC_CMD_BATTERY_CUT_OFF 0x99

/*                                                                           */
/*                                                                 */

/*
                                     
  
                                                             
 */
#define EC_CMD_CHARGE_DUMP 0xa0

/*
                                        
 */
#define EC_CMD_CHARGE_CURRENT_LIMIT 0xa1

struct ec_params_current_limit {
	uint32_t limit;
} __packed;

/*                                                                           */
/*                 */

/*
                                                                              
                                              
 */
#define EC_CMD_REBOOT_EC 0xd2

/*         */
enum ec_reboot_cmd {
	EC_REBOOT_CANCEL = 0,        /*                         */
	EC_REBOOT_JUMP_RO = 1,       /*                              */
	EC_REBOOT_JUMP_RW = 2,       /*                              */
	/*                              */
	EC_REBOOT_COLD = 4,          /*             */
	EC_REBOOT_DISABLE_JUMP = 5,  /*                                */
	EC_REBOOT_HIBERNATE = 6      /*              */
};

/*                                            */
#define EC_REBOOT_FLAG_RESERVED0      (1 << 0)  /*                      */
#define EC_REBOOT_FLAG_ON_AP_SHUTDOWN (1 << 1)  /*                          */

struct ec_params_reboot_ec {
	uint8_t cmd;           /*                    */
	uint8_t flags;         /*                      */
} __packed;

/*
                                    
  
                                                                             
               
 */
#define EC_CMD_GET_PANIC_INFO 0xd3

/*                                                                           */
/*
                
  
                                                      
 */

/*
                                
  
                                                               
  
                              
  
                                                      
                                                 
                                              
                                            
                                             
 */
#define EC_CMD_ACPI_READ 0x80

/*
                                 
  
                                                               
  
                              
  
                                                       
                                                 
                                              
                                                 
                                            
 */
#define EC_CMD_ACPI_WRITE 0x81

/*
                                 
  
                                                                             
                                                                              
                                                        
 */
#define EC_CMD_ACPI_QUERY_EVENT 0x84

/*                                                               */
/*                                                          */
#define EC_ACPI_MEM_VERSION            0x00
/*
                                                                            
          
 */
#define EC_ACPI_MEM_TEST               0x01
/*                                           */
#define EC_ACPI_MEM_TEST_COMPLIMENT    0x02
/*                                                 */
#define EC_ACPI_MEM_KEYBOARD_BACKLIGHT 0x03

/*                                              */
#define EC_ACPI_MEM_VERSION_CURRENT 1


/*                                                                           */
/*
                   
  
                                                                           
           
 */

/*
             
  
                                                                             
                                                                         
                                                                              
  
                                                       
 */
#define EC_CMD_REBOOT 0xd1  /*             */

/*
                                               
  
                                                                              
                                                                            
               
 */
#define EC_CMD_RESEND_RESPONSE 0xdb

/*
                                                                         
                                                                         
                                                 
  
                                                                        
                                                    
  
                                                            
 */
#define EC_CMD_VERSION0 0xdc

#endif  /*           */

#endif  /*                      */
