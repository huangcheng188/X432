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

#include "1wire_UART.h"

#ifndef uchar
typedef unsigned char uchar;
#endif

// Constants
#define TRUE 1
#define FALSE 0

#define DS2488_FAMILY   0x59

// 1-Wire device function commands
#define CMD_WRITE_CONFIGURATION		0x11
#define CMD_READ_CONFIGURATION		0x22
#define CMD_WRITE_BUFFER			0x33
#define CMD_READ_BUFFER				0x44
#define CMD_READ_STATUS				0x55
#define CMD_PIO_WRITE				0x66
#define CMD_PIO_READ				0x77
#define CMD_WRITE_TIMEOUT_VALUE		0x88
#define CMD_READ_TIMEOUT_VALUE		0x99

// 1 - Wire selection methods
#define SELECT_SKIP     0
#define SELECT_RESUME   1
#define SELECT_MATCH    2
#define SELECT_ODMATCH  3
#define SELECT_SEARCH   4
#define SELECT_READROM  5
#define SELECT_ODSKIP   6

// 1-Wire ROM function commands
#define ROM_CMD_SKIP            0xCC
#define ROM_CMD_READ			0x33
#define ROM_CMD_MATCH			0x55
#define ROM_CMD_SEARCH			0xF0
#define ROM_CMD_RESUME          0xA5


// Status Byte Bit Mask
#define MASK_PSW		0x80
#define MASK_TRST		0x40
#define MASK_TOKS		0x20
#define MASK_CMPS		0x10
#define MASK_IOBS		0x08
#define MASK_IOAS		0x04
#define MASK_BUFB		0x02
#define MASK_BUFA		0x01

#ifndef DS2488
extern int WriteConfiguration(uchar PULLUP, uchar QM, uchar PTM, uchar BUFBPE, uchar BUFAPE, uchar SEL);
extern int ReadConfiguration(uchar* config_data);
extern int WriteBuffer(int BLEN, uchar *BUF);
extern int ReadBuffer(uchar *BUF);
extern int ReadStatus(uchar* status_byte);
extern int PIO_Write(uchar PIO_Output_Byte);
extern int PIO_Read(uchar* PIO_Input_Byte);
extern int WriteTimeoutValue(uchar TVAL);
extern int ReadTimeoutValue(uchar* TVAL);

extern void setDeviceSelectMode(int method);
extern int DeviceSelect();
#endif

