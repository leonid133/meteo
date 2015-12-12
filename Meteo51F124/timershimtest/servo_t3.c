
#include "c8051f120.h"
#include "init_t.h"
//#include "comport.h"
#include <intrins.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
//system
#define SYSCLK      24883200  // (24856840L * 1 / 1) 
#define FREQT0 1//1183 //Hz

//сигнальные
sbit P3_3 = P3^3;
sbit LED = P1^6;
sbit LED2 = P2^7;
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
sbit LIR_DATA = P2^0; //P2^2;
sbit LIR_SCK = P2^1;
xdata unsigned int LData, THTL_mem=0;
xdata float A = 0, A_pr = 0, A_zad = 0, A_zad_pr = 0, V = 0, V_pr = 0, V_zad = 0, V_zad_pr = 0, u = 0, u_zad = 0, Accel=0, tau=0.0, K = 1.0;
/*
typedef union INT {                   
   unsigned int i;
   unsigned char b[2];
} INT;
*/
xdata int  UgolMaxV = 4, Vmax = 200, Umax = 2;
xdata float taumax = 0.000025;

xdata unsigned char F;
xdata unsigned int delta, LirData, LirTarget, Fdelta;
//Veter
bit VeterFlag = 0;
sbit WSpeed = P1^1;
sbit WAngle = P1^3;

xdata float Vspeed=0, Angle=0;
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
typedef union LINT {                   
   unsigned long i;
   unsigned char b[4];
} LINT;
#define NS 	75
xdata char mess [NS], r, w, mar;		
bit flNewGPS;
xdata unsigned int Vz, koors, flNoKoord, liTimer_tick_GPS, liTimer_tick;
xdata unsigned long LatGnd=0, LonGnd=0;


#define cT 		0xA04B
#define ctau 	0X17ED

//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------------
void main(void)
{
//ADC
	xdata long Pressure = 0;
	xdata float PressureFl = 0.0;
//UART
//   xdata char RK_code[26], nByte = 0, KontrSumma = 0, PWM;
//	  unsigned int Value;
//	INT Aint;
	FLOAT Bfloat;
	LINT tmpLong;
//GPS
   bit ValidGPS, flPoint; 	
	xdata unsigned char i_comma, tmpGPS[6], nLetter = 7;
	xdata unsigned long temp_koord;
	xdata unsigned int i;

   xdata unsigned char chtmp;
	xdata int tmp;

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
//   UART1_Init();
//	PCA_Init();
//  ADC_Init();

//	LirTarget = 0x03ff;
//	Fdelta = 0;
	flNewGPS = 0;
	u0timer=0;

	while(1)
	{
//ADC
/*		EIP2 |= 0x02;
		if(DavlFlag==1)
		{
			EA = 0; 
			Pressure =  Result * 2430 / 4095;
			EA = 1; 
			DavlFlag = 0;
			PressureFl = PressureFl + (Pressure - PressureFl)*0.5;
		}

//Veter
		if(VeterFlag==1)
		{
			Vspeed = (float)FREQT0/((float)TV*0.75);	//???
			Angle = 360. - (360.*(float)TV/(float)TA);
			VeterFlag = 0;
		}

		//GPS---------------------------------------------------------------------------
		if(flNewGPS == 1)	
		{
			flNewGPS = 0;
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
//lLatGnd = 55UL*60*10000+50UL*10000+8680;
//--------
						if (mess[r] == 'S')   						//znak Latitude
							LatGnd = 54000000UL-temp_koord;		//90UL*60*10000-koord;
						else		  
							LatGnd = 54000000UL+temp_koord;		//90UL*60*10000+koord;
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
//lLonGnd = 49UL*60*10000+6UL*10000+3760;
//----------------
						if (mess[r] == 'W')   						//znak Longitude
							LonGnd = 108000000UL-temp_koord;		//180UL*60*10000-koord;
						else       
							LonGnd = temp_koord+108000000UL;		//180UL*60*10000;
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
*/
		//---------------------------------------------------------------------

//if(u0timer > (3*FREQT0))
//		if((flTransmiter == 0) && (u0timer > FREQT0*0.3))
		if(flTransmiter == 0 && (u0timer >1000))
		{
			u0timer = 0;
			if(wBFM > 4)
			{
   			
				chtmp = 0;
				for (i = 0; i < 5; i++ )
  	     			chtmp = chtmp ^ BuferFromModem[i];
				chtmp = chtmp | 0x80;
				if(chtmp == BuferFromModem[5] && BuferFromModem[0] == (46 | 0x40))
				{
					tmp = BuferFromModem[1]&~0x80;
		      	tmp |= ((int)(BuferFromModem[2]&~0x80)<<7);
					if(tmp<=360 && tmp>=0)
					{
						A_zad = tmp;
						tmp = BuferFromModem[3]&~0x80;
						tmp |= ((int)(BuferFromModem[4]&~0x80)<<7);
						K = 0.01*tmp;
					}
				}
				wBFM = 0;
			}

			BufferInModem[0] = 0x40 | 40;			
			OutModem4(LatGnd, 1);
			OutModem4(LonGnd, 5);
			OutModem2((int)Angle, 9);
			OutModem2((int)Vspeed, 11);
			OutModem2((int)PressureFl, 13);
   		BufferInModem[15] = 0;
			for (i = 0; i < 15; i++ )
		     	BufferInModem[15] = BufferInModem[15] ^ BufferInModem[i];
			BufferInModem[15] = 0x80 |	BufferInModem[15];

			BufferInModem[16] = 0x40|41;
			OutModem2((int)A, 17);
	     	BufferInModem[19] = BufferInModem[16] ^ BufferInModem[17] ^ BufferInModem[18] | 0x80;
			r0 = 0;
		 	rk = 20;
			//dopolnitelnye dannye
			tmp = (int)(Accel);
			OutModem2(tmp, 20);
			rk+=2;
			//A_zad, V, V_zad
			tmp = (int)(A_zad*10.);
			OutModem2(tmp, 22);
			tmp = (int)(V*10.);
			OutModem2(tmp, 24);
			tmp = (int)(V_zad*10.);
			OutModem2(tmp, 26);
			rk+=6;
			//	
			flTransmiter = 1;
			
			SFRPAGE = 0x00;
			TI0 = 1;
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
	xdata float tmp;
	static xdata  unsigned int shim;
	xdata char SFRPAGE_SAVE = SFRPAGE;
	static xdata char flShim = 0;
	SFRPAGE = TIMER01_PAGE;
	RTC++;
	u0timer++;

/*	
	if(WSpeed != 0)
		EX0 = 1;

	if(WAngle != 0)
		EX1 = 1;
*/
if(flShim == 0)
{	
		flShim = 1;

		LData = 0;
		EA = 0;	//опрос ЛИР
	
		LIR_SCK = 1;	shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LIR_SCK = 0;	shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
			
		LIR_SCK = 1;					shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LIR_SCK = 0;					shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;	
		LData = LData | LIR_DATA;	

		LIR_SCK = 1; 							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LIR_SCK = 0;							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LData = (LData << 1) | LIR_DATA;	
	
		LIR_SCK = 1; 							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LIR_SCK = 0;							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LData = (LData << 1) | LIR_DATA;	

		LIR_SCK = 1; 							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LIR_SCK = 0;							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LData = (LData << 1) | LIR_DATA;	
	
		LIR_SCK = 1; 							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LIR_SCK = 0;							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LData = (LData << 1) | LIR_DATA;	

		LIR_SCK = 1; 							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LIR_SCK = 0;							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LData = (LData << 1) | LIR_DATA;	

		LIR_SCK = 1; 							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LIR_SCK = 0;							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LData = (LData << 1) | LIR_DATA;	

		LIR_SCK = 1; 							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LIR_SCK = 0;							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LData = (LData << 1) | LIR_DATA;	

		LIR_SCK = 1; 							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LIR_SCK = 0;							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LData = (LData << 1) | LIR_DATA;	

		LIR_SCK = 1; 							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LIR_SCK = 0;							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LData = (LData << 1) | LIR_DATA;	

		LIR_SCK = 1; 							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LIR_SCK = 0;							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LData = (LData << 1) | LIR_DATA;	

		LIR_SCK = 1; 							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LIR_SCK = 0;							shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;shim++;
		LData = (LData << 1) | LIR_DATA;

		LIR_SCK = 1; 
		EA = 1;
	
		//управление антеной------------------------------------------------------------------------
		tmp = 360.0/4095.0*LData;
		if ((A > 270) && (tmp < 90))
		{
			A = A+(tmp+360-A)/FREQT0*K;
			if (A > 360)
				A = A-360;
		}
		else if ((tmp > 270) && (A < 90))
		{
			A = A+(tmp-A-360)/FREQT0*K;
			if (A < 0)
				A = A+360;
		}
		else
			A = A+(tmp-A)/FREQT0*K;

		if ((A_pr > 270) && (A < 90))
			V = V+((A+360-A_pr)*FREQT0-V)/FREQT0*K; 
		else if ((A > 270) && (A_pr < 90))
			V = V+((A-A_pr-360)*FREQT0-V)/FREQT0*K; 
		else
			V = V+((A-A_pr)*FREQT0-V)/FREQT0*K; 
		A_pr = A;

		u = u+((V-V_pr)*FREQT0-u)/FREQT0*K;
		V_pr = V;
	
		V_zad = A_zad-A;
		while (V_zad > 180)
   	  	V_zad -= 360;
		while (V_zad < -180)
   	  	V_zad += 360;

/*		if ((V_pr > ) && (V < 90))
			u = u+((A+360-A_pr)*FREQT0-u_zad)/FREQT0*K; 
		else if ((V > 270) && (V_pr < 90))
//			u = u+((A-A_pr-360)*FREQT0-u_zad)/FREQT0*K; 
			u_zad = u_zad+((V_zad-V_zad_pr)*FREQT0-u_zad)/FREQT0*K;
		else
			u_zad = u_zad+((V_zad-V_zad_pr)*FREQT0-u_zad)/FREQT0*K; */

		V_zad = 50*V_zad;
		if(V_zad>Vmax){V_zad=Vmax;}
		else if(V_zad<-Vmax){V_zad=-Vmax;}

		u_zad = u_zad+((V_zad-V_zad_pr)*1000-u_zad)/FREQT0*K;
		V_zad_pr = V_zad;
		
	//	tmp = (12*u_zad-u)*ctau + (10*V_zad-V)/Vmax*ctau;
//		tmp =((10*V_zad-V)/Vmax*ctau)/(1+(12*u_zad-u)*ctau*(10*V_zad-V)/Vmax*ctau);
		tmp = (u_zad-u)/Umax*ctau + 0.5*(V_zad-V)/Vmax*ctau + 0.2*(A_zad-A)/360*ctau;
//		tmp = ((V_zad-V)/Vmax*ctau)/(1+(u_zad-u)*ctau*(V_zad-V)/Vmax*ctau)+0.2*(A_zad-A)/360*ctau;

		if (tmp > (ctau)) tmp = ctau;
		if (tmp < -(ctau)) tmp = -(ctau);

		if (tmp > 512)
		{
			shim = 0xffff-tmp;
	   	LED2 = 0;
			LED = 1;
			TH0 = (shim & 0xff00) >> 8;
		   TL0 = shim & 0x00ff;
		}
		else if(tmp < -512)
		{
			shim = 0xffff+tmp;
		   LED = 0;
			LED2 = 1;
			TH0 = (shim & 0xff00) >> 8;
		   TL0 = shim & 0x00ff;
		}
		else
		{
	   	LED = 0;
			LED2 = 0;
			flShim = 0;
		}
	}
	else
	{
		flShim = 0;
		LED = 0;
		LED2 = 0;
		
		shim = cT + 4*ctau;
		TH0 = (shim & 0xff00) >> 8;
		TL0 = shim & 0x00ff;
	}
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
		if(r0 < rk)
		{
			SBUF0 = BufferInModem[r0++];
		}
		else
		{
			flTransmiter = 0;
		}
		TI0 = 0;
  	}
	
	SFRPAGE = SFRPAGE_SAVE;
	return;
}

//-------------------------------------------------------------------

void PCA_isr (void)interrupt 9 
{
	xdata char SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = PCA0_PAGE;
	CF = 0; 										//Сброс флага прерывания шимогенератора
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
		int_dec = 256;               		// Reset counter
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


