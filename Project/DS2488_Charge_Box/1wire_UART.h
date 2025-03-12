// definitions
#define FALSE                          0
#define TRUE                           1

#define MODE_STANDARD                  0x00
#define MODE_OVERDRIVE                 0x01

// One Wire speed bits
#define SPEEDSEL_STD                   0x00
#define SPEEDSEL_OD                    0x08

#ifndef uchar
typedef unsigned char uchar;
#endif

#ifndef OWUART
// Basic 1-Wire functions
extern int  OWReset();
extern unsigned char OWTouchBit(unsigned char sendbit);
extern unsigned char OWTouchByte(unsigned char sendbyte);
extern void OWWriteByte(unsigned char byte_value);
extern void OWWriteBit(unsigned char bit_value);
extern unsigned char OWReadBit();
extern unsigned char OWReadByte();
extern void OWBlock(unsigned char *tran_buf, int tran_len);
extern int  OWSearch();
extern int  OWFirst();
extern int  OWNext();
extern int  OWVerify();
extern void OWTargetSetup(unsigned char family_code);
extern void OWFamilySkipSetup();

extern int OWReadROM(void);
extern int OWSkipROM(void);
extern int OWMatchROM(void);
extern int OWOverdriveSkipROM(void);
extern int OWResume(void);
extern int OWOverdriveMatchROM(void);

// Extended 1-Wire functions
extern int OWSpeed(int new_speed);

// UART connectivity functions (Win32 implementation)
extern void FlushCOM(void);
extern int  WriteCOM(int outlen, unsigned char *outbuf);
extern int  ReadCOM(int inlen, unsigned char *inbuf);
extern void BreakCOM(void);
extern void SetBaudCOM(unsigned char new_baud);
extern void msDelay(int len);
extern int  OpenCOM(char *port_zstr);
extern void CloseCOM(void);
extern void DTRCOM(int state);
extern void RTSCOM(int state);

extern unsigned char ROM_NO[8];
#endif
