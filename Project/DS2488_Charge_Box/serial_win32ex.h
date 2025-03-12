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

/** @file serial_win32.h
*   @brief Include file for serial_win32 module. 
*/

// Baud rate bits
#define PARMSET_9600                0x00
#define PARMSET_19200               0x02
#define PARMSET_57600               0x04
#define PARMSET_115200              0x06
#define PARMSET_122400				0x07
#define PARMSET_806400				0x15
#define PARMSET_1228800				0x17



#ifndef SERIAL_SUPPORT
   extern void FlushCOM(void);
   extern int  WriteCOM(int outlen, unsigned char *outbuf);
   extern int  ReadCOM(int inlen, unsigned char *inbuf);
   extern void BreakCOM(void);
   extern void SetBaudCOM(unsigned char new_baud);
   extern int  OpenCOM(char *port_zstr);
   extern void CloseCOM(void);
   extern void CloseCOM(void);
   extern void msDelay(int len);
   extern void DTRCOM(int state);
   extern void RTSCOM(int state);
   extern int DCDCOM(void);
   extern int RICOM(void);
   extern int CTSCOM(void);
   extern int DSRCOM(void);
#endif


