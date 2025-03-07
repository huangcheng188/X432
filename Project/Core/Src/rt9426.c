         

//add huangcheng 2025-03-05
#include "stm32g0xx_hal.h"
#include <stdio.h>

// 定义 word 类型
typedef uint16_t WORD;

#define DELAY_1MS  1
#define DELAY_5MS  5
#define DELAY_10MS 10
#define DELAY_20MS 20
#define DELAY_60MS 60
#define Delay_msecond HAL_Delay

#if 1
#define RT9426_PRINTF(fmt, ...) printf(fmt "\r\n", ##__VA_ARGS__)
#else
#define RT9426_PRINTF(fmt, ...) 
#endif

#define RT9426_I2C_ADDR (0x55<<1)
#define RT9426_READ_LEN 2
extern I2C_HandleTypeDef hi2c2;

uint8_t Gauge_Write_Register(uint8_t addr, uint16_t data)
{
    HAL_StatusTypeDef ret;
    uint8_t *ptr;

    ptr = (uint8_t *)&data;
    //printf("%x  %x\r\n", *ptr, *(ptr+1));
    ret = HAL_I2C_Mem_Write(&hi2c2, RT9426_I2C_ADDR, addr, I2C_MEMADD_SIZE_8BIT, ptr, RT9426_READ_LEN, HAL_MAX_DELAY);
    //printf("WR:ret=%d data=0x%04x\r\n", ret, data);

    return ret;

}
uint8_t Gauge_Read_Register(uint8_t addr, uint16_t *pdata)
{
    HAL_StatusTypeDef ret;
    uint8_t buf[2];

    ret = HAL_I2C_Mem_Read(&hi2c2, RT9426_I2C_ADDR, addr, I2C_MEMADD_SIZE_8BIT, buf, RT9426_READ_LEN, HAL_MAX_DELAY);
    *pdata = buf[1]<<8 | buf[0];
    //printf("RD:ret=%d buf[0]=0x%02x  buf[1]=0x%02x *pdata=0x%04x\r\n", ret, buf[0], buf[1], *pdata);

    return ret;
}

//add end

/*---------------------------------------------------------------*/
/*-------Richtek RT9426 Driver Sample Code V1.1.4 20190726-------*/
/*---------------------------------------------------------------*/


#define REG_CNTL                   (0x00)
#define REG_RSVD                   (0x02)
#define REG_CURR                   (0x04)
#define REG_TEMP                   (0x06)
#define REG_VBAT                   (0x08)
#define REG_FLAG1                  (0x0A)
#define REG_FLAG2                  (0x0C)
#define REG_RM                     (0x10)
#define REG_FCC                    (0x12)
#define REG_DUMMY                  (0x1E)
#define REG_INTT                   (0x28)
#define REG_CYC                    (0x2A)
#define REG_SOC                    (0x2C)
#define REG_SOH                    (0x2E)
#define REG_VER                    (0x20)
#define REG_FLAG3                  (0x30)
#define REG_IRQ                    (0x36)
#define REG_DC                     (0x3C)
#define REG_BDCNTL                 (0x3E)
#define REG_CMD3F                  (0x3F)
#define REG_SWINDOW1               (0x40)
#define REG_SWINDOW2               (0x42)
#define REG_SWINDOW3               (0x44)
#define REG_SWINDOW4               (0x46)
#define REG_SWINDOW5               (0x48)
#define REG_SWINDOW6               (0x4A)
#define REG_SWINDOW7               (0x4C)
#define REG_SWINDOW8               (0x4E)
#define REG_CMD60                  (0x60)
#define REG_CMD61                  (0x61)
#define REG_CMD78                  (0x78)

#define MASK_REG_FLAG3_BSC_ACS     (0x0001)
#define MASK_REG_FLAG2_SLP_STS 	   (0x8000)
#define MASK_REG_FLAG2_SHDN_STS    (0x4000)
#define SWVER                      (0x0002)
uint8_t I2C_Slave_Address = 0xAA;
uint32_t g_basic_key = 0x12345678;
uint32_t g_advance_key = 0xFFFFFFFF;
uint16_t g_rt9426_update_info[16]={0};

//add huangcheng 2025-03-05
void rt9426_printf_reg(void)
{
    const char *reg_str[16] = {"System_Time", "REG_VBAT", "REG_CURR", "REG_TEMP", "REG_INTT", "REG_SOC", "REG_RM", \
        "REG_SOH", "REG_DC", "REG_FCC", "REG_FLAG1", "REG_FLAG2", "REG_FLAG3", "REG_IRQ"};

    for(uint8_t loop=0;loop<16;loop++)
        RT9426_PRINTF("%s: 0x%04x", reg_str[loop], g_rt9426_update_info[loop]);
}
//add end

int32_t RT9426_Initial(void)
{
   /*---------------Define RT9426 Initail Setting----------------------*/
    #define RT9426_CFG_VERSION         (0x01)
    #define RT9426_BATTERY_TYPE        (4354)
    #define RT9426_DC                  (134)
    #define RT9426_FCC                 (134)
    #define RT9426_FC_VTH              (0x8E)
    #define RT9426_FC_ITH              (0x01)
    #define RT9426_FC_STH              (0x05)
    #define RT9426_FD_VTH              (0x66)
    #define RT9426_OPCFG1              (0x7480)
    #define RT9426_OPCFG2              (0x0040)
    #define RT9426_OPCFG3              (0x3EFB)
    #define RT9426_OPCFG4              (0x2001)
    #define RT9426_OPCFG5              (0x067F)
    #define RT9426_OTC_TTH             (0x0064)
    #define RT9426_OTC_CHG_ITH         (0x0B5F)
    #define RT9426_OTD_TTH             (0x0064)
    #define RT9426_OTD_DCHG_ITH        (0x0B5F)
    #define RT9426_UVOV_TH             (0x00FF)
    #define RT9426_US_TH               (0x00)
    #define RT9426_CURR_DB             (0x02)
    #define RT9426_QS_EN               (0x01)
	#define RT9426_FC_TIME_SOCTH       (0x41)

    WORD RT9426_EXTREG_SIZE=29;

    const WORD RT9426_EXTREG_DATA[87]={
                0x1, 0x4E, 0x0806,
                0x2, 0x4E, 0x4404,
                0x3, 0x4E, 0xB706,
                0x5, 0x46, 0x4105,
                0x5, 0x48, 0x0204,
                0x5, 0x4C, 0x3223,
                0x6, 0x40, 0x8813,
                0x6, 0x42, 0x8224,
                0x6, 0x44, 0xAD77,
                0x6, 0x48, 0xA312,
                0x6, 0x4A, 0x6852,
                0x6, 0x4C, 0x6457,
                0x6, 0x4E, 0x041E,
                0x7, 0x40, 0x131E,
                0x7, 0x42, 0x0405,
                0x7, 0x44, 0x0404,
                0x7, 0x46, 0x0202,
                0x7, 0x48, 0x0101,
                0x7, 0x4A, 0x0202,
                0x7, 0x4C, 0x0002,
                0x7, 0x4E, 0xFF00,
                0x8, 0x40, 0xFAFD,
                0x8, 0x42, 0xFAFA,
                0x8, 0x44, 0xF9F9,
                0x8, 0x46, 0xEFF4,
                0x8, 0x48, 0xFAFA,
                0x8, 0x4A, 0xF8FA,
                0x8, 0x4C, 0xE9F8,
                0x8, 0x4E, 0x00D5};
				
    const WORD RT9426_USR_OCV_TABLE[80]={
                    0x0013, 0x44C0, 0x4B00, 0x561A, 0x59D3, 0x5C0D, 0x5C93, 0x5DC6,
                    0x5F4D, 0x6020, 0x60D3, 0x616D, 0x623A, 0x64BA, 0x66CD, 0x67FA,
                    0x691A, 0x6A5A, 0x6BAD, 0x706D, 0x1900, 0x0B1A, 0x02DE, 0x0258,
                    0x0022, 0x0040, 0x0030, 0x0032, 0x0042, 0x0053, 0x0075, 0x007F,
                    0x0082, 0x007B, 0x0089, 0x008E, 0x0062, 0x01E6, 0x0013, 0xFF80,
                    0x0000, 0x0200, 0x049A, 0x0680, 0x0E4D, 0x17E6, 0x281A, 0x309A,
                    0x3600, 0x39B3, 0x3D33, 0x474D, 0x4F80, 0x5466, 0x589A, 0x5D1A,
                    0x6400, 0x6900, 0x000A, 0x0017, 0x0059, 0x006D, 0x076E, 0x0400,
                    0x0550, 0x0527, 0x03DB, 0x0315, 0x0230, 0x0205, 0x01FA, 0x0216,
                    0x01DE, 0x01CD, 0x029B, 0x0087, 0xFF3D, 0x0000, 0x0000, 0x0000
                    };


    uint16_t loop=0,retry_times=0;
    uint16_t i,j;
    uint16_t regval=0;
    uint16_t sts=0;
    uint16_t count=0;
    uint16_t SLP_count=0;


    /*------------------------Unseal RT9426----------------------------*/
    while(1)
    {
        count++;
        if(Gauge_Write_Register(REG_CNTL,g_basic_key&0xFFFF)==0)
        {
            Delay_msecond(DELAY_1MS);
            if(Gauge_Write_Register(REG_CNTL,g_basic_key>>16)==0)
            {
                Gauge_Write_Register(REG_DUMMY,0x0000);
                Delay_msecond(DELAY_1MS);

                Gauge_Read_Register(REG_FLAG3, &sts);
                if(sts&MASK_REG_FLAG3_BSC_ACS)
                {
                    RT9426_PRINTF("RT9426_Unseal_Pass");
                    break;
                }
            }
        }
        if(count>5)
        {
            RT9426_PRINTF("RT9426_Unseal_Fail");
            return -1;
        }
        Delay_msecond(DELAY_10MS);
    }
    /*------------------------Unseal RT9426----------------------------*/

    /*---------------Read Reg IRQ to reinital Alert pin state------------------*/
    Gauge_Read_Register(REG_IRQ, &regval);
    /*---------------Read Reg IRQ to reinital Alert pin state------------------*/

    /*---------------------Set User Define OCV Table--------------------------*/
    if (RT9426_USR_OCV_TABLE[0] == 0x0013){
        retry_times = 3;
        while (retry_times) {
            for (i = 0; i < 9; i++) {
                Gauge_Write_Register(REG_BDCNTL,0xCB00+i);
                Gauge_Write_Register(REG_BDCNTL,0xCB00+i);
                Delay_msecond(DELAY_1MS);
                for (j = 0; j < 8; j++) {
                    Gauge_Write_Register(0x40+j*2,RT9426_USR_OCV_TABLE[i*8+j]);
                    Delay_msecond(DELAY_1MS);
                }
            }

            Gauge_Write_Register(REG_BDCNTL,0xCB09);
            Gauge_Write_Register(REG_BDCNTL,0xCB09);
            Delay_msecond(DELAY_1MS);
            for (i = 0; i < 5; i++) {
                Gauge_Write_Register(0x40+i*2,RT9426_USR_OCV_TABLE[72+i]);
                Delay_msecond(DELAY_1MS);
            }
            Gauge_Write_Register(REG_DUMMY,0x0000);
            Delay_msecond(DELAY_10MS);
            Gauge_Read_Register(REG_FLAG2, &regval);
            if (regval & 0x0800) {
                RT9426_PRINTF("RT9426_OCV_Setting_Pass");
                break;
            }
            retry_times--;

            if (retry_times == 0)
            {
                RT9426_PRINTF("RT9426_OCV_Setting_Fail");
                return -1;
            }
        }
    }
    /*---------------------Set User Define OCV Table--------------------------*/

    /*------------------------Set Alert Threshold----------------------------*/
    Gauge_Write_Register(REG_BDCNTL,0x6553);
    Gauge_Write_Register(REG_BDCNTL,0x6553);
    Delay_msecond(DELAY_1MS);
    Gauge_Write_Register(REG_SWINDOW1,RT9426_OTC_TTH);
    Delay_msecond(DELAY_1MS);
    Gauge_Write_Register(REG_SWINDOW2,RT9426_OTC_CHG_ITH);
    Delay_msecond(DELAY_1MS);
    Gauge_Write_Register(REG_SWINDOW3,RT9426_OTD_TTH);
    Delay_msecond(DELAY_1MS);
    Gauge_Write_Register(REG_SWINDOW4,RT9426_OTD_DCHG_ITH);
    Delay_msecond(DELAY_1MS);
    Gauge_Write_Register(REG_SWINDOW5,RT9426_UVOV_TH);
    Delay_msecond(DELAY_1MS);
    Gauge_Write_Register(REG_SWINDOW6,((0x4600)|(RT9426_US_TH)));
    Gauge_Write_Register(REG_DUMMY,0x0000);
    Delay_msecond(DELAY_1MS);
    /*------------------------Set Alert Threshold----------------------------*/


    /*-----------------------Set Default OCV Table---------------------------*/
    Gauge_Write_Register(REG_BDCNTL,0x6552);
    Gauge_Write_Register(REG_BDCNTL,0x6552);
    Delay_msecond(DELAY_1MS);
    if(RT9426_BATTERY_TYPE==4400)
    {
        Gauge_Write_Register(REG_SWINDOW1,0x8200);
    }
    else if(RT9426_BATTERY_TYPE==4352)
    {
        Gauge_Write_Register(REG_SWINDOW1,0x8100);
    }
    else if(RT9426_BATTERY_TYPE==4354)
    {
        Gauge_Write_Register(REG_SWINDOW1,0x8300);
    }
    else if(RT9426_BATTERY_TYPE==4200)
    {
        Gauge_Write_Register(REG_SWINDOW1,0x8000);
    }
    else
    {
        Gauge_Write_Register(REG_SWINDOW1,0x8100);
    }
    Gauge_Write_Register(REG_DUMMY,0x0000);
    Delay_msecond(DELAY_1MS);
    /*-----------------------Set Default OCV Table---------------------------*/

    /*-------------------------Set OP CONFIG------------------------------*/
    Gauge_Write_Register(REG_BDCNTL,0x6551);
    Gauge_Write_Register(REG_BDCNTL,0x6551);
    Delay_msecond(DELAY_1MS);
    Gauge_Write_Register(REG_SWINDOW1,RT9426_OPCFG1);
    Delay_msecond(DELAY_1MS);
    Gauge_Write_Register(REG_SWINDOW2,RT9426_OPCFG2);
    Delay_msecond(DELAY_1MS);
    Gauge_Write_Register(REG_SWINDOW3,RT9426_OPCFG3);
    Delay_msecond(DELAY_1MS);
    Gauge_Write_Register(REG_SWINDOW4,RT9426_OPCFG4);
    Delay_msecond(DELAY_1MS);
    Gauge_Write_Register(REG_SWINDOW5,RT9426_OPCFG5);
    Gauge_Write_Register(REG_DUMMY,0x0000);
    Delay_msecond(DELAY_1MS);
    /*-------------------------Set OP CONFIG------------------------------*/

    /*----------------------Set Current Deadband----------------------------*/
    Gauge_Write_Register(REG_BDCNTL,0x6551);
    Gauge_Write_Register(REG_BDCNTL,0x6551);
    Delay_msecond(DELAY_1MS);
    Gauge_Write_Register(REG_SWINDOW7, ((0x0012) | (RT9426_CURR_DB << 8 )));
    Gauge_Write_Register(REG_DUMMY,0x0000);
    Delay_msecond(DELAY_1MS);
    /*----------------------Set Current Deadband----------------------------*/

    /*-----------------------Set FC FD Threshold-----------------------------*/
    regval=(RT9426_FC_VTH) | (RT9426_FC_ITH << 8 );
    Gauge_Write_Register(REG_BDCNTL,0x6555);
    Gauge_Write_Register(REG_BDCNTL,0x6555);
    Delay_msecond(DELAY_1MS);
    Gauge_Write_Register(REG_SWINDOW3,regval);
    Delay_msecond(DELAY_1MS);
    Gauge_Write_Register(REG_SWINDOW4,(0x4100|RT9426_FC_STH));
    Delay_msecond(DELAY_1MS);
    Gauge_Write_Register(REG_SWINDOW6,(0x1200|RT9426_FD_VTH));
    Gauge_Write_Register(REG_DUMMY,0x0000);
    Delay_msecond(DELAY_1MS);
    /*-----------------------Set FC FD Threshold-----------------------------*/

    /*--------------------------Set DC & FCC-------------------------------*/
    Gauge_Write_Register(REG_BDCNTL,0x6552);
    Gauge_Write_Register(REG_BDCNTL,0x6552);
    Delay_msecond(DELAY_1MS);
    Gauge_Write_Register(REG_SWINDOW6,RT9426_DC);
    Delay_msecond(DELAY_1MS);
    Gauge_Write_Register(REG_SWINDOW7,RT9426_FCC);
    Gauge_Write_Register(REG_DUMMY,0x0000);
    Delay_msecond(DELAY_1MS);

    /*--------------------------Set DC & FCC-------------------------------*/

    /*--------------------------Set Ext Register-----------------------------*/
    if(RT9426_EXTREG_SIZE!=0){
        for (loop = 0; loop < RT9426_EXTREG_SIZE; loop++){
            Gauge_Write_Register(REG_BDCNTL,0x6550+(RT9426_EXTREG_DATA[loop*3]));
            Gauge_Write_Register(REG_BDCNTL,0x6550+(RT9426_EXTREG_DATA[loop*3]));
            Delay_msecond(DELAY_1MS);
            Gauge_Write_Register(RT9426_EXTREG_DATA[(loop*3)+1],RT9426_EXTREG_DATA[(loop*3)+2]);
            Delay_msecond(DELAY_1MS);
        }
        Gauge_Write_Register(REG_DUMMY,0x0000);
    }
    /*--------------------------Set Ext Register-----------------------------*/

	/*-------------------------Read Back Check Register-----------------------*/
	Gauge_Write_Register(REG_BDCNTL,0x6501);
	Gauge_Write_Register(REG_BDCNTL,0x6501);
	Delay_msecond(DELAY_5MS);
	Gauge_Read_Register(REG_SWINDOW1,&regval);
	if (regval != RT9426_OPCFG1) {
	  return -1;
	}
	Gauge_Read_Register(REG_SWINDOW2,&regval);
	if (regval != RT9426_OPCFG2) {
	  return -1;
	}
	Gauge_Read_Register(REG_SWINDOW3,&regval);
	if (regval != RT9426_OPCFG3) {
	  return -1;
	}
	Gauge_Read_Register(REG_SWINDOW4,&regval);
	if (regval != RT9426_OPCFG4) {
	  return -1;
	}
	Gauge_Read_Register(REG_SWINDOW5,&regval);
	if (regval != RT9426_OPCFG5) {
	  return -1;
	}
	Gauge_Read_Register(REG_SWINDOW7,&regval);
	if (regval != ((0x0012) | (RT9426_CURR_DB<<8))) {
	  return -1;
	}

	Gauge_Write_Register(REG_BDCNTL,0x6502);  
    Gauge_Write_Register(REG_BDCNTL,0x6502);  
    Delay_msecond(DELAY_5MS);
	
    Gauge_Read_Register(REG_SWINDOW6,&regval); 
	if (regval != RT9426_DC) {
		return -1;
	}
	Gauge_Read_Register(REG_SWINDOW7,&regval); 
	if (regval != RT9426_FCC) {
		return -1;
	}

	Gauge_Write_Register(REG_BDCNTL,0x6503);  
    Gauge_Write_Register(REG_BDCNTL,0x6503);  
    Delay_msecond(DELAY_5MS);
    Gauge_Read_Register(REG_SWINDOW1,&regval); 
	if (regval != RT9426_OTC_TTH) {
		return -1;
	}
	Gauge_Read_Register(REG_SWINDOW2,&regval); 
	if (regval != RT9426_OTC_CHG_ITH) {
		return -1;
	}
	Gauge_Read_Register(REG_SWINDOW3,&regval); 
	if (regval != RT9426_OTD_TTH) {
		return -1;
	}
	Gauge_Read_Register(REG_SWINDOW4,&regval); 
	if (regval != RT9426_OTD_DCHG_ITH) {
		return -1;
	}
	Gauge_Read_Register(REG_SWINDOW5,&regval); 
	if (regval != RT9426_UVOV_TH) {
		return -1;
	}
	Gauge_Read_Register(REG_SWINDOW6,&regval); 
	if (regval != ((0x4600) | (RT9426_US_TH))) {
		return -1;
	}

	Gauge_Write_Register(REG_BDCNTL,0x6505);  
    Gauge_Write_Register(REG_BDCNTL,0x6505);  
    Delay_msecond(DELAY_5MS);
    Gauge_Read_Register(REG_SWINDOW3,&regval); 
	if (regval != ((RT9426_FC_VTH) | (RT9426_FC_ITH << 8 ))) {
		return -1;
	}
	Gauge_Read_Register(REG_SWINDOW4,&regval); 
	if (regval != ((RT9426_FC_TIME_SOCTH<<8) | RT9426_FC_STH)) {
		return -1;
	}
	Gauge_Read_Register(REG_SWINDOW6,&regval); 
	if (regval != (0x1200|RT9426_FD_VTH)) {
		return -1;
	}

	if (RT9426_EXTREG_SIZE != 0) {
		for (loop = 0; loop < RT9426_EXTREG_SIZE; loop++) {		
			Gauge_Write_Register(REG_BDCNTL,0x6500+(RT9426_EXTREG_DATA[loop*3]));		
			Gauge_Write_Register(REG_BDCNTL,0x6500+(RT9426_EXTREG_DATA[loop*3])); 		
			Delay_msecond(DELAY_5MS);
			Gauge_Read_Register(RT9426_EXTREG_DATA[(loop*3)+1],&regval); 
			if (regval != RT9426_EXTREG_DATA[(loop*3)+2]) {
				return -1;
			}
		}
	}
	/*-------------------------Read Back Check Register-----------------------*/

    /*---------------------------Quick Start--------------------------------*/
    if(RT9426_QS_EN == 1)
    {
        Gauge_Write_Register(REG_CNTL,0x4000);
        Gauge_Write_Register(REG_DUMMY,0x0000);
        Delay_msecond(DELAY_5MS);
    }
    /*---------------------------Quick Start--------------------------------*/

    /*-----------------------------Clear RI--------------------------------*/
    Gauge_Read_Register(REG_FLAG3, &regval);
    regval = (regval & ~0x0100);
    Gauge_Write_Register(REG_FLAG3,regval);
    Gauge_Write_Register(REG_DUMMY,0x0000);
    Delay_msecond(DELAY_1MS);
    /*-----------------------------Clear RI--------------------------------*/

    /*------------------------Seal RT9426----------------------------*/
    Gauge_Write_Register(REG_CNTL,0x0020);
    Gauge_Write_Register(REG_DUMMY,0x0000);
    Delay_msecond(DELAY_5MS);

    // Gauge_Read_Register(REG_FLAG3, &sts);
    // if(sts&0x0001)
        // Seal Fail
    // else
        // Seal Pass


    /*------------------------Seal RT9426----------------------------*/

    return 0;
}

void RT9426_Update_Info(void)
{
    uint8_t  loop=0;  //uint8_t BYTE loop=0;
    uint16_t regval=0; //uint16_t WORD regval=0;

    Gauge_Read_Register(REG_FLAG3, &regval); //-- check flag3, if RI=1, reinitial IC
    RT9426_PRINTF("\n RT9426_REG_FLAG3 = %04x\r",regval);
    if (regval & 0x0100) {
        Delay_msecond(DELAY_5MS);
        Gauge_Read_Register(REG_FLAG3, &regval); //-- check flag3, if RI=1, reinitial IC
        RT9426_PRINTF("\n RT9426_REG_FLAG3 = %04x\r",regval);
        if(regval & 0x0100) {
          rt9426_main();
        }
    }

    for(loop=0;loop<16;loop++)
        g_rt9426_update_info[loop] = 0;

    //g_rt9426_update_info[0] = System_Time;    //huangcheng 2025-03-05 cancel
    Gauge_Read_Register(REG_VBAT, &g_rt9426_update_info[1]);
    Gauge_Read_Register(REG_CURR, &g_rt9426_update_info[2]);
    Gauge_Read_Register(REG_TEMP, &g_rt9426_update_info[3]);
    Gauge_Read_Register(REG_INTT, &g_rt9426_update_info[4]);
    Gauge_Read_Register(REG_SOC, &g_rt9426_update_info[5]);
    Gauge_Read_Register(REG_RM, &g_rt9426_update_info[6]);
    Gauge_Read_Register(REG_SOH, &g_rt9426_update_info[7]);
    Gauge_Read_Register(REG_DC, &g_rt9426_update_info[8]);
    Gauge_Read_Register(REG_FCC, &g_rt9426_update_info[9]);
    Gauge_Read_Register(REG_FLAG1, &g_rt9426_update_info[10]);
    Gauge_Read_Register(REG_FLAG2, &g_rt9426_update_info[11]);
    Gauge_Read_Register(REG_FLAG3, &g_rt9426_update_info[12]);
    Gauge_Read_Register(REG_IRQ, &g_rt9426_update_info[13]);

    rt9426_printf_reg();    //add huangcheng 2025-03-05
}



/*void RT9426_Entry_Shutdown(void)
{
	uint8_t SHDN_count =0;
	uint16_t sts=0;
   	Gauge_Read_Register(REG_FLAG2, &sts);
	if(sts&MASK_REG_FLAG2_SHDN_STS)
    {
        RT9426_PRINTF("RT9426_at_Shutdown_mode");
    }
    else
	{
		while(1)
		{
			SHDN_count++
	        Gauge_Write_Register(REG_CNTL,0x64AA);
            Gauge_Write_Register(REG_DUMMY,0x0000);
            Delay_msecond(DELAY_10MS);
			Gauge_Read_Register(REG_FLAG2, &sts);
			if(sts&MASK_REG_FLAG2_SHDN_STS)
			{
				RT9426_PRINTF("RT9426_Entry_Shutdown_Pass");
				break;
			}
			if (SHDN_count >3)
			{
                RT9426_PRINTF("RT9426_Entry_Shutdown_error");
                return -1;
            }
        }
	 }
}

void RT9426_Exit_Shutdown(void)
{
	uint8_t SHDN_count =0;
	uint16_t sts=0;
	Gauge_Read_Register(REG_FLAG2, &sts);
	if(sts&MASK_REG_FLAG2_SHDN_STS)
    {
        wihie(1)
		{
			SHDN_count++
			Gauge_Write_Register(REG_CNTL,0x6400);
			Gauge_Write_Register(REG_DUMMY,0x0000);
			Delay_msecond(DELAY_5MS);
		    Gauge_Read_Register(REG_FLAG2, &sts);
			if((sts&MASK_REG_FLAG2_SHDN_STS)==0)
			{
				Delay_msecond(DELAY_250MS);
				Gauge_Write_Register(REG_CNTL,0x4000);
				Gauge_Write_Register(REG_DUMMY,0x0000);
				Delay_msecond(DELAY_5MS);
				RT9426_PRINTF("RT9426_Exit_SHDN_Pass")
				break;
			}
			if (SHDN_count>3)
			{
				RT9426_PRINTF("RT9426_Exit_SHDN_error");
				return -1;
			}
		}
    }
}*/
int32_t rt9426_main(void)
{
    uint16_t regval=0;
    uint16_t count=0;
    int32_t sts=0;

    /*-----------------------Standard initial-----------------------*/
    while(1)
    {
        count++;
        Gauge_Read_Register(REG_FLAG2, &regval); //--check RDY
        if(regval & 0x0080)
        {
            Delay_msecond(DELAY_60MS);
            Gauge_Read_Register(REG_FLAG3, &regval); //--check RI
            if(regval & 0x0100)
            {
                sts = RT9426_Initial();
                if(sts==0)
                {
                    RT9426_PRINTF("RT9426_init_successful");
                }
                else
                {
                    RT9426_PRINTF("RT9426_init_fail");
                    return -1;
                }
                Gauge_Write_Register(REG_RSVD,SWVER); //Record SW VERSION to RSVD Register
                Gauge_Write_Register(REG_DUMMY,0x0000);
                Delay_msecond(DELAY_5MS);
                break;
            }
            else
            {
                RT9426_PRINTF("No_Need_to_reinitil_RT9426");
                break;
            }
        }
        if(count>20)
        {
            RT9426_PRINTF("RT9426_init_error");
            return -1;
        }
        Delay_msecond(DELAY_20MS);
    }
    /*-----------------------Standard initial-----------------------*/
    count=0;


    /*------------------------Force initial--------------------------*/
    Gauge_Read_Register(REG_RSVD, &regval);
    if(regval!=(SWVER))                    //Compare Present SWVER & Previous SWVER
    {
        /* Depends on differnet SW update condition to Reset RT9426 or not */
            // Gauge_Write_Register(REG_CNTL,0x0041); //---reset RT9426
            // Delay_msecond(DELAY_1000MS);
        /* Depends on differnet SW update condition to Reset RT9426 or not */
        while(1)
        {
            count++;
            Gauge_Read_Register(REG_FLAG2, &regval); //--check RDY
            if(regval & 0x0080)
            {
                Delay_msecond(DELAY_60MS);
				sts = RT9426_Initial();
                if(sts==0)
                {
                    RT9426_PRINTF("RT9426_init_successful");
                }
                else
                {
                    RT9426_PRINTF("RT9426_init_fail");
                    return -1;
                }
                Gauge_Write_Register(REG_RSVD,SWVER);   //Record SW VERSION to RSVD Register
                Gauge_Write_Register(REG_DUMMY,0x0000);
                Delay_msecond(DELAY_5MS);
                break;
            }
            if(count>20)
            {
                RT9426_PRINTF("RT9426_init_error");
                return -1;
            }
            Delay_msecond(DELAY_20MS);
        }
    }
    /*------------------------Force initial-------------------------*/



    return 0;
}

/*********RT9426 Enter Shutdown Mode***********/
int32_t RT9426_Enter_Shutdown_Mode(void)
{
  uint16_t regval;
  uint8_t loop;
  for(loop=0;loop<5;loop++)
  {
    Gauge_Read_Register(REG_FLAG2, &regval);
    if((regval&0x4000)==0)
    {
      Gauge_Write_Register(REG_CNTL,0x64AA);
      Gauge_Write_Register(REG_DUMMY,0x0000);
      Delay_msecond(DELAY_10MS);
    }
    else
    {
      break;
    }
  }
  Gauge_Read_Register(REG_FLAG2, &regval);
  if((regval&0x4000)==0)
  {
    RT9426_PRINTF("RT9426 Enter Shutdown Fail\n");
    return -1;
  }
  else
  {
    return 0;
  }
}


/*********RT9426 Exit Shutdown Mode***********/
int32_t RT9426_Exit_Shutdown_Mode(void)
{
  uint16_t regval;
  uint8_t loop;
  for(loop=0;loop<5;loop++)
  {
    Gauge_Read_Register(REG_FLAG2, &regval);
    if((regval&0x4000)==1)
    {
      Gauge_Write_Register(REG_CNTL,0x6400);
      Gauge_Write_Register(REG_DUMMY,0x0000);
      Delay_msecond(DELAY_10MS);
    }
    else
    {
      break;
    }
  }
  Gauge_Read_Register(REG_FLAG2, &regval);
  if((regval&0x4000)==1)
  {
    RT9426_PRINTF("RT9426 Exit Shutdown Fail\n");
    return -1;
  }
  else
  {
    return 0;
  }
}
