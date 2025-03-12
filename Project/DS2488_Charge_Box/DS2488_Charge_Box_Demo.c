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

/*
DS2488_Charge_Box_demo:
	Demonstration program using the DS9401 UART to 1-Wire charge box emulator
	to communicate with the DS2488EVKIT Earbud emulator.

Program Summary:

	Connect to DS9401 by providing COM# (e.g. COM1-9 or \\.\COM10-256).
	Initialize Charging Box connected to DS2488.
	Enter While loop that can be exited by any key stroke.
	Check if Earbuds in charging box or not.
		Discover both ROM IDs and populate list if not done already.
		See if Earbuds have power or not and charge if dead.
		Set Right/Left ROMID if both earbuds alive.
		Simulate a Write Charger Box Battery Level to Earbuds with small msg.
		Provide a normal charge for a time.
		Receive small msg from earbud of its battery level.

Notes:	1) DS9401 is set to 3.3V 1-Wire by jumper JB1 populated.
		2) DS2488EVKIT requires PIOA' grounded on P2 with
		a jumper (pin 9-11) for Left/Right earbud detection.
		3) DS2488EVKIT JB1,2,3,4,5 & 6 are populated when in the DS9401.
		4) DS2488EVKIT JB1,4,5 & 6 are populated when not in the DS9401.
		(i.e. JB2 & JB3 are to not be populated when not in the box to
		to emulate earbuds are seperate for DS2488 Earbud program.)
		5) Computer virtual COM parts are slow so this demo reacts slowly and
		delays are set very long to accommodate ~11ms between bits.
		6) Use device manager to determine which COM port is DS9401 (Charge Box)
		and DS2488EVKIT (Earbuds).
		7) Recommend starting DS2488 Charge Box demo first and then
		DS2488 Earbud demo.  Both demos are to run together for interaction.
	  
*/



#define Msg_Battery_Box				0x11
#define Msg_Left_Battery_Bud		0x22
#define Msg_Right_Battery_Bud		0x33
#define Msg_Ready_PTM				0x44

#include <stdio.h>
//#include <conio.h>
//#include <windows.h>
#include <time.h>
#include "serial_win32ex.h"
#include "1wire_UART.h"
#include "DS2488.h"

void msDelay(int len);

//int dprintf(char *format, ...);

//DS2488 Charge Box Functions
int DS2488_Discovery(uchar *romid_list);
void SetChargeState(void);
int CheckEarbudPower(int cnt);
int SetLeftRightEarbudROM_ID(void);
void SetOneWireStateIOA(void);
int read_from_BUFB(uchar* BUF);

uchar LEFT_ROM_NO[8], RIGHT_ROM_NO[8];
uchar ROM_ID_LIST[16];
uchar BUF[8];

FILE *DFile;

//--------------------------------------------------------------------------
// Device: DS2488
// IOA 1-Wire Master:  DS9401 (Emulating a Charging Box)

void ds2488_main(int argc, char **argv)
{
	time_t tlong;
	struct tm *tstruct;


	// open log file and print time stamp
//	DFile = fopen("log.txt", "a+");
//	if (DFile == NULL)
//	{
//		dprintf("ERROR, Could not open LOT.TXT log file!\n");
//		exit(1);
//	}

	// print time
//	time(&tlong);
//	tstruct = localtime(&tlong);
//	dprintf("\n#TIMESTAMP: %02d/%02d/%04d %02d:%02d:%02d \n",
//		tstruct->tm_mon + 1, tstruct->tm_mday, tstruct->tm_year + 1900,
//		tstruct->tm_hour, tstruct->tm_min, tstruct->tm_sec);

	dprintf("------------------------------------------------\n");
	dprintf("DS2488 Charging Box Demo\n");
	dprintf("------------------------------------------------\n\n");

	// check arguments to see if incorrect number 
	if (argc != 2)
	{
		dprintf("\nusage: CHARGING BOX PORT INPUT NUM\r\n"
			"  PORT - argument 1 specifies the COM port "
			"(e.g. COM1 to COM9 or \\.\COM10 for COM10 and higher)\r\n"
			"  version 1.01\r\n");
//		exit(0);
	}

	// setup the port
//	if (OpenCOM(argv[1]) != TRUE)
//	{
//		dprintf("Failed to open serial port %s\n", argv[1]);
//		dprintf("\nPress any key to end demo...");
//		_getch();
//		exit(0);
//	}

	dprintf("DS2488 connect and loop by pressing anykey!\n");
	dprintf("*Note: While in loop exit by pressing anykey!\n");

	dprintf("\n***Initializing Charging Box***\n");
	dprintf("Turning 5V charging power on to charge up parasite capacitor\n");
	dprintf("and 3ms delay for POR to complete of the DS2488 devices.\n");
	RTSCOM(0);		// Set RTS_N logic high to make sure 5V Charging Power is on
	msDelay(3);		// Delay 2ms to charge up parasite capacitor and time for POR to complete of the 1-Wire slave device
	dprintf("Setting 1-Wire speed to Overdrive.\n");
	OWSpeed(MODE_OVERDRIVE);
//	srand(time(NULL));   // Initialization, should only be called once.
	int Discovery_cnt = { 0 };
	int Left_Right_Earbud_ROMID_FLAG = { 0 };

	while (1/*!kbhit()*/)	// Hit anykey to exit loop
	{
		uchar status_byte = { 0x00 };
		int earbud_batt_FLAG = { 0 };
		
		// Go to 1-Wire IOA Operation State after being in 5V state
		SetOneWireStateIOA();

		// See if Earbuds in the charge box or not
		if (OWReset())
		{
			dprintf("\n***Earbud(s) In Box***\n");

			// Discover both ROM IDs and populate list if not done already
			if (!(Discovery_cnt == 2))
				Discovery_cnt = DS2488_Discovery(ROM_ID_LIST);

			// Check IOB and VL for power of each earbud to know if battery dead or not
			earbud_batt_FLAG = CheckEarbudPower(Discovery_cnt);

			// Put into charge state
			if (earbud_batt_FLAG)
			{
				SetChargeState();	// Charge right away
					dprintf("\n***Charge State Long Increment***");
				msDelay(5000);		// Charge longer before next check
			}

			// Set Right/Left ROMID if both earbuds alive
			SetOneWireStateIOA();
			if (!Left_Right_Earbud_ROMID_FLAG && SetLeftRightEarbudROM_ID() && !earbud_batt_FLAG)
			{
				dprintf("\nLEFT EARBUD: ");
				for (int i = 0; i < 8; i++)
				{
					dprintf("%02X", LEFT_ROM_NO[i]);
				}
				dprintf("\nRIGHT EARBUD: ");

				for (int i = 0; i < 8; i++)
				{
					dprintf("%02X", RIGHT_ROM_NO[i]);
				}
				dprintf("\n");

				Left_Right_Earbud_ROMID_FLAG = TRUE;
			}

			if (!earbud_batt_FLAG)
			{
				// Simulate a Write Charger Box Battery Level
				for (int i = 0; i < 8; i++)
					ROM_NO[i] = LEFT_ROM_NO[i];
				BUF[0] = Msg_Battery_Box;	// Load Msg
				BUF[1] = rand() % 100;		// Emulate BCD 8-bit Battery level (e.g. 49h is 73% box charge level)
				if (WriteBuffer(2, BUF))
				{
					dprintf("\n***Sent Box Battery Level***\n");
				}

				SetChargeState();
				dprintf("\n***Charge State Normal Increment***\n");
				msDelay(4000);	// Charge normal increment

				// Check Earbud for small msg
				/// Get buffer B info for Earbud if available
				SetOneWireStateIOA();
				if (read_from_BUFB(BUF))
				{
					if (BUF[0] == Msg_Left_Battery_Bud)
						dprintf("Left Earbud Battery Level Received: %02d%%\n", BUF[1]);

					/// Clear BUFA flag by writting to BUF and provide battery voltage
					if (WriteBuffer(0, BUF))
						dprintf("BUFA/BUFB flags cleared.\n");
				}
			}
		}
		else
		{
			RTSCOM(1);		// Set RTS_N non-active to make sure 5V Charging Power is off
			DTRCOM(1);		// Set DTR_N non-active to turn off VPUP
			dprintf("\n***Earbud(s) Not In Box - In Power Saver Mode***\n");
			Sleep(100);
		}

	}

	// release the port
	dprintf("\n ***LOOP EXITED!***");
	// print time
//	time(&tlong);
	tstruct = localtime(&tlong);
	dprintf("\n#TIMESTAMP: %02d/%02d/%04d %02d:%02d:%02d \n",
		tstruct->tm_mon + 1, tstruct->tm_mday, tstruct->tm_year + 1900,
		tstruct->tm_hour, tstruct->tm_min, tstruct->tm_sec);
	CloseCOM();
	dprintf("\nReleased the COM port.");
	msDelay(400);
}

//-------------------------------------------------------------------------
/// Finds all the slaves 
/// 
/// cnt - 0 or 1 or 2 can be found
/// romid_list - 16 byte romid array.
///
/// return@	2 - found two slaves
///			1 - found one slave
///			0 - found no slaves
///
int DS2488_Discovery(uchar *romid_list)
{

	if (!OWReset())
	{
		return 0;
	}

	if (OWFirst())
	{
		for (int i = 7; i >= 0; i--)
		{
			romid_list[i] = ROM_NO[i];
			//printf("%02X", ROM_NO[i]);
		}
	}
	else
		return 0;
	
	//printf("\n");

	if (OWNext())
	{
		for (int i = 7; i >= 0; i--)
		{
			romid_list[i + 8] = ROM_NO[i];
			//printf("%02X", ROM_NO[i]);
		}

		//printf("\n");
		return 2;
	}
	else
		return 1;
}


//-------------------------------------------------------------------------
/// Put into charge state
///
///
void SetChargeState(void)
{
	// Turning off Power for timer period of 26ms
	RTSCOM(1);		// Set RTS_N logic low to make sure 5V Charging Power is off
	DTRCOM(1);		// Set DTR_N low to turn off VPUP
	msDelay(26);

	//Turning on 5V Power
	RTSCOM(0);		// Set RTS_N logic high to enable charging power
}


//-------------------------------------------------------------------------
/// Put into IOA 1-Wire Operation after being in Charge State
///
/// return@	TRUE - Successful
///			FALSE - Failed
///
void SetOneWireStateIOA(void)
{
	DTRCOM(0);		// Set DTR_N High to turn on VPUP
	RTSCOM(1);		// Set RTS_N logic low to make sure 5V Chargeing Power is off
}



//-------------------------------------------------------------------------
/// Check Power State.
///
/// return@	TRUE - Successful then both earbud batteries are dead
///			FALSE - Failed then power is available on VL and IOB
///
int CheckEarbudPower(int cnt)
{
	int dead_earbud_battery = FALSE;
	uchar status_byte;

	// Check if Earbud battery is alive or dead
	for (int i = 0; i < 8; i++)
		ROM_NO[i] = ROM_ID_LIST[i];
	setDeviceSelectMode(SELECT_MATCH);
	ReadStatus(&status_byte);
	// Check 1st earbud
	if (!(cnt && status_byte & MASK_IOBS))
	{
		dprintf("1st Earbud Battery Dead\n");
	}
	// Check for another earbud
	if (cnt == 2)
	{
		for (int i = 0; i < 8; i++)
			ROM_NO[i] = ROM_ID_LIST[i + 8];
	}
	ReadStatus(&status_byte);
	// Check 2nd earbud
	if (!(cnt && status_byte & MASK_IOBS))
	{
		dprintf("2nd Earbud Battery Dead\n");
		return dead_earbud_battery = TRUE;
	}
	return FALSE;
}



//-------------------------------------------------------------------------
/// Set Left Right Earbud ROM_ID.  Requires global LEFT/RIGHT ROMID arrays.
///
/// return@	TRUE - Successful then both earbud romids set
///			FALSE - Failed then HW PIOA signal wrong
///
int SetLeftRightEarbudROM_ID(void)
{
	uchar PIO_input_byte = { 0x00 };


	// Reset ROM_NO from ROM LIST
	for (int i = 0; i < 8; i++)
		ROM_NO[i] = ROM_ID_LIST[i];

	setDeviceSelectMode(SELECT_MATCH);
	PIO_Read(&PIO_input_byte);

	// PIOA of right earbud should be grounded by a jumper on P2.
	if ((PIO_input_byte & 0x10) == 0x10) // Check PIOAL_N bit is high for RIGHT
	{
		for (int j = 0; j < 8; j++)
		{
			RIGHT_ROM_NO[j] = ROM_NO[j];
			LEFT_ROM_NO[j] = ROM_ID_LIST[j + 8];
		}
		return TRUE;
	}
	else if ((PIO_input_byte & 0x01) == 0x01)  // Check PIOAL bit is high for LEFT
	{
		for (int k = 0; k < 8; k++)
		{
			LEFT_ROM_NO[k] = ROM_NO[k];
			RIGHT_ROM_NO[k] = ROM_ID_LIST[k + 8];
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------
//  Description:
//     Reads the buffer
//
// Return@	TRUE - Loaded BUF
//			FALSE - BUFA data not available
//		
int read_from_BUFB(uchar* BUF)
{
	uchar status;

	setDeviceSelectMode(SELECT_MATCH);
	ReadStatus(&status);
	if ((status & 0x02))	// Check if BUFB flag is set
	{
		if (ReadBuffer(BUF))
		{
			return TRUE;
		}
	}
	return FALSE;
}


//--------------------------------------------------------------------------
//  Description:
//     Delay for at least 'len' ms
// 
void msDelay(int len)
{

	Sleep(len);
}

//--------------------------------------------------------------------------
// Debug printf to opened file DFile with a force flush 
//
// Return: number of characters printed
//
//#define dprintf printf
//int dprintf(char *format, ...)
//{
//	int rt;
//	va_list ap;

//	if (DFile != NULL)
//	{
//		va_start(ap, format);
//		rt = vfprintf(DFile, format, ap);
//		va_end(ap);

//		fflush(DFile);
//	}

//	va_start(ap, format);
//	rt = vfprintf(stdout, format, ap);
//	va_end(ap);

//	return rt;
//}


