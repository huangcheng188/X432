//------------Copyright (C) 2012 Maxim Integrated Products --------------
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL MAXIM INTEGRATED PRODCUTS BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Maxim Integrated Products
// shall not be used except as stated in the Maxim Integrated Products
// Branding Policy.
// ---------------------------------------------------------------------------
//
// DS2488.h - Include file for DS2488.c
//

//add huangcheng 

#define TMEX

#define true  1
#define false 0
  
#define CHARGING_POWER_ON 1
#define LOGIC_HIGH  1

typedef unsigned char BOOL;
typedef unsigned long DWORD;

#define MS_RING_ON 0x01
#define MS_DSR_ON  0x01

//#define TOKEN_IOA 0
//#define TOKEN_IOB 1
//#define TOKEN_PTM 2

//#define PARMSET_115200 115200
//#define PARMSET_800000 800000

//#define ZERO_BIT  0
//#define ONE_BIT   1

#define Sleep HAL_Delay 
//add end

/* type defs */
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned long  ulong;


// misc constants
#define TRUE    1
#define FALSE   0


// 1-Wire function commands
#define CMD_WRITE_CONFIGURE             0x11
#define CMD_READ_CONFIGURE              0x22
#define CMD_WRITE_BUFFER     		0x33
#define CMD_READ_BUFFER  		0x44
#define CMD_READ_STATUS        		0x55
#define CMD_WRITE_PIO      		0x66
#define CMD_READ_PIO          		0x77
#define CMD_WRITE_TIMEOUT  		0x88
#define CMD_READ_TIMEOUT    		0x99

//bit definition for configuration
#define CONFIGURE_SEL                  0x01
#define CONFIGURE_BUFAPE	        0x02
#define CONFIGURE_BUFBPE	        0x04
#define CONFIGURE_PTM	       		0x08
#define CONFIGURE_QM	       		0x10
#define CONFIGURE_PULLUP	       	0x20

//bit definition for Status
#define STATUS_BUFA       			0x01	//Buffer A Flag (BUFA). Indicates the buffer was written from the IOA link.
#define STATUS_BUFB       			0x02	//Buffer B Flag (BUFB). Indicates the buffer was written from the IOB link.
#define STATUS_IOAS       			0x04	//IOA State (IOAS)
#define STATUS_IOBS       			0x08	//IOB State (IOBS)
#define STATUS_CMPS       			0x10	//Comparator State (CMPS)
#define STATUS_TOKS       			0x20	//Token State (TOKS)
#define STATUS_TRST       			0x40	//Timer Reset (TRST)
#define STATUS_PSW       			0x80	//Power Source (PSW)

//define PIO bit
#define PIOAS                   0x01
#define PIOBS	                0x02
#define PIOCS	                0x04



#define SELECT_SKIP     0
#define SELECT_RESUME   1
#define SELECT_MATCH    2
#define SELECT_ODMATCH  3
#define SELECT_SEARCH   4
#define SELECT_READROM  5
#define SELECT_ODSKIP   6



#define DS2488_FAMILY   0x59



#ifndef uchar
   typedef unsigned char uchar;
#endif

#ifndef DS2488_DEVICE

extern int DS2488WriteConfigure(uchar *parameter);
extern int DS2488ReadConfigure(uchar *parameter);
extern int DS2488WriteBuffer(uchar *buf, uchar Buf_len);
extern int DS2488ReadBuffer(uchar *buf, uchar *Buf_len);
extern int DS2488ReadStatus(uchar *Status);
extern int DS2488WritePIO(uchar *PIO_Output);
extern int DS2488ReadPIO(uchar *PIO_Input);
extern int DS2488WriteTimeOutValue(uchar *tVAL);
extern int DS2488ReadTimeOutValue(uchar *tVAL);

//high level application function
extern int Detect_Earbuds(uchar *rom_id);

extern unsigned short docrc16(unsigned short data);
extern int DeviceSelect();

#endif


