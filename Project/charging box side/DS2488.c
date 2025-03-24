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
// DS2488.c - 1-Wire functions to authenticate and write to 
//    the DS2488.
//

//#define  One_WIRE_INTERFACE
//#ifdef One_WIRE_INTERFACE
//#include "OneWire_protocol.h"
//#endif

#define DS2488_DEVICE
#include "DS2488.h"
extern unsigned char CRC8;

//#include <memory.h>
#include <string.h>

// function declarations
int DS2488WriteConfigure(uchar *parameter);
int DS2488ReadConfigure(uchar *parameter);
int DS2488WriteBuffer(uchar *buf, uchar Buf_len);
int DS2488ReadBuffer(uchar *buf, uchar *Buf_len);
int DS2488ReadStatus(uchar *Status);
int DS2488WritePIO(uchar *PIO_Output);
int DS2488ReadPIO(uchar *PIO_Input);
int DS2488WriteTimeOutValue(uchar *tVAL);
int DS2488ReadTimeOutValue(uchar *tVAL);

//high level application functions
int Detect_Earbuds(uchar *buf);    //detect by directly reading 64-bit ROMID no matter how many earbuds located in box

unsigned short docrc16(unsigned short data);
int DeviceSelect();

// External function 
extern void msDelay(int delay);

// misc state
static unsigned short CRC16;

static int select_method = SELECT_SKIP;
static int SPECIAL_MODE = 0;
static uchar SPECIAL_VALUES[2]; 

//-----------------------------------------------------------------------------
// ------ DS2488 Functions
//-----------------------------------------------------------------------------
//  Write configuration byte to DS2488. 
// 
//  Parameters
//     *parameter - a pointer to configure parameter
//
//  Returns: TRUE - configure parameter written successfully
//           FALSE - Failed to write configure parameter (no presence or invalid CRC16)
//
int DS2488WriteConfigure(uchar *parameter)
{
   uchar pbuf[16];
   int cnt, i;
   cnt = 0;
   pbuf[cnt++]=CMD_WRITE_CONFIGURE;
   pbuf[cnt++]=parameter[0];
  //for read 2-byte CRC16
   pbuf[cnt++]=0xff;		
   pbuf[cnt++]=0xff;
  //sending data packet
   for(i=0;i<cnt;i++) pbuf[i]=WriteByte(pbuf[i]); 
   memcpy(parameter, &pbuf[1], 3);  //write configure parameter with the reading CRC-16 bytes
// run the CRC over this data packet
   CRC16 = 0;
   for(i=0;i<cnt;i++) docrc16(pbuf[i]);
// return result of inverted CRC
   return (CRC16 == 0xB001); //not 0 because that the calculating result is CRC16 and the reading result is inverted CRC16  
	
}

//-----------------------------------------------------------------------------
//  Read configuration byte from DS2488. 
// 
//  Parameters
//     *parameter - a pointer to configure parameter
//
//  Returns: TRUE - configure parameter is read successfully
//           FALSE - Failed to read configure parameter (no presence or invalid CRC16)
//
int DS2488ReadConfigure(uchar *parameter)
{
   uchar pbuf[16];
   int cnt, i;
   cnt = 0;
   pbuf[cnt++]=CMD_READ_CONFIGURE;
   pbuf[cnt++]=0xff;	//for reading configure parameter
  //for read 2-byte CRC16
   pbuf[cnt++]=0xff;		
   pbuf[cnt++]=0xff;
  //sending data packet
   for(i=0;i<cnt;i++) pbuf[i]=WriteByte(pbuf[i]);
//   parameter[0]=pbuf[1];	//reading configure parameter
   memcpy(parameter, &pbuf[1], 3);  //reading configure parameter with the reading CRC-16 bytes
// run the CRC over this data packet
   CRC16 = 0;
   for(i=0;i<cnt;i++) docrc16(pbuf[i]);
// return result of inverted CRC
   return (CRC16 == 0xB001); //not 0 because that the calculating result is CRC16 and the reading result is inverted CRC16  
	
}

//-----------------------------------------------------------------------------
//  Write buffer to DS2488. 
// 
//  Parameters
//     *buf - a pointer to buffer data be sent
//		Buf_len - buffer data length
//
//  Returns: TRUE - configure parameter written successfully
//           FALSE - Failed to write configure parameter (no presence or invalid CRC16)
//
int DS2488WriteBuffer(uchar *buf, uchar Buf_len)
{
   uchar pbuf[16];
   int cnt, i;
   cnt = 0;
   pbuf[cnt++]=CMD_WRITE_BUFFER;
   if( Buf_len >8 ) Buf_len=8;
   pbuf[cnt++]=Buf_len;
   for( i=0;i<Buf_len;i++ ) pbuf[cnt++]=buf[i];
  //for read 2-byte CRC16
   pbuf[cnt++]=0xff;		
   pbuf[cnt++]=0xff;
  //sending data packet
   for(i=0;i<cnt;i++) pbuf[i]=WriteByte(pbuf[i]);
// run the CRC over this data packet
   CRC16 = 0;
   for(i=0;i<cnt;i++) docrc16(pbuf[i]);
// return result of inverted CRC
   return (CRC16 == 0xB001); //not 0 because that the calculating result is CRC16 and the reading result is inverted CRC16  
	
}

//-----------------------------------------------------------------------------
//  Read buffer from DS2488. 
// 
//  Parameters
//     *buf - a pointer to buffer data be received
//		*Buf_len - a pointer to buffer data length
//
//  Returns: TRUE - configure parameter written successfully
//           FALSE - Failed to write configure parameter (no presence or invalid CRC16)
//
int DS2488ReadBuffer(uchar *buf, uchar *Buf_len)
{
   uchar pbuf[16];
   int cnt, i;
   cnt = 0;
   pbuf[cnt++]=CMD_READ_BUFFER;
   pbuf[cnt++]=0xff;	//for reading buffer data length
   for(i=0;i<cnt;i++) pbuf[i]=WriteByte(pbuf[i]);   
   Buf_len[0]=pbuf[1];
   if( Buf_len[0] > 8 ) return false;
   for( i=0; i< Buf_len[0]+2; i++ ) pbuf[cnt++]=buf[i]=ReadByte();   //read with 2-byte CRC16
// run the CRC over this data packet
   CRC16 = 0;
   for(i=0;i<cnt;i++) docrc16(pbuf[i]);
// return result of inverted CRC
   return (CRC16 == 0xB001); //not 0 because that the calculating result is CRC16 and the reading result is inverted CRC16  
	
}

//-----------------------------------------------------------------------------
//  Read status byte from DS2488. 
// 
//  Parameters
//     *parameter - a pointer to status byte
//
//  Returns: TRUE - status byte is read successfully
//           FALSE - Failed to read status byte (no presence or invalid CRC16)
//
int DS2488ReadStatus(uchar *Status)
{
   uchar pbuf[16];
   int cnt, i;
   cnt = 0;
   pbuf[cnt++]=CMD_READ_STATUS;
   pbuf[cnt++]=0xff;	//for reading status byte
  //for read 2-byte CRC16
   pbuf[cnt++]=0xff;		
   pbuf[cnt++]=0xff;
  //sending data packet
   for(i=0;i<cnt;i++) pbuf[i]=WriteByte(pbuf[i]);
//   Status[0]=pbuf[1];	//reading configure parameter
   memcpy(Status, &pbuf[1], 3);
// run the CRC over this data packet
   CRC16 = 0;
   for(i=0;i<cnt;i++) docrc16(pbuf[i]);
// return result of inverted CRC
   return (CRC16 == 0xB001); //not 0 because that the calculating result is CRC16 and the reading result is inverted CRC16  
	
}

//-----------------------------------------------------------------------------
//  Write PIO logic to DS2488. 
// 
//  Parameters
//     *PIO_Output - a pointer to PIO Output
//
//  Returns: TRUE - PIO Output written successfully
//           FALSE - Failed to write PIO Output (no presence or invalid CRC16)
//
int DS2488WritePIO(uchar *PIO_Output)
{
   uchar pbuf[16];
   int cnt, i;
   cnt = 0;
   pbuf[cnt++]=CMD_WRITE_PIO;
   pbuf[cnt++]=PIO_Output[0];
  //for read 2-byte CRC16
   pbuf[cnt++]=0xff;		
   pbuf[cnt++]=0xff;
  //sending data packet
   for(i=0;i<cnt;i++) pbuf[i]=WriteByte(pbuf[i]);
// run the CRC over this data packet
   CRC16 = 0;
   for(i=0;i<cnt;i++) docrc16(pbuf[i]);
// return result of inverted CRC
   return (CRC16 == 0xB001); //not 0 because that the calculating result is CRC16 and the reading result is inverted CRC16  
	
}

//-----------------------------------------------------------------------------
//  Read PIO input from DS2488. 
// 
//  Parameters
//     *PIO_Input - a pointer to reading PIO input
//
//  Returns: TRUE - PIO input is read successfully
//           FALSE - Failed to reading PIO input (no presence or invalid CRC16)
//
int DS2488ReadPIO(uchar *PIO_Input)
{
   uchar pbuf[16];
   int cnt, i;
   cnt = 0;
   pbuf[cnt++]=CMD_READ_PIO;
   pbuf[cnt++]=0xff;	//for reading PIO Input
  //for read 2-byte CRC16
   pbuf[cnt++]=0xff;		
   pbuf[cnt++]=0xff;
  //sending data packet
   for(i=0;i<cnt;i++) pbuf[i]=WriteByte(pbuf[i]);
//   PIO_Input[0]=pbuf[1];	//reading configure parameter
   memcpy(PIO_Input, &pbuf[1], 3);  //reading configure parameter with the reading CRC-16 bytes
// run the CRC over this data packet
   CRC16 = 0;
   for(i=0;i<cnt;i++) docrc16(pbuf[i]);
// return result of inverted CRC
   return (CRC16 == 0xB001); //not 0 because that the calculating result is CRC16 and the reading result is inverted CRC16  
	
}

//-----------------------------------------------------------------------------
//  Write timeout value to DS2488. 
// 
//  Parameters
//     *tVAL - a pointer to timeout value
//
//  Returns: TRUE - timeout value written successfully
//           FALSE - Failed to write timeout value (no presence or invalid CRC16)
//
int DS2488WriteTimeOutValue(uchar *tVAL)
{
   uchar pbuf[16];
   int cnt, i;
   cnt = 0;
   pbuf[cnt++]=CMD_WRITE_TIMEOUT;
   pbuf[cnt++]=tVAL[0];
  //for read 2-byte CRC16
   pbuf[cnt++]=0xff;		
   pbuf[cnt++]=0xff;
  //sending data packet
   for(i=0;i<cnt;i++) pbuf[i]=WriteByte(pbuf[i]);
// run the CRC over this data packet
   CRC16 = 0;
   for(i=0;i<cnt;i++) docrc16(pbuf[i]);
// return result of inverted CRC
   return (CRC16 == 0xB001); //not 0 because that the calculating result is CRC16 and the reading result is inverted CRC16  
	
}

//-----------------------------------------------------------------------------
//  Read timeout value from DS2488. 
// 
//  Parameters
//     *tVAL - a pointer to reading timeout value
//
//  Returns: TRUE - timeout value is read successfully
//           FALSE - Failed to reading timeout value (no presence or invalid CRC16)
//
int DS2488ReadTimeOutValue(uchar *tVAL)
{
   uchar pbuf[16];
   int cnt, i;
   cnt = 0;
   pbuf[cnt++]=CMD_READ_TIMEOUT;
   pbuf[cnt++]=0xff;	//for reading timeout value
  //for read 2-byte CRC16
   pbuf[cnt++]=0xff;		
   pbuf[cnt++]=0xff;
  //sending data packet
   for(i=0;i<cnt;i++) pbuf[i]=WriteByte(pbuf[i]);
   tVAL[0]=pbuf[1];	//reading configure parameter 
// run the CRC over this data packet
   CRC16 = 0;
   for(i=0;i<cnt;i++) docrc16(pbuf[i]);
// return result of inverted CRC
   return (CRC16 == 0xB001); //not 0 because that the calculating result is CRC16 and the reading result is inverted CRC16  
	
}



//--------------------------------------------------------------------------
//high level application functions
//--------------------------------------------------------------------------
// The Dectet_Earbuds function detects in-box earbuds through doing a Read-ROM command.  This function
// uses the read-ROM function 33h to read a AND-result ROM number and verify CRC8.
//
// Returns:     (2): family code match and CRC8 error means that two earbuds are possibly in box
//              (1) : OWReset successful and Serial Number placed
//                       in the global ROM, CRC8 valid, only one earbud is in box
//            FALSE (0): OWReset did not have presence
//

int Detect_Earbuds(uchar *rom_id)
{
   int i;
   uchar buf[16];
   if (OneWireReset() == true)
   {
      buf[0] = 0x33; // READ ROM
      memset(&buf[1],0xFF,8);
      for(i=0;i<9;i++) buf[i]=WriteByte(buf[i]);     //9-byte
      if( (buf[1]&0x7f) != DS2488_FAMILY ) return false; //not found DS2488
      memcpy(rom_id,&buf[1],8);
      // verify CRC8
      CRC8 = 0;
      for (i = 0; i < 8; i++)   docrc8(buf[i+1]);
      if( CRC8 == 0 ) return 1;    //only one earbud in box
      else return 2;               //two earbuds possibly in box
   }
   return FALSE;

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
   CRC16  ^= data;
   data <<= 1;
   CRC16   ^= data;

   return CRC16;
}
