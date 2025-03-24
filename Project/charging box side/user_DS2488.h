#include <stdio.h>
#include <time.h>
//#include <dir.h>

/* type defs */
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned long  ulong;



/* constants */
#define DS2488_FAM  0x17


//define for what 1-wire interface
#define USE_UART 1

//define token state
#define TOKEN_IOA  0
#define TOKEN_IOB  1
#define TOKEN_PTM  2

//define IOA states
#define IOA_Logic_Low  0
#define IOA_Logic_High  1
#define IOA_Charing_Power_On  2
#define IOA_Logic_High_With_PTM  3


//constant
#define CHARGING_POWER_ON 1
#define CHARGING_POWER_OFF 0
#define LOGIC_HIGH 1
#define LOGIC_LOW 0

//define some structs type



//struct data content in DS2488
struct DS2488_Content
{

   uchar ConfigureByte;
   uchar Status;
   uchar PIO_State;
   uchar Buffer[8];
   uchar RomID[8];
   uchar ChargingPowerState;
   uchar UART_Mode;
   uchar Quiet_Mode;
};



#ifdef TMEX

uchar  Select_IOA_State= IOA_Logic_High;
uchar  L_Earbud_ROMID[8],R_Earbud_ROMID[8], InBox_Earbuds_Number=0;
struct DS2488_Content *DS2488, *DeviceData;


#else
extern uchar  Select_IOA_State;
extern uchar  L_Earbud_ROMID[8],R_Earbud_ROMID[8], InBox_Earbuds_Number;
extern PACKAGE struct DS2488_Content *DS2488, *DeviceData;



#endif