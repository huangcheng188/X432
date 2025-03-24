


//---------------------------------------------------------------------------
//Convert the hex string to buf
//input:
//str, string pointer
//buf, pointer for reserving the data
unsigned char HEXStringToBuf( char *str, unsigned char *buf)
{
   int i;
   unsigned char temp[256], offset=0, Length=0, Start_Digit=0;
   Length=strlen( str );
   if( Length==0 ) return Length;
   if( str[0]=='0' && str[1]=='x' ) Start_Digit=2;
   for( i=0;i<2;i++)
   {
     if( str[2+i+Start_Digit]==' ' || str[2+i+Start_Digit]==',' ) offset++;        //search for ' ' or ','
     else break;
   }
   Length= (Length+offset+Start_Digit)/(2+offset+Start_Digit);
   if( Length >128 ) Length=128;
   for( i=0;i< Length; i++) sscanf(&str[i*(2+offset+Start_Digit)+Start_Digit], "%2X", &temp[i]);
   memcpy(buf,temp,Length);
   return Length;
}
//---------------------------------------------------------------------------
//Convert buf into HEX string without '0x' and ','
//input:
//buf, data pointer
//length, byte length to be converted
//str, pointer for reserving the converted string
void ConvertBufToPureString( unsigned char *buf, unsigned char Length, char *str)
{
   int i;
   for( i=0;i<Length; i++) sprintf( &str[i*3],"%2.2X%c", buf[i],' ');
   if( Length==0 ) str[0]=0x00;
   else  str[3*Length-1]=0x00;
}









 