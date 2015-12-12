
#include "c8051f120.h"
#include "init.h"
#include "comport.h"
#include <intrins.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
//system
#define SYSCLK       (24856840L * 1 / 1) 
#define FREQT0 8006 //Hz
#define PCACLK	15

//отладочные
sbit P3_3 = P3^3;
sbit LED = P1^6;
sbit LED2 = P2^6;
sbit BUTTON = P3^7;
xdata unsigned char InitFlag;
xdata unsigned int iter;

//ADC
sfr16 ADC0     = 0xbe;   // ADC0 data
bit DavlFlag = 0;
xdata long Result;                    
xdata unsigned int_dec = 256;    
xdata long accumulator = 0L;       
//LIR
#define V_MAX 0x15
#define V_MIN 0x04
#define t_MAX 0x3f

sbit LIR_DATA = P2^2; //P2^0;
sbit LIR_SCK = P2^1;

xdata unsigned char F;
xdata unsigned int delta, LirData, LirTarget, Fdelta, Olddelta;
xdata unsigned int v_anteny=0, v_zadan, t_zadan;

xdata float A=0.0, A_zad=0.0, V=0.0, V_zad=0.0, A_pr=0.0, tau=0.0;

typedef union INT {                   
   unsigned int i;
   unsigned char b[2];
} INT;
//Veter
bit VeterFlag = 0;
sbit WSpeed = P1^1;
sbit WAngle = P1^3;

xdata float Vspeed, Angle;
xdata unsigned long RTC, RTCV, RTCA;
xdata unsigned long TV,TA;

typedef union FLOAT {                   
   float f;
   unsigned char b[4];
} FLOAT;
//UART0
#define NBFM 		50
xdata unsigned char BuferFromModem [NBFM]; 
xdata unsigned char wBFM, rBFM, marBFM;
xdata unsigned int u0timer;

#define SIZE_BUFFER0 		50
xdata char BufferInModem[SIZE_BUFFER0]; 
xdata int r0, rk;
bit flTransmiter;	

//GPS
#define NS 	75
xdata char mess [NS], r, w, mar;		
bit flNewGPS;
xdata unsigned int Vz, koors, flNoKoord, liTimer_tick_GPS, liTimer_tick;
xdata unsigned long LatFly, LonFly;

//------------------------------------------------------------------------------------
void main(void)
{
//ADC
	xdata long Pressure=0;
//UART
//   xdata char RK_code[26], nByte = 0, KontrSumma = 0, PWM;
//	  unsigned int Value;
	INT Aint;
	FLOAT Bfloat;
//GPS
   bit ValidGPS, flPoint; 	
	xdata unsigned char i_comma, tmpGPS[6], nLetter = 7;
	xdata unsigned long temp_koord;
	xdata unsigned int i;

	EA = 0;
	WDTCN     = 0xDE;	//выкл собаки
   WDTCN     = 0xAD;
	EA= 1;	 
	
	LED=0;

	Port_IO_Init();
	Oscillator_Init();   
	Interrupts_Init(); 
	Timer_Init();
	UART0_Init();
   UART1_Init();
	PCA_Init();
   ADC_Init();

	LirTarget = 0x03ff;
	Fdelta = 0;
	flNewGPS = 0;
	u0timer=0;
	while(1)
	{
		for(iter=0; iter<16; iter++)
		{
			_nop_();
		}
		
//ADC
		if(DavlFlag==1)
		{
			EA = 0; 
			Pressure =  Result * 2430 / 4095;
			EA = 1; 
			DavlFlag = 0;
		}
		EIP2      |= 0x02;
//Veter
		if(VeterFlag==1)
		{
			Vspeed = (float)FREQT0/((float)TV*0.75);
			Angle = 360. - (360.*(float)TV/(float)TA);
			VeterFlag = 0;
		}
//GPS---------------------------------------------------------------------------

		if(flNewGPS==1)	
		{
			flNewGPS = 0;
		//rashifrovka GPS
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
						else if (flPoint == 0)						//celaya chast
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
						else												//drobnaya chast
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
						if (mess[r] == 'S')   						//znak Latitude
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
						else if (flPoint == 0)						//celaya chast
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
						else												//drobnaya chast
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
						if (mess[r] == 'W')   						//znak Longitude
							LonFly = 108000000UL-temp_koord;		//180UL*60*10000-koord;
						else       
							LonFly = temp_koord+108000000UL;		//180UL*60*10000;
					}
					else if (i_comma == 7)							//skorost v uzlah
					{
						if(mess[r] == '.')
						{
							flPoint = 1;
   		      		Vz = 1.852*atoi(tmpGPS)/3.6;   		//preobrazovat iz uzlov v m/s
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
					else if (i_comma == 8)							//kurs v gradusah
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


//COM
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
			//ES0=0;
		}
	}
}
//------------------------------------------------------------------------------------
void INT0 (void) interrupt 0 //skorost vetra
{
	EX0 =	0;
	VeterFlag = 1;
	TV = RTC - RTCV;
	RTCV = RTC;
	return;
}

//------------------------------------------------------------------------------------
void TIMER_ISR0 (void) interrupt 1
{
	xdata char SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = TIMER01_PAGE;
	RTC++;
	u0timer++;
	if(WSpeed!=0)
		EX0=1;

	if(WAngle!=0)
		EX1=1;

	SFRPAGE = SFRPAGE_SAVE;
	return;
}



//------------------------------------------------------------------------------------
void INT1 (void) interrupt 2 //napravl vetra
{
  	EX1=0;
	TA = RTC - RTCA;
	RTCA = RTC;
	return;
}

//------------------------------------------------------------------------------------
void COM_UART0_isr(void) interrupt 4
{
	xdata char SFRPAGE_SAVE = SFRPAGE;
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

//upravlenie antenoi---------------------------------------------------
void PCA_isr (void)interrupt 9
{
	float tmp;
	unsigned char shim;
	xdata char SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = PCA0_PAGE;
	CF = 0;

	EA = 0;	//опрос ЛИР
	LirData = 0;
			
	LIR_SCK = 1;	//---init
	LIR_SCK = 0;
			
	LIR_SCK = 1;	//---D n-1
	LIR_SCK = 0;
			
	LIR_SCK = 1;	//---D n-1
	LIR_SCK = 0;
	if(LIR_DATA != 0) 
		LirData = LirData | (0x01 << (11));

	LIR_SCK = 1; 
	LIR_SCK = 0;
	if(LIR_DATA != 0) 
		LirData = LirData | (0x01 << (10));

	LIR_SCK = 1; 
	LIR_SCK = 0;
	if(LIR_DATA != 0) 
		LirData = LirData | (0x01 << (9));

	LIR_SCK = 1; 
	LIR_SCK = 0;
	if(LIR_DATA != 0) 
		LirData = LirData | (0x1 << (8));

	LIR_SCK = 1; 
	LIR_SCK = 0;
	if(LIR_DATA!=0) 
		LirData = LirData | (0x1 << (7));

	LIR_SCK = 1; 
	LIR_SCK = 0;
	if(LIR_DATA!=0) 
		LirData = LirData | (0x1 << (6));

	LIR_SCK = 1; 
	LIR_SCK = 0;
	if(LIR_DATA!=0) 
		LirData = LirData | (0x1 << (5));

	LIR_SCK = 1; 
	LIR_SCK = 0;
	if(LIR_DATA!=0) 
		LirData = LirData | (0x1 << (4));

	LIR_SCK = 1; 
	LIR_SCK = 0;
	if(LIR_DATA!=0) 
		LirData = LirData | (0x1 << (3));

	LIR_SCK = 1; 
	LIR_SCK = 0;
	if(LIR_DATA!=0) 
		LirData = LirData | (0x1 << (2));

	LIR_SCK = 1; 
	LIR_SCK = 0;
	if(LIR_DATA!=0) 
		LirData = LirData | (0x1 << (1));

	LIR_SCK = 1; 
	LIR_SCK = 0;
	if(LIR_DATA!=0) 
		LirData = LirData | (0x1 << (0));

	LIR_SCK = 1; 
	EA = 1;

	tmp = 360.0/4095.0*LirData;
	A = A+(tmp-A)/PCACLK*1.5;

	V = V+((A-A_pr)*PCACLK-V)/PCACLK*1.5;
	if (V > 500) 
		V = 500;
	else if (V < -500) 
		V = -500;
	A_pr = A;
	if(LED==1)
	{
		LED=0;
	}

/*
         H_filtr = H_filtr+(H-H_filtr)/FREQ*0.8;
         H_dat = H_dat+(H-H_dat)/FREQ/0.3;
         Vy_dat = Vy_dat+((H_dat-H_dat_pr)*FREQ-Vy_dat)/FREQ/0.3;
         H_dat_pr = H_dat;


		   while (tmp > M_PI)
      		tmp -= D_PI;
		   while (tmp < -M_PI)
      		tmp += D_PI;

		   tmp = ToGrad*tmp;		
			if(tmp > 42)
				tmp = 42;
		   else if(tmp < -42)
				tmp = -42;
			Kren_zad_buf = tmp;
		}
*/

	//управление антеной--------------------------------------------------
	
	V_zad = (A_zad-A);
   while (V_zad > 180)
  		V_zad -= 360;
   while (V_zad < -180)
  		V_zad += 360;	

	V_zad = V_zad/0.85;	
	if (V_zad > 400)
		V_zad = 400;
	else if (V_zad < -400)
		V_zad = -400;

	tau = (V_zad-V)/400*0.000025;

	if (tau > 0.000025)
		tau = 0.000025;
	else if (tau < -0.000025)
		tau = -0.000025;

	tmp = 0x3f * fabs(tau)/0.000025; 
	shim = tmp;
	shim = 0xff - shim;
	if (tau > 0)
	{
		PCA0CN    = 0x40;
	   PCA0MD    = 0x01;
   	PCA0CPM0  = 0x43;
	   PCA0CPM1  = 0x43;
	  	PCA0CPH0  = 0xFF;
   	PCA0CPH1  = shim;
	}
	else
	{
		PCA0CN    = 0x40;
   	PCA0MD    = 0x01;
	   PCA0CPM0  = 0x43;
   	PCA0CPM1  = 0x43;
	   PCA0CPH0  = shim;
   	PCA0CPH1  = 0xFF;
	}

	SFRPAGE = SFRPAGE_SAVE;
	return;
}

//-------------------------------------------------------------------

void ADC0_ISR (void) interrupt 15
{
  	xdata char SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = ADC0_PAGE;
	AD0INT = 0;                         // Clear ADC conversion complete
                                       // indicator
   accumulator += ADC0;                // Read ADC value and add to running
                                       // total
   int_dec--;                          // Update decimation counter

   if (int_dec == 0)                   // If zero, then post result
   {
		int_dec = 256;               // Reset counter
      Result = accumulator >> 8;
      accumulator = 0L;                // Reset accumulator
		DavlFlag = 1;
		EIP2  &= ~0x02;
   }
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


//#endif


