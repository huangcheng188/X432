//---------------------------------------------------------------------------



#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <io.h>

#define TMEX

#include "user_Form.h"
#include "OneWire_protocol.h"
#include "user_DS2488.h"
#include "OneWire_protocol.cpp"
#include "user_DS2488.cpp"
#include "DS2488.h"
#include "DS2488.c"



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
   Select_IOA_State=IOA_Logic_High;
   ComboBox1->ItemIndex=IOA_Logic_High;
   ComboBox1Change(Owner);
}
//---------------------------------------------------------------------------
int  __fastcall Execute_ROM_Command(uchar L_Earbud_Selected, uchar R_Earbud_Selected)
{
   switch(InBox_Earbuds_Number)
   {
      case 0:
      default:
        break;
      case 1:
         return ( owSkipROM() );
        break;
      case 2:
         if( L_Earbud_Selected==true && R_Earbud_Selected ==true ) return( owSkipROM() );
         if( L_Earbud_Selected==true && R_Earbud_Selected ==false ) return( owMatchROM(L_Earbud_ROMID) );
         if( L_Earbud_Selected==false && R_Earbud_Selected ==true ) return( owMatchROM(R_Earbud_ROMID) );
        break;
   }
   return false;
}


//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
   int i,j;
   char tstr[200];
   unsigned char buf[200], flag,refresh_flag;
   static unsigned short idletime=0;
   static unsigned char ROMID_Ready=false;
   static unsigned char Token_State;
   idletime++;
   refresh_flag=(idletime%50);
   switch( Select_IOA_State )
   {
      case IOA_Logic_Low:          //token belong to IOB
      if( refresh_flag ==0 )
      {
//clear the message window
        Memo1->Lines->Clear();
        Memo1->Lines->Add("Waiting for Token belong to IOA...");
        Memo1->Lines->Add("Waiting time (s): "+AnsiString(idletime/50));
        ROMID_Ready=false;
      }
      StatusBar1->Panels->Items[3]->Text="Token belong to 1-wire IOB.";
      GroupBox2->Enabled=false;
      break;

      case IOA_Logic_High:        //token belong to IOA
      if( refresh_flag ==0 )
      {
         flag= Detect_Earbuds(buf);
         if( flag!= InBox_Earbuds_Number) ROMID_Ready=false;  //if earbud numbers in box is changed
         InBox_Earbuds_Number=flag;
         StatusBar1->Panels->Items[1]->Text=AnsiString(InBox_Earbuds_Number);
         if( ROMID_Ready!=false)
         {
           if( InBox_Earbuds_Number == 2 )      //two earbuds in box
           {
                CheckBox41->Enabled=true;
                CheckBox42->Enabled=true;
                ROMID_Ready=true;
           }

           if( InBox_Earbuds_Number == 1 )      //only one earbud in box
           {
              if( (memcmp(buf, L_Earbud_ROMID, 8)) == 0 )
              {
                CheckBox41->Enabled=true;
                CheckBox42->Enabled=false;
                CheckBox42->Checked=false;
              }
              else
              {
                CheckBox41->Enabled=false;
                CheckBox41->Checked=false;
                CheckBox42->Enabled=true;
              }
              ROMID_Ready=true;
           }
           if( InBox_Earbuds_Number == false )      //no earbud in box
           {
              CheckBox41->Enabled=false;
              CheckBox42->Enabled=false;
              CheckBox41->Checked=false;
              CheckBox42->Checked=false;
              ROMID_Ready=false;
           }
         }
         else
         {
           ComboBox1Change(Sender);
           if( InBox_Earbuds_Number != false )  ROMID_Ready=true;
         }
         GroupBox2->Enabled=true;
/*         if( (DS2488ReadStatus(&DeviceData->Status))== true )
         {
           if( DeviceData->Status != DS2488->Status )
           {
//clear the message window
              Memo1->Lines->Clear();
//              if( (DeviceData->Status & STATUS_CMPS ) !=0 ) Memo1->Lines->Add("Charging power applied to IOA");
              if( (DeviceData->Status & STATUS_IOAS ) !=0 ) Memo1->Lines->Add("IOA='1' in logic high.");
              else   Memo1->Lines->Add("IOA='0' in logic low.");
              if( (DeviceData->Status & STATUS_BUFA ) !=0 ) Memo1->Lines->Add("Buffer is written from 1-wire IOA.");
              if( (DeviceData->Status & STATUS_BUFB ) !=0 ) Memo1->Lines->Add("Buffer is written from 1-wire IOB.");
           }
           DS2488->Status= DeviceData->Status;
         }      */
         StatusBar1->Panels->Items[3]->Text="Token belong to 1-wire IOA.";
      }
      break;

      case IOA_Charing_Power_On:
      break;

      case IOA_Logic_High_With_PTM:
      GroupBox2->Enabled=false;
      if( refresh_flag ==0 )
      {
//clear the message window
          Memo1->Lines->Clear();
         StatusBar1->Panels->Items[3]->Text="DS2488 in PTM mode";
      }
      break;
      default:
      break;
   }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)       //write configure command 0x11
{
   unsigned char buf[128];
//clear the message window
   Memo1->Lines->Clear();
   Memo1->Lines->Add("Execute write configure command...");
   HEXStringToBuf( Edit1->Text.c_str(), buf);       //read configure byte from Edit1->Text
   if( CheckBox41->Checked==false && CheckBox42->Checked==false  )
   {
     Memo1->Lines->Add("Please select earbuds to be operated!");
     return;
   }
   if( (Execute_ROM_Command(CheckBox41->Checked,CheckBox42->Checked)) != true )
   {
     Memo1->Lines->Add("No DS2488 presence!");
   }
   else
   {
     if( (DS2488WriteConfigure(buf)) == true ) Memo1->Lines->Add("Success in Writing Configure!");
     else Memo1->Lines->Add("Error in Writing Configure!");
   }
//   Memo1->Lines->Add("Finished writing configure command.");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)   //read configure command 0x22
{
   unsigned char buf[128], flag;
   char str[100];
//clear the message window
   Memo1->Lines->Clear();
   Memo1->Lines->Add("Execute read configure command...");
   if( CheckBox41->Checked==false && CheckBox42->Checked==false  )
   {
     Memo1->Lines->Add("Please select earbuds to be operated!");
     return;
   }
   if( (Execute_ROM_Command(CheckBox41->Checked,CheckBox42->Checked)) != true )
   {
     Memo1->Lines->Add("No DS2488 presence!");
   }
   else
   {
     flag=DS2488ReadConfigure(buf);
     ConvertBufToPureString( buf, 3, str);
     Memo1->Lines->Add("Configure Byte with 2-byte CRC-16 as follows:");
     Memo1->Lines->Add(AnsiString(str));
     if( flag == true )
     {
       Memo1->Lines->Add("Success in Reading Configure!");
       str[2]=0x00;
       Edit2->Text=AnsiString( str );
    //display configure bit information
       if( (buf[0]& CONFIGURE_SEL ) !=0 ) CheckBox9->Checked=true;
       else  CheckBox9->Checked=false;
       if( (buf[0]& CONFIGURE_BUFAPE ) !=0 ) CheckBox10->Checked=true;
       else  CheckBox10->Checked=false;
       if( (buf[0]& CONFIGURE_BUFBPE ) !=0 ) CheckBox11->Checked=true;
       else  CheckBox11->Checked=false;
       if( (buf[0]& CONFIGURE_PTM ) !=0 ) CheckBox12->Checked=true;
       else  CheckBox12->Checked=false;
       if( (buf[0]& CONFIGURE_QM ) !=0 ) CheckBox13->Checked=true;
       else  CheckBox13->Checked=false;
       if( (buf[0]& CONFIGURE_PULLUP ) !=0 ) CheckBox14->Checked=true;
       else  CheckBox14->Checked=false;
       if( (buf[0]& 0x40 ) !=0 ) CheckBox15->Checked=true;       //undefined bit6
       else  CheckBox15->Checked=false;
       if( (buf[0]& 0x80 ) !=0 ) CheckBox16->Checked=true;      //undefined bit7
       else  CheckBox16->Checked=false;
     }
     else Memo1->Lines->Add("Error in Reading Configure!");
   }
//   Memo1->Lines->Add("Finished reading configure command.");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button6Click(TObject *Sender)    //write buffer command 0x33
{
   unsigned char buf[128], Buf_length;
//clear the message window
   Memo1->Lines->Clear();
   Memo1->Lines->Add("Execute write buffer command...");

   Buf_length=HEXStringToBuf( Edit6->Text.c_str(), buf);       //read buffer bytes from Edit6->Text
   if( CheckBox41->Checked==false && CheckBox42->Checked==false  )
   {
     Memo1->Lines->Add("Please select earbuds to be operated!");
     return;
   }
   if( (Execute_ROM_Command(CheckBox41->Checked,CheckBox42->Checked)) != true )
   {
     Memo1->Lines->Add("No DS2488 presence!");
   }
   else
   {
     if( (DS2488WriteBuffer(buf,Buf_length)) == true )
     {
       Memo1->Lines->Add(AnsiString(Buf_length)+" byte(s) written to the buffer.");
       Memo1->Lines->Add("Success in Write Buffer command!");
     }
     else Memo1->Lines->Add("Error in Write Buffer command!");
   }
//   Memo1->Lines->Add("Finished write buffer command.");

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button7Click(TObject *Sender)   //read buffer command 0x44
{
   unsigned char buf[128], Buf_length,flag;
   char str[100];
//clear the message window
   Memo1->Lines->Clear();
   Memo1->Lines->Add("Execute read status command...");
   if( CheckBox41->Checked==false && CheckBox42->Checked==false  )
   {
     Memo1->Lines->Add("Please select earbuds to be operated!");
     return;
   }
   if( (Execute_ROM_Command(CheckBox41->Checked,CheckBox42->Checked)) != true )
   {
     Memo1->Lines->Add("No DS2488 presence!");
   }
   else
   {
     flag=DS2488ReadBuffer(buf,&Buf_length);
     ConvertBufToPureString( &Buf_length, 1, str);
     Memo1->Lines->Add("The reading buffer length="+AnsiString(str));
     ConvertBufToPureString( buf, Buf_length, str);
     Memo1->Lines->Add("buffer ="+AnsiString(str));
     ConvertBufToPureString( &buf[Buf_length], 2, str);
     Memo1->Lines->Add("The return 2-byte CRC16 ="+AnsiString(str));
     if( flag == true )
     {
       ConvertBufToPureString( buf, Buf_length, str);
       Edit7->Text=AnsiString( str );
     }
     else Memo1->Lines->Add("Error in read buffer command!");
   }
//   Memo1->Lines->Add("Finished reading buffer command.");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button3Click(TObject *Sender)     //read status command 0x55
{
   unsigned char buf[128], flag;
   char str[100];
//clear the message window
   Memo1->Lines->Clear();
   Memo1->Lines->Add("Execute read status command...");
   if( CheckBox41->Checked==false && CheckBox42->Checked==false  )
   {
     Memo1->Lines->Add("Please select earbuds to be operated!");
     return;
   }
   if( (Execute_ROM_Command(CheckBox41->Checked,CheckBox42->Checked)) != true )
   {
     Memo1->Lines->Add("No DS2488 presence!");
   }
   else
   {
     flag=DS2488ReadStatus(buf);
     ConvertBufToPureString( buf, 3, str);
     Memo1->Lines->Add("Status Byte with 2-byte CRC-16 as follows:");
     Memo1->Lines->Add(AnsiString(str));
     if( flag == true )
     {
       Memo1->Lines->Add("Success in Reading Status!");
       str[2]=0x00;
       Edit3->Text=AnsiString( str );
    //display DS2488 status bit information
       if( (buf[0]& STATUS_BUFA ) !=0 ) CheckBox17->Checked=true;
       else  CheckBox17->Checked=false;
       if( (buf[0]& STATUS_BUFB ) !=0 ) CheckBox18->Checked=true;
       else  CheckBox18->Checked=false;
       if( (buf[0]& STATUS_IOAS ) !=0 ) CheckBox19->Checked=true;
       else  CheckBox19->Checked=false;
       if( (buf[0]& STATUS_IOBS ) !=0 ) CheckBox20->Checked=true;
       else  CheckBox20->Checked=false;
       if( (buf[0]& STATUS_CMPS ) !=0 ) CheckBox21->Checked=true;
       else  CheckBox21->Checked=false;
       if( (buf[0]& STATUS_TOKS ) !=0 ) CheckBox22->Checked=true;
       else  CheckBox22->Checked=false;
       if( (buf[0]& STATUS_TRST ) !=0 ) CheckBox23->Checked=true;
       else  CheckBox23->Checked=false;
       if( (buf[0]& STATUS_PSW ) !=0 ) CheckBox24->Checked=true;
       else  CheckBox24->Checked=false;
     }
     else Memo1->Lines->Add("Error in Reading status!");
   }
//   Memo1->Lines->Add("Finished reading status command.");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)    //PIO write command 0x66
{
   unsigned char buf[128];
//clear the message window
   Memo1->Lines->Clear();
   Memo1->Lines->Add("Execute PIO write command...");

   HEXStringToBuf( Edit4->Text.c_str(), buf);       //read PIO output logic from Edit4->Text
   if( CheckBox41->Checked==false && CheckBox42->Checked==false  )
   {
     Memo1->Lines->Add("Please select earbuds to be operated!");
     return;
   }
   if( (Execute_ROM_Command(CheckBox41->Checked,CheckBox42->Checked)) != true )
   {
     Memo1->Lines->Add("No DS2488 presence!");
   }
   else
   {
     if( (DS2488WritePIO(buf)) == true ) Memo1->Lines->Add("Success in Writing PIO command!");
     else Memo1->Lines->Add("Error in Writing PIO command!");
   }
//   Memo1->Lines->Add("Finished PIO write command.");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button5Click(TObject *Sender)   //PIO read command 0x77
{
   unsigned char buf[128], flag;
   char str[100];
//clear the message window
   Memo1->Lines->Clear();
   Memo1->Lines->Add("Execute PIO read command...");
   if( CheckBox41->Checked==false && CheckBox42->Checked==false  )
   {
     Memo1->Lines->Add("Please select earbuds to be operated!");
     return;
   }
   if( (Execute_ROM_Command(CheckBox41->Checked,CheckBox42->Checked)) != true )
   {
     Memo1->Lines->Add("No DS2488 presence!");
   }
   else
   {
     flag=DS2488ReadPIO(buf);
     ConvertBufToPureString( buf, 3, str);
     Memo1->Lines->Add("PIO Status with 2-byte CRC-16 as follows:");
     Memo1->Lines->Add(AnsiString(str));
     if( flag == true )
     {
       Memo1->Lines->Add("Success in Reading PIO Status!");
       str[2]=0x00;
       Edit5->Text=AnsiString( str );
    //display DS2488 PIO status bit information
       if( (buf[0]& PIOAS ) !=0 ) CheckBox33->Checked=true;
       else  CheckBox33->Checked=false;
       if( (buf[0]& PIOBS ) !=0 ) CheckBox34->Checked=true;
       else  CheckBox34->Checked=false;
       if( (buf[0]& PIOCS ) !=0 ) CheckBox35->Checked=true;
       else  CheckBox35->Checked=false;
       if( (buf[0]& 0x08 ) !=0 ) CheckBox36->Checked=true;        //should be fixed to '0'
       else  CheckBox36->Checked=false;
       if( (buf[0]& PIOAS ) ==0 ) CheckBox37->Checked=true;       //inverted logic
       else  CheckBox37->Checked=false;
       if( (buf[0]& PIOBS ) ==0 ) CheckBox38->Checked=true;       //inverted logic
       else  CheckBox38->Checked=false;
       if( (buf[0]& PIOCS ) ==0 ) CheckBox39->Checked=true;       //inverted logic
       else  CheckBox39->Checked=false;
       if( (buf[0]& 0x80 ) !=0 ) CheckBox40->Checked=true;        //should be fixed to '1'
       else  CheckBox40->Checked=false;
     }
     else Memo1->Lines->Add("Error in Reading PIO status!");
   }
//   Memo1->Lines->Add("Finished PIO read command.");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button8Click(TObject *Sender)   //Write timeout value command 0x88
{
   unsigned char buf[128];
//clear the message window
   Memo1->Lines->Clear();
   Memo1->Lines->Add("Execute write timeout value command...");

   HEXStringToBuf( Edit8->Text.c_str(), buf);       //read timeout value from Edit8->Text
   if( CheckBox41->Checked==false && CheckBox42->Checked==false  )
   {
     Memo1->Lines->Add("Please select earbuds to be operated!");
     return;
   }
   if( (Execute_ROM_Command(CheckBox41->Checked,CheckBox42->Checked)) != true )
   {
     Memo1->Lines->Add("No DS2488 presence!");
   }
   else
   {
     if( (DS2488WriteTimeOutValue(buf)) == true ) Memo1->Lines->Add("Success in Writing Timeout Value!");
     else Memo1->Lines->Add("Error in Writing Timeout Value!");
   }
//   Memo1->Lines->Add("Finished writing Timeout Value command.");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button9Click(TObject *Sender)   //read timeout value command 0x99
{
   unsigned char buf[128], flag;
   char str[100];
//clear the message window
   Memo1->Lines->Clear();
   Memo1->Lines->Add("Execute read timeout value command...");

/*
      EscapeCommFunction(ComID, CLRDTR);   //set IOA to logic high
      Sleep(27);
     buf[0]=0xE0; buf[1]=0xFF;
     SetBaudCOM(PARMSET_115200);
     Sleep(3);   //wait 3ms to setup
     FlushCOM();
     Sleep(3);   //wait 3ms to setup
     WriteCOM(1,buf);
     ReadCOM(1,&buf[1]);
     ConvertBufToPureString( buf, 2, str);
     Memo1->Lines->Add(AnsiString(str));
     return;                 */

   if( CheckBox41->Checked==false && CheckBox42->Checked==false  )
   {
     Memo1->Lines->Add("Please select earbuds to be operated!");
     return;
   }
   if( (Execute_ROM_Command(CheckBox41->Checked,CheckBox42->Checked)) != true )
   {
     Memo1->Lines->Add("No DS2488 presence!");
   }
   else
   {
     flag=DS2488ReadTimeOutValue(buf);
     ConvertBufToPureString( buf, 3, str);
     Memo1->Lines->Add("Timeout value with 2-byte CRC-16 as follows:");
     Memo1->Lines->Add(AnsiString(str));
     if( flag == true )
     {
       Memo1->Lines->Add("Success in Reading Timeout Value!");
       str[2]=0x00;
       Edit9->Text=AnsiString( str );
     }
     else Memo1->Lines->Add("Error in Reading Timeout Value!");
   }
//   Memo1->Lines->Add("Finished Reading Timeout Value command.");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox1Click(TObject *Sender)
{
   unsigned char temp=0;
   char str[100];
   if( CheckBox1->Checked == true )  temp|=CONFIGURE_SEL;
   if( CheckBox2->Checked == true )  temp|=CONFIGURE_BUFAPE;
   if( CheckBox3->Checked == true )  temp|=CONFIGURE_BUFBPE;
   if( CheckBox4->Checked == true )  temp|=CONFIGURE_PTM;
   if( CheckBox5->Checked == true )  temp|=CONFIGURE_QM;
   if( CheckBox6->Checked == true )  temp|=CONFIGURE_PULLUP;
   if( CheckBox7->Checked == true )  temp|=0x40;
   if( CheckBox8->Checked == true )  temp|=0x80;
   DS2488->ConfigureByte=temp;
   sprintf(str, "%2.2X", temp);
   str[2]=0x00;
   Edit1->Text=AnsiString(str);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox25Click(TObject *Sender)
{
   unsigned char temp=0xf0;
   char str[100];
   if( CheckBox25->Checked == true )  { temp|=PIOAS; temp^=(PIOAS<<4); CheckBox29->Checked = false;}
   else CheckBox29->Checked = true;
   if( CheckBox26->Checked == true )  { temp|=PIOBS; temp^=(PIOBS<<4); CheckBox30->Checked = false;}
   else CheckBox30->Checked = true;
   if( CheckBox27->Checked == true )  { temp|=PIOCS; temp^=(PIOCS<<4); CheckBox31->Checked = false;}
   else CheckBox31->Checked = true;
   DS2488->ConfigureByte=temp;
   sprintf(str, "%2.2X", temp);
   str[2]=0x00;
   Edit4->Text=AnsiString(str);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::ComboBox1Change(TObject *Sender)
{
    uchar buf[32], buf1[32];
    char str[100];
//clear the message window
   Memo1->Lines->Clear();
    
    Select_IOA_State= ComboBox1->ItemIndex;
    switch( Select_IOA_State )
    {
      case IOA_Logic_Low:
         IOA_ChargingPowerControl(CHARGING_POWER_OFF);        //switch off charging power
         IOA_Logic_Control(LOGIC_LOW);                        //put IOA at logic low or low impedence state
         Sleep(27);  //wait IOA to lose the DS2488's token
         break;

      case IOA_Logic_High:
         IOA_ChargingPowerControl(CHARGING_POWER_OFF);       //switch off charging power
         IOA_Logic_Control(LOGIC_HIGH);                      //put IOA at logic high state
         Sleep(27);  //wait IOA to get the DS2488's token
         InBox_Earbuds_Number=owSearchROM(DS2488_FAMILY, buf);  //search earbuds in box
         StatusBar1->Panels->Items[1]->Text=AnsiString(InBox_Earbuds_Number);
//         InBox_Earbuds_Number=2;
         if( InBox_Earbuds_Number==0 )
         {
           CheckBox41->Caption=AnsiString("L-Earbud's ROMID= Not Available");
           CheckBox42->Caption=AnsiString("R-Earbud's ROMID= Not Available");
           GroupBox2->Enabled=false;
           CheckBox41->Enabled=false;
           CheckBox42->Enabled=false;
           CheckBox41->Checked=false;
           CheckBox42->Checked=false;
         }
         if( InBox_Earbuds_Number==1 )
         {
            GroupBox2->Enabled=true;
            ConvertBufToPureString( buf, 8, str);
            owSkipROM();
            DS2488ReadPIO(buf1);
            if( (buf1[0] & PIOBS) == 0 )      //check if L-earbud
            {
               memcpy(L_Earbud_ROMID, buf, 8);
               CheckBox41->Caption=AnsiString("L-Earbud's ROMID= "+AnsiString(str));
               CheckBox42->Caption=AnsiString("R-Earbud's ROMID= Not Available");
               CheckBox41->Enabled=true;
               CheckBox42->Enabled=false;
               CheckBox42->Checked=false;
            }
            else
            {
               memcpy(R_Earbud_ROMID, buf, 8);
               CheckBox41->Caption=AnsiString("L-Earbud's ROMID= Not Available");
               CheckBox42->Caption=AnsiString("R-Earbud's ROMID= "+AnsiString(str));
               CheckBox41->Enabled=false;
               CheckBox42->Enabled=true;
               CheckBox41->Checked=false;
            }
         }
         if( InBox_Earbuds_Number==2 )
         {
            GroupBox2->Enabled=true;
            CheckBox41->Enabled=true;
            CheckBox42->Enabled=true;
            owMatchROM(buf);
            DS2488ReadPIO(buf1);
            if( (buf1[0] & PIOBS) == 0 )      //check if L-earbud
            {
               memcpy(L_Earbud_ROMID, buf, 8);
               ConvertBufToPureString( L_Earbud_ROMID, 8, str);
               CheckBox41->Caption=AnsiString("L-Earbud's ROMID= "+AnsiString(str));
               memcpy(R_Earbud_ROMID, &buf[8], 8);
               ConvertBufToPureString( R_Earbud_ROMID, 8, str);
               CheckBox42->Caption=AnsiString("R-Earbud's ROMID= "+AnsiString(str));
            }
            else
            {
               memcpy(L_Earbud_ROMID, &buf[8], 8);
               ConvertBufToPureString( L_Earbud_ROMID, 8, str);
               CheckBox41->Caption=AnsiString("L-Earbud's ROMID= "+AnsiString(str));
               memcpy(R_Earbud_ROMID, buf, 8);
               ConvertBufToPureString( R_Earbud_ROMID, 8, str);
               CheckBox42->Caption=AnsiString("R-Earbud's ROMID= "+AnsiString(str));
            }
         }
         break;

      case IOA_Charing_Power_On:
         IOA_Logic_Control(LOGIC_HIGH);
         Sleep(27);  //wait 27ms for IOA to get the DS2488's token
         IOA_Logic_Control(LOGIC_LOW);
         Sleep(27);  //wait 27ms for IOA to lose the DS2488's token
         IOA_ChargingPowerControl(CHARGING_POWER_ON);         //switch on charging power
         GroupBox2->Enabled=false;
         break;
      case IOA_Logic_High_With_PTM:
         IOA_ChargingPowerControl(CHARGING_POWER_OFF);       //switch off charging power
         IOA_Logic_Control(LOGIC_HIGH);                      //put IOA at logic high state
         Sleep(27);  //wait IOA to get the DS2488's token
         break;
      default:
      break;
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button10Click(TObject *Sender)
{
   char tstr[100];
   unsigned char DeviceFoundNumber, buf[16];
//clear the message window
   Memo1->Lines->Clear();
         DeviceFoundNumber = (unsigned char)owSearchROM(DS2488_FAMILY, buf);
         if( DeviceFoundNumber != 0 )
         {
            ConvertBufToPureString(&DeviceFoundNumber, 1, tstr);
            Memo1->Lines->Add("Device Founded Number="+ AnsiString(tstr));
            ConvertBufToPureString(buf, 8, tstr);
            Memo1->Lines->Add("ROM ID1= "+ AnsiString(tstr));
            if( DeviceFoundNumber == 2 )
            {
              ConvertBufToPureString(&buf[8], 8, tstr);
              Memo1->Lines->Add("ROM ID2= "+AnsiString(tstr));
            }
         }
         else
         {
           Memo1->Lines->Add("No device founded");
         }

}
//---------------------------------------------------------------------------

