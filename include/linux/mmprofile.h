#ifndef __MMPROFILE_H__
#define __MMPROFILE_H__
#include "mmprofile_static_event.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MMProfileEventNameMaxLen 31

typedef unsigned int MMP_Event;

typedef enum {
    MMProfileFlagStart = 1,
    MMProfileFlagEnd = 2,
    MMProfileFlagPulse = 4,
    MMProfileFlagEventSeparator = 8,
    MMProfileFlagSystrace = 0x80000000,
    MMProfileFlagMax = 0xFFFFFFFF
} MMP_LogType;

typedef enum {
    MMProfileMetaStringMBS = 1,
    MMProfileMetaStringWCS,
    MMProfileMetaStructure,
    MMProfileMetaBitmap,
    MMProfileMetaRaw,
    MMProfileMetaUser = 0x10000000,
    MMProfileMetaUserM4UReg,
    MMProfileMetaMax = 0xFFFFFFFF
} MMP_MetaDataType;

typedef enum {
    MMProfileBitmapRGB565 = 1,
    MMProfileBitmapRGB888,
    MMProfileBitmapRGBA8888,
    MMProfileBitmapBGR888,
    MMProfileBitmapBGRA8888,
    MMProfileBitmapMax = 0xFFFFFFFF
} MMP_PixelFormat;

typedef struct {
    unsigned int data1;         //                     
    unsigned int data2;         //                     
    MMP_MetaDataType data_type;  //               
    unsigned int size;          //               
    void *pData;                 //                  
} MMP_MetaData_t;

typedef struct {
    unsigned int data1;         //                     
    unsigned int data2;         //                     
    unsigned int struct_size;   //                       
    void *pData;                 //                  
    char struct_name[32];        //               
} MMP_MetaDataStructure_t;

typedef struct {
    unsigned int data1;         //                     
    unsigned int data2;         //                     
    unsigned int width;         //            
    unsigned int height;        //             
    MMP_PixelFormat format;      //                   
    unsigned int start_pos;     //                                           
    unsigned int bpp;           //               
    int pitch;         //                             
    unsigned int data_size;     //                        
    unsigned int down_sample_x; //                                  
    unsigned int down_sample_y; //                                
    void *pData;                 //                     
} MMP_MetaDataBitmap_t;

MMP_Event MMProfileRegisterEvent(MMP_Event parent, const char *name);
MMP_Event MMProfileFindEvent(MMP_Event parent, const char *name);
void MMProfileEnableEvent(MMP_Event event, long enable);
void MMProfileEnableFTraceEvent(MMP_Event event, long enable, long ftrace);
void MMProfileEnableEventRecursive(MMP_Event event, long enable);
void MMProfileEnableFTraceEventRecursive(MMP_Event event, long enable, long ftrace);
long MMProfileQueryEnable(MMP_Event event);
void MMProfileLog(MMP_Event event, MMP_LogType type);
void MMProfileLogEx(MMP_Event event, MMP_LogType type, unsigned long data1, unsigned long data2);
long MMProfileLogMeta(MMP_Event event, MMP_LogType type, MMP_MetaData_t *pMetaData);
long MMProfileLogMetaString(MMP_Event event, MMP_LogType type, const char *str);
long MMProfileLogMetaStringEx(MMP_Event event, MMP_LogType type, unsigned long data1, unsigned long data2, const char *str);
long MMProfileLogMetaStructure(MMP_Event event, MMP_LogType type, MMP_MetaDataStructure_t *pMetaData);
long MMProfileLogMetaBitmap(MMP_Event event, MMP_LogType type, MMP_MetaDataBitmap_t *pMetaData);

#define MMProfileLogStructure(event, type, pStruct, struct_type) \
{ \
    MMP_MetaDataStructure_t MetaData; \
    MetaData.data1 = 0; \
    MetaData.data2 = 0; \
    strcpy(MetaData.struct_name, #struct_type); \
    MetaData.struct_size = sizeof(struct_type); \
    MetaData.pData = (void *)(pStruct); \
    MMProfileLogMetaStructure(event, type, &MetaData); \
}

#ifdef __cplusplus
}
#endif
#endif
