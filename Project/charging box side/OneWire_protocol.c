#include "DS2488.h"
#include "OneWire_protocol.h"

#ifdef TMEX
unsigned char WriteCOM(int outlen, uchar *outbuf);
int ReadCOM(int inlen, uchar *inbuf);
void FlushCOM();
void SetBaudCOM(uchar new_baud);
void DTRCOM(int state);
void RTSCOM(int state);

unsigned char  SetOverDrive(void);
short  FindFirstFamily(short);
short   owNext(short , short );
unsigned char  OpenExtensionUART(void);
void CloseUART(void);
short   owNext(short, short);
void owFamilySearchSetup(short );
uchar  docrc8(uchar );
short TMBlockStream(long,unsigned char *,short);

//Find the extension COM port of CP2102 and open this COM port to control the programming pulse
//return true if success, or false if fail
unsigned char  OpenExtensionUART(void)
{
#if 0
//   unsigned char type[4]={1,6,5,2};    //1-Wire adapter type,5-DS9097U, 1-DS9097E, 6-DS9490R
   unsigned char result=FALSE;
   char Com_port[30]="\\\\.\\COM3";
   unsigned long ModemState;
   short i,j;
   DCB dcb1;
   COMMTIMEOUTS CommTimeOuts;
   char tempstr[80];
   short fRetVal;

   DWORD dwDesiredAccess;

//open the found extension COM port number=i;
   for(i=3;i<40;i++)
   {
      sprintf(Com_port, "\\\\.\\COM%d",i);
      Sleep(50);     //wait for 50ms
      if ((ComID =
      CreateFile( Com_port, GENERIC_READ | GENERIC_WRITE,
                 FILE_SHARE_READ | FILE_SHARE_WRITE ,
                  NULL,                 // no security attrs
                  OPEN_EXISTING,
                  NULL, // FILE_FLAG_OVERLAPPED when using overlapped I/O
                  NULL )) == (HANDLE) -1 )
   {
      ComID = 0;
 //     OWERROR(OWERROR_GET_SYSTEM_RESOURCE_FAILED);

//      return false;
   }
   else
   {
/*
      // create events for detection of reading and write to com port
      sprintf(tempstr,"COMM_READ_OVERLAPPED_EVENT_FOR_%s",Com_port);
      osRead.hEvent = CreateEvent(NULL,TRUE,FALSE,tempstr);


      sprintf(tempstr,"COMM_WRITE_OVERLAPPED_EVENT_FOR_%s",Com_port);
      osWrite.hEvent = CreateEvent(NULL,TRUE,FALSE,tempstr);           */

      // get any early notifications
      SetCommMask(ComID, EV_RXCHAR | EV_TXEMPTY | EV_ERR | EV_BREAK);

      // setup device buffers
      SetupComm(ComID, 512, 512);

      // purge any information in the buffer
      PurgeComm(ComID, PURGE_TXABORT | PURGE_RXABORT |
                           PURGE_TXCLEAR | PURGE_RXCLEAR );

      // set up for overlapped non-blocking I/O
      CommTimeOuts.ReadIntervalTimeout = 0;
      CommTimeOuts.ReadTotalTimeoutMultiplier = 20;
      CommTimeOuts.ReadTotalTimeoutConstant = 40;
      CommTimeOuts.WriteTotalTimeoutMultiplier = 20;
      CommTimeOuts.WriteTotalTimeoutConstant = 40;
      SetCommTimeouts(ComID, &CommTimeOuts);

          // setup the com port

      GetCommState(ComID, &dcb1);      //reserve original setup
// setup the com port
      GetCommState(ComID, &dcb);

      dcb.BaudRate = CBR_9600;               // current baud rate
      dcb.fBinary = TRUE;                    // binary mode, no EOF check
      dcb.fParity = FALSE;                   // enable parity checking
      dcb.fOutxCtsFlow = FALSE;              // CTS output flow control
      dcb.fOutxDsrFlow = FALSE;              // DSR output flow control
      dcb.fDtrControl = DTR_CONTROL_DISABLE;  //DTR_CONTROL_ENABLE;  // DTR flow control type
      dcb.fDsrSensitivity = FALSE;           // DSR sensitivity
      dcb.fTXContinueOnXoff = TRUE;          // XOFF continues Tx
      dcb.fOutX = FALSE;                     // XON/XOFF out flow control
      dcb.fInX = FALSE;                      // XON/XOFF in flow control
      dcb.fErrorChar = FALSE;                // enable error replacement
      dcb.fNull = FALSE;                     // enable null stripping
      dcb.fRtsControl = RTS_CONTROL_DISABLE;     //RTS_CONTROL_ENABLE;  // RTS flow control
      dcb.fAbortOnError = FALSE;             // abort reads/writes on error
      dcb.XonLim = 0;                        // transmit XON threshold
      dcb.XoffLim = 0;                       // transmit XOFF threshold
      dcb.ByteSize = 8;                      // number of bits/byte, 4-8
      dcb.Parity = NOPARITY;                 // 0-4=no,odd,even,mark,space
      dcb.StopBits = ONESTOPBIT;             // 0,1,2 = 1, 1.5, 2
      dcb.XonChar = 0;                       // Tx and Rx XON character
      dcb.XoffChar = 1;                      // Tx and Rx XOFF character
      dcb.ErrorChar = 0;                     // error replacement character
      dcb.EofChar = 0;                       // end of input character
      dcb.EvtChar = 0;                       // received event character

      fRetVal =SetCommState(ComID, &dcb);

   // check if successfull
   if (!fRetVal)
   {
      CloseHandle(ComID);
//      CloseHandle(osRead.hEvent);
//      CloseHandle(osWrite.hEvent);
      ComID = 0;
//      OWERROR(OWERROR_SYSTEM_RESOURCE_INIT_FAILED);
      continue;
   }
//      EscapeCommFunction(ComID, CLRDTR);   //Set the Line voltage to 7.0V
      DTRCOM(0);   //Set the IOA to logic high
      Sleep(4);
//check if this com port is extended by CP2102
      RTSCOM(1);
      Sleep(4);
      GetCommModemStatus(ComID, &ModemState);
      if((ModemState&MS_RLSD_ON)==0 )        
      {
         SetCommState(ComID, &dcb1);
         CloseHandle(ComID);
         continue;
      }
      else
      {
        SetBaudCOM(PARMSET_115200);   //BAUDRATE=115.2kbps
        Sleep(3);   //wait 2ms to setup
        FlushCOM();
        tempstr[0]=0x55;
        WriteCOM(1,tempstr);          
        ReadCOM(1,&tempstr[1]);
        if( tempstr[0]!=tempstr[1] )  //check if COM port in loop connection
        {
          SetCommState(ComID, &dcb1);
          CloseHandle(ComID);
          continue;
        }
        break;
      }

     }
   }
     if(i!=40 )  return i;               //return true;
     else return false;
#endif
}

//---------------------------------------------------------------------------
// Closes the connection to the port.
//
//
void CloseUART(void)
{
#if 0
   // disable event notification and wait for thread
   // to halt
   SetCommMask(ComID, 0);

   // drop DTR
   EscapeCommFunction(ComID, CLRDTR);

   // purge any outstanding reads/writes and close device handle
   PurgeComm(ComID, PURGE_TXABORT | PURGE_RXABORT |
                    PURGE_TXCLEAR | PURGE_RXCLEAR );
   CloseHandle(ComID);
//   CloseHandle(osRead.hEvent);
//   CloseHandle(osWrite.hEvent);
   ComID = 0;
#endif
}
//---------------------------------------------------------------------------
// Flush the rx and tx buffers
//
//
void FlushCOM()
{
#if 0
   // purge any information in the buffer
   PurgeComm(ComID, PURGE_TXABORT | PURGE_RXABORT |
                    PURGE_TXCLEAR | PURGE_RXCLEAR );
#endif
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
unsigned char WriteCOM(int outlen, uchar *outbuf)
{
#if 0
   BOOL fWriteStat;
   DWORD dwBytesWritten=0;
   DWORD ler=0,to;

   // calculate a timeout
   to = 20 * outlen + 60;

   // reset the write event
//   ResetEvent(osWrite.hEvent);
//   ResetEvent();

   // write the byte
//   fWriteStat = WriteFile(ComID, (LPSTR) &outbuf[0], outlen, &dwBytesWritten, &osWrite );
   fWriteStat = WriteFile(ComID, (LPSTR) &outbuf[0], outlen, &dwBytesWritten, NULL );

   // check for an error
   if (!fWriteStat)
      ler = GetLastError();

/*
   // if not done writting then wait
   if (!fWriteStat && ler == ERROR_IO_PENDING)
   {
      WaitForSingleObject(osWrite.hEvent,to);

      // verify all is written correctly
      fWriteStat = GetOverlappedResult(ComID, &osWrite,
                   &dwBytesWritten, FALSE);

   }                               */

   // check results of write

   if (!fWriteStat  || (dwBytesWritten != (DWORD)outlen))
      return 0;
   else
      return 1;
#endif
}

//--------------------------------------------------------------------------
// Read an array of bytes to the COM port, verify that it was
// sent out.  Assume that baud rate has been set.
//
/// 'inlen'     - number of bytes to read from COM port
// 'inbuf'     - pointer to a buffer to hold the incomming bytes
//
// Returns: number of characters read
//
int ReadCOM(int inlen, uchar *inbuf)
{
#if 0
   DWORD dwLength=0;
   BOOL fReadStat;
   DWORD ler=0,to;

   // calculate a timeout
   to = 20 * inlen + 60;

   // reset the read event
//   ResetEvent(osRead.hEvent);

   // read
   fReadStat = ReadFile(ComID, (LPSTR) &inbuf[0],
                      inlen, &dwLength, NULL) ;

   // check for an error
   if (!fReadStat)
      ler = GetLastError();
/*
   // if not done writing then wait
   if (!fReadStat && ler == ERROR_IO_PENDING)
   {
      // wait until everything is read
      WaitForSingleObject(osRead.hEvent,to);

      // verify all is read correctly
      fReadStat = GetOverlappedResult(ComID, &osRead,
                   &dwLength, FALSE);
   }                       */

   // check results
   if (fReadStat)
      return dwLength;
   else
      return 0;
#endif
}

//--------------------------------------------------------------------------
// Send a break on the com port for at least 2 ms
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
//
void BreakCOM(void)
{
#if 0
   // start the reset pulse
   SetCommBreak(ComID);

   // sleep
   Sleep(2);

   // clear the break
   ClearCommBreak(ComID);
#endif
}
//--------------------------------------------------------------------------
// Set the active low DTR state
//
// 'state':		TRUE (1)	: output logic low
//				FALSE (0)	: output logic high
//
void DTRCOM(int state)
{
#if 0
   DCB dcb;

   dcb.DCBlength = sizeof(DCB);
   GetCommState(ComID, &dcb);

   if (state)
      dcb.fDtrControl = DTR_CONTROL_ENABLE;
   else
      dcb.fDtrControl = DTR_CONTROL_DISABLE;

   SetCommState(ComID, &dcb);
#endif

}

//--------------------------------------------------------------------------
// Set the active low RTS state
//
// 'state':		TRUE (1)	: output logic low
//				FALSE (0)	: output logic high
//
void RTSCOM(int state)
{
#if 0
   DCB dcb;

   dcb.DCBlength = sizeof(DCB);
   GetCommState(ComID, &dcb);

   if (state)
      dcb.fRtsControl = RTS_CONTROL_ENABLE;
   else
      dcb.fRtsControl = RTS_CONTROL_DISABLE;

   
   SetCommState(ComID, &dcb);
#endif
}
//--------------------------------------------------------------------------
// Set the baud rate on the com port.
//
// 'new_baud'  - new baud rate defined as
//                PARMSET_1200     0x00
//                PARMSET_2400     0x01
//                PARMSET_4800     0x02
//                PARMSET_9600     0x03
//                PARMSET_14400    0x04
//                PARMSET_19200    0x05
//                PARMSET_57600    0x06
//                PARMSET_115200   0x07
//                PARMSET_256000   0x08
//                PARMSET_153600   0x09
//                PARMSET_256000   0x0a
//                PARMSET_460800   0x0b
//                PARMSET_576000   0x0c

void SetBaudCOM(uchar new_baud)
{
#if 0
   DCB dcb1;

   // get the current com port state
   GetCommState(ComID, &dcb1);

   // change just the baud rate
   switch (new_baud)
   {
      case PARMSET_800000:
         dcb1.BaudRate = 800000;
         break;	   
      case PARMSET_153600:
         dcb1.BaudRate = 153600;
         break;
      case PARMSET_512000:
         dcb1.BaudRate = 512000;
         break;

      case PARMSET_460800:
         dcb1.BaudRate = 460800;
         break;
      case PARMSET_256000:
         dcb1.BaudRate = CBR_256000;
         break;
      case PARMSET_115200:
         dcb1.BaudRate = CBR_115200;
         break;
      case PARMSET_57600:
         dcb1.BaudRate = CBR_57600;
         break;
      case PARMSET_38400:
         dcb1.BaudRate = CBR_38400;
         break;
      case PARMSET_19200:
         dcb1.BaudRate = CBR_19200;
         break;
      case PARMSET_14400:
         dcb1.BaudRate = CBR_14400;
         break;
      case PARMSET_1200:
         dcb1.BaudRate = CBR_1200;
         break;
      case PARMSET_2400:
         dcb1.BaudRate = CBR_2400;
         break;
      case PARMSET_4800:
         dcb1.BaudRate = CBR_4800;
         break;
      case PARMSET_9600:
      default:
         dcb1.BaudRate = CBR_9600;
         break;
   }

   // restore to set the new baud rate
   SetCommState(ComID, &dcb1);
#endif 
}


//--------------------------------------------------------------------------
// 1-wire communication Reset and detect the presence pulse
//
unsigned char  OneWireReset(void)
{

   unsigned char TxBuffer,RxBuffer;
/*   if(OneWireRate==NormalMode)
   {
	 TxBuffer=0xF0;  
     SetBaudCOM(PARMSET_9600);
     Sleep(3);   //wait 3ms to setup
     FlushCOM();
     WriteCOM(1,&TxBuffer);
     ReadCOM(1,&RxBuffer);
     FlushCOM();

     SetBaudCOM(PARMSET_153600);   // normal 1-wire speed
     Sleep(3);   //wait 2ms to setup
     FlushCOM();
   }
   else		*/
   {
	 TxBuffer=0xE0;
     SetBaudCOM(PARMSET_115200);
     Sleep(3);   //wait 3ms to setup
     FlushCOM();
     WriteCOM(1,&TxBuffer);
     ReadCOM(1,&RxBuffer);
     FlushCOM();

     SetBaudCOM(PARMSET_800000);   //BAUDRATE=800kbps, 1 bit slot=10*1.25us=12.5us
     Sleep(3);   //wait 2ms to setup
     FlushCOM();

   }

   if( RxBuffer!=TxBuffer) return true;
   else return false;
}

//--------------------------------------------------------------------------
// Write bit to one-wire interface
//
unsigned char  WriteBit(unsigned char bit_value)
{
   unsigned char TxBuffer=ONE_BIT,RxBuffer;
   if( bit_value==0) TxBuffer=ZERO_BIT;
   WriteCOM(1,&TxBuffer);
   ReadCOM(1,&RxBuffer);
   if( RxBuffer==ONE_BIT ) return true;  //return '1'
   else return false;		      //return '0'

}
//--------------------------------------------------------------------------
// Read bit from one wire interface
//
unsigned char  ReadBit(void)
{
   unsigned char TxBuffer=ONE_BIT,RxBuffer;
   WriteCOM(1,&TxBuffer);
   ReadCOM(1,&RxBuffer);
   if( RxBuffer==ONE_BIT ) return true;  //return '1'
   else return false;		      //return '0'
}
//--------------------------------------------------------------------------
// Write byte to one wire interface
//
unsigned char  WriteByte(unsigned char Byte_Value)
{
   unsigned char TxBuffer[8],RxBuffer[8],ReadByte=0;
   short i;
   unsigned char temp[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
   for(i=0;i<8;i++)
   {
      if( Byte_Value&temp[i] ) TxBuffer[i]=ONE_BIT;
      else TxBuffer[i]=ZERO_BIT;
   }
   WriteCOM(8,TxBuffer);
   ReadCOM(8,RxBuffer);
   for(i=0;i<8;i++) if( RxBuffer[i]==ONE_BIT ) ReadByte|=temp[i];
   return ReadByte;
}
//--------------------------------------------------------------------------
// Read byte from one wire interface
//
unsigned char  ReadByte(void)
{
   unsigned char TxBuffer[8],RxBuffer[8],ReadByte=0;
   short i;
   unsigned char temp[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
   for(i=0;i<8;i++) TxBuffer[i]=ONE_BIT;
   WriteCOM(8,TxBuffer);
   ReadCOM(8,RxBuffer);
   for(i=0;i<8;i++) if( RxBuffer[i]==ONE_BIT ) ReadByte|=temp[i];
   return ReadByte;
}
//--------------------------------------------------------------------------
// 1-wire communication Reset and detect the presence pulse
//
unsigned char  SetOverDrive(void)
{
/*   OneWireRate=NormalMode;
   if( (OneWireReset())==false ) return false;
   WriteByte(OVERDRIVE_SKIP_ROM);    */
   OneWireRate=OverDrive;
   return true;
}

//--------------------------------------------------------------------------
// The 'OWReadROM' function does a Read-ROM.  This function
// uses the read-ROM function 33h to read a ROM number and verify CRC8.
//
// Returns:   TRUE (1) : OWReset successful and Serial Number placed
//                       in the global ROM, CRC8 valid
//            FALSE (0): OWReset did not have presence or CRC8 invalid
//
int owReadROM(void)
{
   uchar buf[16];
   int i;

   if (OneWireReset() == true)
   {
      buf[0] = 0x33; // READ ROM
      memset(&buf[1],0xFF,8);
      for(i=0;i<9;i++) buf[i]=WriteByte(buf[i]);     //9-byte
      // verify CRC8
      CRC8 = 0;
      for (i = 0; i < 8; i++)
         docrc8(buf[i+1]);

      if ((CRC8 == 0) && (buf[1] != 0))
      {
         memcpy(ROM_NO,&buf[1],8);
         return TRUE;
      }
   }
   return FALSE;
}


//--------------------------------------------------------------------------
// The 'OWSkipROM' function does a skip-ROM.  This function
// uses the Skip-ROM function CCh.
//
// Returns:   TRUE (1) : OWReset successful and skip rom sent.
//            FALSE (0): OWReset did not have presence
//
int owSkipROM(void)
{
   if (OneWireReset() == 1)
   {
      WriteByte(0xCC);   //skip ROM command
      return TRUE;
   }
   
   return FALSE;
}


/*------------------------------------------------------------------------
 * Match the 64-bit ROMID
 */
short  owMatchROM(uchar *buf)
{
   short i;

   if ( (OneWireReset())==false ) return false;
   if(OneWireRate==NormalMode) WriteByte(MATCH_ROM);   //send Match ROM command
   else WriteByte(OVERDRIVE_MATCH_ROM);
    /* send out 64-bit ROMID */
   for(i=0;i<8;i++) WriteByte(buf[i]);
   return true;
}

//--------------------------------------------------------------------------
// The 'OWReadROM' function does a Read-ROM.  This function
// uses the read-ROM function 33h to read a ROM number and verify CRC8.
//
// Returns:   (number) : found device number
//            (0): no device found
//
int owSearchROM(uchar Search_Family_Code, uchar *ROMID_Buffer)
{
   uchar buf[16],first_read_bit,second_read_bit,ROM_Conflict_flag=false;
   uchar DeviceFoundNumber=0;
   int i,j;
   unsigned char temp[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

   for(j=0;j<2;j++)      //searching device number is 2
   {
     if( j==1 && ROM_Conflict_flag==false ) return DeviceFoundNumber;
     memset(buf, 0x00, 8);
     if (OneWireReset() == true)
     {
        WriteByte(0xF0); // search ROM command
        for(i=0;i<64;i++)
        {
           first_read_bit=ReadBit();
           second_read_bit=ReadBit();
           if( first_read_bit == second_read_bit )
           {
              if( first_read_bit==1 ) return DeviceFoundNumber;
              ROM_Conflict_flag=true;
              WriteBit(j);   //first round conflict bit is selected to '0',  second round conflict bit is selected to '1'
              first_read_bit=j;
           }
           else  WriteBit(first_read_bit);
           if( first_read_bit==1 ) buf[i/8]|=temp[i%8];
        }
      // verify CRC8
        CRC8 = 0;
        for (i = 0; i < 8; i++)  docrc8(buf[i]);
        if ((CRC8 == 0) && ( (buf[0]&0x7f) == Search_Family_Code) )
        {
           memcpy(&ROMID_Buffer[j*8],buf,8);
           DeviceFoundNumber++;
        }
        else return  DeviceFoundNumber;
       }
       else  return DeviceFoundNumber;
     }
     return DeviceFoundNumber;
}



//--------------------------------------------------------------------------

// read or write a group of data from/to the 1-wire device
//
/*
short TMBlockStream(long SHandle,unsigned char *pbuf,short cnt)
{
   short i,j,k,m;
   unsigned char TxBuffer[256],RxBuffer[256],ReadByte=0;
   unsigned char temp[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

   for(i=0;i<1+cnt/32;i++)
   {
       for(j=0;(j<cnt-i*32)&&j<32;j++)
       {
         for(k=0;k<8;k++)
         {
             if( pbuf[i*32+j]&temp[k] ) TxBuffer[j*8+k]=ONE_BIT;
             else TxBuffer[j*8+k]=ZERO_BIT;
         }
       }
       WriteCOM(j*8,TxBuffer);
       ReadCOM(j*8,RxBuffer);
       for(m=0;m<j;m++)
       {
         ReadByte=0;
         for(k=0;k<8;k++) if( RxBuffer[m*8+k]==0xff ) ReadByte|=temp[k];
         pbuf[i*32+m]=ReadByte;
       }

   }
   return cnt;
}                     */

/*--------------------------------------------------------------------------
 * Update the Dallas Semiconductor One Wire CRC (CRC8) from the global
 * variable CRC8 and the argument.  Return the updated CRC8.
 */
uchar dscrc_table[] = {
        0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
      157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
       35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
      190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
       70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
      219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
      101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
      248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
      140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
       17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
      175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
       50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
      202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
       87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
      233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
      116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};
//--------
uchar  docrc8(uchar x)
{
   CRC8 = dscrc_table[CRC8 ^ x];
   return CRC8;
}





//---------------------------------


#else
extern unsigned char WriteCOM(int outlen, uchar *outbuf);
extern int ReadCOM(int inlen, uchar *inbuf);
extern void FlushCOM();
extern void SetBaudCOM(uchar new_baud);
extern void DTRCOM(int state);
extern void RTSCOM(int state);

extern unsigned char _fastcall SetOverDrive(void);
extern unsigned char _fastcall OpenExtensionUART(void);
extern  _fastcall short ow_Next(short , short );
extern void CloseUART(void);
extern unsigned char _fastcall dowcrc8(unsigned char);
//extern short TMBlockStream(long,unsigned char *,short);

#endif
