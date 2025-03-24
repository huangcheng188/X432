#include "DS2488.h"

#ifdef TMEX

/* Local Function Prototypes */
// void _fastcall ROMIDtoString(char *, short *);
// void _fastcall ByteToString(uchar, char *);

// void _fastcall IOA_ChargingPowerControl(uchar On_Off);
// void _fastcall IOA_Logic_Control(uchar High_Low);



//Switch on/off charging power
//input:
// On_Off,  ON for switching on charging power, OFF for switching off charging power
void  IOA_ChargingPowerControl(uchar On_Off)
{
  if( On_Off == CHARGING_POWER_ON )
  {
      RTSCOM(0);   //switch on charging power
      Sleep(1);
  }
  else
  {
      RTSCOM(1);   //switch off charging power
      Sleep(1);
  }
}
//set IOA logic state
//input:
// On_Off,  HIGH for setting IOA to logic high, LOW for setting IOA to logic low
void  IOA_Logic_Control(uchar High_Low)
{
  if( High_Low == LOGIC_HIGH )
  {
      DTRCOM(0);   //set IOA to logic high
      Sleep(2);
  }
  else
  {
      DTRCOM(1);    //set IOA to logic low
      Sleep(2);
  }
}
//---------------------------------
//get the token's state to detect which mode DS2488 operates
//input:
// Output Token value: 0---belong to IOA, 1---belong to IOB, 2---PTM mode
unsigned char  Get_Token_State()
{
   unsigned long ModemState;
  //  EscapeCommFunction(ComID, SETDTR);   //set DTR to logic low for clearing Token's D-trigger
  //  EscapeCommFunction(ComID, CLRDTR);   //set DTR to logic high for normally operating Token's D-trigger
   Sleep(1);     //delay 1ms
  //  GetCommModemStatus(ComID, &ModemState);
   if((ModemState&MS_RING_ON)!=0 )
   {
      if((ModemState&MS_DSR_ON)!=0 ) return TOKEN_IOA;
      else  return TOKEN_IOB;
   }
   return  TOKEN_PTM;
}

//---------------------------------
//convert ROM ID to AnsiString
void  ROMIDtoString(char *str, short *ROM)
{
  short k;
  for (k=0;k<8;k++)
  {
    sprintf(str+2*k+k/4,"%2.2X",ROM[7-k]);
    if( k==3) sprintf(str+2*k+2,"-");
  }
}
//---------------------------------
//convert Byte to AnsiString  with displaying each bit
void  ByteToString(uchar byte, char *str)
{
   uchar i;
   uchar t[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
   sprintf(str,"0x%2.2X=",byte);
   for(i=0;i<8;i++)
   {
      if( (byte&t[i]) ) sprintf(str+strlen(str), "%c", '1');
      else   sprintf(str+strlen(str), "%c", '0');
      if(i==3)  sprintf(str+strlen(str), "%c", '-');
   }
}

 

//---------------------------------

#else

extern void _fastcall ROMIDtoString(char *, short *);
extern void _fastcall ByteToString(uchar, char *);

extern void _fastcall IOA_ChargingPowerControl(uchar On_Off);
extern void _fastcall IOA_Logic_Control(uchar High_Low);


#endif

