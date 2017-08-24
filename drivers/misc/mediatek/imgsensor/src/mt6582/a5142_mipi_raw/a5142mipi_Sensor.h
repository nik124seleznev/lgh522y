/*                                                                            
  
            
            
                       
  
           
           
         
  
               
               
                                 
  
  
          
          
                            
  
                                                                              
                      
                                                                     
                                                                                
              
             
         
  
  
  
                                                                                
                                                                     
                                                                              
                                                                            */

#ifndef _A5142MIPI_SENSOR_H
#define _A5142MIPI_SENSOR_H

typedef enum group_enum {
    PRE_GAIN=0,
    CMMCLK_CURRENT,
    FRAME_RATE_LIMITATION,
    REGISTER_EDITOR,
    GROUP_TOTAL_NUMS
} FACTORY_GROUP_ENUM;


#define ENGINEER_START_ADDR 10
#define FACTORY_START_ADDR 0


typedef enum register_index
{
    SENSOR_BASEGAIN=FACTORY_START_ADDR,
    PRE_GAIN_R_INDEX,
    PRE_GAIN_Gr_INDEX,
    PRE_GAIN_Gb_INDEX,
    PRE_GAIN_B_INDEX,
    FACTORY_END_ADDR
} FACTORY_REGISTER_INDEX;


typedef enum engineer_index
{
    CMMCLK_CURRENT_INDEX=ENGINEER_START_ADDR,
    ENGINEER_END
} FACTORY_ENGINEER_INDEX;


typedef struct
{
    SENSOR_REG_STRUCT   Reg[ENGINEER_END];
    SENSOR_REG_STRUCT   CCT[FACTORY_END_ADDR];
} SENSOR_DATA_STRUCT, *PSENSOR_DATA_STRUCT;


//                                               
//                                                     
#define MIPI_INTERFACE
#define RAW10

#define A5142MIPI_DATA_FORMAT SENSOR_OUTPUT_FORMAT_RAW_B


#define A5142MIPI_WRITE_ID_1    0x6C
#define A5142MIPI_READ_ID_1     0x6D
#define A5142MIPI_WRITE_ID_2    0x6E
#define A5142MIPI_READ_ID_2     0x6F


#define A5142MIPI_IMAGE_SENSOR_FULL_HACTIVE     (2592)
#define A5142MIPI_IMAGE_SENSOR_FULL_VACTIVE     (1944)
#define A5142MIPI_IMAGE_SENSOR_PV_HACTIVE       (1296)
#define A5142MIPI_IMAGE_SENSOR_PV_VACTIVE       (972)


#define A5142MIPI_FULL_START_X                  (6)
#define A5142MIPI_FULL_START_Y                  (6+1)
#define A5142MIPI_IMAGE_SENSOR_FULL_WIDTH       (A5142MIPI_IMAGE_SENSOR_FULL_HACTIVE - 32)  //            
#define A5142MIPI_IMAGE_SENSOR_FULL_HEIGHT      (A5142MIPI_IMAGE_SENSOR_FULL_VACTIVE - 24)  //             

#define A5142MIPI_PV_START_X                    (2)
#define A5142MIPI_PV_START_Y                    (2+1)
#define A5142MIPI_IMAGE_SENSOR_PV_WIDTH         (A5142MIPI_IMAGE_SENSOR_PV_HACTIVE - 16)    //             
#define A5142MIPI_IMAGE_SENSOR_PV_HEIGHT        (A5142MIPI_IMAGE_SENSOR_PV_VACTIVE - 12)    //           


#ifdef MIPI_INTERFACE
    #define A5142MIPI_IMAGE_SENSOR_FULL_HBLANKING   1102
#else
    #define A5142MIPI_IMAGE_SENSOR_FULL_HBLANKING   200
#endif
#define A5142MIPI_IMAGE_SENSOR_FULL_VBLANKING       77


#ifdef MIPI_INTERFACE
    #define A5142MIPI_IMAGE_SENSOR_PV_HBLANKING     1855
#else
    #define A5142MIPI_IMAGE_SENSOR_PV_HBLANKING     279
#endif
#define A5142MIPI_IMAGE_SENSOR_PV_VBLANKING         128


#define A5142MIPI_FULL_PERIOD_PIXEL_NUMS            (A5142MIPI_IMAGE_SENSOR_FULL_HACTIVE + A5142MIPI_IMAGE_SENSOR_FULL_HBLANKING)  //               
#define A5142MIPI_FULL_PERIOD_LINE_NUMS             (A5142MIPI_IMAGE_SENSOR_FULL_VACTIVE + A5142MIPI_IMAGE_SENSOR_FULL_VBLANKING)  //              
#define A5142MIPI_PV_PERIOD_PIXEL_NUMS              (A5142MIPI_IMAGE_SENSOR_PV_HACTIVE + A5142MIPI_IMAGE_SENSOR_PV_HBLANKING)      //                
#define A5142MIPI_PV_PERIOD_LINE_NUMS               (A5142MIPI_IMAGE_SENSOR_PV_VACTIVE + A5142MIPI_IMAGE_SENSOR_PV_VBLANKING)      //               


/*                       */
struct A5142MIPI_SENSOR_STRUCT
{
    kal_uint8 i2c_write_id;
    kal_uint8 i2c_read_id;
    kal_uint16 preview_vt_clk;
    kal_uint16 capture_vt_clk;
};

#endif /*                     */

