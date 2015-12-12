#include "c8051f120.h"
#include "tarservo.h"
#include "init.h"
#include "servo.h"
#include <intrins.h>
#include <float.h>

#define NBFM 		50
xdata char BuferFromModem [NBFM]; // ��� ������� � ����������������� �����
xdata char wBFM, rBFM, marBFM;	 

//xdata unsigned int Value[16]; 

void UART0_isr(void);
void UART1_isr(void);
void Timer0_isr(void);

//-----------------------------------------------------------------------
void main(void)
{
	//��� ������ � ���������������� ������ "�����"
	xdata char RK_code[26], nByte = 0, KontrSumma = 0, PWM;
	unsigned int Value;	

	WDTCN = 0xde;			//���� ���������� ������
	WDTCN = 0xad;
//   WDTCN = 0x07;	   // ���� ����� = 0,021 �

   FLSCL = FLSCL | 1;	//���������� ��������/������ FLASH ������

	port_init(); 
	sysclk();
	SPI_Init ();	
	UART0_Init();	
	UART1_Init(); 
	DAC0_init();	
	ADC_init();
	Timer0_init(); 
	MMC_FLASH_Init();

	config();
//	EA = 0;

//	DAC0 = 0x00;

	rBFM = wBFM = marBFM = 0;
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
		
	}	//while (1)
	return;
}

//-------------------------------------------------------------------
void UART0_isr(void) interrupt 4
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
		TI0 = 0;

	SFRPAGE = SFRPAGE_SAVE;
	return;
}

//---------------------------------------------------------------------------------
void UART1_isr(void) interrupt 20
{
	xdata char SFRPAGE_SAVE = SFRPAGE;//, tmp;
	SFRPAGE = UART1_PAGE;							  

	if (RI1)  
		RI1 = 0;

	TI1 = 0;
	SFRPAGE = SFRPAGE_SAVE;
	return;
}

//----------------------------------------------------------------------
void TIMER0_ISR (void) interrupt 1
{
	xdata char SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = 0;

	TH0 = 0xAE;     // 0xFFFF-49766400/48/FREQ = 0xAEFF
	TL0 = 0xFF;     

	SFRPAGE = SFRPAGE_SAVE;
	return;
}
