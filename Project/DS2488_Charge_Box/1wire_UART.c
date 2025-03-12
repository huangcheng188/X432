 //#include <windows.h>
#include <stdio.h>

#include "serial_win32ex.h"

#define OWUART
#include "1wire_UART.h"

// Basic 1-Wire functions
int  OWReset();
unsigned char OWTouchBit(unsigned char sendbit);
unsigned char OWTouchByte(unsigned char sendbyte);
void OWWriteByte(unsigned char byte_value);
void OWWriteBit(unsigned char bit_value);
unsigned char OWReadBit();
unsigned char OWReadByte();
void OWBlock(unsigned char *tran_buf, int tran_len);
int  OWSearch();
int  OWFirst();
int  OWNext();
int  OWVerify();
void OWTargetSetup(unsigned char family_code);
void OWFamilySkipSetup();
int OWReadROM(void);
int OWSkipROM(void);
int OWMatchROM(void);
int OWOverdriveSkipROM(void);
int OWResume(void);

// Extended 1-Wire functions
int OWSpeed(int new_speed);

// misc utility functions
unsigned char docrc8(unsigned char value);

// search state
unsigned char ROM_NO[8];
int LastDiscrepancy;
int LastFamilyDiscrepancy;
int LastDeviceFlag;
unsigned char crc8;

// OW UART State
int USpeed;		//	1-Wire communication speed

int owdebug = FALSE;

//int dprintf(char *format, ...);


//---------------------------------------------------------------------------
//-------- Basic 1-Wire functions
//---------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Reset all of the devices on the 1-Wire Net and return the result.
//
// Returns@ TRUE(1):  presense pulse(s) detected, device(s) reset
//			SHORT(2):	1-Wire Shorted
//          FALSE(0): no presense pulses detected
//
int OWReset(void)
{
	unsigned char writedata[1], readdata[1];

	// flush the buffers
	FlushCOM();

	if (USpeed == SPEEDSEL_STD)
	{
		SetBaudCOM(PARMSET_9600);	// Target 521us for tRSTL StD
		writedata[0] = 0xF0;
		if (WriteCOM(1, writedata))
		{
			// Interpret the response
			if (ReadCOM(1, readdata) == 1)
			{
				// RX = 00h: Short Detected
				if (readdata[0] == 0x00)
					return 2;
				// RX = TX: No Presence Pulse Detected
				else if (readdata[0] == 0xF0)
					return FALSE;
				// RX < TX: Presence Pulse Detected (i.e. 1-Wire Device Detected)
				else if (readdata[0] < 0xF0)
					return TRUE;
			}
		}
	}
	else if (USpeed == SPEEDSEL_OD)
	{
		SetBaudCOM(PARMSET_122400);	// Target 49.02us for tRSTL OVD
		writedata[0] = 0xE0;
		if (WriteCOM(1, writedata))
		{
			// Interpret the response
			if (ReadCOM(1, readdata) == 1)
			{
				// RX = 00h: Short Detected
				if (readdata[0] == 0x00)
					return 2;
				// RX = TX: No Presence Pulse Detected
				else if (readdata[0] == 0xE0)
					return FALSE;
				// RX < TX: Presence Pulse Detected (i.e. 1-Wire Device Detected)
				else if (readdata[0] < 0xE0)
					return TRUE;
			}
		}

	}
	return FALSE;
}

//--------------------------------------------------------------------------
// Send 1 bit of communication to the 1-Wire Net.
// The parameter 'sendbit' least significant bit is used.
//
// 'sendbit' - 1 bit to send (least significant byte)
//
void OWWriteBit(unsigned char sendbit)
{
	OWTouchBit(sendbit);
}

//--------------------------------------------------------------------------
// Send 1 bit of read communication to the 1-Wire Net and and return the
// result 1 bit read from the 1-Wire Net.
//
// Returns: 0:   0 bit read
//          1:   1 bit read
//			55h: COM Error occurred
//
unsigned char OWReadBit(void)
{
	unsigned char writedata[1], readdata[1];
	// flush the buffers
	FlushCOM();

	if (USpeed == SPEEDSEL_STD)
	{
		SetBaudCOM(PARMSET_115200);	// Target 8.68us for tRL
		writedata[0] = 0xFF;
		if (WriteCOM(1, writedata))
		{
			// Interpret the response
			if (ReadCOM(1, readdata) == 1)
			{
			// RX = TX: Logic 1 bit was read
			if (readdata[0] == 0xFF)
				return TRUE;
			// RX < Expected RX: Logic 0 bit was read
			else if (readdata[0] < 0xFF)
				return FALSE;
			}
		}
	}
	else if (USpeed == SPEEDSEL_OD)
	{
		SetBaudCOM(PARMSET_1228800);	// Target 0.81us for tRL
		writedata[0] = 0xFF;
		if (WriteCOM(1, writedata))
		{
			// Interpret the response
			if (ReadCOM(1, readdata) == 1)
			{
			// RX = TX: Logic 1 bit was read
			if (readdata[0] == 0xFF)
				return TRUE;
			// RX < Expected RX: Logic 0 bit was read
			else if (readdata[0] < 0xFF)
				return FALSE;
			}
		}
	}
	// COM Error Must have occurred
	return 0x55;
}

//--------------------------------------------------------------------------
// Send 1 bit of communication to the 1-Wire Net and return the
// result 1 bit read from the 1-Wire Net.  The parameter 'sendbit'
// least significant bit is used and the least significant bit
// of the result is the return bit.
//
// 'sendbit' - the least significant bit is the bit to send
//
// Returns: 0:   0 bit read from sendbit
//          1:   1 bit read from sendbit
//			55h: COM Error Occurred
//
unsigned char OWTouchBit(unsigned char sendbit)
{
	unsigned char writedata[1], readdata[1];

	// flush the buffers
	FlushCOM();

	// Send a 1 bit and see what comes back
	if ((sendbit & 0x01) == 0x01)
	{
		if (USpeed == SPEEDSEL_STD)
		{
			SetBaudCOM(PARMSET_115200);

			writedata[0] = 0xFF;	// Target 8.68us for tRL
			if (WriteCOM(1, writedata))
			{
				// Interpret the response
				if (ReadCOM(1, readdata) == 1)
				{
					// RX = TX: Logic 1 bit was read
					if (readdata[0] == 0xFF)
						return TRUE;
					// RX < Expected RX: Logic 0 bit was read
					else if (readdata[0] < 0xFF)
						return FALSE;
				}
			}
		}
		else if (USpeed == SPEEDSEL_OD)
		{
			SetBaudCOM(PARMSET_1228800);
			writedata[0] = 0xFF;	// Target 0.81us for tRL
			if (WriteCOM(1, writedata))
			{
				// Interpret the response
				if (ReadCOM(1, readdata) == 1)
				{
					// RX = TX: Logic 1 bit was read
					if (readdata[0] == 0xFF)
						return TRUE;
					// RX < Expected RX: Logic 0 bit was read
					else if (readdata[0] < 0xFF)
						return FALSE;
				}
			}
		}
	}

	// Send a 0 bit and see what comes back to check for short/corruption
	if (sendbit == 0)
	{
		if (USpeed == SPEEDSEL_STD)
		{
			SetBaudCOM(PARMSET_115200);

			writedata[0] = 0xC0;	// Target 60.76us for tW0L
			if (WriteCOM(1, writedata))
			{
				// Interpret the response
				if (ReadCOM(1, readdata) == 1)
				{
					// RX = TX: Logic 0 bit was read so written 0 succussfully
					if (readdata[0] == 0xC0)
						return TRUE;
					// RX < Expected RX: Logic 0 bit was read with short or corruption
					else if (readdata[0] < 0xC0)
						return FALSE;
				}
			}

		}
		else if (USpeed == SPEEDSEL_OD)
		{
			SetBaudCOM(PARMSET_1228800);
			writedata[0] = 0x80;	// Target 6.51us for tW0L
			if (WriteCOM(1, writedata))
			{
				// Interpret the response
				if (ReadCOM(1, readdata) == 1)
				{
					// RX = TX: Logic 0 bit was read so written 0 succussfully
					if (readdata[0] == 0x80)
						return TRUE;
					// RX < Expected RX: Logic 0 bit was read with short or corruption
					else if (readdata[0] < 0x80)
						return FALSE;
				}
			}
		}
	}
	//COM Error must have occurred
	return 0x55;
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net is the same (write operation).
// The parameter 'sendbyte' least significant 8 bits are used.
//
// 'sendbyte' - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same
//
void OWWriteByte(unsigned char sendbyte)
{
	// debug
	if (owdebug)
		printf("%02X ", sendbyte);

	OWTouchByte(sendbyte);
}

//--------------------------------------------------------------------------
// Send 8 bits of read communication to the 1-Wire Net and and return the
// result 8 bits read from the 1-Wire Net.
//
// Returns:  8 bits read from 1-Wire Net
//
unsigned char OWReadByte(void)
{
	unsigned char rt;

	rt = OWTouchByte(0xFF);

	// debug
	if (owdebug)
		printf("[%02X] ", rt);

	return rt;
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and return the
// result 8 bits read from the 1-Wire Net.  The parameter 'sendbyte'
// least significant 8 bits are used and the least significant 8 bits
// of the result is the return byte.
//
// 'sendbyte' - 8 bits to send (least significant byte)
//
// Returns:  8 bits read from sendbyte
//
unsigned char OWTouchByte(unsigned char sendbyte)
{

	int loop;
	unsigned char result = 0;

	for (loop = 0; loop < 8; loop++)
	{
		// shift the result to get it ready for the next bit
		result >>= 1;

		// If sending a '1' then read a bit else write a '0'
		if (sendbyte & 0x01)
		{
			if (OWReadBit())
				result |= 0x80;
		}
		else
			OWWriteBit(0);

		// shift the data byte for the next bit
		sendbyte >>= 1;
	}
	return result;
}


//--------------------------------------------------------------------------
// The 'OWBlock' transfers a block of data to and from the
// 1-Wire Net. The result is returned in the same buffer.
//
// 'tran_buf' - pointer to a block of unsigned
//              chars of length 'tran_len' that will be sent
//              to the 1-Wire Net
// 'tran_len' - length in bytes to transfer
void OWBlock(unsigned char *tran_buf, int tran_len)
{
	int loop;

	for (loop = 0; loop < tran_len; loop++)
	{
		tran_buf[loop] = OWTouchByte(tran_buf[loop]);
	}

	// debug
	if (owdebug)
	{
		for (int i = 0; i < tran_len; i++)
		{
				printf("%02X ", tran_buf[i]);
		}
	}

}


//--------------------------------------------------------------------------
// The 'OWReadROM' function does a Read-ROM.  This function
// uses the read-ROM function 33h to read a ROM number and verify CRC8.
//
// Returns:   TRUE (1) : OWReset successful and Serial Number placed 
//                       in the global ROM, CRC8 valid 
//            FALSE (0): OWReset did not have presence or CRC8 invalid
//
int OWReadROM(void)
{
	uchar buf[256];
	int i;

	if (OWReset() == 1)
	{
		buf[0] = 0x33; // READ ROM
		memset(&buf[1], 0xFF, 8);
		OWBlock(buf, 9);

		// verify CRC8
		crc8 = 0;
		for (i = 0; i < 8; i++)
			docrc8(buf[i + 1]);

		if ((crc8 == 0) && (buf[1] != 0))
		{
			memcpy(ROM_NO, &buf[1], 8);
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
int OWSkipROM(void)
{
	if (OWReset() == 1)
	{
		OWWriteByte(0xCC);
		return TRUE;
	}

	return FALSE;
}


//--------------------------------------------------------------------------
// The 'OWResume' function does a Resume command 0xA5.
//
// Returns:   TRUE (1) : OWReset successful and RESUME sent. 
//            FALSE (0): OWReset did not have presence
//
int OWResume(void)
{
	if (OWReset() == 1)
	{
		OWWriteByte(0xA5);
		return TRUE;
	}

	return FALSE;
}


//--------------------------------------------------------------------------
// The 'OWOverdriveSkipROM' function does an Overdrive skip-ROM. Ignores
// result from standard speed OWReset().
// 
// Returns:   TRUE (1) : OWReset and skip rom sent. 
//            FALSE (0): Could not change to overdrive
//
int OWOverdriveSkipROM(void)
{
	OWSpeed(MODE_STANDARD);

	OWReset();
	OWWriteByte(0x3C);

	return (OWSpeed(MODE_OVERDRIVE) == MODE_OVERDRIVE);
}


//--------------------------------------------------------------------------
// The 'OWMatchROM' function does a Match-ROM using the global ROM_NO device
//
// Returns:   TRUE (1) : OWReset successful and match rom sent. 
//            FALSE (0): OWReset did not have presence
//
int OWMatchROM(void)
{
	uchar buf[9];

	// use MatchROM 
	if (OWReset())
	{
		buf[0] = 0x55;
		memcpy(&buf[1], &ROM_NO[0], 8);
		OWBlock(buf, 9);
		return TRUE;
	}
	else
		return FALSE;
}


//--------------------------------------------------------------------------
// The 'OWOverdriveMatchROM' function does an overdrive Match-ROM using the
// global ROM_NO device
//
// Returns:   TRUE (1) : OWReset successful and match rom sent. 
//            FALSE (0): OWReset did not have presence
//
int OWOverdriveMatchROM(void)
{
	uchar buf[9];

	// use overdrive MatchROM 
	OWSpeed(MODE_STANDARD);
	if (OWReset())
	{
		OWWriteByte(0x69);
		OWSpeed(MODE_OVERDRIVE);
		memcpy(&buf[0], &ROM_NO[0], 8);
		OWBlock(buf, 8);
		return TRUE;
	}
	else
		return FALSE;
}


//--------------------------------------------------------------------------
// Find the 'first' devices on the 1-Wire bus
// Return TRUE  : device found, ROM number in ROM_NO buffer
//        FALSE : no device present
//
int OWFirst()
{
	// reset the search state
	LastDiscrepancy = 0;
	LastDeviceFlag = FALSE;
	LastFamilyDiscrepancy = 0;

	return OWSearch();
}

//--------------------------------------------------------------------------
// Find the 'next' devices on the 1-Wire bus
// Return TRUE  : device found, ROM number in ROM_NO buffer
//        FALSE : device not found, end of search
//
int OWNext()
{
	// leave the search state alone
	return OWSearch();
}

//--------------------------------------------------------------------------
// Verify the device with the ROM number in ROM_NO buffer is present.
// Return TRUE  : device verified present
//        FALSE : device not present
//
int OWVerify()
{
	unsigned char rom_backup[8];
	int i, rslt, ld_backup, ldf_backup, lfd_backup;

	// keep a backup copy of the current state
	for (i = 0; i < 8; i++)
		rom_backup[i] = ROM_NO[i];
	ld_backup = LastDiscrepancy;
	ldf_backup = LastDeviceFlag;
	lfd_backup = LastFamilyDiscrepancy;

	// set search to find the same device
	LastDiscrepancy = 64;
	LastDeviceFlag = FALSE;

	if (OWSearch())
	{
		// check if same device found
		rslt = TRUE;
		for (i = 0; i < 8; i++)
		{
			if (rom_backup[i] != ROM_NO[i])
			{
				rslt = FALSE;
				break;
			}
		}
	}
	else
		rslt = FALSE;

	// restore the search state 
	for (i = 0; i < 8; i++)
		ROM_NO[i] = rom_backup[i];
	LastDiscrepancy = ld_backup;
	LastDeviceFlag = ldf_backup;
	LastFamilyDiscrepancy = lfd_backup;

	// return the result of the verify
	return rslt;
}

//--------------------------------------------------------------------------
// Setup the search to find the device type 'family_code' on the next call
// to OWNext() if it is present.
//
void OWTargetSetup(unsigned char family_code)
{
	int i;

	// set the search state to find SearchFamily type devices
	ROM_NO[0] = family_code;
	for (i = 1; i < 8; i++)
		ROM_NO[i] = 0;
	LastDiscrepancy = 64;
	LastFamilyDiscrepancy = 0;
	LastDeviceFlag = FALSE;
}

//--------------------------------------------------------------------------
// Setup the search to skip the current device type on the next call
// to OWNext().
//
void OWFamilySkipSetup()
{
	// set the Last discrepancy to last family discrepancy
	LastDiscrepancy = LastFamilyDiscrepancy;

	// clear the last family discrpepancy
	LastFamilyDiscrepancy = 0;

	// check for end of list
	if (LastDiscrepancy == 0)
		LastDeviceFlag = TRUE;

}

//--------------------------------------------------------------------------
// The 'OWSearch' function does a general search.  This function
// continues from the previos search state. The search state
// can be reset by using the 'OWFirst' function.
//
// Returns:   TRUE (1) : when a 1-Wire device was found and it's
//                       Serial Number placed in the global ROM 
//            FALSE (0): when no new device was found.  Either the
//                       last search was the last device or there
//                       are no devices on the 1-Wire Net.
//
int OWSearch(void)
{
	int id_bit_number;
	int last_zero, rom_byte_number, search_result;
	int id_bit, cmp_id_bit;
	unsigned char rom_byte_mask, search_direction;

	// initialize for search
	id_bit_number = 1;
	last_zero = 0;
	rom_byte_number = 0;
	rom_byte_mask = 1;
	search_result = 0;
	crc8 = 0;

	// if the last call was not the last one
	if (!LastDeviceFlag)
	{
		// 1-Wire reset
		if (!OWReset())
		{
			// reset the search
			LastDiscrepancy = 0;
			LastDeviceFlag = FALSE;
			LastFamilyDiscrepancy = 0;
			return FALSE;
		}

		// issue the search command 
		OWWriteByte(0xF0);

		// loop to do the search
		do
		{
			// read a bit and its complement
			id_bit = OWReadBit();
			cmp_id_bit = OWReadBit();

			// check for no devices on 1-wire
			if ((id_bit == 1) && (cmp_id_bit == 1))
				break;
			else
			{
				// all devices coupled have 0 or 1
				if (id_bit != cmp_id_bit)
					search_direction = id_bit;  // bit write value for search
				else
				{
					// if this discrepancy if before the Last Discrepancy
					// on a previous next then pick the same as last time
					if (id_bit_number < LastDiscrepancy)
						search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
					else
						// if equal to last pick 1, if not then pick 0
						search_direction = (id_bit_number == LastDiscrepancy);

					// if 0 was picked then record its position in LastZero
					if (search_direction == 0)
					{
						last_zero = id_bit_number;

						// check for Last discrepancy in family
						if (last_zero < 9)
							LastFamilyDiscrepancy = last_zero;
					}
				}

				// set or clear the bit in the ROM byte rom_byte_number
				// with mask rom_byte_mask
				if (search_direction == 1)
					ROM_NO[rom_byte_number] |= rom_byte_mask;
				else
					ROM_NO[rom_byte_number] &= ~rom_byte_mask;

				// serial number search direction write bit
				OWWriteBit(search_direction);

				// increment the byte counter id_bit_number
				// and shift the mask rom_byte_mask
				id_bit_number++;
				rom_byte_mask <<= 1;

				// if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
				if (rom_byte_mask == 0)
				{
					docrc8(ROM_NO[rom_byte_number]);  // accumulate the CRC
					rom_byte_number++;
					rom_byte_mask = 1;
				}
			}
		} while (rom_byte_number < 8);  // loop until through all ROM bytes 0-7

									   // if the search was successful then
		if (!((id_bit_number < 65) || (crc8 != 0)))
		{
			// search successful so set LastDiscrepancy,LastDeviceFlag,search_result
			LastDiscrepancy = last_zero;

			// check for last device
			if (LastDiscrepancy == 0)
				LastDeviceFlag = TRUE;

			search_result = TRUE;
		}
	}

	// if no device found then reset counters so next 'search' will be like a first
	if (!search_result || !ROM_NO[0])
	{
		LastDiscrepancy = 0;
		LastDeviceFlag = FALSE;
		LastFamilyDiscrepancy = 0;
		search_result = FALSE;
	}

	return search_result;
}

//---------------------------------------------------------------------------
//-------- Extended 1-Wire functions
//---------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Set the 1-Wire Net communucation speed.
//
// 'new_speed' - new speed defined as
//                MODE_STANDARD   0x00
//                MODE_OVERDRIVE  0x01
//
// Returns:  current 1-Wire Net speed
//
int OWSpeed(int new_speed)
{
	if (new_speed == MODE_OVERDRIVE)
	{
		USpeed = SPEEDSEL_OD;
	}
	else if (new_speed == MODE_STANDARD)
	{
		USpeed = SPEEDSEL_STD;
	}

	// return the current speed
	return (USpeed == SPEEDSEL_OD) ? MODE_OVERDRIVE : MODE_STANDARD;
}

// TEST BUILD with CRC Table
static unsigned char dscrc_table[] = {
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
	116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53 };

//--------------------------------------------------------------------------
// Calculate the CRC8 of the byte value provided with the current 
// global 'crc8' value. 
// Returns current global crc8 value
//
unsigned char docrc8(unsigned char value)
{
	// See Application Note 27

	// TEST BUILD
	crc8 = dscrc_table[crc8 ^ value];
	return crc8;
}


