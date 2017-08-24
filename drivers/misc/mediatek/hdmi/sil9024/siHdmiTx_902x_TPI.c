
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/bug.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <mach/hardware.h>
#include "hdmi_drv.h"

#include <mach/mt_typedefs.h>

#include "siHdmiTx_902x_TPI.h"
/*                     */


SIHDMITX_CONFIG siHdmiTx;
GLOBAL_SYSTEM g_sys;
GLOBAL_HDCP g_hdcp;
GLOBAL_EDID g_edid;
byte tpivmode[3];		/*                                           */

bool Sii9024A_HDCP_supported = true;	/*                                                                                                             */
extern HDMI_UTIL_FUNCS hdmi_util;


bool HDCP_Supported = false;

extern void HDMI_reset(void);
extern byte sii9024_i2c_read_byte(byte addr);
extern byte sii9024_i2c_write_byte(struct i2c_client *client, byte addr, byte data);
extern byte sii9024_i2c_read_block(struct i2c_client *client, byte addr, byte *data, word len);
extern int sii9024_i2c_write_block(struct i2c_client *client, byte addr, byte *data, word len);

/*                                                                                */
struct i2c_client *sii902xA = NULL;
struct i2c_client *siiEDID = NULL;
struct i2c_client *siiSegEDID = NULL;
struct i2c_client *siiHDCP = NULL;

/*                                                          */


/*                                                                                */
/*                          */
/*                                                                                                   */
/*  */
/*                                                                   */
/*                                                                                */
void DelayMS(word MS)
{
	msleep(MS);		/*                                      */
}

/*                                                                                */
/*                             */
/*                                                            */
/*                                                                                */
byte I2CReadBlock(struct i2c_client *client, byte RegAddr, byte NBytes, byte *Data)
{
	int i;
	for (i = RegAddr; i < (NBytes + RegAddr); i++) {
		Data[i - RegAddr] = i2c_smbus_read_byte_data(client, i);
	}
	return IIC_OK;
}

static s32 i2c_smbus_write_byte_nostop(struct i2c_client *client, unsigned char value)
{
	unsigned short old_flag = client->addr;

	client->addr = client->addr & I2C_RS_FLAG;
	i2c_smbus_write_byte(client, value);
	client->addr = old_flag;

	return IIC_OK;
}

byte siiReadSegmentBlockEDID(struct i2c_client *client, byte Segment, byte Offset, byte *Buffer,
			     byte Length)
{
	int rc;
	i2c_smbus_write_byte_nostop(siiSegEDID, Segment);
	rc = I2CReadBlock(client, Offset, Length, Buffer);
	if (rc < 0)
		return -EIO;
	return IIC_OK;
}

/*                                                                                */
/*                              */
/*                                */
/*                                                                                */
byte ReadByteTPI(byte RegOffset)
{
	byte Readnum;
	Readnum = i2c_smbus_read_byte_data(sii902xA, RegOffset);
	TPI_DEBUG_PRINT(("[9024]read RegOffset=0x%x,Readnum=0x%x\n", RegOffset, Readnum));
	return Readnum;
}

/*                                                                                */
/*                               */
/*                                 */
/*                                                                                */
void WriteByteTPI(byte RegOffset, byte Data)
{
	/*                                                   */
	i2c_smbus_write_byte_data(sii902xA, RegOffset, Data);

	TPI_DEBUG_PRINT(("[9024]write RegOffset=0x%x,Data=0x%x\n", RegOffset, Data));
}


void ReadSetWriteTPI(byte Offset, byte Pattern)
{
	byte Tmp;

	Tmp = ReadByteTPI(Offset);

	Tmp |= Pattern;
	WriteByteTPI(Offset, Tmp);
}

/*                                                                                */
/*                                  */
/*                                                                                 */
/*                                                       */
/*            */
/*                                                                                */
void ReadClearWriteTPI(byte Offset, byte Pattern)
{
	byte Tmp;

	Tmp = ReadByteTPI(Offset);

	Tmp &= ~Pattern;
	WriteByteTPI(Offset, Tmp);
}

/*                                                                                */
/*                                  */
/*                                                                                     */
/*                                                    */
/*            */
/*                                                                                */
void ReadModifyWriteTPI(byte Offset, byte Mask, byte Value)
{
	byte Tmp;

	Tmp = ReadByteTPI(Offset);
	Tmp &= ~Mask;
	Tmp |= (Value & Mask);
	WriteByteTPI(Offset, Tmp);
}

void ReadBlockTPI(byte TPI_Offset, word NBytes, byte *pData)
{
	int i;
	for (i = TPI_Offset; i < (NBytes + TPI_Offset); i++) {
		pData[i - TPI_Offset] = i2c_smbus_read_byte_data(sii902xA, i);
		/*                                                                                        */
	}
	return;
}


void WriteBlockTPI(byte TPI_Offset, word NBytes, byte *pData)
{
	int i;
	for (i = TPI_Offset; i < (NBytes + TPI_Offset); i++) {

		i2c_smbus_write_byte_data(sii902xA, i, pData[i - TPI_Offset]);
		/*                                                                                      */
	}

}

byte ReadIndexedRegister(byte PageNum, byte RegOffset)
{
	WriteByteTPI(0xBC, PageNum);	/*               */
	WriteByteTPI(0xBD, RegOffset);	/*                  */
	return ReadByteTPI(0xBE);	/*                   */
}

/*                                                                                */
/*                                       */
/*                                                            */
/*  */
/*        */
/*                              */
/*                                    */
/*                                           */
/*                                                                                */
void WriteIndexedRegister(byte PageNum, byte RegOffset, byte RegValue)
{
	WriteByteTPI(0xBC, PageNum);	/*               */
	WriteByteTPI(0xBD, RegOffset);	/*                  */
	WriteByteTPI(0xBE, RegValue);	/*                        */
}

/*                                                                                */
/*                                                 */
/*                                                                                     */
/*                                                    */
/*            */
/*                                                                                */
void ReadModifyWriteIndexedRegister(byte PageNum, byte RegOffset, byte Mask, byte Value)
{
	byte Tmp;
	WriteByteTPI(0xBC, PageNum);
	WriteByteTPI(0xBD, RegOffset);
	Tmp = ReadByteTPI(0xBE);

	Tmp &= ~Mask;
	Tmp |= (Value & Mask);

	WriteByteTPI(0xBE, Tmp);
}

/*                                                                                */
void TXHAL_InitPostReset(void)
{
	/*                              */
	WriteByteTPI(0x82, 0x25);
	/*                            */
	WriteByteTPI(0x7C, 0x14);
}

/*                                                                                */
/*                             */
/*                                         */
/*                                                                                */

void TxHW_Reset(void)
{
	TPI_TRACE_PRINT((">>TxHW_Reset()\n"));

	HDMI_reset();
	TXHAL_InitPostReset();
}

/*                                                                                */
/*                                           */
/*                                                         */
/*                                                                                */
void InitializeStateVariables(void)
{
	g_sys.tmdsPoweredUp = FALSE;
	g_sys.hdmiCableConnected = FALSE;
	g_sys.dsRxPoweredUp = FALSE;

#ifdef DEV_SUPPORT_EDID
	g_edid.edidDataValid = FALSE;
	g_edid.HDMI_compatible_VSDB = FALSE;
#endif
}

/*                                                                                */
/*                             */
/*                                   */
/*                                                                                */
void EnableTMDS(void)
{
	TPI_DEBUG_PRINT(("TMDS -> Enabled\n"));
	ReadModifyWriteTPI(0x1A, TMDS_OUTPUT_CONTROL_MASK, TMDS_OUTPUT_CONTROL_ACTIVE);
	WriteByteTPI(0x08, tpivmode[0]);	/*                     */
	g_sys.tmdsPoweredUp = TRUE;
}

/*                                                                                */
/*                              */
/*                                    */
/*                                                                                */
void DisableTMDS(void)
{
	TPI_DEBUG_PRINT(("TMDS -> Disabled\n"));
	ReadModifyWriteTPI(0x1A, TMDS_OUTPUT_CONTROL_MASK | AV_MUTE_MASK,
			   TMDS_OUTPUT_CONTROL_POWER_DOWN | AV_MUTE_MUTED);
	g_sys.tmdsPoweredUp = FALSE;
}

/*                                                                                */
/*                                   */
/*                                                                              */
/*  */
/*                                                                     */
/*                                                        */
/*               */
/*               */
/*                                                                                */
byte EnableInterrupts(byte Interrupt_Pattern)
{
	TPI_TRACE_PRINT((">>EnableInterrupts()\n"));
	ReadSetWriteTPI(0x3C, Interrupt_Pattern);
	return TRUE;
}

/*                                                                                */
/*                                    */
/*                                                                               */
/*  */
/*                                                                     */
/*                                                            */
/*               */
/*               */
/*                                                                                */
byte DisableInterrupts(byte Interrupt_Pattern)
{
	TPI_TRACE_PRINT((">>DisableInterrupts()\n"));
	ReadClearWriteTPI(0x3C, Interrupt_Pattern);

	return TRUE;
}



#ifdef DEV_SUPPORT_EDID
static u8 g_CommData[EDID_BLOCK_SIZE];

#define ReadBlockEDID(a, b, c)				I2CReadBlock(siiEDID, a, b, c)
#define ReadSegmentBlockEDID(a, b, c, d)		siiReadSegmentBlockEDID(siiEDID, a, b, d, c)

/*                                                                                */
/*                                */
/*                                                                   */
/*  */
/*               */
/*                        */
/*               */
/*                                                                                */
#define T_DDC_ACCESS    50

byte GetDDC_Access(byte *SysCtrlRegVal)
{
	byte sysCtrl;
	byte DDCReqTimeout = T_DDC_ACCESS;
	byte TPI_ControlImage;

	TPI_TRACE_PRINT((">>GetDDC_Access()\n"));

	sysCtrl = ReadByteTPI(0x1A);	/*                                                                 */
	*SysCtrlRegVal = sysCtrl;

	sysCtrl |= DDC_BUS_REQUEST_REQUESTED;
	WriteByteTPI(0x1A, sysCtrl);

	while (DDCReqTimeout--)	/*                             */
	{
		TPI_ControlImage = ReadByteTPI(0x1A);

		if (TPI_ControlImage & DDC_BUS_GRANT_MASK)	/*                        */
		{
			sysCtrl |= DDC_BUS_GRANT_GRANTED;
			WriteByteTPI(0x1A, sysCtrl);	/*                                           */
			return TRUE;
		}
		WriteByteTPI(0x1A, sysCtrl);	/*                                    */
		DelayMS(200);
	}

	WriteByteTPI(0x1A, sysCtrl);	/*                                    */
	return FALSE;
}

/*                                                                                */
/*                             */
/*                                       */
/*  */
/*               */
/*                                                              */
/*               */
/*                                                                                */
byte ReleaseDDC(byte SysCtrlRegVal)
{
	byte DDCReqTimeout = T_DDC_ACCESS;
	byte TPI_ControlImage;

	TPI_TRACE_PRINT((">>ReleaseDDC()\n"));

	SysCtrlRegVal &= ~BITS_2_1;	/*                                                       */

	while (DDCReqTimeout--)	/*                             */
	{
		/*                                                                                                    */
		/*                                                                           */
		/*                                                                                          */

		WriteByteTPI(0x1A, SysCtrlRegVal);
		TPI_ControlImage = ReadByteTPI(0x1A);

		if (!(TPI_ControlImage & BITS_2_1))	/*                         */
			return TRUE;
	}

	return FALSE;		/*                                   */
}

/*                                                                                */
/*                                   */
/*                                                                                 */
/*  */
/*                                    */
/*                        */
/*                    */
/*                                                                                */
byte CheckEDID_Header(byte *Block)
{
	byte i = 0;

	if (Block[i])		/*                  */
		return FALSE;

	for (i = 1; i < 1 + EDID_HDR_NO_OF_FF; i++) {
		if (Block[i] != 0xFF)	/*                           */
			return FALSE;
	}

	if (Block[i])		/*                  */
		return FALSE;

	return TRUE;
}

/*                                                                                */
/*                                  */
/*                                                                                 */
/*                             */
/*  */
/*                                                                            */
/*                        */
/*                    */
/*                                                                                */
byte DoEDID_Checksum(byte *Block)
{
	byte i;
	byte CheckSum = 0;

	for (i = 0; i < EDID_BLOCK_SIZE; i++)
		CheckSum += Block[i];

	if (CheckSum)
		return FALSE;

	return TRUE;
}

/*                                                                                */
/*                                         */
/*                                                                                */
/*                                            */
/*  */
/*                                                                                        */
/*               */
/*                    */
/*                                                                                */
#if (CONF__TPI_EDID_PRINT == ENABLE)
void ParseEstablishedTiming(byte *Data)
{
	TPI_EDID_PRINT(("Parsing Established Timing:\n"));
	TPI_EDID_PRINT(("===========================\n"));

	/*                                  */
	if (Data[ESTABLISHED_TIMING_INDEX] & BIT_7)
		TPI_EDID_PRINT(("720 x 400 @ 70Hz\n"));
	if (Data[ESTABLISHED_TIMING_INDEX] & BIT_6)
		TPI_EDID_PRINT(("720 x 400 @ 88Hz\n"));
	if (Data[ESTABLISHED_TIMING_INDEX] & BIT_5)
		TPI_EDID_PRINT(("640 x 480 @ 60Hz\n"));
	if (Data[ESTABLISHED_TIMING_INDEX] & BIT_4)
		TPI_EDID_PRINT(("640 x 480 @ 67Hz\n"));
	if (Data[ESTABLISHED_TIMING_INDEX] & BIT_3)
		TPI_EDID_PRINT(("640 x 480 @ 72Hz\n"));
	if (Data[ESTABLISHED_TIMING_INDEX] & BIT_2)
		TPI_EDID_PRINT(("640 x 480 @ 75Hz\n"));
	if (Data[ESTABLISHED_TIMING_INDEX] & BIT_1)
		TPI_EDID_PRINT(("800 x 600 @ 56Hz\n"));
	if (Data[ESTABLISHED_TIMING_INDEX] & BIT_0)
		TPI_EDID_PRINT(("800 x 400 @ 60Hz\n"));

	/*                                   */
	if (Data[ESTABLISHED_TIMING_INDEX + 1] & BIT_7)
		TPI_EDID_PRINT(("800 x 600 @ 72Hz\n"));
	if (Data[ESTABLISHED_TIMING_INDEX + 1] & BIT_6)
		TPI_EDID_PRINT(("800 x 600 @ 75Hz\n"));
	if (Data[ESTABLISHED_TIMING_INDEX + 1] & BIT_5)
		TPI_EDID_PRINT(("832 x 624 @ 75Hz\n"));
	if (Data[ESTABLISHED_TIMING_INDEX + 1] & BIT_4)
		TPI_EDID_PRINT(("1024 x 768 @ 87Hz\n"));
	if (Data[ESTABLISHED_TIMING_INDEX + 1] & BIT_3)
		TPI_EDID_PRINT(("1024 x 768 @ 60Hz\n"));
	if (Data[ESTABLISHED_TIMING_INDEX + 1] & BIT_2)
		TPI_EDID_PRINT(("1024 x 768 @ 70Hz\n"));
	if (Data[ESTABLISHED_TIMING_INDEX + 1] & BIT_1)
		TPI_EDID_PRINT(("1024 x 768 @ 75Hz\n"));
	if (Data[ESTABLISHED_TIMING_INDEX + 1] & BIT_0)
		TPI_EDID_PRINT(("1280 x 1024 @ 75Hz\n"));

	/*                                   */
	if (Data[ESTABLISHED_TIMING_INDEX + 2] & 0x80)
		TPI_EDID_PRINT(("1152 x 870 @ 75Hz\n"));

	if ((!Data[0]) && (!Data[ESTABLISHED_TIMING_INDEX + 1]) && (!Data[2]))
		TPI_EDID_PRINT(("No established video modes\n"));
}

/*                                                                                */
/*                                      */
/*                                                                             */
/*                                            */
/*  */
/*                                                                                        */
/*               */
/*                    */
/*                                                                                */
void ParseStandardTiming(byte *Data)
{
	byte i;
	byte AR_Code;

	TPI_EDID_PRINT(("Parsing Standard Timing:\n"));
	TPI_EDID_PRINT(("========================\n"));

	for (i = 0; i < NUM_OF_STANDARD_TIMINGS; i += 2) {
		if ((Data[STANDARD_TIMING_OFFSET + i] == 0x01)
		    && ((Data[STANDARD_TIMING_OFFSET + i + 1]) == 1)) {
			TPI_EDID_PRINT(("Standard Timing Undefined\n"));	/*                                                                           */
		} else {
			TPI_EDID_PRINT(("Horizontal Active pixels: %i\n", (int)((Data[STANDARD_TIMING_OFFSET + i] + 31) * 8)));	/*                                                                             */

			AR_Code = (Data[STANDARD_TIMING_OFFSET + i + 1] & TWO_MSBITS) >> 6;
			TPI_EDID_PRINT(("Aspect Ratio: "));

			switch (AR_Code) {
			case AR16_10:
				TPI_EDID_PRINT(("16:10\n"));
				break;

			case AR4_3:
				TPI_EDID_PRINT(("4:3\n"));
				break;

			case AR5_4:
				TPI_EDID_PRINT(("5:4\n"));
				break;

			case AR16_9:
				TPI_EDID_PRINT(("16:9\n"));
				break;
			}
		}
	}
}

/*                                                                                */
/*                                      */
/*                                                                             */
/*                                            */
/*  */
/*                                                                                        */
/*                                                                 */
/*  */
/*                                                                   */
/*               */
/*                    */
/*                                                                                */
byte ParseDetailedTiming(byte *Data, byte DetailedTimingOffset, byte Block)
{
	byte TmpByte;
	byte i;
	word TmpWord;

	TmpWord = Data[DetailedTimingOffset + PIX_CLK_OFFSET] +
	    256 * Data[DetailedTimingOffset + PIX_CLK_OFFSET + 1];

	if (TmpWord == 0x00)	/*                                                                                                  */
	{
		if (Block == EDID_BLOCK_0)	/*                                                                                                    */
		{
			if (Data[DetailedTimingOffset + 3] == 0xFC)	/*                                             */
			{
				TPI_EDID_PRINT(("Monitor Name: "));

				for (i = 0; i < 13; i++) {
					TPI_EDID_PRINT(("%c", Data[DetailedTimingOffset + 5 + i]));	/*                                */
				}
				TPI_EDID_PRINT(("\n"));
			}

			else if (Data[DetailedTimingOffset + 3] == 0xFD)	/*                                                           */
			{
				TPI_EDID_PRINT(("Monitor Range Limits:\n\n"));

				i = 0;
				TPI_EDID_PRINT(("Min Vertical Rate in Hz: %d\n", (int)Data[DetailedTimingOffset + 5 + i++]));	/*  */
				TPI_EDID_PRINT(("Max Vertical Rate in Hz: %d\n", (int)Data[DetailedTimingOffset + 5 + i++]));	/*  */
				TPI_EDID_PRINT(("Min Horizontal Rate in Hz: %d\n", (int)Data[DetailedTimingOffset + 5 + i++]));	/*  */
				TPI_EDID_PRINT(("Max Horizontal Rate in Hz: %d\n", (int)Data[DetailedTimingOffset + 5 + i++]));	/*  */
				TPI_EDID_PRINT(("Max Supported pixel clock rate in MHz/10: %d\n", (int)Data[DetailedTimingOffset + 5 + i++]));	/*  */
				TPI_EDID_PRINT(("Tag for secondary timing formula (00h=not used): %d\n", (int)Data[DetailedTimingOffset + 5 + i++]));	/*  */
				TPI_EDID_PRINT(("Min Vertical Rate in Hz %d\n", (int)Data[DetailedTimingOffset + 5 + i]));	/*  */
				TPI_EDID_PRINT(("\n"));
			}
		}

		else if (Block == EDID_BLOCK_2_3)	/*                                                                              */
		{		/*                                                                */
			TPI_EDID_PRINT(("No More Detailed descriptors in this block\n"));
			TPI_EDID_PRINT(("\n"));
			return FALSE;
		}
	}

	else			/*                                                                                   */
	{
		if ((Block == EDID_BLOCK_0) && (DetailedTimingOffset == 0x36)) {
			TPI_EDID_PRINT(("\n\n\nParse Results, EDID Block #0, Detailed Descriptor Number 1:\n"));
			TPI_EDID_PRINT(("===========================================================\n\n"));
		} else if ((Block == EDID_BLOCK_0) && (DetailedTimingOffset == 0x48)) {
			TPI_EDID_PRINT(("\n\n\nParse Results, EDID Block #0, Detailed Descriptor Number 2:\n"));
			TPI_EDID_PRINT(("===========================================================\n\n"));
		}

		TPI_EDID_PRINT(("Pixel Clock (MHz * 100): %d\n", (int)TmpWord));

		TmpWord = Data[DetailedTimingOffset + H_ACTIVE_OFFSET] +
		    256 * ((Data[DetailedTimingOffset + H_ACTIVE_OFFSET + 2] >> 4) & FOUR_LSBITS);
		TPI_EDID_PRINT(("Horizontal Active Pixels: %d\n", (int)TmpWord));

		TmpWord = Data[DetailedTimingOffset + H_BLANKING_OFFSET] +
		    256 * (Data[DetailedTimingOffset + H_BLANKING_OFFSET + 1] & FOUR_LSBITS);
		TPI_EDID_PRINT(("Horizontal Blanking (Pixels): %d\n", (int)TmpWord));

		TmpWord = (Data[DetailedTimingOffset + V_ACTIVE_OFFSET]) +
		    256 * ((Data[DetailedTimingOffset + (V_ACTIVE_OFFSET) + 2] >> 4) & FOUR_LSBITS);
		TPI_EDID_PRINT(("Vertical Active (Lines): %d\n", (int)TmpWord));

		TmpWord = Data[DetailedTimingOffset + V_BLANKING_OFFSET] +
		    256 * (Data[DetailedTimingOffset + V_BLANKING_OFFSET + 1] & LOW_NIBBLE);
		TPI_EDID_PRINT(("Vertical Blanking (Lines): %d\n", (int)TmpWord));

		TmpWord = Data[DetailedTimingOffset + H_SYNC_OFFSET] +
		    256 * ((Data[DetailedTimingOffset + (H_SYNC_OFFSET + 3)] >> 6) & TWO_LSBITS);
		TPI_EDID_PRINT(("Horizontal Sync Offset (Pixels): %d\n", (int)TmpWord));

		TmpWord = Data[DetailedTimingOffset + H_SYNC_PW_OFFSET] +
		    256 * ((Data[DetailedTimingOffset + (H_SYNC_PW_OFFSET + 2)] >> 4) & TWO_LSBITS);
		TPI_EDID_PRINT(("Horizontal Sync Pulse Width (Pixels): %d\n", (int)TmpWord));

		TmpWord = (Data[DetailedTimingOffset + V_SYNC_OFFSET] >> 4) & FOUR_LSBITS +
		    256 * ((Data[DetailedTimingOffset + (V_SYNC_OFFSET + 1)] >> 2) & TWO_LSBITS);
		TPI_EDID_PRINT(("Vertical Sync Offset (Lines): %d\n", (int)TmpWord));

		TmpWord = (Data[DetailedTimingOffset + V_SYNC_PW_OFFSET]) & FOUR_LSBITS +
		    256 * (Data[DetailedTimingOffset + (V_SYNC_PW_OFFSET + 1)] & TWO_LSBITS);
		TPI_EDID_PRINT(("Vertical Sync Pulse Width (Lines): %d\n", (int)TmpWord));

		TmpWord = Data[DetailedTimingOffset + H_IMAGE_SIZE_OFFSET] +
		    256 *
		    (((Data[DetailedTimingOffset + (H_IMAGE_SIZE_OFFSET + 2)]) >> 4) & FOUR_LSBITS);
		TPI_EDID_PRINT(("Horizontal Image Size (mm): %d\n", (int)TmpWord));

		TmpWord = Data[DetailedTimingOffset + V_IMAGE_SIZE_OFFSET] +
		    256 * (Data[DetailedTimingOffset + (V_IMAGE_SIZE_OFFSET + 1)] & FOUR_LSBITS);
		TPI_EDID_PRINT(("Vertical Image Size (mm): %d\n", (int)TmpWord));

		TmpByte = Data[DetailedTimingOffset + H_BORDER_OFFSET];
		TPI_EDID_PRINT(("Horizontal Border (Pixels): %d\n", (int)TmpByte));

		TmpByte = Data[DetailedTimingOffset + V_BORDER_OFFSET];
		TPI_EDID_PRINT(("Vertical Border (Lines): %d\n", (int)TmpByte));

		TmpByte = Data[DetailedTimingOffset + FLAGS_OFFSET];

		TPI_EDID_PRINT(("\n"));
	}
	return TRUE;
}

/*                                                                                */
/*                                                 */
/*                                                                           */
/*                                      */
/*  */
/*                                                                                        */
/*               */
/*                    */
/*                                                                                */
void ParseBlock_0_TimingDescriptors(byte *Data)
{
	byte i;
	byte Offset;

	ParseEstablishedTiming(Data);
	ParseStandardTiming(Data);

	for (i = 0; i < NUM_OF_DETAILED_DESCRIPTORS; i++) {
		Offset = DETAILED_TIMING_OFFSET + (LONG_DESCR_LEN * i);
		ParseDetailedTiming(Data, Offset, EDID_BLOCK_0);
	}
}
#endif

/*                                                                                */
/*                            */
/*                                                                                   */
/*                          */
/*  */
/*               */
/*                        */
/*                    */
/*                                                                                         */
/*                                                                                */
byte ParseEDID(byte *pEdid, byte *numExt)
{
	byte i, j, k;

	TPI_EDID_PRINT(("\n"));
	TPI_EDID_PRINT(("EDID DATA (Segment = 0 Block = 0 Offset = %d):\n",
			(int)EDID_BLOCK_0_OFFSET));

	for (j = 0, i = 0; j < 128; j++) {
		k = pEdid[j];
		TPI_EDID_PRINT(("%2.2X ", (int)k));
		i++;

		if (i == 0x10) {
			TPI_EDID_PRINT(("\n"));
			i = 0;
		}
	}
	TPI_EDID_PRINT(("\n"));

	if (!CheckEDID_Header(pEdid))	/*                                                                                                                          */
	{
		/*                                                              */
		TPI_DEBUG_PRINT(("EDID -> Incorrect Header\n"));
		return EDID_INCORRECT_HEADER;
	}

	if (!DoEDID_Checksum(pEdid)) {
		/*                        */
		TPI_DEBUG_PRINT(("EDID -> Checksum Error\n"));
		return EDID_CHECKSUM_ERROR;
	}
#if (CONF__TPI_EDID_PRINT == ENABLE)
	ParseBlock_0_TimingDescriptors(pEdid);	/*                                 */
#endif

	*numExt = pEdid[NUM_OF_EXTEN_ADDR];	/*                                                  */
	TPI_EDID_PRINT(("EDID -> 861 Extensions = %d\n", (int)*numExt));

	if (!(*numExt)) {
		/*                              */
		return EDID_NO_861_EXTENSIONS;
	}
	/*                                                                                                                     */
	return (EDID_OK);
}

/*                                                                                */
/*                                           */
/*                                                                                   */
/*                                                                 */
/*  */
/*                                                                  */
/*                                                                         */
/*                    */
/*                                                                                         */
/*                                                                                */
byte Parse861ShortDescriptors(byte *Data)
{
	byte LongDescriptorOffset;
	byte DataBlockLength;
    byte DataIndex,DataIndexbk;
	byte ExtendedTagCode;
	byte VSDB_BaseOffset = 0;

	byte V_DescriptorIndex = 0;	/*                                           */
	byte A_DescriptorIndex = 0;	/*                                           */

	byte TagCode;

	byte i;
	byte j;

	if (Data[EDID_TAG_ADDR] != EDID_EXTENSION_TAG) {
		TPI_EDID_PRINT(("EDID -> Extension Tag Error\n"));
		return EDID_EXT_TAG_ERROR;
	}

	if (Data[EDID_REV_ADDR] != EDID_REV_THREE) {
		TPI_EDID_PRINT(("EDID -> Revision Error\n"));
		return EDID_REV_ADDR_ERROR;
	}

	LongDescriptorOffset = Data[LONG_DESCR_PTR_IDX];	/*                                           */

	g_edid.UnderScan = ((Data[MISC_SUPPORT_IDX]) >> 7) & LSBIT;	/*                                    */
	g_edid.BasicAudio = ((Data[MISC_SUPPORT_IDX]) >> 6) & LSBIT;
	/*                                                               */
	/*                                                               */
	g_edid.YCbCr_4_4_4 = false;
	g_edid.YCbCr_4_2_2 = false;

	DataIndex = EDID_DATA_START;	/*   */

    while (DataIndex < LongDescriptorOffset)
    {
    	DataIndexbk = DataIndex;
		TPI_EDID_PRINT(("Data[0x%x]: 0x%x\n", (int)DataIndex,(int)Data[DataIndex]));
		TagCode = (Data[DataIndex] >> 5) & THREE_LSBITS;
		DataBlockLength = Data[DataIndex++] & FIVE_LSBITS;
		TPI_EDID_PRINT(("Data[0x%x]: 0x%x. TagCode=0x%x,DataBlockLengt=0x%x\n", (int)DataIndex-1,(int)Data[DataIndex-1],TagCode,DataBlockLength));
        if ((DataIndex + DataBlockLength) > LongDescriptorOffset)
        {
			TPI_EDID_PRINT(("EDID -> V Descriptor Overflow\n"));
			return EDID_V_DESCR_OVERFLOW;
		}

		i = 0;		/*                                                      */

		switch (TagCode) {
		case VIDEO_D_BLOCK:
			while ((i < DataBlockLength) && (i < MAX_V_DESCRIPTORS))	/*                         */
			{
				g_edid.VideoDescriptor[V_DescriptorIndex++] = Data[DataIndex++];
				i++;
			}
			DataIndex += DataBlockLength - i;	/*                                                                                     */

			TPI_EDID_PRINT(("EDID -> Short Descriptor Video Block\n"));
			break;

		case AUDIO_D_BLOCK:
			while (i < DataBlockLength / 3)	/*                          */
			{
				j = 0;
				while (j < AUDIO_DESCR_SIZE)	/*   */
				{
					g_edid.AudioDescriptor[A_DescriptorIndex][j++] =
					    Data[DataIndex++];
				}
				A_DescriptorIndex++;
				i++;
			}
			TPI_EDID_PRINT(("EDID -> Short Descriptor Audio Block\n"));
			break;

		case SPKR_ALLOC_D_BLOCK:
			g_edid.SpkrAlloc[i++] = Data[DataIndex++];	/*                                                           */
			DataIndex += 2;	/*                                                                              */
			TPI_EDID_PRINT(("EDID -> Short Descriptor Speaker Allocation Block\n"));
			break;

		case USE_EXTENDED_TAG:
			ExtendedTagCode = Data[DataIndex++];

			switch (ExtendedTagCode) {
			case VIDEO_CAPABILITY_D_BLOCK:
				TPI_EDID_PRINT(("EDID -> Short Descriptor Video Capability Block\n"));

				/*                                                                               */
				/*                                          */
				/*                                                */
				DataIndex += 1;	/*                                                                                                     */
				break;

			case COLORIMETRY_D_BLOCK:
				g_edid.ColorimetrySupportFlags = Data[DataIndex++] & BITS_1_0;
				g_edid.MetadataProfile = Data[DataIndex++] & BITS_2_1_0;

				TPI_EDID_PRINT(("EDID -> Short Descriptor Colorimetry Block\n"));
				break;
			}
				DataIndex = DataIndexbk + DataBlockLength+1;
			break;

		case VENDOR_SPEC_D_BLOCK:
			VSDB_BaseOffset = DataIndex - 1;

                if ((Data[DataIndex++] == 0x03) &&    //                                 
                    (Data[DataIndex++] == 0x0C) &&
                    (Data[DataIndex++] == 0x00)){
					g_edid.HDMI_compatible_VSDB = true;
                    //                        
                	}
               //     
                    //                         



			g_edid.CEC_A_B = Data[DataIndex++];	/*                      */
			g_edid.CEC_C_D = Data[DataIndex++];

#ifdef DEV_SUPPORT_CEC
			/*                                                               */
			/*                                      */
			{
				word phyAddr;
				phyAddr = (word) g_edid.CEC_C_D;	/*                   */
				phyAddr |= ((word) g_edid.CEC_A_B << 8);	/*                  */
				/*                                              */
				if (phyAddr != SI_CecGetDevicePA()) {
					/*                        */
					SI_CecSetDevicePA(phyAddr);
				}
			}
#endif

			if ((DataIndex + 7) > VSDB_BaseOffset + DataBlockLength)	/*                                  */
				g_edid._3D_Supported = FALSE;
			else if (Data[DataIndex + 7] >> 7)
				g_edid._3D_Supported = TRUE;
			else
				g_edid._3D_Supported = FALSE;
				TPI_EDID_PRINT(("DataIndexbk=0x%x,DataBlockLength=0x%x\n",DataIndexbk,DataBlockLength));

                //                                                                                                      
               
				DataIndex = DataIndexbk + DataBlockLength+1;
				
                TPI_EDID_PRINT(("DataIndex=0x%x!!!!!!!!!\n",DataIndex));
			TPI_EDID_PRINT(("EDID -> Short Descriptor Vendor Block\n"));
			TPI_EDID_PRINT(("\n"));
			break;

		default:
			TPI_EDID_PRINT(("EDID -> Unknown Tag Code\n"));
			return EDID_UNKNOWN_TAG_CODE;

		}		/*                       */
	}			/*                                                         */

	return EDID_SHORT_DESCRIPTORS_OK;
}

/*                                                                                */
/*                                          */
/*                                                                                  */
/*                                                        */
/*  */
/*                                                   */
/*                                                                                           */
/*               */
/*                                                                                */
byte Parse861LongDescriptors(byte *Data)
{
	byte LongDescriptorsOffset;
	byte DescriptorNum = 1;

	LongDescriptorsOffset = Data[LONG_DESCR_PTR_IDX];	/*                                      */

	if (!LongDescriptorsOffset)	/*                         */
	{
		TPI_DEBUG_PRINT(("EDID -> No Detailed Descriptors\n"));
		return EDID_NO_DETAILED_DESCRIPTORS;
	}
	/*                               */
	while (LongDescriptorsOffset + LONG_DESCR_LEN < EDID_BLOCK_SIZE) {
		TPI_EDID_PRINT(("Parse Results - CEA-861 Long Descriptor #%d:\n",
				(int)DescriptorNum));
		TPI_EDID_PRINT(("===============================================================\n"));

#if (CONF__TPI_EDID_PRINT == ENABLE)
		if (!ParseDetailedTiming(Data, LongDescriptorsOffset, EDID_BLOCK_2_3))
			break;
#endif
		LongDescriptorsOffset += LONG_DESCR_LEN;
		DescriptorNum++;
	}

	return EDID_LONG_DESCRIPTORS_OK;
}

/*                                                                                */
/*                                     */
/*                                                                                  */
/*                                                       */
/*                                                  */
/*  */
/*                                                            */
/*                                                                         */
/*                    */
/*                                                                                         */
/*                                                                                */
byte Parse861Extensions(byte NumOfExtensions)
{
	byte i, j, k;

	byte ErrCode;

	/*                             */
	/*                             */

	byte Segment = 0;
	byte Block = 0;
	byte Offset = 0;

	g_edid.HDMI_Sink = FALSE;
    g_edid.HDMI_compatible_VSDB = FALSE;	         
    do
    {
		Block++;

		Offset = 0;
		if ((Block % 2) > 0) {
			Offset = EDID_BLOCK_SIZE;
		}

		Segment = (byte) (Block / 2);

		if (Block == 1) {
			ReadBlockEDID(EDID_BLOCK_1_OFFSET, EDID_BLOCK_SIZE, g_CommData);	/*                                  */
		} else {
			ReadSegmentBlockEDID(Segment, Offset, EDID_BLOCK_SIZE, g_CommData);	/*                                 */
		}

		TPI_TRACE_PRINT(("\n"));
		TPI_TRACE_PRINT(("EDID DATA (Segment = %d Block = %d Offset = %d):\n", (int)Segment,
				 (int)Block, (int)Offset));
		for (j = 0, i = 0; j < 128; j++) {
			k = g_CommData[j];
			TPI_EDID_PRINT(("%2.2X ", (int)k));
			i++;

			if (i == 0x10) {
				TPI_EDID_PRINT(("\n"));
				i = 0;
			}
		}
		TPI_EDID_PRINT(("\n"));

		if ((NumOfExtensions > 1) && (Block == 1)) {
			continue;
		}

		ErrCode = Parse861ShortDescriptors(g_CommData);
		if (ErrCode != EDID_SHORT_DESCRIPTORS_OK) {
			return ErrCode;
		}

		ErrCode = Parse861LongDescriptors(g_CommData);
		if (ErrCode != EDID_LONG_DESCRIPTORS_OK) {
			return ErrCode;
		}

	} while (Block < NumOfExtensions);

	return EDID_OK;
}

/*                                                                                */
/*                             */
/*                                       */
/*  */
/*               */
/*                        */
/*               */
/*                                                                                */
byte DoEdidRead(void)
{
	byte SysCtrlReg;
	byte Result;
	byte NumOfExtensions;

	/*                                                           */
	if (g_edid.edidDataValid == FALSE) {
		/*                                             */
		if (GetDDC_Access(&SysCtrlReg)) {
			ReadBlockEDID(EDID_BLOCK_0_OFFSET, EDID_BLOCK_SIZE, g_CommData);	/*                                  */
			Result = ParseEDID(g_CommData, &NumOfExtensions);
			if (Result != EDID_OK) {
				if (Result == EDID_NO_861_EXTENSIONS) {
					TPI_DEBUG_PRINT(("EDID -> No 861 Extensions\n"));
					g_edid.HDMI_Sink = FALSE;
					g_edid.YCbCr_4_4_4 = FALSE;
					g_edid.YCbCr_4_2_2 = FALSE;
					g_edid.CEC_A_B = 0x00;
					g_edid.CEC_C_D = 0x00;
				} else {
					TPI_DEBUG_PRINT(("EDID -> Parse FAILED\n"));
					g_edid.HDMI_Sink = TRUE;
					g_edid.YCbCr_4_4_4 = FALSE;
					g_edid.YCbCr_4_2_2 = FALSE;
					g_edid.CEC_A_B = 0x00;
					g_edid.CEC_C_D = 0x00;
				}
			} else {
				TPI_DEBUG_PRINT(("EDID -> Parse OK\n"));
				Result = Parse861Extensions(NumOfExtensions);	/*                                                    */
				if (Result != EDID_OK) {
					TPI_DEBUG_PRINT(("EDID -> Extension Parse FAILED\n"));
					g_edid.HDMI_Sink = false;/*                          */
					g_edid.YCbCr_4_4_4 = FALSE;
					g_edid.YCbCr_4_2_2 = FALSE;
					g_edid.CEC_A_B = 0x00;
					g_edid.CEC_C_D = 0x00;
				}
				if(g_edid.HDMI_compatible_VSDB)
					g_edid.HDMI_Sink = TRUE;//                        
			}

			if (!ReleaseDDC(SysCtrlReg))	/*                                                        */
			{
				TPI_DEBUG_PRINT(("EDID -> DDC bus release failed\n"));
				return EDID_DDC_BUS_RELEASE_FAILURE;
			}
		} else {
			TPI_DEBUG_PRINT(("EDID -> DDC bus request failed\n"));
			g_edid.HDMI_Sink = TRUE;
			g_edid.YCbCr_4_4_4 = FALSE;
			g_edid.YCbCr_4_2_2 = FALSE;
			g_edid.CEC_A_B = 0x00;
			g_edid.CEC_C_D = 0x00;
			return EDID_DDC_BUS_REQ_FAILURE;
		}

		TPI_DEBUG_PRINT(("EDID -> g_edid.HDMI_Sink = %d\n", (int)g_edid.HDMI_Sink));
		TPI_DEBUG_PRINT(("EDID -> g_edid.YCbCr_4_4_4 = %d\n", (int)g_edid.YCbCr_4_4_4));
		TPI_DEBUG_PRINT(("EDID -> g_edid.YCbCr_4_2_2 = %d\n", (int)g_edid.YCbCr_4_2_2));
		TPI_DEBUG_PRINT(("EDID -> g_edid.CEC_A_B = 0x%x\n", (int)g_edid.CEC_A_B));
		TPI_DEBUG_PRINT(("EDID -> g_edid.CEC_C_D = 0x%x\n", (int)g_edid.CEC_C_D));

		g_edid.edidDataValid = TRUE;
	}
	return EDID_OK;
}

#endif


/*                                                                               */
/*                                                                               */
/*                                              *///                               
/*                                                                                             */
/*                                              *///                               
/*                                                                               */
/*                                                                               */
/*                                                                                */
/*                                   */
/*                                                                                 */
/*                                                             */
/*  */
/*               */
/*                                                  */
/*               */
/*                                                                                */
byte IsHDCP_Supported(void)
{
	byte HDCP_Rev;
	byte HDCP_Supported;

	TPI_TRACE_PRINT((">>IsHDCP_Supported()\n"));

	HDCP_Supported = TRUE;

	/*                 */
	HDCP_Rev = ReadByteTPI(0x30);

	if (HDCP_Rev != (HDCP_MAJOR_REVISION_VALUE | HDCP_MINOR_REVISION_VALUE)) {
		HDCP_Supported = FALSE;
	}
	/*                                                                                            */
	HDCP_Rev = ReadByteTPI(0x36);
	if (HDCP_Rev == 0x09) {
		HDCP_Rev = ReadByteTPI(0x37);
		if (HDCP_Rev == 0x00) {
			HDCP_Rev = ReadByteTPI(0x38);
			if (HDCP_Rev == 0x02) {
				HDCP_Rev = ReadByteTPI(0x39);
				if (HDCP_Rev == 0x02) {
					HDCP_Rev = ReadByteTPI(0x3A);
					if (HDCP_Rev == 0x0a) {
						HDCP_Supported = FALSE;
						TPI_TRACE_PRINT((">>sii902xA found, NO HDCP supported\n"));
					}
				}
			}
		}
	}
	return HDCP_Supported;
}

/*                                                                                */
/*                             */
/*                                                                */
/*  */
/*               */
/*                                                                      */
/*               */
/*                                                                                */
byte AreAKSV_OK(void)
{
	byte B_Data[AKSV_SIZE];
	byte NumOfOnes = 0;
	byte i, j;

	TPI_TRACE_PRINT((">>AreAKSV_OK()\n"));

	ReadBlockTPI(0x36, AKSV_SIZE, B_Data);

	for (i = 0; i < AKSV_SIZE; i++) {
		for (j = 0; j < BYTE_SIZE; j++) {
			if (B_Data[i] & 0x01) {
				NumOfOnes++;
			}
			B_Data[i] >>= 1;
		}
	}
	if (NumOfOnes != NUM_OF_ONES_IN_KSV)
		return FALSE;

	return TRUE;
}

/*                                                                                */
/*                           */
/*                                        */
/*                                                                                */
void HDCP_Off(void)
{
	TPI_TRACE_PRINT((">>HDCP_Off()\n"));

	/*         */
	ReadModifyWriteTPI(0x1A, AV_MUTE_MASK, AV_MUTE_MUTED);
	WriteByteTPI(0x2A, PROTECTION_LEVEL_MIN);

	g_hdcp.HDCP_Started = FALSE;
	g_hdcp.HDCP_LinkProtectionLevel =
	    EXTENDED_LINK_PROTECTION_NONE | LOCAL_LINK_PROTECTION_NONE;
}

/*                                                                                */
/*                          */
/*                                       */
/*                                                                                */
void HDCP_On(void)
{
	if (g_hdcp.HDCP_Override == FALSE) {
		TPI_DEBUG_PRINT(("HDCP Started\n"));

		WriteByteTPI(0x2A, PROTECTION_LEVEL_MAX);

		g_hdcp.HDCP_Started = TRUE;
	} else {
		g_hdcp.HDCP_Started = FALSE;
	}
}

/*                                                                                */
/*                              */
/*                                     */
/*                                                                                */
void RestartHDCP(void)
{
	TPI_DEBUG_PRINT(("HDCP -> Restart\n"));

	DisableTMDS();
	HDCP_Off();
	EnableTMDS();
}

/*                                                                                */
/*                            */
/*                                                                            */
/*                                                     */
/*  */
/*               */
/*                                                                   */
/*                                                                                          */
/*                                                                                             */
/*                                     */
/*                                                                                                        */
/*                                                                                */
void HDCP_Init(void)
{
	TPI_TRACE_PRINT((">>HDCP_Init()\n"));

	g_hdcp.HDCP_TxSupports = FALSE;
	g_hdcp.HDCP_AksvValid = FALSE;
	g_hdcp.HDCP_Started = FALSE;
	g_hdcp.HDCP_LinkProtectionLevel =
	    EXTENDED_LINK_PROTECTION_NONE | LOCAL_LINK_PROTECTION_NONE;

	/*                                               */
	if (!IsHDCP_Supported()) {
		/*                                                                          */
		/*                                                 */
		TPI_DEBUG_PRINT(("HDCP -> TX does not support HDCP\n"));
		return;
	}
	g_hdcp.HDCP_TxSupports = TRUE;

	/*                                               */
	if (!AreAKSV_OK()) {
		/*                                                      */
		/*                          */
		TPI_DEBUG_PRINT(("HDCP -> Illegal AKSV\n"));
		return;
	}
	g_hdcp.HDCP_AksvValid = TRUE;

#ifdef KSVFORWARD
	/*                                                                */
	ReadModifyWriteTPI(0x2A, KSV_FORWARD_MASK, KSV_FORWARD_ENABLE);
	ReadModifyWriteTPI(0x3E _EN, KSV_FIFO_READY_EN_MASK, KSV_FIFO_READY_ENABLE);
#endif

	TPI_DEBUG_PRINT(("HDCP -> Supported by TX, AKSVs valid\n"));
}

#ifdef READKSV
/*                                                                                */
/*                             */
/*                                                   */
/*  */
/*               */
/*                                                    */
/*               */
/*                                                                                */
byte IsRepeater(void)
{
	byte RegImage;

	TPI_TRACE_PRINT((">>IsRepeater()\n"));

	RegImage = ReadByteTPI(0x29);

	if (RegImage & HDCP_REPEATER_MASK)
		return TRUE;

	return FALSE;		/*                */
}

/*                                                                                */
/*                                */
/*                                                                              */
/*                                       */
/*  */
/*                                                                                           */
/*                             */
/*               */
/*               */
/*                                                                                */
void ReadBlockHDCP(byte TPI_Offset, word NBytes, byte *pData)
{
	I2CReadBlock(siiHDCP, TPI_Offset, NBytes, pData);
}

/*                                                                                */
/*                         */
/*                                                                    */
/*  */
/*               */
/*                                                             */
/*                                                                                               */
/*                                                                                */
byte GetKSV(void)
{
	byte i;
	word KeyCount;
	byte KSV_Array[KSV_ARRAY_SIZE];

	TPI_TRACE_PRINT((">>GetKSV()\n"));
	ReadBlockHDCP(DDC_BSTATUS_ADDR_L, 1, &i);
	KeyCount = (i & DEVICE_COUNT_MASK) * 5;
	if (KeyCount != 0) {
		ReadBlockHDCP(DDC_KSV_FIFO_ADDR, KeyCount, KSV_Array);
	}



	return TRUE;
}
#endif

/*                                                                                */
/*                                   */
/*                                          */
/*  */
/*                          */
/*               */
/*               */
/*                                                                                */
void HDCP_CheckStatus(byte InterruptStatusImage)
{
	byte QueryData;
	byte LinkStatus;
	byte RegImage;
	byte NewLinkProtectionLevel;

#ifdef READKSV
	byte RiCnt;
#endif
#ifdef KSVFORWARD
	byte ksv;
#endif

	if ((g_hdcp.HDCP_TxSupports == TRUE) && (g_hdcp.HDCP_AksvValid == TRUE)) {
		if ((g_hdcp.HDCP_LinkProtectionLevel ==
		     (EXTENDED_LINK_PROTECTION_NONE | LOCAL_LINK_PROTECTION_NONE))
		    && (g_hdcp.HDCP_Started == FALSE)) {
			QueryData = ReadByteTPI(0x29);

			if (QueryData & PROTECTION_TYPE_MASK)	/*                    */
			{
				HDCP_On();
			}
		}
		/*                                   */
		if (InterruptStatusImage & SECURITY_CHANGE_EVENT) {
			TPI_DEBUG_PRINT(("HDCP -> "));

			LinkStatus = ReadByteTPI(0x29);
			LinkStatus &= LINK_STATUS_MASK;

			ClearInterrupt(SECURITY_CHANGE_EVENT);

			switch (LinkStatus) {
			case LINK_STATUS_NORMAL:
				TPI_DEBUG_PRINT(("Link = Normal\n"));
				break;

			case LINK_STATUS_LINK_LOST:
				TPI_DEBUG_PRINT(("Link = Lost\n"));
				RestartHDCP();
				break;

			case LINK_STATUS_RENEGOTIATION_REQ:
				TPI_DEBUG_PRINT(("Link = Renegotiation Required\n"));
				HDCP_Off();
				HDCP_On();
				break;

			case LINK_STATUS_LINK_SUSPENDED:
				TPI_DEBUG_PRINT(("Link = Suspended\n"));
				HDCP_On();
				break;
			}
		}
		/*                                  */
		if (InterruptStatusImage & HDCP_CHANGE_EVENT) {
			RegImage = ReadByteTPI(0x29);

			NewLinkProtectionLevel =
			    RegImage & (EXTENDED_LINK_PROTECTION_MASK | LOCAL_LINK_PROTECTION_MASK);
			if (NewLinkProtectionLevel != g_hdcp.HDCP_LinkProtectionLevel) {
				TPI_DEBUG_PRINT(("HDCP -> "));

				g_hdcp.HDCP_LinkProtectionLevel = NewLinkProtectionLevel;

				switch (g_hdcp.HDCP_LinkProtectionLevel) {
				case (EXTENDED_LINK_PROTECTION_NONE | LOCAL_LINK_PROTECTION_NONE):
					TPI_DEBUG_PRINT(("Protection = None\n"));
					RestartHDCP();
					break;

				case LOCAL_LINK_PROTECTION_SECURE:

					if (IsHDMI_Sink()) {
						ReadModifyWriteTPI(0x26, AUDIO_MUTE_MASK,
								   AUDIO_MUTE_NORMAL);
					}

					ReadModifyWriteTPI(0x1A, AV_MUTE_MASK, AV_MUTE_NORMAL);
					TPI_DEBUG_PRINT(("Protection = Local, Video Unmuted\n"));
					break;

				case (EXTENDED_LINK_PROTECTION_SECURE | LOCAL_LINK_PROTECTION_SECURE):
					TPI_DEBUG_PRINT(("Protection = Extended\n"));
#ifdef READKSV
					if (IsRepeater()) {
						RiCnt = ReadIndexedRegister(INDEXED_PAGE_0, 0x25);
						while (RiCnt > 0x70)	/*           */
						{
							RiCnt =
							    ReadIndexedRegister(INDEXED_PAGE_0,
										0x25);
						}
						ReadModifyWriteTPI(0x1A, 0x06, 0x06);
						GetKSV();
						RiCnt = ReadByteTPI(0x1A);
						ReadModifyWriteTPI(0x1A, 0x08, 0x00);
					}
#endif
					break;

				default:
					TPI_DEBUG_PRINT(("Protection = Extended but not Local?\n"));
					RestartHDCP();
					break;
				}
			}
#ifdef KSVFORWARD
			/*                                                     */
			/*                              */
			/*                                   */
			/*                   */
			/*                                      */
			/*                                      */
			if ((ReadByteTPI(0x3E) & KSV_FIFO_READY_MASK) == KSV_FIFO_READY_YES) {
				ReadModifyWriteTPI(0x3E, KSV_FIFO_READY_MASK, KSV_FIFO_READY_YES);
				TPI_DEBUG_PRINT(("KSV Fwd: KSV FIFO has data...\n"));
				{
					/*                                                */
					/*                                                                    */
					/*                                                                                   */
					do {
						ksv = ReadByteTPI(0x41);
						if (ksv & KSV_FIFO_COUNT_MASK) {
							TPI_DEBUG_PRINT(("KSV Fwd: KSV FIFO Count = %d, ", (int)(ksv & KSV_FIFO_COUNT_MASK)));
							ksv = ReadByteTPI(0x42);	/*                                       */
							TPI_DEBUG_PRINT(("Value = %d\n", (int)ksv));
						}
					} while ((ksv & KSV_FIFO_LAST_MASK) == KSV_FIFO_LAST_NO);
					TPI_DEBUG_PRINT(("KSV Fwd: Last KSV FIFO forward complete\n"));
				}
			}
#endif
			ClearInterrupt(HDCP_CHANGE_EVENT);
		}
	}
}




/*                                                                               */
/*                                                                               */
/*                                              *///                               
/*                                                                                          */
/*                                              *///                               
/*                                                                               */
/*                                                                               */

/*                                                                                */
/*                  */
/*                                                                                */
struct ModeIdType {
	byte Mode_C1;
	byte Mode_C2;
	byte SubMode;
};

struct PxlLnTotalType {
	word Pixels;
	word Lines;
};
struct HVPositionType {
	word H;
	word V;
};

struct HVResolutionType {
	word H;
	word V;
};

struct TagType {
	byte RefrTypeVHPol;
	word VFreq;
	struct PxlLnTotalType Total;
};

struct _656Type {
	byte IntAdjMode;
	word HLength;
	byte VLength;
	word Top;
	word Dly;
	word HBit2HSync;
	byte VBit2VSync;
	word Field2Offset;
};

struct Vspace_Vblank {
	byte VactSpace1;
	byte VactSpace2;
	byte Vblank1;
	byte Vblank2;
	byte Vblank3;
};

/*  */
/*                                                                                        */
/*                          */
/*  */
typedef enum {
	PC_640x350_85_08 = 0,
	PC_640x400_85_08,
	PC_720x400_70_08,
	PC_720x400_85_04,
	PC_640x480_59_94,
	PC_640x480_72_80,
	PC_640x480_75_00,
	PC_640x480_85_00,
	PC_800x600_56_25,
	PC_800x600_60_317,
	PC_800x600_72_19,
	PC_800x600_75,
	PC_800x600_85_06,
	PC_1024x768_60,
	PC_1024x768_70_07,
	PC_1024x768_75_03,
	PC_1024x768_85,
	PC_1152x864_75,
	PC_1600x1200_60,
	PC_1280x768_59_95,
	PC_1280x768_59_87,
	PC_280x768_74_89,
	PC_1280x768_85,
	PC_1280x960_60,
	PC_1280x960_85,
	PC_1280x1024_60,
	PC_1280x1024_75,
	PC_1280x1024_85,
	PC_1360x768_60,
	PC_1400x105_59_95,
	PC_1400x105_59_98,
	PC_1400x105_74_87,
	PC_1400x105_84_96,
	PC_1600x1200_65,
	PC_1600x1200_70,
	PC_1600x1200_75,
	PC_1600x1200_85,
	PC_1792x1344_60,
	PC_1792x1344_74_997,
	PC_1856x1392_60,
	PC_1856x1392_75,
	PC_1920x1200_59_95,
	PC_1920x1200_59_88,
	PC_1920x1200_74_93,
	PC_1920x1200_84_93,
	PC_1920x1440_60,
	PC_1920x1440_75,
	PC_12560x1440_60,
	PC_SIZE			/*              */
} PcModeCode_t;

struct VModeInfoType {
	struct ModeIdType ModeId;
	dword PixClk;
	struct TagType Tag;
	struct HVPositionType Pos;
	struct HVResolutionType Res;
	byte AspectRatio;
	struct _656Type _656;
	byte PixRep;
	struct Vspace_Vblank VsVb;
	byte _3D_Struct;
};

#define NSM                     0	/*             */

#define	DEFAULT_VIDEO_MODE		0	/*                      */

#define ProgrVNegHNeg           0x00
#define ProgrVNegHPos		0x01
#define ProgrVPosHNeg		0x02
#define ProgrVPosHPos		0x03

#define InterlaceVNegHNeg	0x04
#define InterlaceVPosHNeg      0x05
#define InterlaceVNgeHPos	0x06
#define InterlaceVPosHPos	0x07

#define VIC_BASE		0
#define HDMI_VIC_BASE           43
#define VIC_3D_BASE		47
#define PC_BASE			64

/*              */
/*                                                   */
#define R_4					0	/*     */
#define R_4or16				1	/*             */
#define R_16					2	/*      */

/*  */
/*                                                      */
/*  */
#define VIC_FOR_480P_60Hz_4X3			2	/*                   */
#define VIC_FOR_480P_60Hz_16X9			3	/*                   */
#define VIC_FOR_720P_60Hz				4	/*                    */
#define VIC_FOR_1080i_60Hz				5	/*                     */
#define VIC_FOR_1080p_60Hz				16	/*                    */
#define VIC_FOR_720P_50Hz				19	/*                    */
#define VIC_FOR_1080i_50Hz				20	/*                     */
#define VIC_FOR_1080p_50Hz				31	/*                   */
#define VIC_FOR_1080p_24Hz				32	/*                   */


static struct VModeInfoType VModesTable[] = {
	/*                                                                                                                                                                                                                                     */
	/*                                                                                                                                                                       */
	/*                                                                                                                                                                                     */
	/*                                                                                                                                                                                                                                     */
	{{1, 0, NSM}, 2517, {ProgrVNegHNeg, 6000, {800, 525} }, {144, 35}, {640, 480}, R_4, {0, 96, 2, 33, 48, 16, 10, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                   */
	{{2, 3, NSM}, 2700, {ProgrVNegHNeg, 6000, {858, 525} }, {122, 36}, {720, 480}, R_4or16, {0, 62, 6, 30, 60, 19, 9, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                          */
	{{4, 0, NSM}, 7425, {ProgrVPosHPos, 6000, {1650, 750} }, {260, 25}, {1280, 720}, R_16, {0, 40, 5, 20, 220, 110, 5, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                               */
	{{5, 0, NSM}, 7425, {InterlaceVPosHPos, 6000, {2200, 562} }, {192, 20}, {1920, 1080}, R_16, {0, 44, 5, 15, 148, 88, 2, 1100}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                           */
	{{6, 7, NSM}, 2700, {InterlaceVNegHNeg, 6000, {1716, 264} }, {119, 18}, {720, 480}, R_4or16, {3, 62, 3, 15, 114, 17, 5, 429}, 1, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                   */
	{{8, 9, 1}, 2700, {ProgrVNegHNeg, 6000, {1716, 262} }, {119, 18}, {1440, 240}, R_4or16, {0, 124, 3, 15, 114, 38, 4, 0}, 1, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                          */
	{{8, 9, 2}, 2700, {ProgrVNegHNeg, 6000, {1716, 263} }, {119, 18}, {1440, 240}, R_4or16, {0, 124, 3, 15, 114, 38, 4, 0}, 1, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                          */
	{{10, 11, NSM}, 5400, {InterlaceVNegHNeg, 6000, {3432, 525} }, {238, 18}, {2880, 480}, R_4or16, {0, 248, 3, 15, 228, 76, 4, 1716}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                          */
	{{12, 13, 1}, 5400, {ProgrVNegHNeg, 6000, {3432, 262} }, {238, 18}, {2880, 240}, R_4or16, {0, 248, 3, 15, 228, 76, 4, 0}, 1, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                          */
	{{12, 13, 2}, 5400, {ProgrVNegHNeg, 6000, {3432, 263} }, {238, 18}, {2880, 240}, R_4or16, {0, 248, 3, 15, 228, 76, 4, 0}, 1, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                          */
	{{14, 15, NSM}, 5400, {ProgrVNegHNeg, 6000, {1716, 525} }, {244, 36}, {1440, 480}, R_4or16, {0, 124, 6, 30, 120, 32, 9, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                           */
	{{16, 0, NSM}, 14835, {ProgrVPosHPos, 6000, {2200, 1125} }, {192, 41}, {1920, 1080}, R_16, {0, 44, 5, 36, 148, 88, 4, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                            */
	{{17, 18, NSM}, 2700, {ProgrVNegHNeg, 5000, {864, 625} }, {132, 44}, {720, 576}, R_4or16, {0, 64, 5, 39, 68, 12, 5, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                           */
	{{19, 0, NSM}, 7425, {ProgrVPosHPos, 5000, {1980, 750} }, {260, 25}, {1280, 720}, R_16, {0, 40, 5, 20, 220, 440, 5, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                */
	{{20, 0, NSM}, 7425, {InterlaceVPosHPos, 5000, {2640, 1125} }, {192, 20}, {1920, 1080}, R_16, {0, 44, 5, 15, 148, 528, 2, 1320}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                            */
	{{21, 22, NSM}, 2700, {InterlaceVNegHNeg, 5000, {1728, 625} }, {132, 22}, {720, 576}, R_4, {3, 63, 3, 19, 138, 24, 2, 432}, 1, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                           */
	{{23, 24, 1}, 2700, {ProgrVNegHNeg, 5000, {1728, 312} }, {132, 22}, {1440, 288}, R_4or16, {0, 126, 3, 19, 138, 24, 2, 0}, 1, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                           */
	{{23, 24, 2}, 2700, {ProgrVNegHNeg, 5000, {1728, 313} }, {132, 22}, {1440, 288}, R_4or16, {0, 126, 3, 19, 138, 24, 2, 0}, 1, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                           */
	{{23, 24, 3}, 2700, {ProgrVNegHNeg, 5000, {1728, 314} }, {132, 22}, {1440, 288}, R_4or16, {0, 126, 3, 19, 138, 24, 2, 0}, 1, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                           */
	{{25, 26, NSM}, 5400, {InterlaceVNegHNeg, 5000, {3456, 625} }, {264, 22}, {2880, 576}, R_4or16, {0, 252, 3, 19, 276, 48, 2, 1728}, 1, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                           */
	{{27, 28, 1}, 5400, {ProgrVNegHNeg, 5000, {3456, 312} }, {264, 22}, {2880, 288}, R_4or16, {0, 252, 3, 19, 276, 48, 2, 0}, 1, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                           */
	{{27, 28, 2}, 5400, {ProgrVNegHNeg, 5000, {3456, 313} }, {264, 22}, {2880, 288}, R_4or16, {0, 252, 3, 19, 276, 48, 3, 0}, 1, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                           */
	{{27, 28, 3}, 5400, {ProgrVNegHNeg, 5000, {3456, 314} }, {264, 22}, {2880, 288}, R_4or16, {0, 252, 3, 19, 276, 48, 4, 0}, 1, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                           */
	{{29, 30, NSM}, 5400, {ProgrVPosHNeg, 5000, {1728, 625} }, {264, 44}, {1440, 576}, R_4or16, {0, 128, 5, 39, 136, 24, 5, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                           */
	{{31, 0, NSM}, 14850, {ProgrVPosHPos, 5000, {2640, 1125} }, {192, 41}, {1920, 1080}, R_16, {0, 44, 5, 36, 148, 528, 4, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                            */
	{{32, 0, NSM}, 7417, {ProgrVPosHPos, 2400, {2750, 1125} }, {192, 41}, {1920, 1080}, R_16, {0, 44, 5, 36, 148, 638, 4, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                 */
	{{33, 0, NSM}, 7425, {ProgrVPosHPos, 2500, {2640, 1125} }, {192, 41}, {1920, 1080}, R_16, {0, 44, 5, 36, 148, 528, 4, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                            */
	{{34, 0, NSM}, 7417, {ProgrVPosHPos, 3000, {2200, 1125} }, {192, 41}, {1920, 1080}, R_16, {0, 44, 5, 36, 148, 528, 4, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                            */
	{{35, 36, NSM}, 10800, {ProgrVNegHNeg, 5994, {3432, 525} }, {488, 36}, {2880, 480}, R_4or16, {0, 248, 6, 30, 240, 64, 10, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                      */
	{{37, 38, NSM}, 10800, {ProgrVNegHNeg, 5000, {3456, 625} }, {272, 39}, {2880, 576}, R_4or16, {0, 256, 5, 40, 272, 48, 5, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                */
	{{39, 0, NSM}, 7200, {InterlaceVNegHNeg, 5000, {2304, 1250} }, {352, 62}, {1920, 1080}, R_16, {0, 168, 5, 87, 184, 32, 24, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                 */
	{{40, 0, NSM}, 14850, {InterlaceVPosHPos, 10000, {2640, 1125} }, {192, 20}, {1920, 1080}, R_16, {0, 44, 5, 15, 148, 528, 2, 1320}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                  */
	{{41, 0, NSM}, 14850, {InterlaceVPosHPos, 10000, {1980, 750} }, {260, 25}, {1280, 720}, R_16, {0, 40, 5, 20, 220, 400, 5, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                 */
	{{42, 43, NSM}, 5400, {ProgrVNegHNeg, 10000, {864, 144} }, {132, 44}, {720, 576}, R_4or16, {0, 64, 5, 39, 68, 12, 5, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                 */
	{{44, 45, NSM}, 5400, {InterlaceVNegHNeg, 10000, {864, 625} }, {132, 22}, {720, 576}, R_4or16, {0, 63, 3, 19, 69, 12, 2, 432}, 1, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                           */
	{{46, 0, NSM}, 14835, {InterlaceVPosHPos, 11988, {2200, 1125} }, {192, 20}, {1920, 1080}, R_16, {0, 44, 5, 15, 149, 88, 2, 1100}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                         */
	{{47, 0, NSM}, 14835, {ProgrVPosHPos, 11988, {1650, 750} }, {260, 25}, {1280, 720}, R_16, {0, 40, 5, 20, 220, 110, 5, 1100}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                        */
	{{48, 49, NSM}, 5400, {ProgrVNegHNeg, 11988, {858, 525} }, {122, 36}, {720, 480}, R_4or16, {0, 62, 6, 30, 60, 16, 10, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                        */
	{{50, 51, NSM}, 5400, {InterlaceVNegHNeg, 11988, {858, 525} }, {119, 18}, {720, 480}, R_4or16, {0, 62, 3, 15, 57, 19, 4, 429}, 1, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                        */
	{{52, 53, NSM}, 10800, {ProgrVNegHNeg, 20000, {864, 625} }, {132, 44}, {720, 576}, R_4or16, {0, 64, 5, 39, 68, 12, 5, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                 */
	{{54, 55, NSM}, 10800, {InterlaceVNegHNeg, 20000, {864, 625} }, {132, 22}, {720, 576}, R_4or16, {0, 63, 3, 19, 69, 12, 2, 432}, 1, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                            */
	{{56, 57, NSM}, 10800, {ProgrVNegHNeg, 24000, {858, 525} }, {122, 42}, {720, 480}, R_4or16, {0, 62, 6, 30, 60, 16, 9, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                         */
	{{58, 59, NSM}, 10800, {InterlaceVNegHNeg, 24000, {858, 525} }, {119, 18}, {720, 480}, R_4or16, {0, 62, 3, 15, 57, 19, 4, 429}, 1, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                                   */

/*                                                                                                                                                                                                       */
/*                */
	/*                                                                                                                                                                                                                                     */
	/*       */
	/*                                                                                                                                                                       */
	/*                                                                                                                                                                                     */
	/*                                                                                                                                                                                                                                     */
	{{1, 0, NSM}, 297000, {ProgrVNegHNeg, 30000, {4400, 2250} }, {384, 82}, {3840, 2160}, R_16, {0, 88, 10, 72, 296, 176, 8, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                       */
	{{2, 0, NSM}, 297000, {ProgrVNegHNeg, 29700, {5280, 2250} }, {384, 82}, {3840, 2160}, R_16, {0, 88, 10, 72, 296, 1056, 8, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                   */
	{{3, 0, NSM}, 297000, {ProgrVNegHNeg, 24000, {5500, 2250} }, {384, 82}, {3840, 2160}, R_16, {0, 88, 10, 72, 296, 1276, 8, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                 */
	{{4, 0, NSM}, 297000, {ProgrVNegHNeg, 24000, {6500, 2250} }, {384, 82}, {4096, 2160}, R_16, {0, 88, 10, 72, 296, 1020, 8, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                           */

/*                                                                                                                                                                                                       */
/*           */
	/*                                                                                                                                                                                                                                     */
	/*       */
	/*                                                                                                                                                                       */
	/*                                                                                                                                                                                     */
	/*                                                                                                                                                                                                                                     */
	{{2, 3, NSM}, 2700, {ProgrVPosHPos, 6000, {858, 525} }, {122, 36}, {720, 480}, R_4or16, {0, 62, 6, 30, 60, 16, 9, 0}, 0, {0, 0, 0, 0, 0}, 8},	/*                                                     */
	{{4, 0, NSM}, 14850, {ProgrVPosHPos, 6000, {1650, 750} }, {260, 25}, {1280, 720}, R_16, {0, 40, 5, 20, 220, 110, 5, 0}, 0, {0, 0, 0, 0, 0}, 0},	/*                                               */
	{{5, 0, NSM}, 14850, {InterlaceVPosHPos, 6000, {2200, 562} }, {192, 20}, {1920, 540}, R_16, {0, 44, 5, 15, 148, 88, 2, 1100}, 0, {23, 22, 0, 0, 0}, 0},	/*                                               */
	{{5, 0, NSM}, 14850, {InterlaceVPosHPos, 6000, {2200, 562} }, {192, 20}, {1920, 540}, R_16, {0, 44, 5, 15, 148, 88, 2, 1100}, 0, {0, 0, 22, 22, 23}, 1},	/*                                                   */
	{{16, 0, NSM}, 29700, {ProgrVPosHPos, 6000, {2200, 1125} }, {192, 41}, {1920, 1080}, R_16, {0, 44, 5, 36, 148, 88, 4, 0}, 0, {0, 0, 0, 0, 0}, 0},	/*                                               */
	{{16, 0, NSM}, 29700, {ProgrVPosHPos, 6000, {2200, 1125} }, {192, 41}, {1920, 1080}, R_16, {0, 44, 5, 36, 148, 88, 4, 0}, 0, {0, 0, 0, 0, 0}, 2},	/*                                                  */
	{{16, 0, NSM}, 29700, {ProgrVPosHPos, 6000, {2200, 1125} }, {192, 41}, {1920, 1080}, R_16, {0, 44, 5, 36, 148, 88, 4, 0}, 0, {0, 0, 0, 0, 0}, 3},	/*                                                     */
	{{16, 0, NSM}, 14850, {ProgrVPosHPos, 6000, {2200, 1125} }, {192, 41}, {1920, 1080}, R_16, {0, 44, 5, 36, 148, 88, 4, 0}, 0, {0, 0, 0, 0, 0}, 8},	/*                                                     */
	{{19, 0, NSM}, 14850, {ProgrVPosHPos, 5000, {1980, 750} }, {260, 25}, {1280, 720}, R_16, {0, 40, 5, 20, 220, 440, 5, 0}, 0, {0, 0, 0, 0, 0}, 0},	/*                                               */
	{{19, 0, NSM}, 14850, {ProgrVPosHPos, 5000, {1980, 750} }, {260, 25}, {1280, 720}, R_16, {0, 40, 5, 20, 220, 440, 5, 0}, 0, {0, 0, 0, 0, 0}, 4},	/*                                             */
	{{19, 0, NSM}, 29700, {ProgrVPosHPos, 5000, {1980, 750} }, {260, 25}, {1280, 720}, R_16, {0, 40, 5, 20, 220, 440, 5, 0}, 0, {0, 0, 0, 0, 0}, 5},	/*                                                             */
	{{20, 0, NSM}, 14850, {InterlaceVPosHPos, 5000, {2640, 562} }, {192, 20}, {1920, 540}, R_16, {0, 44, 5, 15, 148, 528, 2, 1220}, 0, {23, 22, 0, 0, 0}, 0},	/*                                               */
	{{20, 0, NSM}, 14850, {InterlaceVPosHPos, 5000, {2640, 562} }, {192, 20}, {1920, 540}, R_16, {0, 44, 5, 15, 148, 528, 2, 1220}, 0, {0, 0, 22, 22, 23}, 1},	/*                                                   */
	{{31, 0, NSM}, 29700, {ProgrVPosHPos, 5000, {2640, 1125} }, {192, 41}, {1920, 1080}, R_16, {0, 44, 5, 36, 148, 528, 4, 0}, 0, {0, 0, 0, 0, 0}, 0},	/*                                               */
	{{31, 0, NSM}, 29700, {ProgrVPosHPos, 5000, {2640, 1125} }, {192, 41}, {1920, 1080}, R_16, {0, 44, 5, 36, 148, 528, 4, 0}, 0, {0, 0, 0, 0, 0}, 2},	/*                                                  */
	{{31, 0, NSM}, 29700, {ProgrVPosHPos, 5000, {2650, 1125} }, {192, 41}, {1920, 1080}, R_16, {0, 44, 5, 36, 148, 528, 4, 0}, 0, {0, 0, 0, 0, 0}, 3},	/*                                                     */
	{{32, 0, NSM}, 14850, {ProgrVPosHPos, 2400, {2750, 1125} }, {192, 41}, {1920, 1080}, R_16, {0, 44, 5, 36, 148, 638, 4, 0}, 0, {0, 0, 0, 0, 0}, 0},	/*                                               */

/*                                                                                                                                                                                                       */
/*                                                                                                                                 */

	/*                                                                                                                                                                                                                                     */
	/*            */
	/*                                                                                                                                                                                  */
	/*                                                                                                                                                                                                 */
	/*                                                                                                                                                                                                                                     */
	{{PC_BASE, 0, NSM}, 3150, {ProgrVNegHPos, 8508, {832, 445} }, {160, 63}, {640, 350}, R_16, {0, 64, 3, 60, 96, 32, 32, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                    */
	{{PC_BASE + 1, 0, NSM}, 3150, {ProgrVPosHNeg, 8508, {832, 445} }, {160, 44}, {640, 400}, R_16, {0, 64, 3, 41, 96, 32, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                    */
	{{PC_BASE + 2, 0, NSM}, 2700, {ProgrVPosHNeg, 7008, {900, 449} }, {0, 0}, {720, 400}, R_16, {0, 0, 0, 0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                    */
	{{PC_BASE + 3, 0, NSM}, 3500, {ProgrVPosHNeg, 8504, {936, 446} }, {20, 45}, {720, 400}, R_16, {0, 72, 3, 42, 108, 36, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                    */
	{{PC_BASE + 4, 0, NSM}, 2517, {ProgrVNegHNeg, 5994, {800, 525} }, {144, 35}, {640, 480}, R_4, {0, 96, 2, 33, 48, 16, 10, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                    */
	{{PC_BASE + 5, 0, NSM}, 3150, {ProgrVNegHNeg, 7281, {832, 520} }, {144, 31}, {640, 480}, R_4, {0, 40, 3, 28, 128, 128, 9, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                    */
	{{PC_BASE + 6, 0, NSM}, 3150, {ProgrVNegHNeg, 7500, {840, 500} }, {21, 19}, {640, 480}, R_4, {0, 64, 3, 28, 128, 24, 9, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                    */
	{{PC_BASE + 7, 0, NSM}, 3600, {ProgrVNegHNeg, 8500, {832, 509} }, {168, 28}, {640, 480}, R_4, {0, 56, 3, 25, 128, 24, 9, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                    */
	{{PC_BASE + 8, 0, NSM}, 3600, {ProgrVPosHPos, 5625, {1024, 625} }, {200, 24}, {800, 600}, R_4, {0, 72, 2, 22, 128, 24, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                    */
	{{PC_BASE + 9, 0, NSM}, 4000, {ProgrVPosHPos, 6032, {1056, 628} }, {216, 27}, {800, 600}, R_4, {0, 128, 4, 23, 88, 40, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                     */
	{{PC_BASE + 10, 0, NSM}, 5000, {ProgrVPosHPos, 7219, {1040, 666} }, {184, 29}, {800, 600}, R_4, {0, 120, 6, 23, 64, 56, 37, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                    */
	{{PC_BASE + 11, 0, NSM}, 4950, {ProgrVPosHPos, 7500, {1056, 625} }, {240, 24}, {800, 600}, R_4, {0, 80, 3, 21, 160, 16, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                 */
	{{PC_BASE + 12, 0, NSM}, 5625, {ProgrVPosHPos, 8506, {1048, 631} }, {216, 30}, {800, 600}, R_4, {0, 64, 3, 27, 152, 32, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                    */
	{{PC_BASE + 13, 0, NSM}, 6500, {ProgrVNegHNeg, 6000, {1344, 806} }, {296, 35}, {1024, 768}, R_4, {0, 136, 6, 29, 160, 24, 3, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                  */
	{{PC_BASE + 14, 0, NSM}, 7500, {ProgrVNegHNeg, 7007, {1328, 806} }, {280, 35}, {1024, 768}, R_4, {0, 136, 6, 19, 144, 24, 3, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                     */
	{{PC_BASE + 15, 0, NSM}, 7875, {ProgrVPosHPos, 7503, {1312, 800} }, {272, 31}, {1024, 768}, R_4, {0, 96, 3, 28, 176, 16, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                     */
	{{PC_BASE + 16, 0, NSM}, 9450, {ProgrVPosHPos, 8500, {1376, 808} }, {304, 39}, {1024, 768}, R_4, {0, 96, 3, 36, 208, 48, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                  */
	{{PC_BASE + 17, 0, NSM}, 10800, {ProgrVPosHPos, 7500, {1600, 900} }, {384, 35}, {1152, 864}, R_4, {0, 128, 3, 32, 256, 64, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                  */
	{{PC_BASE + 18, 0, NSM}, 16200, {ProgrVPosHPos, 6000, {2160, 1250} }, {496, 49}, {1600, 1200}, R_4, {0, 304, 3, 46, 304, 64, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                   */
	{{PC_BASE + 19, 0, NSM}, 6825, {ProgrVNegHPos, 6000, {1440, 790} }, {112, 19}, {1280, 768}, R_16, {0, 32, 7, 12, 80, 48, 3, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                     */
	{{PC_BASE + 20, 0, NSM}, 7950, {ProgrVPosHNeg, 5987, {1664, 798} }, {320, 27}, {1280, 768}, R_16, {0, 128, 7, 20, 192, 64, 3, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                     */
	{{PC_BASE + 21, 0, NSM}, 10220, {ProgrVPosHNeg, 6029, {1696, 805} }, {320, 27}, {1280, 768}, R_16, {0, 128, 7, 27, 208, 80, 3, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                     */
	{{PC_BASE + 22, 0, NSM}, 11750, {ProgrVPosHNeg, 8484, {1712, 809} }, {352, 38}, {1280, 768}, R_16, {0, 136, 7, 31, 216, 80, 3, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                  */
	{{PC_BASE + 23, 0, NSM}, 10800, {ProgrVPosHPos, 6000, {1800, 1000} }, {424, 39}, {1280, 960}, R_4, {0, 112, 3, 36, 312, 96, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                  */
	{{PC_BASE + 24, 0, NSM}, 14850, {ProgrVPosHPos, 8500, {1728, 1011} }, {384, 50}, {1280, 960}, R_4, {0, 160, 3, 47, 224, 64, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                  */
	{{PC_BASE + 25, 0, NSM}, 10800, {ProgrVPosHPos, 6002, {1688, 1066} }, {360, 41}, {1280, 1024}, R_4, {0, 112, 3, 38, 248, 48, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                   */
	{{PC_BASE + 26, 0, NSM}, 13500, {ProgrVPosHPos, 7502, {1688, 1066} }, {392, 41}, {1280, 1024}, R_4, {0, 144, 3, 38, 248, 16, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                   */
	{{PC_BASE + 27, 0, NSM}, 15750, {ProgrVPosHPos, 8502, {1728, 1072} }, {384, 47}, {1280, 1024}, R_4, {0, 160, 3, 4, 224, 64, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                   */
	{{PC_BASE + 28, 0, NSM}, 8550, {ProgrVPosHPos, 6002, {1792, 795} }, {368, 24}, {1360, 768}, R_16, {0, 112, 6, 18, 256, 64, 3, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                  */
	{{PC_BASE + 29, 0, NSM}, 10100, {ProgrVNegHPos, 5995, {1560, 1080} }, {112, 27}, {1400, 1050}, R_4, {0, 32, 4, 23, 80, 48, 3, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                     */
	{{PC_BASE + 30, 0, NSM}, 12175, {ProgrVPosHNeg, 5998, {1864, 1089} }, {376, 36}, {1400, 1050}, R_4, {0, 144, 4, 32, 232, 88, 3, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                     */
	{{PC_BASE + 31, 0, NSM}, 15600, {ProgrVPosHNeg, 7487, {1896, 1099} }, {392, 46}, {1400, 1050}, R_4, {0, 144, 4, 22, 248, 104, 3, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                     */
	{{PC_BASE + 32, 0, NSM}, 17950, {ProgrVPosHNeg, 8496, {1912, 1105} }, {408, 52}, {1400, 1050}, R_4, {0, 152, 4, 48, 256, 104, 3, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                     */
	{{PC_BASE + 33, 0, NSM}, 17550, {ProgrVPosHPos, 6500, {2160, 1250} }, {496, 49}, {1600, 1200}, R_4, {0, 192, 3, 46, 304, 64, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                   */
	{{PC_BASE + 34, 0, NSM}, 18900, {ProgrVPosHPos, 7000, {2160, 1250} }, {496, 49}, {1600, 1200}, R_4, {0, 192, 3, 46, 304, 64, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                   */
	{{PC_BASE + 35, 0, NSM}, 20250, {ProgrVPosHPos, 7500, {2160, 1250} }, {496, 49}, {1600, 1200}, R_4, {0, 192, 3, 46, 304, 64, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                   */
	{{PC_BASE + 36, 0, NSM}, 22950, {ProgrVPosHPos, 8500, {2160, 1250} }, {496, 49}, {1600, 1200}, R_4, {0, 192, 3, 46, 304, 64, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                    */
	{{PC_BASE + 37, 0, NSM}, 20475, {ProgrVPosHNeg, 6000, {2448, 1394} }, {528, 49}, {1792, 1344}, R_4, {0, 200, 3, 46, 328, 128, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                    */
	{{PC_BASE + 38, 0, NSM}, 26100, {ProgrVPosHNeg, 7500, {2456, 1417} }, {568, 72}, {1792, 1344}, R_4, {0, 216, 3, 69, 352, 96, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                        */
	{{PC_BASE + 39, 0, NSM}, 21825, {ProgrVPosHNeg, 6000, {2528, 1439} }, {576, 46}, {1856, 1392}, R_4, {0, 224, 3, 43, 352, 96, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                    */
	{{PC_BASE + 40, 0, NSM}, 28800, {ProgrVPosHNeg, 7500, {2560, 1500} }, {576, 107}, {1856, 1392}, R_4, {0, 224, 3, 104, 352, 128, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                    */
	{{PC_BASE + 41, 0, NSM}, 15400, {ProgrVNegHPos, 5995, {2080, 1235} }, {112, 32}, {1920, 1200}, R_16, {0, 32, 6, 26, 80, 48, 3, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                       */
	{{PC_BASE + 42, 0, NSM}, 19325, {ProgrVPosHNeg, 5988, {2592, 1245} }, {536, 42}, {1920, 1200}, R_16, {0, 200, 6, 36, 336, 136, 3, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                       */
	{{PC_BASE + 43, 0, NSM}, 24525, {ProgrVPosHNeg, 7493, {2608, 1255} }, {552, 52}, {1920, 1200}, R_16, {0, 208, 6, 46, 344, 136, 3, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                       */
	{{PC_BASE + 44, 0, NSM}, 28125, {ProgrVPosHNeg, 8493, {2624, 1262} }, {560, 59}, {1920, 1200}, R_16, {0, 208, 6, 53, 352, 144, 3, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                       */
	{{PC_BASE + 45, 0, NSM}, 23400, {ProgrVPosHNeg, 6000, {2600, 1500} }, {552, 59}, {1920, 1440}, R_4, {0, 208, 3, 56, 344, 128, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                    */
	{{PC_BASE + 46, 0, NSM}, 29700, {ProgrVPosHNeg, 7500, {2640, 1500} }, {576, 59}, {1920, 1440}, R_4, {0, 224, 3, 56, 352, 144, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                    */
	{{PC_BASE + 47, 0, NSM}, 24150, {ProgrVPosHNeg, 6000, {2720, 1481} }, {48, 3}, {2560, 1440}, R_16, {0, 32, 5, 56, 352, 144, 1, 0}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                                     */
	{{PC_BASE + 48, 0, NSM}, 2700, {InterlaceVNegHNeg, 6000, {1716, 264} }, {244, 18}, {1440, 480}, R_4or16, {3, 124, 3, 15, 114, 17, 5, 429}, 0, {0, 0, 0, 0, 0}, NO_3D_SUPPORT},	/*                   */
};


/*                                                                                */
/*                                                                            */
/*                                                                                  */
/*                                                                                */
/*                                   */
/*                                                                                */
static u8 AspectRatioTable[] = {
	R_4, R_4, R_16, R_16, R_16, R_4, R_16, R_4, R_16, R_4,
	R_16, R_4, R_16, R_4, R_16, R_16, R_4, R_16, R_16, R_16,
	R_4, R_16, R_4, R_16, R_4, R_16, R_4, R_16, R_4, R_16,
	R_16, R_16, R_16, R_16, R_4, R_16, R_4, R_16, R_16, R_16,
	R_16, R_4, R_16, R_4, R_16, R_16, R_16, R_4, R_16, R_4,
	R_16, R_4, R_16, R_4, R_16, R_4, R_16, R_4, R_16
};

/*                                                                                */
/*                                                                                          */
/*                                                                                       */
/*              */
/*                                                                                */
static u8 VIC2Index[] = {
	0, 0, 1, 1, 2, 3, 4, 4, 5, 5,
	7, 7, 8, 8, 10, 10, 11, 12, 12, 13,
	14, 15, 15, 16, 16, 19, 19, 20, 20, 23,
	23, 24, 25, 26, 27, 28, 28, 29, 29, 30,
	31, 32, 33, 33, 34, 34, 35, 36, 37, 37,
	38, 38, 39, 39, 40, 40, 41, 41, 42, 42
};

/*                                                                                */
/*                                         */
/*                                                                              */
/*                                                        */
/*                                                                     */
/*                                                  */
/*                                                       */
/*                                 */
/*                                                       */
/*                                                                                */
byte ConvertVIC_To_VM_Index(void)
{
	byte index;

	/*  */
	/*                                                                            */
	/*                                                                    */
	/*  */
	/*                                                                        */
	/*                                                                           */
	/*                                           */
	/*  */
	if (siHdmiTx.HDMIVideoFormat == VMD_HDMIFORMAT_CEA_VIC) {
		/*  */
		/*                                                                */
		/*                             */
		/*  */
		index = VIC2Index[siHdmiTx.VIC];
	} else if (siHdmiTx.HDMIVideoFormat == VMD_HDMIFORMAT_HDMI_VIC) {
		/*  */
		/*                                                                                 */
		/*                                                                            */
		/*  */
		if ((siHdmiTx.VIC < 1) || (siHdmiTx.VIC > 4)) {
			index = DEFAULT_VIDEO_MODE;
		} else {
			index = (HDMI_VIC_BASE - 1) + siHdmiTx.VIC;
		}
	} else if (siHdmiTx.HDMIVideoFormat == VMD_HDMIFORMAT_3D) {
		/*  */
		/*                                                                                        */
		/*                                                                                       */
		/*                                                                          */
		/*  */
		switch (siHdmiTx.VIC) {
		case VIC_FOR_480P_60Hz_4X3:
		case VIC_FOR_480P_60Hz_16X9:
			/*                                                     */
			if (siHdmiTx.ThreeDStructure == SIDE_BY_SIDE_HALF)
				index = VIC_3D_BASE + 0;
			else
				index = DEFAULT_VIDEO_MODE;
			break;

		case VIC_FOR_720P_60Hz:
			switch (siHdmiTx.ThreeDStructure) {
			case FRAME_PACKING:
				index = VIC_3D_BASE + 1;
				break;
			default:
				index = DEFAULT_VIDEO_MODE;
				break;
			}
			break;

		case VIC_FOR_1080i_60Hz:
			switch (siHdmiTx.ThreeDStructure) {
			case FRAME_PACKING:
				index = VIC_3D_BASE + 2;
				break;
			case VMD_3D_FIELDALTERNATIVE:
				index = VIC_3D_BASE + 3;
				break;
			default:
				index = DEFAULT_VIDEO_MODE;
				break;
			}
			break;

		case VIC_FOR_1080p_60Hz:
			switch (siHdmiTx.ThreeDStructure) {
			case FRAME_PACKING:
				index = VIC_3D_BASE + 4;
				break;
			case VMD_3D_LINEALTERNATIVE:
				index = VIC_3D_BASE + 5;
				break;
			case SIDE_BY_SIDE_FULL:
				index = VIC_3D_BASE + 6;
				break;
			case SIDE_BY_SIDE_HALF:
				index = VIC_3D_BASE + 7;
				break;
			default:
				index = DEFAULT_VIDEO_MODE;
				break;
			}
			break;

		case VIC_FOR_720P_50Hz:
			switch (siHdmiTx.ThreeDStructure) {
			case FRAME_PACKING:
				index = VIC_3D_BASE + 8;
				break;
			case VMD_3D_LDEPTH:
				index = VIC_3D_BASE + 9;
				break;
			case VMD_3D_LDEPTHGRAPHICS:
				index = VIC_3D_BASE + 10;
				break;
			default:
				index = DEFAULT_VIDEO_MODE;
				break;
			}
			break;

		case VIC_FOR_1080i_50Hz:
			switch (siHdmiTx.ThreeDStructure) {
			case FRAME_PACKING:
				index = VIC_3D_BASE + 11;
				break;
			case VMD_3D_FIELDALTERNATIVE:
				index = VIC_3D_BASE + 12;
				break;
			default:
				index = DEFAULT_VIDEO_MODE;
				break;
			}
			break;

		case VIC_FOR_1080p_50Hz:
			switch (siHdmiTx.ThreeDStructure) {
			case FRAME_PACKING:
				index = VIC_3D_BASE + 13;
				break;
			case VMD_3D_LINEALTERNATIVE:
				index = VIC_3D_BASE + 14;
				break;
			case SIDE_BY_SIDE_FULL:
				index = VIC_3D_BASE + 15;
				break;
			default:
				index = DEFAULT_VIDEO_MODE;
				break;
			}
			break;

		case VIC_FOR_1080p_24Hz:
			switch (siHdmiTx.ThreeDStructure) {
			case FRAME_PACKING:
				index = VIC_3D_BASE + 16;
				break;
			default:
				index = DEFAULT_VIDEO_MODE;
				break;
			}
			break;

		default:
			index = DEFAULT_VIDEO_MODE;
			break;
		}
	} else if (siHdmiTx.HDMIVideoFormat == VMD_HDMIFORMAT_PC) {
		if (siHdmiTx.VIC < PC_SIZE) {
			index = siHdmiTx.VIC + PC_BASE;
		} else {
			index = DEFAULT_VIDEO_MODE;
		}
	} else {
		/*                                                                */
		index = DEFAULT_VIDEO_MODE;
	}

	return index;
}


/*         */
/*          */
byte TPI_REG0x63_SAVED = 0;

/*                                                                                */
/*                                  */
/*                                                                          */
/*  */
/*                                     */
/*               */
/*                        */
/*                                                                                */
byte SetEmbeddedSync(void)
{
	byte ModeTblIndex;
	word H_Bit_2_H_Sync;
	word Field2Offset;
	word H_SyncWidth;

	byte V_Bit_2_V_Sync;
	byte V_SyncWidth;
	byte B_Data[8];

	TPI_TRACE_PRINT((">>SetEmbeddedSync()\n"));

	ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x0A, 0x01, 0x01);	/*                               */
	ReadClearWriteTPI(0x60, MSBIT);	/*                                      */
	WriteByteTPI(0x63, 0x30);	/*                                                                    */
	ReadSetWriteTPI(0x60, MSBIT);	/*                                   */

	ModeTblIndex = ConvertVIC_To_VM_Index();

	H_Bit_2_H_Sync = VModesTable[ModeTblIndex]._656.HBit2HSync;
	Field2Offset = VModesTable[ModeTblIndex]._656.Field2Offset;
	H_SyncWidth = VModesTable[ModeTblIndex]._656.HLength;
	V_Bit_2_V_Sync = VModesTable[ModeTblIndex]._656.VBit2VSync;
	V_SyncWidth = VModesTable[ModeTblIndex]._656.VLength;

	B_Data[0] = H_Bit_2_H_Sync & LOW_BYTE;	/*                                     */

	B_Data[1] = (H_Bit_2_H_Sync >> 8) & TWO_LSBITS;	/*                        */
	/*                                                                                           */
	TPI_REG0x63_SAVED = B_Data[1];

	B_Data[2] = Field2Offset & LOW_BYTE;	/*                                     */
	B_Data[3] = (Field2Offset >> 8) & LOW_NIBBLE;	/*                                     */

	B_Data[4] = H_SyncWidth & LOW_BYTE;
	B_Data[5] = (H_SyncWidth >> 8) & TWO_LSBITS;	/*                      */
	B_Data[6] = V_Bit_2_V_Sync;	/*                       */
	B_Data[7] = V_SyncWidth;	/*                */

	WriteBlockTPI(0x62, 8, &B_Data[0]);

	return TRUE;
}

/*                                                                                */
/*                                     */
/*                                          */
/*  */
/*               */
/*               */
/*               */
/*                                                                                */
void EnableEmbeddedSync(void)
{
	TPI_TRACE_PRINT((">>EnableEmbeddedSync()\n"));

	ReadClearWriteTPI(0x60, MSBIT);	/*                             */
	WriteByteTPI(0x63, 0x30);
	ReadSetWriteTPI(0x60, MSBIT);	/*                                   */
	ReadSetWriteTPI(0x63, BIT_6);
}

/*                                                                                */
/*                        */
/*                                                                       */
/*  */
/*               */
/*                    */
/*               */
/*  */
/*                                                                           */
/*                                                                                */
byte SetDE(void)
{
	byte RegValue;
	byte ModeTblIndex;

	word H_StartPos, V_StartPos;
	word Htotal, Vtotal;
	word H_Res, V_Res;

	byte Polarity;
	byte B_Data[12];

	TPI_TRACE_PRINT((">>SetDE()\n"));

	ModeTblIndex = ConvertVIC_To_VM_Index();

	if (VModesTable[ModeTblIndex]._3D_Struct != NO_3D_SUPPORT) {
		return DE_CANNOT_BE_SET_WITH_3D_MODE;
		TPI_TRACE_PRINT((">>SetDE() not allowed with 3D video format\n"));
	}
	/*                                                                               */
	RegValue = ReadByteTPI(0x60);

	if (RegValue & BIT_7) {
		return DE_CANNOT_BE_SET_WITH_EMBEDDED_SYNC;
	}

	H_StartPos = VModesTable[ModeTblIndex].Pos.H;
	V_StartPos = VModesTable[ModeTblIndex].Pos.V;

	Htotal = VModesTable[ModeTblIndex].Tag.Total.Pixels;
	Vtotal = VModesTable[ModeTblIndex].Tag.Total.Lines;

	Polarity = (~VModesTable[ModeTblIndex].Tag.RefrTypeVHPol) & TWO_LSBITS;

	H_Res = VModesTable[ModeTblIndex].Res.H;

	if ((VModesTable[ModeTblIndex].Tag.RefrTypeVHPol & 0x04)) {
		V_Res = (VModesTable[ModeTblIndex].Res.V) >> 1;	/*                                        */
	} else {
		V_Res = (VModesTable[ModeTblIndex].Res.V);
	}

	B_Data[0] = H_StartPos & LOW_BYTE;	/*                         */

	B_Data[1] = (H_StartPos >> 8) & TWO_LSBITS;	/*                            */
	B_Data[1] |= (Polarity << 4);	/*                  */
	B_Data[1] |= BIT_EN_DE_GEN;	/*                     */

	B_Data[2] = V_StartPos & SEVEN_LSBITS;	/*                */
	B_Data[3] = 0x00;	/*                  */
	B_Data[4] = H_Res & LOW_BYTE;	/*                            */
	B_Data[5] = (H_Res >> 8) & LOW_NIBBLE;	/*                            */
	B_Data[6] = V_Res & LOW_BYTE;	/*                            */
	B_Data[7] = (V_Res >> 8) & THREE_LSBITS;	/*                            */
	B_Data[8] = Htotal & LOW_BYTE;	/*                           */
	B_Data[9] = (Htotal >> 8) & LOW_NIBBLE;	/*                           */
	B_Data[10] = Vtotal & LOW_BYTE;	/*                           */
	B_Data[11] = (Vtotal >> 8) & BITS_2_1_0;	/*                           */

	WriteBlockTPI(0x62, 12, &B_Data[0]);
	TPI_REG0x63_SAVED = B_Data[1];

	return DE_SET_OK;	/*                              */
}

/*                                                                                */
/*                            */
/*                                             */
/*  */
/*               */
/*                    */
/*               */
/*                                                                                */
void SetFormat(byte *Data)
{
	ReadModifyWriteTPI(0x1A, OUTPUT_MODE_MASK, OUTPUT_MODE_HDMI);	/*                                               */

	WriteBlockTPI(0x09, 2, Data);	/*                                         */
	WriteByteTPI(0x19, 0x00);	/*                                                                          */

	if (!IsHDMI_Sink()) {
		ReadModifyWriteTPI(0x1A, OUTPUT_MODE_MASK, OUTPUT_MODE_DVI);
	}

	if (siHdmiTx.SyncMode == EMBEDDED_SYNC)
		EnableEmbeddedSync();	/*                                                                */
}

/*                                                                                */
/*                                 */
/*                                        */
/*  */
/*                       */
/*               */
/*               */
/*                                                                                */
void printVideoMode(void)
{
	TPI_TRACE_PRINT((">>Video mode = "));

	switch (siHdmiTx.VIC) {
	case 6:
		TPI_TRACE_PRINT(("HDMI_480I60_4X3\n"));
		break;
	case 21:
		TPI_TRACE_PRINT(("HDMI_576I50_4X3\n"));
		break;
	case 2:
		TPI_TRACE_PRINT(("HDMI_480P60_4X3\n"));
		break;
	case 17:
		TPI_TRACE_PRINT(("HDMI_576P50_4X3\n"));
		break;
	case 4:
		TPI_TRACE_PRINT(("HDMI_720P60\n"));
		break;
	case 19:
		TPI_TRACE_PRINT(("HDMI_720P50\n"));
		break;
	case 5:
		TPI_TRACE_PRINT(("HDMI_1080I60\n"));
		break;
	case 20:
		TPI_TRACE_PRINT(("HDMI_1080I50\n"));
		break;
	case 16:
		TPI_TRACE_PRINT(("HDMI_1080P60\n"));
		break;
	case 31:
		TPI_TRACE_PRINT(("HDMI_1080P50\n"));
		break;
	case PC_BASE + 13:
		TPI_TRACE_PRINT(("HDMI_1024_768_60\n"));
		break;
	case PC_BASE + 9:
		TPI_TRACE_PRINT(("HDMI_800_600_60\n"));
		break;
	default:
		break;
	}
}

/*                                                                                */
/*                            */
/*                                                                                     */
/*  */
/*                                                                      */
/*                                                      */
/*                   */
/*               */
/*                                         */
/*                                                                                */
byte InitVideo(byte TclkSel)
{
	byte ModeTblIndex;

#ifdef DEEP_COLOR
	byte temp;
#endif
	byte B_Data[8];

	byte EMB_Status;	/*                       */
	byte DE_Status;
	byte Pattern;

	TPI_TRACE_PRINT((">>InitVideo()\n"));
	printVideoMode();
	TPI_TRACE_PRINT((" HF:%d", (int)siHdmiTx.HDMIVideoFormat));
	TPI_TRACE_PRINT((" VIC:%d", (int)siHdmiTx.VIC));
	TPI_TRACE_PRINT((" A:%x", (int)siHdmiTx.AspectRatio));
	TPI_TRACE_PRINT((" CS:%x", (int)siHdmiTx.ColorSpace));
	TPI_TRACE_PRINT((" CD:%x", (int)siHdmiTx.ColorDepth));
	TPI_TRACE_PRINT((" CR:%x", (int)siHdmiTx.Colorimetry));
	TPI_TRACE_PRINT((" SM:%x", (int)siHdmiTx.SyncMode));
	TPI_TRACE_PRINT((" TCLK:%x", (int)siHdmiTx.TclkSel));
	TPI_TRACE_PRINT((" 3D:%d", (int)siHdmiTx.ThreeDStructure));
	TPI_TRACE_PRINT((" 3Dx:%d\n", (int)siHdmiTx.ThreeDExtData));

	ModeTblIndex = (byte) ConvertVIC_To_VM_Index();

	Pattern = (TclkSel << 6) & TWO_MSBITS;	/*                                                        */
	ReadSetWriteTPI(0x08, Pattern);	/*                                                                                                */

	/*                                 */
	if ((siHdmiTx.VIC == 6) || (siHdmiTx.VIC == 7) ||	/*      */
	    (siHdmiTx.VIC == 21) || (siHdmiTx.VIC == 22))	/*      */
	{
		if (siHdmiTx.ColorSpace == YCBCR422_8BITS)	/*                   */
		{
			B_Data[0] = VModesTable[ModeTblIndex].PixClk & 0x00FF;
			B_Data[1] = (VModesTable[ModeTblIndex].PixClk >> 8) & 0xFF;
		} else		/*                     */
		{
			B_Data[0] = (VModesTable[ModeTblIndex].PixClk / 2) & 0x00FF;
			B_Data[1] = ((VModesTable[ModeTblIndex].PixClk / 2) >> 8) & 0xFF;
		}

	} else {
		B_Data[0] = VModesTable[ModeTblIndex].PixClk & 0x00FF;	/*                                               */
		B_Data[1] = (VModesTable[ModeTblIndex].PixClk >> 8) & 0xFF;
	}

	B_Data[2] = VModesTable[ModeTblIndex].Tag.VFreq & 0x00FF;	/*                                                      */
	B_Data[3] = (VModesTable[ModeTblIndex].Tag.VFreq >> 8) & 0xFF;

	if ((siHdmiTx.VIC == 6) || (siHdmiTx.VIC == 7) ||	/*      */
	    (siHdmiTx.VIC == 21) || (siHdmiTx.VIC == 22))	/*      */
	{
		B_Data[4] = (VModesTable[ModeTblIndex].Tag.Total.Pixels / 2) & 0x00FF;	/*                                                          */
		B_Data[5] = ((VModesTable[ModeTblIndex].Tag.Total.Pixels / 2) >> 8) & 0xFF;
	} else {
		B_Data[4] = VModesTable[ModeTblIndex].Tag.Total.Pixels & 0x00FF;	/*                                                          */
		B_Data[5] = (VModesTable[ModeTblIndex].Tag.Total.Pixels >> 8) & 0xFF;
	}

	B_Data[6] = VModesTable[ModeTblIndex].Tag.Total.Lines & 0x00FF;	/*                                                         */
	B_Data[7] = (VModesTable[ModeTblIndex].Tag.Total.Lines >> 8) & 0xFF;

	WriteBlockTPI(0x00, 8, B_Data);	/*                                                                             */

	/*                                         */
	/*                      */
	B_Data[0] = 0;		/*                                */
	/*                                                                                                              */
	B_Data[0] |= BIT_BUS_12;	/*                                                                                                                                                                                                                                                                  */
	/*                                                                        */
	B_Data[0] |= (TclkSel << 6) & TWO_MSBITS;

#ifdef CLOCK_EDGE_FALLING
	B_Data[0] &= ~BIT_EDGE_RISE;	/*                     */
#endif
#ifdef CLOCK_EDGE_RISING
	B_Data[0] |= BIT_EDGE_RISE;	/*                    */
#endif
	tpivmode[0] = B_Data[0];	/*                          */
	WriteByteTPI(0x08, B_Data[0]);	/*      */

	/*                                      */
	/*                      */
	/*                      */
	B_Data[0] = 0;		/*                                */
	B_Data[1] = 0;		/*                                */

	if (siHdmiTx.SyncMode == EMBEDDED_SYNC) {
		EMB_Status = SetEmbeddedSync();
		EnableEmbeddedSync();	/*                      */
	}

	if (siHdmiTx.SyncMode == INTERNAL_DE) {
		ReadClearWriteTPI(0x60, MSBIT);	/*                                   */
		DE_Status = SetDE();	/*                                             */
	}

	if (siHdmiTx.ColorSpace == RGB)
		B_Data[0] = (((BITS_IN_RGB | BITS_IN_AUTO_RANGE) & ~BIT_EN_DITHER_10_8) & ~BIT_EXTENDED_MODE);	/*         */

	else if (siHdmiTx.ColorSpace == YCBCR444)
		B_Data[0] = (((BITS_IN_YCBCR444 | BITS_IN_AUTO_RANGE) & ~BIT_EN_DITHER_10_8) & ~BIT_EXTENDED_MODE);	/*      */

	else if ((siHdmiTx.ColorSpace == YCBCR422_16BITS)
		 || (siHdmiTx.ColorSpace == YCBCR422_8BITS))
		B_Data[0] = (((BITS_IN_YCBCR422 | BITS_IN_AUTO_RANGE) & ~BIT_EN_DITHER_10_8) & ~BIT_EXTENDED_MODE);	/*      */

#ifdef DEEP_COLOR
	switch (siHdmiTx.ColorDepth) {
	case 0:
		temp = 0x00;
		ReadModifyWriteTPI(0x40, BIT_2, 0x00);
		break;
	case 1:
		temp = 0x80;
		ReadModifyWriteTPI(0x40, BIT_2, BIT_2);
		break;
	case 2:
		temp = 0xC0;
		ReadModifyWriteTPI(0x40, BIT_2, BIT_2);
		break;
	case 3:
		temp = 0x40;
		ReadModifyWriteTPI(0x40, BIT_2, BIT_2);
		break;
	default:
		temp = 0x00;
		ReadModifyWriteTPI(0x40, BIT_2, 0x00);
		break;
		/*                                                                                                                  */
		/*                                                                                                           */
		/*                                                              */
	}
	B_Data[0] = ((B_Data[0] & 0x3F) | temp);	/*         */
#endif

	B_Data[1] = (BITS_OUT_RGB | BITS_OUT_AUTO_RANGE);	/*         */

	if ((siHdmiTx.VIC == 6) || (siHdmiTx.VIC == 7) ||	/*      */
	    (siHdmiTx.VIC == 21) || (siHdmiTx.VIC == 22) ||	/*      */
	    (siHdmiTx.VIC == 2) || (siHdmiTx.VIC == 3) ||	/*      */
	    (siHdmiTx.VIC == 17) || (siHdmiTx.VIC == 18))	/*      */
	{
		B_Data[1] &= ~BIT_BT_709;
	} else {
		B_Data[1] |= BIT_BT_709;
	}

#ifdef DEEP_COLOR
	B_Data[1] = ((B_Data[1] & 0x3F) | temp);
#endif

#ifdef DEV_SUPPORT_EDID
	if (!IsHDMI_Sink()) {
		B_Data[1] = ((B_Data[1] & 0xFC) | BITS_OUT_RGB);
	} else {
		/*                                         */
		if (g_edid.YCbCr_4_4_4) {
			B_Data[1] = ((B_Data[1] & 0xFC) | BITS_OUT_YCBCR444);
		} else {
			if (g_edid.YCbCr_4_2_2) {
				B_Data[1] = ((B_Data[1] & 0xFC) | BITS_OUT_YCBCR422);
			} else {
				B_Data[1] = ((B_Data[1] & 0xFC) | BITS_OUT_RGB);
			}
		}
	}
#endif

	tpivmode[1] = B_Data[0];	/*                          */
	tpivmode[2] = B_Data[1];	/*                          */
	SetFormat(B_Data);

	ReadClearWriteTPI(0x60, BIT_2);	/*                                                                                            */

	return TRUE;
}

/*                                                                                */
/*                                    */
/*                                                                               */
/*  */
/*                                                                                */
/*               */
/*               */
/*  */
/*                                                             */
/*  */
/*                                                                                */
byte SetAVI_InfoFrames(void)
{
	byte B_Data[SIZE_AVI_INFOFRAME];
	byte i;
	byte TmpVal;
	byte VModeTblIndex;

	TPI_TRACE_PRINT((">>SetAVI_InfoFrames()\n"));

	for (i = 0; i < SIZE_AVI_INFOFRAME; i++)
		B_Data[i] = 0;

#ifdef DEV_SUPPORT_EDID
	if (g_edid.YCbCr_4_4_4)
		TmpVal = 2;
	else if (g_edid.YCbCr_4_2_2)
		TmpVal = 1;
	else
		TmpVal = 0;
#else
	TmpVal = 0;
#endif

	B_Data[1] = (TmpVal << 5) & BITS_OUT_FORMAT;	/*                                 */
	B_Data[1] |= 0x12;	/*                                                                                                                     */
	/*                          */

	if (siHdmiTx.ColorSpace == XVYCC444)	/*                              */
	{
		B_Data[2] = 0xC0;	/*                                                                 */

		if (siHdmiTx.Colorimetry == COLORIMETRY_601)	/*          */
			B_Data[3] &= ~BITS_6_5_4;

		else if (siHdmiTx.Colorimetry == COLORIMETRY_709)	/*          */
			B_Data[3] = (B_Data[3] & ~BITS_6_5_4) | BIT_4;
	}

	else if (siHdmiTx.Colorimetry == COLORIMETRY_709)	/*        */
		B_Data[2] = 0x80;	/*                 */

	else if (siHdmiTx.Colorimetry == COLORIMETRY_601)	/*        */
		B_Data[2] = 0x40;	/*                 */

	else			/*                 */
	{			/*                 */
		B_Data[2] &= ~BITS_7_6;	/*                 */
		B_Data[3] &= ~BITS_6_5_4;	/*                          */
	}

	VModeTblIndex = ConvertVIC_To_VM_Index();

	B_Data[4] = siHdmiTx.VIC;

	/*                                                      */
	if (siHdmiTx.AspectRatio == VMD_ASPECT_RATIO_16x9) {
		B_Data[2] |= _16_To_9;	/*                 */
		/*                                                                                       */
		/*                                                                                             */
		if ((VModesTable[VModeTblIndex].AspectRatio == R_4or16)
		    && (AspectRatioTable[siHdmiTx.VIC - 1] == R_4)) {
			siHdmiTx.VIC++;
			B_Data[4]++;
		}
	} else {
		B_Data[2] |= _4_To_3;	/*                */
	}

	B_Data[2] |= SAME_AS_AR;	/*                                                           */
	B_Data[5] = VModesTable[VModeTblIndex].PixRep;	/*                                         */

	/*                                  */
	B_Data[0] = 0x82 + 0x02 + 0x0D;
	for (i = 1; i < SIZE_AVI_INFOFRAME; i++) {
		B_Data[0] += B_Data[i];
	}
	B_Data[0] = 0x100 - B_Data[0];

	/*                                                          */
	WriteBlockTPI(0x0C, SIZE_AVI_INFOFRAME, B_Data);

	if (siHdmiTx.SyncMode == EMBEDDED_SYNC)
		EnableEmbeddedSync();

	return TRUE;
}

extern void siHdmiTx_PowerStateD0(void);
extern void SetAudioMute(byte audioMute);
/*                                                                                */
/*                                */
/*                                                       */
/*  */
/*               */
/*               */
/*                   */
/*                                                                                */
void siHdmiTx_Init(void)
{
	TPI_TRACE_PRINT((">>siHdmiTx_Init()\n"));

	/*                          */
	if (siHdmiTx.ColorDepth == VMD_COLOR_DEPTH_8BIT) {
		/*                                   */
		siHdmiTx.ColorDepth = VMD_COLOR_DEPTH_16BIT;
		InitVideo(siHdmiTx.TclkSel);
		/*                                                                 */
		siHdmiTx.ColorDepth = VMD_COLOR_DEPTH_8BIT;
	}
	/*                */

	InitVideo(siHdmiTx.TclkSel);	/*                                        */

	siHdmiTx_PowerStateD0();

	if (IsHDMI_Sink())	/*                                         */
	{
		SetAVI_InfoFrames();
		siHdmiTx_AudioSet();	/*                                                                                            */
	} else {
		SetAudioMute(AUDIO_MUTE_MUTED);
	}

/*                                                                                                     */
/*                                                                                                  */

/*                                                                                */
	if (siHdmiTx.ColorSpace == YCBCR422_8BITS)
		ReadSetWriteTPI(0x60, BIT_5);	/*                                             */

/*                                                                    */
	WriteByteTPI(0x63, TPI_REG0x63_SAVED);

/*                                                                                                  */
/*                                                                                                     */

	/*                                                            */
	WriteByteTPI(0x0B, 0x00);


	if ((g_hdcp.HDCP_TxSupports == TRUE) && (g_hdcp.HDCPAuthenticated == VMD_HDCP_AUTHENTICATED)
	    && (Sii9024A_HDCP_supported)) {
		if (g_hdcp.HDCP_AksvValid == TRUE) {
			/*         */
			TPI_DEBUG_PRINT(("TMDS -> Enabled (Video Muted)\n"));
			ReadModifyWriteTPI(0x1A,
					   LINK_INTEGRITY_MODE_MASK | TMDS_OUTPUT_CONTROL_MASK |
					   AV_MUTE_MASK,
					   LINK_INTEGRITY_DYNAMIC | TMDS_OUTPUT_CONTROL_ACTIVE |
					   AV_MUTE_MUTED);

			WriteByteTPI(0x08, tpivmode[0]);	/*                     */
			g_sys.tmdsPoweredUp = TRUE;
			EnableInterrupts(HOT_PLUG_EVENT | RX_SENSE_EVENT | AUDIO_ERROR_EVENT |
					 SECURITY_CHANGE_EVENT | HDCP_CHANGE_EVENT);
		}
	} else {
		TPI_DEBUG_PRINT(("TMDS -> Enabled\n"));
		ReadModifyWriteTPI(0x1A,
				   LINK_INTEGRITY_MODE_MASK | TMDS_OUTPUT_CONTROL_MASK |
				   AV_MUTE_MASK,
				   LINK_INTEGRITY_DYNAMIC | TMDS_OUTPUT_CONTROL_ACTIVE |
				   AV_MUTE_MUTED);
		ReadModifyWriteTPI(0x1A, AV_MUTE_MASK, AV_MUTE_NORMAL);
		WriteByteTPI(0x08, tpivmode[0]);	/*                     */
		g_sys.tmdsPoweredUp = TRUE;
		EnableInterrupts(HOT_PLUG_EVENT | RX_SENSE_EVENT | AUDIO_ERROR_EVENT);
	}
}

/*                                                                                */
/*                                    */
/*                                                       */
/*  */
/*               */
/*                                                                    */
/*                                        */
/*                   */
/*                                                                                */
/*                                                              */
#define T_RES_CHANGE_DELAY      128	/*                                                                   */

byte siHdmiTx_VideoSet(void)
{
	TPI_TRACE_PRINT((">>siHdmiTx_VideoSet()\n"));

	ReadModifyWriteTPI(0x1A, TMDS_OUTPUT_CONTROL_MASK | AV_MUTE_MASK,
	TMDS_OUTPUT_CONTROL_POWER_DOWN | AV_MUTE_MUTED);
	
	//                                                                                   
	//                    
	g_sys.hdmiCableConnected = TRUE;
	g_sys.dsRxPoweredUp = TRUE;

/*
                       
            
      

                                                        
                                                                                                
*/
	siHdmiTx_Init();

	if (Sii9024A_HDCP_supported){
		DelayMS(200);
		HDCP_CheckStatus(ReadByteTPI(0x3D));
		}

	return VIDEO_MODE_SET_OK;
}

/*                                                                                */
/*                                     */
/*                                                                                 */
/*  */
/*                            */
/*                                                      */
/*                                                                                  */
/*                                             */
/*                                                 */
/*               */
/*               */
/*                                                                                */
byte SetAudioInfoFrames(byte ChannelCount, byte CodingType, byte SS, byte Fs, byte SpeakerConfig)
{
	byte B_Data[SIZE_AUDIO_INFOFRAME];	/*    */
	byte i;
	/*                  */

	TPI_TRACE_PRINT((">>SetAudioInfoFrames()\n"));

	for (i = 0; i < SIZE_AUDIO_INFOFRAME; i++)
		B_Data[i] = 0;

	WriteByteTPI(MISC_INFO_FRAMES_CTRL, DISABLE_AUDIO);	/*                                         */

	B_Data[0] = TYPE_AUDIO_INFOFRAMES;	/*      */
	B_Data[1] = AUDIO_INFOFRAMES_VERSION;	/*      */
	B_Data[2] = AUDIO_INFOFRAMES_LENGTH;	/*      */
	B_Data[3] = TYPE_AUDIO_INFOFRAMES +	/*                                         */
	    AUDIO_INFOFRAMES_VERSION + AUDIO_INFOFRAMES_LENGTH;

	B_Data[4] = ChannelCount;	/*                                                                       */
	B_Data[4] |= (CodingType << 4);	/*                                   */

	B_Data[5] = ((Fs & THREE_LSBITS) << 2) | (SS & TWO_LSBITS);

	B_Data[7] = SpeakerConfig;

	for (i = 4; i < SIZE_AUDIO_INFOFRAME; i++)
		B_Data[3] += B_Data[i];

	B_Data[3] = 0x100 - B_Data[3];

	WriteByteTPI(MISC_INFO_FRAMES_CTRL, EN_AND_RPT_AUDIO);	/*                                                   */

	WriteBlockTPI(MISC_INFO_FRAMES_TYPE, SIZE_AUDIO_INFOFRAME, B_Data);

	if (siHdmiTx.SyncMode == EMBEDDED_SYNC)
		EnableEmbeddedSync();

	return TRUE;
}

/*                                                                                */
/*                               */
/*                                  */
/*  */
/*                          */
/*               */
/*               */
/*                                                                                */
void SetAudioMute(byte audioMute)
{
	ReadModifyWriteTPI(0x26, AUDIO_MUTE_MASK, audioMute);
}

#ifndef F_9022A_9334
/*                                                                                */
/*                                   */
/*                                                                                           */
/*  */
/*                                                                  */
/*               */
/*               */
/*                                                                                */
void SetChannelLayout(byte Count)
{
	/*                                */
	WriteByteTPI(0xBC, 0x02);	/*                 */
	WriteByteTPI(0xBD, 0x2F);

	Count &= THREE_LSBITS;

	if (Count == TWO_CHANNEL_LAYOUT) {
		/*                */
		ReadClearWriteTPI(0xBE, BIT_1);
	}

	else if (Count == EIGHT_CHANNEL_LAYOUT) {
		/*              */
		ReadSetWriteTPI(0xBE, BIT_1);
	}
}
#endif

/*                                                                                */
/*                                    */
/*                                                                      */
/*  */
/*               */
/*                                                         */
/*                                        */
/*                   */
/*                                                                                */
byte siHdmiTx_AudioSet(void)
{
	TPI_TRACE_PRINT((">>siHdmiTx_AudioSet()\n"));

	SetAudioMute(AUDIO_MUTE_MUTED);	/*             */

	if (siHdmiTx.AudioMode == AMODE_I2S)	/*           */
	{
		ReadModifyWriteTPI(0x26, AUDIO_SEL_MASK, AUD_IF_I2S);	/*             */
		WriteByteTPI(0x25, 0x08 | AUD_DO_NOT_CHECK);	/*      */
	} else			/*             */
	{
		ReadModifyWriteTPI(0x26, AUDIO_SEL_MASK, AUD_IF_SPDIF);	/*             */
		WriteByteTPI(0x25, AUD_PASS_BASIC);	/*             */
	}

#ifndef F_9022A_9334
	if (siHdmiTx.AudioChannels == ACHANNEL_2CH)
		SetChannelLayout(TWO_CHANNELS);	/*                             */
	else
		SetChannelLayout(EIGHT_CHANNELS);
#else
	if (siHdmiTx.AudioChannels == ACHANNEL_2CH)
		ReadClearWriteTPI(0x26, BIT_5);	/*                                                       */
	else
		ReadSetWriteTPI(0x26, BIT_5);	/*                                                       */
#endif

	if (siHdmiTx.AudioMode == AMODE_I2S)	/*           */
	{
		/*                                                      */
		WriteByteTPI(0x1F, 0x80);	/*      */

		if (siHdmiTx.AudioChannels > ACHANNEL_2CH)
			WriteByteTPI(0x1F, 0x91);

		if (siHdmiTx.AudioChannels > ACHANNEL_4CH)
			WriteByteTPI(0x1F, 0xA2);

		if (siHdmiTx.AudioChannels > ACHANNEL_6CH)
			WriteByteTPI(0x1F, 0xB3);

		/*                                                                             */
		WriteByteTPI(0x21, 0x00);	/*      */
		WriteByteTPI(0x22, 0x00);
		WriteByteTPI(0x23, 0x00);
		WriteByteTPI(0x24, siHdmiTx.AudioFs);
		WriteByteTPI(0x25, (siHdmiTx.AudioFs << 4) | siHdmiTx.AudioWordLength);

		/*                                                  */
		WriteIndexedRegister(INDEXED_PAGE_1, 0x25, siHdmiTx.AudioWordLength);

		/*                           */
		WriteByteTPI(0x20, siHdmiTx.AudioI2SFormat);	/*             */
	}

	WriteByteTPI(0x27, REFER_TO_STREAM_HDR);
	SetAudioInfoFrames(siHdmiTx.AudioChannels & THREE_LSBITS, REFER_TO_STREAM_HDR,
			   REFER_TO_STREAM_HDR, REFER_TO_STREAM_HDR, 0x00);

	SetAudioMute(AUDIO_MUTE_NORMAL);	/*               */

	return AUDIO_MODE_SET_OK;
}

#ifdef F_9022A_9334
/*                                                                                */
/*                                   */
/*                                                                       */
/*  */
/*               */
/*                                                                */
/*                */
/*               */
/*                                                                                      */
/*                                                                                */
byte SetGBD_InfoFrame(void)
{
	byte CheckSum;

	TPI_TRACE_PRINT((">>SetGBD_InfoFrame()\n"));

	/*                                                           */
	WriteByteTPI(MISC_INFO_FRAMES_CTRL, DISABLE_MPEG);	/*                                               */
	WriteByteTPI(MISC_INFO_FRAMES_TYPE, TYPE_GBD_INFOFRAME);	/*             */
	WriteByteTPI(MISC_INFO_FRAMES_VER, NEXT_FIELD | GBD_PROFILE | AFFECTED_GAMUT_SEQ_NUM);	/*              */
	WriteByteTPI(MISC_INFO_FRAMES_LEN, ONLY_PACKET | CURRENT_GAMUT_SEQ_NUM);	/*              */

	CheckSum = TYPE_GBD_INFOFRAME +
	    NEXT_FIELD + GBD_PROFILE + AFFECTED_GAMUT_SEQ_NUM + ONLY_PACKET + CURRENT_GAMUT_SEQ_NUM;

	CheckSum = 0x100 - CheckSum;

	WriteByteTPI(MISC_INFO_FRAMES_CTRL, EN_AND_RPT_MPEG);	/*                                   */
	WriteByteTPI(MISC_INFO_FRAMES_CHKSUM, CheckSum);	/*                         */

	return GBD_SET_SUCCESSFULLY;
}
#endif

#ifdef DEV_SUPPORT_3D
/*                                                                                */
/*                           */
/*                                                                                              */
/*  */
/*               */
/*               */
/*                   */
/*                                                                                */
/*                */
/*                                                              */
#define VSIF_TYPE			0x81
#define VSIF_VERSION		0x01
#define VSIF_LEN				0x06

void Set_VSIF(void)
{
	byte i;
	byte Data[SIZE_MPEG_INFOFRAME];	/*    */

	for (i = 0; i < SIZE_MPEG_INFOFRAME; i++) {
		Data[i] = 0;
	}

	/*                                                      */
	WriteByteTPI(MISC_INFO_FRAMES_CTRL, DISABLE_MPEG);

	/*              */
	Data[0] = VSIF_TYPE;	/*                      */
	Data[1] = VSIF_VERSION;	/*                    */

	/*                                                          */
	Data[4] = 0x03;		/*                        */
	Data[5] = 0x0C;		/*                            */
	Data[6] = 0x00;		/*                        */

	/*                                       */
	Data[7] = siHdmiTx.HDMIVideoFormat << 5;

	/*                                                                                 */
	/*                                                    */
	switch (siHdmiTx.HDMIVideoFormat) {
	case VMD_HDMIFORMAT_HDMI_VIC:
		/*                                                            */
		/*                                               */
		Data[8] = siHdmiTx.VIC;
		Data[9] = 0;
		break;

	case VMD_HDMIFORMAT_3D:
		/*                                                     */
		/*                                                             */
		/*                               */
		Data[8] = siHdmiTx.ThreeDStructure << 4;
		/*                                                                       */
		/*                                 */
		if ((Data[8] >> 4) == VMD_3D_SIDEBYSIDEHALF) {
			Data[2] = VSIF_LEN;
			Data[9] = siHdmiTx.ThreeDExtData << 4;
		} else {
			Data[2] = VSIF_LEN - 1;
		}
		break;

	case VMD_HDMIFORMAT_CEA_VIC:
	default:
		Data[8] = 0;
		Data[9] = 0;
		break;
	}

	/*              */
	Data[3] = VSIF_TYPE +	/*                      */
	    VSIF_VERSION + Data[2];

	/*                                            */
	for (i = 4; i < SIZE_MPEG_INFOFRAME; i++) {
		Data[3] += Data[i];
	}
	/*                   */
	Data[3] = 0x100 - Data[3];	/*                */

	WriteByteTPI(MISC_INFO_FRAMES_CTRL, EN_AND_RPT_MPEG);	/*                                                   */

	WriteBlockTPI(MISC_INFO_FRAMES_TYPE, SIZE_MPEG_INFOFRAME, Data);	/*                                                     */
	WriteByteTPI(0xDE, 0x00);	/*                                                              */
}
#endif




/*                                                                               */
/*                                                                               */
/*                                              *///                               
/*                                                                                                */
/*                                              *///                               
/*                                                                               */
/*                                                                               */


/*                                                                                */
/*                           */
/*                                                                              */
/*  */
/*               */
/*                                                                   */
/*               */
/*                                                                                */
byte StartTPI(void)
{
	byte devID = 0x00;
	word wID = 0x0000;

	TPI_TRACE_PRINT((">>StartTPI()\n"));

	WriteByteTPI(0xC7, 0x00);	/*                                         */

	DelayMS(100);

	devID = ReadIndexedRegister(INDEXED_PAGE_0, 0x03);
	wID = devID;
	wID <<= 8;
	devID = ReadIndexedRegister(INDEXED_PAGE_0, 0x02);
	wID |= devID;

	devID = ReadByteTPI(0x1B);

	TPI_TRACE_PRINT(("0x%04X\n", (int)wID));
	TPI_TRACE_PRINT(("%s:%d:devID=0x%04x\n", __func__, __LINE__, devID));
	Sii9024A_HDCP_supported = true;

	if (wID == 0x9022)
		Sii9024A_HDCP_supported = false;
	if (devID == SII902XA_DEVICE_ID)
		return TRUE;

	TPI_TRACE_PRINT(("Unsupported TX, devID = 0x%X\n", (int)devID));
	return FALSE;
}

/*                                                                                */
/*                                    */
/*                                                                       */
/*  */
/*               */
/*                        */
/*               */
/*                                                                                */
byte siHdmiTx_TPI_Init(void)
{
	TPI_TRACE_PRINT(("\n>>siHdmiTx_TPI_Init()\n"));
	TPI_TRACE_PRINT(("\n%s\n", TPI_FW_VERSION));

	/*                            */
	g_sys.txPowerState = TX_POWER_STATE_D0;

	InitializeStateVariables();

	/*                     */
	TxHW_Reset();
	WriteByteTPI(0xF5, 0x00);
	/*                                     */
	if (StartTPI()) {
		if (Sii9024A_HDCP_supported) {
			TPI_DEBUG_PRINT(("siHdmiTx_TPI_Init,Sii9024A_HDCP_supported\n"));
			g_hdcp.HDCP_Override = FALSE;
			g_hdcp.HDCPAuthenticated = VMD_HDCP_AUTHENTICATED;
			HDCP_Init();
		}
#ifdef DEV_SUPPORT_CEC
		/*               */
#endif

		EnableInterrupts(HOT_PLUG_EVENT);

		return 0;
	}

	return EPERM;
}

/*                                                                                */
/*                                            */
/*                                                 */
/*  */
/*               */
/*               */
/*               */
/*                                                                                */
void OnDownstreamRxPoweredDown(void)
{
	TPI_DEBUG_PRINT(("DSRX -> Powered Down\n"));
	g_sys.dsRxPoweredUp = FALSE;

	if (g_hdcp.HDCP_Started == TRUE && Sii9024A_HDCP_supported)
		HDCP_Off();
	DisableTMDS();
	ReadModifyWriteTPI(0x1A, OUTPUT_MODE_MASK, OUTPUT_MODE_DVI);	/*                                      */
}

extern void HotPlugService(void);
/*                                                                                */
/*                                          */
/*                                             */
/*  */
/*               */
/*               */
/*               */
/*                                                                                */
void OnDownstreamRxPoweredUp(void)
{
	TPI_DEBUG_PRINT(("DSRX -> Powered Up\n"));
	g_sys.dsRxPoweredUp = TRUE;

	HotPlugService();
}

/*                                                                                */
/*                                          */
/*                                                 */
/*  */
/*               */
/*               */
/*               */
/*                                                                                */
void OnHdmiCableDisconnected(void)
{
	TPI_DEBUG_PRINT(("HDMI Disconnected\n"));

	g_sys.hdmiCableConnected = FALSE;

#ifdef DEV_SUPPORT_EDID
	g_edid.edidDataValid = FALSE;
#endif

	OnDownstreamRxPoweredDown();
	/*                          */
}

/*                                                                                */
/*                                       */
/*                                                  */
/*  */
/*               */
/*               */
/*               */
/*                                                                                */
void OnHdmiCableConnected(void)
{
	TPI_DEBUG_PRINT(("Cable Connected\n"));
	/*                                                                                 */
	/*             */

	g_sys.hdmiCableConnected = TRUE;

	if ((Sii9024A_HDCP_supported)
	    && (g_hdcp.HDCP_TxSupports == TRUE)
	    && (g_hdcp.HDCP_AksvValid == TRUE)
	    && (g_hdcp.HDCPAuthenticated == VMD_HDCP_AUTHENTICATED)) {
		TPI_DEBUG_PRINT(("Cable Connected, Sii9024A_HDCP_supported\n"));
		WriteIndexedRegister(INDEXED_PAGE_0, 0xCE, 0x00);	/*               */
		WriteIndexedRegister(INDEXED_PAGE_0, 0xCF, 0x00);
	}

	/*                                                                   */
	/*                          */
	/*                                                                                                                */

#ifdef DEV_SUPPORT_EDID
	DoEdidRead();
#endif

#ifdef READKSV
	ReadModifyWriteTPI(0xBB, 0x08, 0x08);
#endif

	if (IsHDMI_Sink())	/*                                                           */
	{
		TPI_DEBUG_PRINT(("HDMI Sink Detected\n"));
		ReadModifyWriteTPI(0x1A, OUTPUT_MODE_MASK, OUTPUT_MODE_HDMI);
	} else {
		TPI_DEBUG_PRINT(("DVI Sink Detected\n"));
		ReadModifyWriteTPI(0x1A, OUTPUT_MODE_MASK, OUTPUT_MODE_DVI);
	}
}

/*                                                                                */
/*                                        */
/*                                          */
/*                                                                                */
void siHdmiTx_PowerStateD0(void)
{
	ReadModifyWriteTPI(0x1E, TX_POWER_STATE_MASK, TX_POWER_STATE_D0);
	TPI_DEBUG_PRINT(("TX Power State D0\n"));
	g_sys.txPowerState = TX_POWER_STATE_D0;
}

/*                                                                                */
/*                                        */
/*                                          */
/*                                                                                */
void siHdmiTx_PowerStateD2(void)
{
	ReadModifyWriteTPI(0x1E, TX_POWER_STATE_MASK, TX_POWER_STATE_D2);
	TPI_DEBUG_PRINT(("TX Power State D2\n"));
	g_sys.txPowerState = TX_POWER_STATE_D2;
}

/*                                                                                */
/*                                              */
/*                                                       */
/*                                                                                */
void siHdmiTx_PowerStateD0fromD2(void)
{
	ReadModifyWriteTPI(0x1E, TX_POWER_STATE_MASK, TX_POWER_STATE_D0);

	if (Sii9024A_HDCP_supported)
		RestartHDCP();
	else
		EnableTMDS();

	TPI_DEBUG_PRINT(("TX Power State D0 from D2\n"));
	g_sys.txPowerState = TX_POWER_STATE_D0;
}


/*                                                                                */
/*                                        */
/*                                          */
/*                                                                                */
/*                     */
#define D3_hot

void siHdmiTx_PowerStateD3(void)
{
#ifdef D3_hot
	/*        */
	WriteByteTPI(0x3C, HOT_PLUG_EVENT | 0x08);
	WriteByteTPI(0x3D, 0xFF);
	ReadModifyWriteTPI(0x1E, TX_POWER_STATE_MASK, TX_POWER_STATE_D3);
	TPI_DEBUG_PRINT(("TX Power State D3 hot\n"));
	g_sys.txPowerState = TX_POWER_STATE_D3;
#endif

#ifdef D3_cold
	/*                                                                                               */
	WriteByteTPI(0x3D, 0xFF);
	TxHW_Reset();
	WriteByteTPI(0xC7, 0x00);
	DelayMS(100);
	WriteByteTPI(0x3C, HOT_PLUG_EVENT | 0x08);
	WriteByteTPI(0x3D, 0xFF);
	WriteByteTPI(0x1E, 0x04);
	ReadModifyWriteTPI(0x1E, TX_POWER_STATE_MASK, TX_POWER_STATE_D3);
	TPI_DEBUG_PRINT(("TX Power State D3 cold\n"));
	g_sys.txPowerState = TX_POWER_STATE_D3;
#endif
}

/*                         */

/*                                                                                */
/*                                 */
/*                                                                  */
/*  */
/*               */
/*                                                                   */
/*                              */
/*                                                                                */
void HotPlugService(void)
{
	TPI_TRACE_PRINT((">>HotPlugService()\n"));

	DisableInterrupts(0xFF);

	/*                                                                                 */

	siHdmiTx_Init();
}

/*                                                                                */
/*                                    */
/*                                                                         */
/*  */
/*               */
/*               */
/*               */
/*                                                                                */

void siHdmiTx_TPI_Poll(void)
{
	byte InterruptStatus;

	if (g_sys.txPowerState == TX_POWER_STATE_D0) {
		InterruptStatus = ReadByteTPI(0x3D);

		if (InterruptStatus & HOT_PLUG_EVENT)	/*                           */
		{
			TPI_DEBUG_PRINT(("HPD	-> "));
			ReadSetWriteTPI(0x3C, HOT_PLUG_EVENT);	/*                          */

			/*                                           */
			do {
				WriteByteTPI(0x3D, HOT_PLUG_EVENT);	/*                                                          */
				DelayMS(T_HPD_DELAY);	/*                                                                               */
				InterruptStatus = ReadByteTPI(0x3D);	/*                                */
			} while (InterruptStatus & HOT_PLUG_EVENT);	/*                                     */

			if (((InterruptStatus & HOT_PLUG_STATE) >> 2) != g_sys.hdmiCableConnected) {
				if (g_sys.hdmiCableConnected == TRUE) {
					OnHdmiCableDisconnected();
					hdmi_util.state_callback(0);
				} else {
					OnHdmiCableConnected();
					ReadModifyWriteIndexedRegister(INDEXED_PAGE_0, 0x0A, 0x08,
								       0x08);
				}

				if (g_sys.hdmiCableConnected == FALSE) {
					return;
				}

				/*                                      
     
            
     */
			}
			else if((g_sys.hdmiCableConnected)&&(Sii9024A_HDCP_supported))
			{
				TPI_DEBUG_PRINT (("HPD	-> deglitched"));
				ReadModifyWriteTPI(0x1A, TMDS_OUTPUT_CONTROL_MASK | AV_MUTE_MASK,
				TMDS_OUTPUT_CONTROL_POWER_DOWN | AV_MUTE_MUTED);
				HDCP_Off();
				DelayMS(100); 
				ReadModifyWriteTPI(0x1A, TMDS_OUTPUT_CONTROL_MASK, TMDS_OUTPUT_CONTROL_ACTIVE);
				WriteByteTPI(0x08, tpivmode[0]);   
			}
		}
		/*                */
		if (((InterruptStatus & RX_SENSE_STATE) >> 3) != g_sys.dsRxPoweredUp) {
			if (g_sys.hdmiCableConnected == TRUE) {
				if (g_sys.dsRxPoweredUp == TRUE) {
					OnDownstreamRxPoweredDown();
					hdmi_util.state_callback(0);
				} else {
					OnDownstreamRxPoweredUp();
					hdmi_util.state_callback(1);
				}
			}
			DelayMS(100); //                                                                              
			ClearInterrupt(RX_SENSE_EVENT);
		}
		/*                                       */
		if (InterruptStatus & RX_SENSE_EVENT) {
			ClearInterrupt(RX_SENSE_EVENT);
		}
		/*                                          */
		if (InterruptStatus & AUDIO_ERROR_EVENT) {
			/*                                                  */
			/*                                                                               */
			ClearInterrupt(AUDIO_ERROR_EVENT);
		}

		if ((Sii9024A_HDCP_supported)
		    && (g_sys.hdmiCableConnected == TRUE)
		    && (g_sys.dsRxPoweredUp == TRUE)
		    && (g_hdcp.HDCPAuthenticated == VMD_HDCP_AUTHENTICATED)) {
			HDCP_CheckStatus(InterruptStatus);
		}
#ifdef DEV_SUPPORT_CEC
		SI_CecHandler(0, 0);
#endif
	}

	if (g_sys.txPowerState == TX_POWER_STATE_D3)
	{
		/*               */
		siHdmiTx_TPI_Init();
		/*                      */
		TPI_DEBUG_PRINT(("***up from INT ***\n"));
	}

}


/*                                                                                */
/*                                    */
/*                                                */
/*  */
/*                     */
/*               */
/*               */
/*                                                                                */
void siHdmiTx_VideoSel(byte vmode)
{
	siHdmiTx.HDMIVideoFormat = VMD_HDMIFORMAT_CEA_VIC;
	siHdmiTx.ColorSpace = RGB;
	siHdmiTx.ColorDepth = VMD_COLOR_DEPTH_8BIT;
	siHdmiTx.SyncMode = EXTERNAL_HSVSDE;

	switch (vmode) {
	case HDMI_480I60_4X3:
		siHdmiTx.VIC = 6;
		siHdmiTx.AspectRatio = VMD_ASPECT_RATIO_4x3;
		siHdmiTx.Colorimetry = COLORIMETRY_601;
		siHdmiTx.TclkSel = X2;
		break;

	case HDMI_576I50_4X3:
		siHdmiTx.VIC = 21;
		siHdmiTx.AspectRatio = VMD_ASPECT_RATIO_4x3;
		siHdmiTx.Colorimetry = COLORIMETRY_601;
		siHdmiTx.TclkSel = X2;
		break;

	case HDMI_480P60_4X3:
		siHdmiTx.VIC = 2;
		siHdmiTx.AspectRatio = VMD_ASPECT_RATIO_4x3;
		siHdmiTx.Colorimetry = COLORIMETRY_601;
		siHdmiTx.TclkSel = X1;
		break;

	case HDMI_576P50_4X3:
		siHdmiTx.VIC = 17;
		siHdmiTx.AspectRatio = VMD_ASPECT_RATIO_4x3;
		siHdmiTx.Colorimetry = COLORIMETRY_601;
		siHdmiTx.TclkSel = X1;
		break;

	case HDMI_720P60:
		siHdmiTx.VIC = 4;
		siHdmiTx.AspectRatio = VMD_ASPECT_RATIO_16x9;
		siHdmiTx.Colorimetry = COLORIMETRY_709;
		siHdmiTx.TclkSel = X1;
		break;

	case HDMI_720P50:
		siHdmiTx.VIC = 19;
		siHdmiTx.AspectRatio = VMD_ASPECT_RATIO_16x9;
		siHdmiTx.Colorimetry = COLORIMETRY_709;
		siHdmiTx.TclkSel = X1;
		break;

	case HDMI_1080I60:
		siHdmiTx.VIC = 5;
		siHdmiTx.AspectRatio = VMD_ASPECT_RATIO_16x9;
		siHdmiTx.Colorimetry = COLORIMETRY_709;
		siHdmiTx.TclkSel = X1;
		break;

	case HDMI_1080I50:
		siHdmiTx.VIC = 20;
		siHdmiTx.AspectRatio = VMD_ASPECT_RATIO_16x9;
		siHdmiTx.Colorimetry = COLORIMETRY_709;
		siHdmiTx.TclkSel = X1;
		break;

	case HDMI_1080P60:
		siHdmiTx.VIC = 16;
		siHdmiTx.AspectRatio = VMD_ASPECT_RATIO_16x9;
		siHdmiTx.Colorimetry = COLORIMETRY_709;
		siHdmiTx.TclkSel = X1;
		break;

	case HDMI_1080P50:
		siHdmiTx.VIC = 31;
		siHdmiTx.AspectRatio = VMD_ASPECT_RATIO_16x9;
		siHdmiTx.Colorimetry = COLORIMETRY_709;
		siHdmiTx.TclkSel = X1;
		break;

	case HDMI_1024_768_60:
		siHdmiTx.VIC = PC_BASE + 13;
		siHdmiTx.AspectRatio = VMD_ASPECT_RATIO_4x3;
		siHdmiTx.Colorimetry = COLORIMETRY_709;
		siHdmiTx.TclkSel = X1;
		break;

	case HDMI_800_600_60:
		siHdmiTx.VIC = PC_BASE + 9;
		siHdmiTx.AspectRatio = VMD_ASPECT_RATIO_4x3;
		siHdmiTx.Colorimetry = COLORIMETRY_709;
		siHdmiTx.TclkSel = X1;
		break;

	case HDMI_1080P30:
		siHdmiTx.VIC = 34;
		siHdmiTx.AspectRatio = VMD_ASPECT_RATIO_16x9;
		siHdmiTx.Colorimetry = COLORIMETRY_709;
		siHdmiTx.TclkSel = X1;
		break;

	case HDMI_1080P24:
		siHdmiTx.VIC = 32;
		siHdmiTx.AspectRatio = VMD_ASPECT_RATIO_16x9;
		siHdmiTx.Colorimetry = COLORIMETRY_709;
		siHdmiTx.TclkSel = X1;
		break;

	default:
		break;
	}

	TPI_DEBUG_PRINT(("siHdmiTx_VideoSel vmode=%d\n", vmode));
}

/*                                                                                */
/*                                    */
/*                                                */
/*  */
/*                   */
/*               */
/*               */
/*                                                                                */
void siHdmiTx_AudioSel(byte Afs)
{
	siHdmiTx.AudioMode = AMODE_I2S;
	siHdmiTx.AudioChannels = ACHANNEL_2CH;
	siHdmiTx.AudioFs = Afs;
	siHdmiTx.AudioWordLength = ALENGTH_16BITS;
	siHdmiTx.AudioI2SFormat = (MCLK256FS << 4) | SCK_SAMPLE_RISING_EDGE | 0x00;	/*                      */
}
