#include "c8051f120.h"
#include "init.h"
#include "comport.h"
#include <intrins.h>
#include <stdio.h>                     
#include <ctype.h>

#define FREQT1 2041666 //510417

sbit LED = P1^6;
sbit LED2 = P2^6;
sbit BUTTON = P3^7;
//шим на машинку
sbit P1_5 = P1^5;
sbit P1_7 = P1^7;
//--------------
//лир
sbit LIR_DATA = P2^0;
sbit LIR_SCK = P2^1;

xdata unsigned char F;
xdata unsigned int LirData, LirTarget, Fdelta;

typedef union INT {                   
   unsigned int i;
   unsigned char b[2];
} INT;
//---
//ветер
sbit P1_1=P1^1;
sbit P0_7=P0^7;
//xdata unsigned int Vtimer, Atimer;
xdata float Vspeed, Angle, dTimerTickV, dTimerTickA;
xdata unsigned char InitFlag;
xdata unsigned long timer_tick1, timer_tickA, timer_tickB;

typedef union FLOAT {                   
   float f;
   unsigned char b[4];
} FLOAT;
//-----
//UART0
#define NBFM 		50
xdata unsigned char BuferFromModem [NBFM]; 
xdata unsigned char wBFM, rBFM, marBFM;

#define SIZE_BUFFER0 		50
xdata char BufferInModem[SIZE_BUFFER0]; // Для отправки в последовательный порт
xdata int r0, rk;
bit flTransmiter;	

#define NS 	75
xdata char mess [NS], r, w, mar;		// Для анализа посылки GPS

/*
void UART0_isr(void);
void TIMER_ISR(void);
void TIMER_ISR3(void);*/

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
//------------------------------------------------------------------------------------------
void main(void)
{
	xdata unsigned int i;
  	INT Aint;
	FLOAT Bfloat;
	//UART----------
   xdata char RK_code[26], nByte = 0, KontrSumma = 0, PWM;
	unsigned int Value;

	//--------------
 /*  bit ValidGPS, flPoint; 	
	xdata unsigned char i, i_comma, tmpGPS[6], nLetter = 7;
	xdata unsigned long temp_koord;*/

	EA=0;
	WDTCN = 0xDE;                       // Disable watchdog timer
   WDTCN = 0xAD;   
	P1_5=0;
	P1_7=0;
	Init_Device();
	EA = 1;                             // Enable global interrupts
	LirTarget = 0x03ff;
	Fdelta = 0;
	if(InitFlag!=0x00)
	{
		Vspeed=0.0;
		Angle=0.0;
		dTimerTickV=0.0;
		dTimerTickA=0.0;
	
	 	wBFM=0;
		rBFM=0;
		marBFM=0;
		InitFlag=0x00;
	}

//   while(1)
	{
/*
	 	//Read redy UART0-------------------------------------
		TI0=1;
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
			   //TI0   = 0; 
    			//while( !TI0 );
			}
			else
			{
				flTransmiter = 0;
			}
		}
*/		//------------------------------------------------------------
	}

//UART----------------------------
/*	rBFM = wBFM = marBFM = 0;
	for(PWM = 1; PWM < 15; PWM++)
	{
		Value = 37500;
		write(PWM+112, Value);
	}

	while(1)
	{
   	if(rBFM < wBFM+marBFM*NBFM)
   	{
			if ((BuferFromModem[rBFM] & 0xC0) == 0x40)
			{
				nByte = 0;
				KontrSumma = 0;
				PWM = BuferFromModem[rBFM] & 0x3f;
			}
			if (nByte > 25)
				nByte = 25;
			RK_code[nByte] = BuferFromModem[rBFM] & 0x7f;
			KontrSumma = KontrSumma^RK_code[nByte++];

			if ( (nByte == 5) && (KontrSumma == 0) )
			{
				Value = RK_code[1]+((unsigned int)RK_code[2] << 7)+((unsigned int)RK_code[3] << 14);
				write(PWM+112, Value);
			}

         rBFM++;
			if(rBFM >= NBFM)
			{
   			rBFM = 0;
				marBFM = 0;
			}
      }
				//-----------------------------------------------------------------------------------
		if(flNewGPS)	
		{
			flNewGPS = 0;
         OutModem20();

			if (RegimeKren == 0 || RegimeStart == 0) //не ручное управление
				break; 
start1:
			dz = LonMar[n_];
			dz = 0.1856*(dz - LonFly)*cos_Lat0;
		   dx = LatMar[n_];
		   dx = 0.1856*(dx-LatFly);

//		  	if(RegimeKren)//автономный полет
//			{
	   		dz_pr = LonMar[n_-1];
			   dz_pr = 0.1856*(dz_pr-LonFly)*cos_Lat0;
   			dx_pr = LatMar[n_-1];
		   	dx_pr = 0.1856*(dx_pr-LatFly);

			   tmp =sqrt(dz*dz+dx*dx);		//tmp = rasst_toch_mar
   			if ((EPSILON > tmp) && (n_ != i_mar) && (n_ != 0))
			   {
      			if ((n_ == i_mar-2) && (fabs(H_dat-100) > 50))
         			;
      			else
      			{
         			n_++;
         			flCommand = 1;

         			if (RegimeV == 2)
               		Vz_zad = Vz_Mar[n_];
         			if (RegimeVy == 2)
               		H_zad_buf = H_Mar[n_];
         			goto start1;
      			}
//   			}

        	   if ( (fabs(dx-dx_pr) <= FLT_EPSILON) && (fabs(dz-dz_pr) <= FLT_EPSILON) )
  		      	napr_vetv_mar = 0;
        	  	else
  		      	napr_vetv_mar = atan2(dz-dz_pr, dx-dx_pr);

      		if ( (fabs(dx) <= FLT_EPSILON) && (fabs(dz) <= FLT_EPSILON) )
		      	angle = 0;
      		else
		      	angle = atan2(dz, dx);	//napr_toch_mar = atan2(dz, dx),
      		angle = angle-napr_vetv_mar;

			   otkl_ot_mar = tmp*sin(angle);
				if (fabs(otkl_ot_mar) > 500) tmp = 200;	//tmp = l_km
				else                         tmp = 500;
			   dz = otkl_ot_mar*cos(napr_vetv_mar)+tmp*sin(napr_vetv_mar);	//dz = z_toch_pricel
   			dx = -otkl_ot_mar*sin(napr_vetv_mar)+tmp*cos(napr_vetv_mar);	//dx = x_toch_pricel
			}
			tmp = koors;
			tmp =  -tmp/ToGrad;
    		if ( (fabs(dx) > FLT_EPSILON) && (fabs(dz) > FLT_EPSILON) )
				tmp =  tmp + atan2(dz, dx);

		   while (tmp > M_PI)
      		tmp -= D_PI;
		   while (tmp < -M_PI)
      		tmp += D_PI;

		   tmp = ToGrad*tmp;		
			if(tmp > 42)
				tmp = 42;
		   else if(tmp < -42)
				tmp = -42;
			kren_zad_buf = tmp;
		}
      else if (liTimer_tick-liTimer_tick_GPS > 2*FREQ) //Отсутствие координат
		{
			flNoKoord = 1;
			liTimer_tick_GPS = liTimer_tick;	
         OutModem20();
		}

		//Расшифровка посылки GPS
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
				if (i_comma == 3)                //Latitude
				{
					if(mess[r] == '.')
					{
						flPoint = 1;
						i = 0;
					}
					else if (flPoint == 0)			//Целая часть
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
					else					//Дробная часть
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
					if (mess[r] == 'S')   				//знак Latitude
						LatFly = 54000000UL-temp_koord;		//90UL*60*10000-koord;
					else		  
						LatFly = 54000000UL+temp_koord;	//90UL*60*10000+koord;
				}
				else if (i_comma == 5)                //Longitude
				{
					if(mess[r] == '.')
					{
						flPoint = 1;
						i = 0;
					}
					else if (flPoint == 0)			//Целая часть
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
					else					//Дробная часть
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
					if (mess[r] == 'W')   //знак Longitude
						LonFly = 108000000UL-temp_koord;		//180UL*60*10000-koord;
					else       
						LonFly = temp_koord+108000000UL;	//180UL*60*10000;
				}
				else if (i_comma == 7)	//скорость в узлах
				{
					if(mess[r] == '.')
					{
						flPoint = 1;
   		      	Vz = 1.852*atoi(tmpGPS)/3.6;   //??? Преобразовать из узлов в м/с
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
				else if (i_comma == 8)	//курс в градусах
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
		            OutModem20();
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
//-----------------
}

//------------------------------------------------------------------------------------------
//скорость ветра
void INT0 (void) interrupt 0
{
	xdata unsigned int tmpint;
	tmpint = TH1; 
	timer_tick1 = (timer_tick1 & 0xffff0000) + (tmpint<<8) + TL1;
//	dTimerTickV = dTimerTickV + ((timer_tick1 - timer_tickB) - dTimerTickV)*0.01;
	/*	
	if(dTimerTickV < 1000)
		Vspeed = 681.;
	else if(dTimerTickV > 73000000)
		Vspeed = 0.01;
	else
		Vspeed = FREQT1/(dTimerTickV*0.75);
	*/
	timer_tickB = timer_tick1;

//	EX0=0;
	return;
}

//------------------------------------------------------------------------------------
void TIMER_ISR0 (void) interrupt 1
{
	xdata unsigned int i, delta, FdKoeff;
	xdata char SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = TIMER01_PAGE;
	//0xFFFF-TIMERCLC/FREQ; FREQ=1/T; 
//ветер-------------
/*
	if(P0_7!=0)
	{
		EX0=1;
	}
	if(P1_1!=0)
	{
		EX1=1;
	}*/
	//-------------------
	if(F==0x02)
	{
		//Опрос ЛИР----------------------------
		EA=0;
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
		EA=1;
		//---------------------------------------
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
					FdKoeff=4;
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
					FdKoeff=4;
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
	//при отключенном лир, воздействие отсутствует------------------
	if(LirData==0x0FFF)
	{
		F=0x03;
	}
	//---------------------------------------------------------------	
	//ШИМы на машинку--------------------------
	FdKoeff = 6;
	EA=0;
	if(F==0xA1)
	{
		P1_7 = 0;
		Fdelta =  delta - Fdelta;
		Fdelta = delta;
		while(FdKoeff!=0)
		{
			P1_5 = 1;
			for(i=0;i<28;i++)
				_nop_();
			P1_5 = 0;
			for(i=0;i<63;i++)
				_nop_();
			FdKoeff--;
		}
	}
	else if(F==0xA2)
	{
		P1_5 = 0;
		Fdelta =  delta - Fdelta;
		Fdelta = delta;
		while(FdKoeff!=0)
		{
			P1_7 = 1;
			for(i=0;i<28;i++)
				_nop_();
			P1_7 = 0;
			for(i=0;i<63;i++)
				_nop_();
			FdKoeff--;
		}
	}
	else if(F==0xC1)
	{
		P1_7 = 0;
		Fdelta = delta-Fdelta;
		if(Fdelta>0x0001)FdKoeff=2;
		Fdelta = delta;
		while(FdKoeff!=0)
		{
			P1_5 = 1;
			for(i=0;i<25;i++)
				_nop_();
			P1_5 = 0;
			for(i=0;i<66;i++)
				_nop_();
			FdKoeff--;
		}
	}
	else if(F==0xC2)
	{
		P1_5 = 0;
		Fdelta =  delta - Fdelta;
		if(Fdelta>0x0001)FdKoeff=2;
		Fdelta = delta;
		while(FdKoeff!=0)
		{
			P1_7 = 1;
			for(i=0;i<25;i++)
				_nop_();
			P1_7 = 0;
			for(i=0;i<66;i++)
				_nop_();
			FdKoeff--;
		}
	}  
	//---------------------------------------------------------------
	P1_5 =0;
	P1_7 =0;
	EA=1;
	F=0x02;
//**********************

	TH0 = 0xA0;
	TL0 = 0x4B;
		
	SFRPAGE = SFRPAGE_SAVE;
	return;
}



//направление ветра-------------------------------------------------------------------
void INT1 (void) interrupt 2
{
	xdata unsigned int tmpint;
	
	tmpint = TH1;
	timer_tick1 = (timer_tick1&0xFFFF0000) + (tmpint<<8) + TL1;
//	dTimerTickA = dTimerTickA + ((timer_tick1 - timer_tickA) - dTimerTickA)*0.01;
//	Angle = (360.-(dTimerTickA/dTimerTickV)*360.);
	timer_tickA = timer_tick1;

//	EX1=0;
	return;
}

//------------------------------------------------------------------------------------
void TIMER_ISR1 (void) interrupt 3
{
	//INT Aint;
	//FLOAT Bfloat;
	//xdata unsigned int i;
	xdata char SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = TIMER01_PAGE;
	LED2=!LED2;
	/*

	Atimer++;
	Vtimer++;
	
	if(Atimer>0x0460)
	{
		Atimer = 0;
		timer_tickA = timer_tick1;
	}
	if(Vtimer>0x0460)
	{
		Vtimer = 0;
		Vspeed = 0;
		timer_tickB = timer_tick1;	
	}
	if(Atimer>0x02ff && Vtimer>0x02ff)
	{
		dTimerTickV = dTimerTickV + ((timer_tick1 - timer_tickB) - dTimerTickV)*0.01;
		dTimerTickA = dTimerTickA + ((timer_tick1 - timer_tickA) - dTimerTickA)*0.01;
		Vspeed = FREQT1/(dTimerTickV*0.75);
	}*/
	timer_tick1 = timer_tick1 + 0x00010000;
	
	SFRPAGE = SFRPAGE_SAVE;
	return;
}

//-------------------------------------------------------------------
void UART0_isr(void) interrupt 4
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
	  	RI0 = 0;
	}
	if (TI0)
	{
			
/*		if(r0 < rk)
		{
			flTransmiter = 1;
			SBUF0 = BufferInModem[r0++];
		}
		else						
		{
			flTransmiter = 0;			//Окончание передачи
			
		}*/
	
	/*	while(r0 < rk)
		{
			flTransmiter = 1;
			SBUF0 = BufferInModem[r0++];
		}
		flTransmiter = 0;*/
  	}
	SFRPAGE = SFRPAGE_SAVE;
	return;
}

//------------------------------------------------------------------------------------
void TIMER_ISR3 (void) interrupt 14
{
/*	INT Aint;
	FLOAT Bfloat;
	xdata unsigned int i;
*/
	xdata char SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = TMR3_PAGE;
	TF3=0;
	LED=!LED;

	/*
	Atimer++;
	Vtimer++;
	
	if(Atimer>0x0460)
	{
		Atimer = 0;
		timer_tickA = timer_tick1;
	}
	if(Vtimer>0x0460)
	{
		Vtimer = 0;
		Vspeed = 0;
		timer_tickB = timer_tick1;	
	}
	if(Atimer>0x02ff && Vtimer>0x02ff)
	{
		dTimerTickV = dTimerTickV + ((timer_tick1 - timer_tickB) - dTimerTickV)*0.01;
		dTimerTickA = dTimerTickA + ((timer_tick1 - timer_tickA) - dTimerTickA)*0.01;
		Vspeed = FREQT1/(dTimerTickV*0.75);
	}
	timer_tick1 = timer_tick1 + 0x00010000;*/	

	SFRPAGE = SFRPAGE_SAVE;
	return;
}

//------------------------------------------------------------------------------------------
void TIMER_ISR4 (void) interrupt 16
{
	xdata char SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = TMR4_PAGE;

	

	SFRPAGE = SFRPAGE_SAVE;
	return;
}
//------------------------------------------------------------------------------------------


//#endif