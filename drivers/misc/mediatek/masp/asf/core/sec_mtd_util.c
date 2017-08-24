#include "sec_error.h"
#include "sec_boot.h"
#include "sec_mtd.h"
#include "sec_typedef.h"
#include "sec_osal_light.h"

/*                                                                         
               
                                                                          */
#define MOD                         "MTD_UTIL"

/*                                                                         
                   
                                                                          */
char* mtd2pl (char* part_name)
{
    /*                                            */
    /*                   */
    /*                   */
    /*                   */    
    if(0 == mcmp(part_name,MTD_SECCFG,strlen(MTD_SECCFG)))
    {   
        return (char*) PL_SECCFG;
    }
    /*                   */
    /*                   */
    /*                   */    
    else if(0 == mcmp(part_name,MTD_UBOOT,strlen(MTD_UBOOT)))
    {   
        return (char*) PL_UBOOT;
    }
    /*                   */    
    /*                   */
    /*                   */    
    else if(0 == mcmp(part_name,MTD_LOGO,strlen(MTD_LOGO)))
    {
        return (char*) PL_LOGO;
    }
    /*                   */
    /*                   */
    /*                   */    
    else if(0 == mcmp(part_name,MTD_BOOTIMG,strlen(MTD_BOOTIMG)))
    {
        return (char*) PL_BOOTIMG;
    }
    /*                   */    
    /*                   */
    /*                   */    
    else if(0 == mcmp(part_name,MTD_USER,strlen(MTD_USER)))
    {
        return (char*) PL_USER;               
    }   
    /*                   */    
    /*                   */
    /*                   */    
    else if(0 == mcmp(part_name,MTD_ANDSYSIMG,strlen(MTD_ANDSYSIMG)))
    {
        return (char*) PL_ANDSYSIMG;
    }   
    /*                   */    
    /*                   */
    /*                   */    
    else if(0 == mcmp(part_name,MTD_RECOVERY,strlen(MTD_RECOVERY)))
    {
        return (char*) PL_RECOVERY;
    }       
    /*                   */    
    /*                   */
    /*                   */    
    else if(0 == mcmp(part_name,MTD_SECRO,strlen(MTD_SECRO)))
    {
        return (char*) PL_SECRO;
    }
    /*                   */    
    /*                   */
    /*                   */    
    else
    {
        return part_name;
    }    
}

char* pl2mtd (char* part_name)
{
    /*                                            */
    /*                   */
    /*                   */
    /*                   */    
    if(0 == mcmp(part_name,PL_SECCFG,strlen(PL_SECCFG)))
    {   
        return (char*) MTD_SECCFG;
    }
    /*                   */    
    /*                   */
    /*                   */    
    else if(0 == mcmp(part_name,PL_UBOOT,strlen(PL_UBOOT)))
    {   
        return (char*) MTD_UBOOT;
    }
    /*                   */    
    /*                   */
    /*                   */    
    else if(0 == mcmp(part_name,PL_LOGO,strlen(PL_LOGO)))
    {
        return (char*) MTD_LOGO;
    }
    /*                   */    
    /*                   */
    /*                   */    
    else if(0 == mcmp(part_name,PL_BOOTIMG,strlen(PL_BOOTIMG)))
    {
        return (char*) MTD_BOOTIMG;
    }
    /*                   */    
    /*                   */
    /*                   */    
    else if(0 == mcmp(part_name,PL_USER,strlen(PL_USER)))
    {
        return (char*) MTD_USER;               
    }   
    /*                   */    
    /*                   */
    /*                   */    
    else if(0 == mcmp(part_name,PL_ANDSYSIMG,strlen(PL_ANDSYSIMG)))
    {
        return (char*) MTD_ANDSYSIMG;
    }   
    /*                   */    
    /*                   */
    /*                   */    
    else if(0 == mcmp(part_name,PL_RECOVERY,strlen(PL_RECOVERY)))
    {
        return (char*) MTD_RECOVERY;
    }       
    /*                   */    
    /*                   */
    /*                   */    
    else if(0 == mcmp(part_name,PL_SECRO,strlen(PL_SECRO)))
    {
        return (char*) MTD_SECRO;
    }
    /*                   */    
    /*                   */
    /*                   */    
    else
    {
        return part_name;
    }    
}


