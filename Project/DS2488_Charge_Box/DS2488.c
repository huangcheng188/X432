/*******************************************************************************
* Copyright (C) Maxim Integrated Products, Inc., All rights Reserved.
*
* This software is protected by copyright laws of the United States and
* of foreign countries. This material may also be protected by patent laws
* and technology transfer regulations of the United States and of foreign
* countries. This software is furnished under a license agreement and/or a
* nondisclosure agreement and may only be used or reproduced in accordance
* with the terms of those agreements. Dissemination of this information to
* any party or parties not specified in the license agreement and/or
* nondisclosure agreement is expressly prohibited.
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*******************************************************************************
*/

//#include <windows.h>
#include <stdio.h>

#include "serial_win32ex.h"
#include "DS2488.h"

static int select_method = SELECT_SKIP;
unsigned short CRC16;

// Functional Declarations
void setDeviceSelectMode(int method);
int DeviceSelect();
static unsigned short docrc16(unsigned short data);
int WriteConfiguration(uchar PULLUP, uchar QM, uchar PTM, uchar BUFBPE, uchar BUFAPE, uchar SEL);
int ReadConfiguration(uchar* config_data);
int WriteBuffer(int BLEN, uchar *BUF);
int ReadBuffer(uchar *BUF);
int ReadStatus(uchar* status_byte);
int PIO_Write(uchar PIO_Output_Byte);
int PIO_Read(uchar* PIO_Input_Byte);
int WriteTimeoutValue(uchar TVAL);
int ReadTimeoutValue(uchar* TVAL);

/// Select the 1-Wire ROM command for all Device Function Commands.
///
/// @param method Desired ROM command from macros
/// @returns Nothing
void setDeviceSelectMode(int method)
{
	select_method = method;
}

/// Executes the selected ROM command.
///
/// @returns Nothing
int DeviceSelect()
{
	switch (select_method)
	{
	case SELECT_SKIP:
		return OWSkipROM();
	case SELECT_RESUME:
		return OWResume();
	case SELECT_MATCH:
		return OWMatchROM();
	case SELECT_ODMATCH:
		return OWOverdriveMatchROM();
	case SELECT_SEARCH:
		return OWFirst();
	case SELECT_READROM:
		return OWReadROM();
	case SELECT_ODSKIP:
		return OWOverdriveSkipROM();
	};

	return 0;
}

//---------------------------------------------------------------------------
/// Device Function Command: Write Configuration (11h)
///
/// @param SEL - PIOC function (1); CD pin function (0)
/// @param BUFAPE - PIOA outputs invert of BUFA flag (1); PIOA normal (0)
/// @param BUFBPE - PIOB outputs invert of BUFB flag (1); PIOB normal (0)
/// @param PTM - Enabled (1); Disabled (0)
/// @param QM - Enabled (1); Disabled (0)
/// @param PULLUP - Enable weak pullup from IOA to VREG (1); Disable weak
///					pullup and enable weak pulldown on IOA.
/// @return
/// TRUE - command succesful @n
/// FALSE - command failed
int WriteConfiguration(uchar PULLUP, uchar QM, uchar PTM, uchar BUFBPE, uchar BUFAPE, uchar SEL)
{
	unsigned char command = CMD_WRITE_CONFIGURATION;
	unsigned char parameter = (PULLUP << 5) | (QM << 4) | (PTM << 3) | (BUFBPE << 2) | (BUFAPE << 1) | SEL;
	unsigned char crc16_read[2];

	//seed the crc
	CRC16 = 0;

	// Reset/presence
	if (!DeviceSelect())
		return FALSE;

	// Send cmd
	OWWriteByte(command);
	docrc16(command);

	// Send parameter
	OWWriteByte(parameter);
	docrc16(parameter);

	// Read CRC16
	crc16_read[0] = OWReadByte();
	crc16_read[1] = OWReadByte();

	// Perform CRC16 of last 2 bytes received
	docrc16(crc16_read[0]);
	docrc16(crc16_read[1]);

	// verify CRC16 is correct
	return (CRC16 == 0xB001);

}


//---------------------------------------------------------------------------
/// Device Function Command: Read Configuration (22h)
///
/// @return
/// 
/// config_data - data read out from the configuration register 
///
/// TRUE - command succesful @n
/// FALSE - command failed
int ReadConfiguration(uchar* config_data)
{
	unsigned char command = CMD_READ_CONFIGURATION;
	unsigned char crc16_read[2];

	//seed the crc
	CRC16 = 0;

	// Reset/presence
	if (!DeviceSelect())
		return FALSE;

	// Send cmd
	OWWriteByte(command);
	docrc16(command);

	// Read
	*config_data = OWReadByte();
	docrc16(*config_data);

	// Read CRC16
	crc16_read[0] = OWReadByte();
	crc16_read[1] = OWReadByte();

	// Perform CRC16 of last 2 bytes received
	docrc16(crc16_read[0]);
	docrc16(crc16_read[1]);

	// verify CRC16 is correct
	return (CRC16 == 0xB001);

}


//---------------------------------------------------------------------------
/// Device Function Command: Write Buffer (33h)
///
/// @param BLEN - Data length to write; Max 8
/// @param BUF - buffer array to send
///
/// @return
/// TRUE - command succesful @n
/// FALSE - command failed
int WriteBuffer(int BLEN, uchar *BUF)
{
	unsigned char command = CMD_WRITE_BUFFER;
	unsigned char crc16_read[2];
	unsigned char buffer[8];

	memcpy(&buffer, BUF, 8);

	//seed the crc
	CRC16 = 0;

	// Reset/presence
	if (!DeviceSelect())
		return FALSE;

	// Send cmd
	OWWriteByte(command);
	docrc16(command);

	// Send Parameter Length
	OWWriteByte(BLEN);
	docrc16(BLEN);

	// Send buffer
	for (int i = 0; i < BLEN; i++)
	{
		OWWriteByte(buffer[i]);
		docrc16(buffer[i]);
	}

	// Read CRC16
	crc16_read[0] = OWReadByte();
	crc16_read[1] = OWReadByte();

	// Perform CRC16 of last 2 bytes received
	docrc16(crc16_read[0]);
	docrc16(crc16_read[1]);

	// verify CRC16 is correct
	return (CRC16 == 0xB001);
}

//---------------------------------------------------------------------------
/// Device Function Command: Read Buffer (44h)
///
/// BUF - buffer array output
///
/// @return
/// TRUE - command succesful @n
/// FALSE - command failed
int ReadBuffer(uchar *BUF)
{
	unsigned char command = CMD_READ_BUFFER;
	unsigned char blen = 0xFF;
	unsigned char crc16_read[2];

	//seed the crc
	CRC16 = 0;

	// Reset/presence
	if (!DeviceSelect())
		return FALSE;

	// Send cmd
	OWWriteByte(command);
	docrc16(command);

	// Read BLEN
	blen = OWReadByte();
	if (blen == 0xFF)
		return FALSE;
	docrc16(blen);

	// Read buffer
	for (int i = 0; i < blen; i++)
	{
		BUF[i] = OWReadByte();
		docrc16(BUF[i]);
	}

	// Read CRC16
	crc16_read[0] = OWReadByte();
	crc16_read[1] = OWReadByte();

	// Perform CRC16 of last 2 bytes received
	docrc16(crc16_read[0]);
	docrc16(crc16_read[1]);

	// verify CRC16 is correct
	return (CRC16 == 0xB001);
}


//---------------------------------------------------------------------------
/// Device Function Command: Read Status (55h)
///
/// @return
/// 
/// status_byte - data read out from the status register 
///
/// TRUE - command succesful @n
/// FALSE - command failed
int ReadStatus(uchar* status_byte)
{
	unsigned char command = CMD_READ_STATUS;
	unsigned char crc16_read[2];



	// Reset/presence
	if (!DeviceSelect())
		return FALSE;

	//seed the crc
	CRC16 = 0;

	// Send cmd
	OWWriteByte(command);
	docrc16(command);

	// Read
	*status_byte = OWReadByte();
	docrc16(*status_byte);

	// Read CRC16
	crc16_read[0] = OWReadByte();
	crc16_read[1] = OWReadByte();

	// Perform CRC16 of last 2 bytes received
	docrc16(crc16_read[0]);
	docrc16(crc16_read[1]);

	// verify CRC16 is correct
	return (CRC16 == 0xB001);
}

//---------------------------------------------------------------------------
/// Device Function Command: PIO Write (66h)
///
/// @param PIO_Output_Byte:  Sets the pin output values.
/// 0 (bit3),PIOCS(bit 2),PIOBS(bit 1), & PIOAS(bit 0) 
/// and 1's compliment 1(bit 7),/PIOCS(bit 6),/PIOBS(bit 5), & /PIOAS(bit 4) 
///
/// @return
/// TRUE - command succesful @n
/// FALSE - command failed
int PIO_Write(uchar PIO_Output_Byte)
{
	unsigned char command = CMD_PIO_WRITE;
	unsigned char parameter = PIO_Output_Byte;
	unsigned char crc16_read[2];

	//seed the crc
	CRC16 = 0;

	// Reset/presence
	if (!DeviceSelect())
		return FALSE;

	// Send cmd
	OWWriteByte(command);
	docrc16(command);

	// Send parameter
	OWWriteByte(parameter);
	docrc16(parameter);

	// Read CRC16
	crc16_read[0] = OWReadByte();
	crc16_read[1] = OWReadByte();

	// Perform CRC16 of last 2 bytes received
	docrc16(crc16_read[0]);
	docrc16(crc16_read[1]);

	// verify CRC16 is correct
	return (CRC16 == 0xB001);

}

//---------------------------------------------------------------------------
/// Device Function Command: PIO Read (77h)
///
/// @param PIO_Input_Byte: Provides PIO pin values. 
/// 0 (bit3),PIOCS(bit 2),PIOBS(bit 1), & PIOAS(bit 0) 
/// and 1's compliment 1(bit 7),/PIOCS(bit 6),/PIOBS(bit 5), & /PIOAS(bit 4) 
///
/// @return
/// TRUE - command succesful @n
/// FALSE - command failed
int PIO_Read(uchar* PIO_Input_Byte)
{
	unsigned char command = CMD_PIO_READ;
	unsigned char crc16_read[2];

	//seed the crc
	CRC16 = 0;

	// Reset/presence
	if (!DeviceSelect())
		return FALSE;

	// Send cmd
	OWWriteByte(command);
	docrc16(command);

	// Read parameter
	*PIO_Input_Byte = OWReadByte();
	docrc16(*PIO_Input_Byte);

	// Read CRC16
	crc16_read[0] = OWReadByte();
	crc16_read[1] = OWReadByte();

	// Perform CRC16 of last 2 bytes received
	docrc16(crc16_read[0]);
	docrc16(crc16_read[1]);

	// verify CRC16 is correct
	return (CRC16 == 0xB001);
}

//---------------------------------------------------------------------------
/// Device Function Command: Write Timeout Value (88h)
///
/// @param TVAL - Set timer duration = TVAL x 100us
///
/// @return
/// TRUE - command succesful @n
/// FALSE - command failed
int WriteTimeoutValue(uchar TVAL)
{
	unsigned char command = CMD_WRITE_TIMEOUT_VALUE;
	unsigned char parameter = TVAL;
	unsigned char crc16_read[2];

	//seed the crc
	CRC16 = 0;

	// Reset/presence
	if (!DeviceSelect())
		return FALSE;

	// Send cmd
	OWWriteByte(command);
	docrc16(command);

	// Send parameter
	OWWriteByte(parameter);
	docrc16(parameter);

	// Read CRC16
	crc16_read[0] = OWReadByte();
	crc16_read[1] = OWReadByte();

	// Perform CRC16 of last 2 bytes received
	docrc16(crc16_read[0]);
	docrc16(crc16_read[1]);

	// verify CRC16 is correct
	return (CRC16 == 0xB001);

}

//---------------------------------------------------------------------------
/// Device Function Command: Read Timeout Value (99h)
///
/// @param TVAL - Read timer duration = TVAL x 100us
///
/// @return
/// TRUE - command succesful @n
/// FALSE - command failed
int ReadTimeoutValue(uchar* TVAL)
{
	unsigned char command = CMD_READ_TIMEOUT_VALUE;
	unsigned char crc16_read[2];

	//seed the crc
	CRC16 = 0;

	// Reset/presence
	if (!DeviceSelect())
		return FALSE;

	// Send cmd
	OWWriteByte(command);
	docrc16(command);

	// Send parameter
	*TVAL = OWReadByte();
	docrc16(*TVAL);

	// Read CRC16
	crc16_read[0] = OWReadByte();
	crc16_read[1] = OWReadByte();

	// Perform CRC16 of last 2 bytes received
	docrc16(crc16_read[0]);
	docrc16(crc16_read[1]);

	// verify CRC16 is correct
	return (CRC16 == 0xB001);
}

//--------------------------------------------------------------------------
// Calculate a new CRC16 from the input data shorteger.  Return the current
// CRC16 and also update the global variable CRC16.
//
static short oddparity[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };

static unsigned short docrc16(unsigned short data)
{
	data = (data ^ (CRC16 & 0xff)) & 0xff;
	CRC16 >>= 8;

	if (oddparity[data & 0xf] ^ oddparity[data >> 4])
		CRC16 ^= 0xc001;

	data <<= 6;
	CRC16 ^= data;
	data <<= 1;
	CRC16 ^= data;

	return CRC16;
}
