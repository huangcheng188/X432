#include <stdio.h>
#include <time.h>
#include <dir.h>

/* constants */

//#define T1ms 3


// Baud rate bits
#define PARMSET_1200     0x00
#define PARMSET_2400     0x01
#define PARMSET_4800     0x02
#define PARMSET_9600     0x03
#define PARMSET_14400    0x04
#define PARMSET_19200    0x05
#define PARMSET_38400    0x06
#define PARMSET_57600    0x07
#define PARMSET_115200   0x08
#define PARMSET_153600   0x09
#define PARMSET_256000   0x0a
#define PARMSET_460800   0x0b
#define PARMSET_512000   0x0c
#define PARMSET_800000   0x0d




//**** ROM Command
#define READ_ROM 	     		0x33
#define SKIP_ROM  	     		0xCC
#define MATCH_ROM	     		0x55
#define SEARCH_ROM	     		0xF0
#define RESUME_ROM		 		0xA5
#define OVERDRIVE_SKIP_ROM	    0x3C
#define OVERDRIVE_MATCH_ROM	    0x69



//define one wire communication speed
#define NormalMode   0
#define OverDrive    1

//define data pattern for bit '0' and '1'
#define ONE_BIT   0xff
#define ZERO_BIT  0xC0



#define SMALLINT short



#ifdef TMEX
HANDLE ComID;
DCB dcb;
OVERLAPPED osRead,osWrite;
unsigned char  OneWireRate;

// global variables for this module to hold search state information

unsigned char ROM_NO[8];
unsigned char state_buffer[32];
unsigned char CRC8;

#else
extern PACKAGE HANDLE ComID;
extern PACKAGE DCB dcb;
extern PACKAGE OVERLAPPED osRead,osWrite;
extern unsigned char  OneWireRate;

// global variables for this module to hold search state information
extern unsigned char ROM_NO[8];
unsigned char CRC8;

#endif

