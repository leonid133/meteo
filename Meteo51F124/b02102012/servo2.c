#include "c8051f120.h"
#include "init.h"
#include "comport.h"
#include <intrins.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "servo.h"                     

#define SYSCLK 24500000 //Hz
#define FREQT0 8006 //Hz

//����������
sbit LED = P1^6;
sbit LED2 = P2^6;
sbit BUTTON = P3^7;
xdata unsigned char InitFlag;
//--------------
//���
sbit LIR_DATA = P2^2; //P2^0;
sbit LIR_SCK = P2^1;

xdata unsigned char F;
xdata unsigned int delta, LirData, LirTarget, Fdelta;

typedef union INT {                   
   unsigned int i;
   unsigned char b[2];
} INT;
//---
//�����
xdata unsigned char VeterFlag = 0;
sbit WSpeed = P1^1;
sbit WAngle = P1^3;

xdata float Vspeed, Angle;
xdata unsigned long RTC, RTCV, RTCA;
xdata unsigned long TV,TA;

typedef union FLOAT {                   
   float f;
   unsigned char b[4];
} FLOAT;
//-----
//UART0
#define NBFM 		50
xdata unsigned char BuferFromModem [NBFM]; 
xdata unsigned char wBFM, rBFM, marBFM;
xdata unsigned int u0timer;

#define SIZE_BUFFER0 		50
xdata char BufferInModem[SIZE_BUFFER0]; // ��� �������� � ���������������� ����
xdata int r0, rk;
bit flTransmiter;	

//SMBus
#define WRITE 0x00                  // SMBus WRITE command
#define READ  0x01                  // SMBus READ command
// Device addresses (7 bits, lsb is a don't care)
#define CHIP_A 0x00                 // Device address for chip A
#define CHIP_B 0x05                 // Device address for chip B
#define CHIP_C 0xA4                 // Device address for chip C

// SMBus states:
// MT = Master Transmitter
// MR = Master Receiver
#define  SMB_BUS_ERROR  0x00        // (all modes) BUS ERROR
#define  SMB_START      0x08        // (MT & MR) START transmitted
#define  SMB_RP_START   0x10        // (MT & MR) repeated START
#define  SMB_MTADDACK   0x18        // (MT) Slave address + W transmitted;
                                    //  ACK received
#define  SMB_MTADDNACK  0x20        // (MT) Slave address + W transmitted;
                                    //  NACK received
#define  SMB_MTDBACK    0x28        // (MT) data byte transmitted; ACK rec'vd
#define  SMB_MTDBNACK   0x30        // (MT) data byte transmitted; NACK rec'vd
#define  SMB_MTARBLOST  0x38        // (MT) arbitration lost
#define  SMB_MRADDACK   0x40        // (MR) Slave address + R transmitted;
                                    //  ACK received
#define  SMB_MRADDNACK  0x48        // (MR) Slave address + R transmitted;
                                    //  NACK received
#define  SMB_MRDBACK    0x50        // (MR) data byte rec'vd; ACK transmitted
#define  SMB_MRDBNACK   0x58        // (MR) data byte rec'vd; NACK transmitted

char COMMAND;                       // Holds the slave address + R/W bit for
                                    // use in the SMBus ISR.

char WORD;                          // Holds data to be transmitted by the SMBus
                                    // OR data that has just been received.

char BYTE_NUMBER;                   // Used by ISR to check what data has just been
                                    // sent - High address byte, Low byte, or data
                                    // byte

unsigned char HIGH_ADD, LOW_ADD;    // High & Low byte for EEPROM memory address

bit SM_BUSY;                        // This bit is set when a send or receive
                                    // is started. It is cleared by the
                                    // ISR when the operation is finished.

xdata unsigned int Humidity, Temperature, i_measure;

//GPS
#define NS 	75
xdata unsigned int GPStimepulse;
xdata char mess [NS], r, w, mar;		// ��� ������� ������� GPS
bit flNewGPS;
xdata unsigned int Vz, koors, flNoKoord, liTimer_tick_GPS, liTimer_tick;
xdata unsigned long LatFly, LonFly;

/***********************************************************************************
Project:          SHT1x/7x demo program (V2.4)
Filename:         SHT1x_sample_code.c    

Prozessor:        80C51 family
Compiler:         Keil Version 6.23a

Autor:            MST
Copyrigth:        (c) Sensirion AG      
***********************************************************************************/
// Revisions:
// V2.4	 calc_sht11()       Coefficients for humidity and temperature conversion 
//                          changed (for V4 sensors)
//       calc_dewpoint()	New formula for dew point calculation 
#include <math.h>    //Keil library 

typedef union 
{ unsigned int i;
  float f;
} value;

//----------------------------------------------------------------------------------
// modul-var
//----------------------------------------------------------------------------------
enum {TEMP,HUMI};

#define	DATA   	LED
#define	SCK   	LED2

#define noACK 0
#define ACK   1
                            //adr  command  r/w
#define STATUS_REG_W 0x06   //000   0011    0
#define STATUS_REG_R 0x07   //000   0011    1
#define MEASURE_TEMP 0x03   //000   0001    1
#define MEASURE_HUMI 0x05   //000   0010    1
#define RESET        0x1e   //000   1111    0

//----------------------------------------------------------------------------------
char s_write_byte(unsigned char value)
//----------------------------------------------------------------------------------
// writes a byte on the Sensibus and checks the acknowledge 
{ 
  unsigned char i,error=0;  
  for (i=0x80;i>0;i/=2)             //shift bit for masking
  { if (i & value) DATA=1;          //masking value with i , write to SENSI-BUS
    else DATA=0;                        
    _nop_();                        //observe setup time
    SCK=1;                          //clk for SENSI-BUS
    _nop_();_nop_();_nop_();        //pulswith approx. 5 us  	
    SCK=0;
    _nop_();                         //observe hold time
  }
  DATA=1;                           //release DATA-line
  _nop_();                          //observe setup time
  SCK=1;                            //clk #9 for ack 
  error=DATA;                       //check ack (DATA will be pulled down by SHT11)
  SCK=0;        
  return error;                     //error=1 in case of no acknowledge
}

//----------------------------------------------------------------------------------
char s_read_byte(unsigned char ack)
//----------------------------------------------------------------------------------
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1" 
{ 
  unsigned char i,val=0;
  DATA=1;                           //release DATA-line
  for (i=0x80;i>0;i/=2)             //shift bit for masking
  { SCK=1;                          //clk for SENSI-BUS
    if (DATA) val=(val | i);        //read bit  
    SCK=0;  					 
  }
  DATA=!ack;                        //in case of "ack==1" pull down DATA-Line
  _nop_();                          //observe setup time
  SCK=1;                            //clk #9 for ack
  _nop_();_nop_();_nop_();          //pulswith approx. 5 us 
  SCK=0;
  _nop_();                          //observe hold time						    
  DATA=1;                           //release DATA-line
  return val;
}

//----------------------------------------------------------------------------------
void s_transstart(void)
//----------------------------------------------------------------------------------
// generates a transmission start 
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
{  
   DATA=1; SCK=0;                   //Initial state
   _nop_();
   SCK=1;
   _nop_();
   DATA=0;
   _nop_();
   SCK=0;  
   _nop_();_nop_();_nop_();
   SCK=1;
   _nop_();
   DATA=1;		   
   _nop_();
   SCK=0;		   
}

//----------------------------------------------------------------------------------
void s_connectionreset(void)
//----------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
{  
  unsigned char i; 
  DATA=1; SCK=0;                    //Initial state
  for(i=0;i<9;i++)                  //9 SCK cycles
  { SCK=1;
    SCK=0;
  }
  s_transstart();                   //transmission start
}

//----------------------------------------------------------------------------------
char s_softreset(void)
//----------------------------------------------------------------------------------
// resets the sensor by a softreset 
{ 
  unsigned char error=0;  
  s_connectionreset();              //reset communication
  error+=s_write_byte(RESET);       //send RESET-command to sensor
  return error;                     //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char s_read_statusreg(unsigned char *p_value, unsigned char *p_checksum)
//----------------------------------------------------------------------------------
// reads the status register with checksum (8-bit)
{ 
  unsigned char error=0;
  s_transstart();                   //transmission start
  error=s_write_byte(STATUS_REG_R); //send command to sensor
  *p_value=s_read_byte(ACK);        //read status register (8-bit)
  *p_checksum=s_read_byte(noACK);   //read checksum (8-bit)  
  return error;                     //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char s_write_statusreg(unsigned char *p_value)
//----------------------------------------------------------------------------------
// writes the status register with checksum (8-bit)
{ 
  unsigned char error=0;
  s_transstart();                   //transmission start
  error+=s_write_byte(STATUS_REG_W);//send command to sensor
  error+=s_write_byte(*p_value);    //send value of status register
  return error;                     //error>=1 in case of no response form the sensor
}
 							   
//----------------------------------------------------------------------------------
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode)
//----------------------------------------------------------------------------------
// makes a measurement (humidity/temperature) with checksum
{ 
  unsigned char error=0;
  unsigned int i;

  s_transstart();                   //transmission start
  switch(mode){                     //send command to sensor
    case TEMP	: error+=s_write_byte(MEASURE_TEMP); break;
    case HUMI	: error+=s_write_byte(MEASURE_HUMI); break;
    default     : break;	 
  }
  for (i=0;i<65535;i++) if(DATA==0) break; //wait until sensor has finished the measurement
  if(DATA) error+=1;                // or timeout (~2 sec.) is reached
  *(p_value)  =s_read_byte(ACK);    //read the first byte (MSB)
  *(p_value+1)=s_read_byte(ACK);    //read the second byte (LSB)
  *p_checksum =s_read_byte(noACK);  //read checksum
  return error;
}

//----------------------------------------------------------------------------------------
void calc_sth11(float *p_humidity ,float *p_temperature)
//----------------------------------------------------------------------------------------
// calculates temperature [�C] and humidity [%RH] 
// input :  humi [Ticks] (12 bit) 
//          temp [Ticks] (14 bit)
// output:  humi [%RH]
//          temp [�C]
{ const float C1=-2.0468;           // for 12 Bit RH
  const float C2=+0.0367;           // for 12 Bit RH
  const float C3=-0.0000015955;     // for 12 Bit RH
  const float T1=+0.01;             // for 12 Bit RH
  const float T2=+0.00008;          // for 12 Bit RH	

  float rh=*p_humidity;             // rh:      Humidity [Ticks] 12 Bit 
  float t=*p_temperature;           // t:       Temperature [Ticks] 14 Bit
  float rh_lin;                     // rh_lin:  Humidity linear
  float rh_true;                    // rh_true: Temperature compensated humidity
  float t_C;                        // t_C   :  Temperature [�C]

  t_C=t*0.01 - 40.1;                //calc. temperature [�C] from 14 bit temp. ticks @ 5V
  rh_lin=C3*rh*rh + C2*rh + C1;     //calc. humidity from ticks to [%RH]
  rh_true=(t_C-25)*(T1+T2*rh)+rh_lin;   //calc. temperature compensated humidity [%RH]
  if(rh_true>100)rh_true=100;       //cut if the value is outside of
  if(rh_true<0.1)rh_true=0.1;       //the physical possible range

  *p_temperature=t_C;               //return temperature [�C]
  *p_humidity=rh_true;              //return humidity[%RH]
}

//--------------------------------------------------------------------
float calc_dewpoint(float h,float t)
//--------------------------------------------------------------------
// calculates dew point
// input:   humidity [%RH], temperature [�C]
// output:  dew point [�C]
{ float k,dew_point ;
  
  k = (log10(h)-2)/0.4343 + (17.62*t)/(243.12+t);
  dew_point = 243.12*k/(17.62-k);
  return dew_point;
}

//������� �� ����-----------------------------------------------------------------------





// SMBus byte write function-----------------------------------------------------
// Writes a single byte at the specified memory location.
//
// out_byte = data byte to be written
// byte_address = memory location to be written into (2 bytes)
// chip_select = device address of EEPROM chip to be written to
void SM_Send (char chip_select, unsigned int byte_address, char out_byte)
{
   while (SM_BUSY);                          // Wait for SMBus to be free.
   SM_BUSY = 1;                              // Occupy SMBus (set to busy)
   SMB0CN = 0x44;                            // SMBus enabled,
                                             // ACK on acknowledge cycle

   BYTE_NUMBER = 2;                          // 2 address bytes.
   COMMAND = (chip_select | WRITE);          // Chip select + WRITE

   HIGH_ADD = ((byte_address >> 8) & 0x00FF);// Upper 8 address bits
   LOW_ADD = (byte_address & 0x00FF);        // Lower 8 address bits

   WORD = out_byte;                          // Data to be writen
   
   STO = 0;
   STA = 1;                                  // Start transfer
}

// SMBus random read function------------------------------------------------------
// Reads 1 byte from the specified memory location.
//
// byte_address = memory address of byte to read
// chip_select = device address of EEPROM to be read from
char SM_Receive (char chip_select, unsigned int byte_address)
{
   SFRPAGE   = SMB0_PAGE;
	while (SM_BUSY);                          // Wait for bus to be free.
   SM_BUSY = 1;                              // Occupy SMBus (set to busy)
	SMB0CN = 0x44;                            // SMBus enabled, ACK on acknowledge cycle

   BYTE_NUMBER = 2;                          // 2 address bytes
   COMMAND = (chip_select | READ);           // Chip select + READ

   HIGH_ADD = ((byte_address >> 8) & 0x00FF);// Upper 8 address bits
   LOW_ADD = (byte_address & 0x00FF);        // Lower 8 address bits
   
   STO = 0;
   STA = 1;                                  // Start transfer
   while (SM_BUSY);                          // Wait for transfer to finish
   return WORD;
}
//------------------------------------------------------------------------------
/*
void OutModem1(unsigned char Data, char i)
{
	BufferInModem[i] = Data | 0x80;
}

//------------------------------------------------------------------------------
void OutModem2(unsigned int Data, char i)
{
	BufferInModem[i] = (Data & 0x007f)| 0x80;
	BufferInModem[i+1] = ((Data & 0x3f80) >> 7)| 0x80;
}

//------------------------------------------------------------------------------
void OutModem4(unsigned long int Data, char i)
{
	BufferInModem[i] = (Data & 0x0000007f)| 0x80;
	BufferInModem[i+1] = ((Data & 0x3f80) >> 7) | 0x80;
	BufferInModem[i+2] = ((Data & 0x1fc000) >> 14) | 0x80;
   BufferInModem[i+3] = ((Data & 0xfe00000)>> 21) | 0x80;
}
*/
//------------------------------------------------------------------------------------------
void LirQ(void)
{
	xdata char SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = 0x00;

		//����� ���----------------------------
	//	EA=0;
		LirData = 0x0000;
		//---init
		LIR_SCK = 1;
		LIR_SCK = 0;
		//---D n-1
		LIR_SCK = 1;
		LIR_SCK = 0;
		//---D n-1
		LIR_SCK = 1;
		LIR_SCK = 0;
		if(LIR_DATA!=0) 
		{
			LirData = LirData | (0x1 << (11));
		}
		LIR_SCK = 1; 
		LIR_SCK = 0;
		if(LIR_DATA!=0) 
		{
			LirData = LirData | (0x1 << (10));
		}
		LIR_SCK = 1; 
		LIR_SCK = 0;
		if(LIR_DATA!=0) 
		{
			LirData = LirData | (0x1 << (9));
		}
		LIR_SCK = 1; 
		LIR_SCK = 0;
		if(LIR_DATA!=0) 
		{
			LirData = LirData | (0x1 << (8));
		}
		LIR_SCK = 1; 
		LIR_SCK = 0;
		if(LIR_DATA!=0) 
		{
			LirData = LirData | (0x1 << (7));
		}
		LIR_SCK = 1; 
		LIR_SCK = 0;
		if(LIR_DATA!=0) 
		{
			LirData = LirData | (0x1 << (6));
		}
		LIR_SCK = 1; 
		LIR_SCK = 0;
		if(LIR_DATA!=0) 
		{
			LirData = LirData | (0x1 << (5));
		}
		LIR_SCK = 1; 
		LIR_SCK = 0;
		if(LIR_DATA!=0) 
		{
			LirData = LirData | (0x1 << (4));
		}
		LIR_SCK = 1; 
		LIR_SCK = 0;
		if(LIR_DATA!=0) 
		{
			LirData = LirData | (0x1 << (3));
		}
		LIR_SCK = 1; 
		LIR_SCK = 0;
		if(LIR_DATA!=0) 
		{
			LirData = LirData | (0x1 << (2));
		}
		LIR_SCK = 1; 
		LIR_SCK = 0;
		if(LIR_DATA!=0) 
		{
			LirData = LirData | (0x1 << (1));
		}
		LIR_SCK = 1; 
		LIR_SCK = 0;
		if(LIR_DATA!=0) 
		{
			LirData = LirData | (0x1 << (0));
		}
		LIR_SCK = 1; 
	//	EA=1;
	SFRPAGE = SFRPAGE_SAVE;
	return;
}
//------------------------------------------------------------------------------------------

void main(void)
{
value humi_val,temp_val;
  float dew_point;
  unsigned char error,checksum;
  unsigned int i;

  init_uart();
  s_connectionreset();
  while(1)
  { error=0;
    error+=s_measure((unsigned char*) &humi_val.i,&checksum,HUMI);  //measure humidity
    error+=s_measure((unsigned char*) &temp_val.i,&checksum,TEMP);  //measure temperature
    if(error!=0) s_connectionreset();                 //in case of an error: connection reset
    else
    { humi_val.f=(float)humi_val.i;                   //converts integer to float
      temp_val.f=(float)temp_val.i;                   //converts integer to float
      calc_sth11(&humi_val.f,&temp_val.f);            //calculate humidity, temperature
      dew_point=calc_dewpoint(humi_val.f,temp_val.f); //calculate dew point
      printf("temp:%5.1fC humi:%5.1f%% dew point:%5.1fC\n",temp_val.f,humi_val.f,dew_point);
    }
    //----------wait approx. 0.8s to avoid heating up SHTxx------------------------------      
    for (i=0;i<40000;i++);     //(be sure that the compiler doesn't eliminate this line!)
    //-----------------------------------------------------------------------------------                       
  }

/*
	//---------
	xdata unsigned int i;
  	INT Aint;
	FLOAT Bfloat;
	//UART----------
   xdata char RK_code[26], nByte = 0, KontrSumma = 0, PWM;
	unsigned int Value;
	//--------------
   bit ValidGPS, flPoint; 	
	xdata unsigned char i_comma, tmpGPS[6], nLetter = 7;
	xdata unsigned long temp_koord;
	//SMBus---------
	unsigned char check;             // Used for testing purposes
	//--------------

	Init_Device();

	LirTarget = 0x03ff;
	Fdelta = 0;
	flNewGPS = 0;

	SM_BUSY = 0;                     // Free SMBus for first transfer.
   while(1)
	{
	
	
//������ �������� ����� � ����--------------------------------------------------------------
		if(VeterFlag)
		{
			Vspeed = (float)FREQT0/((float)TV*0.75);
			Angle = 360. - (360.*(float)TV/(float)TA);
			VeterFlag = 0;
		}

//����� GPS---------------------------------------------------------------------------

		if(flNewGPS)	
		{
			flNewGPS = 0;
		//����������� ������� GPS
			if (r < w+mar*NS) 
			{
				if(mess[r] == '$')
				{
            	nLetter = 0;
				}
         	else if ((nLetter == 0) && (mess[r] == 'G'))
            	nLetter++;
         	else if ((nLetter == 1) && (mess[r] == 'P'))
            	nLetter++;
         	else if ((nLetter == 2) && (mess[r] == 'R'))
            	nLetter++;
	         else if ((nLetter == 3) && (mess[r] == 'M'))
   	         nLetter++;
      	   else if ((nLetter == 4) && (mess[r] == 'C'))
         	{
            	nLetter++;
					i_comma = 0;
					ValidGPS = 0;
      	   } 
		   	else if(mess[r] == ',')
				{
					i_comma++;
					i = 0;
					flPoint = 0;
				}
		   	else if(i_comma == 2)
				{
					if(mess[r] == 'A') 
						ValidGPS = 1;
					else
					{
						ValidGPS = 0;
	//					flNoKoord = 1;
	//					liTimer_tick_GPS = liTimer_tick;
					}
				}
         	else if (ValidGPS)
				{
					if (i_comma == 3)                			//Latitude
					{
						if(mess[r] == '.')
						{
							flPoint = 1;
							i = 0;
						}
						else if (flPoint == 0)						//����� �����
						{
							tmpGPS[i++] = mess[r];
							if(i == 2)
							{
		      		   	tmpGPS[i] = 0;
								temp_koord = atoi(tmpGPS);
   			      		temp_koord = 60UL*10000*temp_koord;
							}	
							else if(i == 4)
							{
								tmpGPS[0] = tmpGPS[1] = '0';
			      	   	tmpGPS[i] = 0;
   				      	temp_koord = temp_koord+10000UL*atoi(tmpGPS);
							}	
						}
						else												//������� �����
						{
							tmpGPS[i++] = mess[r];
	      		   	tmpGPS[i] = 0;
						}
					}
					else if (i_comma == 4)
					{
		      		temp_koord = temp_koord+atoi(tmpGPS);
//-----------
//lLatFly = 55UL*60*10000+50UL*10000+8680;
//--------
						if (mess[r] == 'S')   						//���� Latitude
							LatFly = 54000000UL-temp_koord;		//90UL*60*10000-koord;
						else		  
							LatFly = 54000000UL+temp_koord;		//90UL*60*10000+koord;
					}
					else if (i_comma == 5)                		//Longitude
					{
						if(mess[r] == '.')
						{
							flPoint = 1;
							i = 0;
						}
						else if (flPoint == 0)						//����� �����
						{
							tmpGPS[i++] = mess[r];
							if(i == 3)
							{
		      		   	tmpGPS[i] = 0;
   			      		temp_koord = atoi(tmpGPS);
   			      		temp_koord = 60UL*10000*temp_koord;
							}	
							else if(i == 5)
							{
								tmpGPS[0] = tmpGPS[1] = tmpGPS[2] = '0';
			      	   	tmpGPS[i] = 0;
   				      	temp_koord = temp_koord+10000UL*atoi(tmpGPS);
							}	
						}
						else												//������� �����
						{
							tmpGPS[i++] = mess[r];
	      		   	tmpGPS[i] = 0;
						}
					}
					else if (i_comma == 6)
					{
		      		temp_koord = temp_koord+atoi(tmpGPS);
//----------------
//lLonFly = 49UL*60*10000+6UL*10000+3760;
//----------------
						if (mess[r] == 'W')   						//���� Longitude
							LonFly = 108000000UL-temp_koord;		//180UL*60*10000-koord;
						else       
							LonFly = temp_koord+108000000UL;		//180UL*60*10000;
					}
					else if (i_comma == 7)							//�������� � �����
					{
						if(mess[r] == '.')
						{
							flPoint = 1;
   		      		Vz = 1.852*atoi(tmpGPS)/3.6;   		//������������� �� ����� � �/�
//---------
//Vz = 20;
//-----------
						}
						else if(flPoint == 0)
						{
							tmpGPS[i++] = mess[r];
	      	   		tmpGPS[i] = 0;
						}
            	}
					else if (i_comma == 8)							//���� � ��������
					{
						if(mess[r] == '.')
						{
							flPoint = 1;
   		      		koors = atoi(tmpGPS);
                  	if (koors < 0)
                     	koors = 360+koors;
							flNoKoord = 0;
							flNewGPS = 1;
							liTimer_tick_GPS = liTimer_tick;	
	//		            OutModem20();
//-----------						   
//koors = 30;
//-----------
						}
						else if(flPoint == 0)
						{
							tmpGPS[i++] = mess[r];
	      		   	tmpGPS[i] = 0;
						}
	            }
				}
				r++;
	   		if(r >= NS)
				{
   	   		r = 0;
					mar = 0;	
				}      
			}
		}
	
//����� SMBus-------------------------------------------------------------------------

if(u0timer>0xf0)
{
	SM_Send(CHIP_A, 0x00, 0x1e);      // Send CHIP, address, (data)
//	SM_Send(CHIP_A, 0x00, 0x05);      // Send CHIP, address, (data)
//	while(SMB0DAT!=0); 
//	check = SM_Receive(CHIP_A, 0x00); // Read address on CHIP
}


//������ � COM ������-----------------------------------------------------------------

		if(u0timer>0xf0)
		{
			u0timer = 0;
			//	ES0=1;
			if(wBFM > 4)
			{
				if(BuferFromModem [wBFM-1]==0x41)
				{
					if(BuferFromModem [wBFM-4]==0x40)
					{
						Aint.b[0] = BuferFromModem[wBFM-3];
						Aint.b[1] = BuferFromModem[wBFM-2];
						if(Aint.i<=0x0fff)
						{
							LirTarget = Aint.i;
						}
					}	
				}
				wBFM = 0;
			}
			//	TI0 = 1; //Read redy UART0
			//Write to UART0--------------------------------------
			Aint.i = LirData;
			BufferInModem[0] = 0x40+1;
			BufferInModem[1] = Aint.b[0];		
			BufferInModem[2] = Aint.b[1];
			Bfloat.f= Vspeed; 
			BufferInModem[3] = Bfloat.b[0];
			BufferInModem[4] = Bfloat.b[1];
			BufferInModem[5] = Bfloat.b[2];
			BufferInModem[6] = Bfloat.b[3];
			Bfloat.f= Angle;
			BufferInModem[7] = Bfloat.b[0];
			BufferInModem[8] = Bfloat.b[1];
			BufferInModem[9] = Bfloat.b[2];
			BufferInModem[10] = Bfloat.b[3];
			BufferInModem[11] = 0x40;
  		   BufferInModem[12] = 0;
  	   	for (i = 0; i < 12; i++ )
		     	BufferInModem[12] = BufferInModem[12] ^ BufferInModem[i];
    	 	BufferInModem[12] = 0x80 | BufferInModem[12];

			flTransmiter = 1;
	
   		r0 = 0;
  		 	rk = 13;
			SFRPAGE = 0x00;

			while (flTransmiter)
			{
				if(r0 < rk)
				{
					flTransmiter = 1;
					SBUF0 = BufferInModem[r0++];
				}
				else
				{
					flTransmiter = 0;
				}
			}
	//		ES0=0;

	//------------------------------------------------------------
	}*/
}

//------------------------------------------------------------------------------------------
//�������� �����
void INT0 (void) interrupt 0
{
	VeterFlag = 1;
	TV = RTC - RTCV;
	RTCV = RTC;
	EX0 =	0;
	return;
}

//------------------------------------------------------------------------------------
void TIMER_ISR0 (void) interrupt 1
{
	xdata unsigned int i, delta, FdKoeff;
	xdata char SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = TIMER01_PAGE;
	RTC++;
	GPStimepulse++;
	/*
	if(GPStimepulse==0x4b00)
	{
		LED=1;
	}
	else if(GPStimepulse>0x5355)
	{
		LED=0;
		GPStimepulse=0;
	}*/
//�����-------------
	if(WSpeed!=0)
		EX0=1;

	if(WAngle!=0)
		EX1=1;

	SFRPAGE = SFRPAGE_SAVE;
	return;
}



//����������� �����-------------------------------------------------------------------
void INT1 (void) interrupt 2
{
	VeterFlag = 1;
	TA = RTC - RTCA;
	RTCA = RTC;
  	EX1=0;
	return;
}

//------------------------------------------------------------------------------------
void TIMER_ISR1 (void) interrupt 3
{
	xdata char SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = TIMER01_PAGE;
	
	SFRPAGE = SFRPAGE_SAVE;
	return;
}

//-------------------------------------------------------------------
void COM_UART0_isr(void) interrupt 4
{
	xdata char SFRPAGE_SAVE = SFRPAGE;
	INT Aint;
	SFRPAGE = UART0_PAGE;

	if (RI0)
  	{
		BuferFromModem [wBFM++] = SBUF0;  // read character
		if(wBFM >= NBFM)
		{
     		wBFM = 0;
			marBFM = 1;
		}/*
		if(BuferFromModem [wBFM-1]==0x41)
		{
			if(BuferFromModem [wBFM-4]==0x40)
			{
				Aint.b[0] = BuferFromModem[wBFM-3];
				Aint.b[1] = BuferFromModem[wBFM-2];
				if(Aint.i<=0x0fff)
				{
					LirTarget = Aint.i;
				}
			}	
		}*/
	  	RI0 = 0;
	}
	if (TI0)
	{
		if(flTransmiter == 0)
			TI0 = 0;	
  	}

	SFRPAGE = UART1_PAGE;
	if (RI1)
  	{
		flNewGPS = 1;
		mess [w++] = SBUF1;  // read character
		if(w >= NS)
		{
     		w = 0;
			mar = 1;
		}
		RI1 = 0;
	}
	SFRPAGE = SFRPAGE_SAVE;
	return;
}

//------------------------------------------------------------------------------------
void SMBus_isr (void)interrupt 7
{
	xdata char SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = SMB0_PAGE;
	
   switch (SMB0STA){                   // Status code for the SMBus (SMB0STA register)

      // Master Transmitter/Receiver: START condition transmitted.
      // The R/W bit of the COMMAND word sent after this state will
      // always be a zero (W) because for both read and write,
      // the memory address must be written first.
      case SMB_START:
         SMB0DAT = (COMMAND & 0xFE);   // Load address of the slave to be accessed.
         STA = 0;                      // Manually clear START bit
         break;

      // Master Transmitter/Receiver: Repeated START condition transmitted.
      // This state should only occur during a read, after the memory address has been
      // sent and acknowledged.
      case SMB_RP_START:
         SMB0DAT = COMMAND;            // COMMAND should hold slave address + R.
         STA = 0;
         break;

      // Master Transmitter: Slave address + WRITE transmitted.  ACK received.
      case SMB_MTADDACK:
         SMB0DAT = HIGH_ADD;           // Load high byte of memory address
                                       // to be written.
         break;

      // Master Transmitter: Slave address + WRITE transmitted.  NACK received.
      // The slave is not responding.  Send a STOP followed by a START to try again.
      case SMB_MTADDNACK:
         STO = 1;
         STA = 1;
         break;

      // Master Transmitter: Data byte transmitted.  ACK received.
      // This state is used in both READ and WRITE operations.  Check BYTE_NUMBER
      // for memory address status - if only HIGH_ADD has been sent, load LOW_ADD.
      // If LOW_ADD has been sent, check COMMAND for R/W value to determine 
      // next state.
      case SMB_MTDBACK:
         switch (BYTE_NUMBER){
            case 2:                    // If BYTE_NUMBER=2, only HIGH_ADD
               SMB0DAT = LOW_ADD;      // has been sent.
               BYTE_NUMBER--;          // Decrement for next time around.
               break;
            case 1:                    // If BYTE_NUMBER=1, LOW_ADD was just sent.
               if (COMMAND & 0x01){    // If R/W=READ, sent repeated START.
                  STO = 0;
                  STA = 1;

               } else { 
                  SMB0DAT = WORD;      // If R/W=WRITE, load byte to write.
                  BYTE_NUMBER--;
               }
               break;
            default:                   // If BYTE_NUMBER=0, transfer is finished.
               STO = 1;
               SM_BUSY = 0;            // Free SMBus
            }
         break;


      // Master Transmitter: Data byte transmitted.  NACK received.
      // Slave not responding.  Send STOP followed by START to try again.
      case SMB_MTDBNACK:
         STO = 1;
         STA = 1;
         break;

      // Master Transmitter: Arbitration lost.
      // Should not occur.  If so, restart transfer.
      case SMB_MTARBLOST:
         STO = 1;
         STA = 1;
         break;

      // Master Receiver: Slave address + READ transmitted.  ACK received.
      // Set to transmit NACK after next transfer since it will be the last (only)
      // byte.
      case SMB_MRADDACK:
         AA = 0;                       // NACK sent on acknowledge cycle.
         break;

      // Master Receiver: Slave address + READ transmitted.  NACK received.
      // Slave not responding.  Send repeated start to try again.
      case SMB_MRADDNACK:
         STO = 0;
         STA = 1;
         break;

      // Data byte received.  ACK transmitted.
      // State should not occur because AA is set to zero in previous state.
      // Send STOP if state does occur.
      case SMB_MRDBACK:
         STO = 1;
         SM_BUSY = 0;
         break;

      // Data byte received.  NACK transmitted.
      // Read operation has completed.  Read data register and send STOP.
      case SMB_MRDBNACK:
         WORD = SMB0DAT;
         STO = 1;
         SM_BUSY = 0;                  // Free SMBus
         break;

      // All other status codes meaningless in this application. Reset communication.
      default:
         STO = 1;                      // Reset communication.
         SM_BUSY = 0;
         break;
      }

   SI=0;                               // clear interrupt flag

	SFRPAGE = SFRPAGE_SAVE;
	return;
}

//------------------------------------------------------------------------------------
void PCA_isr (void)interrupt 9
{
	xdata char SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = PCA0_PAGE;
//-------------------
	u0timer++; // COM u0
//	u1timer++; // GPS u1
//-------------------	
	CF = 0;
//���������� �������------------------------------------------------------------------------
		if(F==0x02)
		{
			LirQ();
			delta = (LirTarget - LirData);	
			if(delta > 0xF000)
				delta = delta - 0xF000;
			if(delta >= 0x07FF)
			{
				delta = 0x0FFF - delta;
				if(delta < 0x0155)
				{
					F = 0xC1;
					if(delta < 0x20 && delta > 0x0B)
					{
						F = 0xC1;
					}
					else if(delta <= 0x0B)
					{
						F = 0x03; 
					}
				}
				else
				{
					F=0xA1;
				}
			}
			else if(delta < 0x07FF)
			{
				if(delta < 0x0155)
				{
					F = 0xC2;
					if(delta < 0x20 && delta > 0x0B)
					{
						F = 0xC2;
					} 
					else if(delta <= 0x0B)
					{
						F = 0x03; 
					}
				}
				else
				{
					F=0xA2;
				}
			}
		}
	   if(LirData==0x0FFF)//��� ����������� ���, ����������� �����������
		{
			F=0x03;
		}
		//���� �� �������--------------------------
		PCA0CPM0  = 0x43;
		PCA0CPH0  = 0xFF;
		PCA0CPL0  = 0xFF;
		PCA0CPM1  = 0x43;
		PCA0CPH1  = 0xFF;
		PCA0CPL1  = 0xFF;

		if(F==0xA1)
		{
			PCA0CPM0  = 0x43;
			PCA0CPH0  = 0xC0;
			PCA0CPM1  = 0x43;
			PCA0CPH1  = 0xFF;
			PCA0CPL1  = 0xFF;
			Fdelta = Fdelta -  delta;
			if(Fdelta>0x0033)
			{
				PCA0CPM0  = 0x43;
				PCA0CPH0  = 0xf0;
				PCA0CPM1  = 0x43;
				PCA0CPH1  = 0xFF;
				PCA0CPL1  = 0xFF;
			}
			else
			{
				PCA0CPH0  = 0xC0 + Fdelta;
			}
			Fdelta = delta;
		}
		else if(F==0xA2)
		{
			PCA0CPM1  = 0x43;
			PCA0CPH1  = 0xC0;
			PCA0CPM0  = 0x43;
			PCA0CPH0  = 0xFF;
			PCA0CPL0  = 0xFF;
			Fdelta = Fdelta -  delta;			
			if(Fdelta>0x0033)
			{
				PCA0CPM1  = 0x43;
				PCA0CPH1  = 0xf0;
				PCA0CPM0  = 0x43;
				PCA0CPH0  = 0xFF;
				PCA0CPL0  = 0xFF;
			}
			else
			{
				PCA0CPH1  = 0xC0 + Fdelta;
			}
			Fdelta = delta;
		}
		else if(F==0xC1)
		{
			PCA0CPM0  = 0x43;
			PCA0CPH0  = 0xe6;
			PCA0CPM1  = 0x43;
			PCA0CPH1  = 0xFF;
			PCA0CPL1  = 0xFF;
			Fdelta = Fdelta -  delta;
			if(Fdelta>0x0019)
			{
				PCA0CPM0  = 0x43;
				PCA0CPH0  = 0xFF;
				PCA0CPL0  = 0xFF;
				PCA0CPM1  = 0x43;
				PCA0CPH1  = 0xFF;
				PCA0CPL1  = 0xFF;
			}
			else
			{
				PCA0CPH0  = 0xe6 + Fdelta;
			}
			Fdelta = delta;
		}
		else if(F==0xC2)
		{
			PCA0CPM1  = 0x43;
			PCA0CPH1  = 0xe6;
			PCA0CPM0  = 0x43;
			PCA0CPH0  = 0xFF;
			PCA0CPL0  = 0xFF;
			Fdelta = Fdelta -  delta;
			if(Fdelta>0x0019)
			{
				PCA0CPM0  = 0x43;
				PCA0CPH0  = 0xFF;
				PCA0CPL0  = 0xFF;
				PCA0CPM1  = 0x43;
				PCA0CPH1  = 0xFF;
				PCA0CPL1  = 0xFF;
			}
			else
			{
				PCA0CPH1  = 0xe6 + Fdelta;
			}
			Fdelta = delta;
		}  
		if(F==0x03)
		{
		   PCA0CPM0  = 0x43;
			PCA0CPH0  = 0xFF;
			PCA0CPL0  = 0xFF;
			PCA0CPM1  = 0x43;
			PCA0CPH1  = 0xFF;
			PCA0CPL1  = 0xFF;
		}
		F=0x02;
	SFRPAGE = SFRPAGE_SAVE;
	return;
}
//------------------------------------------------------------------------------------
void TIMER_ISR3 (void) interrupt 14
{
	xdata char SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = TMR3_PAGE;

	SFRPAGE = SFRPAGE_SAVE;
	return;
}
//-------------------------------------------------------------------
void GPS_UART1_isr(void) interrupt 20
{
	xdata char SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = UART1_PAGE;
	if (RI1)
  	{
		flNewGPS = 1;
		mess [w++] = SBUF1;  // read character
		if(w >= NS)
		{
     		w = 0;
			mar = 1;
		}
		RI1 = 0;
	}
	if (TI1)
	{
			
  	}

	SFRPAGE = SFRPAGE_SAVE;
	return;
}
//------------------------------------------------------------------------------------------
//#endif