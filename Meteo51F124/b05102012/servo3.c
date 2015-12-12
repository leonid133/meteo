#include "c8051f120.h"
#include "init.h"
#include "comport.h"
#include <intrins.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "servo.h"  
#include <math.h> 
         

#define SYSCLK 24500000 //Hz
#define FREQT0 8006 //Hz

//����������
sbit LED = P1^6;
sbit LED2 = P2^6;
sbit LED3 = P3^0;
sbit BUTTON = P3^7;
xdata unsigned char InitFlag;
//--------------
/*
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

//GPS
#define NS 	75
xdata unsigned int GPStimepulse;
xdata char mess [NS], r, w, mar;		// ��� ������� ������� GPS
bit flNewGPS;
xdata unsigned int Vz, koors, flNoKoord, liTimer_tick_GPS, liTimer_tick;
xdata unsigned long LatFly, LonFly;

/*

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
}*/
//------------------------------------------------------------------------------------------


/*
void main(void)
{
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
	}
}
*/
/*
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
	
	if(GPStimepulse==0x4b00)
	{
		LED=1;
	}
	else if(GPStimepulse>0x5355)
	{
		LED=0;
		GPStimepulse=0;
	}
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
		}
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
}*/
//SMB begin----------------------------------------------------------------
/*
//SMBus
#define WRITE 0x00                  // SMBus WRITE command
#define READ  0x01                  // SMBus READ command
// Device addresses (7 bits, lsb is a don't care)
#define CHIP_A 0x00                 // Device address for chip A
#define CHIP_B 0x80                 // Device address for chip B
#define CHIP_C 0xA0                 // Device address for chip C

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



void nop(void)
{
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
}
// SMBus byte write function-----------------------------------------------------
// Writes a single byte at the specified memory location.
//
// out_byte = data byte to be written
// byte_address = memory location to be written into (2 bytes)
// chip_select = device address of EEPROM chip to be written to
void SM_Send (char chip_select, unsigned int byte_address, char out_byte)
{
   unsigned int i;
	xdata unsigned int timesmb;
	timesmb =0;
	while (SM_BUSY);  
	{	
		timesmb++;
		for(i=0; i<40; i++)
		{	
			nop();
		}
		if(timesmb==0xfffc){SM_BUSY = 0;}
	};                        // Wait for SMBus to be free.
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
   unsigned int i;
	xdata unsigned int timesmb;
	SFRPAGE   = SMB0_PAGE;
	timesmb =0; 
	while (SM_BUSY)
	{	
		timesmb++;
		for(i=0; i<40; i++)
		{	
			nop();
		}
		if(timesmb==0xfffc){SM_BUSY = 0;}
	};                          // Wait for bus to be free.
   SM_BUSY = 1;                              // Occupy SMBus (set to busy)
	SMB0CN = 0x44;                            // SMBus enabled, ACK on acknowledge cycle

   BYTE_NUMBER = 2;                          // 2 address bytes
   COMMAND = (chip_select | READ);           // Chip select + READ

   HIGH_ADD = ((byte_address >> 8) & 0x00FF);// Upper 8 address bits
   LOW_ADD = (byte_address & 0x00FF);        // Lower 8 address bits
   
   STO = 0;
   STA = 1;   
	timesmb =0;                               // Start transfer
	while (SM_BUSY)
	{
		timesmb++;
		for(i=0; i<40; i++)
		{	
			nop();
		}
		if(timesmb==0xfffc){SM_BUSY = 0;}
	};                          // Wait for transfer to finish
   return WORD;
}
//------------------------------------------------------------------------------


void main(void)
{
	unsigned char check;
	unsigned int i;
  	Init_Device(); 
	SM_BUSY = 0;   
	while(1)
	{
		SM_Send(CHIP_A, 0x00, 0x03); 		// Send CHIP, address, (data)
		for(i=0; i<40000; i++)
		{	
			nop();
			nop();
			nop();
		}
		check = SM_Receive(CHIP_A, 0x00);// Read address on CHIP
		for(i=0; i<40000; i++)
		{	
			nop();
			nop();
			nop();
		}
	}
}

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
*/
//SMBend------------------------------------------------------------------------------


#define  BYTE     char
#define	DATA 		LED
#define	SCK		LED2
//#define	DATAOUT	0x02
//#define	DATAIN	0xFD

#define noACK 0
#define ACK 1
//adr command r/w
#define STATUS_REG_W 0x06 //000 0011 0
#define STATUS_REG_R 0x07 //000 0011 1
#define MEASURE_TEMP 0x03 //000 0001 1
#define MEASURE_HUMI 0x05 //000 0010 1
#define RESET 0x1e //000 1111 0


#define C1	-4.0 		// for 12 Bit
#define C2	0.0405 		// for 12 Bit
#define C3	-0.0000028 	// for 12 Bit
#define T1	0.01 		// for 14 Bit @ 5V
#define T2	0.00008 	// for 14 Bit @ 5V

float temp, rh, dp;

void SHTInit(void);
void nop(void);
BYTE reverse(BYTE b);
char s_write_byte(unsigned char value);
BYTE	SHTAskMeasurement(BYTE mode);
BYTE	SHTGetMeasurementNoCrc(float *value);
BYTE	SHTGetMeasurementCrc(float *value);

BYTE 	crc;
BYTE	StartCrc;

xdata int TempHumTimer;

void SHTInit(void)
{
	StartCrc = 0;
}
//----------------------------------------------------------------------------------
// writes a byte on the Sensibus and checks the acknowledge
char s_write_byte(unsigned char value)
{
unsigned char i,error=0;
//P1MDOUT |= DATAOUT;
for (i=0x80;i>0;i/=2) //shift bit for masking
{ 
	if (i & value) DATA=1; //masking value with i , write to SENSI-BUS
	else DATA=0;
	SCK=1; //clk for SENSI-BUS
	nop();nop();nop(); //pulswith approx. 5 us
	SCK=0;
}
//P1MDOUT &= DATAIN;
DATA=1; //release DATA-line
SCK=1; //clk #9 for ack
error=DATA; //check ack (DATA will be pulled down by SHT11)
SCK=0;
return error; //error=1 in case of no acknowledge
}

//----------------------------------------------------------------------------------
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1"
char s_read_byte(unsigned char ack)
{
unsigned char i,val=0;

DATA=1; //release DATA-line
for (i=0x80;i>0;i/=2) //shift bit for masking
{
	 SCK=1; //clk for SENSI-BUS
	 nop();
	if (DATA) val=(val | i); //read bit
	nop();
	SCK=0;
}
//P1MDOUT |= DATAOUT;
DATA=!ack; //in case of "ack==1" pull down DATA-Line
SCK=1; //clk #9 for ack
nop();nop();nop(); //pulswith approx. 5 us
SCK=0;
//P1MDOUT &= DATAIN;
DATA=1; //release DATA-line

return val;
}

//----------------------------------------------------------------------------------
void s_transstart(void)
//----------------------------------------------------------------------------------
// generates a transmission start
// 		_____ 		  ________
// DATA:      |_______|
// 		    ___ 	   ___
// SCK : ___|   |___|   |______
{
//P1MDOUT |= DATAOUT;
DATA=1; SCK=0; //Initial state
nop();
SCK=1;
nop();
DATA=0;
nop();
SCK=0;
nop();nop();nop();
SCK=1;
nop();
DATA=1;
nop();
SCK=0;
//P1MDOUT &= DATAIN;
}

//----------------------------------------------------------------------------------
void s_connectionreset(void)
//----------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
// 		_____________________________________________________ 			________
// DATA: 															         |_______|
// 			_ 	  _ 	 _ 	_ 	  _ 	 _ 	_ 	  _ 	 _ 	  	 ___ 		___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
{
unsigned char i;
//P1MDOUT |= DATAOUT;
DATA=1; 
SCK=0; //Initial state
for(i=0;i<9;i++) //9 SCK cycles
{ 
	SCK=1;
	nop();
	SCK=0;
	nop();
}
s_transstart(); //transmission start
}

//----------------------------------------------------------------------------------
BYTE s_softreset(void)
//----------------------------------------------------------------------------------
// resets the sensor by a softreset
{
unsigned char error=0;
s_connectionreset(); //reset communication
error+=s_write_byte(RESET); //send RESET-command to sensor
return error; //error=1 in case of no response form the sensor
}



float	calcTemp(float t)
{
return (t*0.01 - 40);	//calc. Temperature from ticks to [oC]
}



float	calcRH(float rh, float tc)
{
float rh_lin; // rh_lin: Humidity linear
float rh_true; // rh_true: Temperature compensated humidity

rh_lin=(C3 *rh + C2)*rh + C1; //calc. Humidity from ticks to [%RH]
rh_true=(tc-25)*(T1+T2*rh)+rh_lin; //calc. Temperature compensated humidity [%RH]
if(rh_true>100.0)rh_true=100.0; //cut if the value is outside of
if(rh_true<0.1)rh_true=0.1; //the physical possible range
return	rh_true; 
}

//--------------------------------------------------------------------
float calc_dewpoint(float h,float t)
//--------------------------------------------------------------------
// calculates dew point
// input: humidity [%RH], temperature [oC]
// output: dew point [oC]
{ 
	float k,dew_point ;

	k = (log10(h)-2)/0.4343 + (17.62*t)/(243.12+t);
	dew_point = 243.12*k/(17.62-k);
	return dew_point;
}



BYTE	sht(BYTE	task)
{
BYTE	error = 0;
float	val;
	switch(task)
	{
	case 0:	//Ask for temperature
	error += SHTAskMeasurement(MEASURE_TEMP);
	break;
	case 1:	//Get temperature results
	error += SHTGetMeasurementNoCrc(&val);
	if(!error) temp = calcTemp(val);
	return	error;
	break;
	case 2:	//Ask for humidity
	error +=	SHTAskMeasurement(MEASURE_HUMI);
	break;
	case 3:
	return 0;
	break;
	case 4:	//Get humidity result
	error += SHTGetMeasurementNoCrc(&val);
	if(!error)
	{
	rh = calcRH(val,temp);
	dp = calc_dewpoint(rh,temp);
	}
	return	error;
	break;
	default:
	return	0x80;
	break;
	}
	return	error;
}

BYTE	SHTAskMeasurement(BYTE mode)
{
	BYTE error = 0;
	s_transstart(); //transmission start
	error+=s_write_byte(mode);
//	crc = CrcByte(mode,StartCrc);
	return error;
}

BYTE	SHTGetMeasurementNoCrc(float *value)
{
	BYTE error = 0;
	unsigned int i;
	unsigned int result = 0;
	for (i=0;i<65535;i++) 
	{
		nop();
		if(DATA==0) break; //wait until sensor has finished the measurement
	}
	if(DATA) error++; // or timeout (~2 sec.) is reached
	result = 0x100 * s_read_byte(ACK); //read the first byte (MSB)
	result += s_read_byte(noACK); //read the second byte (LSB)
	s_read_byte(noACK); //read checksum
	*value = (float)result;
	return error;
}

BYTE	SHTGetMeasurementCrc(float *value)
{
	BYTE error = 0;
	unsigned int i;
	BYTE b;
	unsigned int result = 0;
	for (i=0;i<65535;i++) 
	{
		nop();
		if(DATA==0) break; //wait until sensor has finished the measurement
	}
	if(DATA) error++; // or timeout (~2 sec.) is reached
	b = s_read_byte(ACK); //read the first byte (MSB)
//	crc = CrcByte(b,crc);
	result = 0x100 * b; 
	b = s_read_byte(ACK); //read the second byte (LSB)
//	crc = CrcByte(b,crc);
	result += b; 
	b = s_read_byte(noACK); //read checksum
	b = reverse(b);
	//if(b != crc) error++;
	*value = (float)result;
	return error;
}

BYTE reverse(BYTE c)
{
	c = (c & 0x0F) << 4 | (c & 0xF0) >> 4;
	c = (c & 0x33) << 2 | (c & 0xCC) >> 2;
	c = (c & 0x55) << 1 | (c & 0xAA) >> 1;
	return c;
}

void nop(void)
{
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
_nop_();
}

void main(void)
{
	unsigned int i;

	SFRPAGE = 0x00;

  	Init_Device(); 
	TempHumTimer=0;
	while(1)
	{
		if(TempHumTimer==5)
		{
			s_connectionreset();
			TempHumTimer=0;
		}
		sht(TempHumTimer++);
		for(i=0; i<40000; i++)
		{	
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
		}
	}
}

//------------------------------------------------------------------------------------------
//#endif