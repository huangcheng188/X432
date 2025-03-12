/*******************************************************************************
* Copyright (C) 2016 Maxim Integrated Products, Inc., All rights Reserved.
* * This software is protected by copyright laws of the United States and
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

/** @file serial_win32ex.c
*  @brief Serial communication primitives for Windows 32-bit. 
*       Extra functions added to manipulate RTS and DTR.
*		Extra functions added to read RI, DCD, CTS and DSR.
*		Custom baud rates added for DS2488EVKIT when talking to
*		Prolific PL-2303GC Virtual COM Port or equavalent.
*/

#include <stdio.h>
//#include <windows.h>
#include "1wire_UART.h"
#define SERIAL_SUPPORT
#include "serial_win32ex.h"

// UART connectivity functions (Win32 implementation)
void FlushCOM(void);
int  WriteCOM(int outlen, unsigned char *outbuf);
int  ReadCOM(int inlen, unsigned char *inbuf);
void BreakCOM(void);
void SetBaudCOM(unsigned char new_baud);
int  OpenCOM(char *port_zstr);
void CloseCOM(void);
void DTRCOM(int state);
void RTSCOM(int state);
int DCDCOM(void);
int RICOM(void);
int CTSCOM(void);

//extern int dprintf(char *format, ...);

// Win32 serial globals
//HANDLE ComID;
//OVERLAPPED osRead,osWrite;

// debug mode
int serialdebug=FALSE;

//---------------------------------------------------------------------------
// Attempt to open a com port.  Keep the handle in ComID.
// Set the starting baud rate to 9600. Enable DTR and disable RTS. 
//
// 'port_zstr' - zero terminate port name.  For this platform
//               use format COMX where X is the port number.
//
//
// Returns: TRUE(1)  - success, COM port opened
//          FALSE(0) - failure, could not open specified port
//
int OpenCOM(char *port_zstr)
{
//   char tempstr[80];
//   short fRetVal;
//   COMMTIMEOUTS CommTimeOuts;
//   DCB dcb;

   // debug
//   if (serialdebug)
//      dprintf("__OpenCOM__%s\n",port_zstr);


   // open COMM device
//   if ((ComID =
//      CreateFile( port_zstr, GENERIC_READ | GENERIC_WRITE,
//                  0,
//                  NULL,                 // no security attrs
//                  OPEN_EXISTING,
//                  FILE_FLAG_OVERLAPPED, // overlapped I/O
//                  NULL )) == (HANDLE) -1 )
//   {
//      ComID = 0;
//      return FALSE;
//   }
//   else
//   {
//      // create events for detection of reading and write to com port
//      sprintf(tempstr,"COMM_READ_OVERLAPPED_EVENT_FOR_MAXIM");
//      osRead.hEvent = CreateEvent(NULL,TRUE,FALSE,tempstr);
//      sprintf(tempstr,"COMM_WRITE_OVERLAPPED_EVENT_FOR_MAXIM");
//      osWrite.hEvent = CreateEvent(NULL,TRUE,FALSE,tempstr);

//      // get any early notifications
//      SetCommMask(ComID, EV_RXCHAR | EV_TXEMPTY | EV_ERR | EV_BREAK);

//      // setup device buffers
//      SetupComm(ComID, 2048, 2048);

//      // purge any information in the buffer
//      PurgeComm(ComID, PURGE_TXABORT | PURGE_RXABORT |
//                           PURGE_TXCLEAR | PURGE_RXCLEAR );

//      // set up for overlapped non-blocking I/O
//      CommTimeOuts.ReadIntervalTimeout = 0; 
//      CommTimeOuts.ReadTotalTimeoutMultiplier = 40; 
//      CommTimeOuts.ReadTotalTimeoutConstant = 200; 
//      CommTimeOuts.WriteTotalTimeoutMultiplier = 40; 
//      CommTimeOuts.WriteTotalTimeoutConstant = 200; 
//      SetCommTimeouts(ComID, &CommTimeOuts);

//      // setup the com port
//      GetCommState(ComID, &dcb);

//      dcb.BaudRate = CBR_9600;               // current baud rate
//      dcb.fBinary = TRUE;                    // binary mode, no EOF check
//      dcb.fParity = FALSE;                   // enable parity checking
//      dcb.fOutxCtsFlow = FALSE;              // CTS output flow control
//      dcb.fOutxDsrFlow = FALSE;              // DSR output flow control
//      dcb.fDtrControl = DTR_CONTROL_ENABLE;  // DTR flow control type (default of)
//      dcb.fDsrSensitivity = FALSE;           // DSR sensitivity
//      dcb.fTXContinueOnXoff = TRUE;          // XOFF continues Tx
//      dcb.fOutX = FALSE;                     // XON/XOFF out flow control
//      dcb.fInX = FALSE;                      // XON/XOFF in flow control
//      dcb.fErrorChar = FALSE;                // enable error replacement
//      dcb.fNull = FALSE;                     // enable null stripping
//      dcb.fRtsControl = RTS_CONTROL_DISABLE; // RTS flow control (default off)
//      //?????dcb.fRtsControl = RTS_CONTROL_ENABLE; // RTS flow control (default off)
//      dcb.fAbortOnError = FALSE;             // abort reads/writes on error
//      dcb.XonLim = 0;                        // transmit XON threshold
//      dcb.XoffLim = 0;                       // transmit XOFF threshold
//      dcb.ByteSize = 8;                      // number of bits/byte, 4-8
//      dcb.Parity = NOPARITY;                 // 0-4=no,odd,even,mark,space
//      dcb.StopBits = ONESTOPBIT;             // 0,1,2 = 1, 1.5, 2
//      dcb.XonChar = 0;                       // Tx and Rx XON character
//      dcb.XoffChar = 1;                      // Tx and Rx XOFF character
//      dcb.ErrorChar = 0;                     // error replacement character
//      dcb.EofChar = 0;                       // end of input character
//      dcb.EvtChar = 0;                       // received event character

//      fRetVal = SetCommState(ComID, &dcb);
//   }

   // check if successfull
//   if (!fRetVal)
//   {
//      CloseHandle(ComID);
//      CloseHandle(osRead.hEvent);
//      CloseHandle(osWrite.hEvent);
//      ComID = 0;
//   }

//   return fRetVal;
}

//---------------------------------------------------------------------------
// Closes the connection to the port.
//
void CloseCOM(void)
{
   // disable event notification and wait for thread
   // to halt
//   SetCommMask(ComID, 0);

//   // drop DTR
//   EscapeCommFunction(ComID, CLRDTR);

//   // purge any outstanding reads/writes and close device handle
//   PurgeComm(ComID, PURGE_TXABORT | PURGE_RXABORT |
//                    PURGE_TXCLEAR | PURGE_RXCLEAR );
//   CloseHandle(ComID);
//   CloseHandle(osRead.hEvent);
//   CloseHandle(osWrite.hEvent);
//   ComID = 0;
}

//---------------------------------------------------------------------------
// Flush the rx and tx buffers
//
void FlushCOM(void)
{
   int rslt;

   // purge any information in the buffer
//   rslt = PurgeComm(ComID, PURGE_TXABORT | PURGE_RXABORT |
//                    PURGE_TXCLEAR | PURGE_RXCLEAR );

   // debug
   if (serialdebug)
      dprintf("__Flush__%d\n",rslt);
}

//--------------------------------------------------------------------------
// Write an array of bytes to the COM port, verify that it was
// sent out.  Assume that baud rate has been set.
//
// 'outlen'   - number of bytes to write to COM port
// 'outbuf'   - pointer ot an array of bytes to write
//
// Returns:  TRUE(1)  - success
//           FALSE(0) - failure
//
int WriteCOM(int outlen, unsigned char *outbuf)
{
   BOOL fWriteStat;
   DWORD dwBytesWritten=0;
   DWORD ler=0,to,rslt=123;
   int i;

   // debug
   if (serialdebug)
   {
      for (i = 0; i < outlen; i++)
         dprintf(">%02X",outbuf[i]);
      dprintf("\n");
   }

   // calculate a timeout
   to = 40 * outlen + 205;

   // reset the write event
//   ResetEvent(osWrite.hEvent);

   // write the byte
//   fWriteStat = WriteFile(ComID, (LPSTR) &outbuf[0],
//                outlen, &dwBytesWritten, &osWrite );

   // check for an error
//   if (!fWriteStat)
//      ler = GetLastError();

   // if not done writting then wait
//   if (!fWriteStat && ler == ERROR_IO_PENDING)
//   {
//      WaitForSingleObject(osWrite.hEvent,to);

//      // verify all is written correctly
//      fWriteStat = GetOverlappedResult(ComID, &osWrite,
//                   &dwBytesWritten, FALSE);
//   }

   // check results of write
   if (!fWriteStat || (dwBytesWritten != (DWORD)outlen))
      return 0;
   else
      return 1;
}

//--------------------------------------------------------------------------
// Read an array of bytes to the COM port, verify that it was
// sent out.  Assume that baud rate has been set.
//
// 'inlen'     - number of bytes to read from COM port
// 'inbuf'     - pointer to a buffer to hold the incomming bytes
//
// Returns: number of characters read
//
int ReadCOM(int inlen, unsigned char *inbuf)
{
   DWORD dwLength=0;
   BOOL fReadStat;
   DWORD ler=0,to,i;
//   COMSTAT ComStat;
   DWORD dwErrorFlags;

   // calculate a timeout
   to = 40 * inlen + 205;

   // reset the read event
//   ResetEvent(osRead.hEvent);

   // read
//   fReadStat = ReadFile(ComID, (LPSTR) &inbuf[0],
//                      inlen, &dwLength, &osRead);

   // check for an error
//   if (!fReadStat)
//      ler = GetLastError();

   // if not done reading then wait
//   if (!fReadStat && ler == ERROR_IO_PENDING)
//   {
//      // wait until everything is read
//      WaitForSingleObject(osRead.hEvent,to);

//      // verify all is read correctly
//      fReadStat = GetOverlappedResult(ComID, &osRead,
//                   &dwLength, FALSE);
//   }

   // check results
   if (fReadStat)
   {
      // check if any more bytes in buffer 
//      ClearCommError(ComID, &dwErrorFlags, &ComStat);

      // debug
      if (serialdebug)
      {
         dprintf("   ");
         if (dwLength == 0)
            dprintf("(%d){",ler);
         for (i = 0; i < dwLength; i++)
            dprintf("<%02X",inbuf[i]);
         dprintf("\n");
//         if (ComStat.cbInQue > 0)
//            dprintf("MORE BYTES %d\n",ComStat.cbInQue);
      }

      return dwLength;
   }
   else
   {
//      if (serialdebug)
//            dprintf("   (%d,%d)[\n",ler,GetLastError());

      return 0;
   }
}

//--------------------------------------------------------------------------
// Send a break on the com port for at least 2 ms
//
void BreakCOM(void)
{
   // start the reset pulse
//   SetCommBreak(ComID);

   // sleep
//   Sleep(2);

   // clear the break
//   ClearCommBreak(ComID);

   // debug
   if (serialdebug)
      dprintf("__Break__\n");
}

//--------------------------------------------------------------------------
// Set the baud rate on the com port.
//
// 'new_baud'  - new baud rate defined as
// PARMSET_9600             0x00
// PARMSET_19200            0x02
// PARMSET_57600            0x04
// PARMSET_115200           0x06
// PARMSET_122400			0x07
// PARMSET_806400		    0x15
// PARMSET_1228800		    0x17
//
void SetBaudCOM(unsigned char new_baud)
{
//   DCB dcb;

   // get the current com port state
//   GetCommState(ComID, &dcb);

   // change just the baud rate
   switch (new_baud)
   {
      case PARMSET_115200:
//         dcb.BaudRate = CBR_115200;
         break;
	  case PARMSET_122400:
//		  dcb.BaudRate = 122400;	// Needed for DS2488 tRSTL; IOA only powered
		  break;
      case PARMSET_57600:
//         dcb.BaudRate = CBR_57600;
         break;
      case PARMSET_19200:
//         dcb.BaudRate = CBR_19200;
         break;
	  case PARMSET_806400:
//		  dcb.BaudRate = 806400;	// Used for DS2488 tW0L
		  break;
//	  case PARMSET_1228800:
//		  dcb.BaudRate = 1228800;	// Used for DS2488 tRL/tW1L
		  break;
//      case PARMSET_9600:
      default:
//         dcb.BaudRate = CBR_9600;
         break;
   }

   // restore to set the new baud rate
//   SetCommState(ComID, &dcb);

   // debug
//   if (serialdebug)
//      dprintf("__SetBaudCOM %d__\n",dcb.BaudRate);
}

//--------------------------------------------------------------------------
// Set the active low DTR state
//
// 'state':		TRUE (1)	: output logic low
//				FALSE (0)	: output logic high
//
void DTRCOM(int state)
{
//   DCB dcb;

//   dcb.DCBlength = sizeof(DCB);
//   GetCommState(ComID, &dcb);

//   if (state)
//      dcb.fDtrControl = DTR_CONTROL_ENABLE;
//   else
//      dcb.fDtrControl = DTR_CONTROL_DISABLE;

//   SetCommState(ComID, &dcb);

   // debug
   if (serialdebug)
      dprintf("__DTR %d__\n",state);
}

//--------------------------------------------------------------------------
// Set the active low RTS state
//
// 'state':		TRUE (1)	: output logic low
//				FALSE (0)	: output logic high
//
void RTSCOM(int state)
{
//   DCB dcb;

//   dcb.DCBlength = sizeof(DCB);
//   GetCommState(ComID, &dcb);

//   if (state)
//      dcb.fRtsControl = RTS_CONTROL_ENABLE;
//   else
//      dcb.fRtsControl = RTS_CONTROL_DISABLE;

//   
//   SetCommState(ComID, &dcb);

   // debug
   if (serialdebug)
      dprintf("__RTS %d__\n",state);
}

//--------------------------------------------------------------------------
// Read the DCD status
//
// Returns:		TRUE (1)	: when logic low
//				FALSE (0)	: when logic high
//
int DCDCOM(void)
{
	DWORD dwModemStatus;
	BOOL fDCD;

//	if (!GetCommModemStatus(ComID, &dwModemStatus))
//		// Error in GetCommModemStatus
//		return;

//	return fDCD = MS_RLSD_ON & dwModemStatus;
}

//--------------------------------------------------------------------------
// Read the active low RI status
//
// Returns:		TRUE (1)	: when logic low
//				FALSE (0)	: when logic high
//
int RICOM(void)
{
	DWORD dwModemStatus;
	BOOL fRI;

//	if (!GetCommModemStatus(ComID, &dwModemStatus))
//		// Error in GetCommModemStatus
//		return;

//	return fRI = MS_RING_ON & dwModemStatus;
}

//--------------------------------------------------------------------------
// Read the active low CTS status
//
// Returns:		TRUE (1)	: when logic low
//				FALSE (0)	: when logic high
//
int CTSCOM(void)
{
	DWORD dwModemStatus;
	BOOL fCTS;

//	if (!GetCommModemStatus(ComID, &dwModemStatus))
//		// Error in GetCommModemStatus
//		return;

//	return fCTS = MS_CTS_ON & dwModemStatus;
}

//--------------------------------------------------------------------------
// Read the active low DSR status
//	
// Returns:		TRUE (1)	: when logic low
//				FALSE (0)	: when logic high
//
int DSRCOM(void)
{
	DWORD dwModemStatus;
	BOOL fDSR;

//	if (!GetCommModemStatus(ComID, &dwModemStatus))
//		// Error in GetCommModemStatus
//		return;

//	return fDSR = MS_DSR_ON & dwModemStatus;
}

