
#include "c8051f120.h"
#include "init_t.h"
#include <intrins.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
//system
#define SYSCLK      24810000  // (24856840L * 1 / 1) 
#define FREQT0 580 //1183 //Hz
#define FREQPCA 15.799 //Hz
//����������
sbit P3_3 = P3^3;
sbit LED = P1^6;  // ��� 1
sbit LED2 = P2^6; // ��� 2
sbit BUTTON = P3^7;

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
xdata float A = 0, A_pr = 0, A_zad = 0, V = 0, V_pr = 0, V_zad = 0, V_zad_pr = 0,  Vmax = 400, u = 0, u_zad = 0, Umax = 36000;

#define UgolMaxV 4.

//Veter
bit VeterFlag = 0;
sbit WSpeed = P1^1;
sbit WAngle = P1^3;

xdata float Vspeed=0, Angle=0;
xdata unsigned int RTC=0, RTCV=0, RTCA=0;
xdata float TV=0,TA=0;

//UART0
#define NBFM 		50
xdata unsigned char BuferFromModem [NBFM]; 
xdata unsigned char wBFM, rBFM, marBFM;
xdata unsigned int u0timer;

#define SIZE_BUFFER0 		50
xdata char BufferInModem[SIZE_BUFFER0]; 
xdata int r0, rk;
bit flTransmiter;	

xdata unsigned char startbit45, startbit46;
xdata unsigned int P_signal=0;
xdata long Lat_Bort=0, Lon_Bort=0, LonFly=0, LatFly=0;
xdata float dy, dx, pi=3.14;
bit flNewAngle;

//GPS

#define NS 	75
xdata char mess [NS], r, w, mar;		
bit flNewGPS;
xdata unsigned int Vz, koors, flNoKoord, liTimer_tick_GPS, liTimer_tick;
xdata unsigned long LatGnd=0, LonGnd=0;


#define cT 		0x885e //0xA04B
#define ctau 	0x2FDA //0X17ED

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

//GPS
   bit ValidGPS, flPoint; 	
	xdata unsigned char i_comma, tmpGPS[6], nLetter = 7;
	xdata unsigned long temp_koord;
	xdata unsigned int i;

   xdata unsigned char chtmp;
	xdata int tmp;

	EA = 0;
	WDTCN     = 0xDE;	//���� ������
   WDTCN     = 0xAD;
	EA= 1;	 
	
	LED=0;
	LED2=0;

	Port_IO_Init();
	Oscillator_Init();   
	Interrupts_Init(); 
	Timer_Init();
	UART0_Init();
   UART1_Init();
	PCA_Init();
   ADC_Init();

	flNewGPS = 0;
	u0timer=0;

	while(1)
	{
//ADC
		EIP2 |= 0x02;
		if(DavlFlag==1)
		{
			EA = 0; 
			Pressure =  Result * 2430 / 4095;
			EA = 1; 
			DavlFlag = 0;
			PressureFl = PressureFl + (Pressure - PressureFl)*0.5;
		}

		//GPS---------------------------------------------------------------------------
		if(flNewGPS == 1)	
		{
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
			flNewGPS = 0;
		}

		//---------------------------------------------------------------------

		if(flTransmiter == 0 && (u0timer > 2*FREQPCA))
		{
			u0timer = 0;

			if(flNewAngle==1)
			{
				flNewAngle=0;
				if(wBFM > 10)
				{
					startbit45 = 0;
					while(startbit45<(wBFM-10))
					{
						chtmp = 0;
						for (i = startbit45; i < (11+startbit45); i++ )
  	   	  				chtmp = chtmp ^ BuferFromModem[i];
						chtmp = chtmp | 0x80;
						if(BuferFromModem[11+startbit45]!= 0x80 && chtmp == BuferFromModem[11+startbit45] && BuferFromModem[startbit45] == (45 | 0x40))
						{
							Lat_Bort = BuferFromModem[1+startbit45]&~0x80;
			         	Lat_Bort |= ((long)(BuferFromModem[2+startbit45]&~0x80)<<7);
      			   	Lat_Bort |= ((long)(BuferFromModem[3+startbit45]&~0x80)<<14);
         				Lat_Bort |= ((long)(BuferFromModem[4+startbit45]&~0x80)<<21);
							Lon_Bort = BuferFromModem[5+startbit45]&~0x80;
		         		Lon_Bort |= ((long)(BuferFromModem[6+startbit45]&~0x80)<<7);
      		   		Lon_Bort |= ((long)(BuferFromModem[7+startbit45]&~0x80)<<14);
	         			Lon_Bort |= ((long)(BuferFromModem[8+startbit45]&~0x80)<<21);
							P_signal =	BuferFromModem[9 + startbit45]&~0x80;
      	   			P_signal |= ((int)(BuferFromModem[10 + startbit45]&~0x80)<<7);
									
							dy = LonGnd;
							dy = 0.1856*(dy - LonFly);
		   				dx = LatGnd;
		  					dx = 0.1856*(dx - LatFly);
							if(dx>0 && dy>=0)
							{
								tmp = atan(dy/dx);
							}
							else if(dx>0 && dy<0)
							{
								tmp = atan(dy/dx)+2*pi;
							}
							else if(dx<0)
							{
								tmp = atan(dy/dx)+pi;
							}
							else if(dx==0 && dy>0)
							{
								tmp = pi/2;
							}
							else if(dx==0 && dy<0)
							{
								tmp= 3*pi/2;
							}
							else if(dx==0 && dy==0)
							{
								tmp =0;
							}	
							
							tmp = tmp / pi * 180.;
							if(tmp<=360 && tmp>=0)
							{
								A_zad = tmp;
    						}
						}
						startbit45++;
					}
				}
				
				if(wBFM > 2)
				{   			
					startbit46 = 0;
					while(startbit46<(wBFM-2))
					{
						chtmp = 0;
						for (i = startbit46; i < (3+startbit46); i++ )
  		     				chtmp = chtmp ^ BuferFromModem[i];
						chtmp = chtmp | 0x80;
						if(BuferFromModem[3+startbit46] != 0x80 && chtmp == BuferFromModem[3+startbit46] && BuferFromModem[startbit46] == (46 | 0x40))
						{
							tmp = BuferFromModem[1+startbit46]&~0x80;
		      			tmp |= ((int)(BuferFromModem[2+startbit46]&~0x80)<<7);
							if(tmp<=360 && tmp>=0)
							{
								A_zad = tmp;
    						}
							
						}
						startbit46++;
					}
					wBFM = 0;
				}
			}
	
			BufferInModem[0] = 0x40 | 40;		
			if(ValidGPS)
		   {
				OutModem4(LatGnd, 1);
				OutModem4(LonGnd, 5);
			}
			else
   		{
   			i=1;
				while(i<9)
				{
					if(i==4)
						OutModem1(0xff, i++);
					else
						OutModem1(0x80, i++);
				}
         }
			OutModem2((int)Angle, 9);
			OutModem2((int)Vspeed, 11);
			OutModem2((int)PressureFl, 13);
   		BufferInModem[15] = 0;

			for (i = 0; i < 15; i++ )
		     	BufferInModem[15] = BufferInModem[15] ^ BufferInModem[i];
			BufferInModem[15] = 0x80 |	BufferInModem[15];

			BufferInModem[16] = 0x40 | 41;
			OutModem2((int)A, 17);
	     	BufferInModem[19] = BufferInModem[16] ^ BufferInModem[17] ^ BufferInModem[18] | 0x80;
			r0 = 0;
		 	rk = 20;
			
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

	if(flShim == 0)
	{	
		flShim = 1;

		LData = 0;
		EA = 0;	//����� ���
	
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
	
		//���������� �������------------------------------------------------------------------------
		tmp = 360.0/4095.0*LData;
		if ((A > 270) && (tmp < 90))
		{
			A = A+(tmp+360-A);
			if (A > 360)
				A = A-360;
		}
		else if ((tmp > 270) && (A < 90))
		{
			A = A+(tmp-A-360);
			if (A < 0)
				A = A+360;
		}
		else
			A = A+(tmp-A);

		if ((A_pr > 270) && (A < 90))
			V = V+((A+360-A_pr)*FREQT0-V); 
		else if ((A > 270) && (A_pr < 90))
			V = V+((A-A_pr-360)*FREQT0-V); 
		else
			V = V+((A-A_pr)*FREQT0-V); 
		A_pr = A;

		u = u+((V-V_pr)*FREQT0-u);
		V_pr = V;
	
		V_zad = A_zad-A;
		while (V_zad > 180)
   	  	V_zad -= 360;
		while (V_zad < -180)
   	  	V_zad += 360;
		
		V_zad = (Vmax/UgolMaxV)*V_zad;
		if(V_zad>Vmax)
			V_zad=Vmax;
		else if(V_zad<-Vmax)
			V_zad=-Vmax;

		u_zad = u_zad+((V_zad-V_zad_pr)*FREQT0-u_zad);
		V_zad_pr = V_zad;
		if(u_zad>Umax)
			u_zad=Umax;
		else if(u_zad<-Umax)
			u_zad=-Umax; 		

		tmp = A_zad-A;
		while (tmp > 180)
   	  	tmp -= 360;
		while (tmp < -180)
   	  	tmp += 360;
		
		if(fabs(tmp)<(UgolMaxV))
			tmp = (u_zad-u)/Umax*ctau + (V_zad-V)/Vmax*ctau;
		else
			tmp = (V_zad-V)/Vmax*ctau;

		if (tmp > (ctau)) 
			tmp = ctau;
		else if (tmp < -(ctau)) 
			tmp = -(ctau);

		if (tmp > 512)
		{
			shim = 0xffff-tmp;
	   	LED2 = 0;
			LED = 1;
			THTL_mem = TH0;
			THTL_mem = ((THTL_mem << 8) & 0x00ff) + TL0;
			TH0 = (shim & 0xff00) >> 8;
		   TL0 = shim & 0x00ff;
			
			shim = 0xffff-0x3fff*fabs(tmp)/(float)ctau;
			PCA0CPH0  = 0xFF;
			PCA0CPL0  = 0xFF;
			PCA0CPH1  = (shim & 0xff00) >> 8;
   		//PCA0CPL1  = shim & 0x00ff;
		}
		else if(tmp < -512)
		{
			shim = 0xffff+tmp;
		   LED = 0;
			LED2 = 1;
			THTL_mem = TH0;
			THTL_mem = ((THTL_mem << 8) & 0x00ff) + TL0;
			TH0 = (shim & 0xff00) >> 8;
		   TL0 = shim & 0x00ff;
			
			shim = 0xffff-0x3fff*fabs(tmp)/(float)ctau;
			PCA0CPH1  = 0xff;	
			PCA0CPL1	 = 0xff;
			PCA0CPH0  = (shim & 0xff00) >> 8;
			//PCA0CPL0  = shim & 0x00ff;
		}
		else
		{
	   	shim = 0xffff - ctau;
		   LED = 0;
			LED2 = 0;
			TH0 = (shim & 0xff00) >> 8;
		   TL0 = shim & 0x00ff;
			
			PCA0CPH0  = 0xFF;
   		PCA0CPH1  = 0xFF;
		}
	//	P3_3=0;
	}
	else
	{
		flShim = 0;
		LED = 0;
		LED2 = 0;
		
		if((THTL_mem + (0xffff-shim))<(0xffff-cT))
		{
			shim = cT + THTL_mem + (0xffff-shim);
			TH0 = (shim & 0xff00) >> 8;
			TL0 = shim & 0x00ff;
		}
		else
		{
			shim = 0xffff; 
			TH0 = (shim & 0xff00) >> 8;
			TL0 = shim & 0x00ff;
		}
	}
	SFRPAGE = SFRPAGE_SAVE;
	return;
}

//------------------------------------------------------------------------------------
void INT1 (void) interrupt 2 //napravl vetra
{
  	EX1=0;
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
		flNewAngle=1;
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
//FREQPCA 15.799 Hz
	xdata float tmp_a;
	xdata char SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = PCA0_PAGE;
	CF = 0; 										//����� ����� ���������� ��������������
	RTC++;
	u0timer++;

	if(WSpeed != 0)
		EX0 = 1;

	if(WAngle != 0)
		EX1 = 1;

	if(RTC<RTCV)
		TV = TV + ((0xffff - RTCV + RTC) - TV)/FREQPCA;
	else if(RTC>RTCV)
		TV = TV + ((RTC - RTCV) - TV)/FREQPCA;		

	if(RTC<RTCA)
		TA = TA + ((0xffff - RTCA + RTC) - TA)/FREQPCA;
	else 
		TA = TA + ((RTC - RTCA) - TA)/FREQPCA;

	
	tmp_a = 0;
	if(TV > 0)
		tmp_a = TA/TV;
	if(tmp_a <= 1 && tmp_a >= 0)
		Angle = Angle + (360. - (360.*tmp_a) - Angle)/FREQPCA*0.8;

	
	Vspeed = Vspeed + (((float)FREQPCA/((float)TV*0.75))-Vspeed)/FREQPCA;

//	P3_3=!P3_3;
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


