/*
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   Copyright (c) 2011 Synaptics, Inc.

   Permission is hereby granted, free of charge, to any person obtaining a copy of
   this software and associated documentation files (the "Software"), to deal in
   the Software without restriction, including without limitation the rights to use,
   copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
   Software, and to permit persons to whom the Software is furnished to do so,
   subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
 */

#include "RefCode.h"
#include "RefCode_PDTScan.h"

#ifdef _F54_TEST_
#include "RefCode_F54_FullRawCapLimit.h"

#ifdef F54_Porting
static unsigned char ImageBuffer[CFG_F54_TXCOUNT*CFG_F54_RXCOUNT*2];
static signed ImageArray[CFG_F54_TXCOUNT] [CFG_F54_RXCOUNT];

static char buf[6000] = {0};
static unsigned char call_cnt = 0;
static int LimitBack[30][46*2];
#endif

unsigned char F54_FullRawCap(int mode)
//                                                                                
{
   signed short temp=0;
   int Result = 0;
#ifdef F54_Porting
    int ret = 0;
    unsigned char product_id[11];
#endif
    int TSPCheckLimit=700;
    short Flex_LowerLimit = -100;
    short Flex_UpperLimit = 500;

    int i, j, k;
    unsigned short length;

    unsigned char command;

    length = numberOfTx * numberOfRx* 2;

    //                                     
    readRMI(F01_Query_Base+11, &product_id[0], sizeof(product_id));

    if(!strncmp(product_id, "PLG245", 6)) {
        printk("set limit array to PLG245 value.\n");
        //                                                  
    } else if(!strncmp(product_id, "PLG260", 6)) {
        printk("set limit array to PLG260 value.\n");
        //                                                  
    } else {
        printk("set limit array to LGIT value.\n");
    }

    //               
    if(call_cnt == 0){
        printk("Backup Limit to LimitBack array\n");
        memset(LimitBack, 0, sizeof(LimitBack));
//                                                    
#if defined(LGE_USE_DOME_KEY)
        memcpy(LimitBack, Limit_PLG260, sizeof(LimitBack));
#else
        memcpy(LimitBack, Limit_PLG245, sizeof(LimitBack));
#endif
    }
    if(get_limit(numberOfTx, numberOfRx) > 0) {
        printk("Get limit from file success!!Use Limit array from file data.\n");
//                                                
#if defined(LGE_USE_DOME_KEY)
        memcpy(Limit_PLG260, LimitFile, sizeof(Limit_PLG260));
#else
        memcpy(Limit_PLG245, LimitFile, sizeof(Limit_PLG260));
#endif
    } else {
        printk("Get limit from file fail!!Use Limit array from image data\n");
        //                                        
#if defined(LGE_USE_DOME_KEY)
        memcpy(Limit_PLG260, LimitBack, sizeof(Limit_PLG260));
#else
        memcpy(Limit_PLG245, LimitBack, sizeof(Limit_PLG245));
#endif

    }
    call_cnt++;
    if(call_cnt > 0) call_cnt = 1;

    if(mode == 3 || mode == 4)
    {
/*                      
                       
                                               
                                               
*/    }

    //         
    command = 0x04;
    writeRMI(F01_Ctrl_Base, &command, 1);

    //                                       
    if(mode >= 0 && mode < 4)    command = 0x03;
    if(mode == 4)                command = 0x02;
    writeRMI(F54_Data_Base, &command, 1);

    if(mode == 3 || mode == 4)
    {
/*                
                       
                                               
*/    }

    if(mode != 3 && mode != 4)
    {
        //                         
        command = 0x06;
        writeRMI(F54_Command_Base, &command, 1);

        do {
            delayMS(1); //        
            readRMI(F54_Command_Base, &command, 1);
        } while (command != 0x00);
    }

    //                                                                          
    command = 0x08;
    writeRMI(F01_Cmd_Base+1, &command, 1);

    command = 0x00;
    writeRMI(F54_Data_LowIndex, &command, 1);
    writeRMI(F54_Data_HighIndex, &command, 1);

    //                                          
    command = 0x01;
    writeRMI(F54_Command_Base, &command, 1);

    //                                    
    do {
     delayMS(1); //        
        readRMI(F54_Command_Base, &command, 1);
   } while (command != 0x00);

    //                                                  
    longReadRMI(F54_Data_Buffer, &ImageBuffer[0], length);

    if( (numberOfTx > 29) || (numberOfRx > 45) ) {
        printk("Limit Index overflow. Test result: Fail\n");
        return 0;
    }

    switch(mode)
    {
        case 0:
        case 1:
#ifdef F54_Porting
            memset(buf, 0, sizeof(buf));
            ret = sprintf(buf, "#ofTx\t%d\n", numberOfTx);
            ret += sprintf(buf+ret, "#ofRx\t%d\n", numberOfRx);

            ret += sprintf(buf+ret, "\n#3.03    Full raw capacitance Test\n");
#else
            printk("#ofTx\t%d\n", numberOfTx);
            printk("#ofRx\t%d\n", numberOfRx);

            printk("\n#3.03    Full raw capacitance Test\n");
#endif
            k = 0;
            for (i = 0; i < numberOfTx; i++)
            {
#ifdef F54_Porting
                ret += sprintf(buf+ret, "%d\t", i);
#else
                printk("%d\t", i);
#endif
                for (j = 0; j < numberOfRx; j++)
                {
                        temp = (short)(ImageBuffer[k] | (ImageBuffer[k+1] << 8));
                        if(CheckButton[i][j] != 1)
                        {
                            if(mode==0)
                            {
#ifdef F54_Porting
                                //                                                         
#if defined(LGE_USE_DOME_KEY)
                                if ((temp >= Limit_PLG260[i][j*2]) && (temp <= Limit_PLG260[i][j*2+1]))
#else
                                if ((temp >= Limit_PLG245[i][j*2]) && (temp <= Limit_PLG245[i][j*2+1]))
#endif
#else
                                if ((temp >= Limit[i][j*2]*1000) && (temp <= Limit[i][j*2+1]*1000))
#endif
                                {
                                    Result++;
#ifdef F54_Porting
                                    ret += sprintf(buf+ret, "%d\t", temp); //                                  
#else
                                    printk("%d\t", temp); //                                  
#endif
                                }
                                else {
#ifdef F54_Porting
                                    ret += sprintf(buf+ret, "%d\t", temp); //                                  
#else
                                    printk("%d\t", temp); //                                  
#endif
                                }
                            }
                            else
                            {
                                if ((temp >= Flex_LowerLimit) && (temp <= Flex_UpperLimit))
                                {
                                    Result++;
#ifdef F54_Porting
                                    ret += sprintf(buf+ret, "%d\t", temp); //                                  
#else
                                    printk("%d\t", temp); //                                  
#endif
                                }
                                else {
#ifdef F54_Porting
                                    ret += sprintf(buf+ret, "%d\t", temp); //                                  
#else
                                    printk("%d\t", temp); //                                  
#endif
                                }
                            }
                        }
                        else
                        {
                            Result++;
#ifdef F54_Porting
                            ret += sprintf(buf+ret, "%d\t", temp); //                                  
#else
                            printk("%d\t", temp); //                                  
#endif
                        }
                  k = k + 2;
                }
#ifdef F54_Porting
                ret += sprintf(buf+ret, "\n");
#else
                printk("\n"); //                                  
#endif
            }

#ifdef F54_Porting
            ret += sprintf(buf+ret, "#3.04    Full raw capacitance Test END\n");
            ret += sprintf(buf+ret, "\n#3.01    Full raw capacitance Test Limit\n");

            //                                                         
            ret += sprintf(buf+ret, "\t");
#else
            printk("#3.04    Full raw capacitance Test END\n");

            printk("\n#3.01    Full raw capacitance Test Limit\n");
            //                                                         
            printk("\t");
#endif
            for (j = 0; j < numberOfRx; j++) {
#ifdef F54_Porting
                ret += sprintf(buf+ret, "%d-Min\t%d-Max\t", j, j);
#else
                printk("%d-Min\t%d-Max\t", j, j);
#endif
            }

#ifdef F54_Porting
            ret += sprintf(buf+ret, "\n");
#else
            printk("\n");
#endif

            for (i = 0; i < numberOfTx; i++)
            {
#ifdef F54_Porting
                ret += sprintf(buf+ret, "%d\t", i);
#else
                printk("%d\t", i);
#endif
                for (j = 0; j < numberOfRx; j++)
                {
                    if(mode==0) {
#ifdef F54_Porting
#if defined(LGE_USE_DOME_KEY)
                        //                                                   
                        //                                                                    
                        ret += sprintf(buf+ret, "%d\t%d\t", Limit_PLG260[i][j*2], Limit_PLG260[i][j*2+1]);
#else
                        ret += sprintf(buf+ret, "%d\t%d\t", Limit_PLG245[i][j*2], Limit_PLG245[i][j*2+1]);
#endif
#else
                        printk("%d\t%d\t", Limit[i][j*2]*1000, Limit[i][j*2+1]*1000);
#endif
                    } else {
#ifdef F54_Porting
                        ret += sprintf(buf+ret, "%d\t%d\t", Flex_LowerLimit, Flex_UpperLimit);
#else
                        printk("%d\t%d\t", Flex_LowerLimit, Flex_UpperLimit);
#endif
                    }
                }
#ifdef F54_Porting
                ret += sprintf(buf+ret, "\n");
#else
                printk("\n");
#endif
            }
#ifdef F54_Porting
            ret += sprintf(buf+ret, "#3.02    Full raw cap Limit END\n");
#else
            printk("#3.02    Full raw cap Limit END\n");
#endif
            break;

        case 2:
            k = 0;
            for (i = 0; i < numberOfTx; i++)
            {
                for (j = 0; j < numberOfRx; j++)
                {
                        temp = (short)(ImageBuffer[k] | (ImageBuffer[k+1] << 8));
                           if (temp > TSPCheckLimit)
                           Result++;
                  k = k + 2;
                }
            }
            break;

        case 3:
        case 4:
            k = 0;
            for (i = 0; i < numberOfTx; i++)
            {
                for (j = 0; j < numberOfRx; j++)
                {
                        ImageArray[i][j] = (short)(ImageBuffer[k] | (ImageBuffer[k+1] << 8));
                  k = k + 2;
                }
            }
#ifdef F54_Porting
                 //                                                         
                 ret += sprintf(ret+buf, "\n\t");
#else
                 //                                                         
                 printk("\n\t");
#endif
                 for (j = 0; j < numberOfRx; j++) {
#ifdef F54_Porting
                    ret += sprintf(ret+buf, "RX%d\t", RxChannelUsed[j]);
#else
                     printk("RX%d\t", RxChannelUsed[j]);
#endif
                 }

#ifdef F54_Porting
                 ret += sprintf(buf+ret, "\n");
#else
                 printk("\n");
#endif

                 for (i = 0; i < numberOfTx; i++)
                 {
#ifdef F54_Porting
                     ret += sprintf(buf+ret, "TX%d\t", TxChannelUsed[i]);
#else
                     printk("TX%d\t", TxChannelUsed[i]);
#endif
                     for (j = 0; j < numberOfRx; j++)
                     {
#ifdef F54_Porting
                        if(mode == 3)        ret += sprintf(buf+ret, "%d\t", (ImageArray[i][j]) / 1000);
                        else if(mode == 4)    ret += sprintf(buf+ret, "%d\t", ImageArray[i][j]);
#else
                        if(mode == 3)       printk("%d\t", (ImageArray[i][j]) / 1000);
                        else if(mode == 4) printk("%d\t", ImageArray[i][j]);
                        //                                                                        
                        //                                                    
#endif

                      }
#ifdef F54_Porting
                  ret += sprintf(ret+buf, "\n");
#else
                  printk("\n");
#endif
                 }
            break;

        default:
            break;
    }

    if(mode != 3 && mode != 4)
    {
        //     
        command= 0x01;
        writeRMI(F01_Cmd_Base, &command, 1);
        delayMS(200);
        readRMI(F01_Data_Base+1, &command, 1); //                                                             
    }

    if(mode >= 0 && mode < 2)
    {
        if(Result == numberOfTx * numberOfRx)
        {
#ifdef F54_Porting
            ret += sprintf(buf+ret, "Test Result: Pass\n");
            write_log(buf);
#else
            printk("Test Result: Pass\n");
#endif
            return 1; //    
        }
        else
        {
#ifdef F54_Porting
            ret += sprintf(buf+ret, "Test Result: Fail\n");
            write_log(buf);
#else
            printk("Test Result: Fail\n");
#endif
            return 0; //    
        }
    }
    else if(mode == 2)
    {
        if(Result == 0) return 0; //                 
        else return 1; //             
     }
    else
    {
#ifdef F54_Porting
        write_log(buf);
#endif
        return 0;
    }
}

//                                                                                
int F54_GetFullRawCap(int mode, char *buf) {
    signed short temp=0;
    //               
    int ret = 0;
    unsigned char product_id[11];
    int TSPCheckLimit=700;
    //                              
    //                             

    int i, j, k;
    unsigned short length;

    unsigned char command;

    int waitcount;

    if (mode < 0 && mode > 4)
        return ret;

    length = numberOfTx * numberOfRx * 2;

    //                                     
    readRMI(F01_Query_Base + 11, &product_id[0], sizeof(product_id));

    if(!strncmp(product_id, "PLG245", 6)) {
        pr_info("set limit array to PLG245 value.\n");
        //                                                  
    } else if(!strncmp(product_id, "PLG260", 6)) {
        pr_info("set limit array to PLG260 value.\n");
        //                                                  
    } else {
        pr_info("set limit array to LGIT value.\n");
    }

    //               
    if (call_cnt == 0)
    {
        pr_info("Backup Limit to LimitBack array\n");
        memset(LimitBack, 0, sizeof(LimitBack));
//                                                    
#if defined(LGE_USE_DOME_KEY)
        memcpy(LimitBack, Limit_PLG260, sizeof(LimitBack));
#else
        memcpy(LimitBack, Limit_PLG245, sizeof(LimitBack));
#endif
    }

    if (get_limit(numberOfTx, numberOfRx) > 0) {
        pr_info("Get limit from file success!!Use Limit array from file data.\n");
//                                                
#if defined(LGE_USE_DOME_KEY)
        memcpy(Limit_PLG260, LimitFile, sizeof(Limit_PLG260));
#else
        memcpy(Limit_PLG245, LimitFile, sizeof(Limit_PLG245));
#endif
    } else {
        pr_info("Get limit from file fail!!Use Limit array from image data\n");
//                                                
#if defined(LGE_USE_DOME_KEY)
        memcpy(Limit_PLG260, LimitBack, sizeof(Limit_PLG260));
#else
        memcpy(Limit_PLG245, LimitBack, sizeof(Limit_PLG245));
#endif
    }

    call_cnt++;

    if (call_cnt > 0)
        call_cnt = 1;

    //         
    command = 0x04;
    writeRMI(F01_Ctrl_Base, &command, 1);

    //                                       
    command = (mode == 4) ? 0x02 : 0x03;
    writeRMI(F54_Data_Base, &command, 1);

#if 0
    if (mode == 3 || mode == 4) {
        /*                
                               
                                                       
         */
    }


    if (mode != 3 && mode != 4) {
        //                         
        command = 0x06;
        writeRMI(F54_Command_Base, &command, 1);

        waitcount = 0;
        do {
            if (++waitcount > 100) {
                pr_info("%s[%d], command = %d\n", __func__, __LINE__, command);
                break;
            }
            delayMS(1); //        
            readRMI(F54_Command_Base, &command, 1);
        } while (command != 0x00);
    }
#endif

    //                                                                          
    readRMI(F01_Cmd_Base+1, &command, 1);
    command |= 0x08;
    writeRMI(F01_Cmd_Base+1, &command, 1);

    command = 0x00;
    writeRMI(F54_Data_LowIndex, &command, 1);
    writeRMI(F54_Data_HighIndex, &command, 1);

    //                                          
    command = 0x01;
    writeRMI(F54_Command_Base, &command, 1);

    //                                    
    waitcount = 0;
    do {
        if (++waitcount > 100) {
            pr_info("%s[%d], command = %d\n", __func__, __LINE__, command);
            break;
        }
        delayMS(1); //        
        readRMI(F54_Command_Base, &command, 1);
    } while (command != 0x00);

    //                                                  
    longReadRMI(F54_Data_Buffer, &ImageBuffer[0], length);

    readRMI(F01_Cmd_Base+1, &command, 1);
    command &= ~0x08;
    writeRMI(F01_Cmd_Base+1, &command, 1);

    if ( (numberOfTx > 29) || (numberOfRx > 45) ) {
        ret = sprintf(buf, "ERROR: Limit Index overflow. Test result: Fail\n");
        return ret;
    }

    *buf = 0;

    ret = sprintf(buf, "Info: Tx=%d Rx=%d\n", numberOfTx, numberOfRx);

    switch(mode) {
    case 0:
    case 1:
        //                                                       
        k = 0;
        for (i = 0; i < numberOfTx; i++) {
            //                                    
            for (j = 0; j < numberOfRx; j++) {
                temp = (short)(ImageBuffer[k] | (ImageBuffer[k+1] << 8));
                /*
                                            
                                 
                                                                                 
                                     
                            
                                                                                   
                                     
                     
                        
                             
                 
                */
                ret += sprintf(buf+ret, "%d", temp); //                                  

                if(j < (numberOfRx-1))
                    ret += sprintf(buf+ret, " ");

                k = k + 2;
            }
            ret += sprintf(buf+ret, "\n");
        }
        break;

    case 2:
        k = 0;
        for (i = 0; i < numberOfTx; i++) {
            for (j = 0; j < numberOfRx; j++) {
                temp = (short)(ImageBuffer[k] | (ImageBuffer[k+1] << 8));

                if (temp > TSPCheckLimit) {
                    ret += sprintf(buf+ret, "1");
                    //         
                } else {
                    ret += sprintf(buf+ret, "0");
                }

                if(j < (numberOfRx-1))
                    ret += sprintf(buf+ret, " ");

                k = k + 2;
            }
            ret += sprintf(ret+buf, "\n");
        }
        break;

    case 3:
    case 4:
        k = 0;

        for (i = 0; i < numberOfTx; i++) {
            for (j = 0; j < numberOfRx; j++) {
                ImageArray[i][j] = (short)(ImageBuffer[k] | (ImageBuffer[k+1] << 8));
                k = k + 2;
            }
        }

        for (i = 0; i < numberOfTx; i++) {
            //                                                    
            for (j = 0; j < numberOfRx; j++) {
                ret += sprintf(buf+ret, "%d", ((mode == 3) ? (ImageArray[i][j] / 1000) : (ImageArray[i][j])));

                if(j < (numberOfRx - 1))
                    ret += sprintf(buf+ret, " ");
            }
            ret += sprintf(ret+buf, "\n");
        }
        break;

    default:
        break;
    }
    //             
    /*
     
               
                       
                                            
                       
                   
                                                                                                              
     
    */
    return ret;
}
#endif

